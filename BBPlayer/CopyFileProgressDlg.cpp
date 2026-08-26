// CopyFileProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BBPlayer.h"
#include "CopyFileProgressDlg.h"

#include <CLUDE/version.h>

//#define DEF_AVI_FILE_MERGE

#define	AUTO_PROGRESS_TIMER		1000

#define PROGRESSVALUE(a, b) (int)((double)(a*100)/(double)b)

BOOL IsFileExist(LPCTSTR lpszPath)
{
	if (_taccess(lpszPath, 0) != -1)
		return TRUE;

	return FALSE;
}

DWORD CALLBACK CopyProgressRoutine(LARGE_INTEGER TotalFileSize,
								   LARGE_INTEGER TotalBytesTransferred,
								   LARGE_INTEGER StreamSize,
								   LARGE_INTEGER StreamBytesTransferred,
								   DWORD dwStreamNumber,
								   DWORD dwCallbackReason,
								   HANDLE hSourceFile,
								   HANDLE hDestinationFile,
								   LPVOID lpData)
{
	CCopyFileProgressDlg* pDlg = (CCopyFileProgressDlg*)lpData;
	if (!pDlg || pDlg->m_bStop)
		return PROGRESS_STOP;

#ifndef DEF_AVI_FILE_MERGE
	pDlg->UpdateProgressbar(PROGRESSVALUE(TotalBytesTransferred.QuadPart, TotalFileSize.QuadPart));
#endif

	if (PROGRESSVALUE(TotalBytesTransferred.QuadPart, TotalFileSize.QuadPart) < 100)
		return PROGRESS_CONTINUE;

	return PROGRESS_STOP;
}

