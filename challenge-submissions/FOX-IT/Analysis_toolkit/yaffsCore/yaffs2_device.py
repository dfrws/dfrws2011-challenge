import logging;
import struct;
import os;
import os.path;
import sys;

from yaffs2_object import *;
from yaffs2_fs import *;

class NANDProps:
	pass;

class BlockStatus:
	UNKNOWN = 0x00;
	NEEDS_SCANNING = 0x02;
	SCANNING = 0x01;
	EMPTY = 0x03;
	ALLOCATING = 0x04;
	FULL = 0x05;
	DIRTY = 0x06;
	CHECKPOINT = 0x07;
	COLLECTING = 0x08;
	DEAD = 0x09;
	
class YaffsDevice:	
	
	def __init__(self, nandProps, fn):
		self.chunkSize = nandProps.pageSize;
		self.eraseSize = nandProps.eraseSize;
		self.spareSize = nandProps.spareSize;
		self._blockStats = [];
		self._scanned = False;
		self._yaffsFs = YaffsFs();
		self._objectHeaders = {};
		if(os.path.isfile(fn)):
			self._nandImageFile = fn;
		else:
			raise Exception("%s does not exist or is not a file"%fn);
		
	def getObjectHeaders(self):
		if not(self._scanned):
			logging.error("Object headers requested, but NAND is not yet scanned!");
			raise Exception("Object headers requested, but NAND is not yet scanned!");			
		return self._objectHeaders;
	
	def scanBlocks(self):
		'''Scan the image file for blocks and status of the blocks'''
		pass;
					
	def scanNandNoSpare(self):
		'''Scan the NAND image file and gather all the information available in the object headers. 
		This method assumes that no spare areas are present in the image. '''
		logging.debug("Scanning NAND image with no spare areas...");
		numBlocks = 0;
		bufSize = self.eraseSize; #read per block
		with open(self._nandImageFile, "rb") as fh:
			blockBytes = fh.read(bufSize);
			while(len(blockBytes) == bufSize):
				numBlocks += 1;
				blockOffset = 0;
				while(blockOffset < bufSize): #parse chunk by chunk
					chunkBytes = blockBytes[blockOffset:blockOffset + self.chunkSize];
					if(YaffsObjectHeader.isHeader(chunkBytes)):
						offset = (fh.tell() - bufSize) + blockOffset;
						#logging.debug("Found object header at offset 0x%08x"%offset);
						yoh = YaffsObjectHeader(offset);						
						yoh.fromBytes(chunkBytes);	
						if(yoh.name in self._objectHeaders.keys()): #the list of objectheaders for this name exists
							self._objectHeaders[yoh.name].append(yoh);
						else:
							self._objectHeaders[yoh.name] = [];
							self._objectHeaders[yoh.name].append(yoh);
											
					blockOffset += self.chunkSize;
				blockBytes = fh.read(bufSize);
			if(len(blockBytes) != 0):
				logging.warning("%d bytes left: size of image is not a multiple of block size"%len(blockBytes));
		logging.debug("Scanning done!");
		self._scanned = True;		
				
	def scanNandImage(self):
		if(self.spareSize == 0):
			logging.info("Spare area size is set to 0, so I will only scan the object headers");
			self.scanNandNoSpare();
			self._scanned = True;
		else:
			pass;
	
	def readBlockByOffset(self, offset):
		'''Determines the block in which this offset is located and reads the entire block'''
		(blockNum, offsInBlock) = divmod(offset, self.eraseSize); 
		with open(self._nandImageFile, "rb") as fh:
			fh.seek(blockNum*self.eraseSize);
			blockData = fh.read(self.eraseSize);
		
		return (blockData, self.eraseSize*blockNum); 
	
	def readChunkData(self, offset, numChunks):
		if(offset % (self.chunkSize + self.spareSize) != 0): #not aligned
			raise ValueError("Attempting to read chunks from non-aligned offset");
		
		with open(self._nandImageFile, "rb") as fh:
			fh.seek(offset);
			chunkData = fh.read((self.chunkSize + self.spareSize) * numChunks);
			nextOffset = fh.tell();
		return (self._unspare(chunkData), nextOffset); 
	
	def _unspare(self, chunkData):
		offset = 0;
		strippedData = "";
		while(offset < len(chunkData)):
			pageBufSize = self.chunkSize if self.chunkSize < (len(chunkData) - offset) else (len(chunkData) - offset);
			strippedData += (chunkData[offset:(offset + pageBufSize)]);
			offset += (self.chunkSize + self.spareSize);
		return strippedData;
		
	
	def readChunkByOffset(self, offset):
		'''Determines the block in which this offset is located and reads the entire block'''
		(chunkNum, offsInChunk) = divmod(offset, self.chunkSize); 
		with open(self._nandImageFile, "rb") as fh:
			fh.seek(chunkNum*self.chunkSize);
			chunkData = fh.read(self.chunkSize);
		
		return (chunkData, chunkOffset); 
	
	def isScanned(self):
		return self._scanned;