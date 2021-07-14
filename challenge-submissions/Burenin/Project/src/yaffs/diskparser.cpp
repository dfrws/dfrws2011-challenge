#include "stdafx.h"
#include "windows.h"
#include "assert.h"
#include "ctypes.h"
#include "testhelper.h"



//--------------------------------------------------------------------------------------------------------------------------------------
ECTYPE SignatureTest(BYTE* pCluster){
	ECTYPE et=ctype_undefined;

	if (et==ctype_undefined) et=TestJPG(pCluster);
	if (et==ctype_undefined) et=TestEmpty( pCluster);
	if (et==ctype_undefined) et=TestJPG( pCluster);
	if (et==ctype_undefined) et=TestEXIF( pCluster);
	if (et==ctype_undefined) et=TestZIP( pCluster);

	if (et==ctype_undefined) et=TestFAT( pCluster);
	if (et==ctype_undefined) et=TestLikeFAT( pCluster);
	if (et==ctype_undefined) et=TestDIR( pCluster);
	if (et==ctype_undefined) et=TestDIR2( pCluster);

	return et;
}
//--------------------------------------------------------------------------------------------------------------------------------------

int AnalizeCluster(BYTE* pCluster,ULONG szCluster,PTCTYPEINFO pti){
	ECTYPE eCurType;

	eCurType=SignatureTest(pCluster);

	return 0;
}

//--------------------------------------------------------------------------------------------------------------------------------------
int Parse(char* pcFileName,ULONG ulStart, ULONG ulBlockSize){
	FILE* f;
	ULONG szCluster=0x1000;
	BYTE* bCluster=new BYTE[szCluster];

	if (NULL==(f=fopen(pcFileName,"rb"))) return -1;
	
	fseek(f,0,SEEK_END);ULONG ulFileSize=ftell(f);fseek(f,0,SEEK_SET);
	if (ulStart+ulBlockSize>=ulFileSize) return -2;

	if (ulBlockSize==0) ulBlockSize=ulFileSize-ulStart+1;
	
	TCTYPEINFO PreClusterInfo,CurCusterInfo;

	for (ULONG i=0;i<ulBlockSize;i+=szCluster){
		fseek(f,i+ulStart,SEEK_SET);
		fread(bCluster,1,szCluster,f);

		AnalizeCluster(bCluster,szCluster,&CurCusterInfo);

		PreClusterInfo=CurCusterInfo;
	}

	delete[]bCluster;
	return 0;
}