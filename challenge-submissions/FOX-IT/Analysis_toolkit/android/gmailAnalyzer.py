import sqlite3;
import os;
import os.path;
import logging;
import datetime;
import sys;

from android.logicalAnalyzer import LogicalAnalyzer;

class GmailAnalyzer(LogicalAnalyzer):			
	'''Class to hold gmail analysis logic.'''
	
	def doAnalysis(self, gdb, downloadDb):
		self.comms = {}; 
		self._checkDbFile(downloadDb);
		self._checkDbFile(gdb);
		self.parseGmailComm(gdb, downloadDb);
		pass;

	def parseGmailComm(self, gmailDb, downloadDb):
		logging.debug("Parsing gmail communication from database...");
		self.comms = [];
		downloadIds = [];
		with sqlite3.connect(gmailDb) as conn:
			conn.isolation_level = None;
			ch = conn.cursor();
			query = '''SELECT msg._id, msg.fromAddress, msg.toAddresses, msg.ccAddresses, msg.bccAddresses, msg.dateReceivedMs, 
								msg.dateSentMs, msg.subject, msg.body, msg.conversation, att.downloadId, 
								att.messages_partId, msg.snippet
						FROM messages msg LEFT JOIN attachments att ON (att.messages_messageId = msg.messageId)
											LEFT JOIN conversations cnv ON (msg.conversation = cnv._id)
						''';
			ch.execute(query);
			emailRows = ch.fetchall();
			
			tempComms = {};
		for er in emailRows:	
			if(er[0] not in tempComms.keys()):
				dtReceived = datetime.datetime.utcfromtimestamp(er[5]/1000);
				dtSent = datetime.datetime.utcfromtimestamp(er[6]/1000);
				tempComms[er[0]] = (er[1], er[2], er[3], er[4], dtReceived, dtSent, er[7], er[12], er[8], {}); #(0from, 1to, 2cc, 3bcc, 4received, 5sent, 6subject, snippet, body 9attachments)
				
			if(er[10] is not None): #There is an attachment, is there always a downloaddb entry??
				with sqlite3.connect(downloadDb) as conn2:
					conn2.isolation_level = None;
					ch = conn2.cursor();
					query = '''SELECT dwnl._id, dwnl._data, dwnl.lastmod, dwnl.total_bytes, dwnl.current_bytes, dwnl.title
								FROM downloads dwnl
								WHERE dwnl._id = %s;'''%er[10];
												
					ch.execute(query);
					dlRow = ch.fetchone();
					dldt = datetime.datetime.utcfromtimestamp(dlRow[2]/1000);
					tempComms[er[0]][9][er[11]] = (dlRow[1], dldt, dlRow[3], dlRow[4], dlRow[5]);
					
		for (ky, vl) in tempComms.items():
			self.comms.append(tuple([ky] + list(vl)));
		
		logging.debug("Parsing done! Found %d emails."%len(self.comms));	
		return;
	
	def processCarvedMessageRows(self, messageRows):
		''' Add formatted tuples (id, messageid, from, to, cc, bcc, Sent (utc), received (utc), subject, snippet, body)'''
		self.comms = [];
		for sr in messageRows:
			dtSent = datetime.datetime.utcfromtimestamp(sr[8]/1000) if not(type(sr[8]).__name__ == "str") else sr[8];
			dtReceived = datetime.datetime.utcfromtimestamp(sr[9]/1000) if not(type(sr[9]).__name__ == "str") else sr[9];
			self.comms.append((sr[0], sr[1], sr[3], sr[4], sr[5], sr[6], dtSent, dtReceived, sr[10], sr[11], sr[14]));	
		return self.comms.sort(key=lambda resval: resval[4]);
		