// BBPlayerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BBPlayer.h"
#include "BBPlayerDlg.h"

#include <Mmsystem.h>
#include <WinInet.h>

#include <BLACKBOX/HttpUrl.h>

#include <shlobj.h>//for knownFolder
//#include <shlwapi.h>
//#pragma comment(lib, "shlwapi")

#include <CLUDE/version.h>
#include <CLENAW/ClImage.h>
#include <CLENAW/ClText.h>
#include <CLENAW/ClButton.h>
#include <CLENAW/ClSlider.h>
#include <CLENAW/ClListCtrl.h>
#include <CLENAW/ClVideoview.h>
#include <CLENAW/ClWebBrowser.h>
#include <CLENAW/AnalogMeter.h>
#include <HELPER/ClLog.H>

#include <BLACKBOX/StrdParser.h>

#include "PasswordDlg.h"
#include "NotifyDlg.h"
#include "SettingDlg.h"
//#include "SelModelDlg.h"
//#include "SelDriveDlg.h"
#include "LogDlg.h"
#include "AboutDlg.h"
#include "CopyFileProgressDlg.h"
#include "DlgReport.h"
#include "NXFSFormatterDlg.h"
#include "BBConfigure.h"
#include "BBoxUtil.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SEEK_ZOOM_DO_NOT_USE		0 //capture시 memory Leak 발생 //20120523 DShowLibrary.lib patch

#define DEF_SEL_DRIVE_DLG_USE		0

#define IsSHIFTpressed() ( (GetKeyState(VK_SHIFT) & (1 << (sizeof(SHORT)*8-1))) != 0   )//jun 191023 멀티체크
#define IsCTRLpressed()  ( (GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT)*8-1))) != 0 )//jun 191023 멀티체크

#ifdef TIME_SEARCH_USE
// definitions
	#define		_GID_HOUR	0
	#define 		_GID_MIN	1
	#define 		_GID_SEC	2
	#define 		_GID_MAX	3
	// color position
	#define		_CP_CONTINUOUS			0
	#define		_CP_MOTION			1
	#define		_CP_GSENSOR			2
	#define		_CP_BUTTON		3
	#define		_CP_PREEVENT		4
	#define		_CP_NULL			5

	// color
	#define		_CONTINUOUS_COLOR	RGB(100, 100, 100)
	#define		_MOTION_COLOR			RGB(0, 0, 220)
	#define		_GSENSOR_COLOR		RGB(220, 220, 0)
	#define		_BUTTON_COLOR			RGB(0, 220, 0)
	#define		_PREEVENT_COLOR		RGB(192, 192, 192)

	#define 	_BG_COLOR			RGB(48, 48, 48)
	#define 	_TEXT_COLOR			RGB(255, 255, 255)
#endif

enum thread_msg{
    	THREAD_MSG_RUN = (WM_USER+ 0x1000),
    	THREAD_MSG_EXIT,
	THREAD_MSG_END
};

#if !DEF_SEL_DRIVE_DLG_USE
static int CALLBACK BrowseForFolder_CallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
   if(uMsg == BFFM_INITIALIZED)
       SendMessage(hwnd, BFFM_SETSELECTION, (WPARAM)TRUE, (LPARAM)lpData);
   return 0;
}
#endif

// CBBPlayerDlg dialog

CBBPlayerDlg::CBBPlayerDlg(CWnd* pParent /*=NULL*/)
	: ClDialog(CBBPlayerDlg::IDD, pParent)
	, m_bFrontFlip(false)
	, m_bFrontMirror(false)
	, m_bRearFlip(false)
	, m_bRearMirror(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	this->m_nCurPlayIndex	= -1;
	this->m_pUIControl		= NULL;
	this->m_cstrSelectDrive	= TEXT("");
	this->m_cstrMp4SavePath 	= TEXT("");
	this->m_cstrJpgSavePath	= TEXT("");
	this->m_cstrLogFilePath		= TEXT("");
	this->m_cstrPassword			= TEXT("");
	this->m_pGSensorView		= NULL;
	this->m_nVolumePosition	= 0;

	this->m_eUnitOfSpeed = UNIT_KM_P_H;
	
	this->m_bFileSaveThreadWorking = FALSE;
	
#if TEXT_DATA_THREAD_USE
	this->m_bTextDataThreadWorking = FALSE;
	this->m_bTextDataThreadExit = FALSE;
	this ->m_pTextDataThread = NULL;
#endif	
	this->m_bIsRemovalNxfs = FALSE;

	this->m_cstrFvfsFileToAvi = TEXT("");		// 20201026 FVFS Added
	this->m_MuxViewCurrentTime = 0;
	this->m_iMuxViewChannel = 0;
}


CBBPlayerDlg::~CBBPlayerDlg()
{
	m_BitmapZoom.DeleteObject();
	m_BitmapMux.DeleteObject();
	
	if( this->m_pUIControl )
		delete this->m_pUIControl;

//	if( this->m_pGSensorView )
//		delete this->m_pGSensorView;
}


void CBBPlayerDlg::DoDataExchange( CDataExchange* pDX )
{
	ClDialog::DoDataExchange(pDX);
#ifdef TIME_SEARCH_USE	
	DDX_Control(pDX, IDC_DATETIMEPICKER_TIME, m_ctrlTime);
	DDX_Control(pDX, IDC_DATETIMEPICKER_DATE, m_ctrlDate);
	DDX_Control(pDX, IDC_ICDBGRAPH_H, m_ctrlICDBGraph[_GID_HOUR]);	
	DDX_Control(pDX, IDC_ICDBGRAPH_M, m_ctrlICDBGraph[_GID_MIN]);
	DDX_Control(pDX, IDC_ICDBGRAPH_S, m_ctrlICDBGraph[_GID_SEC]);
#endif
}

BEGIN_MESSAGE_MAP(CBBPlayerDlg, ClDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_DROPFILES()
	ON_WM_COPYDATA()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER_ADDFILE,			&CBBPlayerDlg::OnAddFileFromArguments		)
	ON_MESSAGE(WM_USER_DRIVECHANGED,			&CBBPlayerDlg::OnDriveChanged		)
	ON_MESSAGE(WM_USER_MEDIAFORCESTOP,			&CBBPlayerDlg::OnMediaForceStop		)
	ON_MESSAGE(WM_CLENAW_SLIDER,				&CBBPlayerDlg::OnSliderScroll		)
	ON_BN_CLICKED(IDC_MAIN_BTN_SET,				&CBBPlayerDlg::OnClickSetting		)
	ON_BN_CLICKED(IDC_MAIN_BTN_LOG,			&CBBPlayerDlg::OnClickLog		)
	ON_BN_CLICKED(IDC_MAIN_BTN_CAPTURE,			&CBBPlayerDlg::OnClickCapture		)
	ON_BN_CLICKED(IDC_MAIN_BTN_ABOUT,			&CBBPlayerDlg::OnClickAbout			)
	ON_BN_CLICKED(IDC_MAIN_BTN_SITE,			&CBBPlayerDlg::OnClickSite			)
	ON_BN_CLICKED(IDC_MAIN_BTN_MINIMIZE,		&CBBPlayerDlg::OnClickMinimize		)
	ON_BN_CLICKED(IDC_MAIN_BTN_MAXIMIZE,		&CBBPlayerDlg::OnClickMaximize		)
	ON_BN_CLICKED(IDC_MAIN_BTN_NORMALIZE,		&CBBPlayerDlg::OnClickNormalize		)
	ON_BN_CLICKED(IDC_MAIN_BTN_EXIT,			&CBBPlayerDlg::OnClickExit			)
	ON_BN_CLICKED(IDC_MAIN_BTN_PREV,			&CBBPlayerDlg::OnClickPrev			)
	ON_BN_CLICKED(IDC_MAIN_BTN_PLAY,			&CBBPlayerDlg::OnClickPlay			)
	ON_BN_CLICKED(IDC_MAIN_BTN_STOP,			&CBBPlayerDlg::OnClickStop			)
	ON_BN_CLICKED(IDC_MAIN_BTN_NEXT,			&CBBPlayerDlg::OnClickNext			)
	ON_BN_CLICKED(IDC_MAIN_BTN_REW,				&CBBPlayerDlg::OnClickRew			)
	ON_BN_CLICKED(IDC_MAIN_BTN_FF,				&CBBPlayerDlg::OnClickFF			)
	ON_BN_CLICKED(IDC_MAIN_BTN_FPS,				&CBBPlayerDlg::OnClickFPS			)
	ON_BN_CLICKED(IDC_MAIN_BTN_OPEN,			&CBBPlayerDlg::OnClickOpen			)
	ON_BN_CLICKED(IDC_MAIN_BTN_SAVE,			&CBBPlayerDlg::OnClickSave			)
	ON_BN_CLICKED(IDC_MAIN_BTN_SAVEALL,			&CBBPlayerDlg::OnClickSaveAll			)	
	ON_BN_CLICKED(IDC_MAIN_BTN_PRINT, 		&CBBPlayerDlg::OnClickPrint)
	ON_BN_CLICKED(IDC_MAIN_BTN_FRCHANGE,		&CBBPlayerDlg::OnClickFRChange		)
	ON_BN_CLICKED(IDC_MAIN_BTN_FMCHANGE, 		&CBBPlayerDlg::OnClickFMChange)
	ON_BN_CLICKED(IDC_MAIN_BTN_ZOOM, 			&CBBPlayerDlg::OnBnClickZoom)
	ON_BN_CLICKED(IDC_MAIN_BTN_NORMAL,		&CBBPlayerDlg::OnClickTabNormal		)
	ON_BN_CLICKED(IDC_MAIN_BTN_EVENT,			&CBBPlayerDlg::OnClickTabEvent		)
	ON_BN_CLICKED(IDC_MAIN_BTN_MANUAL,			&CBBPlayerDlg::OnClickTabManual		)
	ON_BN_CLICKED(IDC_MAIN_BTN_MOTION,			&CBBPlayerDlg::OnClickTabMotion		)
	ON_BN_CLICKED(IDC_MAIN_BTN_CAM_FF, &CBBPlayerDlg::OnBnClickedMainBtnCamFf)
	ON_BN_CLICKED(IDC_MAIN_BTN_CAM_FM, &CBBPlayerDlg::OnBnClickedMainBtnCamFm)
	ON_BN_CLICKED(IDC_MAIN_BTN_CAM_RF, &CBBPlayerDlg::OnBnClickedMainBtnCamRf)
	ON_BN_CLICKED(IDC_MAIN_BTN_CAM_RM, &CBBPlayerDlg::OnBnClickedMainBtnCamRm)
	ON_BN_CLICKED(IDC_MAIN_BTN_VOLUME, &CBBPlayerDlg::OnBnClickedMainBtnVolume)
	ON_BN_CLICKED(IDC_DIGIT_NUM_SPEED, &CBBPlayerDlg::OnBnClickedMainBtnSpeed)
	
	ON_NOTIFY(NM_DBLCLK, IDC_MAIN_FILELIST,		&CBBPlayerDlg::OnNMDblclkFilelist	)
	ON_NOTIFY(NM_CLICK, IDC_MAIN_FILELIST, &CBBPlayerDlg::OnNMClickMainFilelist)
#ifdef TIME_SEARCH_USE	
	ON_NOTIFY(/*MCN_SELECT*/MCN_SELCHANGE, IDC_DATETIMEPICKER_DATE, &CBBPlayerDlg::OnCloseupDatetimepickerDate)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_TIME, &CBBPlayerDlg::OnDatetimechangeDatetimepickerTime)
	ON_MESSAGE(ICM_DBGRAPH, &CBBPlayerDlg::OnICDBGraph)
#endif	
	ON_MESSAGE(CM_GSENSOR_VIEW_SEEK_FRAME, &CBBPlayerDlg::OnGsensorSeekFrame)
	ON_MESSAGE(CM_ZOOM_REDRAW,				&CBBPlayerDlg::OnDrawZoomImage)
	ON_MESSAGE(CM_MAIN_REDRAW,				&CBBPlayerDlg::OnRedraw)
	ON_MESSAGE(CM_MUX_REDRAW,				&CBBPlayerDlg::OnDrawMuxImage)
	ON_BN_CLICKED(IDC_MAIN_BTN_FULL_F, &CBBPlayerDlg::OnBnClickedMainBtnFullF)
	ON_BN_CLICKED(IDC_MAIN_BTN_FULL_R, &CBBPlayerDlg::OnBnClickedMainBtnFullR)
	ON_BN_CLICKED(IDC_MAIN_BTN_FULL_M, &CBBPlayerDlg::OnBnClickedMainBtnFullM)
	ON_BN_CLICKED(IDC_MAIN_BTN_CHECKFILELISTALL, &CBBPlayerDlg::OnBnClickedMainBtnFileListAll)
	ON_BN_CLICKED(IDC_MAIN_BTN_FORMAT, &CBBPlayerDlg::OnBnClickedMainBtnFormat)
	ON_BN_CLICKED(IDC_MAIN_BTN_GPS_PULSE, &CBBPlayerDlg::OnBnClickedMainBtnGpsPulse)
	ON_STN_CLICKED(IDC_MAIN_VIDEO_FRONT, &CBBPlayerDlg::OnStnClickedMainVideoFront)
	ON_STN_DBLCLK(IDC_MAIN_VIDEO_FRONT, &CBBPlayerDlg::OnStnDblclickMainVideoFront)	
	ON_STN_CLICKED(IDC_MAIN_VIDEO_REAR, &CBBPlayerDlg::OnStnClickedMainVideoRear)
	ON_STN_CLICKED(IDC_MAIN_VIDEO_QUADCTRL, &CBBPlayerDlg::OnStnClickedMainVideoRear)
	ON_STN_DBLCLK(IDC_MAIN_VIDEO_REAR, &CBBPlayerDlg::OnStnDblclickMainVideoRear)
	ON_STN_DBLCLK(IDC_MAIN_VIDEO_QUADVIEW, &CBBPlayerDlg::OnStnDblclickMainVideoRear)
	ON_STN_DBLCLK(IDC_MAIN_VIDEO_QUADCTRL, &CBBPlayerDlg::OnStnDblclickMainVideoRear)
END_MESSAGE_MAP()


BOOL CBBPlayerDlg::PreTranslateMessage( MSG* pMsg )
{
#if 0
	if(		(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		||	(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) )
		return TRUE;
#else

	if(pMsg->message == WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
			case VK_RETURN:
			{
				if(this->m_nCurMode == 0)
					OnClickMaximize();	
				else
					OnClickNormalize();
				return TRUE;
			}
			case VK_ESCAPE:	OnClickNormalize();		return TRUE;
			case VK_SPACE:		
			{
				 //CWBCFileManager::PSDRIVEINFO pDriveInfo = this->m_fileManager.getCurDriveInfo();
               //if( pDriveInfo == NULL )    this->OnClickOpen();
               //else                        this->OnClickPlay();
               this->OnClickPlay();
				return TRUE;
			}
			case VK_PRIOR:		OnClickPrev();			return TRUE;
			case VK_NEXT:		OnClickNext();			return TRUE;

			case VK_LEFT:		OnClickRew();				return TRUE;
			case VK_RIGHT:		OnClickFF();				return TRUE;
			case VK_UP:				return TRUE;
			case VK_DOWN:			return TRUE;

			case 'U':				OnClickSetting();			return TRUE;
			case 'I':				OnClickAbout();			return TRUE;
			case 'C':				OnClickCapture();			return TRUE;
			case 'F':				OnClickFPS();				return TRUE;
			case 'L':				OnClickLog();				return TRUE;
			case 'M':				
			{
				ClButton* 		pBtn = BTN_GET( this->m_pUIControl->m_Language.GetButtonText(TEXT("main.button.fmchange")) );
				if(pBtn){
					pBtn->SetCheckState(!pBtn->GetCheckState()); 
				
					OnClickFMChange();		
				}
				return TRUE;
			}
			case 'R':				
			{
				ClButton* 		pBtn = BTN_GET(this->m_pUIControl->m_Language.GetButtonText(TEXT("main.button.frchange")) );
				if(pBtn){
					pBtn->SetCheckState(!pBtn->GetCheckState()); 
				
					OnClickFRChange();		
				}
				return TRUE;
			}
			case 'S':				OnClickSave();			return TRUE;
			case 'P':				OnClickPrint();			return TRUE;
			case 'A':				OnClickSaveAll();			return TRUE;
			case 'O':				OnClickOpen();			return TRUE;
			case 'Z':		
			{
				ClButton* 		pBtnZoom 	= BTN_GET( TEXT("main.button.zoom") );
				if(pBtnZoom)
				{
				  	pBtnZoom->SetCheckState(!pBtnZoom->GetCheckState()); 
				}
				OnBnClickZoom();
				return TRUE;
			}
		}
	}
#endif
	return ClDialog::PreTranslateMessage(pMsg);
}

VOID CBBPlayerDlg::InitResourceID( VOID )
{
	if( !this->m_pUIControl )
	{
		this->m_pUIControl = new CBBPlayerUIControl();
		this->m_pUIControl->SetBBPlayerProp( this->m_pDialogManager, &this->m_multiPlayer, &this->m_textData);
	}

	this->m_pUIControl->InitBBPlayerRes();
	
	return;
}


CString CBBPlayerDlg::GetPathFromPIDL( DWORD pidl )
{
	WCHAR sPath[MAX_PATH];
	CString strTemp = _T("");

	if( SHGetPathFromIDList((struct _ITEMIDLIST *)pidl, sPath) )
		strTemp = sPath;

	return strTemp;
}


VOID CBBPlayerDlg::CheckFilters( VOID )
{
	CString cstrModulePath = ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getModulePath();
	int nCheck = m_multiPlayer.CheckFilters(cstrModulePath.GetBuffer());
	if ( nCheck  )
	{
		if( nCheck == -1 ) OPEN_NOTIFY_DIALOG( NOTIFY_ERR_NO_SRCFILTER , &this->m_pUIControl->m_Language);
		if( nCheck == -2 ) OPEN_NOTIFY_DIALOG( NOTIFY_ERR_NO_VIDFILTER , &this->m_pUIControl->m_Language);
	}
	return;
}


