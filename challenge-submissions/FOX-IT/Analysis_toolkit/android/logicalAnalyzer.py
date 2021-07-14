import sqlite3;
import logging;

class LogicalAnalyzer:		
	'''Super class for logical analyzers'''	
	def __init__(self):
		pass;
		
	def doAnalysis(self):
		pass;

	def _checkDbFile(self, dbFile):
		try:
			fh = sqlite3.connect(dbFile);
			fh.close();
		except Exception as err:
			logging.error("Could not properly read database file %s"%dbFile);
			raise err;
	
	 