// LogDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BBPlayer.h"
#include "LogDlg.h"
#include "NotifyDlg.h"
#include "BBoxUtil.h"
#include <CLUDE/version.h>
#include <BLACKBOX/blackbox_log.h>
#include <CLENAW/ClText.h>
#include <CLENAW/ClListCtrl.h>

#include <WinInet.h>


IMPLEMENT_DYNAMIC(CLogDlg, ClDialog)

CLogDlg::CLogDlg(CWnd* pParent /*=NULL*/)
	: ClDialog(CLogDlg::IDD, pParent)
{
	this->m_cstrLogFilePath = TEXT("");
	this->m_pCLanguage = NULL;
}

CLogDlg::~CLogDlg()
{
}

VOID CLogDlg::InitResourceID( VOID )
{
	this->m_pDialogManager->AddResource( TEXT("log.text.title"),			TYPE_TEXT,		IDC_LOG_TEXT_TITLE		);
	this->m_pDialogManager->AddResource( TEXT("log.listbox.loglist"),		TYPE_LISTBOX,	IDC_LOG_LIST		);
	this->m_pDialogManager->AddResource( m_pCLanguage->GetButtonText(TEXT("log.button.save")),	TYPE_BUTTON,	IDC_LOG_BTN_SAVE		);
	this->m_pDialogManager->AddResource( TEXT("log.button.ok"),			TYPE_BUTTON,	IDOK						);
	this->m_pDialogManager->AddResource( m_pCLanguage->GetButtonText(TEXT("log.button.cancel")),			TYPE_BUTTON,	IDCANCEL				);

	return;
}


VOID CLogDlg::SetResourceState( VOID )
{
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("log.text.title"),			TRUE, TRUE );
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("log.listbox.loglist"),		TRUE, TRUE );
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_pCLanguage->GetButtonText(TEXT("log.button.save")),		TRUE, TRUE );
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("log.button.ok"),			TRUE, TRUE );
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_pCLanguage->GetButtonText(TEXT("log.button.cancel")),			TRUE, TRUE );
	this->m_pDialogManager->AdaptResourceState( this->m_nCurMode );
}
void CLogDlg::DoDataExchange(CDataExchange* pDX)
{
	ClDialog::DoDataExchange(pDX);
}

BOOL CLogDlg::OnInitDialog()
{
	ClDialog::OnInitDialog();

	this->SetCurrentMode( 0 );
	this->SetResourceState();

	ClListCtrl*		pLogList = LST_GET( TEXT("log.listbox.loglist") );
	if(pLogList)
	{
		LPST_LOG_ITEM 	p_read_logitem = NULL;
		
		UINT ulLogItemCount = 0;
		INT nItemCount = 0;
		
		pLogList->SetRedraw( FALSE );
		pLogList->DeleteAllItems();
		
		ak_log_init(this->m_cstrLogFilePath);
		ulLogItemCount = ak_log_get_itemcount();

		
		if(ulLogItemCount)
		{
			UINT i = 0;
			TIME_ZONE_INFORMATION TimeZoneInformation;

			GetTimeZoneInformation( &TimeZoneInformation ); 
			p_read_logitem = (LPST_LOG_ITEM) new BYTE[(sizeof(ST_LOG_ITEM) * ulLogItemCount)];

			ak_log_get_item(0, ulLogItemCount, p_read_logitem);
			
			for(i = 0; i < ulLogItemCount; i++)
			{
				if(p_read_logitem[i].time == 0xFFFFFFFF)
				{
					DEBUGMSG( 1,("	[%s] LOG READ ERROR!!!! (count:%d)\n",__FUNCTION__, i));
				}
				else
				{
					CString cstrIndex;
					char szbuff[256];
					TCHAR lpwzBuff[256];

					cstrIndex.Format(TEXT("%d"), nItemCount);
					pLogList->InsertItem( nItemCount, cstrIndex );
					//tony, for sort
					//pLogList->SetItemData(nItemCount , nItemCount);

					CBBoxUtil::GetTimeToText((time_t)(p_read_logitem[i].time + (TimeZoneInformation.Bias * 60)), szbuff);
					if(ClUtil::STRING::convertChar2Wchar(lpwzBuff, szbuff, 256))
						pLogList->SetItemText( nItemCount, 1, lpwzBuff);
					
					ak_log_get_item_type_string(szbuff, &p_read_logitem[i]);
					if(ClUtil::STRING::convertChar2Wchar(lpwzBuff, szbuff, 256))
						pLogList->SetItemText( nItemCount, 2, lpwzBuff);

					if(ClUtil::STRING::convertChar2Wchar(lpwzBuff,  (char *)p_read_logitem[i].data.byte, 256))
						pLogList->SetItemText( nItemCount, 3, lpwzBuff);
					
					nItemCount++;
				}
				
				
			}

			delete []p_read_logitem;
		}

		ak_log_release(0);

		pLogList->SetRedraw( TRUE );
	}
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CLogDlg, ClDialog)
	ON_BN_CLICKED(IDC_LOG_BTN_SAVE, &CLogDlg::OnBnClickedLogSave)
END_MESSAGE_MAP()


void CLogDlg::SetLogFile( CString cstrLogFile )
{
	this->m_cstrLogFilePath = cstrLogFile;

	return;
}

// CLogDlg message handlers

void CLogDlg::OnBnClickedLogSave()
{
	ClListCtrl*		pLogList = LST_GET( TEXT("log.listbox.loglist") );

	if(pLogList)
	{
		UINT ulItemCount = pLogList->GetItemCount();

		CString strFilter = TEXT("jpeg (*.csv)|*.csv||");
		CString csFileName;

		std::wstring wtsFileName = ClUtil::PATH::getFileNameSpecW( this->m_cstrLogFilePath);
		csFileName.Format(TEXT("%s_%s_log_%d"), FORMAT_LABEL, wtsFileName.c_str(), ulItemCount);
		csFileName.Append(TEXT(".csv"));
		CFileDialog dlg(false, NULL, csFileName, OFN_OVERWRITEPROMPT, TEXT("Excel file (*.csv)|*.csv||"), this);

		if ( dlg.DoModal() == IDOK)
		{
			UINT i;
			CString strLog;
			CString strLogData;
			CString strExtention = dlg.GetFileExt();
			CString strPath = dlg.GetPathName();
			char strBuff[256];
			
			
			ClUtil::STRING::convertWchar2Char(strBuff, strPath.GetBuffer(),  256);
			FILE* fp = fopen(strBuff,"w+");

			fprintf(fp,"No,Date,Log Type, Log Data\n");

 			for( i = 0; i < ulItemCount; i++)
 			{		
 				strLog.Format(TEXT("%s,%s,%s,"), pLogList->GetItemText( i, 0 ), pLogList->GetItemText( i, 1 ),pLogList->GetItemText( i, 2 ));
				strLogData = pLogList->GetItemText( i, 3);
				strLog += strLogData.Left(LOG_ITEM_DATA_SIZE);
				strLog += TEXT("\n");
				
 				if(ClUtil::STRING::convertWchar2Char(strBuff, strLog.GetBuffer(),  256))
	 				fprintf(fp,strBuff);
 			}
			 
			fclose(fp);

			strPath.Append(TEXT(" 파일을 저장 하였습니다."));
			AfxMessageBox(strPath, MB_ICONINFORMATION);
		}
	}
}

