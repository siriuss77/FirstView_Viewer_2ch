#pragma once


#include <CLENAW/ClDialog.h>

class CSelDriveDlg : public ClDialog
{
	DECLARE_DYNAMIC(CSelDriveDlg)

public:
	CSelDriveDlg(CWnd* pParent = NULL);
	virtual ~CSelDriveDlg();

	enum { IDD = IDD_SELDRIVE_DLG };

public:
	virtual BOOL					OnInitDialog();
	afx_msg void					OnSelDriveDropDown();

			const CString&			GetSelectedDrive	( VOID ){ return this->m_cstrDrive; };

protected:
	virtual void					OnOK();
	virtual	VOID					InitResourceID		( VOID );
			VOID					SetResourceState	( VOID );
	virtual	void					DoDataExchange		( CDataExchange* pDX );

	DECLARE_MESSAGE_MAP()

protected:
			CString					m_cstrDrive;
};
