import logging;
import os;
import os.path;

from android.reporting import utils;

class HtmlReportTable():
	def __init__(self, header, content, title="", description=""):
		self._header = header;
		self._content = content;
		self.title = title;
		self.description = description;
		
	def __str__(self):		
		return unicode(self).encode("utf-8");

	def __unicode__(self):
		''' Serialize to unicode HTML'''
		tableStr = "";
		tableStr += (u"<TABLE>\n");
		rowStr = u"<TR>";
		for i in range(len(self._header)):
			rowStr += u"<TH>%s</TH>"%(utils.contentToUnicode(self._header[i]));		
		rowStr += u"</TR>\n";
		tableStr += (rowStr);
		for row in self._content:
			rowStr = u"<TR>";
			for i in range(len(row)):
				rowStr += u"<TD>%s</TD>"%(utils.contentToUnicode(row[i]));		
			rowStr += u"</TR>\n";
			tableStr += rowStr;
		tableStr += u"</TABLE>";
		return tableStr;
	
		
class HtmlReportSection():
	def __init__(self, heading, num):
		self.heading = HtmlReportHeading(heading, num, 2);
		self.num = num;
		self._items = [];
		
	def addTable(self, reportTable):
		self._items.append(reportTable);
		return;
				
	def __unicode__(self):	
		''' Serialize to unicode HTML'''
		tableCount = 1;
		listCount = 1;
		
		outStr = u"<DIV>\n";
		outStr += str(self.heading) + u"\n";
		for item in self._items:
			if(isinstance(item, HtmlReportTable)):
				outStr += u"<P>Table %d. %s </BR> %s </BR> \n%s</P>\n"%(tableCount, utils.contentToUnicode(item.title), utils.contentToUnicode(item.description), unicode(item));					
				tableCount += 1;			
					
			elif(isinstance(item, HtmlReportParagraph)):
				outStr += str(item);
			
			elif(isinstance(item, HtmlReportListing)):
				outStr += u"<P>Table %d. %s </BR> %s </BR> \n%s</P>\n"%(listCount, utils.contentToUnicode(item.title), utils.contentToUnicode(item.description), unicode(item));
				listCount += 1;
			
			else:
				logging.debug("Weird..unknown report item type.");
		
		outStr += u"</DIV>";
		return outStr;
		
	def __str__(self):	
		return unicode(self).encode("utf-8");		
			
class HtmlReportExternal():
	def __init__(self, location, linkname="<...>"):
		self._location = location;
		self._linkname = linkname;
		pass;
		
	def __str__(self):		
		return unicode(self).encode("utf-8");
		
	def __unicode__(self):
		return u"<A href=%s>%s</A>"%(utils.contentToUnicode(self._location), utils.contentToUnicode(self._linkname));		
		
class HtmlReportHeading():
	def __init__(self, content, num, level):
		self.content = content;
		self._level = level;
		self._num = num;
	
	def __unicode__(self):
		''' Serialize to unicode HTML'''		
		elName = u"section_%d"%self._num;
		outStr = u"<H%d id='%s'>%d. %s</H%d>"%(self._level, elName, self._num, utils.contentToUnicode(self.content), self._level);
		return outStr;
		
	def __str__(self):		
		return unicode(self).encode("utf-8");
		
class HtmlReportParagraph():
	def __init__(self, content):
		self.content = content;
	
	def __unicode__(self):
		''' Serialize to unicode HTML'''
		outStr = u"<P>%s</P>"%(utils.contentToUnicode(self.content));
		return outStr;
		
	def __str__(self):		
		return unicode(self).encode("utf-8");