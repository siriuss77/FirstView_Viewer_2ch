
#include "stdafx.h"
#include "BBPlayer.h"
#include "AboutDlg.h"
#include "NotifyDlg.h"

#include <CLENAW/ClText.h>
#include <CLENAW/ClButton.h>

#include <WinInet.h>
#include <afxinet.h>

#define TIMER_ABOUTDLG_UPDDATE_CHECK	2001

IMPLEMENT_DYNAMIC(CAboutDlg, ClDialog)

CString GetHttp(CString url)
{
	CInternetSession*    pSession = NULL;
     CHttpConnection*    pHttpConnect = NULL;
     CHttpFile*            pHttpFile = NULL;
	  CString strCompleteString;
		 
    try{         
    	 DWORD dwServiceType, dwRet;
		 INTERNET_PORT nPort;
		 CString ServerStr, ObjectStr, DataStr;
		 
        pSession = new CInternetSession;

		AfxParseURL(url, dwServiceType, ServerStr, ObjectStr, nPort);
		
        pHttpConnect = pSession->GetHttpConnection(ServerStr, nPort);

        if(pHttpConnect == NULL)
             throw CString(_T("http connection failed!!!"));

        pHttpFile = pHttpConnect->OpenRequest(CHttpConnection::HTTP_VERB_GET, ObjectStr);

 		CString HeaderStr = TEXT("Content-Type: application/x-www-form-urlencoded");
        pHttpFile->AddRequestHeaders(HeaderStr);

        pHttpFile->SendRequest();
     }
     catch(CInternetException *m_pEx)
     {
         m_pEx->ReportError();
         m_pEx->Delete();
     }

     CString szHeaders;
    pHttpFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, szHeaders);

    BOOL bUTF8mode = szHeaders.Find(_T("charset=UTF-8")) >= 0 || 
        szHeaders.Find(_T("charset=utf-8")) >= 0 ||
        szHeaders.Find(_T("CHARSET=UTF-8")) >= 0 ||
        szHeaders.Find(_T("CHARSET=utf-8")) >= 0;

    DWORD m_dwStatusCode;
     pHttpFile->QueryInfoStatusCode(m_dwStatusCode);
		if(m_dwStatusCode == 200)
    {
        CString strResult;
        while(pHttpFile->ReadString(strResult))
        {
            if (bUTF8mode)
                strCompleteString += CW2T(CA2W((char*)strResult.GetBuffer(), CP_UTF8));
            else
                strCompleteString += CW2T(CA2W((char*)strResult.GetBuffer()));
						
            strResult.ReleaseBuffer();
        }
    }

    if(pHttpFile)
     {
         delete pHttpFile;
         pHttpFile = NULL;
     }
     if(pHttpConnect)
     {
         delete pHttpConnect;
         pHttpConnect = NULL;
     }
     if(pSession)
     {
         pSession->Close();
         delete pSession;
         pSession = NULL;
     }

	return strCompleteString;
}

CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
	: ClDialog(CAboutDlg::IDD, pParent)
{
	m_pCLanguage = NULL;
}

CAboutDlg::~CAboutDlg()
{
}


VOID CAboutDlg::InitResourceID( VOID )
{
	this->m_pDialogManager->AddResource( TEXT("about.text.title"),			TYPE_TEXT,		IDC_ABOUT_TEXT_TITLE		);
	
	this->m_pDialogManager->AddResource( TEXT("about.text.titleviewer"),			TYPE_TEXT,		IDC_ABOUT_TEXT_TITLEVIEWER);
	this->m_pDialogManager->AddResource( TEXT("about.text.viewerver"),		TYPE_TEXT,		IDC_ABOUT_TEXT_VERVIEWER	);
	this->m_pDialogManager->AddResource( TEXT("about.text.viewercomment"),	TYPE_TEXT,		IDC_ABOUT_TEXT_COMMENTVIEWER);


	this->m_pDialogManager->AddResource( TEXT("about.text.titlefirmware"),			TYPE_TEXT,		IDC_ABOUT_TEXT_TITLEFIRMWARE);
	this->m_pDialogManager->AddResource( TEXT("about.text.firmwarever"),	TYPE_TEXT,		IDC_ABOUT_TEXT_VERFIRMWARE	);
	this->m_pDialogManager->AddResource( TEXT("about.text.firmwarecomment"),	TYPE_TEXT,		IDC_ABOUT_TEXT_COMMENTFIRMWARE);
	
	if(m_pCLanguage) {
		this->m_pDialogManager->AddResource( m_pCLanguage->GetButtonText(TEXT("about.button.udateviewer")),	TYPE_BUTTON,	IDC_ABOUT_BTN_UPDATE_VIEWER);
		this->m_pDialogManager->AddResource( m_pCLanguage->GetButtonText(TEXT("about.button.updatefirmware")),	TYPE_BUTTON,	IDC_ABOUT_BTN_UPDATE_FIRMWARE);
		this->m_pDialogManager->AddResource( m_pCLanguage->GetButtonText(TEXT("about.button.website")),	TYPE_BUTTON,	IDC_ABOUT_BTN_GOTOSITE		);
		this->m_pDialogManager->AddResource( TEXT("about.button.exit"),			TYPE_BUTTON,	IDCANCEL						);
		this->m_pDialogManager->AddResource( m_pCLanguage->GetButtonText(TEXT("about.button.ok")),			TYPE_BUTTON,	IDOK						);
	}

	return;
}


