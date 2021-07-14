/**
 *	@file	FileCarving.h
 *	@date	2011/06/03
 *	@author	Jungheum Park, Jinkook Kim, Sungsu Lim, Minsu Park

 *	@brief	Defines for Carving
 */


enum FILEFORMAT
{
	FORMAT_UNKNOWN			=	0xF0,

	// Documents (0x00 ~ 0x1F)
	FORMAT_DOCUMENT			=	0x00,
	FORMAT_HWP				=	0x01,
	FORMAT_HWP3				=	0x02,
	FORMAT_HWP5				=	0x03,
	FORMAT_DOC				=	0x04,
	FORMAT_XLS				=	0x05,
	FORMAT_PPT				=	0x06,
	FORMAT_DOCX				=	0x07,
	FORMAT_XLSX				=	0x08,
	FORMAT_PPTX				=	0x09,
	FORMAT_ODT				=	0x0A,
	FORMAT_ODS				=	0x0B,
	FORMAT_RTF				=	0x0C,
	FORMAT_PDF				=	0x0D,
	FORMAT_COMPOUND			=	0x0E,
	FORMAT_OOXML			=	0x0F,

	// Graphics (0x20 ~ 0x3F)
	FORMAT_GRAPHIC			=	0x20,
	FORMAT_JPEG_EXIF		=	0x21,
	FORMAT_JPEG_JFIF		=	0x22,
	FORMAT_GIF				=	0x23,
	FORMAT_PNG				=	0x24,
	FORMAT_BMP				=	0x25,
	FORMAT_TIFF				=	0x26,
	FORMAT_PSD				=	0x27,
	FORMAT_EPS				=	0x28,
	FORMAT_WMF				=	0x29,
	FORMAT_EMF				=	0x2A,
	FORMAT_ICO				=	0x2B,
	FORMAT_CUR				=	0x2C,
	FORMAT_PSP				=	0x2D,

	// Multimedia (0x40 ~ 0x5F)
	FORMAT_MULTIMEDIA		=	0x40,
	FORMAT_MP3				=	0x41,
	FORMAT_AVI				=	0x42,
	FORMAT_CDA				=	0x43,
	FORMAT_QCP				=	0x44,
	FORMAT_RMI				=	0x45,
	FORMAT_WAV				=	0x46,
	FORMAT_ASFWMAWMV		=	0x47,
	FORMAT_MID				=	0x48,
	FORMAT_OGG				=	0x49,
	FORMAT_RA				=	0x4A,
	FORMAT_RAM				=	0x4B,
	FORMAT_MOV				=	0x4C,
	FORMAT_MP4				=	0x4D,
	FORMAT_MPG				=	0x4E,
	FORMAT_SWF				=	0x4F,
	FORMAT_FLAC				=	0x50,
	FORMAT_IVR				=	0x51,

	// Archive (0x60 ~ 0x6F)
	FORMAT_ARCHIVE			=	0x60,
	FORMAT_ZIP				=	0x61,
	FORMAT_RAR				=	0x62,
	FORMAT_GZIP				=	0x63,
	FORMAT_7Z				=	0x64,
	FORMAT_ALZ				=	0x65,
	FORMAT_BZIP2			=	0x66,
	FORMAT_LZH				=	0x67,
	FORMAT_CAB				=	0x68,
	FORMAT_JAR				=	0x69,

	// Email (0x70 ~ 0x7F)
	FORMAT_EMAIL			=	0x70,
	FORMAT_EML				=	0x71,
	FORMAT_DBX				=	0x72,
	FORMAT_PST				=	0x73,
	FORMAT_NSF				=	0x74,
	FORMAT_WAB				=	0x75,
	FORMAT_ENL				=	0x76,

	// Database (0x80 ~ 0x8F)			
	FORMAT_DATABASE			=	0x80,
	FORMAT_MDB				=	0x81,
	FORMAT_MYD				=	0x82,
	FORMAT_EDB				=	0x83,
	FORMAT_MDF				=	0x84,
	FORMAT_LDF				=	0x85,
	FORMAT_SQLITE			=	0x1000,	// SQLite format 3					// hmi
	FORMAT_SQLITE_TL_PAGE	=	0x1001,	// SQLITE Table B-Tree Leaf Page		// hmi
	FORMAT_SQLITE_TI_PAGE	=	0x1002,	// SQLITE Table B-Tree Leaf Page		// hmi
	FORMAT_SQLITE_IL_PAGE	=	0x1003,	// SQLITE Table B-Tree Leaf Page		// hmi
	FORMAT_SQLITE_II_PAGE	=	0x1004,	// SQLITE Table B-Tree Leaf Page		// hmi
	FORMAT_SQLITE_PM_PAGE	=	0x1005,		// hmi
	FORMAT_SQLITE_PM_ROOT_PAGE	= 0x1006,	// hmi
	FORMAT_SQLITE_PM_FREE_PAGE	= 0x1007,	// hmi
	FORMAT_SQLITE_FREE_PAGE	=	0x1010,		// hmi
	FORMAT_SQLITE_SLACK_PAGE=	0x1011,		// Slack or Overflow Page	// hmi
	FORMAT_SQLITE_EXCEPT	=	0X1012,