BOOL CBBPlayerDlg::OpenFile( CString strFilename, DWORD init_indexno)
{

	//////////////////////////////////////////////////////////////////////////
	//{
	BOOL bPlayResult = FALSE;
 
#if TEXT_DATA_THREAD_USE
  	if(m_pTextDataThread == NULL)
  	{
  		this->m_bTextDataThreadExit = FALSE;
		m_pTextDataThread = AfxBeginThread( this->ThreadTextDataProc, this );
  	}	

	UINT delay_timeout = 1000;
	while(this->m_bTextDataThreadWorking && delay_timeout--)
	{
		this->m_bTextDataThreadParsingStop = TRUE;
		::Sleep(10);
	}
#endif

	m_multiPlayer.SetAACPlaySkip( false );

#if defined(__CLBB_NXFS_SUPPORT__)
	CWBCFileManager::PSDRIVEINFO pCurDrive = this->m_fileManager.getCurDriveInfo();
	if( pCurDrive && pCurDrive->m_eDriveMode == CWBCFileManager::DRIVE_MODE_REMOVAL_NXFS ){
		CWBCFileManager::PSFILEINFO pFileInfo = this->m_fileManager.getListupFileInfo( /*this->m_nCurPlayIndex*/ init_indexno );
        void* pNxfsHandle = this->m_fileManager.getNxFSHandle();
        bPlayResult = m_multiPlayer.OpenFile(pFileInfo->m_cstrFileName, pNxfsHandle, pFileInfo->m_fdNxFS );
	}
    else
#endif
	{
//// ++{ 20201026 FVFS Added *******************
		if(m_cstrFvfsFileToAvi.GetLength()){
			if(strFilename.Compare(m_cstrFvfsFileToAvi) != 0){
				//if(!CBBoxUtil::ReplaceFileData(m_cstrFvfsFileToAvi, "FVFS   CDR MOVIE", 16, 0)){
				//}
				CT2A ascii(m_cstrFvfsFileToAvi);
				security_file_change(ascii.m_psz, true);
				
				m_cstrFvfsFileToAvi = _T("");
			}
		}
		
		if(strFilename.Find(_T("fvfs")) >= 0 ) {
			//const char avi_riff_hd[16] = { 0x52, 0x49, 0x46, 0x46, 0xf8, 0xff, 0xbf, 0x00, 0x41, 0x56, 0x49, 0x20, 0x4c, 0x49, 0x53, 0x54 };
			
			//if(!CBBoxUtil::ReplaceFileData(strFilename, avi_riff_hd, 16, 0)){
			//	OPEN_NOTIFY_DIALOG( NOTIFY_ERR_INVALID_STREAM ,&this->m_pUIControl->m_Language);
			//	return FALSE;
			//}
			CT2A ascii(strFilename);
			security_file_change(ascii.m_psz, false);
			m_cstrFvfsFileToAvi = strFilename;
		}
//// ++}***************************************
		bPlayResult = m_multiPlayer.OpenFile(strFilename, NULL, -1) ;
	}
	if( !bPlayResult)
	{
		OPEN_NOTIFY_DIALOG( NOTIFY_ERR_INVALID_STREAM ,&this->m_pUIControl->m_Language);
		return FALSE;
	}

	LONGLONG durationTime = m_multiPlayer.GetDuration();
	if( !durationTime )
	{
		m_multiPlayer.StreamStop();
		m_multiPlayer.CloseFile();
		OPEN_NOTIFY_DIALOG( NOTIFY_ERR_INVALID_STREAM ,&this->m_pUIControl->m_Language);
		return FALSE;
	}

	this->m_pUIControl->SetStreamInfo( this->m_multiPlayer.GetStreamCount(), TRUE,	TRUE);

#if TEXT_DATA_THREAD_USE	
	m_cstrCurrentPlayFileName = strFilename;
	if(m_pTextDataThread)
  			m_pTextDataThread->PostThreadMessage(THREAD_MSG_RUN, 0, 0);

 #if 0 // 제거 : Text data threadd에서 처리
 //20190716  : UI 재생시간과 영상의 타임스템프 동기를 위해 추가된 코드
 	ClListCtrl* pFileList = LST_GET( TEXT("main.listbox.filelist") );
	if(pFileList && CBBoxUtil::GetEventType(pFileList->GetItemText(m_nCurPlayIndex, 1)) == CBBoxUtil::BB_EVENT_GSENSOR ) {
		HRESULT hr = 0;
		unsigned char text[1024];
		int size = 1024;
		unsigned long cts;
		while(TRUE)
		{
			ZeroMemory(text, 1024);
			cts = 0;
			size = 1024;
			hr = m_multiPlayer.GetTextData(text, &size, &cts);

			if( hr == ERROR_HANDLE_EOF )
			{
				break;
			}
			else if( hr == S_OK )
			{
				if(text[0] == 'M'){
					
					
					if(pFileList){
					    struct tm dt;

						dt.tm_sec   =  	text[7+5];
						dt.tm_min   =  	text[7+4];
						dt.tm_hour  =  	text[7+3];
					    
						dt.tm_mday  =   text[7+2];
						dt.tm_mon   = 	text[7+1];
						dt.tm_year  = 	100 + text[7+0];
						dt.tm_isdst = 0;

						pFileList->m_dwfile_time = mktime(&dt);
						//pFileList->m_dwfile_time  -= 10;   // jun 170623
					}
						
					break;
				}
			}
		}
	}
 #endif

#else
	m_textData.m_GSensorList.clear();
	m_textData.m_GPSList.clear();
	m_textData.m_PulseList.clear();

	HRESULT hr = 0;
	int textcount = 1;
	unsigned char text[1024];
	int size = 1024;
	unsigned long cts;

	int isAvicFile = CBBoxUtil::GetFileType(strFilename);
	CWBCSensorData::EGSENSORTYPE gsensor_type = CWBCSensorData::GSENSOR_OLD_8_BIT;
	if(isAvicFile)
		gsensor_type = CWBCSensorData::GSENSOR_AVIC_8_BIT;
	
	while(TRUE)
	{
		ZeroMemory(text, 1024);
		cts = 0;
		size = 1024;
		hr = m_multiPlayer.GetTextData(text, &size, &cts);
		if( hr == S_OK )
		{
			m_textData.ParseTextData(text, cts, gsensor_type);
		}
		else if( hr == ERROR_HANDLE_EOF )
		{
			break;
		}
		else if(hr == E_FAIL)
		{
		}

		textcount++;
	}

	if( m_pGSensorView )
		m_pGSensorView->SetSensorData( &this->m_textData );
#endif


 #if 0
	int nPos = strFilename.ReverseFind( TEXT('\\') );
	CString cstrFileName = strFilename.Mid(nPos+1);
	ClText* pTxtFile = TXT_GET( TEXT("main.text.file") );
	if( pTxtFile )
	{
		CString cstrFileNo;
		ClListCtrl* pFileList = LST_GET( TEXT("main.listbox.filelist") );

		if(pFileList)
			cstrFileNo.Format(TEXT("    [%d/%d]"), m_nCurPlayIndex + 1,  pFileList->GetItemCount());
		
			cstrFileName += cstrFileNo;
		
		pTxtFile->SetWindowText( cstrFileName );
	}
 #else
	m_pUIControl->PlayTimeProgress(0);
 #endif
	
	ClText* pTextLatitude = TXT_GET( TEXT("main.text.latitude") );
	ClText* pTextLongitude = TXT_GET( TEXT("main.text.longitude") );
	ClText* pTextSpeed = TXT_GET( TEXT("main.text.gps_speed") );
	
	return TRUE;
}


BOOL CBBPlayerDlg::OpenDisk(BOOL init)
{
	CString cstrOpenPath;
	CString cstrInvalidPath;
	ClListCtrl* pFileList = LST_GET( TEXT("main.listbox.filelist") );
	if(pFileList)
	{ 
		BOOL bSubFolderSearch = FALSE;
		WCHAR pszPathname[512];

		ClButton* 		pBtnAll = BTN_GET( TEXT("main.button.checkfilelistall") );
		if(pBtnAll) pBtnAll->SetCheckState(FALSE);
		
		pFileList->SetRedraw( FALSE );

		pFileList->DeleteAllItems();
		
		this->m_pUIControl->SetLogFileExist( FALSE );
		this->m_pUIControl->SetNxfsFileExist( FALSE );
		this->m_cstrLogFilePath = TEXT("");
		
		wcscpy(pszPathname, this->m_cstrSelectDrive.GetBuffer());
		cstrInvalidPath = this->m_cstrSelectDrive ;
		if(cstrInvalidPath.Find(TEXT("\\\\")) == 0) //20210128 network path
			cstrOpenPath = m_cstrSelectDrive;
		else
			ClUtil::PATH::makeValidPath( cstrInvalidPath, cstrOpenPath );
		
		cstrOpenPath += TEXT("\\");

		this->m_fileManager.dropDrive();
		
		if(cstrOpenPath.GetLength() > 3 && init == FALSE)
		{
			this->OpenDirectory( cstrOpenPath, TRUE );
		}
		else
		{
			TCHAR   pszLabel[MAX_PATH];
			pszPathname[3] = 0;
			
			if(init == 0)
				::GetVolumeInformation(pszPathname, pszLabel, MAX_PATH, NULL, NULL, NULL, NULL, NULL);
			
			CString	csVolume = pszLabel;

			if(init || csVolume.Find(FORMAT_LABEL) == 0 )  //210323 SSD No Search
			{
				int drive, maxDrive;

#if 0				
				if(pszPathname[0] > 'C')
				{
					pszPathname[0] = pszPathname[0] - 1;
					maxDrive = 3;
				}
				else
#endif

#if 1
				UINT nCount=::GetLogicalDriveStrings(256,pszPathname);
				WCHAR *pBufIndex = pszPathname; 

				for(int i=0;i<nCount/4;i++)
				{
					INT nDriveIndex = pBufIndex[0] - 'A';
					
					if(pBufIndex[0] == 'C')
					{
						pBufIndex+= 4;
						continue;
					}
					

					::GetVolumeInformation(pBufIndex, pszLabel, MAX_PATH, NULL, NULL, NULL, NULL, NULL);
					csVolume = pszLabel;
					DEBUGMSG(1,("drive : %s	(%d:%s) \n", pBufIndex, nDriveIndex, pszLabel));

					if(init)
					{
						if(csVolume.Find(FORMAT_LABEL) == 0)  //210323 SSD No Search
						{
							//m_cstrSelectDrive.Format(pBufIndex);
							
							this->OpenDirectory( pBufIndex, TRUE );

							if(pFileList->GetItemCount()&& !this->m_bIsRemovalNxfs)   
						//	if(pFileList->GetItemCount())                                      // jun 201021
								m_cstrSelectDrive.Format(_T("%s"), pBufIndex);         // jun 201021
								
						}
					}
					else if(::GetDriveType(pBufIndex) == DRIVE_REMOVABLE || ::GetDriveType(pBufIndex) == DRIVE_REMOTE || csVolume.Find(FORMAT_LABEL) == 0) // more than c:  210323 SSD No Search
					{
						this->OpenDirectory( pBufIndex, TRUE );

					//	if(pFileList->GetItemCount())                              // jun 201021
					//		m_cstrSelectDrive.Format(_T("%s"), pBufIndex);     // jun 201021
					}
	
				    	pBufIndex+= 4;
				}
#endif				
			}
			else if(pszPathname[1] == ':')
			{
				if(::GetDriveType(pszPathname) == DRIVE_REMOVABLE || ::GetDriveType(pszPathname) == DRIVE_REMOTE)
					bSubFolderSearch = TRUE;
				else
					bSubFolderSearch = FALSE;

				this->OpenDirectory( cstrOpenPath, bSubFolderSearch );
			}
		}

		pFileList->Sort(ClListCtrl::SORT_STR_DATE);
		pFileList->SetRedraw( TRUE );
			
		this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_SEL_DRIVE );

#ifdef TIME_SEARCH_USE
		CTime time;
		if( pFileList->GetItemCount())
		{
			time = CTime(pFileList->GetItemData( pFileList->GetItemCount()-1));
			UpdateIndexFileList(pFileList->GetItemCount()-1);
		}
		else
			time = CTime::GetCurrentTime();

		UpdateGraphDB(&time, 1);

		ShowWindowTimeSearch(SW_SHOW);
#endif
	}
	
	return TRUE;
}

BOOL CBBPlayerDlg::OpenDirectory( CString cstrDir , BOOL bSubFolderSearch )
{
	if( !this->m_cstrSelectDrive.GetLength() )
		return FALSE;

	ClListCtrl* pFileList = LST_GET( TEXT("main.listbox.filelist") );

	if(pFileList == FALSE)
		return FALSE;

		WCHAR *pBufIndex = cstrDir.GetBuffer();
		INT nDriveIndex = pBufIndex[0] - 'A';

#if defined(__CLBB_NXFS_SUPPORT__)
		CWBCFileManager::PSDRIVEINFO pCurDrive = this->m_fileManager.getCurDriveInfo();
		if(pCurDrive && pCurDrive->m_eDriveMode == CWBCFileManager::DRIVE_MODE_REMOVAL_NXFS)
			return FALSE;

		 this->m_fileManager.openDrive();
        int nDrives = this->m_fileManager.getDrives();

        int nIndex;
		 this->m_bIsRemovalNxfs = FALSE;

        for( nIndex = 0; nIndex < nDrives; nIndex ++ )
        {
            const CWBCFileManager::PSDRIVEINFO pDrive = this->m_fileManager.getDriveInfo( nIndex );
            if( pDrive != NULL ){
							
				DEBUGMSG(1,("drive : %s	(%d:%s) \n", pBufIndex, nIndex, pDrive->m_cstrComboFace));
				if(pDrive->m_cstrComboFace.Find(cstrDir) >= 0){
					if(pDrive->m_eDriveMode == CWBCFileManager::DRIVE_MODE_REMOVAL_NXFS){
						this->m_bIsRemovalNxfs = TRUE;
					}
					break;
				}
            }
        }

		if(this->m_bIsRemovalNxfs){
			DEBUGMSG(1,("select drive : %d\n", nIndex));
			if( this->m_fileManager.selectDrive(nIndex) ){
				this->m_fileManager.listupFiles( pFileList );
				this->m_pUIControl->SetNxfsFileExist( TRUE );
				this->m_cstrSelectDrive = cstrDir;
			}
			else{
				this->m_bIsRemovalNxfs = FALSE;
			}
		}
#endif

	if(this->m_bIsRemovalNxfs == FALSE){
		WIN32_FIND_DATA	findData;
		HANDLE			hDir = NULL;

		hDir = FindFirstFileEx( cstrDir + TEXT("*.*"), FindExInfoStandard, &findData, FindExSearchNameMatch, NULL, 0 );
		if( hDir == INVALID_HANDLE_VALUE )
			return FALSE;

		SYSTEMTIME stUTC, stLocal;
		

		std::wstring wtsFilter = ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getSDDirInfo(  this->m_pUIControl->GetListTabPrevEvent() - CBBPlayerUIControl::EVENT_LIST_NORMAL);
		if( !wtsFilter.length() )
			wtsFilter += TEXT("*");
		do
		{
			if( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				if( !_tcscmp(findData.cFileName, TEXT(".")) || !_tcscmp(findData.cFileName, TEXT("..")) )
					continue;

				if(bSubFolderSearch)
				{
					CString cstrSubDir = cstrDir + findData.cFileName + TEXT("\\");
					this->OpenDirectory( cstrSubDir );
				}
			}
			else
			{
				CString cstrFileName(findData.cFileName);
				std::wstring wtsExt = ClUtil::PATH::getExtSpecW( findData.cFileName );
				if( !((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->isExistExtension( wtsExt.c_str() ) )
				{
					if( ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->isExistLogFile( findData.cFileName  ))
					{
						this->m_cstrLogFilePath = cstrDir + findData.cFileName;
						this->m_pUIControl->SetLogFileExist(TRUE);
					}
					continue;
				}
	// 추가이벤트항목 Event teb에 표시 jun 201021 			
				if(*(wtsFilter.c_str()) != TEXT('*') && cstrFileName.Find(wtsFilter.c_str()) == -1 && \
					(this->m_pUIControl->GetListTabPrevEvent() == CBBPlayerUIControl::EVENT_LIST_EVENT && CBBoxUtil::GetEventType(findData.cFileName) != CBBoxUtil::BB_EVENT_GSENSOR) )
					continue;

				FileAddToFileList(cstrDir, &findData);
			}

		} while( FindNextFile(hDir, &findData) );

		FindClose(hDir);
	}

	return TRUE;
}


VOID CBBPlayerDlg::UpdateIndexFileList(INT index)
{   
	
	ClListCtrl* pFileList = LST_GET( TEXT("main.listbox.filelist") );
	if( pFileList && pFileList->GetItemCount() > index )
	{
		for( int iRow = 0; iRow <= pFileList->GetItemCount(); iRow++ )
			pFileList->SetItemState( iRow, LVIS_GLOW, LVIS_GLOW|LVIS_SELECTED|LVIS_DROPHILITED );

		pFileList->SetItemState( index, LVIS_SELECTED|LVIS_DROPHILITED,LVIS_GLOW | LVIS_SELECTED|LVIS_DROPHILITED );
		pFileList->SetHotItem( index );

		pFileList->EnsureVisible( index, FALSE );

		pFileList->m_nSelectItemNo = index;
	//	pFileList->m_dwfile_time = pFileList->GetItemData(index);  // jun 170602
		CString strTime = pFileList->GetItemText(index , ClListCtrl::SORT_STR_DATE); 
		 pFileList->m_dwfile_time  = (DWORD)_tstoi64(strTime);

		if(CBBoxUtil::GetEventType(pFileList->GetItemText(index, 1)) == CBBoxUtil::BB_EVENT_GSENSOR )
 	       pFileList->m_dwfile_time  -= 10;   // jun 170623
	}
}

BOOL CBBPlayerDlg::FileAddToFileList(CString cstrDir, WIN32_FIND_DATA *pFindData)
{
	ClListCtrl* pFileList = LST_GET( TEXT("main.listbox.filelist") );

	if(pFileList == FALSE)
		return FALSE;

	INT nItemCount = pFileList->GetItemCount();
	CString cstrText;
	SYSTEMTIME stUTC, stLocal;
	
	//분리된 채널 추가 안되도록
	CString cstrFile(pFindData->cFileName);
	//if(cstrFile.Find(TEXT("_front.")) >= 0 || cstrFile.Find(TEXT("_rear.")) >= 0)
	  if(cstrFile.Find(TEXT("_F.")) >= 0 || cstrFile.Find(TEXT("_R.")) >= 0)
		return FALSE;
	
	//tony, for sort	
	//20010119_071403_I2 daview
	if(pFindData->cFileName[8] == L'_' && pFindData->cFileName[15] == L'_')
	{
		TCHAR szTime[9];
		_tcsncpy(szTime, &pFindData->cFileName[0], 8);
		szTime[8] = '\0';
		cstrText = szTime;

		_tcsncpy(szTime, &pFindData->cFileName[9], 6);
		szTime[6] = '\0';
		cstrText += szTime;			

		stLocal.wYear = ((pFindData->cFileName[0] - L'0') * 1000) + ((pFindData->cFileName[1] - L'0') * 100) + ((pFindData->cFileName[2] - L'0') * 10)  + pFindData->cFileName[3] - L'0';
		stLocal.wMonth = ((pFindData->cFileName[4] - L'0') * 10) + pFindData->cFileName[5] - L'0' ;
		stLocal.wDay = ((pFindData->cFileName[6] - L'0') * 10) + pFindData->cFileName[7] - L'0';

		stLocal.wHour = ((pFindData->cFileName[9] - L'0') * 10) + pFindData->cFileName[10] - L'0';
		stLocal.wMinute = ((pFindData->cFileName[11] - L'0') * 10) + pFindData->cFileName[12] - L'0';
		stLocal.wSecond = ((pFindData->cFileName[13] - L'0') * 10) + pFindData->cFileName[14] - L'0';

		//Merge 된 파일은 추가 안되도록
		if(pFindData->cFileName[18] ==  L'-' && pFindData->cFileName[19 + 8] == L'_' && pFindData->cFileName[19 + 15] == L'_')
			return FALSE;
		
	}
	else if(pFindData->cFileName[3] == L'_' && pFindData->cFileName[10] == L'_') //avic file
	{
		TCHAR szTime[9];
		int offset;
		_tcsncpy(szTime, &pFindData->cFileName[4], 6);
		szTime[6] = '\0';
		cstrText = TEXT("20");
		cstrText += szTime;

		_tcsncpy(szTime, &pFindData->cFileName[11], 6);
		szTime[6] = '\0';
		cstrText += szTime;			
		
		stLocal.wYear = 2000 + ((pFindData->cFileName[4] - L'0') * 10)  + pFindData->cFileName[5] - L'0';
		stLocal.wMonth = ((pFindData->cFileName[6] - L'0') * 10) + pFindData->cFileName[7] - L'0' ;
		stLocal.wDay = ((pFindData->cFileName[8] - L'0') * 10) + pFindData->cFileName[9] - L'0';

		stLocal.wHour = ((pFindData->cFileName[11] - L'0') * 10) + pFindData->cFileName[12] - L'0';
		stLocal.wMinute = ((pFindData->cFileName[13] - L'0') * 10) + pFindData->cFileName[14] - L'0';
		stLocal.wSecond = ((pFindData->cFileName[15] - L'0') * 10) + pFindData->cFileName[16] - L'0';
	}
	else
	{
		FileTimeToSystemTime(&pFindData->ftLastWriteTime, &stUTC);
		SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
		cstrText.Format( TEXT("%02d/%02d/%d"), stLocal.wMonth, stLocal.wDay, stLocal.wYear );
		pFileList->SetItemText( nItemCount, 1, cstrText );
		if(stLocal.wYear < 2000)
			stLocal.wYear = 2000;

		cstrText.Format( TEXT("%d%02d%02d%02d%02d%02d"), stLocal.wYear - 2000, stLocal.wMonth, stLocal.wDay,stLocal.wHour,stLocal.wMinute,stLocal.wSecond);
	}
	
	pFileList->InsertItem( nItemCount, cstrText );

	//20170523 tony, for sort 기존에는 녹화시간으로 저장하였으나 동일한 녹화시간의 파일에선 쏘팅이 안되는 문제가 있어 일런번호로 변경
	pFileList->SetItemData(nItemCount , nItemCount);

#if 0
	CString str;
	
	str.Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), time.GetYear(), time.GetMonth(), time.GetDay(), \
		time.GetHour(), time.GetMinute(), time.GetSecond());
	
	pFileList->SetItemText( nItemCount, 1, str);
#else
	pFileList->SetItemText( nItemCount, 1, pFindData->cFileName);
#endif

	INT64 fileSize = (((INT64)pFindData->nFileSizeHigh)<<32) + pFindData->nFileSizeLow;
	fileSize /= 1024;
	cstrText.Format(TEXT("%d.%01dMB"), int(fileSize / 1000), int(fileSize % 1000 / 100));
	pFileList->SetItemText( nItemCount, 2, cstrText );

	cstrText = cstrDir + pFindData->cFileName;
	pFileList->SetItemText( nItemCount, 3, cstrText );

	//20170523 : filename2 ex) E2016...
	cstrText = pFindData->cFileName;
	cstrText.Insert(0, cstrText.GetBuffer()[16]);
	pFileList->SetItemText( nItemCount, 4, cstrText);

	if(stLocal.wYear < 1970)   // jun 190926 1970년 이전 연도에러 수정
	   stLocal.wYear = 1970;   // jun 190926
	//20170523 : str_date
	CTime time = CTime(stLocal);
	CString strTime;
	strTime.Format(_T("%d"), time.GetTime());
	pFileList->SetItemText(nItemCount , 5,  strTime);
	
	return TRUE;
}

VOID CBBPlayerDlg::StopMedia( VOID )
{
	int nPlayState = m_multiPlayer.GetState();
	if(		nPlayState == PLAY_STATE_RUNNING
		||	nPlayState == PLAY_STATE_PAUSED
		||	nPlayState == PLAY_STATE_STOPPED
		||	nPlayState == PLAY_STATE_CLOSED
		||	nPlayState < 0 )
	{
		KillTimer(MAINDLG_TID_PROGRESS);

		m_multiPlayer.StreamStop();
		m_multiPlayer.SetPosition(0);

		m_multiPlayer.CloseFile();
		m_nCurPlayIndex = -1;

		m_textData.m_GSensorList.clear();
		m_textData.m_GPSList.clear();
		m_textData.m_PulseList.clear();


//		if( m_pGSensorView )
//			m_pGSensorView->ShowWindow(SW_HIDE);

		this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_MEDIA_STOPPED );
		// Video 가 UI 영역에 Display되는 문제로 임시로 추가
		//this->InvalidateRect( NULL );
	}
