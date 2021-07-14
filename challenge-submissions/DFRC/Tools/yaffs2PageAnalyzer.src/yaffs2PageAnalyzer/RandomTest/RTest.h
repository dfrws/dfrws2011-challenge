/**
 *	@file	RTest.h
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


#pragma once
#include <math.h>

#define RANDOM_BLOCK		0xA0
#define NON_RANDOM_BLOCK	0xA1

#define BYTESIZE	256L

//--------------------------------------------------------
// Randomness Test Functions

int Randomness_Test(char *buffer, unsigned long bufSize);

double pokerTest(unsigned long numberOfBits, short m);

void processInBuffer(char *inBuf, int count);

int CheckRamSlack(char *buffer, unsigned long bufSize);

int IsAsciiData(char *buffer, unsigned long bufSize);

