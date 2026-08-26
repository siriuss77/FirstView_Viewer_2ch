// BBPlayer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "BBPlayer.h"
#include "BBPlayerDlg.h"

#include <HELPER/ClImagePool.H>
#include <HELPER/ClLog.H>

#ifdef _DEBUG
#include <conio.h>
#define new DEBUG_NEW
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

#if defined(USING_CLUDE_PACK)
#include <CLUDE/ClCludeUnpacker.H>
#endif



// CBBPlayerApp

BEGIN_MESSAGE_MAP(CBBPlayerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CBBPlayerApp construction

CBBPlayerApp::CBBPlayerApp()
{
}


// The one and only CBBPlayerApp object

CBBPlayerApp theApp;
HANDLE hInitialMutex;

// CBBPlayerApp initialization

BOOL CBBPlayerApp::InitInstance()
{
	WNDCLASS wndClass;
	::GetClassInfo( AfxGetInstanceHandle(), MAKEINTRESOURCE(32770), &wndClass );
	wndClass.lpszClassName = STR_CLASS_NAME;
	
	if( !::RegisterClass(&wndClass) )
	{
		_ASSERTE(! __FUNCTION__ " Failed to register window class");
		return FALSE;
	}

	hInitialMutex = CreateMutex( NULL, FALSE, STR_MUTEX_NAME );
	if( hInitialMutex == NULL )
		return FALSE;

	if( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		HWND hWndPrev = ::FindWindow( STR_CLASS_NAME, NULL );
		if( hWndPrev ) {

			::SetForegroundWindow( hWndPrev );

#if 1
			{
				int iCnt = 0;
	 			LPWSTR* pStr = CommandLineToArgvW( GetCommandLine(), &iCnt);

				if(iCnt) {
					//::SendMessage(hWndPrev, WM_USER_ADDFILE, (WPARAM)iCnt, (LPARAM)pStr);

					COPYDATASTRUCT cds;
					TCHAR file_path[256];
					lstrcpy(file_path, pStr[1]);
					cds.dwData = 27001; 
					cds.lpData = (void *)file_path;
					cds.cbData = sizeof(file_path);
					::SendMessage(hWndPrev, WM_COPYDATA, NULL, (LPARAM)&cds);
				}
				
				LocalFree(pStr);
			}
#endif			
		}

		CloseHandle( hInitialMutex );
		hInitialMutex = NULL;
		return TRUE;
	}

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup( &m_gdiplusToken, &gdiplusStartupInput, NULL );

	// To support multi-language.
    //CWBCConfigure::getInstance()->init();
	
	CBBPlayerDlg dlg;
	const char* lpszDlgXml = NULL;
	std::wstring wtsBaseImagePath;

#if defined(_DEBUG)
	DWORD dwTickStart = GetTickCount();
	DWORD dwTickEnd = 0;
#endif

#if defined(USING_CLUDE_PACK)

	ClCludeUnPacker* pCludePack = new ClCludeUnPacker();

	if( !pCludePack->init() )
	{
		CLD_FL( DF_ERROR, "Can not load CLUDE.PACK" );
		goto LOADING_FAILED;
	}

	if( !this->m_AppGlobalSetting.loadCludePack(pCludePack) )
	{
		CLD_FL( DF_ERROR, "Can not load application's information in CLUDE.PACK" );
		goto LOADING_FAILED;
	}

	if( !ClImagePool::getInstance()->loadCludePack(pCludePack) )
	{
		CLD_FL( DF_ERROR, "Can not load image resource in CLUDE.PACK" );
		goto LOADING_FAILED;
	}

	if( pCludePack ) delete pCludePack;

#else

	if( !this->m_AppGlobalSetting.loadCludeFile( "" ) )
	{
		CLD_FL( DF_ERROR, "Can not load about application's information." );
		goto LOADING_FAILED;
	}

	wtsBaseImagePath += this->m_AppGlobalSetting.getImageBasePath();
	wtsBaseImagePath += TEXT("\\");
	ClImagePool::getInstance()->setBasePath( wtsBaseImagePath );

#endif

	m_pMainWnd = &dlg;

	lpszDlgXml = this->m_AppGlobalSetting.getXml( TEXT("MAIN") );
	if( !lpszDlgXml || !strlen(lpszDlgXml) )
	{
		CLD_FL( DF_ERROR, "XML file not ready(Target name : MAIN)" );
		return FALSE;
	}

	if( !dlg.OpenCludeBuffer(lpszDlgXml) )
	{
		CLD_FL( DF_FATAL, "Main Dialog can not CLUDING!" );
		return FALSE;
	}

#if defined(_DEBUG)
	dwTickEnd = GetTickCount();
	CLD_FL( DF_REPORT, "CLENAW frameworks has been started...(loading : %d ms)", dwTickEnd - dwTickStart );
#endif

	dlg.DoModal();

	//CWBCConfigure::releaseInstance();

LOADING_FAILED:

#if defined(_DEBUG)
	printf("\n PRESS ANY KEY TO CLOSE DEBUG WINDOW.");
	_getch();
#endif

	return FALSE;
}


int CBBPlayerApp::ExitInstance( VOID )
{
	ClImagePool::releaseInstance();
	GdiplusShutdown(m_gdiplusToken);

	return __super::ExitInstance();
}


ClCludeApp* CBBPlayerApp::GetGlobalSetting( VOID )
{
	return &this->m_AppGlobalSetting;
}


CBBConfigure* CBBPlayerApp::GetConfigure( VOID )
{
	return &this->m_Configure;
}
