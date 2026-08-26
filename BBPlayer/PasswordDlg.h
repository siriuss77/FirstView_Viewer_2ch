#include "afxwin.h"
#if !defined(AFX_PASSWORDDLG_H__72A3ADDF_75EB_4990_ADFD_E60AE0CA0EAF__INCLUDED_)
#define AFX_PASSWORDDLG_H__72A3ADDF_75EB_4990_ADFD_E60AE0CA0EAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PasswordDlg.h : header file
//

#include <CLENAW/ClDialog.h>
#include <BLACKBOX/Language.h>
#include <BBConfigure.h>
#include <ConfigTextFile.h>

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg dialog

class CPasswordDlg : public CDialog
{
// Construction
public:
	CPasswordDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPasswordDlg)
	enum { IDD = IDD_PASSWORD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPasswordDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPasswordDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CEdit m_cePassword;
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	
private:
	CString m_csPassword[3];
	CLanguage * 			m_pCLanguage;
public:
	void 	OnLanguageChange();
	void 	SetPassword(TCHAR *password, TCHAR *password2 = NULL, TCHAR *password3 = NULL);
	void		SetLanguage				( CLanguage * cLang){m_pCLanguage = cLang;}
	void  	SetItemText(int nIDDlgItem, UINT resource, CString index_str = NULL);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSWORDDLG_H__72A3ADDF_75EB_4990_ADFD_E60AE0CA0EAF__INCLUDED_)
