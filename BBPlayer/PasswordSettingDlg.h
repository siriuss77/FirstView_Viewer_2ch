#include "afxwin.h"
#if !defined(AFX_PASSWORDSETTINGDLG_H__ACA761C9_B5F6_4528_8944_41761CBC08DE__INCLUDED_)
#define AFX_PASSWORDSETTINGDLG_H__ACA761C9_B5F6_4528_8944_41761CBC08DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PasswordSettingDlg.h : header file
//
#include <CLENAW/ClDialog.h>
#include <BLACKBOX/Language.h>
#include <BBConfigure.h>
#include <ConfigTextFile.h>

/////////////////////////////////////////////////////////////////////////////
// CPasswordSettingDlg dialog

class CPasswordSettingDlg : public CDialog
{
// Construction
public:
	CPasswordSettingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual BOOL OnInitDialog();
// Dialog Data
	//{{AFX_DATA(CPasswordSettingDlg)
	enum { IDD = IDD_PASSWORD_SETTING };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPasswordSettingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPasswordSettingDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void 	SetOldPassword(TCHAR *OldPassword, TCHAR *OldPassword2 = NULL, TCHAR *OldPassword3 = NULL);
	CString 	GetPassword(void);
	void		SetLanguage				( CLanguage * cLang){m_pCLanguage = cLang;}
	void  	SetItemText(int nIDDlgItem, UINT resource, CString index_str = NULL);
private:
	CString m_csOldPassword[3];
	CString m_csNewPassword;
	CEdit m_ceOldPassword;
	CEdit m_ceNewPassword;
	CEdit m_ceConfirm;
	CLanguage * 			m_pCLanguage;
public:
	void OnLanguageChange();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSWORDSETTINGDLG_H__ACA761C9_B5F6_4528_8944_41761CBC08DE__INCLUDED_)
