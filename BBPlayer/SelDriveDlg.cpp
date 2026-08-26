
#include "stdafx.h"
#include "BBPlayer.h"
#include "SelDriveDlg.h"

#include <CLENAW/ClDialog.h>
#include <HELPER/ClLog.H>


IMPLEMENT_DYNAMIC(CSelDriveDlg, ClDialog)

CSelDriveDlg::CSelDriveDlg(CWnd* pParent /*=NULL*/)
	: ClDialog(CSelDriveDlg::IDD, pParent)
{

}

CSelDriveDlg::~CSelDriveDlg()
{
}


void CSelDriveDlg::DoDataExchange(CDataExchange* pDX)
{
	ClDialog::DoDataExchange(pDX);
}


VOID CSelDriveDlg::InitResourceID( VOID )
{
	this->m_pDialogManager->AddResource( TEXT("seldrive.button.ok"),		TYPE_BUTTON,	IDOK				);
	this->m_pDialogManager->AddResource( TEXT("seldrive.button.cancel"),	TYPE_BUTTON,	IDCANCEL			);

	this->m_pDialogManager->AddResource( TEXT("seldrive.text.title"),		TYPE_TEXT,		IDC_SELDRV_TITLE	);
	this->m_pDialogManager->AddResource( TEXT("seldrive.text.drive"),		TYPE_TEXT,		IDC_SELDRV_DRIVE	);

	return;
}


VOID CSelDriveDlg::SetResourceState( VOID )
{
	this->m_pDialogManager->SetResourceState( 0, TEXT("seldrive.button.ok"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( 0, TEXT("seldrive.button.cancel"),	TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( 0, TEXT("seldrive.text.title"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( 0, TEXT("seldrive.text.drive"),		TRUE,	TRUE	);
	this->m_pDialogManager->AdaptResourceState( 0 );
}


BEGIN_MESSAGE_MAP(CSelDriveDlg, ClDialog)
	ON_CBN_DROPDOWN(IDC_SELDRV_COMBDRV, &CSelDriveDlg::OnSelDriveDropDown)
END_MESSAGE_MAP()


BOOL CSelDriveDlg::OnInitDialog()
{
	ClDialog::OnInitDialog();

	if( !this->m_pDialogManager )
	{
		CLD_FL( DF_ERROR, "SelDrive Dialog can not CLUDING! You must call 'OpenCLUDE' before 'DoModal' or 'ShowWindow'." );
		return FALSE;
	}

	CComboBox* pComboDrive = (CComboBox*)GetDlgItem( IDC_SELDRV_COMBDRV );
	pComboDrive->MoveWindow( 170, 60, 210, 100 );

	CString strDrivePath = TEXT(" :\\");
	DWORD drive_info = ::GetLogicalDrives();

	for(int i=0; drive_info; i++)
	{
		if(drive_info & 1)
		{
			strDrivePath.Format( TEXT("%c:\\"),'A' + i );

			switch(::GetDriveType(strDrivePath))
			{
			case DRIVE_REMOVABLE:
				pComboDrive->AddString(strDrivePath);
				break;
			case DRIVE_FIXED:
			case DRIVE_CDROM:
			default: break;
			}
		}
		drive_info >>=1;
	}

	if ( pComboDrive->GetCount() <= 0)
	{
		OnCancel();
	}

	pComboDrive->SetCurSel(0);

	this->SetResourceState();

	return TRUE;
}


void CSelDriveDlg::OnOK()
{
	CComboBox* pComboDrive = (CComboBox*)GetDlgItem( IDC_SELDRV_COMBDRV );

	INT nLen = pComboDrive->GetLBTextLen( pComboDrive->GetCurSel() );
	WCHAR* pwzDrv = new WCHAR[nLen+1];
	pComboDrive->GetLBText( pComboDrive->GetCurSel(), pwzDrv );
	this->m_cstrDrive = pwzDrv;

	if( pwzDrv ) delete [] pwzDrv;

	ClDialog::OnOK();
}

void CSelDriveDlg::OnSelDriveDropDown()
{
	CComboBox* pComboDrive = (CComboBox*)GetDlgItem( IDC_SELDRV_COMBDRV );
	pComboDrive->ResetContent();

	CString strDrivePath = TEXT(" :\\");
	DWORD drive_info = ::GetLogicalDrives();

	for(int i=0; drive_info; i++)
	{
		if(drive_info & 1)
		{
			strDrivePath.Format( TEXT("%c:\\"),'A'+i );

			switch(::GetDriveType(strDrivePath))
			{
			case DRIVE_REMOVABLE:
				pComboDrive->AddString(strDrivePath);
				break;
			case DRIVE_FIXED:
			case DRIVE_CDROM:
			default:
				break;
			}
		}
		drive_info >>=1;
	}

	pComboDrive->SetCurSel(0);

	return;
}
