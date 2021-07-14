#include <stdafx.h>
#include <assert.h>
#include "windows.h"

#include "./afs_parser.h"
#include "./afsblockhlpr.h"

int CopyFileBlock(FILE* fIn, __int64 inOffset,__int64 inszData, FILE*fOut, __int64 outOffset=-1);
char* CTimeToSystemTimeString(char* pBuffer,ULONG CTime);
//---------------------------------------------------------------------------------------------------
CAFSFile::CAFSFile():pcAFSTemplateName(NULL){

}

//---------------------------------------------------------------------------------------------------
CAFSFile::~CAFSFile(){
	Reset();
}
//---------------------------------------------------------------------------------------------------
int CAFSFile::Init(char* pcFileName,FILE* fIn){
	int res=0;
	pcAFSTemplateName=(char*)malloc(strlen(pcFileName)+1);
	if (pcAFSTemplateName!=NULL)
		strcpy(pcAFSTemplateName,pcFileName);
	fBlockFile=fIn;
	return res;
}

//---------------------------------------------------------------------------------------------------
bool CAFSFile::IsNeedProcessing(char* pcFileName){
	return 0!=stricmp(pcFileName,"cc_data");
}

//---------------------------------------------------------------------------------------------------
int CAFSFile::Reset(){
	if (pcAFSTemplateName) free(pcAFSTemplateName);
	pcAFSTemplateName=NULL;
	tAfsFileSet.clear();

	return 0;
}

//---------------------------------------------------------------------------------------------------
// Добавить файл к группе
//---------------------------------------------------------------------------------------------------
int CAFSFile::Insert(yaffs_ObjectHeader* pObjectHdr,__int64 i64OffsetBlock, __int64 i64SizeBlock){
	int res=0;

	TAFSFILE AfsFile;
	AfsFile.parentObjectId=pObjectHdr->parentObjectId;
	AfsFile.i64MaxBlockSize=0;
	
	strcpy(AfsFile.pcFileName,pObjectHdr->name);
	
	std::pair< TAFSFILESET::iterator, bool > prf;
	prf=tAfsFileSet.insert(AfsFile);
	
	if (prf.second == true){
		res=1;
	}
		TAFSBLOCK block;
		block.i64Offset=i64OffsetBlock;
		block.inbandIsShrink=pObjectHdr->inbandIsShrink;
		block.isShrink=pObjectHdr->isShrink;
		block.szBlock=i64SizeBlock;
		block.yst_mtime=pObjectHdr->yst_mtime;
		block.yst_atime=pObjectHdr->yst_atime;
		block.yst_ctime=pObjectHdr->yst_ctime;
		block.szFile=pObjectHdr->fileSize;

		strcpy(block.pcFileName,pObjectHdr->name);

		std::pair< TAFS::iterator, bool > pr;
		pr=prf.first->afsBlocks.insert(block);
		if (pr.second == true){
			if (prf.first->i64MaxBlockSize<i64SizeBlock) prf.first->i64MaxBlockSize=i64SizeBlock;
		}
	//}
	return res;
}