DWORD WINAPI CopyFileThread(LPVOID lParam)
{
	CCopyFileProgressDlg* pDlg = (CCopyFileProgressDlg*)lParam;
	if (!pDlg)
		return 0;

	BOOL bCancel = FALSE;
	BOOL bCheckedItemCopy = FALSE;
	//::CopyFileEx(pDlg->m_strSrcPath, pDlg->m_strDestPath, &CopyProgressRoutine, pDlg, &bCancel, /*COPY_FILE_FAIL_IF_EXISTS*/0);
	int nListupIndex = 0;
	int checkIndex = 0;
	int nCount = 0;
	int nFileCount = 0;
	int nChannel = 2;
	int nListupFiles = pDlg->mp_fileManager->getListupFiles();
	CString cstrListFileName;
	int nPos;

#ifdef DEF_AVI_FILE_MERGE
	pDlg->SetTimer(AUTO_PROGRESS_TIMER, 10, NULL);
	pDlg->mp_fileManager->m_listAviFilePath.clear();
#endif

	if(pDlg->m_nCopyFiles < nListupFiles)
		nListupFiles = pDlg->m_nListupIndex + pDlg->m_nCopyFiles; 

	if(pDlg->mp_FileList){
		nCount = pDlg->mp_FileList->GetItemCount();
		pDlg->m_nListupIndex = 0;
		nListupFiles = 0;
		for(int i=0; i < nCount; i++){
			if(pDlg->mp_FileList->GetCheck(i))
				nListupFiles++;
		}
		
		bCheckedItemCopy = TRUE;
	}

	for(checkIndex = 0, nListupIndex = pDlg->m_nListupIndex; nListupIndex < nListupFiles; nListupIndex++){
		if (pDlg->m_bStop || pDlg->mp_fileManager == NULL)
			return 0;
		
		if(bCheckedItemCopy){
			for(; checkIndex < nCount; checkIndex++){
				if(pDlg->mp_FileList->GetCheck(checkIndex))
					break;
			}

			if(checkIndex >= nCount)
				goto exit;
		}
		else 
			checkIndex = nListupIndex;

		DWORD init_indexno = checkIndex;
		BOOL bFileSaveOk = FALSE;  //원본파일 삭제   jun 170406
		CString cstrNewFileName;
		
		
		if( pDlg->mp_FileList)
			init_indexno = pDlg->mp_FileList->GetItemData(checkIndex);		

			if(pDlg->m_bIsRemovalNxfs){
				CWBCFileManager::PSFILEINFO pFileInfo = pDlg->mp_fileManager->getListupFileInfo( init_indexno );
			    if( pFileInfo == NULL )
			        break;

				cstrListFileName = pFileInfo->m_cstrFullPath;
				nPos = cstrListFileName.ReverseFind( '.' );

				cstrNewFileName = pDlg->m_strDestPath;
		    	cstrNewFileName	+= TEXT("\\");
				
			    cstrNewFileName += cstrListFileName.Left( nPos );
			    cstrNewFileName += TEXT(".");
			    cstrNewFileName += TEXT("avi");

				if(bCheckedItemCopy)
					pDlg->m_strFileName.Format(TEXT("%d/%d "), nListupIndex + 1,  nListupFiles);
				else
					 pDlg->m_strFileName.Format(TEXT("%d/%d "), nListupIndex + 1 - pDlg->m_nListupIndex,  nListupFiles - pDlg->m_nListupIndex);
				 //pDlg->UpdateData();
				 CWnd* pWndPage = pDlg->GetDlgItem(IDC_COPYDLG_STATIC_PAGE);
				 if(pWndPage)
				 	pWndPage->SetWindowText(pDlg->m_strFileName);
				 
				 CWnd* pWnd = pDlg->GetDlgItem(IDC_FILENAME);
				if (pWnd)
				 	pWnd->SetWindowText(cstrNewFileName);
				
				pDlg->mp_fileManager->copy2Desktop( cstrNewFileName, pFileInfo, &CopyProgressRoutine, pDlg);				
#ifdef DEF_AVI_FILE_MERGE
				pDlg->mp_fileManager->m_listAviFilePath.push_back(cstrNewFileName);
#endif

			}
			else {
#ifdef DEF_AVI_FILE_MERGE
				pDlg->mp_fileManager->m_listAviFilePath.push_back(pDlg->mp_FileList->GetItemText( checkIndex, 3 ));
#else				
				SHFILEOPSTRUCT stFileOP;
				CString cstrSourceFilePath = pDlg->mp_FileList->GetItemText( checkIndex, 3 );
				cstrListFileName = pDlg->mp_FileList->GetItemText( checkIndex, 1 );
				nPos = cstrListFileName.ReverseFind( '.' );

				cstrSourceFilePath += TEXT('\0');
		
				cstrNewFileName = pDlg->m_strDestPath;
		    	      cstrNewFileName	+= TEXT("\\");
				
			    cstrNewFileName += cstrListFileName.Left( nPos );  //원본만 그래로 백업시 삭제 
			    cstrNewFileName += TEXT(".");  //원본만 그래로 백업시 삭제 
			    cstrNewFileName += TEXT("avi");  //원본만 그래로 백업시 삭제 

				if(bCheckedItemCopy)
					pDlg->m_strFileName.Format(TEXT("%d/%d "), nListupIndex + 1,  nListupFiles);
				else
					 pDlg->m_strFileName.Format(TEXT("%d/%d "), nListupIndex + 1 - pDlg->m_nListupIndex,  nListupFiles - pDlg->m_nListupIndex);
				 //pDlg->UpdateData();
				 CWnd* pWndPage = pDlg->GetDlgItem(IDC_COPYDLG_STATIC_PAGE);
				 if(pWndPage)
				 	pWndPage->SetWindowText(pDlg->m_strFileName);
				 
				 CWnd* pWnd = pDlg->GetDlgItem(IDC_FILENAME);
				if (pWnd)
				 	pWnd->SetWindowText(cstrNewFileName);
				

				bool bLastFileDelete = false;
			 	LARGE_INTEGER TotalBytesTransferred;
				LARGE_INTEGER TotalFileSize;
				LARGE_INTEGER StreamSize = {0,};
				LARGE_INTEGER StreamBytesTransferred = {0,};

				TotalBytesTransferred.QuadPart = nListupIndex;

				TotalFileSize.QuadPart = nListupFiles;
				 if( CopyProgressRoutine(TotalFileSize,
								   TotalBytesTransferred,
								   StreamSize,
								   StreamBytesTransferred,
								   0,
								   0,
								   NULL,
								   NULL,
								   pDlg) != PROGRESS_CONTINUE){
					bLastFileDelete = true;
				 }

				//if(bLastFileDelete)
					//CFile::Remove(cstrNewFileName.GetBuffer());

				if(cstrSourceFilePath.Compare(cstrNewFileName) ){
					ZeroMemory( &stFileOP, sizeof(SHFILEOPSTRUCT) );
					stFileOP.hwnd	= pDlg->GetSafeHwnd();
					stFileOP.wFunc	= FO_COPY;
					stFileOP.fFlags = FOF_NOCONFIRMATION;
					stFileOP.pFrom	= cstrSourceFilePath;
					stFileOP.pTo	= cstrNewFileName;

					if( SHFileOperation(&stFileOP) )
					{
						printf( "File copying failed." );
						//OPEN_NOTIFY_DIALOG( NOTIFY_ERR_COPY_FILE ,&this->m_pUIControl->m_Language);
					}				
				}			
#endif				
			}
			checkIndex++;

#if 0
		if (IsFileExist(m_strDestPath))
		{
			if (IDNO == ::AfxMessageBox(_T("이미 같은 파일이 존재합니다. 덮어씌우시겠습니까?"), MB_YESNO))
			{
				PostMessage(WM_CLOSE);
				return TRUE;
			}
		}
#endif		
		//pDlg->UpdateProgressbar(PROGRESSVALUE(nListupIndex + 1 - pDlg->m_nListupIndex , nListupFiles - pDlg->m_nListupIndex));
		if(pDlg->m_bStop)
			break;
#ifndef DEF_AVI_FILE_MERGE
		bFileSaveOk = pDlg->mp_fileManager->fileSeparate(cstrNewFileName, TEXT("avi"), pDlg->m_bIsRemovalNxfs);  //원본파일 삭제   jun 170406


#ifdef REAR_CAM_OFF //원본파일 삭제   jun 170406   180205
		if(bFileSaveOk){
			CFile::Remove(cstrNewFileName);
		}
 #else
 #endif
 #endif
	}

#ifdef DEF_AVI_FILE_MERGE
	pDlg->mp_fileManager->fileMerge(pDlg->m_strDestPath, pDlg->m_bIsRemovalNxfs);
	pDlg->mp_fileManager->m_listAviFilePath.clear();
	pDlg->KillTimer(AUTO_PROGRESS_TIMER);
#endif

exit:
	
	if (pDlg->m_bStop)
		return 0;

	if (pDlg && pDlg->GetSafeHwnd())
		pDlg->PostMessage(WM_CLOSE);

	return 0;
}


