
// ConfigWriterDlg.h : header file
//

#pragma once

#include <BBPlayer/BBConfigure.h>
#include <CLUDE/ClCludeApp.H>
#include <string>

#define	SZ_WINDOW_MINI								80
#define	SZ_WINDOW_NORMAL							230

// CConfigWriterDlg dialog
class CConfigWriterDlg : public CDialog
{
// Construction
public:
	CConfigWriterDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CONFIGWRITER_DIALOG };

protected:

	virtual void					DoDataExchange(CDataExchange* pDX);
	virtual BOOL					OnInitDialog();


public:

	afx_msg void					OnPaint				();
	afx_msg HCURSOR					OnQueryDragIcon		();
	afx_msg void					OnClickDetail		();
	afx_msg void					OnClickAppPath		();
	afx_msg void					OnCbnDropdownDrive	();
	afx_msg void					OnCbnSelchangeSelModel();
	afx_msg void					OnClickWritetime	();
	afx_msg void					OnClickWriteConfig	();


protected:

			void					FillConfigList		( void );
			BOOL					GetComboString		( int nResID, CString& cstrComboString, int nSize );


// Implementation
protected:

	DECLARE_MESSAGE_MAP()


protected:

			HICON					m_hIcon;

			CBBConfigure			m_Configure;
			ClCludeApp				m_CludeApp;

			std::string				m_stsAppPath;

			INT						m_nOriginHeight;
			BOOL					m_bIsShowDetail;
			BOOL					m_bIsNeedFind;
};
