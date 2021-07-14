import os;
import os.path;
import fnmatch;

def fmodeStr(fmode):
	fmode = fmode & 0x0000FFFF;
	fmStr = "";
	for i in [0,1,2,3]:
		mod = fmode >> (i*3) # shift it all the way right and
		mod &= 0x00000007;
		fmStr = str(mod) + fmStr;
	return fmStr;
	
def createDir(path):
	if(os.path.isdir(path)):
		return;
	else:
		try:
			os.makedirs(path);
		except:
			raise Exception("Could not create folder %s."%path);

def checkDir(path):
	if(os.path.isdir(path)):
		return;
	else:
		raise Exception("Folder %s does not exist."%path);
			
def checkWriteFile(path):
	try:
		open(path, "wb");
	except IOError:
		raise Exception("Could not open file %s for writing."%path);

def checkReadFile(path):
	try:
		open(path, "rb");
		return True;
	except IOError:
		raise Exception("Could not open file %s for reading."%path);
		

def findFiles(searchStr, searchPath):
	'''Given a search path, find file'''
	hits = [];
	for (dirPath, dirs, files) in os.walk(searchPath):
		for fil in files:
			if(fnmatch.fnmatch(fil, searchStr)):
				hits.append(os.path.join(dirPath, fil));
	return hits;

def findDirs(searchStr, searchPath):
	'''Given a search path, find file'''
	hits = [];
	for (dirPath, dirs, files) in os.walk(searchPath):
		for dir in dirs:
			if(fnmatch.fnmatch(dir, searchStr)):
				hits.append(os.path.join(dirPath, dir));
	return hits;	
	
def isNumber(str):
	try:
		int(str);
		return True;
	except ValueError:
		return False
