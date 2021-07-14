/*
License: DFRC@KU

Copyright (c) 2011, Jewan Bang (jwbang@korea.ac.kr),
DFRC(Digital Forensic Research Center). All rights reserved.

Refer to AUTHORS for acknowledgements.

Redistribution and use in source and binary forms, with or without modification, 
are permitted for any non-commercial purpose provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
- All advertising materials mentioning features or use of this software
  must acknowledge the contribution by people stated in the acknowledgements.

You may not use or distribute this Software or any derivative works in any form 
for commercial purposes. Examples of commercial purposes would be running business 
operations, licensing, leasing, or selling the Software, distributing the Software 
for use with commercial products, using the Software in the creation or use of 
commercial products or any other activity which purpose is to procure a commercial 
gain to you or others. 
  
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
SHALL JUERGEN GALL AND CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
*/
#pragma once

#include "resource.h"
#include <atlconv.h>
#define YAFFS_NOBJECT_BUCKETS		256
#define UNYAFFS2_OBJTABLE_SIZE	YAFFS_NOBJECT_BUCKETS
#pragma comment(lib, "atls.lib")
#define DEFAULT_CHUNKSIZE 2048
#define MTD_MAX_OOBFREE_ENTRIES		8

static unsigned yaffs2_chunksize = DEFAULT_CHUNKSIZE;
static unsigned yaffs2_sparesize = 0;

struct nand_oobfree {
        unsigned int offset;
        unsigned int length;
};

unsigned int IsBig(unsigned int a, unsigned int b);

#define EXTRA_OBJECT_TYPE_SHIFT (28)
#define EXTRA_OBJECT_TYPE_MASK  ((0x0f) << EXTRA_OBJECT_TYPE_SHIFT)
#define MTD_MAX_OOBFREE_ENTRIES		8

#define ECCGETLAYOUT    _IOR('M', 17, struct nand_ecclayout)

bool CreateFolder(CString FolderPath);
void DODADODA();
CString m_path;
CString ExtractPath;
int oddsize = 64;

#pragma pack(1)
struct TAG {
			char x;
		char x1;
		unsigned int BLOCKID;

		unsigned int OBJECTID;

        unsigned int CHUNKID;
        unsigned int bytecount;
		char OBJECTTYPE;
        byte a[45];
};
#pragma pack()
#pragma pack(1)
struct TAG_EXT {
		char x0;
		char x1;
	    unsigned int BLOCKID;
		unsigned int OBJECTID;


		unsigned int PARENTOBJECTID; // 
        unsigned int FILESIZE;
        unsigned int ECC;

		unsigned char OBJECTTYPE;			// Object Type  0x30 / 0x10 
		unsigned char ExtendedTagFlag;	     // Extended Tag Flag 0x80
		unsigned char E1;
		unsigned char E2;
		unsigned char UNKNOWN01;
        byte a[13];
		unsigned int UNKNOWN02;
		byte b[20];
		
};
#pragma pack()
enum yaffs_obj_type {
	YAFFS_OBJECT_TYPE_UNKNOWN,
	YAFFS_OBJECT_TYPE_FILE,
	YAFFS_OBJECT_TYPE_SYMLINK,
	YAFFS_OBJECT_TYPE_DIRECTORY,
	YAFFS_OBJECT_TYPE_HARDLINK,
	YAFFS_OBJECT_TYPE_SPECIAL
};
#pragma pack(1)
struct CHUNKHEADER {
        unsigned char OBJECTTYPE;
		byte a0[3];
        unsigned int PARENTOBJECTID;
        short CHECKSUM;
        char FILENAME[256];
		short FFFF;
		unsigned int UID;
		unsigned int GID;
		__time32_t LastAccessTime;
		__time32_t LastModifiedTime;
		__time32_t LastMetaModifiedTime;
		unsigned int FILESIZE;
		int asa;
};
#pragma pack()


struct YAFFSSTRUCT {
	unsigned int OFFSET;
	unsigned int BLOCK_ID;
	unsigned int OBJECT_ID;
	unsigned int CHUNK_ID;
	unsigned int OBJECT_TYPE_IN_TAG;
	unsigned int PARENT_OBJECT_ID;
	unsigned int FILE_SIZE;
	unsigned int OBJECT_TYPE_IN_CHUNK;
	unsigned int UNKNOWN;
	CString FILE_NAME;
	CString MODIFIED;
	CString ACCESSED;
	CString META_MODIFIED;
};