	// Internet (0x90 ~ 0x9F)
	FORMAT_WEB				=	0x90,
	FORMAT_HTML				=	0x91,
	FORMAT_XML				=	0x92,
	FORMAT_URL				=	0x93,
	FORMAT_IDX				=	0x94,

	// Execute, Library  (0xA0 ~ 0xAF)
	FORMAT_EXELIB			=	0xA0,
	FORMAT_PE				=	0xA1,
	FORMAT_EXE				=	0xA2,
	FORMAT_DLL				=	0xA3,

	// Other (0xB0 ~ 0xBF)
	FORMAT_OTHER			=	0xB0,
	FORMAT_LNK				=	0xB1,
	FORMAT_HLP				=	0xB2,
	FORMAT_REGEDIT			=	0xB3,
	FORMAT_REGDAT			=	0xB4,
	FORMAT_PF				=	0xB5,

	// FileSystem (0xC0 ~ 0xCF)
	FORMAT_MFTe				=	0xC0,
	FORMAT_DIRe				=	0xC1,
};


// Document Types
static const CHAR	*HWP3_sigHEADER			= "HWP Document File V3";								/* HWP 3.0 HEADER SIGNAUTRE									*/
static const SHORT	HWP_sigBODYTEXT[8]		= {0x42, 0x6F, 0x64, 0x79, 0x54, 0x65, 0x78, 0x74};		/* HWP 5.0 BODYTEXT				: "BodyText(UNICODE)"		*/
static const BYTE	COMPOUND_sigHEADER[8]	= {0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1};		/* COMPOUND SIGNATURE			: D0CF11E0A1B11AE1			*/
static const CHAR	*OOXML_sigHEADER		= "[Content_Types].xml";
static const CHAR	*ODF_sigHEADER1			= "mimetypeapplication";								// ZIP sig + offset 30
static const CHAR	*ODF_sigHEADER2			= "opendocument";
static const CHAR	*ODT_sigHEADER			= "textPK";
static const CHAR	*ODS_sigHEADER			= "spreadsheetPK";
static const CHAR	*RTF_sigHEADER			= "{\rtf";
static const BYTE	PDF_sigHEADER[7]		= {0x25, 0x50, 0x44, 0x46, 0x2D, 0x31, 0x2E};			/* PDF HEADER SIGNATURE			: 255044462D312E("%PDF-1.") */


// Graphics Types
static const BYTE	JPEG_EXIF_sigHEADER[4]	= {0xFF, 0xD8, 0xFF, 0xE1};								/* JPEG EXIF HEADER SIGNATURE	: FFD8FFE1					*/
static const BYTE	JPEG_JFIF_sigHEADER[4]	= {0xFF, 0xD8, 0xFF, 0xE0};								/* JPEG JFIF HEADER	SIGNATURE	: FFD8FFE0					*/
static const BYTE	GIF_sigHEADER1[6]		= {0x47, 0x49, 0x46, 0x38, 0x37, 0x61};					/* GIF HEADER SIGNATURE	1		: 474946383761("GIF87a")	*/
static const BYTE	GIF_sigHEADER2[6]		= {0x47, 0x49, 0x46, 0x38, 0x39, 0x61};					/* GIF HEADER SIGNATURE	2		: 474946383961("GIF89a")	*/
static const BYTE	PNG_sigHEADER[8]		= {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};		/* PNG HEADER SIGNATURE			: 89504E470D0A1A0A			*/
static const BYTE	BMP_sigHEADER[2]		= {0x42, 0x4D};					// "BM"
static const BYTE	TIFF_sigHEADER1[3]		= {0x49, 0x49, 0x2A};			// "||*"
static const BYTE	TIFF_sigHEADER2[3]		= {0x4D, 0x4D, 0x2A};			// "MM*"
static const BYTE	PSD_sigHEADER[4]		= {0x38, 0x42, 0x50, 0x53};		// "8BPS"
static const CHAR	*EPS_sigHEADER			= "%!PS-Adobe-";
static const BYTE	WMF_sigHEADER[4]		= {0xD7, 0xCD, 0xC6, 0x9A};
static const BYTE	EMF_sigHEADER1[4]		= {0x01, 0x00, 0x00, 0x00};
static const BYTE	EMF_sigHEADER2[3]		= {0x45, 0x4D, 0X46};			// offset : 41
static const BYTE	ICO_sigHEADER[4]		= {0x00, 0x00, 0x01, 0x00};
static const BYTE	CUR_sigHEADER[4]		= {0x00, 0x00, 0x02, 0x00};
static const BYTE	PSP_sigHEADER[4]		= {0x7E, 0x42, 0x4B, 0x00};