#ifdef TIME_SEARCH_USE
	ShowWindowTimeSearch(SW_SHOW);
#endif	

//// ++{ 20201026 FVFS Added *******************
	if(m_cstrFvfsFileToAvi.GetLength()){
		//if(!CBBoxUtil::ReplaceFileData(m_cstrFvfsFileToAvi, "FVFS   CDR MOVIE", 16, 0)){
		//}
		CT2A ascii(m_cstrFvfsFileToAvi);
		security_file_change(ascii.m_psz, true);
		m_cstrFvfsFileToAvi = _T("");
	}
//// ++}***************************************

}

VOID CBBPlayerDlg::SetVolume(INT nVolume) // max 100
{
	DWORD nRVol;
	
	if(m_nVolumePosition == nVolume)
		return;

	if(nVolume < 0)
		nVolume = 0;
	else if(nVolume > 100)
		nVolume = 100;
#ifdef AUDIO_VOLUME_CONTROL_USE
	nRVol = DWORD( nVolume * 655.35 );
	nRVol = nRVol << 16;
	nRVol = DWORD( nRVol + nVolume *  655.35 );
 #if 0
	HWAVEOUT hwo;
	WAVEFORMATEX wf;
	wf.wFormatTag = WAVE_FORMAT_PCM;
	wf.nChannels = 1;
	wf.nSamplesPerSec = 8000 * 1000;
	wf.wBitsPerSample = 8;
	wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
	wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
	wf.cbSize = 0;

	 for (UINT id = 0; id < ::waveOutGetNumDevs(); id++) {                     
		  if (::waveOutOpen(&hwo, id, &wf, 0, 0, CALLBACK_NULL) 
		                     == MMSYSERR_NOERROR) 
		                 {
		 ::waveOutSetVolume( hwo, DWORD(nRVol) );
		 ::waveOutClose(hwo);
		 break;
	 	}
	 }
	 printf("SetVolume : %d %d 0x%8x\r\n",m_nVolumePosition ,nVolume, nRVol);
 #else
	//::waveOutSetVolume( NULL, DWORD(nRVol) );
	m_multiPlayer.SetVolume(nRVol);
 #endif
#endif
	m_nVolumePosition = nVolume;
}

BOOL CBBPlayerDlg::StartPlay(void)
{
	ClVideoView*		pVideoFront = VID_GET( TEXT("main.video.front"));
	ClVideoView*		pVideoRear = VID_GET( TEXT("main.video.rear"));
	//if(pVideoFront)	pVideoFront->ShowWindow( SW_HIDE );
	//if(pVideoRear)	pVideoRear->ShowWindow( SW_HIDE );
	
	// Video 가 UI 영역에 Display되는 문제로 임시로 추가
#if 0//BUILD_MODEL == MODEL_RALLY_EYES
  #if 0
	CRect rect;
	GetClientRect(rect);
		
	ClWebBrowser* pSponsor = MAP_GET( TEXT("main.web.sponsor") );
	if(pSponsor)
	{
		CRect rectSponsor;
		pSponsor->GetWindowRect(rectSponsor);
		rect.bottom -=  rectSponsor.Height();
	}
	this->InvalidateRect( rect );
  #else
	this->InvalidateRect( NULL );
  #endif
#endif

	m_multiPlayer.StreamPlay();
	SetTimer( MAINDLG_TID_PROGRESS, MAINDLG_TMDUR_PROGRESS, NULL );
		
#if 0// (BUILD_MODEL != MODEL_RALLY_EYES)     //jun 200729 반전기능삭제
	m_multiPlayer.SetFlip(MP_VIDEO_CH_1, m_bFrontFlip);
	m_multiPlayer.SetMirror(MP_VIDEO_CH_1, m_bFrontMirror);
	m_multiPlayer.SetFlip(MP_VIDEO_CH_2, m_bRearFlip);
	m_multiPlayer.SetMirror(MP_VIDEO_CH_2, m_bRearMirror);
#endif	
	// speed check
	{
		ClSlider* pSldSpeed	= SLD_GET( TEXT("main.slider.speed") );
		ClSlider* pSldBright	= SLD_GET( TEXT("main.slider.bright") );
		if(pSldSpeed)
		{
			float fSpeed = this->m_pUIControl->GetSpeedFromPos( pSldSpeed->GetPos() );
			m_multiPlayer.SetPlaySpeed( fSpeed );
		}

		if(pSldBright){
			m_multiPlayer.SetBrightness(MP_VIDEO_CH_1, (pSldBright->GetPos()) * 10 );

			if(m_multiPlayer.GetStreamCount() > 1)
				m_multiPlayer.SetBrightness(MP_VIDEO_CH_2, (pSldBright->GetPos()) * 10);
		}
	}

	if( m_textData.m_GSensorList.size() <= 0 )
	{
//								if( m_pGSensorView )
//									m_pGSensorView->ShowWindow(SW_HIDE);
	}


	//::Sleep(200);
	this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_MEDIA_PLAY );
	
#ifdef TIME_SEARCH_USE
	ShowWindowTimeSearch(SW_HIDE);
#endif	
	return true;
}

BOOL CBBPlayerDlg::PlayerFileListIndex(INT index)
{
	ClListCtrl* pFileList = LST_GET( TEXT("main.listbox.filelist") );
	if( pFileList && pFileList->GetItemCount() > index && index >= 0 )
	{
		m_nCurPlayIndex = index;
		CString strFilename = pFileList->GetItemText(m_nCurPlayIndex, 3);
		DWORD init_indexno = pFileList->GetItemData(m_nCurPlayIndex);
		if( strFilename.GetLength() )
		{
			UpdateIndexFileList(index);
			
			if( OpenFile(strFilename, init_indexno) )
			{
				this->StartPlay();
			}
			else
			{
				this->StopMedia();
			}

			pFileList->EnsureVisible( m_nCurPlayIndex, FALSE );

#ifdef TIME_SEARCH_USE
			CTime time(pFileList->GetItemData(index));
			UpdateGraphDB(&time);
#endif			
		}

		return TRUE;
	}
	else
	{
		// Video 가 UI 영역에 Display되는 문제로 임시로 추가
		//this->InvalidateRect( NULL );
	}

	return FALSE;
}


UINT CBBPlayerDlg::FileSaveThreadWork(LPVOID pParam)
{
	CBBPlayerDlg *pInst = (CBBPlayerDlg*)pParam;
	BOOL bFileSaveOk = FALSE;
	
	pInst->m_bFileSaveThreadWorking = TRUE;

	if(pInst->m_bIsRemovalNxfs && !(pInst->m_cstrSaveCurrentFileName.Find(TEXT(".mp4")) > 0 || pInst->m_cstrSaveCurrentFileName.Find(TEXT(".avi")) > 0)){
		ClListCtrl* pFileList = (ClListCtrl*)pInst->m_pDialogManager->GetControl( TEXT("main.listbox.filelist") );

		if(pFileList){
			DWORD init_indexno = pFileList->GetItemData(pInst->m_nCurPlayIndex);
			pInst->m_fileManager.copy2Desktop(init_indexno, NULL, NULL);
			bFileSaveOk = TRUE;
		}
	}
	else
	if(pInst->m_nStreamCount >= 2)
	{
		bFileSaveOk = pInst->m_fileManager.fileSeparate(pInst->m_cstrSaveCurrentFileName, pInst->m_cstrSaveFileExt, pInst->m_bIsRemovalNxfs);

#ifdef REAR_CAM_OFF //원본파일 삭제   jun 170406
		if(pInst->m_bIsRemovalNxfs && bFileSaveOk){
			CFile::Remove(pInst->m_cstrSaveCurrentFileName.GetBuffer());
		}
#else

#endif
	}


	if(bFileSaveOk == FALSE &&   pInst->m_cstrSaveCurrentFileName.Compare(pInst->m_cstrSaveDstFile) != FALSE )
	{
		SHFILEOPSTRUCT stFileOP;
		pInst->m_cstrSaveCurrentFileName += TEXT('\0');
		pInst->m_cstrSaveDstFile += TEXT('.');
		pInst->m_cstrSaveDstFile += pInst->m_cstrSaveFileExt;

		ZeroMemory( &stFileOP, sizeof(SHFILEOPSTRUCT) );
		stFileOP.hwnd	= pInst->GetSafeHwnd();
		stFileOP.wFunc	= FO_COPY;
		stFileOP.fFlags = FOF_NOCONFIRMATION;
		stFileOP.pFrom	= pInst->m_cstrSaveCurrentFileName;
		stFileOP.pTo	= pInst->m_cstrSaveDstFile;

		if( SHFileOperation(&stFileOP) )
		{
			CLD_FL( DF_ERROR, "File copying failed." );
			//OPEN_NOTIFY_DIALOG( NOTIFY_ERR_COPY_FILE ,&this->m_pUIControl->m_Language);
		}
	}
	else
	{
		//OPEN_NOTIFY_DIALOG( NOTIFY_ERR_COPY_FILE ,&this->m_pUIControl->m_Language );
	}
			
	pInst->m_bFileSaveThreadWorking = FALSE;
	pInst->mp_SaveThread = FALSE;
	return TRUE;
}

VOID CBBPlayerDlg::SetUnitOfSpeed(VOID)
{
#if 0
	CString csDrive = this->m_cstrSelectDrive.Left(3);
	ST_CFG_ITEM stSetting;
	csDrive.Append(BLACKBOX_CONFIG_FILE);
	
	if( CConfigText::Load(csDrive, &stSetting))
		SetUnitOfSpeed(stSetting.stDisplay.eUnitOfSpeed);
	else
		SetUnitOfSpeed(m_eUnitOfSpeed);
#endif
}

VOID CBBPlayerDlg::SetUnitOfSpeed(ENUM_UNIT_OF_SPEED eUnitOfSpeed, BOOL bWriteConfig /*= FALSE*/)
{
	if(eUnitOfSpeed >= UNIT_ENUM_END)
		eUnitOfSpeed = UNIT_KM_P_H;
	
	m_eUnitOfSpeed = eUnitOfSpeed;

	if(bWriteConfig)
	{
		CString csDrive = this->m_cstrSelectDrive.Left(3);
		ST_CFG_ITEM stSetting;
		csDrive.Append(BLACKBOX_CONFIG_FILE);

#if 0		
		if( CConfigText::Load(csDrive, &stSetting))
		{
			csDrive = this->m_cstrSelectDrive.Left(2);
			stSetting.stDisplay.eUnitOfSpeed = m_eUnitOfSpeed;
			CConfigText::Save(csDrive, &stSetting);
		}
#else
		this->m_ConfigViewer.SetConfig(CConfig::UNIT_OF_SPEED, (BYTE)m_eUnitOfSpeed);
#endif

	}
	CGSensorView* m_pGSensorView = GSG_GET( TEXT("main.gsensor.graph") );
	if(m_pGSensorView) m_pGSensorView->SetSpeedRange( (int)m_eUnitOfSpeed );

	ClWebBrowser*	pMap			= MAP_GET( TEXT("main.web.map") );
	if(pMap)	pMap->SetUnitOfSpeed(m_eUnitOfSpeed);

	CDigitNum* pDigit		= DIGIT_GET(TEXT("main.digit.speed"));
	if(pDigit)		pDigit->SetUnit((int)m_eUnitOfSpeed);

}


// CBBPlayerDlg message handlers

BOOL CBBPlayerDlg::OnInitDialog( VOID )
{
	ClDialog::OnInitDialog();

	if( !this->m_pDialogManager )
	{
		CLD_FL( DF_ERROR, "Main Dialog can not CLUDING! You must call 'OpenCLUDE' before 'DoModal' or 'ShowWindow'." );
		return FALSE;
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	HWND hWnd = GetSafeHwnd();
	LPITEMIDLIST ppidl;

	if( SHGetSpecialFolderLocation(hWnd, CSIDL_DESKTOP, &ppidl) == NOERROR )
	{
		SHChangeNotifyEntry shCNE;
		shCNE.pidl = ppidl;
		shCNE.fRecursive = TRUE;

		ULONG uiSHChangeNotifyRegister = SHChangeNotifyRegister( hWnd,
			SHCNE_DISKEVENTS, SHCNE_DRIVEADD | SHCNE_DRIVEREMOVED, 
			WM_USER_DRIVECHANGED, 1, &shCNE);

		//ASSERT(uiSHChangeNotifyRegister != 0);	// Shell notification failed
	}
	else
	{
		ASSERT(FALSE);    // Failed to get desktop location
	}

	// UAC에서 관리자 권한으로 실행할 경우 아래 내용이 있어야 함.
	ChangeWindowMessageFilter(0x0049, MSGFLT_ADD);
	ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
	DragAcceptFiles(TRUE);
	 

	this->m_fileManager.setParentWindow( hWnd );

	this->m_fileManager.addDedicatedDir( TEXT("INFINITE") );
	this->m_fileManager.addDedicatedDir( TEXT("EVENT") );
	this->m_fileManager.addDedicatedDir( TEXT("MOTION") );
	this->m_fileManager.addDedicatedDir( TEXT("SELECT") );
	
	this->m_fileManager.addExtFilter( TEXT("avi") );
	this->m_fileManager.addExtFilter( TEXT("mp4") );
	this->m_fileManager.addExtFilter( TEXT("enc") );

	this->m_fileManager.setRecursiveScan( TRUE, FALSE );


	m_multiPlayer.Initialize( 2, GetDlgItem(IDC_MAIN_VIDEO_FRONT)->m_hWnd, GetDlgItem(IDC_MAIN_VIDEO_REAR)->m_hWnd );
#if TEXT_DATA_THREAD_USE
	m_multiPlayerForTextRead.Initialize( 1, GetDlgItem(IDC_MAIN_WEB_MAP)->m_hWnd);
	//m_multiPlayerForTextRead.OnSize();
#endif

	this->m_pUIControl->InitExchangeGeometry();
	this->SetCurrentMode( 0 );

	this->m_pUIControl->InitTooltip();
	this->m_pUIControl->InitLanguage();
	
//	DWORD dwConnectionTypes;
//	if( !InternetGetConnectedState(&dwConnectionTypes, 0) )
//		OPEN_NOTIFY_DIALOG( NOTIFY_ERR_NO_INTERNET ,&this->m_pUIControl->m_Language);

	ClWebBrowser* pMap = MAP_GET( TEXT("main.web.map") );
	if( pMap ){
		pMap->SetSensorData( &this->m_textData);
	}

	m_pGSensorView = GSG_GET( TEXT("main.gsensor.graph") );
	if(m_pGSensorView)
	{
		CRect cRectGsensor(0,0,200,50);
		GetDlgItem(IDC_MAIN_GSENSOR_VIEW)->GetWindowRect(&cRectGsensor);
		m_pGSensorView->DestroyWindow();
		m_pGSensorView->Create(WS_VISIBLE | WS_CHILD, cRectGsensor, this) ;
		m_pGSensorView->SetSeekMessageId(CM_GSENSOR_VIEW_SEEK_FRAME);
	}

	CDigitNum* pDigit		= DIGIT_GET(TEXT("main.digit.speed"));
	CAnalogMeter *pAMeter= AMETER_GET( TEXT("main.analogmeter.rpm") );
	if(pAMeter) pAMeter->ShowMeter();
	if(pDigit) pDigit->DrawDigits(0 , 1);
	
	this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_INIT );
	this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_LIST_NORMAL );
	this->CheckFilters();

	this->m_ConfigViewer.LoadConfig();
	this->m_ConfigViewer.GetConfig(CConfig::PATH_LAST, &this->m_cstrSelectDrive);
	this->m_ConfigViewer.GetConfig(CConfig::FILE_SAVE_LAST, &this->m_cstrMp4SavePath);
	this->m_ConfigViewer.GetConfig(CConfig::JPG_SAVE_LAST, &this->m_cstrJpgSavePath);

	this->m_fileManager.m_pcstrMp4SavePath = &this->m_cstrMp4SavePath;

#ifdef BUILD_PASSWORD_USE
	CPasswordDlg pwDlg;
	this->m_ConfigViewer.GetConfig(CConfig::LOGIN_PASSWORD, &m_cstrPassword);
	pwDlg.SetLanguage(&this->m_pUIControl->m_Language);
	pwDlg.SetPassword(m_cstrPassword.GetBuffer(), DEFAULT_MASTERKEY, DEFAULT_MASTERKEY); // 180626 master key
	if(pwDlg.DoModal() != IDOK) // check error
	{
		EndDialog( IDOK );
		return FALSE;	
	}
#endif

	SetVolume(80);

	this->m_ConfigViewer.GetConfig(CConfig::CAMERA_FLIP_MIRROR, &m_bFrontFlip, &m_bFrontMirror, &m_bRearFlip, &m_bRearMirror);
	this->m_ConfigViewer.GetConfig(CConfig::UNIT_OF_SPEED, (BYTE *)&m_eUnitOfSpeed);
	if(m_eUnitOfSpeed >= UNIT_ENUM_END)
		m_eUnitOfSpeed = UNIT_KM_P_H;

	ClButton* 		pBtn;
	pBtn = BTN_GET( TEXT("main.button.frontflip") );
	if(pBtn)	
		pBtn->SetCheckState(m_bFrontFlip);
	
	pBtn = BTN_GET( TEXT("main.button.frontmirror") );
	if(pBtn)	
		pBtn->SetCheckState(m_bFrontMirror);

	pBtn = BTN_GET( TEXT("main.button.rearflip") );
	if(pBtn)	
		pBtn->SetCheckState(m_bRearFlip);
	
	pBtn = BTN_GET( TEXT("main.button.rearmirror") );
	if(pBtn)	
		pBtn->SetCheckState(m_bRearMirror);

#ifdef RDF_USE
	ClVideoView*		pVideo = VID_GET( TEXT("main.video.front"));
	
	if(pVideo)
		pVideo->GetWindowRect(&m_rVideoFront);
#endif

	UpdateData(FALSE);

#ifdef TIME_SEARCH_USE
//	m_ctrlDate.SetCalID(CAL_JAPAN);
	m_ctrlDate.SetMonthView();
	m_ctrlDate.SetColor(MCSC_BACKGROUND, RGB(0, 0, 0));
	m_ctrlDate.SetColor(MCSC_MONTHBK, RGB(0, 0, 0));
	m_ctrlDate.SetColor(MCSC_TEXT , RGB(180, 180, 180));

 	m_ctrlDate.SetColor(MCSC_TITLEBK, RGB(100, 100, 100));
 	m_ctrlDate.SetColor(MCSC_TRAILINGTEXT, RGB(50, 50, 50));
 	m_ctrlDate.SetColor(MCSC_TITLETEXT, RGB(255, 255, 255));
 
	m_ctrlTime.SetMonthCalColor(MCSC_BACKGROUND, RGB(0, 0, 0));
	m_ctrlTime.SetMonthCalColor(MCSC_MONTHBK, RGB(0, 0, 0));
	m_ctrlTime.SetMonthCalColor(MCSC_TEXT , RGB(255, 255, 255));

 	m_ctrlTime.SetMonthCalColor(MCSC_TITLEBK, RGB(0, 0, 0));
 	m_ctrlTime.SetMonthCalColor(MCSC_TRAILINGTEXT, RGB(255, 2255, 255));
 	m_ctrlTime.SetMonthCalColor(MCSC_TITLETEXT, RGB(255, 255, 255));

	__m_isTimeBarEnabled	= TRUE;

	CICDBGraph::ITEM	item[60];
	//::ZeroMemory(item, 60*sizeof(CICDBGraph::ITEM));
	memset((void *)item, 0x81, 60 * sizeof(CICDBGraph::ITEM));


	int i;
	for( i = 0; i < _GID_MAX; i++)
	{
  #ifdef RDF_USE
  		m_ctrlICDBGraph[i].SetGraph( i, 2);
  #else
		m_ctrlICDBGraph[i].SetGraph( i, 1);
  #endif
		m_ctrlICDBGraph[i].SetColor(_CP_CONTINUOUS,		_CONTINUOUS_COLOR);
		m_ctrlICDBGraph[i].SetColor(_CP_MOTION,		_MOTION_COLOR);
		m_ctrlICDBGraph[i].SetColor(_CP_GSENSOR,			_GSENSOR_COLOR);
		m_ctrlICDBGraph[i].SetColor(_CP_BUTTON,	_BUTTON_COLOR);
		m_ctrlICDBGraph[i].SetColor(_CP_PREEVENT,		_PREEVENT_COLOR);

  #ifdef RDF_USE
  		m_ctrlICDBGraph[i].SetGraphItem(1, (i == _GID_HOUR) ? 24 : 60, item);
  #endif
		m_ctrlICDBGraph[i].SetGraphItem(0, (i == _GID_HOUR) ? 24 : 60, item);
  
	}