// CCopyFileProgressDlg dialog

IMPLEMENT_DYNAMIC(CCopyFileProgressDlg, CDialog)

CCopyFileProgressDlg::CCopyFileProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCopyFileProgressDlg::IDD, pParent)
	, m_strFileName(_T(""))
	, m_strSrcPath(_T(""))
	, m_strDestPath(_T(""))
	, m_hThread(NULL)
	, m_bStop(FALSE)
{
	m_nCopyFiles = 0;
	m_nListupIndex = 0;
	mp_fileManager = NULL;
	m_pCLanguage = NULL;
	mp_FileList = NULL;
	m_bIsRemovalNxfs = FALSE;
	m_nProgressPos = 0;
}

CCopyFileProgressDlg::~CCopyFileProgressDlg()
{
	ExitCopyFileThread();
}

void CCopyFileProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COPYPROGRESS, m_ProgressCtrl);
}


BEGIN_MESSAGE_MAP(CCopyFileProgressDlg, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CCopyFileProgressDlg message handlers

void CCopyFileProgressDlg::OnCancel()
{
	m_bStop = TRUE;

	ExitCopyFileThread();
	CDialog::OnCancel();
}

BOOL CCopyFileProgressDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (mp_fileManager == NULL 
		|| m_nCopyFiles == 0
		|| (m_bIsRemovalNxfs && mp_fileManager->getListupFiles() == 0)
		|| (m_nCopyFiles > 1 && m_strDestPath.IsEmpty()) )
	{
		PostMessage(WM_CLOSE);
		return TRUE;
	}

	SetTimer(1, 100, NULL);
	m_ProgressCtrl.SetRange(0, 100);

	 CWnd* pWnd = this->GetDlgItem(IDCANCEL);
	 if(pWnd && m_pCLanguage){
	 	CString strCancel;
	 	if(m_pCLanguage->GetLenguageText(&strCancel, CLanguage::TEXT_CANCEL))
	 		pWnd->SetWindowText(strCancel);
	 }

	 if(m_pCLanguage){
	 	CString strTitle;
	 	if(m_pCLanguage->GetLenguageText(&strTitle, CLanguage::TEXT_COPYING_FILES))
	 		this->SetWindowText(strTitle);
	 }
	 
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCopyFileProgressDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (1 == nIDEvent)
	{
		KillTimer(1);
		RunCopyFileThread();
	}
	else if( nIDEvent == AUTO_PROGRESS_TIMER ){
		UpdateProgressbar(PROGRESSVALUE((m_nProgressPos % 100), 70));
		m_nProgressPos++;
	}
	CDialog::OnTimer(nIDEvent);
}

