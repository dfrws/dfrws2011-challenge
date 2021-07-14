def find_object_chunks(outputPath, searchStr):
	_check_NAND_set();
	global _yaffs2dev;
	ohDict = _yaffs2dev.getObjectHeaders();	
	found = False;
	
	logging.debug("Searching object headers for '%s'..."%searchStr); 
	sys.stdout.write("\nSearching object headers for '%s'..."%searchStr);
	for key in ohDict.keys(): #first, we locate the object headers
		if searchStr.lower() == key:
			found = True;
			oheaders = ohDict[key];		
	if(found):
		logging.debug("Found %d object headers"%len(oheaders));			
		sys.stdout.write("done! %d object headers found."%len(oheaders));
	if not(found):
		logging.debug("No object headers found containing '%s'"%searchStr);
		sys.stdout.write("done! No object headers found.");
		return;
	chunks = [];
	oheaders.sort(key=lambda oh: oh.mtime);
	logging.debug("Scanning for trailing chunks...");
	sys.stdout.write("\nScanning for trailing chunks...");
	numChunks = 8;
	for oh in oheaders: #next, we collect the preceeding chunks		
		curOffset = oh.offset +  _yaffs2dev.chunkSize;
		(chunksData, nextOffset) = _yaffs2dev.readChunkData(curOffset, numChunks);
		tempChunks = [];
		ptr = 0;
		boink = False;
		while len(chunksData) != 0 and not boink:
			while(ptr + _yaffs2dev.chunkSize) < len(chunksData): #still a chunk left here
				chunk = chunksData[ptr: ptr + _yaffs2dev.chunkSize];
				if(YaffsObjectHeader.isHeader(chunk)): #boink, stop here
					boink = True;
					break;
				else:
					tempChunks.append((chunk, curOffset + ptr)); 
				ptr += _yaffs2dev.chunkSize;
			curOffset = nextOffset;
			(chunksData, nextOffset) = _yaffs2dev.readChunkData(curOffset, numChunks);
		chunks.extend(tempChunks);
	logging.debug("Scanning done! Found %d chunks"%len(chunks));
	sys.stdout.write("done! %d chunks found."%len(chunks));
	
	# Oke, now we read the chunks and write them to an output file
	chunkFilePath = os.path.join(outputPath, "%s_chunks.bin"%searchStr);
	logging.debug("Writing chunks to chunkFile %s..."%chunkFilePath);
	sys.stdout.write("\nWriting chunks to chunkFile %s..."%chunkFilePath);
	with open(chunkFilePath, "wb") as fh:
		for chunk in chunks:
			fh.write(chunk[0]);
	
	logging.debug("Done writing!");
	sys.stdout.write("done!");