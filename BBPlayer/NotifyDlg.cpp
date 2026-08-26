
#include "stdafx.h"
#include "BBPlayer.h"
#include "NotifyDlg.h"

#include <CLENAW/ClText.h>
#include <HELPER/ClLog.H>


IMPLEMENT_DYNAMIC(CNotifyDlg, ClDialog)

CNotifyDlg::CNotifyDlg(CWnd* pParent /*=NULL*/)
	: ClDialog(CNotifyDlg::IDD, pParent)
{
	this->m_notifyStyle = STYLE_NORMAL;
	this->m_pCLanguage = NULL;
}

CNotifyDlg::~CNotifyDlg()
{
}

void CNotifyDlg::DoDataExchange(CDataExchange* pDX)
{
	ClDialog::DoDataExchange(pDX);
}


VOID CNotifyDlg::InitResourceID( VOID )
{
	if(m_pCLanguage) {
		this->m_pDialogManager->AddResource( m_pCLanguage->GetButtonText(TEXT("notify.button.ok")),			TYPE_BUTTON,	IDOK					);
		this->m_pDialogManager->AddResource( m_pCLanguage->GetButtonText(TEXT("notify.button.selok")),		TYPE_BUTTON,	IDC_NOTIFY_BTN_SELOK	);
		this->m_pDialogManager->AddResource( m_pCLanguage->GetButtonText(TEXT("notify.button.selcancel")),	TYPE_BUTTON,	IDC_NOTIFY_BTN_SELCANCEL);
	}
	this->m_pDialogManager->AddResource( TEXT("notify.text.title"),			TYPE_TEXT,		IDC_NOTIFY_TXT_TITLE	);
	this->m_pDialogManager->AddResource( TEXT("notify.text.comment"),		TYPE_TEXT,		IDC_NOTIFY_TXT_COMMENT	);
}


VOID CNotifyDlg::SetResourceState( VOID )
{
	if( this->m_notifyStyle == STYLE_OKCANCEL )
	{
		this->m_nCurMode = 1;
		
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("notify.text.title"),			TRUE, TRUE	);
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("notify.text.comment"),		TRUE, TRUE	);

		if(m_pCLanguage) {
			this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_pCLanguage->GetButtonText(TEXT("notify.button.selok")),		TRUE, TRUE	);
			this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_pCLanguage->GetButtonText(TEXT("notify.button.selcancel")),	TRUE, TRUE	);
		}
		this->m_pDialogManager->AdaptResourceState( this->m_nCurMode );
	}
	else
	{
		this->m_nCurMode = 0;
		if(m_pCLanguage) {
			this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_pCLanguage->GetButtonText(TEXT("notify.button.ok")),	TRUE, TRUE	);
		}
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("notify.text.title"),	TRUE, TRUE	);
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("notify.text.comment"),TRUE, TRUE	);

		this->m_pDialogManager->AdaptResourceState( this->m_nCurMode );
	}

	return;
}


BOOL CNotifyDlg::SetNotifyStyle( NOTIFY_STYLE style )
{
	if( style == STYLE_INVALID )
		return FALSE;

	this->m_notifyStyle = style;

	return TRUE;
}


BOOL CNotifyDlg::SetTitle( CString cstrTitle )
{
	m_cstrTitle = cstrTitle;
	return TRUE;
}


BOOL CNotifyDlg::SetComment( CString cstrComment )
{
	m_cstrComment = cstrComment;

	return TRUE;
}


BEGIN_MESSAGE_MAP(CNotifyDlg, ClDialog)
	ON_BN_CLICKED(IDC_NOTIFY_BTN_SELCANCEL,		&CNotifyDlg::OnClickSelCancel	)
	ON_BN_CLICKED(IDC_NOTIFY_BTN_SELOK,			&CNotifyDlg::OnClickSelOK		)
END_MESSAGE_MAP()


BOOL CNotifyDlg::OnInitDialog()
{
	ClDialog::OnInitDialog();

	if( !this->m_pDialogManager )
	{
		CLD_FL( DF_ERROR, "Notification Dialog can not CLUDING! You must call 'OpenCLUDE' before 'DoModal' or 'ShowWindow'." );
		return FALSE;
	}

	ClText* pTxtTitle = TXT_GET( TEXT("notify.text.title") );
	if( pTxtTitle )
		pTxtTitle->SetWindowText( m_cstrTitle );

	ClText* pTxtComment = TXT_GET( TEXT("notify.text.comment") );
	if( pTxtComment )
		pTxtComment->SetWindowText( m_cstrComment );

	this->SetResourceState();

	return TRUE;
}


void CNotifyDlg::OnClickSelCancel()
{
	this->EndDialog( IDCANCEL );
}


void CNotifyDlg::OnClickSelOK()
{
	this->EndDialog( IDOK );
}
