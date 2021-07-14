import sqlite3;
import os;
import os.path;
import logging;
import datetime;
import sys;


from android.logicalAnalyzer import LogicalAnalyzer;

class ContactAnalyzer(LogicalAnalyzer):		
	MTYPE_PHONE = 10;
	MTYPE_NAME = 6;
	MTYPE_EMAIL = 1;
	MTYPE_POSTADDR = 3;
	MTYPE_IM = 2;
	
	CALL_TYPES = {1: "In", 2: "Out", 3: "Missed"}
	MIME_TYPES = {1: "Email", 2: "IM", 3: "Postal address", 6: "Name", 10: "Phone"};
	
	def __init__(self):
		self.contacts = [];
		self.calls = [];
	
	'''Class to hold sms analysis logic.'''
	def doAnalysis(self, contactDb):
		self.contacts = []; 
		self._checkDbFile(contactDb);
		self.parseContacts(contactDb);
		self.parseCallHistory(contactDb);
		pass;

	def parseContacts(self, contactDb):
		'''Sets a list (id, display name, updates, names, emails, addresses, phones)		
		Given a contacts2.db file, parse out the contacts. 
		WARNING: the mimetypes may differ, these should be extracted from mimetypes table'''
		logging.debug("Parsing contacts from database...");
		self.contacts = [];
		with sqlite3.connect(contactDb) as conn:
			mimStr = "(%d, %d, %d, %d, %d)"%(self.MTYPE_PHONE, self.MTYPE_NAME, self.MTYPE_EMAIL, self.MTYPE_POSTADDR, self.MTYPE_IM);
			conn.isolation_level = None;
			ch = conn.cursor();
			query = '''SELECT cts._id, rc.display_name, stat.status, dt.mimetype_id, dt.data1, dt.data2, dt.data3, dt.data4, dt.data5, dt.data6, dt.data7, dt.data8, dt.data9
						FROM contacts cts LEFT JOIN raw_contacts rc ON (cts._id=rc.contact_id)
											 INNER JOIN data dt ON (dt.raw_contact_id=rc._id)
											 LEFT JOIN status_updates stat ON (cts.status_update_id=stat.status_update_data_id)
						WHERE dt.mimetype_id in %s;'''%mimStr;
			ch.execute(query);
			contactRows = ch.fetchall();
			
		tempContacts = {}
		for cr in contactRows:
			if(cr[0] not in tempContacts.keys()):
				tempContacts[cr[0]] = (cr[1], [], [], [], [], []); #(display name, updates, names, emails, addresses, phones)
				
			if(cr[2] is not None): #There is a status
				tempContacts[cr[0]][1].append(cr[2]);
			if(cr[3] is not None): #There is data
				dataList = (None, cr[4] , cr[5] , cr[6], cr[7], cr[8], cr[9], cr[10], cr[11], cr[12]); # -3 is data column
				if(cr[3] == self.MTYPE_PHONE):
					tempContacts[cr[0]][5].append(dataList[4][::-1]);
				elif(cr[3] == self.MTYPE_NAME):
					tempContacts[cr[0]][2].append("%s %s"%(dataList[2] or "", dataList[3] or ""));
				elif(cr[3] == self.MTYPE_EMAIL):
					tempContacts[cr[0]][3].append(dataList[1]);
				elif(cr[3] == self.MTYPE_POSTADDR):
					tempContacts[cr[0]][4].append("%s %s %s"%(dataList[4] or "", dataList[9] or "", dataList[7] or ""));
				else: #other mimetypes not supported
					pass;
		
		for (ky, vl) in tempContacts.items():
			self.contacts.append(tuple([ky] + list(vl)));
			
		logging.debug("Parsing done! Found %d contacts."%len(self.contacts));	
		return;

	def parseCallHistory(self, contactDb):
		'''Sets a list (id, number, date/time (UTC), duration, type, name)'''
		logging.debug("Parsing call history from database...");
		self.calls = [];
		with sqlite3.connect(contactDb) as conn:
			conn.isolation_level = None;
			ch = conn.cursor();
			query = '''SELECT cl._id, cl.number, cl.date, cl.duration, cl.type, cl.name
						FROM calls cl ''';
			ch.execute(query);
			callRows = ch.fetchall();
			
		for cr in callRows: 
			dt = datetime.datetime.utcfromtimestamp(cr[2]/1000);				
			self.calls.append((cr[0], cr[1], dt, cr[3], self.CALL_TYPES[cr[4]], cr[5]));
				
			#logging.debug(self.contacts);
		logging.debug("Parsing done! Found %d call records."%len(self.calls));	
		return;
	
	def processCarvedCallsRows(self, callsRows):
		'''Sets a dictionary containing formatted tuples (number, date/time (utc), duration, type, name)''';
		self.calls = [];
		for cr in callsRows:
			dt = datetime.datetime.utcfromtimestamp(cr[2]/1000) if not(type(cr[2]).__name__ == "str") else cr[2];
			self.calls.append((cr[0], cr[1], dt, cr[3], self.CALL_TYPES[cr[4]], cr[6]));			
		return;
		
	def processCarvedContactsDataRows(self, rawContactRows, dataRows):
		'''Sets a dictionary with formatted tuples (display names, extra info)'''
		self.contacts = []; 
		tempContacts = {};
		processedData = [];
		for cr in rawContactRows:
			if(cr[0] not in tempContacts.keys()):
				tempContacts[cr[0]] = (cr[16], []); #(display name, names, emails, addresses, phones)
						
			for dr in dataRows:
				#print(dr);
				#print("matching %d to %d"%(dr[3], cr[0]));
				if(dr[3] == cr[0] and (dr[0] not in processedData)): #There is data for this contact
					dataList = (None, dr[7] , dr[8] , dr[9], dr[10], dr[11], dr[12], dr[13], dr[14], dr[15]); # -3 is data column
					dataStr = ""; 
					for dl in dataList:
						dataStr += " %s"%dl if dl is not None else "";
					if(len(dataStr) > 0):
						tempContacts[cr[0]][1].append(dataStr);
					processedData.append(dr[0]);
		
		for (ky, vl) in tempContacts.items():
			self.contacts.append(tuple([ky] + list(vl)));			