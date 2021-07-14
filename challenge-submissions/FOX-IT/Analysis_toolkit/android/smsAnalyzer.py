import sqlite3;
import os;
import os.path;
import logging;
import datetime;
import sys;

from android.logicalAnalyzer import LogicalAnalyzer;

class SMSAnalyzer(LogicalAnalyzer):		
	'''Class to hold sms analysis logic.'''
	SMS_TYPE_DIR = {1: "in", 2: "out", 3:"draft", 4:"pending"}; #maps type num to direction
	
	smsList = [];
	mmsList = [];
		
	def doAnalysis(self, smsDb):	
		self._checkDbFile(smsDb);
		self._parseSms(smsDb);
		self._parseMms(smsDb);
		pass;

	def _parseSms(self, smsDb):
		logging.debug("Parsing SMS messages from database...");
		self.smsList = [];
		with sqlite3.connect(smsDb) as conn:
			conn.isolation_level = None;
			ch = conn.cursor();
			query = "SELECT sms.address, sms.date, sms.read, sms.type, sms.body \
					FROM sms LEFT JOIN threads trds ON (trds._id=sms.thread_id)";
			ch.execute(query);
			self.smsList.append(["Address", "date/time (UTC)", "read", "type", "body"]);
			results = ch.fetchall();
			for res in results:
				self.smsList.append([res[0], datetime.datetime.utcfromtimestamp(res[1]/1000), bool(res[2]), self.SMS_TYPE_DIR[res[3]], res[4]]);
			#logging.debug(results);
		logging.debug("Parsing done! Found %d messages."%len(results));	
		return;
	
	def _parseMms(self, smsDb):
		return;
		logging.debug("Parsing MMS messages from database...");
		with sqlite3.connect(smsDb) as conn:
			conn.isolation_level = None;
			ch = conn.cursor();
		
		logging.debug("Parsing done!");	
		return;
		
	def processCarvedSmsRows(self, smsRows):
		''' Add formatted tuples (id, address, date/time (utc), read, type, body)'''
		self.smsList = [];
		for sr in smsRows:
			dt = datetime.datetime.utcfromtimestamp(sr[4]/1000) if not(type(sr[4]).__name__ == "str") else sr[4];	
			self.smsList.append((sr[0], sr[2], dt, sr[6], self.SMS_TYPE_DIR[sr[8]], sr[11]));
		return;