VOID CAboutDlg::SetResourceState( VOID )
{
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("about.text.title"),			TRUE, TRUE );
	
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("about.text.titleviewer"),		TRUE, TRUE );
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("about.text.viewerver"),		TRUE, TRUE );
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("about.text.viewercomment"),	TRUE, TRUE );

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("about.text.titlefirmware"),		TRUE, TRUE );
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("about.text.firmwarever"),		TRUE, TRUE );
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("about.text.firmwarecomment"),	TRUE, TRUE );
	
	if(m_pCLanguage) {
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_pCLanguage->GetButtonText(TEXT("about.button.udateviewer")),	TRUE, FALSE);
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_pCLanguage->GetButtonText(TEXT("about.button.updatefirmware")),			TRUE, FALSE );
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_pCLanguage->GetButtonText(TEXT("about.button.website")),	TRUE, FALSE );

		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("about.button.exit"),	TRUE, FALSE );
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_pCLanguage->GetButtonText(TEXT("about.button.ok")),			TRUE, FALSE );
	}
	this->m_pDialogManager->AdaptResourceState( this->m_nCurMode );
}


void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	ClDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAboutDlg, ClDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_ABOUT_BTN_GOTOSITE, &CAboutDlg::OnClickGotosite)
	ON_BN_CLICKED(IDC_ABOUT_BTN_UPDATE_VIEWER, &CAboutDlg::OnClickUpdateViewer)
	ON_BN_CLICKED(IDC_ABOUT_BTN_UPDATE_FIRMWARE, &CAboutDlg::OnClickUpdateFirmware)
END_MESSAGE_MAP()


BOOL CAboutDlg::OnInitDialog()
{
	ClDialog::OnInitDialog();

	this->SetResourceState();

	CString strTextDate(TEXT("Date"));
	CString strTextVersion(TEXT("Version"));
	CString strTextCehcking(TEXT("Checking..."));

	if(m_pCLanguage != NULL){
		this->m_pCLanguage->GetLenguageText(&strTextDate , CLanguage::TEXT_DATE);
		this->m_pCLanguage->GetLenguageText(&strTextVersion, CLanguage::TEXT_VERSION);
		this->m_pCLanguage->GetLenguageText(&strTextCehcking, CLanguage::TEXT_CHECKING);
	}

	ClButton*	pBtn	= BTN_GET( m_pCLanguage->GetButtonText(TEXT("about.button.website")) );
	if(pBtn) pBtn->SetTooltipText( ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getUpdateSite());
	
	//viewer
	ClText* pViewerversion = TXT_GET( TEXT("about.text.viewerver") );
	if( pViewerversion )
	{
		CString cstrViewerVersion	= ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getBBPCLWVersion();
		CString cstrViewerBuildDate	= ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getBBPCLWBuildDate();
		CString cstrVersion;

		cstrVersion = strTextDate + TEXT(" : ") + cstrViewerBuildDate + TEXT("\n") + strTextVersion + TEXT(" : ") + cstrViewerVersion;
		pViewerversion->SetWindowText( cstrVersion );
	}
	ClText* pViewerComment = TXT_GET( TEXT("about.text.viewercomment") );
	if( pViewerComment )
	{
		pViewerComment->SetWindowText( strTextCehcking );
	}

	//firmware
	ClText* pFirmwareVersion = TXT_GET( TEXT("about.text.firmwarever") );
	if( pFirmwareVersion )
	{
		CString cstrVersion;
		cstrVersion.Format( TEXT("%s : %s"),strTextVersion.GetBuffer(), ((CBBPlayerApp*)AfxGetApp())->GetConfigure()->getFirmVersion() );
		pFirmwareVersion->SetWindowText( cstrVersion );
	}

	ClText* pFirmwareComment = TXT_GET( TEXT("about.text.firmwarecomment") );
	if( pFirmwareComment )
	{
		pFirmwareComment->SetWindowText( strTextCehcking );
	}

	this->OnLanguageChange();

	SetTimer(TIMER_ABOUTDLG_UPDDATE_CHECK, 200, NULL);

	this->m_bThreadServerCheckExit= FALSE;
	this->m_pThreadServerCheck = AfxBeginThread( this->ThreadServerCheck,this );
	
	return TRUE;
}

