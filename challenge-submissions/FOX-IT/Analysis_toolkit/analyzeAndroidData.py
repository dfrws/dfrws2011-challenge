# analyzeAndroidData.py
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
import csv;
import traceback;

from android.reporting import *;
from android import *;
import utils;

OUTPUT_FOLDER = "analyzeAndroidData";
VERSION = "0.1";
SCRIPT_NAME = "analyzeAndroidData";

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
	parser = argparse.ArgumentParser(description="Analyze relevant Android data structures from a user data partition.");
	
	# Analyze Yaffs2
	parser.add_argument("udatafolder", help="The path to the root of the user data folder structure.", type=str);
	parser.add_argument("outputfolder", help="The path to the output folder. Androilyzer will create a subfolder '%s' and write the results to this folder."%OUTPUT_FOLDER);
	parser.add_argument("-debug", action="store_true", dest="debug");
	parser.add_argument("-csv", action="store_true", dest="csv");
	
	# Parse arguments
	argNs = parser.parse_args();
	utils.createDir(os.path.join(argNs.outputfolder, OUTPUT_FOLDER));
	
	if(argNs.debug):
		logging.basicConfig(format='%(asctime)s - %(levelname)s - %(message)s', filename=os.path.join(argNs.outputfolder, OUTPUT_FOLDER, "%s.log"%SCRIPT_NAME), level=logging.DEBUG)
		#logging.basicConfig(format='%(asctime)s - %(levelname)s - %(message)s', level=logging.DEBUG);
	else:
		logging.basicConfig(format='%(asctime)s - %(levelname)s - %(message)s', filename=os.path.join(argNs.outputfolder, OUTPUT_FOLDER,"%s.log"%SCRIPT_NAME), level=logging.INFO)
		#logging.basicConfig(format='%(asctime)s - %(levelname)s - %(message)s', level=logging.INFO);
	
	logging.info("******** %s run on %s **********"%(SCRIPT_NAME, datetime.datetime.now()));	
	try:
		do_analyze_datastructures(argNs.udatafolder, argNs.outputfolder, argNs.csv);
	except Exception as ex: 
		logging.error(traceback.format_exc());
		logging.error("Script aborted.");
	else:
		logging.info("Script finished without errors.");
	return;
	
def do_analyze_datastructures(udataPath, outputPath, exportCsv):
	global SCRIPT_NAME;
	try:
		utils.checkDir(udataPath);
		outPath = os.path.join(outputPath, OUTPUT_FOLDER);
		utils.createDir(outPath);
	except Exception as ex:
		logging.warning("Error occured while reading user data path and creating output directory. %s"%str(ex));		
		raise ex;
	
	logging.info("User data folder set: %s"%udataPath);
	
	if(exportCsv):
		report = csvReporter.CsvReporter("%s report on folder %s "%(SCRIPT_NAME, os.path.basename(udataPath)));		
	else:
		report = htmlReporter.HtmlReporter("%s report on folder %s "%(SCRIPT_NAME, os.path.basename(udataPath))); #get reporter
	
	report.setOutputPath(outPath, SCRIPT_NAME);
	logging.info("Android analyzer initiated.");
	
	report.newSection("Contact Information");
	analyze_contacts(udataPath, outputPath, report);
	
	report.newSection("SMS/MMS");
	analyze_sms(udataPath, outputPath, report);	
	
	report.newSection("Gmail communications");
	analyze_emails(udataPath, outputPath, report);
	
	report.newSection("Browser information");
	analyze_browserdata(udataPath, outputPath, report);
	
	report.newSection("Packages");
	analyze_packages(udataPath, outputPath, report);
	
	logging.info("Generating report...");
	report.outputReport();
	logging.info("Report done! Output written to %s."%os.path.abspath(outPath));
	return;


def analyze_browserdata(udataPath, outputPath, report):
	browsAn = browserAnalyzer.BrowserAnalyzer();
	browserDbFn = "browser.db";
	logging.info("Scanning %s for browser database files with name '%s'"%(udataPath, browserDbFn));
	brwDbFiles = utils.findFiles(browserDbFn, udataPath);
	if(len(brwDbFiles) == 0):
		logging.warning("No browser databases found");
	else:
		logging.info("Found %d browser database files:"%len(brwDbFiles));
		for db in brwDbFiles:
			logging.info("Analyzing %s..."%db);
			browsAn.parseHistory(db);
			browsAn.parseBookmarks(db);
			logging.info("Analysis done!");
			report.addTableByList(("date/time (UTC)", "visits", "title", "url"), browsAn.history, "Browser history", "History records found in database %s"%db);
			report.addTableByList(("title", "url"), browsAn.bookmarks, "Bookmarks", "Bookmarks found in database %s"%db);
				
	browserDbFn = "webview.db";
	logging.info("Scanning %s for browser database files with name '%s'"%(udataPath, browserDbFn));
	brwDbFiles = utils.findFiles(browserDbFn, udataPath);
	if(len(brwDbFiles) == 0):
		logging.warning("No %s databases found"%browserDbFn);
	else:
		logging.info("Found %d browser database files:"%len(brwDbFiles));
		for db in brwDbFiles:
			logging.info("Analyzing %s..."%db);
			browsAn.parseCredentials(db);
			logging.info("Analysis done!");
			if(len(browsAn.credentials) > 0):
				report.addTableByList(("host", "username", "password"), browsAn.credentials, "Browser credentials", "Credentials found in database %s"%db);
				
	browserDbFn = "webviewCache.db";
	logging.info("Scanning %s for browser database files with name '%s'"%(udataPath, browserDbFn));
	brwDbFiles = utils.findFiles(browserDbFn, udataPath);
	if(len(brwDbFiles) == 0):
		logging.warning("No %s databases found"%browserDbFn);
	else:
		logging.info("Found %d browser database files:"%len(brwDbFiles));
		for db in brwDbFiles:			
			logging.info("Analyzing %s..."%db);
			browsAn.parseCache(db, udataPath);
			logging.info("Analysis done!");
			if(len(browsAn.cached) > 0):
				report.addTableByList(("url", "filepath", "last modified", "expires", "mimetype", "content length"), sorted(browsAn.cached, key=lambda chd:chd[2]), "Cached files", "Cached files found in database %s"%db);
					
