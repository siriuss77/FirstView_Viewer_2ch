
#include "stdafx.h"
#include "BBPlayer.h"
#include "SelModelDlg.h"

#include <HELPER/ClLog.H>


IMPLEMENT_DYNAMIC(CSelModelDlg, ClDialog)

CSelModelDlg::CSelModelDlg(CWnd* pParent /*=NULL*/)
	: ClDialog(CSelModelDlg::IDD, pParent)
{

}

CSelModelDlg::~CSelModelDlg()
{

}

void CSelModelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


VOID CSelModelDlg::InitResourceID( VOID )
{
	this->m_pDialogManager->AddResource( TEXT("selmodel.button.ok"),		TYPE_BUTTON	,	IDOK					);
	this->m_pDialogManager->AddResource( TEXT("selmodel.button.cancel"),	TYPE_BUTTON	,	IDCANCEL				);
	this->m_pDialogManager->AddResource( TEXT("selmodel.text.title"),		TYPE_TEXT,		IDC_SELMODEL_TEXT_TITLE );
	this->m_pDialogManager->AddResource( TEXT("selmodel.text.comment"),		TYPE_TEXT,		IDC_SELMODEL_TEXT_COMMENT );
	this->m_pDialogManager->AddResource( TEXT("selmodel.text.model"),		TYPE_TEXT,		IDC_SELMODEL_TEXT_MODEL );

	return;
}


VOID CSelModelDlg::SetResourceState( VOID )
{
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("selmodel.button.ok"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("selmodel.button.cancel"),	TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("selmodel.text.title"),	TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("selmodel.text.comment"),	TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("selmodel.text.model"),	TRUE,	TRUE	);

	this->m_pDialogManager->AdaptResourceState( this->m_nCurMode );
}


BEGIN_MESSAGE_MAP(CSelModelDlg, ClDialog)
END_MESSAGE_MAP()


BOOL CSelModelDlg::OnInitDialog()
{
	ClDialog::OnInitDialog();

	if( !this->m_pDialogManager )
	{
		CLD_FL( DF_ERROR, "SELMODEL Dialog can not CLUDING! You must call 'OpenCLUDE' before 'DoModal' or 'ShowWindow'." );
		return FALSE;
	}

	this->SetResourceState();

	CComboBox* pModelCombo = (CComboBox*)GetDlgItem(IDC_SELMODEL_COMBMODEL);
	if( pModelCombo )
	{
		CONST INT nModelCount = ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getModelListSize();
		if( nModelCount <= 0 )
		{
			CLD_FL( DF_ERROR, "Unexpected error. Model name does not existed." );
			return FALSE;
		}

		INT nCount;
		for( nCount = 0; nCount < nModelCount; nCount++ )
			pModelCombo->AddString( ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getModelName(nCount) );

		pModelCombo->SetCurSel( 0 );
	}

	return TRUE;
}


const CString& CSelModelDlg::GetSelectedModel( VOID )
{
	return this->m_cstrSelModel;
}


void CSelModelDlg::OnOK()
{
	CComboBox* pComboDrive = (CComboBox*)GetDlgItem( IDC_SELMODEL_COMBMODEL );
	if( pComboDrive )
	{
		INT nLen = pComboDrive->GetLBTextLen( pComboDrive->GetCurSel() );
		WCHAR* pwzDrv = new WCHAR[nLen+1];
		pComboDrive->GetLBText( pComboDrive->GetCurSel(), pwzDrv );
		this->m_cstrSelModel = pwzDrv;

		if( pwzDrv ) delete [] pwzDrv;
	}

	ClDialog::OnOK();
}
