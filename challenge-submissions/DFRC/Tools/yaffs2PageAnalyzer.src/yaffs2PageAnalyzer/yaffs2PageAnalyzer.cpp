/**
	@mainpage		yaffs2PageAnalyzer for DFRWS 2011 Forensics Challenge
	@date			2011/07/21
	@author			Jungheum Park (junghmi@gmail.com)
	@affiliation	Digital Forensic Research Center @ Korea University

	@section	MODIFYINFO	History

				- Jungheum Park / 2010.06.05
					- Initial Version
				- Jungheum Park / 2010.06.25
					- Add classification functions
				- Jungheum Park / 2010.07.07
					- Add ananysis functions
				- Jungheum Park / 2010.07.20
					- Modify some functions

	@license

		License: DFRC@KU

		Copyright (c) 2011, Jungheum Park (junghmi@korea.ac.kr),
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


#include "stdafx.h"
#include "yaffs2PageAnalyzer.h"

#include "PageClassifier.h"
#include "PageAnalyzer.h"


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	if(argc != 3)
	{
		printf("=======================================================\n");
		printf("  YAFFS2 Page Analyzer v0.5\n");
		printf("  Digital Forensic Research Center, Korea University.\n");
		printf("=======================================================\n");
		printf("  Usage : yaffs2PageAnalyzer.exe  <imagefilefullpath>  <outputfolderfullpath>\n");
		printf("  e.g., > yaffs2PageAnalyzer  C:\\mtd7.dd  C:\\output\n");
		return TRUE;
	}

	CString srcPath1 = argv[1];
	CString strPath2 = _T("");
	CString dstPath  = _T("");
	CString dstPathBase = argv[2];

	dstPath.Format(_T("%s\\CLASSIFICATION_RESULT"), dstPathBase);

	CPageClassifier pageClassifier(2048, 1024);
	pageClassifier.RunPageClassifier(srcPath1, strPath2, dstPath);

	srcPath1 = dstPath;

	dstPath.Format(_T("%s\\ANALYSIS_RESULT"), dstPathBase);

	CPageAnalyzer pageAnalyzer(2048, 1024);
	pageAnalyzer.RunPageAnalyzer(srcPath1, strPath2, dstPath);

	return TRUE;
}
