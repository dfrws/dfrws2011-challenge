import os;
import os.path;
import logging;
import datetime;
import codecs;
import cgi;
import md5;
import struct;

from android.reporting import utils;
from android.reporting.csvReportObjects import *;

class CsvReporter():	
	def __init__(self, title="Report"):
		self._outputPath = None;
		self._currentSection = None;
		self._sections = [];
		self._title = title;
	
	def setOutputPath(self, outputPath, appendix="report"):
		if not(os.path.isdir(outputPath)):
			raise Exception("%s is not a valid path to an existing directory"%outputPath);
		self._outputPath = os.path.join(outputPath, "%s_CSV"%appendix);
		utils.createDir(self._outputPath);
					
	def newSection(self, heading):
		if(self._currentSection is not None):
			self._sections.append(self._currentSection);
		
		self._currentSection = CsvReportSection(heading, len(self._sections)+1);
	
	def addTable(self, reportTable):
		self._currentSection.addTable(reportTable);
	
	def addTableByList(self, header, content, title="", description=""):
		repTable = CsvReportTable(header, content, title, description);
		self._currentSection.addTable(repTable);
	
	def getExternal(self, str):
		return str;
	
	def outputReport(self):
		if(self._currentSection is not None):
			self._sections.append(self._currentSection);
		logging.debug("Generating csv report...");		
		if(self._outputPath is None):
			logging.error("Report output path is not set!");
			raise Exception("Report output path is not set!");		
		
		for section in self._sections:
			section.outputSection(self._outputPath);
					
		logging.debug("Report done! Written to %s"%self._outputPath);
	