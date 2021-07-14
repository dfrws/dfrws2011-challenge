import os;
import os.path;
import logging;
import datetime;
import codecs;
import cgi;
import md5;
import struct;

from android.reporting.htmlReportObjects import *;
from android.reporting import utils;

class HtmlReporter():	
	def __init__(self, title="Androilyer Report"):
		self._outputPath = None;
		self._includedPath = None;
		self._currentSection = None;
		self._sections = [];
		self._title = title;
	
	def setOutputPath(self, outputPath, prefix="report"):
		if not(os.path.isdir(outputPath)):
			raise Exception("%s is not a valid path to an existing directory"%outputPath);
		self._outputPath = outputPath;
		self._filename = "%s.html"%prefix;
		self._includesPath = os.path.join(self._outputPath, "includes");
		if not(os.path.exists(self._includesPath)):
			os.mkdir(self._includesPath);
		
	
	def newSection(self, heading):
		if(self._currentSection is not None):
			self._sections.append(self._currentSection);
		
		self._currentSection = HtmlReportSection(heading, len(self._sections)+1);
	
	def getExternal(self, htmlData):
		m = md5.new();
		m.update(htmlData.encode("utf-8"));		
		filename = "%s.html"%m.digest().encode('hex');
		path = os.path.join(self._includesPath, filename); 
		with open(path, "w") as fh:
			fh.write(htmlData.encode("utf-8"));
		ext = HtmlReportExternal(os.path.relpath(path, self._outputPath));
		return ext;
	
	def addTable(self, reportTable):
		self._currentSection.addTable(reportTable);
	
	def addTableByList(self, header, content, title="", description=""):
		repTable = HtmlReportTable(header, content, title, description);
		self._currentSection.addTable(repTable);
	
	def outputReport(self):
		if(self._currentSection is not None):
			self._sections.append(self._currentSection);
		logging.debug("Generating report...");		
		if(self._outputPath is None):
			logging.error("Report output path is not set!");
			raise Exception("Report output path is not set!");		
		repPath = os.path.join(self._outputPath, self._filename);
		
		with codecs.open(repPath, encoding="utf-8", mode="w") as out:
			out.write(self._writeHeader());
			out.write(u"<HR />\n");
			out.write(self._writeIndex());			
			for section in self._sections:
				out.write(u"<HR/>\n");
				out.write(unicode(section));
					
			out.write(self._writeFooter());
			logging.debug("Report done! Written to %s"%repPath);
						
	def _writeCSS(self):
		cssStr = u'''<STYLE type='text/css'>\n \
					table {border-color:black; border-width:2px; border-style:solid;} \n \
					td,th {max-width: 800px; border-style:solid; border-width:1px; margin:0px; padding:5px; text-align:left;} \n \
					</STYLE>''';
		return cssStr;
			
	def _writeTitle(self):
		titleStr = u"<H1>%s</H1>"%(utils.htmlEncode(self._title));
		return titleStr;
		
	def _writeHeader(self):
		dtNow = datetime.datetime.now();
		genStr = u"Report generated on %s"%(dtNow.strftime("%m/%d/%Y %I:%M:%S %p"));
		headStr = u"<HTML>\n<HEAD>\n%s\n<TITLE>%s</TITLE></HEAD>"%(self._writeCSS(), utils.htmlEncode(self._title));
		headStr += u"<BODY>" + "%s\n<P>%s</P>"%(self._writeTitle(), genStr);
		return headStr;
	
	def _writeIndex(self):
		outStr = u"<UL>\n";
		for sect in self._sections:
			link = u"<A href='#section_%d'>%s</A>"%(sect.num, sect.heading.content);
			outStr += u"<LI>%s</LI>"%link;
		outStr += u"</UL>";
		return outStr;
	
	def _writeFooter(self):
		return u"</BODY></HTML>";
	
	