//---------------------------------------------------------------------------------------------------
char* getfilename(char* pcname){
	char* p1=strrchr(pcname,'\\');
	if (p1) return p1+1;
	p1=strrchr(pcname,'/');
	if (p1) return p1+1;
	return pcname;
}
//---------------------------------------------------------------------------------------------------
int setfiletime(char* pcname,ULONG yst_ctime,ULONG yst_atime,ULONG yst_mtime){
	int res=0;
	FILETIME cft,aft,mft;
	ULARGE_INTEGER ftime;

	HANDLE hFile=CreateFile(pcname,
		FILE_WRITE_ATTRIBUTES,
		NULL,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS,
		NULL);


	if (INVALID_HANDLE_VALUE==hFile) return -1;	

	ftime.QuadPart = 116444736000000000+(unsigned __int64)(yst_ctime)*10000000;
	cft.dwLowDateTime=ftime.LowPart;cft.dwHighDateTime=ftime.HighPart;

	ftime.QuadPart = 116444736000000000+(unsigned __int64)(yst_atime)*10000000;
	aft.dwLowDateTime=ftime.LowPart;aft.dwHighDateTime=ftime.HighPart;

	ftime.QuadPart = 116444736000000000+(unsigned __int64)(yst_mtime)*10000000;
	mft.dwLowDateTime=ftime.LowPart;mft.dwHighDateTime=ftime.HighPart;


	SetFileTime(hFile,&cft,&aft,&mft);

	CloseHandle(hFile);

	return res;
}
//---------------------------------------------------------------------------------------------------
// Сохранить все куски файла в хронологической последовательности
//---------------------------------------------------------------------------------------------------
#define WFSEPARATOR "-------------- END OF WHOLE BLOCK %08X --------------"
int cuBlockNumber;
int CAFSFile::Save2(char* pcFileName, TAFS& tAFS, char* pcWholeFileDir){
	int res=0;
	FILE* fData, *fLog, *fWholeFile;
	TAFS::iterator icur=tAFS.begin(),iend=tAFS.end();

	char* pcDataFileName=(char*)malloc(strlen(pcFileName)+1+20);

	char* pcDataFileLog=(char*)malloc(strlen(pcFileName)+1+20);
	sprintf(pcDataFileLog,"%s.log",pcFileName);

	if (NULL==(fLog=fopen(pcDataFileLog,"wt+"))) {
		free(pcDataFileName);
		free(pcDataFileLog);
		return -3;
	}

	sprintf(pcDataFileLog,"%s.(full)",pcFileName);
	if (NULL==(fWholeFile=fopen(pcDataFileLog,"wb+"))){
		free(pcDataFileName);
		free(pcDataFileLog);
		fclose(fLog);
		return -3;
	}

	int cnt=0;
	char strMTime[6*3+1+1];

	while (icur!=iend){
		if (cnt)
			sprintf(pcDataFileName,"%s.(%d)",pcFileName,cnt);
		else{
			sprintf(pcDataFileName,"%s",pcFileName);
		}

		if (NULL!=(fData=fopen(pcDataFileName,"wb+"))) {
			// Если для даного файла 1 блок и он больше файла - то это целый файл
			if (icur->szBlock>=icur->szFile){
				CopyFileBlock(fBlockFile,icur->i64Offset,icur->szFile,fWholeFile);
				CopyFileBlock(fBlockFile,icur->i64Offset,icur->szFile,fData,0);

				// тут тема сохранить целый файл
				if (pcWholeFileDir){
					FILE* fRealFileFile;
					int wcnt=1;
					char* pcNewFile=(char*)malloc(strlen(pcFileName)+MAX_PATH);

					sprintf(pcNewFile,"%s\\%s",pcWholeFileDir,getfilename(pcFileName));
					while (NULL!=(fRealFileFile=fopen(pcNewFile,"r"))){
						sprintf(pcNewFile,"%s\\%s.(%d)",pcWholeFileDir,getfilename(pcFileName),wcnt++);
						fclose(fRealFileFile);
					}

					if(NULL!=(fRealFileFile=fopen(pcNewFile,"wb+"))){
						CopyFileBlock(fBlockFile,icur->i64Offset,icur->szFile,fRealFileFile);
						fclose(fRealFileFile);
						setfiletime(pcNewFile,icur->yst_ctime,icur->yst_atime,icur->yst_mtime);
					}
					free(pcNewFile);
				}
			}
			else{
				CopyFileBlock(fBlockFile,icur->i64Offset,icur->szBlock,fWholeFile);
				CopyFileBlock(fBlockFile,icur->i64Offset,icur->szBlock,fData,0);
			}
#ifndef DONT_USE_WHOLEFILESEPARATOR
			fprintf(fWholeFile,WFSEPARATOR,cuBlockNumber++);
#endif
			fclose(fData);

			setfiletime(pcDataFileName,icur->yst_ctime,icur->yst_atime,icur->yst_mtime);

			fprintf(fLog,"Offset: %08I64x; Block size %08x(%d); File size: %08x(%d); isShrink %d; Time %s;\n",
				icur->i64Offset,
				icur->szBlock,
				icur->szFile,
				icur->isShrink,
				CTimeToSystemTimeString(strMTime,icur->yst_mtime));

			cnt++;
		}

		icur++;
	}

	fclose(fWholeFile);
	fclose(fLog);
	free(pcDataFileLog);
	free(pcDataFileName);

	return res;
}

