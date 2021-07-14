// AndroidProcessorDlg.h : header file
//
#define USE_DROP
#pragma once

#ifdef USE_DROP
#include <vcl/controldropfiles.hpp>
#include "afxwin.h"
CONTROLWITHDROPFILES_DECLARATION(CEditWithDrop,CEdit);
#else
#define CEditWithDrop CEdit
#endif



// CAndroidProcessorDlg dialog
class CAndroidProcessorDlg : public CDialog
{
// Construction
public:
	CAndroidProcessorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_ANDROIDPROCESSOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_strFileTAR;
	CString m_strDirToUntar;
	afx_msg void OnBnClickedBtnUntar();
	CString m_strImage;
	afx_msg void OnBnClickedBtnUntar2();
	CString m_strDestDir;
	CString m_strExtInFileN;
	CString m_strExtOutFileN;
	afx_msg void OnBnClickedBtnExtract();
	CString m_strSize;

	CEditWithDrop m_edtIF,m_edtD,m_edtIFP,m_edtDF,m_edtIFEX,m_edtOFEX;
	BOOL m_btnModeSkipNoSpace;
	CString m_strTextField;
	CEdit m_edtTextField;
	BOOL m_btnSkipSmall2Big;
	BOOL m_btnURLonly;
	BOOL m_chkUniqOnly;
public:
	CString m_edtException;
public:
	afx_msg void OnBnClickedCheck4();
public:
	BOOL m_chkExceptEnable;
public:
	CEdit m_edtbtnException;
};
