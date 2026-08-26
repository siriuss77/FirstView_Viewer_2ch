#pragma once


#include <CLENAW/ClDialog.h>

class CSelModelDlg : public ClDialog
{
	DECLARE_DYNAMIC(CSelModelDlg)

public:
	CSelModelDlg(CWnd* pParent = NULL);
	virtual ~CSelModelDlg();

	enum { IDD = IDD_SELMODEL_DLG };

public:

			const CString&			GetSelectedModel	( VOID );

public:

	virtual	BOOL					OnInitDialog		();


protected:

	virtual void					DoDataExchange		( CDataExchange* pDX );
	virtual	VOID					InitResourceID		( VOID );
	virtual void					OnOK				();
			VOID					SetResourceState	( VOID );

	DECLARE_MESSAGE_MAP()


private:
			CString					m_cstrSelModel;

};
