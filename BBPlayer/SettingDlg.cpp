
#include "stdafx.h"
#include "BBPlayer.h"
#include "SettingDlg.h"
#include <CLUDE/version.h>
#include <CLENAW/ClImage.h>
#include <CLENAW/ClText.h>
#include <CLENAW/ClButton.h>
#include <CLENAW/ClSlider.h>
#include <HELPER/ClLog.H>
#include <HELPER/CLUtil.H>

//#include "SelDriveDlg.h"
#include "NotifyDlg.h"
#include "PasswordSettingDlg.h"

#define TIMER_SYSTEM		500

#define MULT_LANGUAGE_USE 	0

IMPLEMENT_DYNAMIC(CSettingDlg, ClDialog)

static int CALLBACK BrowseForFolder_CallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
   if(uMsg == BFFM_INITIALIZED)
       SendMessage(hwnd, BFFM_SETSELECTION, (WPARAM)TRUE, (LPARAM)lpData);
   return 0;
}

CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: ClDialog(CSettingDlg::IDD, pParent), m_cstrPassword(TEXT("")), m_cstrSelDrive(TEXT(""))        // jun 200805
{
	m_bChangedEventMode = FALSE;
	m_pCLanguage = NULL;
	m_ipreEventMode = 0;

	m_bChangedVideoQualit = FALSE;
	m_ipreVideoQuality = 0;
	m_ipreGsensorSensi = 0;
}

CSettingDlg::~CSettingDlg()
{
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	ClDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SETTING_COMBO_DISP_TIMEZONE, m_cbTimeZone);
	DDX_Control(pDX, IDC_SETTING_COMBO_DISP_TIME_OFFSET, m_cbTimeOffset);

	//DDX_Text(pDX, IDC_SETTING_STATIC_DISPLAY_TIMESET_VALUE, m_csCurrenttime);

	DDX_Radio(pDX, IDC_SETTING_QUALITY_HIGH, m_stCfgDaview.iVideoQuality);
	DDX_Radio(pDX, IDC_SETTING_EVENT_CAPACITY_A, m_stCfgDaview.iEventMode);
	DDX_Radio(pDX, IDC_SETTING_RECORD_AUDIO_OFF, m_stCfgDaview.iAudioRecEnable);
	DDX_Radio(pDX, IDC_SETTING_PULSE_RESET_OFF, m_stCfgDaview.iPulseReset);
	DDX_Radio(pDX, IDC_SETTING_PULSE_UD_TACHO1_LOW, m_stCfgDaview.iPulseTacho1LowHigh);
	DDX_Radio(pDX, IDC_SETTING_PULSE_UD_TACHO4_LOW, m_stCfgDaview.iPulseTacho4LowHigh);
	DDX_Radio(pDX, IDC_SETTING_PULSE_UD_TACHO5_LOW, m_stCfgDaview.iPulseTacho5LowHigh);
	DDX_Radio(pDX, IDC_SETTING_OSDSPEED_OFF, m_stCfgDaview.iOsdSpeed);
	DDX_Text(pDX, IDC_SETTING_EDIT_CARNUMBER, m_csCarNumber);  // jun 200805
	DDX_Text(pDX, IDC_SETTING_EDIT_DRIVERCODE, m_csDriverCode);  // jun 221017
}


VOID CSettingDlg::InitResourceID( VOID )
{
	// default
	this->m_pDialogManager->AddResource( TEXT("setting.text.title"),			TYPE_TEXT,		IDC_SETTING_TEXT_TITLE		);
	this->m_pDialogManager->AddResource( TEXT("setting.button.exit"),			TYPE_BUTTON,		IDC_SETTING_CLOSE );
	this->m_pDialogManager->AddResource( TEXT("setting.slider.gsensorsens"),		TYPE_SLIDER,			IDC_SETTING_SLD_GSENSORSENS );
	this->m_pDialogManager->AddResource( TEXT("setting.slider.speakervolume"),		TYPE_SLIDER,			IDC_SETTING_SLD_SPEAKER_VOLUME);
	this->m_pDialogManager->AddResource( TEXT("setting.slider.enginecylinders"),		TYPE_SLIDER,			IDC_SETTING_SLD_ENGINE_CYLINDERS);  // jun 20201202

	this->m_pDialogManager->AddResource( m_pCLanguage->GetButtonText(TEXT("setting.button.settime")),			TYPE_BUTTON,		IDC_SETTING_BTN_SETTIME );
//    this->m_pDialogManager->AddResource( m_pCLanguage->GetButtonText(TEXT("setting.button.pulsereset")),			TYPE_BUTTON,		IDC_SETTING_BTN_PULSERESET );

	this->m_pDialogManager->AddResource( m_pCLanguage->GetButtonText(TEXT("setting.button.format")),				TYPE_BUTTON,		IDC_SETTING_BTN_FORMAT );
#ifdef BUILD_PASSWORD_USE
	this->m_pDialogManager->AddResource(  m_pCLanguage->GetButtonText(TEXT("setting.button.password")),				TYPE_BUTTON,		IDC_SETTING_BTN_PASSWORD);
#endif
	this->m_pDialogManager->AddResource(  m_pCLanguage->GetButtonText(TEXT("setting.button.init")),				TYPE_BUTTON,		IDC_SETTING_INIT	);
	this->m_pDialogManager->AddResource(  m_pCLanguage->GetButtonText(TEXT("setting.button.save")),				TYPE_BUTTON,		IDC_SETTING_SAVE	);
	this->m_pDialogManager->AddResource(  m_pCLanguage->GetButtonText(TEXT("setting.button.cancel")),				TYPE_BUTTON,		IDC_SETTING_CANCEL	);

	return;
}


