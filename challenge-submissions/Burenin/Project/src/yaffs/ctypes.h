//--------------------------------------------------------------------------------------------------------------------------------------
typedef enum _ECTYPE {
	ctype_init, // еще не определен сначала
	ctype_zip,ctype_jpg,ctype_exif,ctype_fat,ctype_empty,ctype_likefat,ctype_dir,ctype_dir2,ctype_undefined
}ECTYPE;

typedef enum _ECMODE {
	cmode_begin,cmode_end,emode_mid
} ECMODE;

typedef struct _TCTYPEINFO{
	_TCTYPEINFO(){
		eType=ctype_init;eMode=cmode_begin;
	};
	ECTYPE eType;
	ECMODE eMode;
	char cFileName[0x100];
} TCTYPEINFO,* PTCTYPEINFO;