//---------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------
int CAFSFile::Process(){
	int res=0;
	TAFSFILESET::iterator icur=tAfsFileSet.begin(),iend=tAfsFileSet.end();
	while (icur!=iend)
	{
		TAFS::reverse_iterator icur_rev=icur->afsBlocks.rbegin(),icur_end=icur->afsBlocks.rend();
		if (icur_rev->szFile <= icur_rev->szBlock)
			if (icur_rev->szBlock-icur_rev->szFile < BLOCK_SIZE){
				// Этот блок соответствует размеру файла
			}

		/*while (icur_rev!=icur_end)
		{
			icur_rev++;
		}*/
		icur++;
	}

	return res;
}
//---------------------------------------------------------------------------------------------------
int CAFSFile::MakeMMSSMS(char* pcFileDir){
	int res=0;
	TAFSFILE tmpAfs;
	strcpy(tmpAfs.pcFileName,"mmssms.db");
	tmpAfs.parentObjectId=0x000001f3;

	TAFSFILESET::iterator icur=tAfsFileSet.find(tmpAfs);

	//Save(pcFileDir,icur->afsBlocks,icur->i64MaxBlockSize);

	return res;
}
//---------------------------------------------------------------------------------------------------
int CAFSFile::SaveAll(char* pcFileDir){
	int res=0;
	TAFSFILESET::iterator icur=tAfsFileSet.begin(),iend=tAfsFileSet.end();
	
	FILE* fLog;
	char pcLogFileName[MAX_PATH];
	sprintf(pcLogFileName,"%s\\SaveAll.log",pcFileDir);
	if (NULL==(fLog=fopen(pcLogFileName,"wt+"))) return -2;

	char* pcNewDirnFile=(char*)malloc(strlen(pcFileDir)+1+256+1+256);
	char* pcWholeFileDir=(char*)malloc(strlen(pcFileDir)+1+256+1+256);

	while (icur!=iend)
	{
		cuBlockNumber=0;
		if (icur->pcFileName[0]!='\0'){
			sprintf(pcNewDirnFile,"%s\\%s",pcFileDir,icur->pcFileName);
			int cnt=0;
			if (!CreateDirectory(pcNewDirnFile,NULL)){
				DWORD rle=GetLastError();
				assert(rle!=ERROR_PATH_NOT_FOUND);
				if (ERROR_ALREADY_EXISTS==rle){
					do 
					{
						sprintf(pcNewDirnFile,"%s\\%s(%d)",pcFileDir,icur->pcFileName,cnt++);
					} while(!CreateDirectory(pcNewDirnFile,NULL));
				}
			}
			sprintf(pcNewDirnFile+strlen(pcNewDirnFile),"\\%s",icur->pcFileName);
			strcpy(pcWholeFileDir,pcFileDir);
			strcpy(getfilename(pcWholeFileDir),"!WFD");

			Save2(pcNewDirnFile,icur->afsBlocks,pcWholeFileDir);

			if (cnt)
				fprintf(fLog,"File: %s(%d) , BlockCount:%d\n",icur->pcFileName,cnt,icur->afsBlocks.size());
			else
				fprintf(fLog,"File: %s , BlockCount:%d\n",icur->pcFileName,icur->afsBlocks.size());
		}


		icur++;
	}
	//free(pcSkippedFBlockFile);
	free(pcNewDirnFile);
	free(pcWholeFileDir);
	//fclose(fSBFile);
	fclose(fLog);
	return res;
}

