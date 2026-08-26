#pragma once


#include <CLENAW/ClDialog.h>
#include <BLACKBOX/TextData.h>

class CGSensorDlg : public ClDialog
{
	DECLARE_DYNAMIC(CGSensorDlg)

public:
	CGSensorDlg(CWnd* pParent = NULL);
	virtual ~CGSensorDlg();

	enum { IDD = IDD_GSENSOR_DLG };

public:
			VOID					Synchronize			( DWORD dwCTS );
			VOID					SetSensorData		( CTextData* pSensorData );

public:

	virtual	BOOL					OnInitDialog();


protected:
	virtual void					DoDataExchange		( CDataExchange* pDX );
	virtual VOID					InitResourceID		( VOID );
			VOID					SetResourceState	( VOID );

	DECLARE_MESSAGE_MAP()
};