#endif

	CConfigText::SaveVersion(CBBoxUtil::GetModuleDir(), _SW_VERSION);
	

 
	int iCnt = 0;
 	LPWSTR* pStr = CommandLineToArgvW( GetCommandLine(), &iCnt);
	 
	if(iCnt > 0) {
		OnAddFileFromArguments(iCnt, (LPARAM)pStr[1]);
	}
	LocalFree(pStr);

	ClListCtrl* pFileList = LST_GET( TEXT("main.listbox.filelist") );
		
	if(pFileList && pFileList->GetItemCount() == 0)
		this->OpenDisk(TRUE);

 
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBBPlayerDlg::OnPaint( VOID )
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
 		CPaintDC dc(this); // device context for painting

#if BUILD_MODEL == MODEL_RALLY_EYES
		if(m_pUIControl) m_pUIControl->Redraw();
#endif
		if( m_multiPlayer.GetState() == STATE_RUNNING)	
			m_multiPlayer.OnDraw(&dc);
		//else											
			//m_multiPlayer.OnSize();   //20200729 zoom On/Off 프레임 이동 문제가 있어 삭제

		m_multiPlayer.UpdateWindow();
		//ClDialog::OnPaint();
	}

	return;
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CBBPlayerDlg::OnQueryDragIcon( VOID )
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CBBPlayerDlg::OnSize( UINT nType, int cx, int cy )
{
	ClDialog::OnSize(nType, cx, cy);
	m_multiPlayer.OnSize();

	return;
}


void CBBPlayerDlg::OnTimer( UINT_PTR nIDEvent )
{
	short keyState = GetKeyState(VK_LBUTTON);

	if ( (nIDEvent == MAINDLG_TID_PROGRESS) && !(keyState & 0x80) )
	{
		this->KillTimer( MAINDLG_TID_PROGRESS );
		
		int nPlayState = m_multiPlayer.GetState();
		LONGLONG nCurrentTime = m_multiPlayer.GetCurrentPosition();
		LONGLONG nDurationTime = m_multiPlayer.GetDuration();

		if( nPlayState != STATE_CLOSED && nPlayState >= 0 )
		{
			//printf("#### %llu, %llu, %d \r\n", nCurrentTime, nDurationTime, nPlayState);
			if(nCurrentTime >= nDurationTime || nCurrentTime < 0 )
			{
				if(PlayerFileListIndex(m_nCurPlayIndex + 1) == FALSE)
				{
					this->StopMedia();
					//m_nCurPlayIndex = 0;
					//SetTimer( MAINDLG_TID_PROGRESS, MAINDLG_TMDUR_PROGRESS, NULL );
				}
			}
			else
			{
				this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_MEDIA_PROGRESS );
				this->SetTimer( MAINDLG_TID_PROGRESS, MAINDLG_TMDUR_PROGRESS, NULL );
			}
		}
	}
	else if(nIDEvent == MAINDLG_TID_ZOOMREDRAW)
	{
		this->KillTimer( MAINDLG_TID_ZOOMREDRAW );
		this->SendMessage( CM_ZOOM_REDRAW, 0, 0);
	}
	else if(nIDEvent == MAINDLG_TID_MUXREDRAW)
	{
		if(m_MuxViewCurrentTime != this->m_multiPlayer.GetCurrentPosition()) {
			this->KillTimer( MAINDLG_TID_MUXREDRAW );
			this->SendMessage( CM_MUX_REDRAW, 33, 0);
		}
	}
	else if(nIDEvent == MAINDLG_TID_MAINREDRAW)
	{
		this->KillTimer( MAINDLG_TID_MAINREDRAW );
		this->InvalidateRect( NULL );
	}
	else if(nIDEvent == MAINDLG_TID_ACTIVEDRAW)
	{
		this->KillTimer( MAINDLG_TID_ACTIVEDRAW );
		this->m_pUIControl->SetActiveItem();
	}

	ClDialog::OnTimer(nIDEvent);
}

void CBBPlayerDlg::OnDropFiles(HDROP hDropInfo)
{
	int nFiles;
	int nFirstAddFile = 0;
	TCHAR szPathName[MAX_PATH];
    
   	ClListCtrl* pFileList = LST_GET( TEXT("main.listbox.filelist") );

	if(pFileList == NULL)
		return;
	
	// 드롭된 파일의 갯수
	nFiles = DragQueryFile(hDropInfo, 0xFFFFFFFF, szPathName, MAX_PATH);

    for(int i = 0; i < nFiles; ++i)
    {
		DragQueryFile(hDropInfo, i, szPathName, MAX_PATH);     // 파일의 경로 얻어옴

		CString str(szPathName) ;
       	
		int count;
		for( count = 0; count < pFileList->GetItemCount(); count++)
		{
			CString strItem3 = pFileList->GetItemText(count, 3); // full path
			if(str.Find(strItem3) >= 0)
			{
				pFileList->SetItemState( count, LVIS_SELECTED|LVIS_DROPHILITED, LVIS_SELECTED|LVIS_DROPHILITED );
				break;
			}
		}
			
        if(count >= pFileList->GetItemCount())
        {
        	//CString cstrFileName(CBBoxUtil::GetFileName(str));
        	//if(cstrFileName.Find(TEXT(".mp4")) > 0 || cstrFileName.Find(TEXT(".avi")) > 0)
        	std::wstring wtsExt = ClUtil::PATH::getExtSpecW( str );
			if( ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->isExistExtension( wtsExt.c_str() ) )
        	{
				WIN32_FIND_DATA	findData;
				HANDLE			hDir = NULL;

				hDir = FindFirstFileEx( str, FindExInfoStandard, &findData, FindExSearchNameMatch, NULL, 0 );
				if( hDir == INVALID_HANDLE_VALUE )
					continue;

				m_cstrSelectDrive = str.Left(str.ReverseFind( '\\' ));
				FileAddToFileList(CBBoxUtil::GetDir(str), &findData);
				FindClose(hDir);

				if(nFirstAddFile == 0)
				{
					nFirstAddFile = pFileList->GetItemCount();
	        	}
			}
		}
   	}

	if(nFirstAddFile)
	{
		pFileList->SetItemState( nFirstAddFile-1, LVIS_SELECTED|LVIS_DROPHILITED, LVIS_SELECTED|LVIS_DROPHILITED );
		this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_LIST_NORMAL );
		this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_SEL_DRIVE );
		pFileList->SetRedraw( TRUE );
	}
 	CDialog::OnDropFiles(hDropInfo);
}


void CBBPlayerDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	return CDialog::OnMouseMove(nFlags, point);
}

BOOL CBBPlayerDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	ClListCtrl*		pFileList = LST_GET( TEXT("main.listbox.filelist") );
	ClButton* 		pBtnZoom 	= BTN_GET( TEXT("main.button.zoom") );
	ClVideoView* 		pZoomVideo =  VID_GET( TEXT("main.video.zoom") );
	CRect 			fileListRect(0,0,0,0);
	CRect 			zoomRect(0,0,0,0);
	CRect			gsensorViewRect(0,0,0,0);

	//printf("OnMouseWheel : %d %d (x:%d, y:%d)\r\n", nFlags,zDelta, pt.x, pt.y);

	if(pFileList)
	{
		pFileList->GetWindowRect(fileListRect);
	}

	if(pBtnZoom)
	{
		if(pBtnZoom->GetCheckState())
		{
			if(pZoomVideo)	pZoomVideo->GetWindowRect(zoomRect);
		}
	}

	if(m_pGSensorView)
	{
		if(m_pGSensorView->IsWindowVisible())
			m_pGSensorView->GetWindowRect(gsensorViewRect);
	}

	
	if( gsensorViewRect.PtInRect(pt) )
	{
		if(m_pGSensorView) 	m_pGSensorView->SendMessage(WM_MOUSEWHEEL, MAKEWPARAM(nFlags, zDelta), MAKELPARAM(pt.x, pt.y));
	}
	else  if( zoomRect.PtInRect(pt) )
	{
		if(pZoomVideo) 	pZoomVideo->SendMessage(WM_MOUSEWHEEL, MAKEWPARAM(nFlags, zDelta), MAKELPARAM(pt.x, pt.y));
	}
	else if(fileListRect.PtInRect(pt))
	{
		if(pFileList)	pFileList->SendMessage(WM_MOUSEWHEEL, MAKEWPARAM(nFlags, zDelta), MAKELPARAM(pt.x, pt.y));
	}
#if 0
	else // default volume up/down
	{
		if(zDelta > 0)
			SetVolume(m_nVolumePosition + 5);
		else
			SetVolume(m_nVolumePosition - 5);			
	}
#endif	
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CBBPlayerDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	ClVideoView*		pVideoFront = VID_GET( TEXT("main.video.front"));
	ClVideoView*		pVideoRear = VID_GET( TEXT("main.video.rear"));
	ClWebBrowser*	pMap			= MAP_GET( TEXT("main.web.map") );

	ClText* pTextLatitude = TXT_GET( TEXT("main.text.latitude") );
	ClText* pTextLongitude = TXT_GET( TEXT("main.text.longitude") );

	ClButton* pButtonLog = BTN_GET( TEXT("main.button.log") );

	CRect			cur_window(0,0,0,0);
	CRect 			videofrontRect(0,0,0,0);
	CRect 			videorearRect(0,0,0,0);
	CRect			mapRect(0,0,0,0);
	CRect			lonRect(0,0,0,0);
	CRect			latRect(0,0,0,0);
	CRect			LogButtonRect(0,0,0,0);
	
	CPoint abs_point;
		
	//printf("OnLButtonDblClk : %d (x:%d, y:%d)\r\n", nFlags, point.x, point.y);

	this->GetWindowRect(cur_window);
	abs_point.x = cur_window.left + point.x;
	abs_point.y = cur_window.top + point.y;
	
	if(pVideoFront)
		pVideoFront->GetWindowRect(videofrontRect);

	if(pVideoRear)
		pVideoRear->GetWindowRect(videorearRect);

	if(pMap)
		pMap->GetWindowRect(mapRect);

	if(pTextLatitude)
		pTextLatitude->GetWindowRect(latRect);
	if(pTextLongitude)
		pTextLongitude->GetWindowRect(lonRect);

	// Hidden Log버튼
	if(pButtonLog){
		pButtonLog->GetWindowRect(LogButtonRect);
		if( LogButtonRect.PtInRect(abs_point)){
			OnClickLog();
			return;
		}
	}
	
	if( videorearRect.PtInRect(abs_point))
		this->m_pUIControl->m_ActiveFront = this->m_pUIControl->m_MaxScreenActive = CBBPlayerUIControl::ITEM_REAR_VIDEO;
	else if( mapRect.PtInRect(abs_point) || latRect.PtInRect(abs_point) || lonRect.PtInRect(abs_point))
		this->m_pUIControl->m_ActiveFront = this->m_pUIControl->m_MaxScreenActive = CBBPlayerUIControl::ITEM_MAP;
	else {
		ClVideoView*		pVideoCtrl= VID_GET( TEXT("main.video.quadctrl"));
		if(pVideoCtrl && pVideoCtrl->GetQuadViewMode()) {
			this->m_pUIControl->m_ActiveFront = this->m_pUIControl->m_MaxScreenActive = CBBPlayerUIControl::ITEM_REAR_VIDEO;
		}
		else
			this->m_pUIControl->m_ActiveFront = this->m_pUIControl->m_MaxScreenActive = CBBPlayerUIControl::ITEM_FRONT_VIDEO;
	}
		
	if(this->m_nCurMode == 0)
		OnClickMaximize();	
	else
		OnClickNormalize();
	
	ClDialog::OnLButtonDblClk(nFlags, point);
}

BOOL CBBPlayerDlg::OnCopyData( CWnd * pWnd, COPYDATASTRUCT* pcds )
{
	switch ( pcds->dwData ) {
		case 27001:
		{
			//LPWSTR pStr = (LPWSTR)pcds->lpData;
			//AfxMessageBox( pStr);

			this->ShowWindow( SW_RESTORE  ); //20201030	Pai-r No74 Issue patch
			
			
			OnAddFileFromArguments(2, (LPARAM)pcds->lpData);
		}
		break;
	}
	
	return ClDialog::OnCopyData(pWnd, pcds);
}

LRESULT CBBPlayerDlg::OnAddFileFromArguments( WPARAM wParam, LPARAM lParam )
{
	int folder_depth = 0;
	TCHAR file_url[256];
#if 0	
	DWORD dwSize = 256;
	
	BOOL IsNetPath = ::PathIsNetworkPath((TCHAR *)lParam);

	if(::PathCreateFromUrl((TCHAR *)lParam, file_url, &dwSize, 0) == S_OK){
		AfxMessageBox(file_url);
	}
		
	CString cstrMessage;
	cstrMessage.Format( TEXT("%s (%d)"), (TCHAR *)lParam, (int)IsNetPath );
		
	AfxMessageBox( cstrMessage);
	

	//Uri u0 = Uri::Parse(std::wstring((TCHAR *)lParam));
	//cstrMessage.Format( TEXT("%s, %s, %s (%d)"), u0.QueryString, u0.Protocol,  u0.Host, u0.Port);
	//AfxMessageBox( cstrMessage);
#endif
	
	lstrcpy(file_url, (TCHAR *)lParam);
	
	TCHAR * file_path = _tcsstr(file_url, _T("///"));
	int len;
	
	if(file_path) {
		file_path += 3;
	}
	else {
		file_path = _tcsstr(file_url, _T("//"));

		if(file_path)
			file_path += 2;
		else
			file_path = file_url;
	}

	len = _tcslen(file_path);
	for(int i = 0; i < len; i++){
		if(file_path[i] == _T('/')) {
			if(i == len -1){
				file_path[i] = '\0';
			}
			else {
				file_path[i] = '\\';
				folder_depth++;
			}
		}
		else if(file_path[i] == _T('\\')){
			folder_depth++;
		}
	}

	if(wParam > 0) {
	    CString str(file_path);

		if(folder_depth == 0){
			LPWSTR wszPath = NULL;
			if( ::SHGetKnownFolderPath(FOLDERID_Downloads, KF_FLAG_CREATE, NULL, &wszPath) == S_OK) {
				str.Format(TEXT("%s\\%s"), wszPath, file_path);
			}
			 
			::CoTaskMemFree(wszPath);
		}

		std::wstring wtsExt = ClUtil::PATH::getExtSpecW( str );
		if( ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->isExistExtension( wtsExt.c_str() ) ) 	{
		//if(str.Find(TEXT(".mp4")) >= 0 || str.Find(TEXT(".avi")) >= 0 || str.Find(TEXT(".fvfs")) >= 0){
			WIN32_FIND_DATA	findData;
			HANDLE			hDir = NULL;

			hDir = FindFirstFileEx( str, FindExInfoStandard, &findData, FindExSearchNameMatch, NULL, 0 );
			if( hDir != INVALID_HANDLE_VALUE ) {			

				ClListCtrl* pFileList = LST_GET( TEXT("main.listbox.filelist") );

				int count;
				BOOL add = TRUE;

				// 파일 중복 검사
				for( count = 0; count < pFileList->GetItemCount(); count++)
				{
					CString strItem3 = pFileList->GetItemText(count, 3); // full path
					if(str.Find(strItem3) == 0){
						PlayerFileListIndex(count);

						add = FALSE;
					}
				}

				if(add) {
					m_cstrSelectDrive = str.Left(str.ReverseFind( '\\' ));
					FileAddToFileList(CBBoxUtil::GetDir(str), &findData);
					PlayerFileListIndex(pFileList->GetItemCount() - 1);
				}
				
				FindClose(hDir);			
			}
		}
		else {
			this->m_cstrSelectDrive = str;
			this->OpenDisk();
			//OpenDirectory(str, true);
		}
		
	}

	return 0;
}

LRESULT CBBPlayerDlg::OnDriveChanged( WPARAM wParam, LPARAM lParam )
{
	struct SHNOTIFYSTRUCT{ DWORD dwItem1; DWORD dwItem2; };
	SHNOTIFYSTRUCT *shns = (SHNOTIFYSTRUCT *)wParam;
	CString strPath, strMsg;

	switch(lParam)
	{
	case SHCNE_DRIVEREMOVED:
		{
			strPath = GetPathFromPIDL(shns->dwItem1);
			if( !strPath.IsEmpty() && strPath == m_cstrSelectDrive )
			{
				KillTimer(MAINDLG_TID_PROGRESS);
				m_multiPlayer.Release();				
				m_cstrSelectDrive.Empty();
				this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_DROP_DRIVE );
				OPEN_NOTIFY_DIALOG( NOTIFY_ERR_REMOVE_DISK ,&this->m_pUIControl->m_Language);
				((CBBPlayerApp*)AfxGetApp())->GetConfigure()->clearConfig();
			}
		}
		break;

/*		case SHCNE_MEDIAINSERTED:	break;
		case SHCNE_MEDIAREMOVED:	break;
		case SHCNE_DRIVEADD:		break;*/
	}

	return NULL;
}


LRESULT CBBPlayerDlg::OnSliderScroll( WPARAM wParam, LPARAM lParam )
{
	ClSlider* pSlider = (ClSlider*)wParam;
	INT nPos = (INT)lParam;

	ClSlider* pSldTime	= SLD_GET( TEXT("main.slider.time") );
#ifdef AUDIO_VOLUME_CONTROL_USE
	ClSlider* pSldVloume= SLD_GET( TEXT("main.slider.volume") );
#endif
	ClSlider* pSldSpeed	= SLD_GET( TEXT("main.slider.speed") );
	ClSlider* pSldBright= SLD_GET( TEXT("main.slider.bright") );

	if( pSldTime == pSlider )
	{
		m_multiPlayer.SetPosition( (long)nPos );
		this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_MEDIA_PROGRESS );
#if !SEEK_ZOOM_DO_NOT_USE
		if( m_multiPlayer.GetState() == PLAY_STATE_PAUSED )
			this->SendMessage( CM_ZOOM_REDRAW, 1, 0);
#endif
	}
	else if( pSldSpeed == pSlider )
	{
		if( m_multiPlayer.GetState() == PLAY_STATE_RUNNING )
		{
			ClText* pTextSpeed = TXT_GET( TEXT("main.text.speed") );
			CString csSpeed;
			
			float fSpeed = this->m_pUIControl->GetSpeedFromPos( nPos );
			m_multiPlayer.SetPlaySpeed( fSpeed );

			csSpeed.Format(TEXT("%0.1fx"), fSpeed);
			if(pTextSpeed)	pTextSpeed->SetWindowText(csSpeed);
		}
	}
#ifdef AUDIO_VOLUME_CONTROL_USE
	else if( pSldVloume == pSlider )
	{
		ClButton* pBtnVolume = BTN_GET( TEXT("main.button.volume") );
		if( nPos <= 0)
		{
			if( pBtnVolume ) pBtnVolume->SetCheckState();
		}
		else
		{
			if( pBtnVolume ) pBtnVolume->SetCheckState( FALSE );
		}
		this->SetVolume(nPos * 5);
	}
#endif
	else if( pSldBright == pSlider )
	{
		ClText* pTextBright = TXT_GET( TEXT("main.text.bright") );
		CString csBright;

			
		m_multiPlayer.SetBrightness( MP_VIDEO_CH_1,  (nPos) * 10);

		if(m_multiPlayer.GetStreamCount() > 1)
		{
			m_multiPlayer.SetBrightness( MP_VIDEO_CH_2,  (nPos) * 10);
		}

		csBright.Format(TEXT("%d"), (nPos) * 10);
		if(pTextBright)	pTextBright->SetWindowText(csBright);
	}

	return 0;
}


LRESULT CBBPlayerDlg::OnMediaForceStop( WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER( wParam );
	UNREFERENCED_PARAMETER( lParam );

	this->StopMedia();

	return 0;
}


void CBBPlayerDlg::OnClickSetting( VOID )
{
	const char* xmlFile = ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getXml( TEXT("SETTING") );
	if( !xmlFile || !strlen(xmlFile) )
	{
		CLD_FL( DF_ERROR, "XML file not ready(Target name : SETTING)" );
		return;
	}

	this->StopMedia();

	CSettingDlg settingDlg;
	settingDlg.SetLanguage(&this->m_pUIControl->m_Language);
#ifdef BUILD_PASSWORD_USE	// jun 200805
	settingDlg.SetPassword(m_cstrPassword);
#endif
	if( !settingDlg.OpenCludeBuffer(xmlFile) )	return;

	settingDlg.SetSelDrive( this->m_cstrSelectDrive );
	settingDlg.DoModal();
	
#ifdef BUILD_PASSWORD_USE	// jun 200805	
	m_cstrPassword = settingDlg.GetPassword();
#endif

	SetUnitOfSpeed();
	return;
}


void CBBPlayerDlg::OnClickLog( VOID )
{

	const char* xmlFile = ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getXml( TEXT("LOG") );
	if( !xmlFile || !strlen(xmlFile) )
	{
		CLD_FL( DF_ERROR, "XML file not ready(Target name : LOG)" );
		return;
	}

	CLogDlg logDlg;
	logDlg.SetLanguage(&this->m_pUIControl->m_Language);
	
	if( !logDlg.OpenCludeBuffer(xmlFile) )	return;

	logDlg.SetLogFile( this->m_cstrLogFilePath );
	logDlg.DoModal();

	return;
}