// Multimedia Types
static const BYTE	RIFF_sigHEADER[4]		= {0x52, 0x49, 0x46, 0x46};		// "RIFF"
static const BYTE	MP3_sigHEADER[3]		= {0x49, 0x44, 0x33};			// "ID3"
static const BYTE	AVI_sigHEADER[8]		= {0x41, 0x56, 0x49, 0x20, 0x4C, 0x49, 0x53, 0x54};	// RIFF - "AVI LIST"
static const BYTE	CDA_sigHEADER[8]		= {0x43, 0x44, 0x44, 0x41, 0x66, 0x6D, 0x74, 0x20};	// RIFF - "CDDAfmt "
static const BYTE	QCP_sigHEADER[8]		= {0x51, 0x4C, 0x43, 0x4D, 0x66, 0x6D, 0x74, 0x20};	// RIFF - "QLCMfmt "
static const BYTE	RMI_sigHEADER[8]		= {0x52, 0x4D, 0x49, 0x44, 0x64, 0x61, 0x74, 0x61};	// RIFF - "RMIDdata"
static const BYTE	WAV_sigHEADER[8]		= {0x57, 0x41, 0x56, 0x45, 0x66, 0x6D, 0x74, 0x20};	// RIFF - "WAVEfmt "
static const BYTE	ASFWMAWMV_sigHEADER[16]	= {0x30, 0x26, 0xB2, 0x75, 0x8E, 0x66, 0xCF, 0x11, 0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C};
static const BYTE	MID_sigHEADER[4]		= {0x4D, 0x54, 0x68, 0x64};		// "MThd"
static const BYTE	OGG_sigHEADER[8]		= {0x4F, 0x67, 0x67, 0x53, 0x00, 0x02, 0x00, 0x00};	// "Oggs    "
static const BYTE	RA_sigHEADER[4]			= {0x2E, 0x52, 0x45, 0x43};
static const BYTE	RAM_sigHEADER[7]		= {0x72, 0x74, 0x73, 0x70, 0x3A, 0x2F, 0x2F};		// "rtsp://"
static const BYTE	MOV_sigHEADER1[4]		= {0x6D, 0x6F, 0x6F, 0x76};					// offset : 4 "moov"	
static const BYTE	MOV_sigHEADER2[4]		= {0x66, 0x72, 0x65, 0x65};					// offset : 4 "free"
static const BYTE	MOV_sigHEADER3[4]		= {0x6D, 0x64, 0x61, 0x74};					// offset : 4 "mdat"
static const BYTE	MOV_sigHEADER4[4]		= {0x77, 0x69, 0x64, 0x65};					// offset : 4 "wide"
static const BYTE	MOV_sigHEADER5[6]		= {0x66, 0x74, 0x79, 0x70, 0x71, 0x74};		// offset : 4 "ftypgt"
static const BYTE	MP4_sigHEADER[8]		= {0x66, 0x74, 0x79, 0x79, 0x33, 0x67, 0x70, 0x35};	// offset : 4 "ftyp3gp5" 
static const BYTE	MPG_sigHEADER[3]		= {0x00, 0x00, 0x01};			// 4 byte : 0xBx (x is 0 to F)
static const BYTE	SWF_sigHEADER1[3]		= {0x43, 0x57, 0x53};			// "CWS"
static const BYTE	SWF_sigHEADER2[3]		= {0x46, 0x4C, 0x56};			// "FLV"
static const BYTE	SWF_sigHEADER3[3]		= {0x46, 0x57, 0x53};			// "FWS"
static const BYTE	FLAC_sigHEADER[4]		= {0x66, 0x4C, 0x61, 0x43};		// "fLaC"
static const BYTE	IVR_sigHEADER[4]		= {0x2E, 0x52, 0x45, 0x43};		// " REC"


