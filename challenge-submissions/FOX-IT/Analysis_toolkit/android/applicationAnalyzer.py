import lxml.etree;
import logging;
import datetime;
import csv;
import md5;

from android.logicalAnalyzer import LogicalAnalyzer;

class ApplicationAnalyzer(LogicalAnalyzer):
	
	def __init__(self):
		pass;
	
	def doAnalysis(self, packageXml):
		self.packages = {};
		if self.checkPackageFile(packageXml):
			self.parsePackageList(packageXml);
		return;
		
	
	def checkPackageFile(self, packageXml):
		with open(packageXml, "r") as fh:
			headerBytes = fh.read(256);
			if(headerBytes.startswith("<?xml") and "<packages>" in headerBytes):
				return True
			else:
				return False;

	def parsePackageList(self, packageXml):
		logging.debug("Parsing installed packages from XML file...");
		parser = lxml.etree.XMLParser(recover=True);
		tree = lxml.etree.parse(packageXml, parser);
		logging.debug("Parse results: \n%s"%parser.error_log);
		packageEls = tree.findall(".//package");
		for el in packageEls: 
			try: 
				(md5str, pkg) = self.parsePackageElement(el);	
				self.packages[md5str] = pkg;
			except Exception as ex:
				logging.warning("Error during XML parsing of %s: %s \n skipping..."%(fil, str(ex)));
	
	def parsePackageElement(self, el):
		if("ts" in el.attrib.keys()):
			dt = datetime.datetime.utcfromtimestamp(float(el.attrib["ts"])/1000);	
		else: 
			dt = datetime.datetime.utcfromtimestamp(0);
		sharedUid = el.attrib["sharedUserId"] if ("sharedUserId" in el.attrib.keys()) else "";
		system = el.attrib["system"] if ("system" in el.attrib.keys()) else "";
		userId = el.attrib["userId"] if ("userId" in el.attrib.keys()) else "";
		flags = el.attrib["flags"] if ("flags" in el.attrib.keys()) else "";
		name = el.attrib["name"] if ("name" in el.attrib.keys()) else "";
		codepath = el.attrib["codePath"] if ("codePath" in el.attrib.keys()) else "";
		version = el.attrib["version"] if ("version" in el.attrib.keys()) else "";
		type = ("system" if el.attrib["system"]=="true" else "user") if ("system" in el.attrib.keys()) else "";
		
		hash = md5.new();
		hash.update(name + codepath + flags + str(dt) + version + userId + type);
		md5str = hash.digest().encode("hex");
		return (md5str, (name, codepath, flags, dt, version, userId, type));
	
	def processCarvedPackageElements(self, xmlFiles):
		self.packages = {}
		for fil in xmlFiles:
			parser = lxml.etree.XMLParser(recover=True);
			tree = lxml.etree.parse(fil, parser);
			if(tree.getroot().tag == "package"):
				try: 
					(md5str, pkg) = self.parsePackageElement(tree.getroot());	
					self.packages[md5str] = pkg;
				except Exception as ex:
					logging.warning("Error during XML parsing of %s: %s \n skipping..."%(fil, str(ex)));
			packageEls = tree.findall(".//package");
			for el in packageEls: 
				try: 
					(md5str, pkg) = self.parsePackageElement(el);	
					self.packages[md5str] = pkg;
				except Exception as ex:
					logging.warning("Error during XML parsing of %s: %s \n skipping..."%(fil, str(ex)));
				