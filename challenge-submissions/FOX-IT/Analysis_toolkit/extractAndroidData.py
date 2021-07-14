# extractAndroidData.py
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

from yaffsCore import *;
from android.reporting import *;
from android import *;
from carving import *;

import utils;

OUTPUT_FOLDER = "extractAndroidData";
VERSION = "0.1";
SCRIPT_NAME = "extractAndroidData";

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
	parser = argparse.ArgumentParser(description="Carve and parse important Android Sqlite data records from a YAFFS2 image file. ");
	
	# Analyze Yaffs2
	def_pagesize = 2048;
	def_oob = 0;
	def_erasesize = def_pagesize * 32;
	parser.add_argument("imagefile", help="The path to the YAFFS2 image file.", type=str);
	parser.add_argument("-p", help="The NAND page size (ex OOB bytes) default:%d."%def_pagesize, type=int, default=def_pagesize, dest="pagesize");
	parser.add_argument("-o", help="The NAND OOB size default:%d."%def_oob, type=int, default=def_oob, dest="oob");
	parser.add_argument("-e", help="The NAND erase size default:%d"%def_erasesize, type=int, default=def_erasesize, dest="erasesize");
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
		do_extract_datastructures(argNs.imagefile, argNs.outputfolder, argNs.pagesize, argNs.oob, argNs.erasesize, argNs.csv);
	except Exception as ex: 
		logging.error(traceback.format_exc());
		logging.error("Script aborted.");
	else:
		logging.info("Script finished without errors.");
	return;
	
def do_extract_datastructures(imageFile, outputPath, pagesize, oobsize, erasesize, exportCsv):
	global SCRIPT_NAME;
	try:
		utils.checkReadFile(imageFile);
		outPath = os.path.join(outputPath, OUTPUT_FOLDER);
		utils.createDir(outPath);
	except Exception as ex:
		logging.warning("Error occured while reading image and creating output directory. %s"%str(ex));		
		raise ex;
	
	nandProps = yaffs2_device.NANDProps();
	nandProps.pageSize = pagesize;
	nandProps.eraseSize = erasesize;
	nandProps.spareSize = oobsize;
	_yaffs2dev = yaffs2_device.YaffsDevice(nandProps, imageFile);
	logging.info("Image file set: %s"%imageFile);
	
	sqlPagesize = 1024;
	sqlRessize = 0;
	if(exportCsv):
		report = csvReporter.CsvReporter("%s report on %s "%(SCRIPT_NAME, os.path.basename(imageFile)));		
	else:
		report = htmlReporter.HtmlReporter("%s report on %s "%(SCRIPT_NAME, os.path.basename(imageFile))); #get reporter
	
	report.setOutputPath(outPath, "%s_%s"%(SCRIPT_NAME, os.path.basename(imageFile)));
	carver = sqliteCarver.SqliteCarver();
	carver.setDevice(imageFile);
	carver.setParameters(pagesize, oobsize, sqlPagesize, sqlRessize);
	logging.info("Sqlite carver initiated with parameters: \n fs page size: %d\n fs oob size: %d\n sqlite page size: %d\n sqlite res. region: %d"%(pagesize, oobsize, sqlPagesize, sqlRessize));
	
	logging.info("Carving sqlite records...");
	carver.carve();
	logging.info("Carving done!");	
	
	logging.info("Generating report...");
	report.newSection("Contact Information");
	contactAn = contactAnalyzer.ContactAnalyzer();	
	process_carved_contacts(report, carver, contactAn);
	
	report.newSection("Call history");
	process_carved_callhistory(report, carver, contactAn);
	
	report.newSection("SMS/MMS");
	smsAn = smsAnalyzer.SMSAnalyzer();
	process_carved_sms(report, carver, smsAn);
	
	report.newSection("Gmail communications");
	gmailAn = gmailAnalyzer.GmailAnalyzer();
	process_carved_gmail(report, carver, gmailAn);
	
	report.newSection("Browser information");
	browserAn = browserAnalyzer.BrowserAnalyzer();
	process_carved_browserdata(report, carver, browserAn);
	
	report.newSection("Downloads");
	process_carved_downloadData(report, carver, browserAn);
	
	report.newSection("Google Maps information");
	locAn = locationAnalyzer.LocationAnalyzer();
	process_carved_locationData(report, carver, locAn);
	
	report.newSection("Google Talk information");
	chatAn = chatAnalyzer.ChatAnalyzer();
	process_carved_chatData(report, carver, chatAn);
	
	report.outputReport();
	logging.info("Report done! Output written to %s."%os.path.abspath(outPath));
	return;

