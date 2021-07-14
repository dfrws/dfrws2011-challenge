#include <stdafx.h>
#include "windows.h"
#include "assert.h"
#include "sqlite_parser.h"

//---------------------------------------------------------------------------------------------------------
bool IsJournalHdr(BYTE* pData){
	return (0==memcmp(pData,JOURNALHDR_MAGIC,8));
}

//---------------------------------------------------------------------------------------------------------
int parse_journal(FILE* fIn,IN OUT __int64* pposition){
	_TJOURNAL_HDR HdrJournal;

	_fseeki64(fIn,*pposition,SEEK_SET);
	fread(&HdrJournal,sizeof(HdrJournal),1,fIn);

	if (!IsJournalHdr((BYTE*)&HdrJournal)) return -1;

	__int64 pNextHdrOffset=HdrJournal.jhSectorSize+HdrJournal.jhRecordCount*HdrJournal.jhPageSize+8;

	return 0;
}

//---------------------------------------------------------------------------------------------------------
int parse_sqlimg(char* pcFileName){
	FILE *fIn;
	if (NULL==(fIn=fopen(pcFileName,"rb"))) return -1;

	_fseeki64(fIn,0,SEEK_END);
	__int64 size=_ftelli64(fIn);
	_fseeki64(fIn,0,SEEK_SET);

	BYTE Block[0x200];

	for (__int64 i=0;i<size;i+=sizeof(Block)){
		fread(Block,sizeof(Block)/sizeof(Block[0]),sizeof(Block[0]),fIn);
		if (IsJournalHdr(Block)){
			parse_journal(fIn,&i);
		}
	}

	return 0;
}

//---------------------------------------------------------------------------------------------------------