VOID CSettingDlg::SetResourceState( VOID )
{
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("setting.text.title"),				TRUE, TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("setting.button.exit"),				TRUE, TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("setting.slider.gsensorsens"),		TRUE, TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("setting.slider.speakervolume"),			TRUE, TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("setting.slider.enginecylinders"),			TRUE, TRUE	);  // jun 20201202
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_pCLanguage->GetButtonText(TEXT("setting.button.settime")),			TRUE, TRUE	);
//	this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_pCLanguage->GetButtonText(TEXT("setting.button.pulsereset")),			TRUE, TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_pCLanguage->GetButtonText(TEXT("setting.button.format")),			TRUE, TRUE	);
#ifdef BUILD_PASSWORD_USE
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_pCLanguage->GetButtonText(TEXT("setting.button.password")),				TRUE, TRUE	);
#endif
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_pCLanguage->GetButtonText(TEXT("setting.button.init")),				TRUE, TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_pCLanguage->GetButtonText(TEXT("setting.button.save")),				TRUE, TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_pCLanguage->GetButtonText(TEXT("setting.button.cancel")),			TRUE, TRUE	);
	
	this->m_pDialogManager->AdaptResourceState( this->m_nCurMode );

	return;
}


void CSettingDlg::SetSelDrive( CString cstrSelDrive )
{
	this->m_cstrSelDrive = cstrSelDrive;

	return;
}

void CSettingDlg::SetLanguage( CLanguage * cLang)
{
	m_pCLanguage = cLang;
	
	return;
}


BEGIN_MESSAGE_MAP(CSettingDlg, ClDialog)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SETTING_CLOSE, 			&CSettingDlg::OnBnClickedSettingClose)
	ON_BN_CLICKED(IDC_SETTING_INIT,				&CSettingDlg::OnClickSettingInit	)
	ON_BN_CLICKED(IDC_SETTING_SAVE,			    &CSettingDlg::OnClickSettingSave	)
	ON_BN_CLICKED(IDC_SETTING_CANCEL,			&CSettingDlg::OnClickSettingCancel	)
	ON_BN_CLICKED(IDC_SETTING_BTN_FORMAT,		&CSettingDlg::OnClickFormatDisk		)
	ON_BN_CLICKED(IDC_SETTING_BTN_PASSWORD,		&CSettingDlg::OnClickPassword		)
	ON_BN_CLICKED(IDC_SETTING_BTN_SETTIME,		&CSettingDlg::OnClickSetTime		)
	ON_BN_CLICKED(IDC_SETTING_BTN_PULSERESET,	&CSettingDlg::OnClickPulsereset		)
	ON_MESSAGE( WM_CLENAW_SLIDER,				&CSettingDlg::OnSliderScroll		)
	ON_BN_CLICKED(IDC_SETTING_EVENT_CAPACITY_A, &CSettingDlg::OnBnClickedSettingEventCapacity)
	ON_BN_CLICKED(IDC_SETTING_EVENT_CAPACITY_B, &CSettingDlg::OnBnClickedSettingEventCapacity)
	ON_BN_CLICKED(IDC_SETTING_EVENT_CAPACITY_C, &CSettingDlg::OnBnClickedSettingEventCapacity)
	ON_BN_CLICKED(IDC_SETTING_QUALITY_HIGH, &CSettingDlg::OnBnClickedSettingQuality)
	ON_BN_CLICKED(IDC_SETTING_QUALITY_MIDDLE, &CSettingDlg::OnBnClickedSettingQuality)
	ON_BN_CLICKED(IDC_SETTING_QUALITY_LOW, &CSettingDlg::OnBnClickedSettingQuality)
	ON_STN_CLICKED(IDC_SETTING_STATIC_SPEAKER_VOLUME_VALUE, &CSettingDlg::OnStnClickedSettingStaticSpeakerVolumeValue)
END_MESSAGE_MAP()


