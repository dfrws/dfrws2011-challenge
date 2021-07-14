#include <stdafx.h>
#include <set>
#include "./textextractor.h"
//--------------------------------------------------------------------------------------------------

typedef bool (*TPROCESSOR) (char*,PTEXTRPARAM pParam);
typedef std::set<CString> SETSTR;

//--------------------------------------------------------------------------------------------------

static FILE* fInFile=NULL,* fOutFile=NULL;
static SETSTR setString;

//--------------------------------------------------------------------------------------------------
int extractor_initialize(char* pcInFileName,char* pcOutFileName){
	int res=0;

	if (fInFile) fclose(fInFile);
	fInFile=NULL;

	if (fOutFile) fclose(fOutFile);
	fOutFile=NULL;

	if (NULL==(fInFile=fopen(pcInFileName,"rb"))) return -1;
	if (NULL==(fOutFile=fopen(pcOutFileName,"wt+"))) return -2;

	return res;
}
//--------------------------------------------------------------------------------------------------
int extractor_deinitialize(){
	int res=0;
	if (fInFile) fclose(fInFile);
	fInFile=NULL;

	if (fOutFile) fclose(fOutFile);
	fOutFile=NULL;
	return res;
}

//--------------------------------------------------------------------------------------------------
bool extractor_default_processor(char* pcLine,PTEXTRPARAM pParam){
	bool res=true;
	int len=strlen(pcLine);

	int cntSpace=0,cntSmall2Big=0,cntNumbers=0,cntPrefix=0,cntLetters=0;
	for (int i=0;i<len;i++)
	{
		char b=pcLine[i],b1=pcLine[+1];
		if (b==' ') cntSpace++;
		if (b>='0' &&b<='9') cntNumbers++;
		if (b<'0' || (b>=':' && b<='@') || (b>='[' && b<='`') || b>'z') cntPrefix++;
		if ((b>='a' && b<='z') || (b>='A' && b<='Z')) cntLetters++;
		if ((b>='a' && b<='z') && (b1>='A' && b<='Z')) cntSmall2Big++;// ....dE...
	}

	if (pParam->flURLonly){
		if (0==strstr(pcLine,"://") && 0==strstr(pcLine,":\\\\")) res=false;
	}
	else{
		if (cntSpace==0 && cntSmall2Big>0 && pParam->flSmall2Big) res=false;
		if (cntLetters==0) res=false;
		if (cntSpace==0 && pParam->flSkipNoSpace) res=false;
	}


	if (res && pParam->flUniqOnly){
		std::pair<SETSTR::iterator,bool> pr;
		pr=setString.insert(CString(pcLine));
		if (!pr.second){
			res=false;
		}
	}

	if (res && pParam->pm_Object){
		CString strPrev;
		pParam->pm_Object->GetWindowText(strPrev);
		strPrev+=CString(pcLine)+"\x0D\x0A";
		pParam->pm_Object->SetWindowText(strPrev);
	}

	return res;
}

//--------------------------------------------------------------------------------------------------
#define BASE_BLOCK_LEN 0x10

int extractor_process(int minLen,TPROCESSOR tProc=extractor_default_processor,PTEXTRPARAM pParam=NULL){
	int res=0;
	_fseeki64(fInFile,0,SEEK_END);
	__int64 size=_ftelli64(fInFile);
	_fseeki64(fInFile,0,SEEK_SET);

	char b;
	int curlen=0;

	int szstrLine=BASE_BLOCK_LEN;
	char* strLine=(char*)malloc(szstrLine);

	for (__int64 i=0;i<size;i++){
		fread(&b,1,1,fInFile);
		if ((b!='\x0D' && b!='\x0A')&& b<0x20 || b>0x7E || strchr(pParam->strExceptSybols.GetBuffer(),b)!=NULL) {
			if (curlen>minLen){
				strLine[curlen]='\0';
				if (tProc(strLine,pParam)){
					fprintf(fOutFile,"%s\n",strLine);
				}
			}
			curlen=0;
			continue;
		}
		if (curlen>=szstrLine-1){ // оставляем 1 на завершающий '\0'
			szstrLine+=BASE_BLOCK_LEN;
			strLine=(char*)realloc(strLine,szstrLine);
		}
		strLine[curlen]=b;
		curlen++;
	}

	if (curlen){
		if (curlen>minLen){
			strLine[curlen]='\0';
			if (tProc(strLine,pParam)){
				fprintf(fOutFile,"%s\n",strLine);
			}
		}
	}

	free(strLine);

	return res;
}

//--------------------------------------------------------------------------------------------------
int ExtractASCIIStrings(char* pcInFileName,char* pcOutFileName,int minLen,PTEXTRPARAM pParam){
	int res=0;

	setString.clear();

	if (0==(res=extractor_initialize(pcInFileName,pcOutFileName))){
		res=extractor_process(minLen,extractor_default_processor,pParam);
		extractor_deinitialize();
	}

	return res;
}