#include <stdafx.h>
#include <list>
#include "windows.h"
#include "assert.h"
#include "afs_parser.h"
#include "./afsblockhlpr.h"

//------------------------------------------------------------------------------------------------------------------
struct _TFILEITEM{
	yaffs_ObjectType ya_type;
	__int64 OffsetInImage;
	long szFile;
	ULONG Parent;
	//SYSTEMTIME modifySysTime;
	ULONG modifyCTime;
	char pcFileName[MAX_PATH];
};
typedef std::list<_TFILEITEM> LSTFILES;

//------------------------------------------------------------------------------------------------------------------
void composehex(BYTE* hex,char* str,int szstr){
	memset(str,0,szstr);
	for (int i=0;i<16;i++){
		sprintf(str+strlen(str),"%02X ",hex[i]);
	}

	strcat(str,"  -  ");

	for (int i=0;i<16;i++){
		if (hex[i]<0x20 || hex[i]>0x80) 
			strcat(str,".");
		else
			sprintf(str+strlen(str),"%c",hex[i]);
	}
}



//------------------------------------------------------------------------------------------------------------------
_TFILENAMETYPE FileExtensions[]={
	{"gif",	ET_GIF},
	{"jpg",	ET_JPG},
	{"jpeg", ET_JPG},
	{"zip", ET_ZIP},
	{"db", ET_SQLITEDB},
	{"db-journal", ET_SQLITEJRNL},
	{"xml", ET_XML},
	{"png", ET_PNG},
	{"ogg", ET_OGG},

};