BOOL CSettingDlg::OnInitDialog()
{
	ClDialog::OnInitDialog();

	m_hBgBrush = ::CreateSolidBrush(RGB(59, 63, 72));
	
	this->SetCurrentMode( 0 );
	this->SetResourceState();
	ClButton* pBtnVideo = BTN_GET( TEXT("setting.button.videotab") );
	if( pBtnVideo ) pBtnVideo->SetCheckState();

	CBBConfigure* pSrcConfig = ((CBBPlayerApp*)AfxGetApp())->GetConfigure();
	if( !pSrcConfig )
	{
		CLD_FL( DF_ERROR, "Can not find global Configuration object." );
		return FALSE;
	}

	this->m_Configure.cloneConfig( *pSrcConfig );
	this->OnClickSettingInit();


	//config init
	m_bTimeSetSave = FALSE;
	m_bConfigOpen = FALSE;

	CString data;
	int time;
	for(time = -12; time <13; time++)
	{
		data.Format(TEXT("GMT %c %02d:%02d"), ((time < 0) ?  '-' : '+'), ((time < 0) ? -time : time), 0);

#if (BUILD_LANGUAGE  == LANGUAGE_KOREAN)
		if(time  == 9)
			data.Append(TEXT("        Korea"));
#endif

		m_cbTimeZone.AddString(data);
	}

	for(time = 0; time < 7; time++)
	{
		if(time==0)
			data.Format(TEXT("Offset"));
		else
			data.Format(TEXT("+%d Min"), time*5);

		m_cbTimeOffset.AddString(data);
	}

	this->OnLanguageChange();

	WIN32_FIND_DATA	findData;
	HANDLE			hDir = NULL;
	CString csDrive = this->m_cstrSelDrive.Left(3);
	
	TCHAR   pszLabel[MAX_PATH];

	::GetVolumeInformation(csDrive.GetBuffer(), pszLabel, MAX_PATH, NULL, NULL, NULL, NULL, NULL);
	
	csDrive.Append(BLACKBOX_CONFIG_FILE);

	CString	csVolume = pszLabel;

//	if(csVolume.Find(FORMAT_LABEL) >= 0)
	{
		hDir = FindFirstFileEx( csDrive, FindExInfoStandard, &findData, FindExSearchNameMatch, NULL, 0 );
		if( hDir != INVALID_HANDLE_VALUE )
		{
			m_bConfigOpen = TRUE;
			
			FindClose(hDir);
		}
	}

	if( CConfigText::Load(csDrive, &m_stCfgDaview))
	{
		this->InitConfigValue(&m_stCfgDaview);
	}
	else	
	{
		this->InitConfigValue();
	}
  
	((CEdit*)GetDlgItem(IDC_SETTING_EDIT_CARNUMBER))->SetLimitText(4);        // jun 200805

#ifndef CARNUMBER          // jun 200812 CARNUMBER Delete
	((CEdit*)GetDlgItem(IDC_SETTING_EDIT_CARNUMBER))->ShowWindow(SW_HIDE);
	((CStatic*)GetDlgItem(IDC_SETTING_STATIC_CARNUMBER))->ShowWindow(SW_HIDE);
#endif	

	((CEdit*)GetDlgItem(IDC_SETTING_EDIT_DRIVERCODE))->SetLimitText(64);        // jun 221017
#ifndef DRIVERCODE          // jun 221017 DRIVERCODE Delete
	((CEdit*)GetDlgItem(IDC_SETTING_EDIT_DRIVERCODE))->ShowWindow(SW_HIDE);
	((CStatic*)GetDlgItem(IDC_SETTING_STATIC_DRIVERCODE))->ShowWindow(SW_HIDE);
#endif	


	SetTimer(TIMER_SYSTEM,1000,NULL) ;	//1000 - 1초
	
	return TRUE;
}

HBRUSH CSettingDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	//UINT nID = pWnd->GetDlgCtrlID();

	//if(nID)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetBkColor(RGB(59, 63, 72));
		pDC->SetTextColor(RGB(255, 255, 255));
		return m_hBgBrush;
	}

	return hbr;
}

