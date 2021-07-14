# processCarvedPackageXML
# By Ivo Pooters (Fox-IT)
# Copyright 2011 Fox-IT
# version 0.1

import argparse;
import os;
import os.path;
import logging;
import sys;
import utils;
import datetime;

from android import *;
from android.reporting import *;
from android.reporting.htmlReportObjects import *;
import utils;

OUTPUT_FOLDER = "carvedPackageXML";
VERSION = "0.1";
SCRIPT_NAME = "processCarvedPackageXML";

def main():
	global OUTPUT_FOLDER;
	global VERSION;
	global SCRIPT_NAME;

	print("="*40);
	print(SCRIPT_NAME);
	print("v%s"%VERSION);
	print("By Ivo Pooters");
	print("Copyright 2011 Fox-IT");
	print("="*40);
	parser = argparse.ArgumentParser(description="Parse XML files for elements from packages.xml");
	
	# Analyze Yaffs2
	parser.add_argument("inputfolder", help="The path to the folder containing the XML files.");
	parser.add_argument("outputfolder", help="The path to the output folder. Androilyzer will create a subfolder '%s' and write the results to this folder."%OUTPUT_FOLDER);
	parser.add_argument("-debug", action="store_true", dest="debug");
	
	# Parse arguments
	argNs = parser.parse_args();
	utils.createDir(os.path.join(argNs.outputfolder, OUTPUT_FOLDER));
	
	if(argNs.debug):
		logging.basicConfig(format='%(asctime)s - %(levelname)s - %(message)s', filename=os.path.join(argNs.outputfolder, OUTPUT_FOLDER, "%s.log"%SCRIPT_NAME), level=logging.DEBUG)
		#logging.basicConfig(format='%(asctime)s - %(levelname)s - %(message)s', level=logging.DEBUG);
	else:
		logging.basicConfig(format='%(asctime)s - %(levelname)s - %(message)s', filename=os.path.join(argNs.outputfolder, OUTPUT_FOLDER,"%s.log"%SCRIPT_NAME), level=logging.INFO)
		#logging.basicConfig(format='%(asctime)s - %(levelname)s - %(message)s', level=logging.INFO);
	
	try:
		do_process_XML(argNs.inputfolder, argNs.outputfolder);
	except Exception as ex: 
		logging.error(traceback.format_exc());
		logging.error("Script aborted.");
	else:
		logging.info("Script finished without errors.");
	return;
	
	
def do_process_XML(inputPath, outputPath):
	logging.info("******** %s run on %s **********"%(SCRIPT_NAME, datetime.datetime.now()));
	utils.checkDir(inputPath);
	outPath = os.path.join(outputPath, OUTPUT_FOLDER);
	utils.createDir(outPath);
	
	appAn = applicationAnalyzer.ApplicationAnalyzer();
	fnPtrn = "*";
	logging.info("Scanning %s for files with name pattern '%s'"%(inputPath, fnPtrn));
	files = utils.findFiles(fnPtrn, inputPath);
	if(len(files) == 0):
		logging.warning("No files found");
	else:
		report = htmlReporter.HtmlReporter(); #get reporter
		report.setOutputPath(outPath, "%s.html"%SCRIPT_NAME);
		logging.info("Found %d file(s):"%len(files));
		report.newSection("Carved packages.xml analysis");
		logging.info("Analyzing files...");
		appAn.processCarvedPackageElements(files);
		logging.info("Analysis done!");
		sortedPackages = sorted(appAn.packages.values(), key=lambda pack: pack[3]);
		logging.info("Found %d unique package registrations"%len(sortedPackages));
		htmlTable = HtmlReportTable(("Name", "Path", "flags", "date/time (UTC)", "version", "user ID", "type"), sortedPackages, "Installed packages", "Registered installed packages.");
		report.addTable(htmlTable);
		report.outputReport();
	
if __name__ == '__main__':
    main()