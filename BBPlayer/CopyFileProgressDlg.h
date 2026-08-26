#pragma once
#include "afxcmn.h"
#include <CLWBC/wbcFileManager.h>
#include <BLACKBOX/Language.h>
#include <CLENAW/ClListCtrl.H>

// CCopyFileProgressDlg dialog

class CCopyFileProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(CCopyFileProgressDlg)

public:
	CCopyFileProgressDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCopyFileProgressDlg();

// Dialog Data
	enum { IDD = IDD_COPYFILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	HANDLE m_hThread;

	virtual void OnCancel();

	CLanguage * 			m_pCLanguage;
	
public:
	CProgressCtrl m_ProgressCtrl;
	CString m_strFileName;

	CString m_strSrcPath;
	CString m_strDestPath;
	int	m_nProgressPos;
	int	m_nListupIndex;
	int 	m_nCopyFiles;
	CWBCFileManager *mp_fileManager;
	ClListCtrl* mp_FileList;
	BOOL m_bIsRemovalNxfs;

	BOOL m_bStop;

	void	SetLanguage	( CLanguage * cLang) 	{m_pCLanguage = cLang;}
	void UpdateProgressbar(int nDoing);
	void RunCopyFileThread();
	void ExitCopyFileThread();

	virtual BOOL OnInitDialog();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
