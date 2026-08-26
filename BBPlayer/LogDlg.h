#pragma once


#include <CLENAW/ClDialog.h>
#include <BLACKBOX/Language.h>

class CLogDlg : public ClDialog
{
	DECLARE_DYNAMIC(CLogDlg)

public:
	CLogDlg(CWnd* pParent = NULL);
	virtual ~CLogDlg();

// Dialog Data
	enum { IDD = IDD_LOG_DLG };

protected:
	virtual	VOID					InitResourceID		( VOID );
			VOID					SetResourceState	( VOID );
	virtual 	VOID 					DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual 	BOOL 					OnInitDialog();
			
	DECLARE_MESSAGE_MAP()
public:
			VOID					SetLogFile( CString cstrLogFile );
	afx_msg 	void 					OnBnClickedLogSave();
	void								SetLanguage				( CLanguage * cLang) 	{m_pCLanguage = cLang;}

private:
	CLanguage * 			m_pCLanguage;
	CString	m_cstrLogFilePath;

};