void CCopyFileProgressDlg::RunCopyFileThread()
{
	// int this example, we set it up so that child processes do not
	// inherit security attributes...

	CWnd* pWnd = GetDlgItem(IDC_FILENAME);
	if (pWnd)
	{
		CString strFileName;

		TCHAR szFName[_MAX_FNAME] = {0};
		TCHAR szExt[_MAX_EXT] = {0};

		_tsplitpath_s((LPCTSTR)m_strDestPath, NULL, 0, NULL, 0, szFName, _countof(szFName), szExt, _countof(szExt));

		strFileName = szFName;
		strFileName += szExt;

		pWnd->SetWindowText(strFileName);
	}

	DWORD dwThreadId;
	m_hThread = CreateThread(NULL,				// pointer to security attributes
								0,              // initial thread stack size
								CopyFileThread,	// pointer to thread function
								this,			// argument for new thread
								0,              // creation flags
								&dwThreadId		// pointer to receive thread ID
								);

	// now you can do things like:  WaitForSingleObject(hThread, INFINITE);
	// or: GetExitCodeThread(hThread, &dwExitCode);
}

void CCopyFileProgressDlg::ExitCopyFileThread()
{
	if (m_hThread)
	{
		DWORD dwExitCode = 0;
		if (::GetExitCodeThread(m_hThread, &dwExitCode))
		{
			if (STILL_ACTIVE != dwExitCode)
			{
				DWORD dwRet = ::WaitForSingleObject(m_hThread, 5000);
				if (WAIT_TIMEOUT == dwRet || WAIT_FAILED == dwRet)
				{
					::TerminateThread(m_hThread, dwExitCode);
				}
			}
		}
		::CloseHandle(m_hThread);
		m_hThread = NULL;
	}
}

void CCopyFileProgressDlg::UpdateProgressbar(int nDoing)
{
	if (!m_ProgressCtrl.GetSafeHwnd())
		return;

	m_ProgressCtrl.SetPos(nDoing);
}