BOOL CAboutDlg::VersionCheck(void)
{
	DWORD state;
	CString strDate;
	CString strServerError;
	bool lastversion = FALSE;
	bool serverError = FALSE;
	ClText* pViewerComment = TXT_GET( TEXT("about.text.viewercomment") );
	ClText* pFirmwareComment = TXT_GET( TEXT("about.text.firmwarecomment") );

	if(m_pCLanguage == NULL)
		return FALSE;
	
	if(InternetGetConnectedState(&state, NULL))
    {
    	this->m_pCLanguage->GetLenguageText(&strServerError , CLanguage::TEXT_SERVER_ERROR);
		//CString cstrWebSite = ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getUpdateSite();
		CString cstrWebViewer = ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getUpdateViewerSite();
		cstrWebViewer = GetHttp(cstrWebViewer);

		CString cstrViewerVersion	= ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getBBPCLWVersion();
		
		if(cstrWebViewer.Find(cstrViewerVersion) >= 0){
			 if(this->m_pCLanguage->GetLenguageText(&strDate , CLanguage::TEXT_LASTVERSION)){
				if( pViewerComment ) 
					pViewerComment->SetWindowText( strDate );
			}
			 lastversion = TRUE;
		}
		else if(!cstrWebViewer.IsEmpty()){
			if(this->m_pCLanguage->GetLenguageText(&strDate , CLanguage::TEXT_UPDATENEED)){

				if( pViewerComment ) 
					pViewerComment->SetWindowText( strDate );
			 }

			m_strViewerFile = ClUtil::STRING::extractSubString(cstrWebViewer, TEXT("FILE="), TEXT("<"));
			
			ClButton*	pBtn	= BTN_GET( m_pCLanguage->GetButtonText(TEXT("about.button.udateviewer")) );
			if(pBtn) {
				pBtn->SetTooltipText( m_strViewerFile);
				pBtn->EnableWindow( TRUE );
			}
		}
		else{
			if( pViewerComment ) 
					pViewerComment->SetWindowText( strServerError );

			if( pFirmwareComment ) 
					pFirmwareComment->SetWindowText( strServerError);
			
			serverError = TRUE;
		}

		//firmware
		CString cstrFirmwareVersion = ((CBBPlayerApp*)AfxGetApp())->GetConfigure()->getFirmVersion();

		if(!cstrFirmwareVersion.IsEmpty()){
			if(!serverError) {
				CString cstrWebFirmware = ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getUpdateFirmwareSite();
				cstrWebFirmware = GetHttp(cstrWebFirmware);
				
				if(cstrWebFirmware.Find(cstrFirmwareVersion) >= 0){
					 if(this->m_pCLanguage->GetLenguageText(&strDate , CLanguage::TEXT_LASTVERSION)){
						if( pFirmwareComment ) 
							pFirmwareComment->SetWindowText( strDate );
					}
				}
				else{
					 lastversion = FALSE;
					if(this->m_pCLanguage->GetLenguageText(&strDate , CLanguage::TEXT_UPDATENEED)){
						if( pFirmwareComment ) 
							pFirmwareComment->SetWindowText( strDate );
					}

					m_strFirmwareFile = ClUtil::STRING::extractSubString(cstrWebFirmware, TEXT("FILE="), TEXT("<"));
					
					ClButton*	pBtn	= BTN_GET( m_pCLanguage->GetButtonText(TEXT("about.button.updatefirmware")) );
					if(pBtn){
						pBtn->SetTooltipText( m_strFirmwareFile);

						pBtn->EnableWindow( TRUE );
					}
				}
			}
		}
		else{
			if( pFirmwareComment ) 
						pFirmwareComment->SetWindowText( TEXT("..."));
		}
	}
	else
	{
		if(this->m_pCLanguage->GetLenguageText(&strDate , CLanguage::TEXT_INTERNETDISCONNECT)){
			if( pViewerComment ) pViewerComment->SetWindowText( strDate );
			if( pFirmwareComment ) pFirmwareComment->SetWindowText( strDate );
		}
	}

	ClButton*	pBtnWebSite	= BTN_GET( m_pCLanguage->GetButtonText(TEXT("about.button.website")) );
	ClButton*	pBtnExit	= BTN_GET( TEXT("about.button.exit"));
	ClButton*	pBtnOk	= BTN_GET( m_pCLanguage->GetButtonText(TEXT("about.button.ok")) );
	if(pBtnWebSite) pBtnWebSite->EnableWindow( TRUE );
	if(pBtnExit) pBtnExit->EnableWindow( TRUE );
	if(pBtnOk) pBtnOk->EnableWindow( TRUE );
	
	return FALSE;
}

