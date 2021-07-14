import logging;
import os;
import os.path;
import struct;
import md5;

from carving import utils;
from carving import sqliteTemplates;

class SqliteCarver():
	'''Represents a sqlite carver entity. This object holds methods, constants and variables for carving any image file for sqlite records.'''
	BTREE_LEAF_ID = 0x0D;
	SQL_TYPE_INT = 1;
	SQL_TYPE_TEXT = 2;
	SQL_TYPE_FLOAT = 4;
	SQL_TYPE_BLOB = 8;
	SQL_TYPE_NULL = 16;
	
	STYPE_NULL = 0;
	STYPE_INT8 = 1;
	STYPE_INT16 = 2;
	STYPE_INT24 = 3;
	STYPE_INT32 = 4;
	STYPE_INT48 = 5;
	STYPE_INT64 = 6;
	STYPE_FLOAT = 7;
	STYPE_CONST0 = 8;
	STYPE_CONST1 = 9;
	
	STYPE_MAP = {STYPE_NULL:(SQL_TYPE_NULL, 0), STYPE_INT8:(SQL_TYPE_INT, 1), STYPE_INT16:(SQL_TYPE_INT, 2), STYPE_INT24:(SQL_TYPE_INT, 3),
				 STYPE_INT32:(SQL_TYPE_INT, 4), STYPE_INT48:(SQL_TYPE_INT, 6), STYPE_INT64:(SQL_TYPE_INT, 8), STYPE_FLOAT:(SQL_TYPE_FLOAT, 8),
				 STYPE_CONST0:(SQL_TYPE_INT, 0), STYPE_CONST1:(SQL_TYPE_INT, 0)};
				 
	
	
	def __init__(self):
		'''Sets all variables to initial value.'''
		self.records = [];
		self.catalogue = {};
		self._image = None;
		self._sqlPageSize = None;
		self._resRegionSize = None;
		self._fsPageSize = None;
		self._fsSpareSize = None;
		self._bufSize = 2048*32;
		for (tmplName, tmpl) in sqliteTemplates.templates.items():
			self.catalogue[tmplName] = [];			
			
	def setDevice(self, imageFile):
		'''Store the image file. If the image file can not be opened in read mode, an IOError is raised.'''
		self._image = imageFile;
		try:
			open(self._image, "rb");
		except IOError:
			logging.error("Could not open file %s for reading."%self._image);
			raise Exception("Could not open file %s for reading."%self._image);
			
	def setParameters(self, fsPageSize, fsSpareSize, sqlPageSize=1024, resRegionSize=0):
		'''Method to set parameters.
		sqlPageSize: the sqlite page size as defined by the Sqlite specification. This may vary per sqlite engine. 
		resRegionSize: the reserved region size as defined by the Sqlite specs. This is usually 0, but may differ. 
		'''
		self._sqlPageSize = sqlPageSize; #The sqlite page size
		self._resRegionSize = resRegionSize; #the reserved region size at the end of the pages, usually zero in well-formed db's
		self._fsPageSize = fsPageSize;
		self._fsSpareSize = fsSpareSize;
		self._bufSize = (self._fsPageSize + self._fsSpareSize)*32;
	
	def carve(self):
		''' The main method of this class. Scans the image file page by page looking for sqlite pages. 
		Make sure the setDevice and setParameters have been called before using this method. Else the function just returns. 
		'''
		if(self._image is None or self._sqlPageSize is None):
			logging.error("Can not initiate carving, image file or parameters not set");
			return;
		if(self._sqlPageSize > self._fsPageSize):
			logging.error("Carving with sql page size > file system page size: not supported!");
			return;
		logging.info("Initiating Sqlite carving...");
		imageSize = os.path.getsize(self._image);
		milestone = 10;
		with open(self._image, "rb") as fh: 
			blockOffset = fh.tell();
			buffer = fh.read(self._bufSize);
			while (len(buffer) > 0):
				if(self._fsSpareSize > 0):
					block = self._unSpare(buffer);					
				else:
					block = buffer;
				offset = 0;
				while(offset < len(block)):
					pageBufSize = self._sqlPageSize if (self._sqlPageSize < (len(block) - offset)) else (len(block) - offset);
					sqlPage = block[offset:(offset + pageBufSize)];
					if(self._isLeafPage(sqlPage)):						
						fileOffset = self._getFileOffset(blockOffset, self._bufSize, offset);
						logging.debug("Found Sqlite leaf page at offset %d"%(fileOffset));
						try:
							records = self._parseLeafPage(sqlPage);	
							for (cols, vals, hash, cellPtr, intKey) in records:
								self.records.append((cols, vals, hash, fileOffset + cellPtr, intKey));
						except ParseException as ex:
							logging.warning("Exception during parsing in page at offset %d: %sPossibly not an sqlite page after all...skipping remainder of page."%(fileOffset, ex));																
					offset += self._sqlPageSize;
				blockOffset = fh.tell();
				if((blockOffset * 100) / imageSize > milestone):
					logging.info("%d %% done..."%milestone);
					milestone += 10;
					#break;
				buffer = fh.read(self._bufSize);
		logging.info("Sqlite carving done! Found %d records."%len(self.records));
		logging.info("Deduplicating records...");
		curLength = len(self.records);
		self._deduplicateRecords();		
		logging.info("Done deduplicating! Reduced number of records from %d to %d."%(curLength, len(self.records)));
		logging.info("Creating catalogue of records...");
		self._catalogueRecords();
		logging.info("Catalogue done!");		
		for (cat, lst) in self.catalogue.items(): 
			logging.info("%s: %d records"%(cat, len(lst)));
	
	def _deduplicateRecords(self):
		'''Deduplicate the records based on the MD5 digest of the column values.'''
		hashList = [];
		newList = [];
		for (cols, vals, hash, offset, key) in self.records:
			if(hash in hashList): #Sorry, you re out
				pass;
			else:
				newList.append((cols, vals, hash, offset, key));
				hashList.append(hash);
		self.records = newList;
					
	def _catalogueRecords(self):
		''' Catalogues the carved records. 
		Each record is matched to the templates by comparing the number of columns and (if number is equal) the type of the columns 
		to that of the template. The catalogue is stored as a class variable dictionary.'''
		for (recCols, recVals, hash, offset, key) in self.records:
			matched = False;
			for (tmplName, tmpl) in sqliteTemplates.templates.items():				
				logging.debug("matching record %s %s \n against \n %s"%(recCols, recVals, tmpl));					
				if(len(tmpl) == len(recCols)):		
					colMatch = True;
					primKeyPos = sqliteTemplates.primKeyPositions[tmplName];
					for (index, coltype) in enumerate(recCols):
						if not(coltype[1] & tmpl[index][1]):
							colMatch = False;
							break;								
					if(colMatch):
						matched = True;
						logging.debug("Record matched to %s."%tmplName);			
						if(primKeyPos == -1):
							self.catalogue[tmplName].append((recCols, recVals, hash, offset, key));
						else:	
							self.catalogue[tmplName].append((recCols, recVals[:primKeyPos] + [key] + recVals[primKeyPos + 1:], hash, offset, key));
			if not matched:
				if not ("unmatched" in self.catalogue.keys()):
					self.catalogue["unmatched"] = [];
				self.catalogue["unmatched"].append((recCols, recVals, hash, offset, key));			
		
	def _parseLeafPage(self, sqlPage):
		'''Read the cell content offset from the page header then jump into the cell content. '''
		logging.debug("Parsing page...");
		(nextFreeblock, numCells, cellContOffs) = struct.unpack(">HHH", sqlPage[1:7]);
		(arrSize, cellPointers) = self._parseCellPointerArray(sqlPage[8:], numCells);
		
		offset = 0;
		records = [];
		for cellPtr in cellPointers:
			logging.debug("Cell found at page offset %d"%(cellPtr));
			((cols, vals, hash), cellSize, intKey) = self._parseTableLeafCell(0, sqlPage[cellPtr:]);
			records.append((cols, vals, hash, cellPtr, intKey));
		
		logging.debug("Parsing page done! Header indicated %d cells, found %d."%(numCells, len(cellPointers)));
		return records;
	
	def _parseCellPointerArray(self, arrayContent, numCells):
		'''Parse the cell pointer array. 
		The cell pointer array is a sequence of 2-byte BE integers. Each integer represents
		an offset into the page to the cell''' 
		arrOffs = 0;
		cellPointers = [];		
		for i in range(numCells):
			(cellPtr,) = struct.unpack(">H", arrayContent[arrOffs:(arrOffs + 2)]);
			cellPointers.append(cellPtr);			
			arrOffs += 2;			
			
		return (arrOffs, cellPointers);
	
	
	def _parseTableLeafCell(self, offset, cellContent):
		'''Parse the table leaf cell at offset in the cell content. 
		A table leaf cell has a fixed format: 
		| num bytes of payload (varint) | integer key (varint) | initial portion of payload (n bytes) | (opt) page num of 1st overflow page (4-byte int)'''
		logging.debug("parsing cell...");
		cellCont = cellContent[offset:];
		relOffset = 0;
		try:
			(viSize, payloadSize) = utils.parseVarint(cellCont, relOffset);
			relOffset += viSize;
			(viSize, intKey) = utils.parseVarint(cellCont, relOffset);
			relOffset += viSize;
		except ValueError as ex:
			raise ParseException(str(ex), cellCont);
		
		overflow = payloadSize > (self._sqlPageSize - 35);
		logging.debug("Payload size: %d, key: %d"%(payloadSize, intKey));
		if(overflow):			
			localBytes = self._overflowLocalSize(payloadSize, self._sqlPageSize);
			logging.debug("Overflow cell payload encountered...%d bytes are stored in this page."%localBytes);			
			record = self._parseTableCellPayload(cellCont[relOffset:(relOffset + localBytes)], True);
		else:
			record = self._parseTableCellPayload(cellCont[relOffset:(relOffset + payloadSize)], False);
		logging.debug("Parsing cell done!");
		return (record, relOffset + payloadSize, intKey);
	
	def _overflowLocalSize(self, payloadSize, usableSize):
		'''Calculate the number bytes of an overflow payload that is stored in this page. 
		If the payload size P is less than or equal to U-35 then the entire payload is stored on the b-tree leaf page. 
		Let M be ((U-12)*32/255)-23. If P is greater than U-35 then the number of byte stored on the b-tree leaf page is the smaller of M+((P-M)%(U-4)) and U-35. 
		Note that number of bytes stored on the leaf page is never less than M. 
		CORRECTION: observed that Android sqlite engine never chooses U-35, but M instead. 
		'''
		minEmbeddedFraction = 32;
		minLocal = ((usableSize - 12) * minEmbeddedFraction / 255) - 23;
		maxLocal = usableSize - 35;
		localSize = minLocal + ((payloadSize - minLocal) % (usableSize - 4));
		#print("payload: %d, usable: %d, min-local: %d, local-size: %d"%(payloadSize, usableSize, minLocal, localSize));
		if(localSize < maxLocal):
			return localSize;
		else:
			return minLocal;
		
	def _parseTableCellPayload(self, payloadContent, overflow):
		''' Parse the payload part of a leaf cell. 
		A payload has a fixed format: 
		| payload header size (varint) | stype col 1 (varint) ... stype col n (varint) | value col 1 (stype size) ... value col n |
		'''
		logging.debug("parsing payload %s..."%list(payloadContent));
		record = None;
		payloadOffset = 0;
		cols = [];
		vals = [];
		try:
			(viSize, headerSize) = utils.parseVarint(payloadContent, payloadOffset);
			payloadOffset += viSize;
			(viSize, stype) = utils.parseVarint(payloadContent, payloadOffset);
			payloadOffset += viSize;
			cols.append(self._columnType(stype));
			while (payloadOffset < headerSize):
				(viSize, stype) = utils.parseVarint(payloadContent, payloadOffset);
				cols.append(self._columnType(stype));
				payloadOffset += viSize;	
			
			hash = md5.new();
			overflowReached = False;
			for ctype in cols:
				if(overflowReached):
					vals.append(sqliteTemplates.OVERFLOW_STRING);
					hash.update(sqliteTemplates.OVERFLOW_STRING);
				else:
					try: 
						(colSize, value) = self._parseColumnValue(ctype, payloadContent, payloadOffset);
						payloadOffset += colSize;
						vals.append(value);
						hash.update(str(value));
					except ValueError as er: 
						if(overflow): # this exception is expected, 
							overflowReached = True;
							remainSize = len(payloadContent) - payloadOffset;
							(remainStr,) = struct.unpack(">%ds"%remainSize, payloadContent[payloadOffset:payloadOffset + remainSize]);
							vals.append(remainStr + sqliteTemplates.OVERFLOW_STRING);
							hash.update(remainStr + sqliteTemplates.OVERFLOW_STRING);		
						else: #Not an overflow, then something is wrong
							raise;
				
			record = (cols, vals, hash.digest());
		except ValueError as ex:
			raise ParseException(ex, payloadContent);
		logging.debug("Parsing payload done! %s %s"%(cols, vals));		
		return record;
	
	def _parseColumnValue(self, columnType, payloadContent, offset):
		'''Uses the column size and type from columnType to read the column value from the payload. 
		Offset is the offset into the payloadContent where the column value starts.
		Returns (column size, column value);''' 
		colPayload = payloadContent[offset:];
		try:
			if(columnType[0] == self.STYPE_NULL):
				(colSize, value) = (0, None);
			elif(columnType[0] == self.STYPE_INT8):
				(colSize, (value,)) = (1, struct.unpack(">b", colPayload[:1]));
			elif(columnType[0] == self.STYPE_INT16):
				(colSize, (value,)) = (2, struct.unpack(">h", colPayload[:2]));
			elif(columnType[0] ==  self.STYPE_INT24):
				(temp1, temp2) = struct.unpack(">hB", colPayload[:3]);
				(colSize, value) = (3, (temp1 << 8) | temp2);
			elif(columnType[0] == self.STYPE_INT32):
				(colSize, (value,)) = (4, struct.unpack(">i", colPayload[:4]));
			elif(columnType[0] == self.STYPE_INT48):
				(temp1, temp2) = struct.unpack(">iH", colPayload[:6]);
				(colSize, value) = (6, (temp1 << 16) | temp2);
			elif(columnType[0] == self.STYPE_INT64):
				(colSize, (value,)) = (8, struct.unpack(">q", colPayload[:8]));
			elif(columnType[0] == self.STYPE_FLOAT):
				(colSize, (value,)) = (8, struct.unpack(">d", colPayload[:8]));
			elif(columnType[0] == self.STYPE_CONST0):
				(colSize, value) = (0, 0);
			elif(columnType[0] == self.STYPE_CONST1):
				(colSize, value) = (0, 1);
			elif(columnType[1] == self.SQL_TYPE_BLOB):
				if(len(colPayload) < columnType[2]): #more bytes expected than present
					raise ValueError("payload column at payload offset %d does not contain expected bytes for stype %d: %s"%(offset, columnType[0], list(colPayload[:columnType[2]])));
				(colSize, value) = (columnType[2], colPayload[:columnType[2]]);
			elif(columnType[1] == self.SQL_TYPE_TEXT):
				(colSize, (value,)) = (columnType[2], struct.unpack(">%ds"%columnType[2], colPayload[:columnType[2]]));
			else:
				raise ValueError("Unkown column type %d."%columnType[0]);			
		except struct.error as se:
			logging.debug(se);
			raise ValueError("payload column at payload offset %d does not contain expected bytes for stype %d: %s"%(offset, columnType[0], list(colPayload[:columnType[2]])));
		return (colSize, value);
	
	def _columnType(self, stype):
		''' Determine the column type and length from the stype value. 
			Returns (stype, column type, size).
		'''
		if(stype in self.STYPE_MAP.keys()):
			(colType, size) = self.STYPE_MAP[stype];
		elif(stype > 11 and not utils.isOdd(stype)):
			colType = self.SQL_TYPE_BLOB;
			size = (stype - 12)/2;
		elif(stype > 12 and utils.isOdd(stype)):
			colType = self.SQL_TYPE_TEXT;
			size = (stype - 13)/2;
		else:
			raise ValueError("Unknown sqlite serial type %d"%stype);
		return (stype, colType, size);
	
	def _isLeafPage(self, sqlPage):
		''' The first 8 bytes of a sqlite leaf page should conform to the format: 
		| page type (1) | 1st freeblock offs (2) | Num cells in page (2) | 1st cell offset (2) 
		page type should be equal to 0x0D
		1st freeblock offs: 0 - sqlite page size
		num cells: 1 - sqlite page size/10
		1st cell offset: 8 - sqlite page size
		''' 
		pageHeader = sqlPage[0:8];		
		score = 0;
		(ptype, offset, numCells, cellOffset) = struct.unpack(">BHHHx", pageHeader);
		
		if(ptype == self.BTREE_LEAF_ID):
			score += 1;
		if(offset in ([0] + range(8, self._sqlPageSize))):
			score += 1;
		if(cellOffset in range(8, self._sqlPageSize)):
			score += 1;
		if(numCells in range(1, self._sqlPageSize/10)): # I guess at least 10 bytes per cell
			score += 1;		
		return (score > 3);
		
	def _getFileOffset(self, blockOffset, blockSize, offset):
		if(offset == 0):
			return blockOffset;
		(numPages, inPageOffset) = divmod(offset, self._fsPageSize); #how many yaffs pages have I passed and how far in the current page am I?
		realInBlockOffset = numPages*(self._fsPageSize + self._fsSpareSize) + inPageOffset;
		return blockOffset + realInBlockOffset;
	
	def _unSpare(self, block):
		offset = 0;
		strippedBlock = "";
		while(offset < len(block)):
			pageBufSize = self._fsPageSize if self._fsPageSize < (len(block) - offset) else (len(block) - offset);
			strippedBlock += (block[offset:(offset + pageBufSize)]);
			offset += (self._fsPageSize + self._fsSpareSize);
		return strippedBlock;
		

class ParseException(Exception):
	def __init__(self, msg, bytes="", offset=0):
		self._msg = msg;
		self._bytes = bytes;
		self._offset = offset;
		pass;
	
	def __str__(self):
		resStr = "%s at offset %d in: %s\n"%(self._msg, self._offset, list(self._bytes));
		return resStr;