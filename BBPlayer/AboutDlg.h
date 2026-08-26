#pragma once


#include <CLENAW/ClDialog.h>
#include <BLACKBOX/Language.h>

class CAboutDlg : public ClDialog
{
	DECLARE_DYNAMIC(CAboutDlg)

public:
	CAboutDlg(CWnd* pParent = NULL);
	virtual ~CAboutDlg();

	enum { IDD = IDD_ABOUT_DLG };

public:
	virtual BOOL					OnInitDialog		( VOID );
	afx_msg void					OnClickGotosite		( VOID );
	afx_msg void 					OnClickUpdateViewer		(VOID);
	afx_msg void 					OnClickUpdateFirmware		(VOID);
	afx_msg void 					OnTimer(UINT_PTR nIDEvent);

	void 						SetSelDrive( CString cstrSelDrive ) { m_cstrSelDrive = cstrSelDrive;}
	VOID						SetLanguage				( CLanguage * cLang) 	{m_pCLanguage = cLang;}
	VOID 						OnLanguageChange(VOID);
	VOID  						SetItemText(int nIDDlgItem, UINT resource, CString index_str = NULL );
	BOOL 						VersionCheck(void);
	static UINT 						ThreadServerCheck(void* pArg);
	BOOL						m_bThreadServerCheckExit;
	CWinThread* 				m_pThreadServerCheck;
protected:
	
	virtual void					DoDataExchange		( CDataExchange* pDX );
	virtual	VOID					InitResourceID		( VOID );
			VOID					SetResourceState	( VOID );

	DECLARE_MESSAGE_MAP()

private:
	CLanguage * 			m_pCLanguage;
	CString				m_strViewerFile;
	CString				m_strFirmwareFile;
	CString				m_cstrSelDrive;
};
