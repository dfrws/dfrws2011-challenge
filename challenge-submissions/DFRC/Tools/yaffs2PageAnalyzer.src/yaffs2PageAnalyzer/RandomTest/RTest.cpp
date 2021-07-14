/**
 *	@file	RTest.cpp
 *	@date	2011/06/15
 *	@author	Jungheum Park

 *	@brief	Randomness Test
 */

/**
	@mainpage	Randomness Test

	@section	MODIFYINFO	History

		- Jungheum Park / 2011.06.15
			- Initial Version
 */


#include "../stdafx.h"
#include "RTest.h"


unsigned long gFreqCount[BYTESIZE];

int Randomness_Test(char *buffer, unsigned long bufSize)
{
	int		ret = NON_RANDOM_BLOCK;
	double	testResult = 0;
	
	ZeroMemory(gFreqCount, sizeof(gFreqCount));

	// Ignore the ram slack -> 0x00
	bufSize = CheckRamSlack(buffer, bufSize);
	
	if(bufSize < 256)
		bufSize = 256;

	processInBuffer(buffer, bufSize);

	testResult = pokerTest(bufSize*8, 8);

	if(testResult < (double)1000)
	{
		ret = RANDOM_BLOCK;
	}
	else if(testResult > 1000 && testResult < 5000)
	{
		// Check ASCII only page
		if(IsAsciiData(buffer, bufSize) == TRUE)
		{
			ret = NON_RANDOM_BLOCK;
		}
		else
		{
			ret = RANDOM_BLOCK;
		}
	}
	else
	{
		ret = NON_RANDOM_BLOCK;
	}

	return ret;
}

int IsAsciiData(char *buffer, unsigned long bufSize)
{
	BYTE ch;
	UINT count = 0;
	BOOL asciiData = FALSE;

	for(UINT i = 0; i < bufSize; i++)
	{
		ch = buffer[i];

		if(ch >= 0x20 && ch < 0x7F)
			count++;
	}

	if(count*2 >= bufSize)
		asciiData = TRUE;
	else
		asciiData = FALSE;

	return asciiData;
}

int CheckRamSlack(char *buffer, unsigned long bufSize)
{
	int count = 0;
	int startOfRamSlack = bufSize;

	for(int i = bufSize-1; i >= 0; i--)
	{
		if(buffer[i] != 0x00)
			break;
		else
			count++;
	}	

	if(count >= 5)
	{
		startOfRamSlack = bufSize-count;
	}

	return startOfRamSlack;
}

double pokerTest(unsigned long numberOfBits, short m)
{
	int		i;
	double	size;
	double	poker = 0;
	unsigned long	F, f, fsq, pokerF;

	F		= 0;
	f		= 0;
	fsq		= 0;
	pokerF	= numberOfBits/m;
	size	= pow((double)2, m);

	for (i = 0; i < size; i++)	// size = 256
	{
		if (f <= pokerF)			
			F = f;			

		f   += gFreqCount[i];
		fsq += gFreqCount[i] * gFreqCount[i];
	}

	if (f <= pokerF)
		F = f;					

	poker = ((double)(size) / (double)F) * (double)fsq - (double)F;
	return poker;
}

void processInBuffer(char *inBuf, int count)
{
	BYTE ch, next;

	ch = inBuf[0];
	gFreqCount[ch] = 1L;

	for(int i = 1; i < count; i++)
	{
		next = inBuf[i];
		gFreqCount[next]++;
		ch = next;
	}
}
