#include "stdafx.h"
#include "windows.h"
#include "assert.h"

#include "ctypes.h"

//--------------------------------------------------------------------------------------------------------------------------------------
#define SimpleTest(type,FuncName,mask,off)\
	ECTYPE FuncName(BYTE* pCluster){\
	ECTYPE et=ctype_undefined;\
	if (0==memcmp(pCluster+off,mask,sizeof(mask))) et=type;\
	return et;\
}
//SimpleTest(ctype_jpg,TestJPG)


//--------------------------------------------------------------------------------------------------------------------------------------
BYTE maskjpg[]  ={0xFF,0xD8,0xFF,0xE0,0x00,0x10,'J','F','I','F'};

ECTYPE TestJPG(BYTE* pCluster){
	ECTYPE et=ctype_undefined;
	if (0==memcmp(pCluster+6,maskjpg,sizeof(maskjpg))) et=ctype_jpg;

	return et;
}

//--------------------------------------------------------------------------------------------------------------------------------------
BYTE maskexif[] ={0xFF,0xD8,0xFF,0xE1,0x1D,0xFC,'E','x','i','f'};

ECTYPE TestEXIF(BYTE* pCluster){
	ECTYPE et=ctype_undefined;
	if (0==memcmp(pCluster+6,maskexif,sizeof(maskexif))) et=ctype_exif;

	return et;
}

//--------------------------------------------------------------------------------------------------------------------------------------
BYTE maskedel[] ={0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE};
BYTE maskempty[] ={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
BYTE maskempty2[] ={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

ECTYPE TestEmpty(BYTE* pCluster){
	ECTYPE et=ctype_undefined;
	if (0==memcmp(pCluster,maskedel,sizeof(maskedel)))  et=ctype_empty;
	if (0==memcmp(pCluster,maskempty,sizeof(maskempty))) et=ctype_empty;
	if (0==memcmp(pCluster,maskempty2,sizeof(maskempty2))) et=ctype_empty;

	return et;
}

//--------------------------------------------------------------------------------------------------------------------------------------
BYTE maskezip[] ={'P','K',0x03,0x04,0x0A,0x00,0x00,0x00};
ECTYPE TestZIP(BYTE* pCluster){
	ECTYPE et=ctype_undefined;
	if (0==memcmp(pCluster,maskezip,sizeof(maskezip)))  et=ctype_zip;

	return et;
}


//--------------------------------------------------------------------------------------------------------------------------------------
BYTE maskfat[] ={0xFE,0xFF};
ECTYPE TestFAT(BYTE* pCluster){
	ECTYPE et=ctype_undefined;
	if (0==memcmp(pCluster,maskezip,sizeof(maskezip)))  et=ctype_fat;
	
	for (int j=2;j<0x10;j++) 
		if (pCluster[j]<0xF7) {et=ctype_undefined;break;}

	return et;
}

//--------------------------------------------------------------------------------------------------------------------------------------
BYTE masklikefat[] ={0xDF,0xDA,0x19,0x00,0xD7,0x98,0xD3,0x0A};
ECTYPE TestLikeFAT(BYTE* pCluster){
	ECTYPE et=ctype_undefined;
	if (0==memcmp(pCluster,masklikefat,sizeof(masklikefat)))  et=ctype_likefat;

	return et;
}

//--------------------------------------------------------------------------------------------------------------------------------------
BYTE maskdir_1[]  ={".          "};
BYTE maskdir_1a[] ={"PHONE      "};
BYTE maskdir_2[]  ={"..         "};
ECTYPE TestDIR(BYTE* pCluster){
	ECTYPE et=ctype_undefined;
	if (0==memcmp(pCluster,maskdir_1, sizeof(maskdir_1)))      et=ctype_likefat;
	if (0==memcmp(pCluster,maskdir_1a,sizeof(maskdir_1a)))     et=ctype_likefat;

	if (0!=memcmp(pCluster+0x20,maskdir_2,sizeof(maskdir_2)))  et=ctype_undefined;

	return et;
}

//--------------------------------------------------------------------------------------------------------------------------------------
BYTE maskdir2[]  ={'/',0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
ECTYPE TestDIR2(BYTE* pCluster){
	ECTYPE et=ctype_undefined;
	if (0==memcmp(pCluster,maskdir2, sizeof(maskdir2)))      et=ctype_dir2;

	return et;
}



