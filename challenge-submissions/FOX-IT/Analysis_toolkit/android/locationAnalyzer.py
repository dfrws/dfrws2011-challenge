import sqlite3;
import os;
import os.path;
import logging;
import datetime;
import sys;

from android.logicalAnalyzer import LogicalAnalyzer;

class LocationAnalyzer(LogicalAnalyzer):		
	'''Class to hold location analysis logic.'''
		
	def doAnalysis(self):	
		pass;
		
	def processCarvedSearchHistory(self, cRows):
		''' Add formatted tuples (id, type, data1, data2)'''
		self.searches = [];	
		for cr in cRows: 
			self.searches.append((cr[0], cr[1], cr[2], cr[3]));
		return;
	
	def processCarvedSuggestions(self, cRows):
		''' Add formatted tuples (id, data1, singleResult, displayQuery)'''
		self.suggestions = [];		
		for cr in cRows: 
			self.suggestions.append((cr[0], cr[1], cr[2], cr[3]));
		return;
	
	def processCarvedDestinationHistory(self, cRows):
		''' Add formatted tuples (date/time (utc), dest. lat/long, dest. title, dest. address, source lat/long)'''
		self.routes = [];
		for cr in cRows:
			dt = datetime.datetime.utcfromtimestamp(cr[0]/1000);
			destLL = "%d / %d"%(cr[1],cr[2]);
			sourceLL = "%d / %d"%(cr[6], cr[7]);
			self.routes.append((dt, destLL, cr[3], cr[4], sourceLL));
		return;