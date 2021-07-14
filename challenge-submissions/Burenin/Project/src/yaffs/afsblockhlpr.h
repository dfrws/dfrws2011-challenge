
#include <set>

//------------------------------------------------------------------------------------------------------
typedef struct _TAFSBLOCK{
	ULONG yst_mtime;// modify
	ULONG yst_ctime;
	ULONG yst_atime;

	__int64 i64Offset;
	__int64 szBlock;

	__int64 szFile;

	char pcFileName[256];
	ULONG inbandIsShrink;
	ULONG isShrink;

	bool operator<(const _TAFSBLOCK& newBlock)const{
		return yst_mtime<newBlock.yst_mtime;
	}

	bool operator>(const _TAFSBLOCK& newBlock)const{
		return yst_mtime>newBlock.yst_mtime;
	}

	bool operator==(const _TAFSBLOCK& newBlock)const{
		return yst_mtime==newBlock.yst_mtime && inbandIsShrink==newBlock.inbandIsShrink &&
			i64Offset==newBlock.i64Offset && szBlock==newBlock.szBlock;
	}
}TAFSBLOCK;

//------------------------------------------------------------------------------------------------------

typedef std::set<TAFSBLOCK> TAFS;

//------------------------------------------------------------------------------------------------------
typedef struct _TAFSFILE{
	char pcFileName[256];
	int parentObjectId;
	TAFS afsBlocks;
	__int64 i64MaxBlockSize;
	
	bool operator<(const _TAFSFILE& newFile)const{
		char curFileStr[256+8*2+1+1];
		char newFileStr[256+8*2+1+1];
		sprintf(curFileStr,"%s_%08x",pcFileName,parentObjectId);
		sprintf(newFileStr,"%s_%08x",newFile.pcFileName,newFile.parentObjectId);

		return 0<stricmp(newFileStr,curFileStr);
	}
	/*
	bool operator==(char* pcNewFileName){
		return 0==stricmp(pcNewFileName,pcFileName) ;
	}
	bool operator==(const char* pcNewFileName){
		return 0==stricmp(pcNewFileName,pcFileName) ;
	}
	bool operator==(const _TAFSFILE& newFile) const{
		return 0==stricmp(newFile.pcFileName,pcFileName) && (newFile.parentObjectId==parentObjectId || newFile.parentObjectId==0);
	}*/
}TAFSFILE;

//------------------------------------------------------------------------------------------------------

typedef std::set<TAFSFILE> TAFSFILESET;

//------------------------------------------------------------------------------------------------------
class CAFSFile
{
	//TAFS tAFS;
	TAFSFILESET tAfsFileSet;

	char* pcAFSTemplateName;
	FILE* fBlockFile;

public:
	CAFSFile();
	~CAFSFile();
	int Reset();
	int Init(char* pcFileName,FILE* fIn);
	bool IsNeedProcessing(char* pcFileName);
	int Insert(yaffs_ObjectHeader* pObjectHdr,__int64 i64OffsetBlock, __int64 i64SizeBlock);
	int Save(char* pcFileName, TAFS& tAFS, __int64 i64MaxBlockSize);
	int Save2(char* pcFileName, TAFS& tAFS, char* pcWholeFileDir);
	int Process();
	int SaveAll(char* pcFileDir);
	int MakeMMSSMS(char* pcFileDir);
protected:
private:
};
//------------------------------------------------------------------------------------------------------
