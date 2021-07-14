#include "stdafx.h"
//---------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------
int process_tar_uncompress(char* pcSourceImage,char* pcOutDirectory)
{
	char strFile[MAX_PATH];//=C:\DFRWS\Case1-1
	FILE *fIn,*fOut,*fLog;
	if (NULL==(fIn=fopen(pcSourceImage,"rb"))) return -1;

	CString sPath=CString(pcOutDirectory)+"\\log_untar.txt";
	if (NULL==(fLog=fopen(sPath.GetBuffer(),"wt+"))) return -2;

	_fseeki64(fIn,0,SEEK_END);
	__int64 size=_ftelli64(fIn);
	_fseeki64(fIn,0,SEEK_SET);

	BYTE Block[0x200];
	char   *stopstring;

	for (__int64 i=0;i<size;i+=sizeof(Block)){
		fread(Block,sizeof(Block)/sizeof(Block[0]),sizeof(Block[0]),fIn);

		__int64 i64FileSize=_strtoi64((char*)Block+124,&stopstring,8);
		if (i64FileSize==0){
			if (Block[0]==0) continue;
			i64FileSize=0x3bc000000;
		}

		sPath=CString(pcOutDirectory)+"\\"+(char*)(Block+2);
		//sprintf_s(strFile,sizeof(strFile),"C:\\DFRWS\\Case1-1\\%s",(char*)Block+2);

		if (NULL==(fOut=fopen(sPath.GetBuffer(),"wb+"))) return -1;

		__int64 cur_position=_ftelli64(fIn);
		fprintf(fLog,"File entry %s position %016I64X, size %016I64X\n",Block,cur_position,i64FileSize);

		for (__int64 j=0;j<i64FileSize;j+=sizeof(Block),i+=sizeof(Block)){
			fread(Block,sizeof(Block),1,fIn);
			fwrite(Block,min(sizeof(Block),i64FileSize-j),1,fOut);
		}
		fclose(fOut);
	}

	fclose(fLog);
	fclose(fIn);
	return 0;
}