void CAboutDlg::OnLanguageChange(void)
{
	CString strText;

	if(m_pCLanguage == NULL)
		return;

	SetItemText(IDC_ABOUT_TEXT_TITLE, CLanguage::TEXT_INFO);

	if(this->m_pCLanguage->GetLenguageText(&strText , CLanguage::TEXT_VIEWER))
		SetItemText(IDC_ABOUT_TEXT_TITLEVIEWER, CLanguage::TEXT_INFOVERSION, strText);

	if(this->m_pCLanguage->GetLenguageText(&strText , CLanguage::TEXT_FIRMWARE));
		SetItemText(IDC_ABOUT_TEXT_TITLEFIRMWARE, CLanguage::TEXT_INFOVERSION, strText);
		
}

void  CAboutDlg::SetItemText(int nIDDlgItem, UINT resource, CString index_str)
{
	CString strText;
	if(m_pCLanguage)
	{
		if(this->m_pCLanguage->GetLenguageText(&strText , resource))
			this->GetDlgItem(nIDDlgItem)->SetWindowText(index_str + strText);
	}
}

void CAboutDlg::OnClickGotosite()
{
	DWORD dwConnectionTypes;
	if( !InternetGetConnectedState(&dwConnectionTypes, 0) )
	{
		OPEN_NOTIFY_DIALOG( NOTIFY_ERR_NO_INTERNET , m_pCLanguage);
		return;
	}

	CString cstrWebSite = ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getUpdateSite();
	::ShellExecute( NULL, TEXT("open"), cstrWebSite, NULL, NULL, SW_SHOWNORMAL );

	return;
}

void CAboutDlg::OnClickUpdateViewer()
{
	DWORD dwConnectionTypes;
	if( !InternetGetConnectedState(&dwConnectionTypes, 0) )
	{
		OPEN_NOTIFY_DIALOG( NOTIFY_ERR_NO_INTERNET , m_pCLanguage);
		return;
	}

	if(!m_strViewerFile.IsEmpty()){
		::ShellExecute( NULL, TEXT("open"), m_strViewerFile, NULL, NULL, SW_SHOWNORMAL );
	}
	return;
}

void CAboutDlg::OnClickUpdateFirmware()
{
	DWORD dwConnectionTypes;
	if( !InternetGetConnectedState(&dwConnectionTypes, 0) )
	{
		OPEN_NOTIFY_DIALOG( NOTIFY_ERR_NO_INTERNET , m_pCLanguage);
		return;
	}

	if(!m_strFirmwareFile.IsEmpty()){
		::ShellExecute( NULL, TEXT("open"), m_strFirmwareFile, NULL, NULL, SW_SHOWNORMAL );
	}
	return;
}

void CAboutDlg::OnTimer( UINT_PTR nIDEvent )
{
	
	if(nIDEvent == TIMER_ABOUTDLG_UPDDATE_CHECK)
	{
		if(m_bThreadServerCheckExit){
			this->KillTimer( TIMER_ABOUTDLG_UPDDATE_CHECK );
		}
	}

	ClDialog::OnTimer(nIDEvent);
}

UINT CAboutDlg::ThreadServerCheck(void* pArg)
{
	CAboutDlg* pDlg = (CAboutDlg*)pArg;

	if (pDlg == NULL)
	{
		pDlg->m_pThreadServerCheck = NULL;
		return 0;
	}

	pDlg->VersionCheck();
	pDlg->m_bThreadServerCheckExit=TRUE;
}

