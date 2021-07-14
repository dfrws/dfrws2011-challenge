# extractObjectHeaders.py
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

from yaffsCore import *;
import utils;

OUTPUT_FOLDER = "extractObjectHeaders";
VERSION = "0.1";
SCRIPT_NAME = "extractObjectHeaders";

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
	parser = argparse.ArgumentParser(description="Carve and parse the YAFFS2 object headers from a YAFFS image file.");
	
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
		do_extract_objectheaders(argNs.imagefile, argNs.outputfolder, argNs.pagesize, argNs.oob, argNs.erasesize);
	except Exception as ex: 
		logging.error("Script aborted.");
	else:
		logging.info("Script finished without errors.");
	return;
	
def do_extract_objectheaders(imageFile, outputPath, pagesize, oobsize, erasesize):
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
	
	logging.info("Scanning image file and building objects...");
	_yaffs2dev.scanNandImage();
	logging.info("Scan done!");
	
	logging.info("Analyzing object headers...");
	ohDict = _yaffs2dev.getObjectHeaders();	
	curOheaders = mostrecent_object_headers(ohDict);
	logging.info("Analysis done!");
	
	logging.info("Exporting object header...");
	fnPrefix = os.path.basename(imageFile);
	export_object_headers(ohDict, curOheaders, outPath, fnPrefix);
	logging.info("Export done! Output written to %s"%os.path.abspath(outputPath));
	
def export_object_headers(ohs, lastOhs, outputPath, fnPrefix):	
	csvFile = os.path.join(outputPath, "%s_object_headers.csv"%fnPrefix);
	csvFile2 = os.path.join(outputPath, "%s_last_state.csv"%fnPrefix);	
	logging.debug("Writing to csv file %s..."%csvFile);
	with open(csvFile, "wb") as fh:
		writer = csv.DictWriter(fh, ohs.values()[0][0].toDict().keys());
		writer.writeheader();
		for oheaders in ohs.values():
			for oh in oheaders:
				writer.writerow(oh.toDict());
	logging.debug("Writing done!");
	logging.debug("Writing to csv file %s..."%csvFile2);
	with open(csvFile2, "wb") as fh:
		writer = csv.DictWriter(fh, lastOhs[0].toDict().keys());
		writer.writeheader();
		for oh in lastOhs:
			writer.writerow(oh.toDict());
	logging.debug("Writing done!");
	return;

def mostrecent_object_headers(ohDict):
	'''For each file/folder/special etc extract the last version of the object header. 
	This is done by comparing the max(modtime, accesstime, createtime) of each of the version. The object with 
	the highest timestamp is the last version. 
	If the max of the timestamps are equal, than the object header with the higher offset in the image is chosen. '''
	results = [];
	for name in ohDict.keys():
		oheaders = ohDict[name];
		kingOh = oheaders[0];		
		for oh in oheaders:
			if(max(oh.atime, oh.mtime, oh.ctime) > max(kingOh.atime, kingOh.mtime, kingOh.ctime)):
				kingOh = oh;
			elif(max(oh.atime, oh.mtime, oh.ctime) == max(kingOh.atime, kingOh.mtime, kingOh.ctime)):
				if(oh.offset > kingOh.offset):
					kingOh = oh;
					
		results.append(kingOh);
	return results;

if __name__ == '__main__':
    main()