void CBBPlayerDlg::OnClickCapture( VOID )
{
	int nPlayState = m_multiPlayer.GetState();
	if (nPlayState == PLAY_STATE_RUNNING || nPlayState == PLAY_STATE_PAUSED)
	{
		m_multiPlayer.StreamPause();

		CBitmap bitmapFront;
		CBitmap bitmapRear;

		bitmapFront.DeleteObject();
		if ( m_multiPlayer.GetCaptureToDib(0, bitmapFront) )
		{
			CString strFilter = TEXT("jpeg (*.jpg)|*.jpg||");
			CString csFileName;
			ClListCtrl* pFileList = LST_GET( TEXT("main.listbox.filelist") );
			
			if(pFileList == NULL)
				return;
			
			std::wstring wtsFileName = ClUtil::PATH::getFileNameSpecW( pFileList->GetItemText( m_nCurPlayIndex, 3 ) );
			csFileName.Format(TEXT("%s_%d"),  wtsFileName.c_str(), m_multiPlayer.GetCurrentPosition());
			csFileName.Append(TEXT(".jpg"));
			CFileDialog dlg(false, NULL, csFileName, OFN_OVERWRITEPROMPT, TEXT("jpeg file (*.jpg)|*.jpg||"), this);

			dlg.m_ofn.lpstrInitialDir = m_cstrJpgSavePath.GetBuffer(); // 오픈할때 초기 경로 지정

			if ( dlg.DoModal() == IDOK)
			{
				CString strExtention = dlg.GetFileExt();
				CString strPath;// = dlg.GetFolderPath(); // GetFolderPath function windows xp os error 발생함

				std::wstring wtsPath = ClUtil::PATH::getPathSpecW( dlg.GetPathName().GetBuffer());
				m_cstrJpgSavePath.Format(TEXT("%s\\"), wtsPath.c_str());

				if (strExtention.GetLength() > 0)
					strPath = dlg.GetPathName().Left(dlg.GetPathName().GetLength() - 4);
				else
					strPath = dlg.GetPathName();

				CImage imageFront;
				imageFront.Attach(bitmapFront);
				CString strSaveFrontFilename = strPath + TEXT("_front.jpg");
				imageFront.Save(strSaveFrontFilename, Gdiplus::ImageFormatJPEG);

#ifdef REAR_CAM_OFF  //  jun 170406

	/*			if (m_multiPlayer.GetStreamCount() > 1)
				{
					bitmapRear.DeleteObject();
					if ( m_multiPlayer.GetCaptureToDib(1, bitmapRear) )
					{
						CImage imageRear;
						imageRear.Attach(bitmapRear);
						CString strSaveRearFilename = strPath + TEXT("_rear.jpg");
						imageRear.Save(strSaveRearFilename, Gdiplus::ImageFormatJPEG);
					}
				}
   */	
#else
                if (m_multiPlayer.GetStreamCount() > 1)
				{
					bitmapRear.DeleteObject();
					if ( m_multiPlayer.GetCaptureToDib(1, bitmapRear) )
					{
						CImage imageRear;
						imageRear.Attach(bitmapRear);
						CString strSaveRearFilename = strPath + TEXT("_rear.jpg");
						imageRear.Save(strSaveRearFilename, Gdiplus::ImageFormatJPEG);
					}
				}
#endif

			}
		}

		if( nPlayState == PLAY_STATE_RUNNING )
			m_multiPlayer.StreamPlay();
	}
	return;
}

void CBBPlayerDlg::OnClickAbout( VOID )
{
#if 0
	CAboutDlg aboutDlg;
	const char* xmlFile = ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getXml( TEXT("ABOUT") );
	if( !xmlFile || !strlen(xmlFile) )
	{
		CLD_FL( DF_ERROR, "XML file not ready(Target name : ABOUT)" );
		return;
	}

	aboutDlg.SetLanguage(&this->m_pUIControl->m_Language);
	aboutDlg.SetSelDrive(this->m_cstrSelectDrive);
		
	if( !aboutDlg.OpenCludeBuffer(xmlFile) ) return;

	// fw version load
	CString csDrive = this->m_cstrSelectDrive.Left(3);
	ST_CFG_DAVIEW stSetting;
	csDrive.Append(BLACKBOX_CONFIG_FILE);
	if( CConfigText::Load(csDrive, &stSetting))
	{
		TCHAR szVersion[256];
		if(ClUtil::STRING::convertChar2Wchar(szVersion, stSetting.strFWVersion, 256))
		{
			CString cstrVersion(szVersion);
			((CBBPlayerApp*)AfxGetApp())->GetConfigure()->setFirmVersion(cstrVersion.Left(64));
		}
	}
	
	aboutDlg.DoModal();
#elif 0
	bool	bSuccess;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset( &si, 0, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;

	ZeroMemory(&pi, sizeof(pi));
	bSuccess = CreateProcess(NULL,TEXT("patch.exe"),NULL,NULL,NULL,NORMAL_PRIORITY_CLASS,NULL,NULL,&si,&pi);


#else

    CFile patch_file;
	CString fileName = CBBoxUtil::GetModuleDir();
	fileName.Append(_T("\\"));
	fileName.Append(_T("patch.exe"));

 	if(patch_file.Open(fileName, CFile::modeRead | CFile::typeBinary, NULL) )
	{

		BYTE * buffer = new BYTE[4096 * 1024];
		int file_size;
		file_size = patch_file.Read(buffer, (4096 * 1024));

		if(file_size){
			CFile newPatch_file;
			if(newPatch_file.Open(TEXT("patch1.exe"), CFile::modeCreate | CFile::modeWrite)){
				newPatch_file.Write(buffer, file_size);
				newPatch_file.Close();
			}
		}

		delete buffer;

		patch_file.Close();
	}
	
   //CFile::Rename(TEXT("patch.exe"), TEXT("patch1.exe")); 
   WinExec("patch1.exe", SW_SHOW);
	 
#endif

	return;
}


void CBBPlayerDlg::OnClickSite( VOID )
{
	DWORD dwConnectionTypes;
	if( !InternetGetConnectedState(&dwConnectionTypes, 0) )
	{
		OPEN_NOTIFY_DIALOG( NOTIFY_ERR_NO_INTERNET ,&this->m_pUIControl->m_Language);
		return;
	}

	CString cstrWebSite = ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getCompanySite();
	::ShellExecute( NULL, TEXT("open"), cstrWebSite, NULL, NULL, SW_SHOWNORMAL );

	return;
}

void CBBPlayerDlg::OnClickMinimize( VOID )
{
	this->ShowWindow( SW_MINIMIZE );
	this->GetWindowRect(&this->m_pUIControl->m_rWindowRect);  //20200729
	return;
}

void CBBPlayerDlg::OnClickMaximize( VOID )
{
	if(this->m_nCurMode == 0)
	{
		ClVideoView*		pVideoCtrl = VID_GET( TEXT("main.video.quadctrl"));
		ClButton* pBtnPlay = BTN_GET( TEXT("main.button.play") );
		BOOL bReplay = FALSE;
		
		if(m_multiPlayer.GetState() == PLAY_STATE_RUNNING && pBtnPlay->GetCheckState())
		{
				m_multiPlayer.StreamPause();
				bReplay = TRUE;
		}
		
		if(pVideoCtrl && pVideoCtrl->GetQuadViewMode()) {
			pVideoCtrl->SetQuadViewMode(FALSE);
			this->KillTimer( MAINDLG_TID_MUXREDRAW );
		}
		
#ifdef TIME_SEARCH_USE
		ShowWindowTimeSearch(SW_HIDE);
#endif	

		// No 78 뷰어 후방 화면 확대 후 최대화 시 화면 겹치는 문제 수정
		if(this->m_pUIControl->m_ActiveFront <= CBBPlayerUIControl::ITEM_REAR_VIDEO)
			this->m_pUIControl->m_MaxScreenActive = this->m_pUIControl->m_ActiveFront;
		else
			this->m_pUIControl->m_MaxScreenActive = this->m_pUIControl->m_ActiveFront = CBBPlayerUIControl::ITEM_FRONT_VIDEO;


		this->EnableEasyMove( FALSE );
		this->ShowWindow( SW_MAXIMIZE );
		this->GetWindowRect(&this->m_pUIControl->m_rWindowRect); //20200729
		this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_MODE_CHANGE );
		this->SetCurrentMode( 1 );

		if(bReplay)
			m_multiPlayer.StreamPlay();
	}

	return;
}

void CBBPlayerDlg::OnClickNormalize( VOID )
{
	if(this->m_nCurMode == 1)
	{
		ClVideoView*		pVideoCtrl= VID_GET( TEXT("main.video.quadctrl"));
		ClButton* pBtnPlay = BTN_GET( TEXT("main.button.play") );
		BOOL bReplay = FALSE;
		
		if(m_multiPlayer.GetState() == PLAY_STATE_RUNNING && pBtnPlay->GetCheckState())
		{
				m_multiPlayer.StreamPause();
				bReplay = TRUE;
		}
			
		if(pVideoCtrl && pVideoCtrl->GetQuadViewMode()) {
			pVideoCtrl->SetQuadViewMode(FALSE);
			this->KillTimer( MAINDLG_TID_MUXREDRAW );
		}
		
		this->m_pUIControl->m_MaxScreenActive = CBBPlayerUIControl::ITEM_FRONT_VIDEO;
	  //this->m_pUIControl->m_ActiveFront = CBBPlayerUIControl::ITEM_FRONT_VIDEO;
		ClButton* pBtnFRChange = BTN_GET( this->m_pUIControl->m_Language.GetButtonText(TEXT("main.button.frchange")) );
	  
	 	 if(pBtnFRChange && pBtnFRChange->GetCheckState()){
	   	  //this->m_pUIControl->m_ActiveFront = CBBPlayerUIControl::ITEM_REAR_VIDEO;
	  	  //this->m_pUIControl->m_ActiveRear = CBBPlayerUIControl::ITEM_FRONT_VIDEO;
			this->m_pUIControl->m_ActiveFront = CBBPlayerUIControl::ITEM_FRONT_VIDEO;    //160625 jun 
	  		this->m_pUIControl->m_ActiveRear = CBBPlayerUIControl::ITEM_REAR_VIDEO;      //160625 jun 
		}
	  	else {
	   		this->m_pUIControl->m_ActiveFront = CBBPlayerUIControl::ITEM_FRONT_VIDEO;
	  		this->m_pUIControl->m_ActiveRear = CBBPlayerUIControl::ITEM_REAR_VIDEO;
	 	}

		this->m_pUIControl->m_ActiveMap = CBBPlayerUIControl::ITEM_MAP;

		this->EnableEasyMove( TRUE );
		this->ShowWindow( SW_NORMAL );
		this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_MODE_CHANGE );
		this->SetCurrentMode( 0 );

#ifdef TIME_SEARCH_USE
		ClButton* 		pBtnStop = BTN_GET( TEXT("main.button.stop") );
		if(pBtnStop)
		{
			if(pBtnStop->IsWindowEnabled())
				ShowWindowTimeSearch(SW_HIDE);
			else
				ShowWindowTimeSearch(SW_SHOW);
		}
#endif		

		if(bReplay)
			m_multiPlayer.StreamPlay();
	}

	
	return;
}

void CBBPlayerDlg::OnClickExit( VOID )
{
#if 0
	INT nReturn = IDOK;
	OPEN_SLECTION_DIALOG( nReturn, NOTIFY_QUES_WANT_QUIT ,&this->m_pUIControl->m_Language );
	if( nReturn == IDOK )
	{
		m_multiPlayer.StreamStop();
		m_multiPlayer.CloseFile();
		KillTimer(MAINDLG_TID_PROGRESS);

		EndDialog( IDOK );
	}
#else
	this->StopMedia();

	//ShowWindow(SW_HIDE);

	if(this->m_bFileSaveThreadWorking == TRUE &&  this->mp_SaveThread)
	{
		int delay_timeout = 1000;
		while(this->m_bFileSaveThreadWorking && delay_timeout--)
			::Sleep(10);
		
		delete mp_SaveThread;
		mp_SaveThread = NULL;
	}

#if TEXT_DATA_THREAD_USE
	if(m_pTextDataThread)
	{
		UINT delay_timeout = 1000;
		m_bTextDataThreadExit = TRUE;
		m_pTextDataThread->PostThreadMessage(THREAD_MSG_EXIT, 0, 0);

		while(this->m_bTextDataThreadWorking && delay_timeout--)
			::Sleep(10);

		if(this->m_bTextDataThreadWorking && m_pTextDataThread)
		{
			::Sleep(100);
			delete m_pTextDataThread;
			m_pTextDataThread = NULL;
			::Sleep(1);
		}
	}
	m_multiPlayerForTextRead.Release();
  #endif
  	m_multiPlayer.CloseFile();
  	m_multiPlayer.Release();			
	//Sleep(1000);
	
	this->m_ConfigViewer.SetConfig(CConfig::PATH_LAST, &this->m_cstrSelectDrive);
	this->m_ConfigViewer.SetConfig(CConfig::FILE_SAVE_LAST, &this->m_cstrMp4SavePath);
	this->m_ConfigViewer.SetConfig(CConfig::JPG_SAVE_LAST, &this->m_cstrJpgSavePath);
	this->m_ConfigViewer.SetConfig(CConfig::CAMERA_FLIP_MIRROR, m_bFrontFlip, m_bFrontMirror, m_bRearFlip, m_bRearMirror);
#ifdef BUILD_PASSWORD_USE
	this->m_ConfigViewer.SetConfig(CConfig::LOGIN_PASSWORD, &this->m_cstrPassword);
#endif
	this->m_ConfigViewer.SaveConfig();

	EndDialog( IDOK );
#endif
	return;
}

void CBBPlayerDlg::OnClickPrev( VOID )
{
	if (m_nCurPlayIndex <= 0) return;

	if(PlayerFileListIndex(m_nCurPlayIndex - 1) == FALSE)
	{
		this->StopMedia();
	}

	return;
}

void CBBPlayerDlg::OnClickPlay( VOID )
{
	int nPlayState = m_multiPlayer.GetState();

	if( nPlayState == PLAY_STATE_PAUSED )
	{
		
		this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_MEDIA_REPLAY );
		m_multiPlayer.StreamPlay();
		SetTimer( MAINDLG_TID_PROGRESS, MAINDLG_TMDUR_PROGRESS, NULL );
	}
	else if( nPlayState == PLAY_STATE_RUNNING )
	{
		m_multiPlayer.StreamPause();
		this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_MEDIA_PAUSED );
	}
	else
	{
		ClListCtrl* pFileList = LST_GET( TEXT("main.listbox.filelist") );
		if( pFileList && pFileList->GetItemCount() <= 0 )
			return;

		int nIndex = (int)pFileList->GetFirstSelectedItemPosition()-1;
		if( nIndex < 0 )
		{
			nIndex = 0;
			UpdateIndexFileList(nIndex);
		}

		if( nIndex != m_nCurPlayIndex )
			m_nCurPlayIndex = nIndex;
		
		if(PlayerFileListIndex(m_nCurPlayIndex) == FALSE)
		{
			this->StopMedia();
		}
	}

	return;
}

void CBBPlayerDlg::OnClickStop( VOID )
{
	this->m_pUIControl->UserDataSynchronize(this->m_multiPlayer.GetCurrentPosition() , TRUE);
	this->StopMedia();
	return;
}


void CBBPlayerDlg::OnClickNext( VOID )
{
	ClListCtrl* pFileList = LST_GET( TEXT("main.listbox.filelist") );
	if( pFileList && pFileList->GetItemCount() > m_nCurPlayIndex + 1){
		if(PlayerFileListIndex(m_nCurPlayIndex + 1) == FALSE)
		{
			this->StopMedia();
		}
	}

	return;
}


void CBBPlayerDlg::OnClickRew( VOID )
{
	//int nPlayState = m_multiPlayer.GetState();	  //jun 160611
	//if (nPlayState == PLAY_STATE_RUNNING || nPlayState == PLAY_STATE_PAUSED)  //jun 160611
	
	int nPlayState = m_multiPlayer.GetState();	  //jun 160611
	if(nPlayState != PLAY_STATE_RUNNING  && nPlayState != PLAY_STATE_PAUSED)  //jun 160611
		return;  //jun 160611
	
	if( nPlayState == PLAY_STATE_RUNNING )  //jun 160611
		m_multiPlayer.StreamPause();  //jun 160611
		
	{
		if(this->m_multiPlayer.GetCurrentPosition() < 1000 && m_nCurPlayIndex)
		{
			PlayerFileListIndex(m_nCurPlayIndex - 1);

			if(this->m_multiPlayer.GetDuration() > 1000)
				this->m_multiPlayer.SetPosition(this->m_multiPlayer.GetDuration() - 1000);
			else
				this->m_multiPlayer.SetPosition(this->m_multiPlayer.GetDuration());
		}
		else
		{
			m_multiPlayer.StreamBackward(2);
		}
		
		//this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_MEDIA_PROGRESS );  //jun 160611
		 this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_MEDIA_PAUSED );  //jun 160611
#if !SEEK_ZOOM_DO_NOT_USE
		//this->OnDrawZoomImage();
		this->SendMessage( CM_ZOOM_REDRAW, 1, 0);
#endif
	}

	return;
}


void CBBPlayerDlg::OnClickFF( VOID )
{
	int nPlayState = m_multiPlayer.GetState();
	if (nPlayState == PLAY_STATE_RUNNING || nPlayState == PLAY_STATE_PAUSED)
	{
		m_multiPlayer.StreamForward(1);
		this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_MEDIA_PROGRESS );
#if !SEEK_ZOOM_DO_NOT_USE
		this->SendMessage( CM_ZOOM_REDRAW, 1, 0);
#endif
	}

	return;
}

void CBBPlayerDlg::OnClickFPS( VOID )
{
	int nPlayState = m_multiPlayer.GetState();	
	if(nPlayState != PLAY_STATE_RUNNING  && nPlayState != PLAY_STATE_PAUSED)
		return;
	
	if( nPlayState == PLAY_STATE_RUNNING ){
		m_multiPlayer.StreamPause();
	}

	//m_multiPlayer.GoStep( (DWORD)-1 );
	m_multiPlayer.GoStep( (DWORD) 1 );

	this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_MEDIA_PAUSED );

#if !SEEK_ZOOM_DO_NOT_USE
	this->SendMessage( CM_ZOOM_REDRAW, 1, 0);
#endif
	return;
}


void CBBPlayerDlg::OnClickOpen( VOID )
{
	int nPlayState = m_multiPlayer.GetState();	
	if( nPlayState == PLAY_STATE_RUNNING || nPlayState == PLAY_STATE_PAUSED)
		this->PostMessage( WM_USER_MEDIAFORCESTOP );
	
	ITEMIDLIST *pidlBrowse; // 구조체 초기화
	WCHAR pszPathname[512];  // 경로 저장을 위한 변수
	BROWSEINFO BrInfo; 
    	static bool isDirSelected = false;
		
	if(isDirSelected == false)
	{
		CString msg;
		isDirSelected = true;
		memset(&BrInfo, 0, sizeof(BrInfo));
		BrInfo.pidlRoot = NULL;
		BrInfo.hwndOwner = this->m_hWnd;//GetSafeHwnd();

		this->m_pUIControl->m_Language.GetLenguageText(&msg, CLanguage::TIP_BTN_SELDIR);

		BrInfo.lpszTitle = msg.GetBuffer();
		
		//BrInfo.ulFlags = BIF_RETURNONLYFSDIRS; 
		BrInfo.ulFlags = 0; //BIF_RETURNONLYFSDIRS;   210323

		BrInfo.lpfn = BrowseForFolder_CallbackProc;
		BrInfo.lParam = (LPARAM)(LPCWSTR)this->m_cstrSelectDrive;

		this->EnableWindow( FALSE );
		
		pidlBrowse = ::SHBrowseForFolder(&BrInfo); 
		if(pidlBrowse != NULL)
		{ 
		    ::SHGetPathFromIDList(pidlBrowse, pszPathname);
		    UpdateData(FALSE);
//		    if(wcslen(pszPathname) != 3)
//		    {
//		        wcscat(pszPathname, TEXT("\\"));
//		    }
		    this->m_cstrSelectDrive.Format(pszPathname);    

		    this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_LIST_NORMAL );

		    if(this->m_nCurMode == 0)
		    	this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_INIT );
			
		    this->OpenDisk();


			this->SetUnitOfSpeed();
		}
		
		
	    	isDirSelected = false;

		this->EnableWindow( TRUE );
		this->SetActiveWindow();
#ifdef TIME_SEARCH_USE
		UpdateGraphDB(NULL, 1);
#endif
	}

	return;
}

