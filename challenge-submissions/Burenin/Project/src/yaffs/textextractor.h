
//------------------------------------------------------------------------------------
typedef struct _TEXTRPARAM 
{
	bool flSkipNoSpace;
	bool flSmall2Big;
	bool flURLonly;
	bool flUniqOnly;
	CString strExceptSybols;
	CEdit* pm_Object;
}TEXTRPARAM,*PTEXTRPARAM;
//------------------------------------------------------------------------------------
int ExtractASCIIStrings(char* pcInFileName,char* pcOutFileName,int minLen,PTEXTRPARAM pParam=NULL);