//---------------------------------------------------------------------------------------------------
int CAFSFile::Save(char* pcFileName, TAFS& tAFS, __int64 i64MaxBlockSize){
	int res=0;
	FILE* fData,*fLog;

	// ------------------  detect NoSpare  ------------------
	TAFS::iterator icur=tAFS.begin(),iend=tAFS.end();
	while (icur!=iend)
	{
		if (icur->isShrink!=0) break;
		icur++;
	}

	// ------------------
	// произошло обрезание файла, теперь не знаю что делать
	if (icur!=iend) return -1;

	// ------------------
	// созадаём файл !!! b kju
	if (NULL==(fData=fopen(pcFileName,"wb+"))) return -2;

	char* pcLogFileName=(char*)malloc(strlen(pcFileName)+1+7);
	if (!pcLogFileName) return -10;
	sprintf(pcLogFileName,"%s.log",pcFileName);
	if (NULL==(fLog =fopen(pcLogFileName,"wt+"))) return -3;
	free(pcLogFileName);

	BYTE Data=0;
	//i64MaxBlockSize=0xc400;
	for (__int64 i=0;i<i64MaxBlockSize;i++){
		fwrite(&Data,1,1,fData);
	}

	// ------------------
	// Сначала запишем прямо заголовок
	icur=tAFS.begin();
	CopyFileBlock(fBlockFile,icur->i64Offset,icur->szBlock,fData,0);

	// ------------------
	icur++;
	BYTE destFileBlock[BLOCK_SIZE];
	while (icur!=iend){
		// смотрим куда бы блоки из данного файла лучше всего бы подошли
		BYTE* pBlockData=(BYTE*)malloc(icur->szBlock);// Это новые данные
		if (!pBlockData) return -3;
		_fseeki64(fBlockFile,icur->i64Offset,SEEK_SET);
		fread(pBlockData,icur->szBlock,1,fBlockFile);

		fprintf(fLog,"--------------------------------------\n\
			Анализируем блок Size:%I64x RealOff:%I64x\
			\n--------------------------------------\n", 
			icur->szBlock,icur->i64Offset);
		
		std::set<int> SavedBlocks;
		__int64 PlaceToSave=0;
		for (__int64 i=0;i<icur->szBlock;i+=BLOCK_SIZE){
			long maxcntDiffAll=0x7FFFFFFF;
			long maxcntDiffNonZero=0x7FFFFFFF;
			__int64 maxcntOffset=0;

			// каждый следующий блок должен идти после предыдущего в блоке : laceToSave*BLOCK_SIZE
			// кроме того для оставшихся должно остаться место : i64MaxBlockSize-(icur->szBlock-i)
			for (__int64 j=PlaceToSave;j<i64MaxBlockSize-(icur->szBlock-i-BLOCK_SIZE);j+=BLOCK_SIZE){
				_fseeki64(fData,j,SEEK_SET);
				fread(destFileBlock,BLOCK_SIZE,1,fData);// Это данные, которые уже запистали

				long cntDiffAll=0;
				long cntDiffNonZero=0;
				for (long k=0;k<BLOCK_SIZE;k++){
					if (destFileBlock[k]!=*(pBlockData+i+k)) cntDiffAll++;
					if (destFileBlock[k]!=*(pBlockData+i+k) && destFileBlock[k]!=0) cntDiffNonZero++;
				}

				// тут выбираем самое подходящее место
				fprintf(fLog,"Offset:%I64d -> cntDiffAll=%d,cntDiffNonZero=%d\n",j,cntDiffAll,cntDiffNonZero);
				if (cntDiffAll<maxcntDiffAll && cntDiffAll>0){
					std::set<int>::iterator idat=SavedBlocks.find(j/BLOCK_SIZE);
					if (idat==SavedBlocks.end()){
						maxcntDiffAll=cntDiffAll;
						maxcntOffset=j;
					}
				}
			}

			//теперь в самый подходящий копируем
			PlaceToSave=maxcntOffset+BLOCK_SIZE;
			fprintf(fLog,"maxcntOffset=%I64d,maxcntDiffAll=%d,maxcntDiffNonZero=%d\n\n",
				maxcntOffset,maxcntDiffAll,maxcntDiffNonZero);

			_fseeki64(fBlockFile,maxcntOffset,SEEK_SET);
			SavedBlocks.insert(maxcntOffset/BLOCK_SIZE);

			fprintf(fLog,"So write to Offset=%I64d\n\n",
				maxcntOffset);

			fwrite(pBlockData+i,BLOCK_SIZE,1,fData);
		}

		if (pBlockData) free(pBlockData);

		icur++;
	}

	fclose(fLog);
	fclose(fData);
	return res;
}
//---------------------------------------------------------------------------------------------------
