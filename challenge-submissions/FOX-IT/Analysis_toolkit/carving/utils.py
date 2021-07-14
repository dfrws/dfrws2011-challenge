import struct;
import logging;

def parseVarint(varintBytes, offset, signed=False):
	''' Parses a varint value from the provided bytes. Returns the byte size of the varint and the value.'''
	viBytes = varintBytes[offset:];
	# walk max 9 bytes, while the MSB is 1, add the last 7 LSB's to the integer. 
	byteNum = 0;
	value = 0;
	intComplete = False;
	while (byteNum < 9 and byteNum < len(viBytes) and not intComplete): 
		(viByte,) = struct.unpack(">B", viBytes[byteNum]);
		if((viByte & 0b10000000) == 0b10000000 and byteNum < 8): #not yet the last varint byte
			value = (value << 7) | (viByte & 0b01111111); #add the 7 LSB's to the value
		elif((viByte & 0b10000000) == 0b10000000 and byteNum == 8): #last stop, let's finalyze the integer
			value = (value << 8) | (viByte);
			intComplete = True;
		else: #that's our signal, marks the end of the varint
			value = (value << 7) | (viByte & 0b01111111);
			intComplete = True;
		byteNum += 1;
	if not intComplete:
		raise ValueError("No valid varint found.");		
	return (byteNum, value);
	
def isOdd(num):
	return bool(num & 1);