void CBBPlayerDlg::OnClickSave( VOID )
{
	ClListCtrl* pFileList = LST_GET( TEXT("main.listbox.filelist") );
	int ItemNo = 0;
	if( pFileList )
	{
		BOOL isChecked = FALSE;
		int nCount = pFileList->GetItemCount();

		ItemNo = (int)pFileList->GetFirstSelectedItemPosition()-1;
		
		for(int i = 0; i< nCount;i++) {
			if(pFileList->GetCheck(i)) {
				//if(isChecked)
				{
					OnClickSaveAll();
					return;
				}
				isChecked = TRUE;
				ItemNo = i;
			}

			
		}
			
		this->m_nCurPlayIndex = (int)pFileList->GetFirstSelectedItemPosition()-1;
		
		if( this->m_nCurPlayIndex == -1 )
		{
			OPEN_NOTIFY_DIALOG( NOTIFY_ERR_NO_SELECTFILE ,&this->m_pUIControl->m_Language);
			return;
		}

		
		INT nPlayState = this->m_multiPlayer.GetState();
		BOOL bRunning = FALSE;

		m_nStreamCount = m_multiPlayer.GetStreamCount();
		
		if( nPlayState == PLAY_STATE_RUNNING )
		{
			bRunning = TRUE;
			m_multiPlayer.StreamPause();
			this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_MEDIA_PAUSED );
		}
		
		if( this->m_bIsRemovalNxfs){
			CString cstrSourceFile = pFileList->GetItemText( m_nCurPlayIndex, 3 );
			this->OnClickStop();
			bRunning = FALSE;
#if 0			
			if(this->m_bFileSaveThreadWorking == FALSE )
					mp_SaveThread = AfxBeginThread(this->FileSaveThreadWork,this);
#else
			CString strFolder;
			CString strSaveDirTitle;
			if(!this->m_pUIControl->m_Language.GetLenguageText(&strSaveDirTitle , CLanguage::TEXT_SAVEDIRECTORY))
				strSaveDirTitle = _T("Save Directory");
		
			if (this->m_fileManager.BrowseFolder(strFolder, strSaveDirTitle.GetBuffer(), this->GetSafeHwnd())){
				CCopyFileProgressDlg dlg;
				dlg.SetLanguage(&this->m_pUIControl->m_Language);
				dlg.mp_fileManager = &this->m_fileManager;
				dlg.mp_FileList = pFileList;
				dlg.m_nListupIndex = ItemNo;
				dlg.m_strDestPath = strFolder;
				dlg.m_nCopyFiles = 1;

				dlg.DoModal();

				//ch 분리
				if(m_nStreamCount >= 2){
					std::wstring wtsFileName = ClUtil::PATH::getFileNameSpecW( cstrSourceFile);
				    CString cstrNewFileName = wtsFileName.c_str();
				    cstrNewFileName += TEXT(".");
				    cstrNewFileName += TEXT("avi");

					UINT delay_timeout = 1000;
					m_cstrSaveFileExt		= CString(TEXT("avi"));//saveDlg.GetFileExt();
					m_cstrSaveDstFile		= strFolder + TEXT("\\") +  wtsFileName.c_str();
					m_cstrSaveCurrentFileName = strFolder + TEXT("\\") + cstrNewFileName;
						
					std::wstring wtsPath = ClUtil::PATH::getPathSpecW( m_cstrSaveDstFile.GetBuffer());
					m_cstrMp4SavePath.Format(TEXT("%s\\"), wtsPath.c_str());

					while(this->m_bFileSaveThreadWorking && delay_timeout--)
						::Sleep(10);
					
					if(this->m_bFileSaveThreadWorking == FALSE )
						mp_SaveThread = AfxBeginThread(this->FileSaveThreadWork,this);
				}
			}
#endif			
		}
		else {
			CString cstrListFileName = pFileList->GetItemText( m_nCurPlayIndex, 3 );
			CString cstrExtension;
			CString cstrFilter;
			CString cstrNewFileName;
			int nPos;

			std::wstring wtsFileName = ClUtil::PATH::getFileNameSpecW( cstrListFileName);
			cstrNewFileName.Format(TEXT("%s"),  wtsFileName.c_str());

			nPos = cstrListFileName.ReverseFind('.');
			if( nPos < 0 )	cstrExtension = TEXT("*");
			else			cstrExtension = cstrListFileName.Mid( nPos + 1, cstrListFileName.GetLength() - nPos - 1 );

			cstrFilter.Format( TEXT("video(*.%s)|*.%s||"), cstrExtension, cstrExtension );
			CFileDialog saveDlg( FALSE, NULL, cstrNewFileName, OFN_OVERWRITEPROMPT|OFN_ENABLESIZING, cstrFilter, NULL );

			saveDlg.m_ofn.lpstrInitialDir = m_cstrMp4SavePath.GetBuffer(); // 오픈할때 초기 경로 지정
			
			if( saveDlg.DoModal() == IDOK )
			{
				UINT delay_timeout = 1000;
				m_cstrSaveFileExt		= cstrExtension;//saveDlg.GetFileExt();
				m_cstrSaveDstFile		= saveDlg.GetPathName();
				m_cstrSaveCurrentFileName = cstrListFileName;
					
				std::wstring wtsPath = ClUtil::PATH::getPathSpecW( m_cstrSaveDstFile.GetBuffer());
				m_cstrMp4SavePath.Format(TEXT("%s\\"), wtsPath.c_str());

				while(this->m_bFileSaveThreadWorking && delay_timeout--)
					::Sleep(10);
				
				if(this->m_bFileSaveThreadWorking == FALSE )
					mp_SaveThread = AfxBeginThread(this->FileSaveThreadWork,this);
			}
		}

		if( bRunning )
		{
			this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_MEDIA_REPLAY );
			m_multiPlayer.StreamPlay();
		}
	}

	return;
}

void CBBPlayerDlg::OnClickSaveAll( VOID )
{
	ClListCtrl* pFileList = LST_GET( TEXT("main.listbox.filelist") );

	if(!this->m_bIsRemovalNxfs){
		ClButton* pBtnSaveall = BTN_GET( TEXT("main.button.saveall") );

		if( pBtnSaveall ) pBtnSaveall->EnableWindow( FALSE );
		//return;
	}

	this->OnClickStop();

	CString strFolder;
	CString strSaveDirTitle;
	if(!this->m_pUIControl->m_Language.GetLenguageText(&strSaveDirTitle , CLanguage::TEXT_SAVEDIRECTORY))
		strSaveDirTitle = _T("Save Directory");
	
	if (this->m_fileManager.BrowseFolder(strFolder, strSaveDirTitle.GetBuffer(), this->GetSafeHwnd()))
	{
		CCopyFileProgressDlg dlg;

		dlg.SetLanguage(&this->m_pUIControl->m_Language);

		dlg.mp_fileManager = &this->m_fileManager;
		
		dlg.mp_FileList = pFileList;
		dlg.m_nListupIndex = 0;
		dlg.m_nCopyFiles = pFileList->GetItemCount(); //this->m_fileManager.getListupFiles(); // all
		dlg.m_strDestPath = strFolder;
		dlg.m_bIsRemovalNxfs = this->m_bIsRemovalNxfs;

		dlg.DoModal();
	}
	
	return;
}

void CBBPlayerDlg::OnClickPrint()
{
	CWaitCursor wait;

	CDlgReport  dlg;
	REPORT_ITEM item;

	CString str;
	ClListCtrl* pFileList = LST_GET( TEXT("main.listbox.filelist") );
	CString strFileName;
//	time_t file_time;

//	if(pFileList) {
//			file_time = pFileList->GetItemData(m_nCurPlayIndex);
//	}
	
	int nPlayState = m_multiPlayer.GetState();
	if (nPlayState == PLAY_STATE_RUNNING || nPlayState == PLAY_STATE_PAUSED)
	{
		CString strSnapShotPath;
		CString strSnapShotPathRear;
		
		m_multiPlayer.StreamPause();

		CImage imageFront;
		CImage imageRear;
		CImage imageMap;

		CBitmap bitmapFront;
		CBitmap bitmapRear;
		CBitmap bitmapMap;

		bitmapFront.DeleteObject();
		if ( m_multiPlayer.GetCaptureToDib(0, bitmapFront) )
		{
			
			imageFront.Attach(bitmapFront);

			if (m_multiPlayer.GetStreamCount() > 1)
			{
				bitmapRear.DeleteObject();
				if ( m_multiPlayer.GetCaptureToDib(1, bitmapRear) )
				{
					imageRear.Attach(bitmapRear);
				}
			}
		}

		//TIME_ZONE_INFORMATION tz;
		//GetTimeZoneInformation(&tz);

		//CTime date(file_time + tz.Bias * 60);
		//CTime date(file_time);  // jun 170602
		//CTime date(pFileList->m_dwfile_time + dwCurrentTime/1000);    // jun 170602
		CTime date(pFileList->m_dwfile_time + (this->m_multiPlayer.GetCurrentPosition() / 1000));    // jun 170602
		
	    str.Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), date.GetYear(), date.GetMonth(), date.GetDay(), \
			date.GetHour(), date.GetMinute(), date.GetSecond());
	//	str.Format(_T("%04d/%02d/%02d %02d:%02d"), date.GetYear(), date.GetMonth(), date.GetDay(), \
	//		date.GetHour(), date.GetMinute());                                                               // jun 170117 print_second del


		item.streamCount = m_multiPlayer.GetStreamCount();
		_tcscpy(item.strDateTime, str);

		item.cpStreamImage[FrontCamera] = &imageFront;
		item.cpStreamImage[RearCamera] = &imageRear;

		double x = 0, y = 0, z = 0;
		
		if(m_pGSensorView)
		{
			GVALUE gv;
			if(m_pGSensorView->GetCurrentSensorData(&gv)){
				x = gv.GX;
				y = gv.GY;
				z = gv.GZ;
			}
		}

		ClWebBrowser* pMap = MAP_GET( TEXT("main.web.map") );

		if(pMap){
			if(pMap->IsInternetConnected() && pMap->IsGPSData()){
			   CClientDC dc(pMap);
			   CDC memory_dc;
			   CRect r;

			   memory_dc.CreateCompatibleDC(&dc); 
			   pMap->GetWindowRect(r);

				bitmapMap.DeleteObject();
			   bitmapMap.CreateCompatibleBitmap(&dc, r.Width(),r.Height());

			   CBitmap* p_bitmap = memory_dc.SelectObject(&bitmapMap);
			   memory_dc.BitBlt(0, 0, r.Width(),r.Height(), &dc, 0, 0, SRCCOPY); 
				memory_dc.SelectObject(p_bitmap);

				imageMap.Attach(bitmapMap);
					
				item.cpMapImage = &imageMap;
				
				item.fEGps	  = pMap->m_fLat;
				item.fNGps    = pMap->m_fLng;
				item.fSpeed = pMap->m_fSpeed;
				item.nSpeedMode = 0;
			}
			else {
				item.cpMapImage = NULL;
								
				item.fEGps	  = 0.0;
				item.fNGps    = 0.0;
				item.fSpeed = 0.0;
				item.nSpeedMode = 0;
			}
		}

		CDigitNum* pDigit		= DIGIT_GET(TEXT("main.digit.speed"));
		if (pDigit){
			item.pDigit = pDigit->GetValue();
			item.nSpeedMode = pDigit->GetUnit();
		}
		
		item.fXSensor = x;
		item.fYSensor = y;
		item.fZSensor = z;

		dlg.SetReportItem(&item);

		dlg.SetLanguage(&this->m_pUIControl->m_Language);

		RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

		dlg.DoModal();
	}

	if( nPlayState == PLAY_STATE_RUNNING )
		m_multiPlayer.StreamPlay();
}

void CBBPlayerDlg::OnNMDblclkFilelist( NMHDR *pNMHDR, LRESULT *pResult )
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	//int nIndex = (int)pFileList->GetFirstSelectedItemPosition()-1;
	PlayerFileListIndex(pNMItemActivate->iItem);

	*pResult = 0;

	return;
}


void CBBPlayerDlg::OnClickFRChange( VOID )
{	
	this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_FRCHANGE );

	return;
}


void CBBPlayerDlg::OnClickFMChange()
{
	this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_FMCHANGE );

	return;
}

void CBBPlayerDlg::OnBnClickZoom()
{
	int nPlayState = m_multiPlayer.GetState();
	ClButton* 		pBtnZoom 	= BTN_GET( TEXT("main.button.zoom") );

	if(pBtnZoom == NULL)
		return;
	
	if(this->m_pUIControl->m_ActiveFront == CBBPlayerUIControl::ITEM_MAP)
	{
		pBtnZoom->SetCheckState(FALSE);
		return;
	}
	
	if (nPlayState == PLAY_STATE_RUNNING || nPlayState == PLAY_STATE_PAUSED)
	{
		if(nPlayState == PLAY_STATE_RUNNING)
		{
			m_multiPlayer.StreamPause();
			this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_MEDIA_PAUSED );
		}

		this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_ZOOM );			
	}
	else
		return;

	if(pBtnZoom->GetCheckState() == FALSE){
		//this->m_multiPlayer.SetPosition(this->m_multiPlayer.GetCurrentPosition()); //20200729 zoom On/Off 프레임 이동 문제가 있어 삭제
	}
	else
		this->OnDrawZoomImage();
	
	return;
}

LRESULT CBBPlayerDlg::OnDrawMuxImage(WPARAM wParam, LPARAM lParam)
{
	ClVideoView* 		pVideoCtrl = VID_GET( TEXT("main.video.quadctrl") );
	DWORD 			capture_ch = 0;

	int nPlayState = m_multiPlayer.GetState();

	if (nPlayState != PLAY_STATE_RUNNING && nPlayState != PLAY_STATE_PAUSED)
		return 0;
			
	if( pVideoCtrl == NULL)
		return 0;
	
	if(pVideoCtrl->GetQuadViewMode() == FALSE)
		return 0;

	if(m_MuxViewCurrentTime != this->m_multiPlayer.GetCurrentPosition() || m_iMuxViewChannel != pVideoCtrl->GetQuadCtrlChannel()) {
		m_MuxViewCurrentTime = this->m_multiPlayer.GetCurrentPosition();
		m_iMuxViewChannel = pVideoCtrl->GetQuadCtrlChannel();
		
		if(m_multiPlayer.GetStreamCount() > 1)
			capture_ch = 1;

		m_BitmapMux.DeleteObject();

		//DEBUGMSG(1, ("%d:%02d:%03d\r\n", (int)m_MuxViewCurrentTime / 1000/ 60, (int)m_MuxViewCurrentTime / 1000 % 60, (int)m_MuxViewCurrentTime % 1000));
		
		if (this->m_multiPlayer.GetCaptureToDib(capture_ch, m_BitmapMux) )
		{	
			ClVideoView* 		pVideo = VID_GET( TEXT("main.video.quadview") );
			
			if(pVideo) {
				pVideo->SetQuadView(m_BitmapMux, m_iMuxViewChannel);
				pVideoCtrl->SetQuadView(m_BitmapMux, 4);
			}
		}
	}

	if(wParam)
	{
		this->SetTimer( MAINDLG_TID_MUXREDRAW, 33, NULL );
	}
	
	return 0;
}



#define SORTING_BUTTON_USE  1

void CBBPlayerDlg::OnClickTabNormal( VOID )
{
	BOOL bDoNothing = FALSE;
	if( this->m_pUIControl->GetListTabPrevEvent() == CBBPlayerUIControl::EVENT_LIST_NORMAL )
		bDoNothing = TRUE;

	this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_LIST_NORMAL );
	if( bDoNothing )
	{
#if 0//SORTING_BUTTON_USE
		FileListSorting(ClListCtrl::SORT_UPDOWN);
#endif
		return;
	}

	if(this->m_bIsRemovalNxfs)
		this->m_fileManager.listupFiles( LST_GET( TEXT("main.listbox.filelist") ));
	else
		this->OpenDisk();

	return;
}


void CBBPlayerDlg::OnClickTabEvent( VOID )
{
	BOOL bDoNothing = FALSE;
	if( this->m_pUIControl->GetListTabPrevEvent() == CBBPlayerUIControl::EVENT_LIST_EVENT )
		bDoNothing = TRUE;

	this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_LIST_EVENT );
	if( bDoNothing )
	{
#if 0//SORTING_BUTTON_USE
		FileListSorting(ClListCtrl::SORT_UPDOWN);
#endif
		return;
	}

	if(this->m_bIsRemovalNxfs)
		this->m_fileManager.listupFiles( CBBPlayerUIControl::DIR_TYPE_EVENT, LST_GET( TEXT("main.listbox.filelist") ));
	else
		this->OpenDisk();

	return;
}


void CBBPlayerDlg::OnClickTabManual( VOID )
{

	BOOL bDoNothing = FALSE;
	if( this->m_pUIControl->GetListTabPrevEvent() == CBBPlayerUIControl::EVENT_LIST_MANUAL )
		bDoNothing = TRUE;

	this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_LIST_MANUAL );

	
	if( bDoNothing )
	{
#if 0//SORTING_BUTTON_USE
		FileListSorting(ClListCtrl::SORT_UPDOWN);
#endif
		return;
	}

	if( this->m_bIsRemovalNxfs)
		this->m_fileManager.listupFiles( CBBPlayerUIControl::DIR_TYPE_MANUAL, LST_GET( TEXT("main.listbox.filelist") ));
	else
		this->OpenDisk();


	return;
}


void CBBPlayerDlg::OnClickTabMotion( VOID )
{
#if SORTING_BUTTON_USE
	FileListSorting();
#else
	BOOL bDoNothing = FALSE;
	if( this->m_pUIControl->GetListTabPrevEvent() == CBBPlayerUIControl::EVENT_LIST_MOTION )
		bDoNothing = TRUE;

	this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_LIST_MOTION );
#if 0//SORTING_BUTTON_USE
		FileListSorting(ClListCtrl::SORT_UPDOWN);
#endif
	if( this->m_bIsRemovalNxfs)
		this->m_fileManager.listupFiles( CBBPlayerUIControl::DIR_TYPE_MOTION, LST_GET( TEXT("main.listbox.filelist") ));
	else
		this->OpenDisk();	
#endif
	return;
}

void CBBPlayerDlg::OnBnClickedMainBtnCamFf()
{
	ClButton* 		pBtn = BTN_GET( TEXT("main.button.frontflip") );
	if(pBtn)	
		m_bFrontFlip = pBtn->GetCheckState();
	
	m_multiPlayer.SetFlip(MP_VIDEO_CH_1, m_bFrontFlip);
}

void CBBPlayerDlg::OnBnClickedMainBtnCamFm()
{
	ClButton* 		pBtn = BTN_GET( TEXT("main.button.frontmirror") );
	if(pBtn)	
		m_bFrontMirror = pBtn->GetCheckState();
	
	m_multiPlayer.SetMirror(MP_VIDEO_CH_1, m_bFrontMirror);
}

void CBBPlayerDlg::OnBnClickedMainBtnCamRf()
{
	ClButton* 		pBtn = BTN_GET( TEXT("main.button.rearflip") );
	if(pBtn)
		m_bRearFlip = pBtn->GetCheckState();
	
	m_multiPlayer.SetFlip(MP_VIDEO_CH_2, m_bRearFlip);
}

void CBBPlayerDlg::OnBnClickedMainBtnCamRm()
{
	ClButton* 		pBtn = BTN_GET( TEXT("main.button.rearmirror") );
	if(pBtn)
	 	m_bRearMirror = pBtn->GetCheckState();
	
	m_multiPlayer.SetMirror(MP_VIDEO_CH_2, m_bRearMirror);
}

void CBBPlayerDlg::OnBnClickedMainBtnVolume()
{
	ClButton* 		pBtn = BTN_GET( TEXT("main.button.volume") );
	ClSlider* pSldVloume= SLD_GET( TEXT("main.slider.volume") );

	if(pBtn && pSldVloume)
	{
	 	if(pBtn->GetCheckState()) // mute
 			this->SetVolume(0);
		else
			this->SetVolume(pSldVloume->GetPos() * 5);
	}
}

void CBBPlayerDlg::OnBnClickedMainBtnSpeed()
{
#if 1
	m_eUnitOfSpeed = (ENUM_UNIT_OF_SPEED)(((int)m_eUnitOfSpeed+1) % UNIT_ENUM_END);
	SetUnitOfSpeed(m_eUnitOfSpeed, TRUE);
#else //20170226 PulseSpeedUse
	static DWORD count;
	if(count++ % 2){ // ?? 두번 호출 되어 임시로 넣었습니다.
		if( this->m_textData.m_bPulseSpeedUse ) {
			this->m_textData.m_bPulseSpeedUse = FALSE;
		}
		else if(this->m_textData.m_PulseList.size() > 0 ){
			this->m_textData.m_bPulseSpeedUse = TRUE;
		}
		
		SetUnitOfSpeed(m_eUnitOfSpeed, FALSE);
	}
	
#endif	
	
}

void CBBPlayerDlg::OnNMClickMainFilelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here

	ClListCtrl* pFileList = LST_GET( TEXT("main.listbox.filelist") );

	//DEBUGMSG(1, ("IsSHIFTpressed %d\r\n", IsSHIFTpressed()));  //jun 191023 멀티체크
	
#ifdef TIME_SEARCH_USE
	if(pFileList &&  pNMItemActivate->iItem < pFileList->GetItemCount() && pNMItemActivate->iItem >= 0)
	{
		CTime time(pFileList->GetItemData(pNMItemActivate->iItem));
		UpdateGraphDB(&time);
	}
