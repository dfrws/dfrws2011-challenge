// AndroidProcessorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AndroidProcessor.h"
#include "AndroidProcessorDlg.h"
#include "yaffs/untar.h"
#include "yaffs/afs_parser.h"
#include "yaffs/textextractor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef USE_DROP
CONTROLWITHDROPFILES_DEFINITION(CEditWithDrop,CEdit);
#endif
// CAndroidProcessorDlg dialog

//------------------------------------------------------------------------------------
CAndroidProcessorDlg::CAndroidProcessorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAndroidProcessorDlg::IDD, pParent)
	, m_strFileTAR(_T(""))
	, m_strDirToUntar(_T(""))
	, m_strImage(_T(""))
	, m_strDestDir(_T(""))
	, m_strExtInFileN(_T(""))
	, m_strExtOutFileN(_T(""))
	, m_strSize(_T(""))
	, m_btnModeSkipNoSpace(FALSE)
	, m_strTextField(_T(""))
	, m_btnSkipSmall2Big(FALSE)
	, m_btnURLonly(FALSE)
	, m_chkUniqOnly(FALSE)
	, m_edtException(_T(""))
	, m_chkExceptEnable(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

//------------------------------------------------------------------------------------
void CAndroidProcessorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strFileTAR);
	DDX_Text(pDX, IDC_EDIT2, m_strDirToUntar);
	DDX_Text(pDX, IDC_EDIT3, m_strImage);
	DDX_Text(pDX, IDC_EDIT4, m_strDestDir);
	DDX_Text(pDX, IDC_EDIT5, m_strExtInFileN);
	DDX_Text(pDX, IDC_EDIT6, m_strExtOutFileN);
	DDX_Text(pDX, IDC_EDIT7, m_strSize);
	DDX_Control(pDX, IDC_EDIT1, m_edtIF);
	DDX_Control(pDX, IDC_EDIT2, m_edtD);
	DDX_Control(pDX, IDC_EDIT3, m_edtIFP);
	DDX_Control(pDX, IDC_EDIT4, m_edtDF);
	DDX_Control(pDX, IDC_EDIT5, m_edtIFEX);
	DDX_Control(pDX, IDC_EDIT6, m_edtOFEX);
	DDX_Check(pDX, IDC_CHK_NOSPACE, m_btnModeSkipNoSpace);
	DDX_Text(pDX, IDC_EDIT9, m_strTextField);
	DDX_Control(pDX, IDC_EDIT9, m_edtTextField);
	DDX_Check(pDX, IDC_CHECK2, m_btnSkipSmall2Big);
	DDX_Check(pDX, IDC_CHECK3, m_btnURLonly);
	DDX_Check(pDX, IDC_CHK_UNIQ, m_chkUniqOnly);
	DDX_Text(pDX, IDC_EDIT8, m_edtException);
	DDX_Check(pDX, IDC_CHECK4, m_chkExceptEnable);
	DDX_Control(pDX, IDC_EDIT8, m_edtbtnException);
}

BEGIN_MESSAGE_MAP(CAndroidProcessorDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_UNTAR, &CAndroidProcessorDlg::OnBnClickedBtnUntar)
	ON_BN_CLICKED(IDC_BTN_UNTAR2, &CAndroidProcessorDlg::OnBnClickedBtnUntar2)
	ON_BN_CLICKED(IDC_BTN_EXTRACT, &CAndroidProcessorDlg::OnBnClickedBtnExtract)
	ON_BN_CLICKED(IDC_CHECK4, &CAndroidProcessorDlg::OnBnClickedCheck4)
END_MESSAGE_MAP()


// CAndroidProcessorDlg message handlers

//------------------------------------------------------------------------------------
BOOL CAndroidProcessorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	m_strFileTAR="c:\\DFRWS\\Case1.tar";
	m_strDirToUntar="c:\\DFRWS\\Case1-1";
	m_strImage="C:/DFRWS/case1/mtdblock6.img";
	m_strDestDir="C:/DFRWS/LOG/Files6";

	m_strExtInFileN="C:/DFRWS/LOG/Files6/LST/mmssms.db/mmssms.db.(full)";
	m_strExtOutFileN="C:/DFRWS/LOG/Files6/mmssms.db.(full).txt";
	m_strSize="5";

	m_btnSkipSmall2Big=TRUE;
	m_btnModeSkipNoSpace=TRUE;
	m_chkExceptEnable=TRUE;

	m_edtException="%^|+=";

	::EnableWindow(m_edtbtnException.m_hWnd, m_chkExceptEnable);

	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

//------------------------------------------------------------------------------------
void CAndroidProcessorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//------------------------------------------------------------------------------------
// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAndroidProcessorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//------------------------------------------------------------------------------------
void CAndroidProcessorDlg::OnBnClickedBtnUntar()
{
	CString strText;
	UpdateData();
	int res=process_tar_uncompress(m_strFileTAR.GetBuffer(),m_strDirToUntar.GetBuffer());

	strText.Format("Untar complete with status %d",res);
	MessageBox(strText,"Processing complete",MB_OK | MB_ICONINFORMATION);
}

//------------------------------------------------------------------------------------
void CAndroidProcessorDlg::OnBnClickedBtnUntar2()
{
	CString strText;
	UpdateData();
	int res=afs_parser(m_strImage.GetBuffer(), m_strDestDir.GetBuffer(),false);

	strText.Format("Image parsing complete with status %d",res);
	MessageBox(strText,"Processing complete",MB_OK | MB_ICONINFORMATION);
}

//------------------------------------------------------------------------------------
void CAndroidProcessorDlg::OnBnClickedBtnExtract()
{
	CString strText;
	UpdateData();
	char* en;

	TEXTRPARAM tParam;
	tParam.flSkipNoSpace=m_btnModeSkipNoSpace;
	tParam.pm_Object=&m_edtTextField;
	tParam.flSmall2Big=m_btnSkipSmall2Big;
	tParam.flURLonly=m_btnURLonly;
	tParam.flUniqOnly=m_chkUniqOnly;
	if (m_chkExceptEnable)
		tParam.strExceptSybols=m_edtException;
	else
		tParam.strExceptSybols="";

	m_edtTextField.SetWindowText("");

	int res=ExtractASCIIStrings(m_strExtInFileN.GetBuffer(), m_strExtOutFileN.GetBuffer(),strtoul(m_strSize.GetBuffer(),&en,10),&tParam);
	strText.Format("ExtractASCIIStrings complete with status %d",res);
	MessageBox(strText,"Processing complete",MB_OK | MB_ICONINFORMATION);
}

//------------------------------------------------------------------------------------
void CAndroidProcessorDlg::OnBnClickedCheck4()
{
	UpdateData();
	::EnableWindow(m_edtbtnException.m_hWnd, m_chkExceptEnable);
}