def analyze_packages(udataPath, outputPath, report):
	appAn = applicationAnalyzer.ApplicationAnalyzer();
	appXml = "packages.xml";
	logging.info("Scanning %s for package file '%s'"%(udataPath, appXml));
	appXmlFiles = utils.findFiles(appXml, udataPath);
	if(len(appXmlFiles) == 0):
		logging.warning("No package files found");
	else:
		logging.info("Found %d package file(s):"%len(appXmlFiles));
		for xmlF in appXmlFiles:
			logging.info("Analyzing %s..."%xmlF);
			appAn.doAnalysis(xmlF);
			logging.info("Analysis done!");
			sortedPackages = sorted(appAn.packages.values(), key=lambda pack: pack[3]);
			report.addTableByList(("Name", "Path", "flags", "date/time (UTC)", "version", "user ID", "type"), sortedPackages, "Installed packages", "Installed packages registered in file %s"%xmlF);
	
def analyze_sms(udataPath, outputPath, report):
	smsAn = smsAnalyzer.SMSAnalyzer();
	smsDbfn = "mmssms.db";
	logging.info("Scanning %s for sms database files with name '%s'"%(udataPath, smsDbfn));
	smsDbFiles = utils.findFiles(smsDbfn, udataPath);
	if(len(smsDbFiles) == 0):
		logging.warning("No sms databases found");
	else:
		logging.info("Found %d sms database files:"%len(smsDbFiles));
		for db in smsDbFiles:
			logging.info("Analyzing %s..."%db);
			smsAn.doAnalysis(db);
			logging.info("Analysis done!");
			report.addTableByList(smsAn.smsList[0], smsAn.smsList[1:], "Text messages", "SMS messages found in database %s"%db);
			
def analyze_contacts(udataPath, outputPath, report):
	contactAn = contactAnalyzer.ContactAnalyzer();
	contactDbFn = "contacts2.db";
	logging.info("Scanning %s for contact database files with name '%s'"%(udataPath, contactDbFn));
	contactDbFiles = utils.findFiles(contactDbFn, udataPath);
	if(len(contactDbFiles) == 0):
		logging.warning("No contact databases found");
	else:
		logging.info("Found %d contact database files:"%len(contactDbFiles));
		for db in contactDbFiles:
			logging.info("Analyzing %s..."%db);
			contactAn.doAnalysis(db);
			logging.info("Analysis done!");
			report.addTableByList(("Id", "display name", "updates", "name", "email address", "postal address", "phone number"), contactAn.contacts, "Contacts", "Contacts found in database %s"%db);
			report.addTableByList(("Id", "number", "date/time (UTC)", "duration", "type", "name"), sorted(contactAn.calls, key=lambda cl: cl[2]), "Call history", "Call history found in database %s"%db);
			

def analyze_emails(udataPath, outputPath, report):
	gmailAn = gmailAnalyzer.GmailAnalyzer();
	gmailDbFn = "mailstore.*@gmail.com.db";
	dlDbFn = "downloads.db";
	logging.info("Scanning %s for gmail database files with name '%s'"%(udataPath, gmailDbFn));
	gmailDbFiles = utils.findFiles(gmailDbFn, udataPath);
	logging.info("Scanning %s for download database file with name '%s'"%(udataPath, dlDbFn));
	dlDbFiles = utils.findFiles(dlDbFn, udataPath);
	if(len(gmailDbFiles) == 0 or len(dlDbFiles) == 0):
		logging.warning("No gmail databases or download database found.");
	else:
		dlDbFile = dlDbFiles[0];
		logging.info("Found %d gmail database files and download database %s:"%(len(gmailDbFiles), dlDbFile));
		for db in gmailDbFiles:
			logging.info("Analyzing %s..."%db);
			gmailAn.doAnalysis(db, dlDbFile);
			logging.info("Analysis done!");
			content = [];
			attHeader = ("part", "data", "last modified", "total bytes", "current_bytes", "title");
			for comm in gmailAn.comms:
				if(len(comm[10].keys()) > 0): # there are attachments
					attContent = [];
					keys = comm[10].keys();
					keys.sort();
					for key in keys:
						attContent.append((key,) + comm[10][key]);
					attHtmlTable = htmlReportObjects.HtmlReportTable(attHeader, attContent);
					content.append(comm[:-2] + (report.getExternal(comm[9]),)+ (attHtmlTable,));					
				else: # no attachment
					content.append(comm[:-2] + (report.getExternal(comm[9]),) + (None,));
				
			report.addTableByList(("Id", "from", "to", "cc", "bcc", "received (UTC)", "sent (UTC)", "subject", "snippet", "body", "attachments"), content, "Gmail messages", "Gmail messages found in database %s"%db);
			
			
if __name__ == '__main__':
    main()