// Archive Types
static const BYTE	ZIP_sigHEADER[4]		= {0x50, 0x4B, 0x03, 0x04};								/* ZIP HEADER SIGNATURE			: 504B0304					*/
static const BYTE	RAR_sigHEADER[6]		= {0x52, 0x61, 0x72, 0x21, 0x1A, 0x07};					/* RAR HEADER SIGNATURE			: 526172211A07				*/
static const BYTE	GZIP_sigHEADER[3]		= {0x1F, 0x8B, 0x08};
static const BYTE	ZIP7_sigHEADER[6]		= {0x37, 0x7A, 0xBC, 0xAF, 0x27, 0x1C};
static const BYTE	ALZ_sigHEADER[4]		= {0x41, 0x4C, 0x5A, 0x01};								/* ALZ HEADER SIGNATURE			: 414C5A01					*/
static const BYTE	BZIP2_sigHEADER[3]		= {0x42, 0x5A, 0x68};			// "BZh"
static const BYTE	LZH_sigHEADER[3]		= {0x2D, 0x6C, 0x68};			// "-lh"
static const BYTE	CAB_sigHEADER[4]		= {0x49, 0x53, 0x63, 0x28};		// "ISc("
static const BYTE	JAR_sigHEADER[6]		= {0x4A, 0x41, 0x52, 0x43, 0x53, 0x00};	// "JARCS"


// Email Types
static const BYTE	EML_sigHEADER[4]		= {0x46, 0x72, 0x6F, 0x6D};		// "FHom"
static const BYTE	DBX_sigHEADER[4]		= {0xCF, 0XAD, 0x12, 0xFE};	
static const BYTE	PST_sigHEADER[4]		= {0x21, 0x42, 0x44, 0x4E};		// "!BDN"
static const BYTE	NSF_sigHEADER[6]		= {0x1A, 0x00, 0x00, 0x04, 0x00, 0x00};
static const BYTE	WAB_sigHEADER[16]		= {0x9C, 0xCB, 0xCB, 0x8D, 0x13, 0x75, 0xD2, 0x11, 0x91, 0x58, 0x00, 0xC0, 0x4F, 0x79, 0x56, 0xA4};
static const BYTE	ENL_sigHEADER[10]		= {0x40, 0x40, 0x40, 0x20, 0x00, 0x00, 0x40, 0x40, 0x40, 0x40};


// Database Types
static const BYTE	MDB_sigHEADER[15]		= {0x53, 0x74, 0x61, 0x72, 0x64, 0x61, 0x72, 0x64, 0x20, 0x4A, 0x65, 0x74, 0x20, 0x44, 0x42};	// "Standard Jet DB"
static const BYTE	MDF_sigHEADER[4]			= {0x1, 0x0f, 0x00, 0x00}; // MS SQL Server 2000, 2005, 2008 & ndf file
static const BYTE	SQLITE_sigHeader[16]	= {0x53, 0x51, 0x4c, 0x69, 0x74, 0x65, 0x20, 0x66, 0x6f, 0x72, 0x6d, 0x61, 0x74, 0x20, 0x33, 0x00};	// "SQLite format 3"


// Internet Types
static const CHAR	*HTML_sigHEADER1		= "<html";
static const CHAR	*HTML_sigHEADER2		= "<HTML";
static const CHAR	*HTML_sigHEADER3		= "<!doctype ";
static const CHAR	*HTML_sigHEADER4		= "<!DOCTYPE ";
static const CHAR	*XML_sigHEADER			= "<?xml version";
static const CHAR	*URL_sigHEADER			= "[InternetShortcut]";
static const BYTE	IDX_sigHEADER[16]		= {0x43, 0x6c, 0x69, 0x65, 0x6e, 0x74, 0x20, 0x55, 0x72, 0x6c, 0x43, 0x61, 0x63, 0x68, 0x65, 0x20}; // Index.dat


// Executable, Library Types
static const BYTE	PE_sigHEADER[2]			= {0x4D, 0x5A};				// "MZ"


// Other Types
static const BYTE	LNK_sigHEADER[4]		= {0x4C, 0x00, 0x00, 0x00};
static const BYTE	HLP_sigHEADER[4]		= {0x3F, 0x5F, 0x03, 0x00};
static const CHAR	*REGEDIT_sigHEADER		= "REGEDIT4";
static const BYTE	REGEDITU_sigHEADER[16]	= {0x0FF, 0x0FE, 0x57, 0x00, 0x69, 0x00, 0x6E, 0x00, 0x64, 0x00, 0x6F, 0x00, 0x77, 0x00, 0x73, 0x00};
static const BYTE	REGDAT_sigHEADER[4]		= {0x72, 0x65, 0x67, 0x66};	// "regf"
static const BYTE	PF_sigHEADER[4]			= {0x53, 0x43, 0x43, 0x41}; // Prefetch File
static const BYTE	MFTe_sigHEADER[4]		= {0x46, 0x49, 0x4c, 0x45}; // NTFS MFTentry
static const BYTE	BOM_UTF8[3]				= {0xEF, 0xBB, 0xBF};