def process_carved_gmail(report, carver, gmailAn):	
	logging.info("Processing carved Gmail message records...");
	messageRows = [vls for (cls, vls, hsh, fo, key) in carver.catalogue["MAILSTORE.*@GMAIL.COM.DB-messages"]];
	if(len(messageRows) == 0):
		logging.info("No Gmail message records found");
	else:
		logging.info("Found %d Gmailmessage records"%len(messageRows));
		gmailAn.processCarvedMessageRows(messageRows);
		gmMsgs = [];
		for msg in gmailAn.comms:
			htmlExt = report.getExternal(msg[-1]);
			gmMsgs.append(tuple(list(msg[:-1]) + [htmlExt]));
		report.addTableByList(("id", "message_id", "from", "to", "cc", "bcc", "Sent (utc)", "received (utc)", "subject", "snippet", "body"), sorted(gmMsgs, key=lambda gm: gm[6]), "Gmail messages", "Carved Gmail message records");
	
	logging.info("Processing done!");	
	
def process_carved_chatData(report, carver, chatAn):
	logging.info("Processing carved Google talk records...");
	talkContactRows = [vls for (cls, vls, hsh, fo, key) in carver.catalogue["TALK.DB-contacts"]];
	talkMessageRows = [vls for (cls, vls, hsh, fo, key) in carver.catalogue["TALK.DB-messages"]];
	if(len(talkMessageRows) == 0):
		logging.info("No Google talk records found");
	else:
		logging.info("Found %d Google talk records"%len(talkMessageRows));
		chatAn.processCarvedTalkMessages(talkContactRows, talkMessageRows);
		report.addTableByList(("id", "contact", "message", "date/time (utc)", "real date/time (UTC)", "direction"), chatAn.messages, "Google Talk messages", "Carved Google Talk records");
			
	logging.info("Processing done!");
	
def process_carved_locationData(report, carver, locAn):
	# logging.info("Processing carved maps search history records...");
	# cRows = [vls for (cls, vls, hsh, fo, key) in carver.catalogue["SEARCH_HISTORY.DB-history"]];
	# if(len(cRows) == 0):
		# logging.info("No search history records found");
	# else:
		# logging.info("Found %d search history records"%len(cRows));
		# locAn.processCarvedSearchHistory(cRows);
		# report.addTableByList(("id", "type", "data1", "data2"), locAn.searches, "Google maps searches", "Carved Google Maps search records.");
	# logging.info("Processing done!");
	
	logging.info("Processing carved maps search suggestions records...");
	cRows = [vls for (cls, vls, hsh, fo, key) in carver.catalogue["SEARCH_HISTORY.DB-suggestions"]];
	if(len(cRows) == 0):
		logging.info("No search suggestions records found");
	else:
		logging.info("Found %d search suggestions records"%len(cRows));
		locAn.processCarvedSuggestions(cRows);
		report.addTableByList(("id", "data1", "singleResult", "displayQuery"), locAn.suggestions, "Google maps suggestions", "Carved Google Maps suggestion records.");
	logging.info("Processing done!");
		
	logging.info("Processing carved maps destination history records...");
	cRows = [vls for (cls, vls, hsh, fo, key) in carver.catalogue["DA_DESTINATION_HISTORY-destination_history"]];
	if(len(cRows) == 0):
		logging.info("No maps destination history records found");
	else:
		logging.info("Found %d maps destination history records"%(len(cRows)));
		locAn.processCarvedDestinationHistory(cRows);
		report.addTableByList(("date/time (utc)", "dest. lat/long", "dest. title", "dest. address", "source lat/long"), locAn.routes, "Google maps destinations", "Carved Google Maps destination records.");
	logging.info("Processing done!");	
	
def process_carved_downloadData(report, carver, browserAn):
	logging.info("Processing carved downnload records...");
	dwnlRows = [vls for (cls, vls, hsh, fo, key) in carver.catalogue["DOWNLOADS.DB-downloads"]];
	if(len(dwnlRows) == 0):
		logging.info("No download records found");
	else:
		logging.info("Found %d download records"%len(dwnlRows));
		browserAn.processCarvedDownloadRows(dwnlRows);
		report.addTableByList(("id", "URI", "file location", "last modified (UTC)", "total bytes", "application UID"), sorted(browserAn.downloads, key=lambda dwnl:dwnl[3]), "Downloads", "Carved download records.");
	logging.info("Processing done!");
	