void CSettingDlg::OnTimer(UINT nIDEvent)
{
	switch(nIDEvent)
	{
	case TIMER_SYSTEM :
		{
			if(this->IsWindowVisible())
			{
				static int OldSec = 0 ;
				tm m_today;
				struct tm *today = NULL ;
				time_t ltime;
				
				time(&ltime);
				ltime += m_cbTimeOffset.GetCurSel() * 5 * 60;
				today = localtime(&ltime);

  				if( OldSec != today->tm_sec )
				{
					m_today.tm_year = today->tm_year + 1900 ;
					m_today.tm_mon = today->tm_mon + 1 ;
					m_today.tm_mday = today->tm_mday ;
					m_today.tm_hour = today->tm_hour ;
					m_today.tm_min = today->tm_min ;
					m_today.tm_sec = today->tm_sec ;

					m_csCurrenttime.Format(TEXT("%04d/%02d/%02d - %02d:%02d:%02d"), 
											m_today.tm_year,
											m_today.tm_mon,
											m_today.tm_mday,
											m_today.tm_hour,
											m_today.tm_min,
											m_today.tm_sec);
					OldSec = today->tm_sec;
					
					SetDlgItemText(IDC_SETTING_STATIC_DISPLAY_TIMESET_VALUE, m_csCurrenttime); 
				}
			}
		}
		break;
		
	default :
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

void CSettingDlg::OnBnClickedSettingClose()
{
	EndDialog( IDOK );
	return;
}

void CSettingDlg::OnClickSettingInit()
{
	INT value = 0;

	CConfigText::CfgDefaultSet(&m_stCfgDaview);
	this->InitConfigValue(&m_stCfgDaview);
	
	CBBConfigure* pSrcConfig = ((CBBPlayerApp*)AfxGetApp())->GetConfigure();
	this->m_Configure.cloneConfig( *pSrcConfig );

	return;
}


void CSettingDlg::OnClickSettingSave()
{
	ClSlider* pSldGSensor = SLD_GET( TEXT("setting.slider.gsensorsens") );
	ClSlider* pSldVolume = SLD_GET( TEXT("setting.slider.speakervolume") );
	ClSlider* pSldCylinders = SLD_GET( TEXT("setting.slider.enginecylinders") );  // jun 20201202
	UpdateData();
	
	//display

	m_stCfgDaview.iGmt = m_cbTimeZone.GetCurSel() - 12;
	//record
	//device
	if(pSldGSensor)
	{
		if(pSldGSensor->GetRangeMax())
			m_stCfgDaview.iGsensorSensi = pSldGSensor->GetPos();
	}
	
	if(pSldVolume)		m_stCfgDaview.iSpeakerVol = pSldVolume->GetPos();

	if(pSldCylinders)		  // jun 20201202
{
		if(pSldCylinders->GetRangeMax())
			m_stCfgDaview.iEngineCylinders = pSldCylinders->GetPos();
	}

  
	ClUtil::STRING::convertWchar2Char(m_stCfgDaview.strCarNumber, m_csCarNumber.GetBuffer(),  m_csCarNumber.GetLength()+1);        // jun 200805
	wcscpy(m_stCfgDaview.strDriverCode, m_csDriverCode.GetBuffer());        // jun 221017

	//system
		
	CConfigText::Save(&m_stCfgDaview);


	TCHAR   pszPathname[MAX_PATH];
	wcscpy(pszPathname, m_cstrSelDrive.GetBuffer());
	if(!m_bConfigOpen)
	{
		if(this->SelectMemory(pszPathname))
			pszPathname[3] = 0;
		else
			return;
	}

	pszPathname[2] = 0;

	CString str;

	if(m_ipreGsensorSensi != m_stCfgDaview.iGsensorSensi){
		CString time_string = CConfigText::make_time_string(time(0));
		ClUtil::STRING::convertWchar2Char(m_stCfgDaview.strLastSetupTime, time_string.GetBuffer(),  time_string.GetLength()+1);
		
		m_ipreGsensorSensi = m_stCfgDaview.iGsensorSensi;
	}
	
	str = CConfigText::Save(pszPathname, &m_stCfgDaview);

	if(m_bChangedEventMode){
		this->SaveFormatTextFile(pszPathname, BLACKBOX_FORMAT_FILE_NAME);
	}
	else if(m_bChangedVideoQualit){
		this->SaveFormatTextFile(pszPathname, BLACKBOX_VRHD_FORMAT_FILE_NAME);
	}

	this->m_pCLanguage->GetLenguageText(&str , CLanguage::TEXT_FILESAVECOMPLETE);

	AfxMessageBox(CString(pszPathname) +_T("\\") + BLACKBOX_CONFIG_FOLDER  + BLACKBOX_CONFIG_FILE_NAME + _T("  ") +  str, MB_ICONINFORMATION);

#if 0
	////////////////////////////////////////////////////////////////////////
	CBBConfigure* pSrcConfig = ((CBBPlayerApp*)AfxGetApp())->GetConfigure();
	pSrcConfig->cloneConfig( this->m_Configure );

	CString cstrConfigPath;
	ClUtil::PATH::makeValidPath( this->m_cstrSelDrive + TEXT("\\") +
		((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getConfigPath(), cstrConfigPath );

	pSrcConfig->writeConfig( cstrConfigPath );
#endif

	EndDialog( IDOK );
	return;
}


void CSettingDlg::OnClickSettingCancel()
{
	EndDialog( IDCANCEL );
	return;
}

void CSettingDlg::OnClickFormatDisk()
{
	BOOL bFound = FALSE;
	CString cstrDrivePath = TEXT(" :\\");
	DWORD drive_info = ::GetLogicalDrives();
	INT nDriveIndex;
	
	this->GetParent()->PostMessage( WM_USER_MEDIAFORCESTOP );

#if 0		
	
	for( nDriveIndex = 0; drive_info; nDriveIndex++ )
	{
		if(drive_info & 1)
		{
			cstrDrivePath.Format( TEXT("%c:\\"),'A' + nDriveIndex );
			if( (::GetDriveType(cstrDrivePath) == DRIVE_REMOVABLE)
				&& (cstrDrivePath == this->m_cstrSelDrive) )
			{
				bFound = TRUE;
				break;
			}
		}
		drive_info >>=1;
	}

	if( bFound )
		SHFormatDrive( this->GetSafeHwnd(), nDriveIndex, SHFMT_ID_DEFAULT, 0 );
#else
	TCHAR   pszPathname[MAX_PATH];
	TCHAR   pszVolumeName[MAX_PATH];

	if(!this->SelectMemory(pszPathname, pszVolumeName))
		return;
	
	pszPathname[3] = 0;
	nDriveIndex = pszPathname[0] - 'A';
	DEBUGMSG(1,("drive : %s	(%d) \n", pszPathname, nDriveIndex));
		
	SHFormatDrive(this->m_hWnd, nDriveIndex, SHFMT_ID_DEFAULT, 0 /*SHFMT_OPT_FULL*/);

#endif
	return;
}

void CSettingDlg::OnClickPassword()
{
#ifdef BUILD_PASSWORD_USE
	CPasswordSettingDlg pwDlg;
	pwDlg.SetLanguage(this->m_pCLanguage);
	pwDlg.SetOldPassword(m_cstrPassword.GetBuffer(), DEFAULT_MASTERKEY, DEFAULT_MASTERKEY);
	if(pwDlg.DoModal() == IDOK) // change ok
	{
		m_cstrPassword = pwDlg.GetPassword();
	}
#endif	
	return;
}

void CSettingDlg::OnClickSetTime()
{
#if 0
	INT nReturn = IDOK;
	OPEN_SLECTION_DIALOG( nReturn, NOTIFY_QUES_WANT_SETTIME ,m_pCLanguage );
	if( nReturn != IDOK )
		return;
#endif
	char tmp[MAX_PATH] = { 0, };
	time_t curTime = 0;
	struct tm *curLocalTime = NULL;

	curTime = time(NULL)+1; // tony , +1 is usb send delay time
	curTime += m_cbTimeOffset.GetCurSel() * 5 * 60;
	curLocalTime = localtime(&curTime);

	memset(tmp, 0, MAX_PATH);
	sprintf(tmp, "%04d/%02d/%02d %02d:%02d:%02d", 
	curLocalTime->tm_year + 1900, curLocalTime->tm_mon + 1, curLocalTime->tm_mday,
	curLocalTime->tm_hour, curLocalTime->tm_min, curLocalTime->tm_sec);

	printf("SET RTC : %s\r\n", tmp);

	{
		CString str;
		//AfxMessageBox("Device not found.\n");
		
		
		TCHAR   pszPathname[MAX_PATH];
		if(m_bConfigOpen)
		{
			wcscpy(pszPathname, m_cstrSelDrive.GetBuffer());
		}
		else
		{
			if(!this->SelectMemory(pszPathname))
				return;
		}

		pszPathname[2] = 0;
		int drive = pszPathname[0] - 'A';
		
		if(drive > 2 && pszPathname[1] == ':') // more than c:
		{
			this->m_stTime.nYear = curLocalTime->tm_year + 1900;
			this->m_stTime.nMonth = curLocalTime->tm_mon + 1;
			this->m_stTime.nDate = curLocalTime->tm_mday;
			this->m_stTime.nHour = curLocalTime->tm_hour;
			this->m_stTime.nMinute = curLocalTime->tm_min;
			this->m_stTime.nSec = curLocalTime->tm_sec;
			this->m_stTime.nTimeSet = 1;
			
			str = CConfigText::Save(pszPathname, &this->m_stTime);

			m_bTimeSetSave = TRUE;

#if (BUILD_LANGUAGE == LANGUAGE_KOREAN)			
			str.Append(TEXT(" 저장 하였습니다.\n"));
			str.Append(TEXT("에스디 카드를 제품에 삽입하시고 \n설정 시간에 맞춰 전원을 연결하여 주십시오.\n"));
#else
			this->m_pCLanguage->GetLenguageText(&str , CLanguage::TEXT_FILESAVECOMPLETE);
			//str.Append(TEXT("SD card is inserted into the product. \nPlease turn on the power switch when the set time\n"));
#endif
			AfxMessageBox(str, MB_ICONINFORMATION);	
		}
		else
		{
			str.Format(TEXT("%s is system drive."), pszPathname);
			AfxMessageBox(str, MB_ICONERROR);
		}
	}
	
	return;
}

void CSettingDlg::OnClickPulsereset()
{
	m_stCfgDaview.iPulseReset = TRUE;
}



LRESULT CSettingDlg::OnSliderScroll( WPARAM wParam, LPARAM lParam )
{
	ClSlider* pSlider = (ClSlider*)wParam;
	INT nPos = (INT)lParam;

	ClSlider* pSldTGSensor	= SLD_GET( TEXT("setting.slider.gsensorsens") );
	ClSlider* pSldVolume = SLD_GET( TEXT("setting.slider.speakervolume") );
	ClSlider* pSldCylinders = SLD_GET( TEXT("setting.slider.enginecylinders") );  // jun 20201202

	if( pSldTGSensor == pSlider )	this->updateGsensorValue(nPos, pSldTGSensor->GetRangeMax());
	else if( pSldVolume == pSlider )	this->updateVolumeValue(nPos);
	else if( pSldCylinders == pSlider )	this->updateCylindersValue(nPos, pSldCylinders->GetRangeMax());  // jun 20201202
	else
	{
		CLD_FL( DF_ERROR, "Unknown slider control has been found. You need to add case to process messages");;
	}

	return 0;
}

void CSettingDlg::OnLanguageChange(void)
{
	CString strText;

	if(m_pCLanguage == NULL)
		return;

	//title
	SetItemText(IDC_SETTING_TEXT_TITLE, CLanguage::TEXT_SETTING);

	//display
	SetItemText(IDC_SETTING_STATIC_DISPLAY_TIMEZONE, CLanguage::TEXT_TIMEZONE, L"1. ");
	SetItemText(IDC_SETTING_STATIC_DISPLAY_TIMESET, CLanguage::TEXT_TIMESETTING, L"2. ");

	// record

	if(this->m_pCLanguage->GetLenguageText(&strText , CLanguage::TEXT_VIDEO))
//		SetItemText(IDC_SETTING_STATIC_VIDEO_QUALITY, CLanguage::TEXT_QUALITY, L"3. " + strText + L" ");
		SetItemText(IDC_SETTING_STATIC_VIDEO_QUALITY, CLanguage::TEXT_QUALITY, L"3. ");

		SetItemText(IDC_SETTING_QUALITY_HIGH, CLanguage::TEXT_HIGH);
		SetItemText(IDC_SETTING_QUALITY_MIDDLE, CLanguage::TEXT_MIDDLE);
		SetItemText(IDC_SETTING_QUALITY_LOW, CLanguage::TEXT_LOW);

	SetItemText(IDC_SETTING_STATIC_EVENT_CAPACITY, CLanguage::TEXT_EVENT_CAPACITY, L"4. ");
	
	if(this->m_pCLanguage->GetLenguageText(&strText , CLanguage::TEXT_AUDIO))
//		SetItemText(IDC_SETTING_STATIC_RECORD_AUDIO, CLanguage::TEXT_SAVE, L"5. " + strText + L" ");
		SetItemText(IDC_SETTING_STATIC_RECORD_AUDIO, CLanguage::TEXT_AUDIO, L"5. ");

		SetItemText(IDC_SETTING_RECORD_AUDIO_OFF, CLanguage::TEXT_OFF);
		SetItemText(IDC_SETTING_RECORD_AUDIO_ON, CLanguage::TEXT_ON);
		
	if(this->m_pCLanguage->GetLenguageText(&strText , CLanguage::TEXT_VOICE_GUIDANCE))
		SetItemText(IDC_SETTING_STATIC_SPEAKER_VOLUME, CLanguage::TEXT_VOLUME, L"6. " + strText + L" ");

	SetItemText(IDC_SETTING_STATIC_ENGINE_CYLINDERS, CLanguage::TEXT_ACCIDENTCYLINDERS, L"8. ");  // jun 20201202
//		SetItemText(IDC_SETTING_STATIC_ENGINE_CYLINDERS_LOW, CLanguage::TEXT_CYLINDERSLOW, L" ");
//		SetItemText(IDC_SETTING_STATIC_ENGINE_CYLINDERS_HIGH, CLanguage::TEXT_CYLINDERSHIGH,  L" ");
		
	SetItemText(IDC_SETTING_STATIC_DEVICE_GSENSOR_SENSITIVE, CLanguage::TEXT_ACCIDENTSENS, L"7. ");
		SetItemText(IDC_SETTING_STATIC_DEVICE_GSENSOR_LOW, CLanguage::TEXT_SENSIBILITY, L" ");
		SetItemText(IDC_SETTING_STATIC_DEVICE_GSENSOR_HIGH, CLanguage::TEXT_INSENSIBILITY,  L" ");

	SetItemText(IDC_SETTING_STATIC_PULSERESET, CLanguage::TEXT_PULSERESET, L"9. ");
		SetItemText(IDC_SETTING_PULSE_RESET_OFF, CLanguage::TEXT_OFF);
		SetItemText(IDC_SETTING_PULSE_RESET_ON, CLanguage::TEXT_ON);

	SetItemText(IDC_SETTING_STATIC_PULSE_UD_TACHO1, CLanguage::TEXT_BRAKE, L"10. ");
//		SetItemText(IDC_SETTING_PULSE_UD_TACHO1_LOW, CLanguage::TEXT_LOW);
//		SetItemText(IDC_SETTING_PULSE_UD_TACHO1_HIGH, CLanguage::TEXT_HIGH);

	SetItemText(IDC_SETTING_STATIC_PULSE_UD_TACHO4, CLanguage::TEXT_INPUT1, L"11. ");
//		SetItemText(IDC_SETTING_PULSE_UD_TACHO4_LOW, CLanguage::TEXT_LOW);
//		SetItemText(IDC_SETTING_PULSE_UD_TACHO4_HIGH, CLanguage::TEXT_HIGH);

	SetItemText(IDC_SETTING_STATIC_PULSE_UD_TAHCO5, CLanguage::TEXT_INPUT2, L"12. ");
//		SetItemText(IDC_SETTING_PULSE_UD_TACHO5_LOW, CLanguage::TEXT_LOW);
//		SetItemText(IDC_SETTING_PULSE_UD_TACHO5_HIGH, CLanguage::TEXT_HIGH);

	SetItemText(IDC_SETTING_STATIC_OSDSPEED, CLanguage::TEXT_OSDSPEED, L"13. ");  // jun 200813
//		SetItemText(IDC_SETTING_OSDSPEED_OFF, CLanguage::TEXT_OFF);
//		SetItemText(IDC_SETTING_OSDSPEED_ON, CLanguage::TEXT_ON);

	SetItemText(IDC_SETTING_STATIC_DRIVERCODE, CLanguage::TEXT_DRIVERCODE, L"14. ");  // jun 221017
	SetItemText(IDC_SETTING_STATIC_CARNUMBER, CLanguage::TEXT_CARNUMBER, L"15. ");  // jun 200805



}

void  CSettingDlg::SetItemText(int nIDDlgItem, UINT resource, CString index_str)
{
	CString strText;
	if(m_pCLanguage)
	{
		if(this->m_pCLanguage->GetLenguageText(&strText , resource))
			this->GetDlgItem(nIDDlgItem)->SetWindowText(index_str + strText);
	}
}

BOOL CSettingDlg::InitConfigValue(LPST_CFG_DAVIEW spConfig)
{
	ClSlider* pSldGSensor = SLD_GET( TEXT("setting.slider.gsensorsens") );
	ClSlider* pSldVolume = SLD_GET( TEXT("setting.slider.speakervolume") );
	ClSlider* pSldCylinders = SLD_GET( TEXT("setting.slider.enginecylinders") );  // jun 20201202
	INT nPos = 0;

	if(spConfig == NULL)
		CConfigText::Load(&m_stCfgDaview);
	else if(&m_stCfgDaview != spConfig)
		m_stCfgDaview = *spConfig;
	
	//display	
	m_cbTimeOffset.SetCurSel(0); 
	m_cbTimeZone.SetCurSel(m_stCfgDaview.iGmt+ 12) ;
	

	// device
	if( pSldGSensor )
	{
		nPos = m_stCfgDaview.iGsensorSensi;
		this->updateGsensorValue(nPos, pSldGSensor->GetRangeMax());
		pSldGSensor->SetPos(nPos);
	}

	if( pSldVolume)
	{
		nPos = m_stCfgDaview.iSpeakerVol;
		this->updateVolumeValue(nPos);
		pSldVolume->SetPos(nPos);
	}

	if( pSldCylinders)  // jun 20201202
	{
		nPos = m_stCfgDaview.iEngineCylinders;
		this->updateCylindersValue(nPos, pSldCylinders->GetRangeMax());
		pSldCylinders->SetPos(nPos);
	}


       m_csCarNumber.Format(TEXT("%s"), CString(m_stCfgDaview.strCarNumber)); //m_stCfgDaview.strCarNumber);  // jun 200805
       m_csDriverCode.Format(TEXT("%s"), CString(m_stCfgDaview.strDriverCode)); //m_stCfgDaview.strDriverCode);  // jun 221017
       
	m_ipreEventMode = m_stCfgDaview.iEventMode;
	m_ipreVideoQuality = m_stCfgDaview.iVideoQuality;
	m_ipreGsensorSensi = m_stCfgDaview.iGsensorSensi;
	UpdateData(FALSE);
	
	return TRUE;
}

VOID CSettingDlg::updateGsensorValue(int nPos, int nRang)
{
	CString csValue;
	double dRang = 10.0;
	if(nRang)
		dRang = (double)nRang;

 	if(nRang > 10)
		csValue.Format(TEXT("%d"), nPos);
	else
		csValue.Format(TEXT("%d"), nPos);

	SetDlgItemText(IDC_SETTING_STATIC_DEVICE_GSENSOR_SENSITIVE_VALUE, csValue); 
}

VOID CSettingDlg::updateVolumeValue(int nPos)
{
	CString csValue;
	if(nPos == 0)
		csValue.Format(TEXT("0 (OFF)"));
	else
		csValue.Format(TEXT("%d"), nPos);
	
	SetDlgItemText(IDC_SETTING_STATIC_SPEAKER_VOLUME_VALUE, csValue); 
}

VOID CSettingDlg::updateCylindersValue(int nPos, int nRang)  // jun 20201202
{
	CString csValue;
        double dRang = 10.0;
	if(nRang)
		dRang = (double)nRang;

 	if(nRang > 10)
		csValue.Format(TEXT("%d"), nPos);
	else
		csValue.Format(TEXT("%d"), nPos);
	
	SetDlgItemText(IDC_SETTING_STATIC_ENGINE_CYLINDERS_VALUE, csValue); 
}

BOOL CSettingDlg::SelectMemory(LPWSTR pszPathname, LPWSTR pszVolumeName, LPWSTR pszTitle)
{
	BOOL result = FALSE;
	ITEMIDLIST*  pidlBrowse;
	CString csTitle;
	
	
	BROWSEINFO BrInfo;
	memset(&BrInfo, 0, sizeof(BrInfo));
	BrInfo.hwndOwner=this->GetSafeHwnd();
	BrInfo.pidlRoot=NULL;
	//BrInfo.pszDisplayName=pszPathname;
	
	if(pszTitle)
		BrInfo.lpszTitle = pszTitle;
	else
	{
		this->m_pCLanguage->GetLenguageText(&csTitle , CLanguage::TEXT_SELECTSDCARD);
		BrInfo.lpszTitle = csTitle.GetBuffer();
	}
	BrInfo.ulFlags=BIF_RETURNONLYFSDIRS;
	BrInfo.lpfn = BrowseForFolder_CallbackProc;
	BrInfo.lParam = (LPARAM)pszPathname;

	this->EnableWindow( FALSE );
	//다이얼로그를 띄우기
	pidlBrowse=::SHBrowseForFolder(&BrInfo);

	if(pidlBrowse != NULL) 
		::SHGetPathFromIDList(pidlBrowse, pszPathname); //선택한 폴더를 얻어옴
	else {
		this->EnableWindow( TRUE );
		return FALSE;
	}


	TCHAR   pszLabel[MAX_PATH];
	::GetVolumeInformation(pszPathname, pszLabel, MAX_PATH, NULL, NULL, NULL, NULL, NULL);

	if(pszVolumeName)
		wcscpy(pszVolumeName, pszLabel);
	
	CString	csVolume = pszLabel;
	
	pszPathname[3] = 0;
	INT nDriveIndex = pszPathname[0] - 'A';
	DEBUGMSG(1,("drive : %s	(%d) \n", pszPathname, nDriveIndex));
		
	if(nDriveIndex > 2 && pszPathname[1] == ':' && (::GetDriveType(pszPathname) == DRIVE_REMOVABLE || ::GetDriveType(pszPathname) == DRIVE_REMOTE || csVolume.Find(FORMAT_LABEL) >= 0)) // more than c:
	{
		result = TRUE;
	}
	else if(pszPathname[1] == ':')
	{
		CString msg; 
		msg.Format(TEXT("%s is system drive."), pszPathname);
		AfxMessageBox(msg, MB_ICONERROR);
	}
	
	this->EnableWindow( TRUE );

	return result;
}

void CSettingDlg::OnBnClickedSettingQuality()
{
	UpdateData();

	if(strstr( m_stCfgDaview.strFWVersion, "NxFS" ) == NULL){
		if(m_stCfgDaview.iVideoQuality != m_ipreVideoQuality){
			CString str;
			this->m_pCLanguage->GetLenguageText(&str , CLanguage::TEXT_OLD_FILE_DELETE);
			if (IDYES == ::AfxMessageBox(str, MB_YESNO | MB_ICONINFORMATION))
			{				
				m_ipreVideoQuality = m_stCfgDaview.iVideoQuality;
				m_bChangedVideoQualit = TRUE;
			}
			else {
				m_stCfgDaview.iVideoQuality = m_ipreVideoQuality;
				UpdateData(FALSE);
			}
		}
	}
}

void CSettingDlg::OnBnClickedSettingEventCapacity()
{
	UpdateData();
	if(m_stCfgDaview.iEventMode != m_ipreEventMode){
		CString str;
		this->m_pCLanguage->GetLenguageText(&str , CLanguage::TEXT_OLD_FILE_DELETE);
		if (IDYES == ::AfxMessageBox(str, MB_YESNO | MB_ICONINFORMATION))
		{				
			m_ipreEventMode = m_stCfgDaview.iEventMode;
			m_bChangedEventMode = TRUE;
		}
		else {
			m_stCfgDaview.iEventMode = m_ipreEventMode;
			UpdateData(FALSE);
		}
	}
}

void CSettingDlg::SaveFormatTextFile(CString file_dir, LPWSTR file_name)
{
	CString fileName;
	CFile file;
	
	fileName = file_dir;
	fileName.Append(_T("\\"));
	if(file_name == NULL)
		fileName.Append(BLACKBOX_FORMAT_FILE_NAME);
	else
		fileName.Append(file_name);

	if(file.Open(fileName.GetBuffer(), CFile::modeCreate | CFile::modeReadWrite))
	{
		char *pstrMsg = "\r\n------=_NextPart_000_000C_01D1B749.AB76CBC0--\r\n\r\n\r\n";
		file.Write((void *)pstrMsg, strlen(pstrMsg));
		file.Close();
	}
}

void CSettingDlg::OnStnClickedSettingStaticSpeakerVolumeValue()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
