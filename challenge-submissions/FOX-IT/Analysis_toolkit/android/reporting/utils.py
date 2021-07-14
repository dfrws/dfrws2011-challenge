import cgi;
import datetime;
import logging;
import os;
import os.path;

def htmlEncode(str):
		result = cgi.escape(str, True);
		return result.replace("\n", "<BR>");

		
def contentToUnicode(value):
	''' Converts the value of any type to a unicode string'''
	if(value is None):
		colStr = u"";
	elif(type(value) is datetime.datetime):
		dtFormat = "%m/%d/%Y %I:%M:%S %p";
		colStr = unicode(value.strftime(dtFormat), "utf-8");		
	elif(type(value).__name__ == "unicode"):
		colStr = htmlEncode(value);
	elif(type(value).__name__ == "str"):
		colStr = htmlEncode(value);
	elif(type(value).__name__ == "list"):
		colStr = "<UL>";
		for valEl in value:
			colStr += "<LI>%s</LI>"%contentToUnicode(valEl);
		colStr += "<UL>";
	else: 
		try:
			colStr = unicode(str(value), "utf-8", errors='replace');		
		except UnicodeDecodeError as err:
			logging.warning("Trouble parsing the string %s to unicode"%(list(value)));
			colStr = unicode(list(value));
	return colStr;
	
def createDir(path):
	if(os.path.isdir(path)):
		return;
	else:
		try:
			os.makedirs(path);
		except:
			raise Exception("Could not create folder %s."%path);
			
def sanitizePathString(str):
	sanity = {"/": "-", "\\":"-"};
	res = str;
	for (i,j) in sanity.items():
		res = res.replace(i, j);
	return res;
	
def contentToCsvString(value):
	''' Converts the value of any type to a unicode string'''
	if(value is None):
		colStr = u"";
	elif(type(value) is datetime.datetime):
		dtFormat = "%d/%m/%Y %I:%M:%S %p";
		colStr = unicode(value.strftime(dtFormat), "utf-8");		
	elif(type(value).__name__ == "unicode" or type(value).__name__ == "str"):
		colStr = unicode(value, "utf-8", errors='replace');
	else: 
		colStr = str(value).encode("utf-8", errors='replace');
		
	sanity = {"\n":" "};
	res = colStr;
	for (i,j) in sanity.items():
		res = res.replace(i, j);
	return res;