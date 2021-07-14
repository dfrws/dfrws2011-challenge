import sqlite3;
import os;
import os.path;
import logging;
import datetime;
import sys;

from android.logicalAnalyzer import LogicalAnalyzer;

class ChatAnalyzer(LogicalAnalyzer):		
	'''Class to hold message analysis logic.'''
	MSG_TYPE = {1: "in", 0: "out"}; 
	
	def doAnalysis(self):	
		pass;
		
	def processCarvedTalkMessages(self, contactRows, msgRows):
		'''Sets a message list with formatted tuples (id, contact, message, date, real date, direction) and
		   Sets a contact list with formatted tuples (id, username, nickname)'''
		self.messages = []; 
		tempContacts = {};
		for cr in contactRows:
			if(cr[0] not in tempContacts.keys()):
				tempContacts[cr[0]] = ((cr[1], cr[2])); #(display name, names, emails, addresses, phones)
				
		for mr in msgRows:
			dt = datetime.datetime.utcfromtimestamp(mr[4]/1000);
			realDt = datetime.datetime.utcfromtimestamp(mr[5]/1000);
			if(mr[1] in tempContacts.keys()):
				contactInfo = "%s (%s)"%(tempContacts[mr[1]][1], tempContacts[mr[1]][0]);
			else: 
				contactInfo = "**unknown contact**";
			self.messages.append((mr[0], contactInfo, mr[3], dt, realDt, self.MSG_TYPE[mr[6]]));
			
		return;