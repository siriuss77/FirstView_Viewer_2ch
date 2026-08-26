
#include "stdafx.h"
#include "BBPlayer.h"
#include "GSensorDlg.h"

#include <CLENAW/ClGSensorGraph.h>
#include <HELPER/ClLog.H>


IMPLEMENT_DYNAMIC(CGSensorDlg, ClDialog)

CGSensorDlg::CGSensorDlg(CWnd* pParent /*=NULL*/)
	: ClDialog(CGSensorDlg::IDD, pParent)
{

}

CGSensorDlg::~CGSensorDlg()
{
}

void CGSensorDlg::DoDataExchange(CDataExchange* pDX)
{
	ClDialog::DoDataExchange(pDX);
}


VOID CGSensorDlg::InitResourceID()
{
	this->m_pDialogManager->AddResource( TEXT("gsensor.button.exit"),	TYPE_BUTTON,	IDOK					);
	this->m_pDialogManager->AddResource( TEXT("gsensor.text.title"),	TYPE_TEXT,		IDC_GSENSOR_TEXT_TITLE	);
	this->m_pDialogManager->AddResource( TEXT("gsensor.graph"),			TYPE_GSENSOR,	IDC_GSENSOR_GRAPH		);
}


VOID CGSensorDlg::SetResourceState( VOID )
{
	this->m_pDialogManager->SetResourceState( 0, TEXT("gsensor.button.exit"),	TRUE, TRUE	);
	this->m_pDialogManager->SetResourceState( 0, TEXT("gsensor.text.title"),	TRUE, TRUE	);
	this->m_pDialogManager->SetResourceState( 0, TEXT("gsensor.graph"),			TRUE, TRUE	);

	this->m_pDialogManager->AdaptResourceState( 0 );

	return;
}


BEGIN_MESSAGE_MAP(CGSensorDlg, ClDialog)
END_MESSAGE_MAP()


BOOL CGSensorDlg::OnInitDialog()
{
	ClDialog::OnInitDialog();

	if( !this->m_pDialogManager )
	{
		CLD_FL( DF_ERROR, "GSensor Dialog can not CLUDING! You must call 'OpenCLUDE' before 'DoModal' or 'ShowWindow'." );
		return FALSE;
	}

	this->SetResourceState();

	return TRUE;
}


void CGSensorDlg::Synchronize( DWORD dwCTS )
{
	ClGSensorGraph* pGSensorGraph = GSG_GET( TEXT("gsensor.graph") );
	if( pGSensorGraph )
		pGSensorGraph->SetCTS( dwCTS );

	return;
}


VOID CGSensorDlg::SetSensorData( CTextData* pSensorData )
{
	ClGSensorGraph* pGSensorGraph = GSG_GET( TEXT("gsensor.graph") );
	if( pGSensorGraph )
		pGSensorGraph->SetSensorData( pSensorData );

	return;
}