//------------------------------------------------------------------------------------------------------------------
_TFILETYPE FileTypes[]={
	{{'G','I','F','8','9','a',0,0,0,0,0,0,0,0,0,0},6,0,ET_GIF},
	{{'S','Q','L','i','t','e',' ','f','o','r','m','a','t',' ','3',0},16,0,ET_SQLITEDB},
	{{'\xD9','\xD5','\x05','\xF9','\x20','\xA1','\x63','\xD7','\x00','\x00','\x00','\x0C','\x46','\xFA','\x8C','\x97'},8,0,ET_SQLITEJRNL},
	{{'J','F','I','F',0,0,0,0,0,0,0,0,0,0,0,0},4,6,ET_JPG},
	{{'<','?','x','m','l',0,0,0,0,0,0,0,0,0,0,0},5,0,ET_XML},
	{{'\x89','P','N','G',0,0,0,0,0,0,0,0,0,0,0,0},4,0,ET_PNG},
	{{'O','g','g','S',0,0,0,0,0,0,0,0,0,0,0,0},4,0,ET_OGG},
	
};
//------------------------------------------------------------------------------------------------------------------
_FTYPE getfiletypebyname(char* filename){
	_FTYPE type=ET_NOEXT;
	char* point;
	point=strrchr(filename,'.');
	if (point>0){
		type=ET_NONE;
		++point;
		for (int i=0;i<sizeof(FileTypes)/sizeof(FileTypes[0]);i++){
			if (0==stricmp(point,FileExtensions[i].pcExt)){
				type=FileExtensions[i].type;
				break;
			}
		}
	}
	return type;
}
//------------------------------------------------------------------------------------------------------------------
_FTYPE getfiletypebydata(BYTE* pData, ULONG szData){
	_FTYPE type=ET_NONE;
	for (int i=0;i<sizeof(FileTypes)/sizeof(FileTypes[0]);i++){
		if (0==memcmp(pData+FileTypes[i].ulOffset,FileTypes[i].Mask,FileTypes[i].szRealMask)){
			type=FileTypes[i].type;
			break;
		}
	}
	return type;
}
#define NEW_SIZE_ONLY
//------------------------------------------------------------------------------------------------------------------
int InsertNew(_TFILEITEM* pItem,LSTFILES* plstFiles){
	LSTFILES::iterator icur=(*plstFiles).begin(),iend=(*plstFiles).end();
	while (icur!=iend)
	{
#ifdef NEW_SIZE_ONLY
		if (strcmp(icur->pcFileName,pItem->pcFileName)==0 && pItem->Parent==icur->Parent && pItem->ya_type==icur->ya_type){
			if (pItem->modifyCTime > icur->modifyCTime ){
#else
		if (strcmp(icur->pcFileName,pItem->pcFileName)==0 && pItem->modifyCTime > icur->modifyCTime && pItem->Parent==icur->Parent && pItem->ya_type==icur->ya_type){
#endif
				icur->modifyCTime=pItem->modifyCTime;
				icur->szFile=pItem->szFile;
				icur->OffsetInImage=pItem->OffsetInImage;
				//icur->modifySysTime=pItem->modifySysTime;
#ifdef NEW_SIZE_ONLY
		}
#endif

			break;
		}
		++icur;
	}
	if (icur==iend){
		plstFiles->push_back(*pItem);
	}
	return 0;
}

//------------------------------------------------------------------------------------------------------------------
SYSTEMTIME CTimeToSystemTime(ULONG CTime)
{
	SYSTEMTIME systime;
	FILETIME ft;
	ULARGE_INTEGER ftime;
	ftime.QuadPart = 116444736000000000+(unsigned __int64)(CTime)*10000000;
	ft.dwLowDateTime=ftime.LowPart;
	ft.dwHighDateTime=ftime.HighPart;
	FileTimeToSystemTime(&ft,&systime);

	return systime;
}
//------------------------------------------------------------------------------------------------------------------
char* CTimeToSystemTimeString(char* pBuffer,ULONG CTime)
{
	SYSTEMTIME systime=CTimeToSystemTime(CTime);
	sprintf(pBuffer,"%02d.%02d.%02d %02d:%02d:%02d",systime.wDay,systime.wMonth,systime.wYear,systime.wHour,systime.wMinute,systime.wSecond);
	return pBuffer;
}

//------------------------------------------------------------------------------------------------------------------
int CopyFileBlock(FILE* fIn, __int64 inOffset,__int64 inszData, FILE*fOut, __int64 outOffset=-1){
	BYTE fileBlock[BLOCK_SIZE];

	if (outOffset>=0){
		fseek(fOut,outOffset,SEEK_SET);
	}

	__int64 PrevOffset=_ftelli64(fIn);
	_fseeki64(fIn,inOffset,SEEK_SET);

	__int64 bytesremain=inszData;
	do 
	{
		fread(fileBlock,1,sizeof(fileBlock),fIn);
		fwrite(fileBlock,1,min(sizeof(fileBlock),bytesremain),fOut);
		bytesremain-=sizeof(fileBlock);
	} while(bytesremain>0);

	_fseeki64(fIn,PrevOffset,SEEK_SET);
	return 0;
}

#ifdef PLACE_ATWORK
#define SET_DISK "Z"
#else
#define SET_DISK "C"
#endif

//------------------------------------------------------------------------------------------------------------------
int afs_parser(char* pcImage, char* pcDestFolder,bool flIsCreate){
	CAFSFile asfFile;
	char pcName[YAFFS_MAX_NAME_LENGTH+1];
	char pcWholeDir[YAFFS_MAX_NAME_LENGTH+1];
	char pcSkippedName[YAFFS_MAX_NAME_LENGTH*2+1];
	FILE *fIn,*fOut,*fLog,*fList,*fSBFile;


	LSTFILES lstFiles;

	//sprintf_s(pcName,sizeof(pcName),"%s:/DFRWS/Case1/mtdblock%d.img",SET_DISK,filenum);
	sprintf_s(pcName,sizeof(pcName),"%s",pcImage);
	if (NULL==(fIn=fopen(pcName,"rb"))) return -1;

	asfFile.Init("mmssms.db",fIn);

	//ULONG ulFileStart;

	//sprintf_s(pcName,sizeof(pcName),"%s:/DFRWS/LOG/afs_list%d.log",SET_DISK,filenum);
	sprintf_s(pcName,sizeof(pcName),"%s/afs_list.log",pcDestFolder);
	if (NULL==(fLog=fopen(pcName,"wt+"))) return -2;


	sprintf_s(pcSkippedName,sizeof(pcSkippedName),"%s/SkippedBlocks.dat",pcDestFolder);
	if (NULL==(fSBFile=fopen(pcSkippedName,"wb+"))) {
		return -3;
	}

	//sprintf_s(pcName,sizeof(pcName),"%s:/DFRWS/LOG/yaffs%d.csv",SET_DISK,filenum);
	sprintf_s(pcName,sizeof(pcName),"%s/yaffs.csv",pcDestFolder);
	if (NULL==(fList=fopen(pcName,"wt+"))) return -4;

#ifdef USE_OLDMMS
	FILE *fSMS;
	//sprintf_s(pcName,sizeof(pcName),"%s:/DFRWS/LOG/mmssms.db",SET_DISK);
	sprintf_s(pcName,sizeof(pcName),"%s/mmssms.db",pcDestFolder);
	if (NULL==(fSMS=fopen(pcName,"wt+"))) return -2;
#endif
	_fseeki64(fIn,0,SEEK_END);
	__int64 size=_ftelli64(fIn);
	_fseeki64(fIn,0,SEEK_SET);

	BYTE hdrBlock[BLOCK_SIZE];
	BYTE fileBlock[BLOCK_SIZE];

	__int64 szDataBlock=0;// размер предыдущего блока данных
	__int64 dataPosition=0;// позиция предыдущего блока данных

	BYTE Pilot[0x10];
	char strPilot[sizeof(Pilot)*6+1+5];
	
	// даныне предыдущего заголовка
	//long szFilePrev=0;// размер файла по заголовку
	__int64 seekFilePrev=0;// позиция заголовка
	yaffs_ObjectHeader preHeader;
	//char pcNameFilePrev[YAFFS_MAX_NAME_LENGTH+1]={0};
	//yaffs_ObjectType tTypeFilePrev=YAFFS_OBJECT_TYPE_UNKNOWN;
	_FTYPE tFileTypePrev=ET_NONE;

	long szFile=0;
	yaffs_ObjectHeader* pHdr=(yaffs_ObjectHeader*)hdrBlock;
	memset(&preHeader,0,sizeof(preHeader));
	
	for (__int64 i=0;i<size;i+=sizeof(hdrBlock)){
		__int64 st=_ftelli64(fIn);
		bool flMustSkipped=false;
		char* FileName;

		fread(hdrBlock,sizeof(hdrBlock)/sizeof(hdrBlock[0]),sizeof(hdrBlock[0]),fIn);

#define TEST_MASK "ca"
		
		FileName=pHdr->name;//(char*)hdrBlock+0x0A;

		// пропускаем если есть непечатные символы
		for (int k=0;k<256;k++){
			if (FileName[k]==0) break;
			if (FileName[k]<0x20 || FileName[k]>0x7E) {
				flMustSkipped=true;
				break;
			}
		}

		// пропускаем если нет имени или размер = -1
		// пропускаем размер = 0
		//|| szFile==0xFFFFFFFF szFile==0 || 
		if ( flMustSkipped || /*(*(WORD*)(hdrBlock+0x8)*/ pHdr->sum__NoLongerUsed !=0xFFFF) {
			if (szDataBlock==0) {
				dataPosition=st;
				memcpy(Pilot,hdrBlock,sizeof(Pilot));
			}
			szDataBlock+=sizeof(hdrBlock);
			if (flIsCreate) fprintf(fLog,"-----------------------------skipped block\n");
			
			continue;
		}

		//szFilePrev=szFile;
		szFile=*(ULONG*)(hdrBlock+0x124);


		//---------------------------------- Обрабатываем список файлов -------------------------------------
		char strMTime[6*3+1+1];

		if (szDataBlock)
			fprintf(fList,"0x%08I64x;%d;0x%08x;%s;0x%08x;0x%x;0x%x;0x%x;\n",
			dataPosition,0,0,"'----------  DATA",szDataBlock,0,0,0);

		if (strcmp(pHdr->name,"cc_data")) // пропускаем - их слишком много
			fprintf(fList,"0x%08I64x;%d;0x%08x;%s;0x%08x;0x%x;0x%x;0x%x;%s ; %d; %d; %d; %d;\n",
			st,pHdr->type,pHdr->parentObjectId,pHdr->name,pHdr->fileSize,
			pHdr->yst_ctime,pHdr->yst_atime,pHdr->yst_mtime,
			CTimeToSystemTimeString(strMTime,pHdr->yst_mtime),
			pHdr->inbandShadowsObject,
			pHdr->inbandIsShrink,
			pHdr->shadowsObject,
			pHdr->isShrink
			);
		
		//------------------------------------ Хитрый парсер блоков ---------------------------------------
/*
		if (szDataBlock){
			if (asfFile.IsNeedProcessing(pHdr->name))
				asfFile.Insert(pHdr,dataPosition,szDataBlock);
			else{
				if (asfFile.IsNeedProcessing(preHeader.name))
					asfFile.Insert(&preHeader,dataPosition,szDataBlock);
			}
		}
*/		
		//------------------------------------ Хитрый парсер блоков ---------------------------------------

		if (szDataBlock){
			if (asfFile.IsNeedProcessing(pHdr->name) && pHdr->isShrink==0){
				// в блоке 6 off:3640000 сначала идет 800h от предыдущего Hdr, а потом остаток 1000h от db !!!!
				if (szDataBlock>pHdr->fileSize && pHdr->fileSize>0){
					if (szDataBlock-pHdr->fileSize>=BLOCK_SIZE){
						int illegaldata=int((szDataBlock-pHdr->fileSize)/ BLOCK_SIZE) * BLOCK_SIZE;

						// !!!!!!!!!!!!!! вставить сохранение
						CopyFileBlock(fIn,dataPosition,illegaldata,fSBFile);
											
						// потом пропустить
						dataPosition+=illegaldata;
						szDataBlock-=illegaldata;
					}
				}

				if (pHdr->type == YAFFS_OBJECT_TYPE_FILE)
					asfFile.Insert(pHdr,dataPosition,szDataBlock);
			}
			else{
				if (asfFile.IsNeedProcessing(preHeader.name) && preHeader.isShrink!=0)
					if (pHdr->type == YAFFS_OBJECT_TYPE_FILE)
						asfFile.Insert(&preHeader,dataPosition,szDataBlock);
			}
		}
#ifdef USE_OLDMMS
		//------------------------------ Пользовательская группировка файлов --------------------------------
		// СОбираем куски рядом mmssms.db только чтобы вытащить строки
		if (0==strcmp(pHdr->name,"mmssms.db") && pHdr->fileSize>0 && szDataBlock>0){
			CopyFileBlock(fIn,dataPosition,szDataBlock,fSMS);
		}
		if (0==strcmp(preHeader.name,"mmssms.db") && pHdr->fileSize>0 && szDataBlock>0){
			CopyFileBlock(fIn,dataPosition,szDataBlock,fSMS);
		}
#endif
		//---------------------------------------------------------------------------------------------------
		if (0==memcmp(hdrBlock+BLOCK_SIZE-(sizeof(TEST_MASK)-1),TEST_MASK,sizeof(TEST_MASK)-1)){
			FileName=NULL;
		}

		//---------------------------------------------------------------------------------------------------
		composehex(Pilot,strPilot,sizeof(strPilot));

#define SKIP_ZERO_AND_MINUS1
		if (szDataBlock) {
			char* strSizingNext="";
			char* strSizingPrev="";
			
			_FTYPE tFileType=getfiletypebyname(FileName);
			_FTYPE tFileTypeByData=getfiletypebydata(Pilot,sizeof(Pilot));

			_TFILEITEM fItem;
			fItem.modifyCTime=pHdr->yst_mtime;
			fItem.Parent=pHdr->parentObjectId;

			if (szDataBlock>=szFile && szFile>0 && szDataBlock-szFile<BLOCK_SIZE && (tFileType==tFileTypeByData || tFileType==ET_NOEXT)) {
				strSizingNext="++ Next file acquired";
				fItem.OffsetInImage=dataPosition;
				strcpy(fItem.pcFileName,FileName);
				fItem.ya_type=pHdr->type;
				fItem.szFile=szFile;

				InsertNew(&fItem,&lstFiles);
				//lstFiles.push_back(fItem);

			}
			if (szDataBlock>=preHeader.fileSize && preHeader.fileSize>0 && 
				szDataBlock-preHeader.fileSize<BLOCK_SIZE && (preHeader.type==tFileTypeByData || tFileType==ET_NOEXT)){
				strSizingPrev="++ Prev file acquired";

				fItem.OffsetInImage=dataPosition;
				strcpy(fItem.pcFileName,preHeader.name);
				fItem.ya_type=preHeader.type;
				fItem.szFile=preHeader.fileSize;
				InsertNew(&fItem,&lstFiles);
				//lstFiles.push_back(fItem);
			}
			
			fprintf(fLog,"%Offset: %I64x  \n%s\nSize: %I64x  %s  %s\n----------------------------------------------------\n",
				dataPosition,strPilot,szDataBlock, strSizingPrev,strSizingNext);
		}

#ifdef SKIP_ZERO_AND_MINUS1
		if (szFile>0)
#endif
		{

			fprintf(fLog,"File: \"%30s\" Size:%08x - (%d) - Start:%I64x  \n",FileName,szFile,szFile,st);
			fprintf(fLog,"data 0000:%08x %08x 0108:%08x %08x 0110:%08x %08x 01CC:%08x 01E8:%08x 01F8:%08x %08x\n----------------------------------------------------\n",
				*(ULONG*)(hdrBlock+0x0000),
				*(ULONG*)(hdrBlock+0x0004),
				*(ULONG*)(hdrBlock+0x0108),
				*(ULONG*)(hdrBlock+0x010C),
				*(ULONG*)(hdrBlock+0x0110),
				*(ULONG*)(hdrBlock+0x0114),
				*(ULONG*)(hdrBlock+0x01CC),
				*(ULONG*)(hdrBlock+0x01E8),
				*(ULONG*)(hdrBlock+0x01F8),
				*(ULONG*)(hdrBlock+0x01FC)
				);
			seekFilePrev=st;
			//tTypeFilePrev=pHdr->type;
			tFileTypePrev=getfiletypebyname(FileName);
			memcpy(&preHeader,pHdr,sizeof(preHeader));
			//strcpy(pcNameFilePrev,FileName);
		}

		szDataBlock=0;

	}
	fclose(fLog);
#ifdef USE_OLDMMS
	sprintf_s(pcName,sizeof(pcName),"%s/sms",pcDestFolder);
	CreateDirectory(pcName,NULL);

	sprintf_s(pcName,sizeof(pcName),"%s/sms/mmssms_new.db",pcDestFolder);
	asfFile.MakeMMSSMS(pcName);
	//asfFile.Save2(pcName);
#endif

	sprintf_s(pcName,sizeof(pcName),"%s/LST",pcDestFolder);
	CreateDirectory(pcName,NULL);

	sprintf_s(pcWholeDir,sizeof(pcWholeDir),"%s/!WFD",pcDestFolder);
	CreateDirectory(pcWholeDir,NULL);
	
	asfFile.SaveAll(pcName);
/*
	FILE* fProcess;
	LSTFILES::iterator icur=lstFiles.begin(),ilast=lstFiles.end();

	while(icur!=ilast){
		int ncnt=0;
		char* tmpfn=icur->pcFileName;

		if (flIsCreate){

			sprintf_s(pcName,sizeof(pcName),"%s:\\DFRWS\\LOG\\Files%d\\%s",SET_DISK,filenum,tmpfn);
			while(NULL!=(fProcess=fopen(pcName,"rb"))){
				ncnt++;
				sprintf_s(pcName,sizeof(pcName),"%s:\\DFRWS\\LOG\\Files%d\\%s(%d)",SET_DISK,filenum,tmpfn,ncnt);
				fclose(fProcess);
			}

			if (NULL!=(fProcess=fopen(pcName,"wb+"))){
				CopyFileBlock(fIn,icur->OffsetInImage,icur->szFile,fProcess);
				fclose(fProcess);

			}
			else{
				//assert(false);
				ULONG err=GetLastError();
			}

		}

		++icur;
	}*/

	fclose(fList);

#ifdef USE_OLDMMS
	fclose(	fSMS);
#endif

	fclose(fIn);
	fclose(fSBFile);

	return 0;
}