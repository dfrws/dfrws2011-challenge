
#define JOURNALHDR_MAGIC "\xd9\xd5\x05\xf9\x20\xa1\x63\xd7"

#pragma pack(push,1)

struct _TJOURNAL_HDR{
	BYTE  jhMagic[8];
	ULONG jhRecordCount;			//is set to the number of journal records that follow this journal header in the journal file
	ULONG jhChecksumInitializer;	//field is set to a pseudo-random value. It is used as part of the algorithm to calculate the checksum for all journal records that follow this journal header
	ULONG jhDatabasePageCount;		//is set to the number of pages that the database file contained before any modifications associated with write transaction are applied
	ULONG jhSectorSize;				//is set to the sector size of the device on which the journal file was created, in bytes. This value is required when reading the journal file to determine the size of each journal header
	ULONG jhPageSize;				//field contains the database page size used by the corresponding database file when the journal file was created, in bytes
};

struct _TJOURNAL_RCRDHDR{
	ULONG jrhPageNumber;			//The page number of the database page associated with this journal record, stored as a 4 byte big-endian unsigned integer
	BYTE jrhOriginalData[1];		//This field contains the original data for the page, exactly as it appeared in the database file before the write transaction began
	//ULONG jrhChecksumValue; - offset = (4 + page-size) //This field contains a checksum value, calculated based on the contents of the journaled database page data (the previous field) and the values stored in the checksum initializer field of the preceding journal header.
};

//Master Journal Pointer
struct _TJOURNAL_MSTRHDR1{
	ULONG	jmhLockingPageNumber;
	BYTE	jmhMasterJournalName[1];
};

struct _TJOURNAL_MSTRHDR2{
	ULONG	jmhNameLength;
	ULONG	jmhChecksum;
	BYTE	jhMagic[8];
};

#pragma pack(pop)

int parse_sqlimg(char* pcFileName);