import logging;
import os;
import os.path;
import csv;
import cStringIO;
import codecs;

from android.reporting import utils;

class CsvReportTable():
	def __init__(self, header, content, title="", description=""):
		self._header = header;
		self._content = content;
		self.title = title;
		self.description = description;
		
	def output(self, outPath):
		csvFile = os.path.join(outPath, "%s.csv"%utils.sanitizePathString(self.title));
		with open(csvFile, "wb") as fh:
			writer = UnicodeCsvWriter(fh);
			writer.writerow(self._header);
			for cont in self._content:
				writer.writerow([utils.contentToCsvString(cel) for cel in cont]);
		return;
	
		
class CsvReportSection():
	def __init__(self, heading, num):
		self.heading = heading;
		self.num = num;
		self._items = [];
		
	def addTable(self, reportTable):
		self._items.append(reportTable);
		return;
				
	def outputSection(self, outPath):	
		for item in self._items:
			if(isinstance(item, CsvReportTable)):
				csvPath = os.path.join(outPath, utils.sanitizePathString(self.heading));
				utils.createDir(csvPath);
				item.output(csvPath);
			else:
				logging.debug("Weird..unknown report item type.");
		
		return True;
		

class UnicodeCsvWriter:
    """
    A CSV writer which will write rows to CSV file "f",
    which is encoded in the given encoding.
    """

    def __init__(self, fh, dialect=csv.excel, encoding="utf-8"):
        # Redirect output to a queue
        self.queue = cStringIO.StringIO()
        self.writer = csv.writer(self.queue, dialect=dialect)
        self.stream = fh;
        self.encoder = codecs.getincrementalencoder(encoding)();

    def writerow(self, row):
        self.writer.writerow([s.encode("utf-8") for s in row])
        # Fetch UTF-8 output from the queue ...
        data = self.queue.getvalue()
        data = data.decode("utf-8")
        # ... and reencode it into the target encoding
        data = self.encoder.encode(data)
        # write to the target stream
        self.stream.write(data)
        # empty queue
        self.queue.truncate(0)

    def writerows(self, rows):
        for row in rows:
            self.writerow(row)		