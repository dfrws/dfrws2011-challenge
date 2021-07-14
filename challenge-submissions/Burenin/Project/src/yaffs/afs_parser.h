/*
+0x0A  -> FileName
+0x124 -> FileSize

0x800 -> BlockSize

*/
#pragma once
#include "settings.h"

#define YAFFS_MAX_NAME_LENGTH       255
#define YAFFS_MAX_ALIAS_LENGTH      159

typedef enum {
	YAFFS_OBJECT_TYPE_UNKNOWN,
	YAFFS_OBJECT_TYPE_FILE,
	YAFFS_OBJECT_TYPE_SYMLINK,
	YAFFS_OBJECT_TYPE_DIRECTORY,
	YAFFS_OBJECT_TYPE_HARDLINK,
	YAFFS_OBJECT_TYPE_SPECIAL
} yaffs_ObjectType;

#define BLOCK_SIZE 0x800

typedef enum _FTYPE{
	ET_NONE=0,
	ET_SQLITEDB,
	ET_SQLITEJRNL,
	ET_ZIP,
	ET_JPG,
	ET_GIF,
	ET_XML,
	ET_PNG,ET_OGG,
	ET_NOEXT
};

struct _TFILETYPE{
	BYTE	Mask[16];
	ULONG	szRealMask;
	ULONG	ulOffset;
	_FTYPE  type;
};

struct _TFILENAMETYPE{
	char*	pcExt;
	_FTYPE	type;
};

typedef struct {
	yaffs_ObjectType type;
	int parentObjectId;
	WORD sum__NoLongerUsed;
	char name[YAFFS_MAX_NAME_LENGTH + 1];
	ULONG yst_mode; 

	ULONG yst_uid;
	ULONG yst_gid;

	ULONG yst_atime;//access
	ULONG yst_mtime;// modify
	ULONG yst_ctime;//create

	int fileSize;
	int equivalentObjectId;
	char alias[YAFFS_MAX_ALIAS_LENGTH + 1];
	ULONG yst_rdev; 
	ULONG Unk[6];
	ULONG inbandShadowsObject;
	ULONG inbandIsShrink;
	ULONG reservedSpace[2];
	int shadowsObject; 
	ULONG isShrink;

} yaffs_ObjectHeader;


//------------------------------------------------------------------------------------------------------------------
int afs_parser(char* pcImage, char* pcDestFolder, bool flIsCreate);
//int afs_parser(int filenum, bool flIsCreate);