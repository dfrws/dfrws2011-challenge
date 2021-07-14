import sqlite3
import os;
import os.path;
import logging;
import datetime;
import sys;

from android.logicalAnalyzer import LogicalAnalyzer;

class BrowserAnalyzer(LogicalAnalyzer):			
	'''Class to hold browser analysis logic.'''
	
	def doAnalysis(self, browserDb, webviewDb, wvCacheDb, userdatapath):
		self.history = [];
		self.bookmarks = [];
		self.cached = [];
		self.credentials = [];
		self._checkDbFile(browserDb);
		self._checkDbFile(webviewDb);
		self._checkDbFile(wvCacheDb);
		self.parseBookmarks(browserDb);
		self.parseHistory(browserDb);
		self.parseCredentials(webviewDb);
		self.parseCached(wvCacheDb, userdatapath);
		pass;

	def parseBookmarks(self,browserDb):
		logging.debug("Parsing browser bookmarks from database...");
		self.bookmarks = [];
		with sqlite3.connect(browserDb) as conn:
			conn.isolation_level = None;
			ch = conn.cursor();
			query = '''SELECT bm.title, bm.url
						FROM bookmarks bm
						WHERE bm.bookmark=1;
						''';
			ch.execute(query);
			bmRows = ch.fetchall();
			
			for bm in bmRows:	
				self.bookmarks.append((bm[0], bm[1]));				
			
		logging.debug("Parsing done! Found %d bookmarks."%len(self.bookmarks));	
		return;

	def parseHistory(self, browserDb):
		logging.debug("Parsing browser history from database...");
		self.history = [];
		with sqlite3.connect(browserDb) as conn:
			conn.isolation_level = None;
			ch = conn.cursor();
			query = '''SELECT bm.title, bm.url, bm.visits, bm.date
						FROM bookmarks bm
						WHERE bm.bookmark=0;
						''';
			ch.execute(query);
			hstRows = ch.fetchall();
			
			for hst in hstRows:	
				title = "" if hst[0] == hst[1] else hst[0];
				dt = datetime.datetime.utcfromtimestamp(hst[3]/1000);
				self.history.append((dt, hst[2], title, hst[1]));				
			
		logging.debug("Parsing done! Found %d history records."%len(self.history));	
		return;
		
	def parseCredentials(self, webviewDb):
		logging.debug("Parsing credentials from database...");
		self.credentials = [];
		with sqlite3.connect(webviewDb) as conn:
			conn.isolation_level = None;
			ch = conn.cursor();
			query = '''SELECT pwd.host, pwd.username, pwd.password 
						FROM password pwd 
						''';
			ch.execute(query);
			crRows = ch.fetchall();
			
			for cr in crRows:	
				self.credentials.append((cr[0], cr[1], cr[2]));		
			
			query = '''SELECT ht.host, ht.username, ht.password
						FROM httpauth ht
						'''
			ch.execute(query);
			htRows = ch.fetchall();
			
			for ht in htRows:	
				self.credentials.append((ht[0], ht[1], ht[2]));	
				
		logging.debug("Parsing done! Found %d credentials."%len(self.credentials));	
		return;
		
	def parseCache(self, wvCacheDb, userdatapath):
		logging.debug("Parsing web cache from database...");
		appFolder = os.path.dirname(os.path.dirname(os.path.abspath(wvCacheDb)));
		dirTuples = os.walk(appFolder);
		cacheFolder = None;
		for (dirname, dirs, files) in os.walk(appFolder):
			if(os.path.basename(dirname).lower() == "webviewcache"):
				logging.debug("Cache folder found at %s"%dirname);
				cacheFolder = os.path.abspath(dirname);
				break;
		
		if(cacheFolder is None):
			logging.warning("No corresponding cache dir found for %s"%wvCacheDb);
			self.cached = [];
			return;
		
		self.cached = [];
		with sqlite3.connect(wvCacheDb) as conn:
			conn.isolation_level = None;
			ch = conn.cursor();
			query = '''SELECT ch.url, ch.filepath, ch.expires, ch.mimetype, ch.contentlength
						FROM cache ch
						''';
			ch.execute(query);
			chRows = ch.fetchall();
			
			for ch in chRows:	
				dt = datetime.datetime.utcfromtimestamp(ch[2]/1000);
				filepath = os.path.join(cacheFolder, ch[1]);
				mts = os.stat(filepath).st_mtime;				
				mdt = datetime.datetime.utcfromtimestamp(mts/1000);
				self.cached.append((ch[0], os.path.relpath(filepath, userdatapath), mdt, dt, ch[3], ch[4]));		
							
		logging.debug("Parsing done! Found %d cached files."%len(self.cached));	
		return;
	
	def processCarvedDownloadRows(self, dwnlRows):
		'''Sets the downloadn lists. (id, uri, _data, lastmod, totalbytes, uid)''' 
		self.downloads = [];
		for dwnl in dwnlRows:
			dt = datetime.datetime.utcfromtimestamp(dwnl[15]/1000) if not(type(dwnl[15]).__name__ == "str") else dwnl[15];
			self.downloads.append((dwnl[0], dwnl[1], dwnl[7], dt, dwnl[22], dwnl[25]));
		return;
		
	def processCarvedHistoryRows(self, histRows):
		'''Sets the bookmark and history lists. bookmarks: (id, title, url), history: (id, date/time (utc), title, url, visits )''' 
		self.bookmarks = [];
		self.history = [];
		for hst in histRows:
			if(hst[7] == 1):
				title = "" if hst[2] == hst[1] else hst[1];
				self.bookmarks.append((hst[0], title, hst[2]));
			else:
				title = "" if hst[1] == hst[2] else hst[1];
				dt = datetime.datetime.utcfromtimestamp(hst[4]/1000);
				self.history.append((hst[0], dt, title, hst[2], hst[3]));
		return;
		
	def processCarvedCachedPosRows(self, cposRows):
		'''Sets the positions lists as (lat/long, altitude, accuracy, speed, date/time (utc))		
		''' 
		self.positions = [];
		for pr in cposRows: 
			dt = datetime.datetime.utcfromtimestamp(pr[7]/1000);
			latlng = "%s/%s"%(str(pr[0]), str(pr[1]));
			self.positions.append((latlng, pr[2], pr[3], pr[6], dt));
		return;
		
		
	def processCarvedCredentialRows(self, passwRows, httpauthRows):
		'''Sets the credential lists as (id, host, username,password)
		Also filters out carved records that have a key larger than 65000. 
		''' 
		self.credentials = [];
		for pr in passwRows:
			if(pr[0] < 65000):
				self.credentials.append((pr[0], pr[1], pr[2], pr[3]));
		for hr in httpauthRows: 
			if(hr[0] < 65000):
				self.credentials.append((hr[0], hr[1], hr[3], hr[4]));
		return;