#endif	
	if(pFileList && pNMItemActivate->iSubItem == 0 && pNMItemActivate->iItem >= 0){
		int seq = pNMItemActivate->iItem;

//jun 191023 멀티체크
		if(IsSHIFTpressed() && pFileList->m_nStartItemNo != -1) {
			if(pFileList->m_nStartItemNo > seq){
				for(int i = seq; i <= pFileList->m_nStartItemNo; i++)
					pFileList->SetCheck(i, TRUE); 
			}
			else {
				for(int i = pFileList->m_nStartItemNo; i <= seq; i++)
					pFileList->SetCheck(i, TRUE); 
			}
		}
		else {
			pFileList->m_nStartItemNo = seq;
			if(pFileList->GetCheck(seq)){
				ClButton* 		pBtnAll = BTN_GET( TEXT("main.button.checkfilelistall") );
				
				pFileList->SetCheck(seq, FALSE); 
				if( pBtnAll ) pBtnAll->SetCheckState( FALSE );
			}
			else {
				pFileList->SetCheck(seq, TRUE); 
			}
		}

		ClListCtrl* pFileList = LST_GET( TEXT("main.listbox.filelist") );
		ClButton* pBtnSave = BTN_GET( TEXT("main.button.save") );
		
		BOOL isChecked = FALSE;
		if(pFileList && pBtnSave){
			int nCount = pFileList->GetItemCount();
			for(int i = 0; i< nCount;i++) {
					if(pFileList->GetCheck(i)) {
						isChecked = TRUE;
						break;
					}
			}
		}
		if( /*this->m_pUIControl->GetMediaStatus() == CBBPlayerUIControl::EVENT_MEDIA_PLAY || */ isChecked ){
			pBtnSave->EnableWindow(TRUE);
		}
		else
			pBtnSave->EnableWindow(FALSE);
	}

	*pResult = 0;
}

LRESULT CBBPlayerDlg::OnGsensorSeekFrame(WPARAM wParam, LPARAM lParam)
{
	INT nPos = (INT)wParam;

#ifdef RDF_USE
	ClButton* 		pBtnStop = BTN_GET( TEXT("main.button.stop") );
	if(nPos >= 0 && m_pGSensorView && pBtnStop)
	{
		if(!pBtnStop->IsWindowEnabled())
		{
			TIME_ZONE_INFORMATION _TimeZoneInformation;
 			GetTimeZoneInformation( &_TimeZoneInformation);

			time_t seek_time = __m_ctmPrevSelected.GetTime() - ((__m_ctmPrevSelected.GetTime() - (_TimeZoneInformation.Bias * 60)) % (24 * 60 * 60)) + (nPos/1000);
			CTime time = CTime( seek_time );
			//UpdateGraphTime(&time);
			UpdateGraphDB(&time);
			m_pUIControl->UserDataSynchronize(nPos);
			return 0;
		}
	}
#endif

	if(nPos >= 0)
	{
		m_multiPlayer.SetPosition( (long)nPos );
		this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_MEDIA_PROGRESS );
	}

	if(m_multiPlayer.GetState() == PLAY_STATE_PAUSED && !lParam)
	{
		ClButton* pBtnPlay = BTN_GET( TEXT("main.button.play") );

		if(pBtnPlay->GetCheckState() == TRUE)
			m_multiPlayer.StreamPlay();
#if !SEEK_ZOOM_DO_NOT_USE
		else
			this->SendMessage( CM_ZOOM_REDRAW, 1, 0);
#endif
		
		//SetTimer( MAINDLG_TID_PROGRESS, MAINDLG_TMDUR_PROGRESS, NULL );
		//this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_MEDIA_REPLAY );
	}
	else if(m_multiPlayer.GetState() == PLAY_STATE_RUNNING && lParam)
	{
		m_multiPlayer.StreamPause();
		//this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_MEDIA_PAUSED );
	}
	return 0;
}

LRESULT CBBPlayerDlg::OnDrawZoomImage(WPARAM wParam, LPARAM lParam)
{
	ClButton* 		pBtnZoom 	= BTN_GET( TEXT("main.button.zoom") );
	DWORD 			capture_ch;

	if(pBtnZoom == NULL)
		return 0;
	
	if(pBtnZoom->GetCheckState() == FALSE)
		return 0;

	if(this->m_pUIControl->m_ActiveFront == CBBPlayerUIControl::ITEM_FRONT_VIDEO)
		capture_ch = 0;
	else if(this->m_pUIControl->m_ActiveFront == CBBPlayerUIControl::ITEM_REAR_VIDEO)
		capture_ch = 1;
	else
		return 0;

	m_BitmapZoom.DeleteObject();

	if ( this->m_multiPlayer.GetCaptureToDib(capture_ch, m_BitmapZoom) )
	{	
		ClVideoView* 		pVideo = VID_GET( TEXT("main.video.zoom") );
			
		if(pVideo) 		pVideo->SetZoomImage(m_BitmapZoom);
	}

	if(wParam)
	{
		this->SetTimer( MAINDLG_TID_ZOOMREDRAW, 200, NULL );
	}
	
	return 0;
}

LRESULT CBBPlayerDlg::OnRedraw(WPARAM wParam, LPARAM lParam)
{
	this->SetTimer( MAINDLG_TID_MAINREDRAW, 250, NULL );
	//this->InvalidateRect( NULL );
	return 0;
}


#ifdef TIME_SEARCH_USE

void CBBPlayerDlg::OnCloseupDatetimepickerDate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CTime	time1, time2, timeprev = __m_ctmPrevSelected;
	
	//OnButtonPause();	// tony, Play mode GetTime error patch
	//Sleep(50);

	m_ctrlDate.GetCurSel(time1);
	m_ctrlTime.GetTime(time2);

	if( time1 != time2 )
	{
		//CTWSYSMANSearch *	psysman_search = (CTWSYSMANSearch*)GetTWStatus()->GetSysMan(CTWStatus::SYSMAN_SEARCH);

		UpdateGraphDB(&time1, TRUE, TRUE);
		m_ctrlDate.SetCurSel(time1);
		m_ctrlDate.SetToday((const CTime*)&time1);
		m_ctrlTime.SetTime((const CTime*)&time1);

		//update list
		UpdateFileListFromTime(&time1);
		
		//psysman_search->SetSearchTimeDiff(time1.GetTime() - timeprev.GetTime(), TRUE);

		//::AfxPostThreadMessage(TW_TM_SEARCH, TW_WPARM_TIMEUPDATE, TW_LPARM_GRAPHUPDATE);
	}
		
	*pResult = 0;
}

void CBBPlayerDlg::OnDatetimechangeDatetimepickerTime(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CTime	time1, time2, timeprev = __m_ctmPrevSelected;

//	OnButtonPause();	// tony, Play mode GetTime error patch
//	Sleep(50);

	m_ctrlTime.GetTime(time1);
	m_ctrlDate.GetCurSel(time2);

	if( time1 != time2 )
	{
		//CTWSYSMANSearch *	psysman_search = (CTWSYSMANSearch*)GetTWStatus()->GetSysMan(CTWStatus::SYSMAN_SEARCH);

		m_ctrlDate.SetCurSel(time1);
		m_ctrlDate.SetToday((const CTime*)&time1);
		UpdateGraphDB(&time1);

		//update list
		UpdateFileListFromTime(&time1);
		
		//psysman_search->SetSearchTimeDiff(time1.GetTime() - timeprev.GetTime());
		
		//::AfxPostThreadMessage(TW_TM_SEARCH, TW_WPARM_TIMEUPDATE, TW_LPARM_GRAPHUPDATE);
	}
	
	*pResult = 0;
}

long CBBPlayerDlg::OnICDBGraph(WPARAM wParam, LPARAM lParam)
{
	WORD				msg = LOWORD(wParam);
	WORD				gid = HIWORD(wParam);

	if( msg == ICM_DBGRAPH_W_LCLICK )
	{
		CTime				time;
		CTime				timeprev = __m_ctmPrevSelected;
		SYSTEMTIME			systm;
		WORD				pos_prev = LOWORD(lParam);
		WORD				pos_curr = HIWORD(lParam);
		BYTE				btPrevHourCount = 24;
		BYTE				btDSTHour = 24;
		//m_ctrlTime.GetTime(time);
		//time.GetAsSystemTime(systm);
		timeprev.GetAsSystemTime(systm);

		if( gid == _GID_HOUR )
		{
			if(pos_curr < 24)
			{
				systm.wHour	= pos_curr;
				systm.wMinute = 0;
				systm.wSecond = 0;
			}

			if( btPrevHourCount == 23 )
			{
				/* dst first day */
				if( pos_curr >= btDSTHour )
					systm.wHour	= pos_curr + 1;
			}
			else if( btPrevHourCount == 25 )
			{
				/* dst last day */
				if( pos_curr >= btDSTHour )
					systm.wHour	= pos_curr - 1;				
			}
			else
			{
				/* dst other day */
				//if( __m_btDSTHour == 25 )
				//	systm.wHour	= pos_curr + 1;
			}
		}
		else if( gid == _GID_MIN ) 

		{
			systm.wMinute	= pos_curr;
			systm.wSecond = 0;
		}
		else						
		{
			systm.wSecond	= pos_curr;
		}
		
		time = CTime(systm);
		UpdateGraphTime(&time);

		m_ctrlTime.SetTime((const CTime*)&time);
		m_ctrlDate.SetCurSel(time);
		m_ctrlDate.SetToday((const CTime*)&time);

		/* update list */
		UpdateFileListFromTime(&time);
		
		//::AfxPostThreadMessage(TW_TM_SEARCH, TW_WPARM_TIMEUPDATE, TW_LPARM_NO_GRAPHUPDATE);
	}
	else if( msg  == ICM_DBGRAPH_W_RCLICK )
	{
				
	}
	else if( msg  == ICM_DBGRAPH_W_LDBLCLICK )
	{
		PlayerFileListIndex(UpdateFileListFromTime());
	}

	return 0;
}

BOOL CBBPlayerDlg::UpdateGraphTime(CTime * pTime/*=NULL*/, BOOL isForce/*=FALSE*/)
{
	if( __m_isTimeBarEnabled || isForce )
	{
		CTime	_time = CTime::GetCurrentTime();
		CTime * _pTime = pTime;

		if( !_pTime )
		{
			//m_ctrlTime.GetTime(_time);
			_pTime = &_time;
		}
		else
		{
			//m_ctrlTime.GetTime(_time);

			m_ctrlDate.SetCurSel((*pTime));
			m_ctrlDate.SetToday((const CTime*)pTime);
			m_ctrlTime.SetTime((const CTime*)pTime);
		}
	
		// update graph
		if((__m_ctmPrevSelected.GetYear() != _pTime->GetYear()) ||\
			(__m_ctmPrevSelected.GetMonth() != _pTime->GetMonth()))
		{
			isForce = TRUE;
		}
		if(isForce || (__m_ctmPrevSelected.GetDay() != _pTime->GetDay()))
		{
			isForce = TRUE;
 			m_ctrlICDBGraph[_GID_HOUR].SetGraphItem(0, 24, _m_item_h);
  #ifdef RDF_USE
  			m_ctrlICDBGraph[_GID_HOUR].SetGraphItem(1, 24, _m_rdf_h);
  #endif
			m_ctrlICDBGraph[_GID_HOUR].SetSeletedSel(_pTime->GetHour());
			m_ctrlICDBGraph[_GID_HOUR].Invalidate();
		}
		if(isForce || (__m_ctmPrevSelected.GetHour() != _pTime->GetHour()))
		{
			isForce = TRUE;
			m_ctrlICDBGraph[_GID_MIN].SetGraphItem(0, 60, _m_item_m[_pTime->GetHour()]);
  #ifdef RDF_USE
  			m_ctrlICDBGraph[_GID_MIN].SetGraphItem(1, 60, _m_rdf_m[_pTime->GetHour()]);
  #endif			
			m_ctrlICDBGraph[_GID_MIN].SetSeletedSel(_pTime->GetMinute());
			m_ctrlICDBGraph[_GID_MIN].Invalidate();
		}
		if(isForce || (__m_ctmPrevSelected.GetMinute() != _pTime->GetMinute()))
		{
			m_ctrlICDBGraph[_GID_SEC].SetGraphItem(0, 60, _m_item_s[_pTime->GetHour()][_pTime->GetMinute()]);
  #ifdef RDF_USE
  			m_ctrlICDBGraph[_GID_SEC].SetGraphItem(1, 60, _m_rdf_s[_pTime->GetHour()][_pTime->GetMinute()]);
  #endif
			m_ctrlICDBGraph[_GID_SEC].SetSeletedSel(_pTime->GetSecond());
			m_ctrlICDBGraph[_GID_SEC].Invalidate();
		}
  #ifdef RDF_USE
		m_pUIControl->UserDataSynchronize(((_pTime->GetHour() * 3600) + (_pTime->GetMinute() * 60) + _pTime->GetSecond()) * 1000);
  #endif
		__m_ctmPrevSelected = *_pTime;

		//UpdateIndexFileList();

		return TRUE;
	}

	return FALSE;
}
BOOL CBBPlayerDlg::UpdateGraphDB(CTime * pTime/*=NULL*/, BOOL isForce/*=FALSE*/, BOOL isFileTimeSet /*=FALSE*/)
{
	CTime * _pTime = pTime;

	if( !_pTime )
	{
		//m_ctrlTime.GetTime(_time);
		_pTime = &__m_ctmPrevSelected;
	}
	
	//Graph update
	if(isForce || (__m_ctmPrevUpdateDB.GetYear() != _pTime->GetYear()) ||\
		(__m_ctmPrevUpdateDB.GetMonth() != _pTime->GetMonth()) || \
		(__m_ctmPrevUpdateDB.GetDay() != _pTime->GetDay()) )
	{
		ClListCtrl* pFileList = LST_GET( TEXT("main.listbox.filelist") );
		SYSTEMTIME timeFrom;
		SYSTEMTIME timeUntil;
		
		m_ctrlDate.SetCurSel((*_pTime));
		m_ctrlDate.SetToday((const CTime*)_pTime);

		int nCount = m_ctrlDate.GetMonthRange(&timeFrom, &timeUntil, GMR_DAYSTATE);
		// Allocate the state array based on the return value.

		LPMONTHDAYSTATE pDayState;
		pDayState = new MONTHDAYSTATE[nCount];
		memset(pDayState, 0, sizeof(MONTHDAYSTATE) * nCount);

		// Find the first fully visible month. 
		int nIndex = (timeFrom.wDay == 1) ? 0 : 1;

		if(pFileList)
		{
			DWORD  fc;
			DWORD dwFileCount = pFileList->GetItemCount();

			::ZeroMemory(_m_item_h, 24 * sizeof(CICDBGraph::ITEM));
			::ZeroMemory(_m_item_m, 24 * 60 * sizeof(CICDBGraph::ITEM));
			::ZeroMemory(_m_item_s, 24 * 60 * 60 * sizeof(CICDBGraph::ITEM));
			
			for( fc = 0; fc < dwFileCount; fc++)
			{
				time_t fileTime = pFileList->GetItemData(fc);
				CTime ctimeFile(fileTime);
				int event_type = CBBoxUtil::GetEventType(pFileList->GetItemText(fc, 1)); // file name
				
				if(_pTime->GetYear() == ctimeFile.GetYear() && _pTime->GetMonth() == ctimeFile.GetMonth())
				{
					pDayState[nIndex] |= 1 << (ctimeFile.GetDay()-1);   //day
				
					if(_pTime->GetDay() == ctimeFile.GetDay())
					{
						int hour = ctimeFile.GetHour();

						_m_item_h[hour].set = 1;
						if(_m_item_h[hour].color <  event_type)
							_m_item_h[hour].color = event_type;
													
						//if(_pTime->GetHour() == hour)
						{
							int min = ctimeFile.GetMinute();
							_m_item_m[hour][min].set = 1;
							if(_m_item_m[hour][min].color <  event_type)
								_m_item_m[hour][min].color = event_type;
							
							//if(_pTime->GetMinute() == min)
							{
								int sec = ctimeFile.GetSecond();
								_m_item_s[hour][min][sec].set = 1;
								if(_m_item_s[hour][min][sec].color < event_type)
									_m_item_s[hour][min][sec].color = event_type;

								if(isFileTimeSet)
								{
									SYSTEMTIME			systm;
									
									_pTime->GetAsSystemTime(systm);

									systm.wHour	= hour;
									systm.wMinute = min;
									systm.wSecond = sec;

									isFileTimeSet = FALSE;

									*_pTime = CTime(systm);
								}
							}
						}
					}
				}
			}
		}

		// Set state and clean up
		m_ctrlDate.SetDayState(nCount, pDayState);
		delete [] pDayState;

#ifdef RDF_USE // RDF Data USE
		UpdateRdfData(_pTime);
#endif
	}

 	UpdateGraphTime(_pTime, 1);
	__m_ctmPrevUpdateDB = *_pTime;
	
	return TRUE;
}

BOOL CBBPlayerDlg::UpdateRdfData(CTime *pTime /*=NULL*/)
{
#ifdef RDF_USE // RDF Data USE
		CTime * _pTime = pTime;
		struct tm time;
		int rdf_size = 0;

		ClButton* 		pBtnStop = BTN_GET( TEXT("main.button.stop") );
		if(pBtnStop)
		{
			if(pBtnStop->IsWindowEnabled()) // play mode
				return FALSE;
		}
		
		if( !_pTime )
		{
			//m_ctrlTime.GetTime(_time);
			_pTime = &__m_ctmPrevSelected;
		}

		if(m_pGSensorView)
			m_pGSensorView->ErasePoint();
		
		m_textData.m_GPSList.clear();
		
		::ZeroMemory(_m_rdf_h, 24 * sizeof(CICDBGraph::ITEM));
		::ZeroMemory(_m_rdf_m, 24 * 60 * sizeof(CICDBGraph::ITEM));
		::ZeroMemory(_m_rdf_s, 24 * 60 * 60 * sizeof(CICDBGraph::ITEM));
		
		_pTime->GetLocalTm(&time);
		
		rdf_size = m_cRdf.Load(m_cstrSelectDrive, &time);
		if(rdf_size)
		{
			int pos = 0;
			//m_pGSensorView->AppendPoint(0.0, 0.0, 0.0, 0.0, 0, 0, 0);
						
			for( pos = 0; pos < rdf_size; pos ++)
			{
				time_t localTime;
				
				LPST_RDF_DATA pRdf = m_cRdf.GetRdfData(pos);
				CTime ctime(pRdf->bbox.utd_time); 
				int hour = ctime.GetHour();
				int min = ctime.GetMinute();
				int sec = ctime.GetSecond();

				_m_rdf_h[hour].set = 1;
				if(_m_rdf_h[hour].color <  pRdf->gps.event)
					_m_rdf_h[hour].color = pRdf->gps.event;

				_m_rdf_m[hour][min].set = 1;
				if(_m_rdf_m[hour][min].color <  pRdf->gps.event)
					_m_rdf_m[hour][min].color = pRdf->gps.event;

				_m_rdf_s[hour][min][sec].set = 1;
				if(_m_rdf_s[hour][min][sec].color < pRdf->gps.event)
					_m_rdf_s[hour][min][sec].color = pRdf->gps.event;


				DWORD time = (m_cRdf.GetLocalTime(pRdf->bbox.utd_time) % (24 * 60 * 60)) * 1000;
				// g-sensor data update
				if(m_pGSensorView)
				{
					int i;
					for(i = 0; i < 10; i++)
						m_pGSensorView->AppendPoint((double)pRdf->alG[i].x/100, (double)pRdf->alG[i].y/100, (double)pRdf->alG[i].z/100, pRdf->alPulse[i/5].speed, pRdf->gps.event, time + (i*100));
				}

				// gps data update
				CGPSData data;
				data.m_bNotDegree = TRUE;
				data.m_cts = time;

				if(pRdf->gps.latitude || pRdf->gps.longitude )
					data.m_bValid = TRUE;
					
				data.m_sp_state = pRdf->gps.sp_state;
				data.m_fLat = (double)pRdf->gps.latitude / 10000.0;
				data.m_fLng = (double)pRdf->gps.longitude / 10000.0;
				data.m_fSpeed = (double)pRdf->gps.speed / 1.853184; //Km/h to Knots
				data.m_nCog = (int)pRdf->gps.cog;
				data.m_fPdop = (double)pRdf->gps.pdop /10;
					
				m_textData.m_GPSList.push_back(data);
		
			}

			this->m_pUIControl->SetStreamInfo( 2, TRUE,	TRUE);
			this->m_pUIControl->SetActiveItem();

			m_pUIControl->UserDataSynchronize(0);
			m_pUIControl->UserDataSynchronize(((_pTime->GetHour() * 3600) + (_pTime->GetMinute() * 60) + _pTime->GetSecond()) * 1000);
  
		}
#endif
	return TRUE;
}

INT CBBPlayerDlg::UpdateFileListFromTime(CTime *pTime /*= NULL*/)
{
	ClListCtrl* pFileList = LST_GET( TEXT("main.listbox.filelist") );
	if(pFileList)
	{
		DWORD  count;
		DWORD dwFileCount = pFileList->GetItemCount();
		time_t currentTime = 0;

		if(pTime)
			currentTime = pTime->GetTime(); 
		else
		 	currentTime = __m_ctmPrevSelected.GetTime(); 


		for( count = 0; count < dwFileCount; count++)
		{
			time_t fileTime = pFileList->GetItemData(count);
			if(currentTime <= fileTime)
			{
				break;
			}
		}
		UpdateIndexFileList(count);
		pFileList->EnsureVisible( count, FALSE );
		
		return count;
	}

	return 0;
}

