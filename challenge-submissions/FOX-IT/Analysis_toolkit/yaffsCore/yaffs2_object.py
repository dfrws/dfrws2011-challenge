import logging;
import datetime;
import struct;
import utils;


class YaffsObjectType:
	UNKNOWN = 0x00;
	FILE = 0x01;
	SOFT_LINK = 0x02;
	DIR = 0x03;
	HARD_LINK = 0x04;
	SPECIAL = 0x05;
	
	descrMap = {0x00:"unknown", 0x01:"file", 0x02:"symbolic", 0x03:"directory", 0x04:"hard link", 0x05:"special"};
	
	def __iter__(self):
		return (x for x in [self.UNKNOWN, self.FILE, self.SOFT_LINK, self.DIR, self.HARD_LINK, self.SPECIAL]);
	
	

class YaffsObject:
		
	def __init__(self):
		pass;
		

class YaffsDir(YaffsObject):
	def __init__(self):
		pass;
		
class YaffsSymbolicLink(YaffsObject):
	
	def __init__(self):
		pass;
		
class YaffsFile(YaffsObject):
	
	def __init__(self):
		pass;
		
class YaffsHardLink(YaffsObject):
	def __init__(self):
		pass;
		
class YaffsSpecialFile(YaffsObject):
	def __init__(self):
		pass;

		
		
class YaffsObjectHeader:
	'''Represents a YAFFS2 object header. This object can be used to create an object.'''
	def __init__(self, offset):
		self.offset = offset;
		pass;

	def toDict(self):
		dtFormat = "%d-%m-%Y %H:%M"
		fmStr = utils.fmodeStr(self.fmode);
		return {'objectType':YaffsObjectType.descrMap[self.objectType], 'parentId':str(self.parentId), 'name':self.name, 'fileMode':fmStr, 
				'UID':str(self.uid), 'GID':str(self.gid), 'accessed':self.atime.strftime(dtFormat), 'modified':self.mtime.strftime(dtFormat),
				'created':self.ctime.strftime(dtFormat), 'size':str(self.fsize), 'offset':str(self.offset)};
		
	@staticmethod
	def isHeader(ohData):
		'''Checks if the provided chunk is an object header'''
		if(len(ohData) < 0x200): #chunk too small to be header
			return False;
		(objType, parentId, ffValue, padding) = struct.unpack("<IIH502x%ds"%(len(ohData)-0x200), ohData);
		if not(objType in YaffsObjectType()): 
			return False;
		elif(ffValue != 0xFFFF): 
			return False;
		elif(any([0xFF != ord(el) for el in padding])):
			return False;				
		else:
			return True;
	
	def fromBytes(self, ohData):
		'''Parse an objectHeader from a chunk of NAND data'''
		isHeader = True;
		if(len(ohData) < 0x200): #chunk too small to be header
			isHeader = False;
		(objType, parentId, ffValue, name, fmode, uid, gid, atime, mtime, ctime, fsize, padding) = struct.unpack("<IIH255s3xIIIIIII216x%ds"%(len(ohData)-0x200), ohData);
		if not(objType in YaffsObjectType()): 
			isHeader = False;
		elif(ffValue != 0xFFFF): 
			isHeader = False;
		elif(any([0xFF != ord(el) for el in padding])):
			isHeader = False;					
		
		if not(isHeader): 
			logging.error("Can not parse object header, not a valid object header");
			raise Exception("Can not parse object header, not a valid object header");
		
		self.objectType = objType;
		self.parentId = parentId;
		self.name = name.rstrip(chr(0x00));		
		self.fmode = fmode;
		self.uid = uid;
		self.gid = gid;
		self.atime = datetime.datetime.utcfromtimestamp(atime);
		self.ctime = datetime.datetime.utcfromtimestamp(ctime);
		self.mtime = datetime.datetime.utcfromtimestamp(mtime);
		self.fsize = fsize;
		