def process_carved_browserdata(report, carver, browserAn):
	logging.info("Processing carved browser history records...");
	histRows = [vls for (cls, vls, hsh, fo, key) in carver.catalogue["BROWSER.DB-bookmarks"]];
	if(len(histRows) == 0):
		logging.info("No browser history records found");
	else:
		logging.info("Found %d browser history records"%len(histRows));
		browserAn.processCarvedHistoryRows(histRows);
		report.addTableByList(("id", "date/time (utc)", "title", "url", "visits"), sorted(browserAn.history, key=lambda hst: hst[1]), "Browser history", "Carved browser history records.");
		report.addTableByList(("id", "title", "url"), browserAn.bookmarks, "Browser bookmarks", "Carved browser bookmark records.");
	logging.info("Processing done!");
		
	logging.info("Processing carved credential records...");
	passwordRows = [vls for (cls, vls, hsh, fo, key) in carver.catalogue["WEBVIEW.DB-password"]];
	httpauthRows = [vls for (cls, vls, hsh, fo, key) in carver.catalogue["WEBVIEW.DB-httpauth"]];
	if(len(passwordRows) == 0 and len(httpauthRows) == 0):
		logging.info("No credential records found");
	else:
		logging.info("Found %d credential records"%(len(passwordRows)+len(httpauthRows)));
		browserAn.processCarvedCredentialRows(passwordRows, httpauthRows);
		report.addTableByList(("id", "host", "username", "password"), browserAn.credentials, "Credentials", "Carved credentials.");
	logging.info("Processing done!");
	
	logging.info("Processing carved cachedpositions records...");
	cposRows = [vls for (cls, vls, hsh, fo, key) in carver.catalogue["CACHEDPOSITIONS.DB-cachedpositions"]];
	if(len(cposRows) == 0):
		logging.info("No carved cachedpositions records found");
	else:
		logging.info("Found %d cachedpositions records"%(len(cposRows)));
		browserAn.processCarvedCachedPosRows(cposRows);
		report.addTableByList(("lat/lng", "alt", "accuracy", "speed", "date/time (utc)"), browserAn.positions, "Cached Positions", "Carved cached positions.");
	logging.info("Processing done!");
	
def process_carved_sms(report, carver, smsAn):	
	logging.info("Processing carved sms records...");
	smsRows = [vls for (cls, vls, hsh, fo, key) in carver.catalogue["MMSSMS.DB-sms"]];
	if(len(smsRows) == 0):
		logging.info("No sms records found");
	else:
		logging.info("Found %d sms records"%len(smsRows));
		smsAn.processCarvedSmsRows(smsRows);
		report.addTableByList(("id", "address", "date/time (utc)", "read", "type", "body"), sorted(smsAn.smsList, key=lambda sms: sms[2]), "Text messages", "Carved SMS records");
	
	logging.info("Processing done!");
	
	
def process_carved_callhistory(report, carver, contactAn):
	logging.info("Processing carved call records...");
	callRows = [vls for (cls, vls, hsh, fo, key) in carver.catalogue["CONTACTS2.DB-calls"]];
	if(len(callRows) == 0):
		logging.info("No call records found.");
	else:
		logging.info("Carved %d call records."%len(callRows));
		contactAn.processCarvedCallsRows(callRows);
		report.addTableByList(("id", "number", "date/time (utc)", "duration", "type", "name"), sorted(contactAn.calls, key=lambda cl: cl[2]), "Contacts2.db call history", "Carved call history.");
		
	logging.info("Processing done!");
	return;
	
def process_carved_contacts(report, carver, contactAn):
	logging.info("Processing carved contact records...");
	contactRows = [vls for (cls, vls, hsh, fo, key) in carver.catalogue["CONTACTS2.DB-raw_contacts"]];
	dataRows = [vls for (cls, vls, hsh, fo, key) in carver.catalogue["CONTACTS2.DB-data"]];	
	if(len(contactRows) == 0):
		logging.info("No contact records found.");
	else:
		logging.info("Carved %d contact records."%len(contactRows));
		contactAn.processCarvedContactsDataRows(contactRows, dataRows);
		report.addTableByList(("Id", "display name", "extra info"), contactAn.contacts, "Contacts2.db contacts", "Carved contacts.");
		
	logging.info("Processing done!");
	return;
	
	
if __name__ == '__main__':
    main()