VOID CBBPlayerDlg::ShowWindowTimeSearch(int nCmdShow)
{
	int i;

	if(m_pGSensorView == NULL)
		return;
	
	if(nCmdShow == SW_HIDE && this->m_nCurMode == 0)
	{
#ifdef RDF_USE 	
//		m_pGSensorView->SetWindowRect(&m_rGSensorGraph);
//		::SetWindowPos(m_pGSensorView->m_hWnd, NULL, m_rGSensorGraph.left, m_rGSensorGraph.top, m_rGSensorGraph.Width(), m_rGSensorGraph.Height(), SWP_NOZORDER);
#else
		m_pGSensorView->ShowWindow(SW_SHOW);
		m_pGSensorView->EnableWindow( TRUE );
#endif		
	}
	else
	{
#ifdef RDF_USE 
//		m_pGSensorView->SetWindowRect(&m_rVideoFront);
//		::SetWindowPos(m_pGSensorView->m_hWnd, NULL, m_rVideoFront.left, m_rVideoFront.top, m_rVideoFront.Width(), m_rVideoFront.Height(), SWP_NOZORDER);
#else
		m_pGSensorView->ShowWindow(SW_HIDE);
#endif
	}

	if(this->m_nCurMode == 1)
		nCmdShow = SW_HIDE;
	
	m_ctrlDate.ShowWindow(nCmdShow);
	m_ctrlTime.ShowWindow(nCmdShow);

	for( i = 0; i < 3; i++)
		m_ctrlICDBGraph[i].ShowWindow(nCmdShow);

	if(nCmdShow == SW_SHOW)
		UpdateRdfData();

}
#endif

void CBBPlayerDlg::FileListSorting(int nSortupDown /* = 0 */)
{
	ClListCtrl* pFileList = LST_GET( TEXT("main.listbox.filelist") );
	if(pFileList){
		ClButton* 		pBtnMotion = BTN_GET( this->m_pUIControl->m_Language.GetButtonText(TEXT("main.button.tabmotion")) );

		BOOL isChecked = 0;
		
		if(pBtnMotion)	
			isChecked = pBtnMotion->GetCheckState();

		if(isChecked)
			pFileList->Sort(ClListCtrl::SORT_FILENAME2, nSortupDown);
		else
			pFileList->Sort(ClListCtrl::SORT_STR_DATE, nSortupDown);
		
		pFileList->SetRedraw( TRUE );
	}
}

#if 0//TEXT_DATA_THREAD_USE
UINT CBBPlayerDlg::ThreadTextDataProc(void* pArg)
{
	CBBPlayerDlg* pDlg = (CBBPlayerDlg*)pArg;

	if (pDlg == NULL)
	{
		return 0;
	}

	pDlg->m_bTextDataThreadWorking = TRUE;
	pDlg->m_bTextDataThreadExit = FALSE;
	
	pDlg->m_textData.m_GSensorList.clear();
	pDlg->m_textData.m_GPSList.clear();

	if(pDlg->m_pGSensorView)
	{
		pDlg->m_pGSensorView->ErasePoint();
		pDlg->m_pGSensorView->m_bRedrawBlocked = TRUE;
	}
	
	HRESULT hr = 0;
	int textcount = 1;
	unsigned char text[1024];
	int size = 1024;
	unsigned long cts;

	LONGLONG durationTime = pDlg->m_multiPlayerForTextRead.GetDuration();
	if(durationTime)
	{
		while(!pDlg->m_bTextDataThreadExit)
		{
			ZeroMemory(text, 1024);
			cts = 0;
			size = 1024;
			hr = pDlg->m_multiPlayerForTextRead.GetTextData(text, &size, &cts);
			if( hr == S_OK )
			{
				 pDlg->m_textData.ParseTextData(text, cts);
				 if (text[0] == 'M' && pDlg->m_pGSensorView)
			 	{
			 		//CTextData::ITER_GSENSOR iGSensor = pDlg->m_textData.m_GSensorList.end();
					//pDlg->m_pGSensorView->AppendPoint(iGSensor->m_gseonsorX, iGSensor->m_gseonsorY, iGSensor->m_gseonsorZ, iGSensor->m_speed, 0, iGSensor->m_cts);
					CGSensorData data((gsensor_acc*)&text[1], (float*)&text[7], cts);
					pDlg->m_pGSensorView->AppendPoint(data.m_gseonsorX, data.m_gseonsorY, data.m_gseonsorZ, data.m_speed, 0, data.m_cts);
				}
				else if (text[0] == 'G')
			 	{
			 	}
				 	
			}
			else if( hr == ERROR_HANDLE_EOF )
			{
				break;
			}
			else if(hr == E_FAIL)
			{
			}

			textcount++;
			if(textcount > 20000)
			{
				break;
			}
//			else if(textcount > 500)
//			{
//				::Sleep(0);
//			}
		}
	}
//	if( pDlg->m_pGSensorView )
//		pDlg->m_pGSensorView->SetSensorData( &pDlg->m_textData );

	if(pDlg->m_pGSensorView)
	{
		pDlg->m_pGSensorView->m_bRedrawBlocked = FALSE;
		pDlg->m_pGSensorView->m_nLastRefreshPoint = 0;
		pDlg->m_pGSensorView->m_nDrawIndex = 0;
	}
		
	pDlg->m_multiPlayerForTextRead.CloseFile();
	pDlg->m_bTextDataThreadWorking = FALSE;
	pDlg->m_pTextDataThread = NULL;
	return 0;
}
#endif // end of TEXT_DATA_THREAD_USE

#if TEXT_DATA_THREAD_USE
UINT CBBPlayerDlg::ThreadTextDataProc(void* pArg)
{
	CBBPlayerDlg* pDlg = (CBBPlayerDlg*)pArg;

	if (pDlg == NULL)
	{
		pDlg->m_pTextDataThread = NULL;
		return 0;
	}

	while(pDlg->m_bTextDataThreadExit == FALSE)
	{
		MSG		msg;
		if(GetMessage( &msg, NULL, 0, 0))
		{
			HRESULT hr = 0;
			int textcount = 1;
			unsigned char text[1024];
			int size = 1024;
			unsigned long cts;
			BOOL bPlayResult;
			CString strFileName;
			
			LONGLONG durationTime;
			pDlg->m_bTextDataThreadWorking = TRUE;
			
			switch(msg.message)
			{  
				case THREAD_MSG_RUN : {
					pDlg->m_textData.m_GSensorList.clear();
					pDlg->m_textData.m_GPSList.clear();
					pDlg->m_textData.m_PulseList.clear();

					if(pDlg->m_pGSensorView)
					{
						pDlg->m_pGSensorView->ErasePoint();
						pDlg->m_pGSensorView->m_bRedrawBlocked = TRUE;
					}

	//// ++{*******************************************************************************************
	// i3 strd chunk parsing
					CStrdParser strdParser;
					CT2A ascii(pDlg->m_cstrCurrentPlayFileName.GetBuffer());

					pDlg->m_textData.m_bI3FileType = FALSE;
						
#if 1
					if(strdParser.CStrdParserOpen(ascii.m_psz)){
						DEBUGMSG(1,("#####\r\n####\r\n I3 AVI File : %5.3f\n#####\r\n####\r\n", strdParser.duration));
						pDlg->m_textData.m_bI3FileType = TRUE;
						durationTime = (LONGLONG)(strdParser.duration * 1000);
						//break;
					}

#endif
	//// ++}*******************************************************************************************


					if(pDlg->m_textData.m_bI3FileType == false){
#if defined(__CLBB_NXFS_SUPPORT__)
						if( pDlg->m_fileManager.getCurDriveInfo()){

							DEBUGMSG(1,("drivemode : %d\n", pDlg->m_fileManager.getCurDriveInfo()->m_eDriveMode));
							
							if( pDlg->m_fileManager.getCurDriveInfo()->m_eDriveMode == CWBCFileManager::DRIVE_MODE_REMOVAL_NXFS )
						    {
						    	CWBCFileManager::PSFILEINFO pFileInfo = pDlg->m_fileManager.getListupFileInfo( pDlg->m_nCurPlayIndex );
						        void* pNxfsHandle = pDlg->m_fileManager.getNxFSHandle();
						        bPlayResult = pDlg->m_multiPlayerForTextRead.OpenFile(pFileInfo->m_cstrFileName, pNxfsHandle, pFileInfo->m_fdNxFS );
								 strFileName = pFileInfo->m_cstrFileName;
						    }
						}
					    else
#endif
						{
								bPlayResult = pDlg->m_multiPlayerForTextRead.OpenFile(pDlg->m_cstrCurrentPlayFileName, NULL, -1);
								strFileName = pDlg->m_cstrCurrentPlayFileName;
						}
								
						if(!bPlayResult)
							break;
						
						durationTime = pDlg->m_multiPlayerForTextRead.GetDuration();
					}
					
					if(durationTime)
					{
						CTextData::ITER_GSENSOR iGSensor;
						int mapEnable = 0;
						int isAvicFile = CBBoxUtil::GetFileType(strFileName);
						CWBCSensorData::EGSENSORTYPE gsensor_type = CWBCSensorData::GSENSOR_OLD_8_BIT;
						if(isAvicFile)
							gsensor_type = CWBCSensorData::GSENSOR_AVIC_8_BIT;

						
						pDlg->m_bTextDataThreadParsingStop = FALSE;
						while(pDlg->m_bTextDataThreadExit == FALSE && pDlg->m_bTextDataThreadParsingStop == FALSE)
						{
							ZeroMemory(text, 1024);
							cts = 0;
							size = 1024;

							if(pDlg->m_textData.m_bI3FileType)
								hr = strdParser.GetTextData(ascii.m_psz, text, &size, &cts);
							else
								hr = pDlg->m_multiPlayerForTextRead.GetTextData(text, &size, &cts);
							
							if( hr == S_OK )
							{
								pDlg->m_textData.ParseTextData(text, cts, gsensor_type);

								//printf ("          # %s\r\n", text);
								
								 if (text[0] == 'M' && pDlg->m_pGSensorView)
							 	{
							 			if(pDlg->m_textData.m_GSensorList.size() == 1){
											iGSensor = pDlg->m_textData.m_GSensorList.begin();

////{++ ************************************************************
//20190716  : UI 재생시간과 영상의 타임스템프 동기를 위해 추가된 코드
											ClListCtrl* pFileList = (ClListCtrl*)pDlg->m_pDialogManager->GetControl(TEXT("main.listbox.filelist"));		//LST_GET( TEXT("main.listbox.filelist") );
											if(pFileList){
											    struct tm dt;

												if(pDlg->m_textData.m_bI3FileType){
													dt.tm_year = 70+((text[4]+30)%100);
													dt.tm_mon = text[5]-1;
													dt.tm_mday = text[6];
													dt.tm_hour = text[7];
													dt.tm_min = text[8];
													dt.tm_sec = text[9];
												}
												else {
													dt.tm_sec   =  	text[7+5];
													dt.tm_min   =  	text[7+4];
													dt.tm_hour  =  	text[7+3];
												    
													dt.tm_mday  =   text[7+2];
													dt.tm_mon   = 	text[7+1];
													dt.tm_year  = 	100 + text[7+0];
												}

												dt.tm_isdst = 0;
												
												pFileList->m_dwfile_time = mktime(&dt);
												//pFileList->m_dwfile_time  -= 10;   // jun 170623
											}
////++}*************************************************************											
							 			}
										else
											iGSensor++;

										pDlg->m_pGSensorView->AppendPoint(iGSensor->m_gseonsorX, iGSensor->m_gseonsorY, iGSensor->m_gseonsorZ, iGSensor->m_speed, 0, iGSensor->m_cts);

										if(cts > 2000 && pDlg->m_pUIControl->m_MaxScreenActive == CBBPlayerUIControl::ITEM_MAP && mapEnable == 0){
											pDlg->SetTimer( MAINDLG_TID_ACTIVEDRAW, 50, NULL );
										}
										
								}
								else if (text[0] == 'G')
							 	{
							 		if(mapEnable == 0 && pDlg->m_textData.m_GPSList.size()){
										ClButton*	pBtnMap		= (ClButton*)pDlg->m_pDialogManager->GetControl( TEXT("main.button.map")	);
										//ClWebBrowser*	pMap			= (ClWebBrowser*)pDlg->m_pDialogManager->GetControl( TEXT("main.web.map") );
										
										if( pBtnMap ) pBtnMap->EnableWindow( TRUE );
										//if( pMap ) pMap->ShowWindow( SW_SHOW );
										
										mapEnable = 1;
										
							 		}
							 	}
								 	
							}
							else if( hr == ERROR_HANDLE_EOF )
							{
								break;
							}
							else if(hr == E_FAIL)
							{
							}

							textcount++;
							if(textcount > 30000)
							{
								break;
							}
							//else if(textcount > 500)
							//{
							//	::Sleep(0);
							//}
						}

					//	if( pDlg->m_pGSensorView )
					//		pDlg->m_pGSensorView->SetSensorData( &pDlg->m_textData );

						ClButton* 		pBtnGpsPulse = (ClButton*)pDlg->m_pDialogManager->GetControl( TEXT("main.button.gpspulse") );
						if(pBtnGpsPulse)	{
							 if(pDlg->m_textData.m_bPulseSpeedEnable == 0 && pDlg->m_textData.m_PulseList.size() == 0 ){
								pBtnGpsPulse->SetCheckState(FALSE);
								pDlg->m_textData.m_bPulseSpeedUse = FALSE;
							}
						}
						
						if(pDlg->m_pGSensorView)
						{
							pDlg->m_pGSensorView->m_bRedrawBlocked = FALSE;
							pDlg->m_pGSensorView->m_nLastRefreshPoint = 0;
							pDlg->m_pGSensorView->m_nDrawIndex = 0;
						}
					}
					//pDlg->m_multiPlayerForTextRead.CloseFile();
					break;
				}
				
			case THREAD_MSG_EXIT: {
	//				pDlg->m_multiPlayerForTextRead.CloseFile();
					pDlg->m_bTextDataThreadExit  = TRUE;
					break;
				}
			}

			pDlg->m_bTextDataThreadWorking = FALSE;
		}	
	}
	
	pDlg->m_bTextDataThreadWorking = FALSE;
	pDlg->m_pTextDataThread = NULL;
	return 0;
}
#endif // end of TEXT_DATA_THREAD_USE


void CBBPlayerDlg::OnBnClickedMainBtnFullF()
{
	ClVideoView*		pVideoCtrl= VID_GET( TEXT("main.video.quadctrl"));
	if(pVideoCtrl && pVideoCtrl->GetQuadViewMode()) {
		pVideoCtrl->SetQuadViewMode(FALSE);
		this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_MUX );
	}
	
	this->m_pUIControl->m_ActiveFront = this->m_pUIControl->m_MaxScreenActive = CBBPlayerUIControl::ITEM_FRONT_VIDEO;
	this->m_pUIControl->SetActiveItem();
}

void CBBPlayerDlg::OnBnClickedMainBtnFullR()
{
	ClVideoView*		pVideoCtrl= VID_GET( TEXT("main.video.quadctrl"));
	if(pVideoCtrl && pVideoCtrl->GetQuadViewMode()) {
		pVideoCtrl->SetQuadViewMode(FALSE);
		this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_MUX );
	}
	
	this->m_pUIControl->m_ActiveFront = this->m_pUIControl->m_MaxScreenActive = CBBPlayerUIControl::ITEM_REAR_VIDEO;
	this->m_pUIControl->SetActiveItem();
}

void CBBPlayerDlg::OnBnClickedMainBtnFullM()
{
	ClVideoView*		pVideoCtrl= VID_GET( TEXT("main.video.quadctrl"));
	if(pVideoCtrl && pVideoCtrl->GetQuadViewMode()) {
		pVideoCtrl->SetQuadViewMode(FALSE);
		this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_MUX );
	}
	
	this->m_pUIControl->m_ActiveFront = this->m_pUIControl->m_MaxScreenActive = CBBPlayerUIControl::ITEM_MAP;
	this->m_pUIControl->SetActiveItem();
}

void CBBPlayerDlg::OnBnClickedMainBtnFileListAll()
{
	ClButton* pBtnSave = BTN_GET( TEXT("main.button.save") );
	ClButton* 		pBtnAll = BTN_GET( TEXT("main.button.checkfilelistall") );
	ClListCtrl* pFileList = LST_GET( TEXT("main.listbox.filelist") );
	if(pFileList){
		BOOL isChecked;
		if(pBtnAll)	
			isChecked = pBtnAll->GetCheckState();

		int nCount=pFileList->GetItemCount(); 
		
		if(isChecked){
			for(int i = 0; i< nCount;i++) 
				pFileList->SetCheck(i); 

			if( pBtnSave && nCount) pBtnSave->EnableWindow( TRUE );
		}
		else {
			for(int i = 0; i< nCount;i++) 
				pFileList->SetCheck(i, FALSE); 

			if( pBtnSave ) pBtnSave->EnableWindow( FALSE );
		}
	}
}

void CBBPlayerDlg::OnBnClickedMainBtnFormat()
{
	this->StopMedia();

	CNXFSFormatterDlg formatterDlg;
	formatterDlg.SetLanguage(&this->m_pUIControl->m_Language);
	formatterDlg.SetSelDrive( this->m_cstrSelectDrive );
	formatterDlg.DoModal();
}

void CBBPlayerDlg::OnBnClickedMainBtnGpsPulse()
{
	ClButton* 		pBtnGpsPulse = BTN_GET( TEXT("main.button.gpspulse") );
	BOOL isChecked;
	if(pBtnGpsPulse)	{
		isChecked = pBtnGpsPulse->GetCheckState();

		 if(isChecked == 0 ){
			this->m_textData.m_bPulseSpeedEnable = FALSE;
		}
		 else {
		 	this->m_textData.m_bPulseSpeedEnable = TRUE;
		 }

		 if (isChecked == 0 || this->m_textData.m_PulseList.size() == 0 || (m_pUIControl->m_bTachoDisplayEnable && m_pUIControl->m_TachoDisplayPULSE.m_bPulseState == 0)){
		 	pBtnGpsPulse->SetCheckState(FALSE);
			this->m_textData.m_bPulseSpeedUse = FALSE;
	 	}
		
	}

	this->m_pUIControl->UserDataSynchronize(this->m_multiPlayer.GetCurrentPosition() , TRUE);
}

void CBBPlayerDlg::OnStnClickedMainVideoFront()
{
	ClVideoView*		pVideoCtrl= VID_GET( TEXT("main.video.quadctrl"));
	ClVideoView*		pVideoFront= VID_GET( TEXT("main.video.front"));

	if(pVideoCtrl && pVideoCtrl->GetQuadViewMode()) {
		pVideoCtrl->SetQuadViewMode(FALSE);
		this->m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_MUX );
		this->m_pUIControl->UserDataSynchronize(this->m_multiPlayer.GetCurrentPosition() , TRUE);
	}
}

void CBBPlayerDlg::OnStnDblclickMainVideoFront()
{
	this->m_pUIControl->m_ActiveFront = this->m_pUIControl->m_MaxScreenActive = CBBPlayerUIControl::ITEM_FRONT_VIDEO;
	
	if(this->m_nCurMode == 0)
		OnClickMaximize();	
	else
		OnClickNormalize();
}


void CBBPlayerDlg::OnStnClickedMainVideoRear()
{
	if(this->m_nCurMode == 0){
		int nPlayState = m_multiPlayer.GetState();
		ClButton* pBtnFRChange = BTN_GET( this->m_pUIControl->m_Language.GetButtonText(TEXT("main.button.frchange")) );
	  
	 	 if(pBtnFRChange && pBtnFRChange->GetCheckState())
			 return;
		
		if (nPlayState == PLAY_STATE_RUNNING || nPlayState == PLAY_STATE_PAUSED)
		{
			ClVideoView*		pVideoCtrl= VID_GET( TEXT("main.video.quadctrl"));
			ClVideoView*		pVideoRear= VID_GET( TEXT("main.video.rear"));
			ClVideoView*		pVideoFront= VID_GET( TEXT("main.video.front"));
			if(pVideoCtrl && pVideoCtrl->GetQuadViewMode() == FALSE) {
				
				if(pVideoRear) pVideoCtrl->SetQuadCtrlChannel(pVideoRear->GetQuadCtrlChannel());
				
				pVideoCtrl->SetQuadViewMode(TRUE);				
				m_pUIControl->SendUIEvent( CBBPlayerUIControl::EVENT_MUX );
			}

			if(pVideoCtrl && pVideoCtrl->GetQuadViewMode()) {
				m_MuxViewCurrentTime = 0xffffff;
				m_iMuxViewChannel = 4;
				this->OnDrawMuxImage(1, 0);
			}
		}
	}
	return;
}

void CBBPlayerDlg::OnStnDblclickMainVideoRear()
{
	this->m_pUIControl->m_ActiveFront = this->m_pUIControl->m_MaxScreenActive = CBBPlayerUIControl::ITEM_REAR_VIDEO;
	
	if(this->m_nCurMode == 0)
		OnClickMaximize();	
	else
		OnClickNormalize();
}



