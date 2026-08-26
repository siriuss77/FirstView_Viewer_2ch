//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: BBPlayerUIControl.CPP
//
//	Description: BBPlayer UI controller
//
//	Update History
// 
//		2011.11.23 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. INCLUDE

#include "StdAfx.h"
#include "BBPlayer.h"
#include "BBPlayerUIControl.h"
#include "NotifyDlg.h"
#include "ConfigTextFile.h"

#include <CLUDE/version.h>
#include <CLENAW/ClImage.h>
#include <CLENAW/ClButton.h>
#include <CLENAW/ClListCtrl.h>
#include <CLENAW/ClVideoview.h>
#include <CLENAW/ClWebBrowser.h>
#include <CLENAW/GSensorView.h>

#include <WinInet.h>

//================================================================================================= E. INCLUDE
#define DEF_LISTBOX_FILELIST_MOVE_USE		0
//================================================================================================= S. CON/DCON

#if (BUILD_MODEL == MODEL_RALLY_EYES)	
#define 		GetLangText(text)	(text)
#else
#define 		GetLangText(text)	 (m_Language.GetButtonText(text))
#endif

CBBPlayerUIControl::CBBPlayerUIControl(void)
{
	this->m_pDialogManager	= NULL;
	this->m_nStreamChannel	= -1;
	this->m_nPreStreamChannel = -1;
	this->m_bIsStreamGPS		= FALSE;
	this->m_bIsStreamGSensor	= FALSE;
	this->m_bIsLogFileExist		= FALSE;
	this->m_bIsNxfsFileExist		= FALSE;
	this->m_nCurMode		= 0;

	this->m_pSldTime			= NULL;
	this->m_pTextDuration		= NULL;
	this->m_pTextCurTime		= NULL;
	this->m_eCurUIState		= UISTATE_INVALID;

	this->m_eSelectedListTab= EVENT_LIST_NORMAL;
	this->m_eMediaStatus	= EVENT_MEDIA_STOPPED;

	this->m_MaxScreenActive = ITEM_FRONT_VIDEO;
	
	this->m_ActiveFront		= ITEM_FRONT_VIDEO;
	this->m_ActiveRear		= ITEM_REAR_VIDEO;
	this->m_ActiveMap		= ITEM_MAP;

	INT nCounter;
	for( nCounter = 0; nCounter <(INT)PRSGEO_INVALID; nCounter ++ )
		this->m_geoPreserved[nCounter] = ClGeometry( 0, 0, 0, 0 );

	this->m_bTachoDisplayThreadExit = FALSE;
	this->m_pTachoDisplayThread = NULL;
}


CBBPlayerUIControl::~CBBPlayerUIControl(void)
{

}

//================================================================================================= E. CON/DCON
//================================================================================================= S. INIT/DEINIT

VOID CBBPlayerUIControl::InitBBPlayerRes( VOID )
{
	if(this->m_Language.GetLanguageCount())
		this->m_Language.LoadLenguage(BUILD_LANGUAGE);
	
	this->m_pDialogManager->AddResource( TEXT("main.button.site"),			TYPE_BUTTON,	IDC_MAIN_BTN_SITE		);
	this->m_pDialogManager->AddResource( TEXT("main.button.setting"),		TYPE_BUTTON,	IDC_MAIN_BTN_SET		);
	this->m_pDialogManager->AddResource( TEXT("main.button.capture"),		TYPE_BUTTON,	IDC_MAIN_BTN_CAPTURE	);
	this->m_pDialogManager->AddResource( TEXT("main.button.print"),		TYPE_BUTTON,	IDC_MAIN_BTN_PRINT	);
	this->m_pDialogManager->AddResource( TEXT("main.button.log"),			TYPE_BUTTON,	IDC_MAIN_BTN_LOG	);
	this->m_pDialogManager->AddResource( TEXT("main.button.about"),			TYPE_BUTTON,	IDC_MAIN_BTN_ABOUT		);

	this->m_pDialogManager->AddResource( TEXT("main.button.min"),			TYPE_BUTTON,	IDC_MAIN_BTN_MINIMIZE	);
	this->m_pDialogManager->AddResource( TEXT("main.button.max"),			TYPE_BUTTON,	IDC_MAIN_BTN_MAXIMIZE	);
	this->m_pDialogManager->AddResource( TEXT("main.button.nor"),			TYPE_BUTTON,	IDC_MAIN_BTN_NORMALIZE	);
	this->m_pDialogManager->AddResource( TEXT("main.button.exit"),			TYPE_BUTTON,	IDC_MAIN_BTN_EXIT		);

	this->m_pDialogManager->AddResource( TEXT("main.button.prev"),			TYPE_BUTTON,	IDC_MAIN_BTN_PREV		);
	this->m_pDialogManager->AddResource( TEXT("main.button.play"),			TYPE_BUTTON,	IDC_MAIN_BTN_PLAY		);
	this->m_pDialogManager->AddResource( TEXT("main.button.stop"),			TYPE_BUTTON,	IDC_MAIN_BTN_STOP		);
	this->m_pDialogManager->AddResource( TEXT("main.button.next"),			TYPE_BUTTON,	IDC_MAIN_BTN_NEXT		);
	this->m_pDialogManager->AddResource( TEXT("main.button.rew"),			TYPE_BUTTON,	IDC_MAIN_BTN_REW		);
	this->m_pDialogManager->AddResource( TEXT("main.button.ff"),			TYPE_BUTTON,	IDC_MAIN_BTN_FF			);
	this->m_pDialogManager->AddResource( TEXT("main.button.fps"),			TYPE_BUTTON,	IDC_MAIN_BTN_FPS		);

	this->m_pDialogManager->AddResource( TEXT("main.button.front"),			TYPE_BUTTON,	IDC_MAIN_BTN_FULL_F);
	this->m_pDialogManager->AddResource( TEXT("main.button.rear"),			TYPE_BUTTON,	IDC_MAIN_BTN_FULL_R	);
	this->m_pDialogManager->AddResource( TEXT("main.button.map"),			TYPE_BUTTON,	IDC_MAIN_BTN_FULL_M	);
	
	this->m_pDialogManager->AddResource( GetLangText(TEXT("main.button.open")),			TYPE_BUTTON,	IDC_MAIN_BTN_OPEN		);
	this->m_pDialogManager->AddResource( TEXT("main.button.format"),		TYPE_BUTTON,	IDC_MAIN_BTN_FORMAT		);
	this->m_pDialogManager->AddResource( TEXT("main.button.save"),			TYPE_BUTTON,	IDC_MAIN_BTN_SAVE		);
	this->m_pDialogManager->AddResource( TEXT("main.button.saveall"),			TYPE_BUTTON,	IDC_MAIN_BTN_SAVEALL		);

	this->m_pDialogManager->AddResource( m_Language.GetButtonText(TEXT("main.button.tabnormal")),		TYPE_BUTTON,	IDC_MAIN_BTN_NORMAL		);
	this->m_pDialogManager->AddResource( m_Language.GetButtonText(TEXT("main.button.tabevent")),		TYPE_BUTTON,	IDC_MAIN_BTN_EVENT		);
	this->m_pDialogManager->AddResource( m_Language.GetButtonText(TEXT("main.button.tabmanual")),		TYPE_BUTTON,	IDC_MAIN_BTN_MANUAL		);
	this->m_pDialogManager->AddResource( m_Language.GetButtonText(TEXT("main.button.tabmotion")),		TYPE_BUTTON,	IDC_MAIN_BTN_MOTION		);

	this->m_pDialogManager->AddResource( TEXT("main.slider.time"),			TYPE_SLIDER,	IDC_MAIN_SLD_TIME		);

	this->m_pDialogManager->AddResource( TEXT("main.image.speed0"),			TYPE_IMAGE,		IDC_MAIN_IMAGE_SPEED0	);
	this->m_pDialogManager->AddResource( TEXT("main.image.speed1"),			TYPE_IMAGE,		IDC_MAIN_IMAGE_SPEED1	);
	this->m_pDialogManager->AddResource( TEXT("main.slider.speed"),			TYPE_SLIDER,	IDC_MAIN_SLD_SPEED		);
	this->m_pDialogManager->AddResource( TEXT("main.text.speed"),			TYPE_TEXT,		IDC_MAIN_TEXT_SPEED	);
	
	this->m_pDialogManager->AddResource( TEXT("main.image.bright"),			TYPE_IMAGE,		IDC_MAIN_IMAGE_BRIGHT	);
	this->m_pDialogManager->AddResource( TEXT("main.slider.bright"),		TYPE_SLIDER,	IDC_MAIN_SLD_BRIGHT		);
	this->m_pDialogManager->AddResource( TEXT("main.text.bright"),			TYPE_TEXT,		IDC_MAIN_TEXT_BRIGHT		);
	
	this->m_pDialogManager->AddResource( TEXT("main.button.volume"),		TYPE_BUTTON,	IDC_MAIN_BTN_VOLUME		);
	this->m_pDialogManager->AddResource( TEXT("main.slider.volume"),		TYPE_SLIDER,	IDC_MAIN_SLD_VOLUME		);

	this->m_pDialogManager->AddResource( TEXT("main.image.file"),			TYPE_IMAGE,		IDC_MAIN_IMAGE_FILE		);
	this->m_pDialogManager->AddResource( TEXT("main.text.file"),			TYPE_TEXT,		IDC_MAIN_TEXT_FILE		);
	this->m_pDialogManager->AddResource( TEXT("main.text.curtime"),			TYPE_TEXT,		IDC_MAIN_TEXT_CURTIME	);
	this->m_pDialogManager->AddResource( TEXT("main.text.duration"),		TYPE_TEXT,		IDC_MAIN_TEXT_DURATION	);

	this->m_pDialogManager->AddResource( TEXT("main.text.latitude"),		TYPE_TEXT,		IDC_MAIN_TEXT_LATITUDE	);
	this->m_pDialogManager->AddResource( TEXT("main.text.longitude"),		TYPE_TEXT,		IDC_MAIN_TEXT_LONGITUDE );
	this->m_pDialogManager->AddResource( TEXT("main.text.altitude"),		TYPE_TEXT,		IDC_MAIN_TEXT_ALTITUDE	);
	this->m_pDialogManager->AddResource( TEXT("main.text.gps_speed"),		TYPE_TEXT,		IDC_MAIN_TEXT_GPS_SPEED);

	
	this->m_pDialogManager->AddResource( TEXT("main.button.checkfilelistall"),		TYPE_BUTTON,	IDC_MAIN_BTN_CHECKFILELISTALL		);
	this->m_pDialogManager->AddResource( TEXT("main.listbox.filelist"),		TYPE_LISTBOX,	IDC_MAIN_FILELIST		);

	this->m_pDialogManager->AddResource( TEXT("main.video.quadview"),			TYPE_VIDEO,		IDC_MAIN_VIDEO_QUADVIEW);
	this->m_pDialogManager->AddResource( TEXT("main.video.quadctrl"),			TYPE_VIDEO,		IDC_MAIN_VIDEO_QUADCTRL);
	
	this->m_pDialogManager->AddResource( TEXT("main.video.zoom"),			TYPE_VIDEO,		IDC_MAIN_VIDEO_ZOOM	);
	this->m_pDialogManager->AddResource( TEXT("main.video.front"),			TYPE_VIDEO,		IDC_MAIN_VIDEO_FRONT	);
	this->m_pDialogManager->AddResource( TEXT("main.video.rear"),			TYPE_VIDEO,		IDC_MAIN_VIDEO_REAR		);
	this->m_pDialogManager->AddResource( TEXT("main.web.map"),				TYPE_WEB,		IDC_MAIN_WEB_MAP		);
	
	this->m_pDialogManager->AddResource( GetLangText(TEXT("main.button.frchange")),		TYPE_BUTTON,	IDC_MAIN_BTN_FRCHANGE	);
	this->m_pDialogManager->AddResource( m_Language.GetButtonText(TEXT("main.button.fmchange")),		TYPE_BUTTON,	IDC_MAIN_BTN_FMCHANGE	);
	this->m_pDialogManager->AddResource( TEXT("main.button.zoom"),			TYPE_BUTTON,	IDC_MAIN_BTN_ZOOM	);

	this->m_pDialogManager->AddResource( TEXT("main.button.frontflip"),			TYPE_BUTTON,	IDC_MAIN_BTN_CAM_FF);
	this->m_pDialogManager->AddResource( TEXT("main.button.frontmirror"),		TYPE_BUTTON,	IDC_MAIN_BTN_CAM_FM);
	this->m_pDialogManager->AddResource( TEXT("main.button.rearflip"),			TYPE_BUTTON,	IDC_MAIN_BTN_CAM_RF);
	this->m_pDialogManager->AddResource( TEXT("main.button.rearmirror"),		TYPE_BUTTON,	IDC_MAIN_BTN_CAM_RM);
	
	this->m_pDialogManager->AddResource( TEXT("main.gsensor.graph"),			TYPE_GSENSOR,	IDC_MAIN_GSENSOR_VIEW 	);

	this->m_pDialogManager->AddResource( TEXT("main.digit.speed"),			TYPE_DIGIT,	IDC_DIGIT_NUM_SPEED);

	//this->m_pDialogManager->AddResource( TEXT("main.digit.rpm"),			TYPE_DIGIT,	IDC_DIGIT_NUM_RPM);
	this->m_pDialogManager->AddResource( TEXT("main.analogmeter.rpm"),			TYPE_AMETER,	IDC_DIGIT_NUM_RPM);


	this->m_pDialogManager->AddResource( TEXT("main.button.tacho1"),			TYPE_BUTTON,	IDC_MAIN_BTN_TACHO1		);
	this->m_pDialogManager->AddResource( TEXT("main.button.tacho2"),			TYPE_BUTTON,	IDC_MAIN_BTN_TACHO2		);
	this->m_pDialogManager->AddResource( TEXT("main.button.tacho3"),			TYPE_BUTTON,	IDC_MAIN_BTN_TACHO3		);
	this->m_pDialogManager->AddResource( TEXT("main.button.tacho4"),			TYPE_BUTTON,	IDC_MAIN_BTN_TACHO4		);
	this->m_pDialogManager->AddResource( TEXT("main.button.tacho5"),			TYPE_BUTTON,	IDC_MAIN_BTN_TACHO5		);

	this->m_pDialogManager->AddResource( TEXT("main.button.gpspulse"),		TYPE_BUTTON,	IDC_MAIN_BTN_GPS_PULSE		);
	
#if (BUILD_MODEL == MODEL_RALLY_EYES)
	this->m_pDialogManager->AddResource( TEXT("main.digit.latitude"),			TYPE_DIGIT,	IDC_DIGIT_NUM_LATITUDE);
	this->m_pDialogManager->AddResource( TEXT("main.digit.longitude"),			TYPE_DIGIT,	IDC_DIGIT_NUM_LONGITUDE);

	this->m_pDialogManager->AddResource( TEXT("main.web.sponsor"),			TYPE_WEB,	IDC_MAIN_WEB_SPONSOR);
#endif

	return;
}

VOID CBBPlayerUIControl::ButtonTooltipSet( const WCHAR* pButtonName, INT Index) //
{
	CString strText;
	HashedString hash(pButtonName);
	ClButton* pBtn 		= BTN_GET( hash);
	if(pBtn)
	{
		if(this->m_Language.GetLenguageText(&strText , Index))
			pBtn->SetTooltipText( strText.GetBuffer());
	}
}

VOID CBBPlayerUIControl::SliderTooltipSet( const WCHAR* pSliderName, INT Index) //
{
	CString strText;
	HashedString hash(pSliderName);
	ClSlider* pSld 		= SLD_GET( hash);
	if(pSld)
	{
		if(this->m_Language.GetLenguageText(&strText , Index))
			pSld->SetTooltipText( strText.GetBuffer());
	}
}

VOID CBBPlayerUIControl::ImageTooltipSet( const WCHAR* pImageName, INT Index) //
{
	CString strText;
	HashedString hash(pImageName);
	ClImage* pImg 		= IMG_GET( hash);
	if(pImg)
	{
		if(this->m_Language.GetLenguageText(&strText , Index))
			pImg->SetTooltipText( strText.GetBuffer());
	}
}

VOID CBBPlayerUIControl::InitTooltip(VOID)
{
	CString strText = ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getCompanySite();
	ClButton* pBtnSite = BTN_GET( TEXT("main.button.site"));
	pBtnSite->SetTooltipText( strText.GetBuffer());

	this->SliderTooltipSet(TEXT("main.slider.volume"), CLanguage::TIP_PROGVOLUME);
	this->SliderTooltipSet(TEXT("main.slider.speed"), CLanguage::TIP_PROGPLAYSPEED);
	this->SliderTooltipSet(TEXT("main.slider.bright"), CLanguage::TIP_BRIGHTNESS);

	this->ButtonTooltipSet(TEXT("main.button.volume"), CLanguage::TIP_PROGVOLUME);
	this->ImageTooltipSet(TEXT("main.image.speed0"), CLanguage::TIP_PROGPLAYSPEED);
	this->ImageTooltipSet(TEXT("main.image.bright"), CLanguage::TIP_BRIGHTNESS);
	
	this->SliderTooltipSet(TEXT("main.slider.time"), CLanguage::TIP_PROGPLAYTIME);
	
	this->ButtonTooltipSet( TEXT("main.button.min"), CLanguage::TIP_BTN_MINIMIZE);
	this->ButtonTooltipSet( TEXT("main.button.max"), CLanguage::TIP_BTN_MAXIMIZE);
	this->ButtonTooltipSet( TEXT("main.button.exit"), CLanguage::TIP_BTN_CLOSE);

	this->ButtonTooltipSet( GetLangText(TEXT("main.button.frchange")), CLanguage::TIP_BTN_FRCHANGE);
	this->ButtonTooltipSet( m_Language.GetButtonText(TEXT("main.button.fmchange")), CLanguage::TIP_BTN_FMCHANGE);
	this->ButtonTooltipSet( TEXT("main.button.zoom"), CLanguage::TIP_BTN_ZOOM);

	this->ButtonTooltipSet( TEXT("main.button.frontflip"), CLanguage::TEXT_FLIP);
	this->ButtonTooltipSet( TEXT("main.button.frontmirror"), CLanguage::TEXT_MIRROR);
	this->ButtonTooltipSet( TEXT("main.button.rearflip"), CLanguage::TEXT_FLIP);
	this->ButtonTooltipSet( TEXT("main.button.rearmirror"), CLanguage::TEXT_MIRROR);
	
	this->ButtonTooltipSet( TEXT("main.button.prev"), CLanguage::TIP_BTN_PREV);
	this->ButtonTooltipSet( TEXT("main.button.play"), CLanguage::TIP_BTN_PLAY);
	this->ButtonTooltipSet( TEXT("main.button.stop"), CLanguage::TIP_BTN_STOP);
	this->ButtonTooltipSet( TEXT("main.button.next"), CLanguage::TIP_BTN_NEXT);
	this->ButtonTooltipSet( TEXT("main.button.rew"), CLanguage::TIP_BTN_SEEK_N);
	this->ButtonTooltipSet( TEXT("main.button.ff"), CLanguage::TIP_BTN_SEEK_P);
	this->ButtonTooltipSet( TEXT("main.button.fps"), CLanguage::TIP_BTN_SEEK);

	this->ButtonTooltipSet( m_Language.GetButtonText(TEXT("main.button.tabnormal")), CLanguage::TIP_BTN_ALLFILE);
	this->ButtonTooltipSet( m_Language.GetButtonText(TEXT("main.button.tabmotion")), CLanguage::TIP_BTN_MOTION);
	this->ButtonTooltipSet( m_Language.GetButtonText(TEXT("main.button.tabevent")), CLanguage::TIP_BTN_GSENSOR);
	this->ButtonTooltipSet( m_Language.GetButtonText(TEXT("main.button.tabmanual")), CLanguage::TIP_BTN_EMERGENC);

	this->ButtonTooltipSet( TEXT("main.button.front"), CLanguage::TIP_BTN_FRONT);
	this->ButtonTooltipSet( TEXT("main.button.rear"), CLanguage::TIP_BTN_REAR);
	this->ButtonTooltipSet( TEXT("main.button.map"), CLanguage::TIP_BTN_MAP);
	
	this->ButtonTooltipSet( GetLangText(TEXT("main.button.open")), CLanguage::TIP_BTN_FILEOPEN);
	this->ButtonTooltipSet( TEXT("main.button.format"), CLanguage::TIP_BTN_FORMAT);
	this->ButtonTooltipSet( TEXT("main.button.setting"), CLanguage::TIP_BTN_SETUP);
	this->ButtonTooltipSet( TEXT("main.button.capture"), CLanguage::TIP_BTN_CAPTURE);
	this->ButtonTooltipSet( TEXT("main.button.print"), CLanguage::TIP_BTN_PRINT);
	this->ButtonTooltipSet( TEXT("main.button.save"), CLanguage::TIP_BTN_SAVE);
	this->ButtonTooltipSet( TEXT("main.button.log"), CLanguage::TIP_BTN_LOG);
	this->ButtonTooltipSet( TEXT("main.button.saveall"), CLanguage::TIP_BTN_SAVEALL);
	this->ButtonTooltipSet( TEXT("main.button.about"), CLanguage::TIP_BTN_INFO);

	this->ButtonTooltipSet( TEXT("main.button.checkfilelistall"), CLanguage::TEXT_SELECTALL);

	this->ButtonTooltipSet( TEXT("main.button.gpspulse"), CLanguage::TIP_BTN_GPSPULSE);
}


VOID CBBPlayerUIControl::InitLanguage(VOID)
{
	m_pSldTime		= SLD_GET( TEXT("main.slider.time")	);
	m_pTextDuration	= TXT_GET( TEXT("main.text.duration"));
	m_pTextCurTime	= TXT_GET( TEXT("main.text.curtime"));




	
}

VOID CBBPlayerUIControl::InitExchangeGeometry( VOID )
{
	ClDialogManager::PCONTROL_INFO pInfo;
	pInfo = this->m_pDialogManager->GetResource( TEXT("main.video.front") );
	if( pInfo && pInfo->m_pContainer )
		this->m_geoPreserved[PRSGEO_VIDEO_FRONT] = pInfo->m_pContainer->GetGeometry();

#ifdef REAR_CAM_OFF  // jun 170406
//	pInfo = this->m_pDialogManager->GetResource( TEXT("main.video.rear") );
//	if( pInfo && pInfo->m_pContainer )
//		this->m_geoPreserved[PRSGEO_VIDEO_REAR] = pInfo->m_pContainer->GetGeometry();
#else
	pInfo = this->m_pDialogManager->GetResource( TEXT("main.video.rear") );
	if( pInfo && pInfo->m_pContainer )
		this->m_geoPreserved[PRSGEO_VIDEO_REAR] = pInfo->m_pContainer->GetGeometry();
#endif

	pInfo = this->m_pDialogManager->GetResource( TEXT("main.web.map") );
	if( pInfo && pInfo->m_pContainer )
		this->m_geoPreserved[PRSGEO_WEB_MAP] = pInfo->m_pContainer->GetGeometry();
	
	pInfo = this->m_pDialogManager->GetResource( TEXT("main.listbox.filelist") );
	if( pInfo && pInfo->m_pContainer )
		this->m_geoPreserved[PRSGEO_LISTBOX_FILELIST] = pInfo->m_pContainer->GetGeometry();

	pInfo = this->m_pDialogManager->GetResource( TEXT("main.gsensor.graph") );
	if( pInfo && pInfo->m_pContainer )
		this->m_geoPreserved[PRSGEO_GSENSOR_GRAPH] = pInfo->m_pContainer->GetGeometry();

	pInfo = this->m_pDialogManager->GetResource( TEXT("main.web.sponsor") );
	if( pInfo && pInfo->m_pContainer )
		this->m_geoPreserved[PRSGEO_WEB_SPONSOR] = pInfo->m_pContainer->GetGeometry();
	return;
}

//================================================================================================= E. INIT/DEINIT
//================================================================================================= S. EVENT

VOID CBBPlayerUIControl::SendUIEvent( CBBPlayerUIControl::UI_EVENT event )
{
	switch( event )
	{
	case EVENT_INIT			:	this->OnEventInit();			break;
	case EVENT_MODE_CHANGE	:	this->OnEventModeChange();		break;
	case EVENT_SEL_DRIVE	:	this->OnEventSelectDrive();		break;
	case EVENT_DROP_DRIVE	:	this->OnEventDropDrive();		break;

	case EVENT_MEDIA_PLAY	:	this->OnEventMediaPlay();		break;
	case EVENT_MEDIA_REPLAY	:	this->OnEventMediaReplay();		break;
	case EVENT_MEDIA_PAUSED	:	this->OnEventMediaPause();		break;
	case EVENT_MEDIA_STOPPED:	this->OnEventMediaStop();		break;
	case EVENT_MEDIA_PROGRESS:	this->OnEventMediaProgress();	break;

	case EVENT_FRCHANGE	:	this->OnEventFRChange();		break;
	case EVENT_FMCHANGE	:	this->OnEventFMChange();		break;
	case EVENT_ZOOM		:	this->OnEventZoom();			break;
	case EVENT_MUX		:	this->OnEventMux();			break;
	
	case EVENT_LIST_NORMAL	:	this->OnEventListSort(0); this->m_eSelectedListTab = EVENT_LIST_NORMAL;	break;
	case EVENT_LIST_EVENT	:	this->OnEventListSort(1); this->m_eSelectedListTab = EVENT_LIST_EVENT;	      break;
	case EVENT_LIST_MANUAL	:	this->OnEventListSort(2); this->m_eSelectedListTab = EVENT_LIST_MANUAL;	break;
	case EVENT_LIST_MOTION	:	this->OnEventListSort(3); this->m_eSelectedListTab = EVENT_LIST_MOTION;	break;

	case EVENT_SEL_MODEL	:	this->OnEvnetSelectModel();		break;
	}

	return;
}


VOID CBBPlayerUIControl::OnEventInit( VOID )
{
	DWORD dwConnectionTypes;
	
	this->m_nCurMode = 0;

	ClWebBrowser* pMap = MAP_GET( TEXT("main.web.map") );
	if( pMap )
	{
		pMap->SetInternetConnected( FALSE );
		if( InternetGetConnectedState(&dwConnectionTypes, 0) )
		{
			CString strPath = ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getMapPath();
			pMap->Navigate(strPath, 0,0,0,0);
			pMap->SetInternetConnected();
		}
	}

#ifdef BUILD_MODEL == MODEL_RALLY_EYES
	ClWebBrowser* pSponsor = MAP_GET( TEXT("main.web.sponsor") );

	if( pSponsor && InternetGetConnectedState(&dwConnectionTypes, 0) )
	{
		CString strPath = ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getSponsorLinkPath();
		pSponsor->Navigate(strPath, 0,0,0,0);
		pSponsor->SetInternetConnected(TRUE );
#if 0
		 ICustomDoc *pCustom = NULL;
		 HRESULT hr = NULL;
		 if(IDispatch* pDocDispatch = pSponsor->get_Document()) 
		 { 
	        hr = pDocDispatch->QueryInterface(IID_ICustomDoc, (void**)&pCustom);
	     } 
		 IDocHostUIHandler *pMyDocHostUIHandler = NULL;
		 if( SUCCEEDED(hr))
		 {
			 DOCHOSTUIINFO Info;
			 DOCHOSTUIINFO *pInfo = &Info;
			 memset(pInfo, NULL, sizeof(DOCHOSTUIINFO));
			 pInfo->cbSize = sizeof(DOCHOSTUIINFO);
			 pInfo ->dwFlags = DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_SCROLL_NO | DOCHOSTUIFLAG_NO3DOUTERBORDER;
		    
			 pMyDocHostUIHandler->GetHostInfo(pInfo);

			 pCustom->SetUIHandler( pMyDocHostUIHandler );
			 pCustom->Release();
		     
		 }
#endif
	}
#endif

	ClSlider* pSldSpeed		= SLD_GET( TEXT("main.slider.speed")	);
	ClSlider* pSldBright		= SLD_GET( TEXT("main.slider.bright")	);
	// set slider control
	if( pSldSpeed )	
	{
		ClText* pTextSpeed = TXT_GET( TEXT("main.text.speed") );
		pSldSpeed->SetPos(5);
		
		if( pTextSpeed )
			pTextSpeed->SetWindowText( TEXT("1.0x") );
	}

	if(pSldBright)
	{
		ClText* pTextBright = TXT_GET( TEXT("main.text.bright") );
		pSldBright->SetPos(0);
		
		if( pTextBright )
			pTextBright->SetWindowText( TEXT("0") );
	}
	
	this->SetUIState( UISTATE_INIT );

	return;
}


VOID CBBPlayerUIControl::OnEventModeChange( VOID )
{
	if( this->m_nCurMode == 0 )	this->m_nCurMode = 1;
	else						this->m_nCurMode = 0;

	this->SetUIState( this->m_eCurUIState );

	BOOL bIsInternetConnected = FALSE;
	DWORD dwConnectionTypes;
	if( InternetGetConnectedState(&dwConnectionTypes, 0) )
		bIsInternetConnected = TRUE;

	
	this->SetActiveItem();

	return;
}


VOID CBBPlayerUIControl::OnEventSelectDrive( VOID )
{
	if( this->m_eCurUIState != UISTATE_INIT )
		return;
	
	this->SetUIState( UISTATE_SELDRIVE );

#if 0
	ClButton* pBtnTabNormal = BTN_GET( TEXT("main.button.tabnormal") );
	if( pBtnTabNormal )
	{
		pBtnTabNormal->SetCheckState( TRUE );
		this->m_eSelectedListTab = EVENT_LIST_NORMAL;
	}
#endif
	return;
}


VOID CBBPlayerUIControl::OnEventDropDrive( VOID )
{
	this->m_nStreamChannel	= -1;
	this->m_bIsStreamGPS	= FALSE;
	this->m_bIsStreamGSensor= FALSE;

	this->m_eCurUIState		= UISTATE_INVALID;

	this->m_eSelectedListTab= EVENT_LIST_NORMAL;
	this->m_eMediaStatus	= EVENT_MEDIA_STOPPED;

	this->SetUIState( UISTATE_INIT );

	ClListCtrl* pFileList	= LST_GET( TEXT("main.listbox.filelist"));
	ClText* pTextFile		= TXT_GET( TEXT("main.text.file")		);
	
//	ClSlider* pSldVolume	= SLD_GET( TEXT("main.slider.volume")	);
	ClSlider* pSldSpeed		= SLD_GET( TEXT("main.slider.speed")	);
	ClSlider* pSldBright		= SLD_GET( TEXT("main.slider.bright")	);
	ClButton* pBtnPlay		= BTN_GET( TEXT("main.button.play")		);


	if( pFileList ) pFileList->DeleteAllItems();

	// Text
	if( pTextFile )	pTextFile->SetWindowText( TEXT("") );
	if( m_pTextCurTime )
	{
		m_pTextCurTime->SetWindowText( TEXT("00:00:00") );

		m_pTextCurTime->Invalidate();
	}

	if( m_pTextDuration )
	{
		m_pTextDuration->SetWindowText( TEXT("00:00:00") );
		m_pTextDuration->Invalidate();
	}

	// set slider control
	if( m_pSldTime )	m_pSldTime->SetPos(0);
//	if( pSldVolume )pSldVolume->SetPos(50);
	if( pSldSpeed )	
	{
		ClText* pTextSpeed = TXT_GET( TEXT("main.text.speed") );
		pSldSpeed->SetPos(5);
		
		if( pTextSpeed )
			pTextSpeed->SetWindowText( TEXT("1.0x") );
	}

	if(pSldBright)
	{
		ClText* pTextBright = TXT_GET( TEXT("main.text.bright") );
		pSldBright->SetPos(0);
		
		if( pTextBright )
			pTextBright->SetWindowText( TEXT("0") );
	}
	if( pBtnPlay )	
	{
		pBtnPlay->SetCheckState( FALSE );
		this->ButtonTooltipSet( TEXT("main.button.play"), CLanguage::TIP_BTN_PLAY);
	}

	return;
}


VOID CBBPlayerUIControl::OnEventMediaPlay( VOID )
{	
	ClButton*	pBtnPlay		= BTN_GET( TEXT("main.button.play") );
	ClVideoView*		pVideoRear= VID_GET( TEXT("main.video.rear"));
	ClVideoView*		pVideoCtrl= VID_GET( TEXT("main.video.quadctrl"));
	
	LONGLONG durationTime = this->m_pMultiPlayer->GetDuration();
	
	
	if( m_pSldTime )
		m_pSldTime->SetRange(0, (int)durationTime);

	if( m_pTextDuration )
	{
		CString str;
		durationTime /= 1000;
		int nHour = int(durationTime / 3600);
		int nMinute = int ((durationTime % 3600) / 60);
		int nSecond = int ((durationTime % 3600) % 60);

		str.Format( TEXT("%02d:%02d:%02d"), nHour, nMinute, nSecond );
		m_pTextDuration->SetWindowText(str);
		m_pTextDuration->Invalidate();
	}

	if( pBtnPlay )	
	{
		pBtnPlay->SetCheckState( TRUE );
		this->ButtonTooltipSet( TEXT("main.button.play"), CLanguage::TIP_BTN_PAUSE);
	}
	
	if(this->m_eMediaStatus != EVENT_MEDIA_PLAY){
		this->m_eMediaStatus = EVENT_MEDIA_PLAY;
		this->SetUIState( UISTATE_MEDIA );
	
	
#if 1
		if(pVideoCtrl && pVideoCtrl->GetQuadViewMode() && this->m_nStreamChannel > 1){
			this->OnEventMux();
		}
		else {
			if(pVideoCtrl && pVideoCtrl->GetQuadViewMode()){
				pVideoCtrl->SetQuadViewMode(FALSE);
			}
				
			if(this->m_ActiveMap == ITEM_FRONT_VIDEO){
				this->m_ActiveFront = ITEM_FRONT_VIDEO;
				this->m_ActiveRear = ITEM_REAR_VIDEO;
				this->m_ActiveMap = ITEM_MAP;
			}

			if(this->m_nCurMode == 1 && this->m_MaxScreenActive == ITEM_REAR_VIDEO)
			{
				this->m_ActiveFront = this->m_MaxScreenActive = ITEM_REAR_VIDEO;
			}
			else
			if(this->m_nStreamChannel == 1 && (this->m_ActiveRear == ITEM_FRONT_VIDEO || this->m_ActiveRear == ITEM_REAR_VIDEO))
			{
				this->m_ActiveFront = ITEM_FRONT_VIDEO;
				this->m_ActiveRear = ITEM_NONE;
				this->m_ActiveMap = ITEM_MAP;
			}
			else if( this->m_nStreamChannel > 1 &&   (this->m_ActiveRear == ITEM_MAP || this->m_ActiveRear == ITEM_NONE))
			{
				this->m_ActiveFront = ITEM_FRONT_VIDEO;
				this->m_ActiveRear = ITEM_REAR_VIDEO;
				this->m_ActiveMap = ITEM_MAP;
			}
			this->SetActiveItem(TRUE);
		}
#endif
	}	
	else {
		ClButton*	pBtnFRChange	= BTN_GET( GetLangText(TEXT("main.button.frchange")) );
		if(m_pMultiPlayer && m_nPreStreamChannel != m_pMultiPlayer->GetStreamCount()){
			m_nPreStreamChannel = m_pMultiPlayer->GetStreamCount();
			OnEventFRChange();
		}
					
  		if(m_pMultiPlayer && m_pMultiPlayer->GetStreamCount() > 1) {
#if (BUILD_MODEL != MODEL_DAVIEW_2CH_F)                // jun 170406		
			if(pBtnFRChange) pBtnFRChange->EnableWindow( TRUE );
#endif	
		}
		else {
			if(pVideoRear) pVideoRear->ShowWindow( SW_HIDE );
			
			if(pBtnFRChange) pBtnFRChange->EnableWindow( FALSE );
			if(pBtnFRChange && pBtnFRChange->GetCheckState()){
				pBtnFRChange->SetCheckState(FALSE);
				OnEventFRChange();
			}
			if(pVideoCtrl && pVideoCtrl->GetQuadViewMode()){
				pVideoCtrl->SetQuadViewMode(FALSE);
				this->OnEventMux();
			}
		}
	}


	this->m_bTachoDisplayThreadExit = FALSE;
	m_bTachoDisplayEnable = FALSE;
	m_pTachoDisplayThread = AfxBeginThread( this->ThreadTachoDisplayProc, this );

	CDigitNum* pDigit		= DIGIT_GET(TEXT("main.digit.speed"));
	CAnalogMeter *pAMeter= AMETER_GET( TEXT("main.analogmeter.rpm") );
	if(pAMeter) pAMeter->UpdateNeedle(0.0);
	if(pDigit) pDigit->DrawDigits(0);
	
	return;
}


VOID CBBPlayerUIControl::OnEventMediaReplay( VOID )
{
	ClButton* 		pBtnPlay 		= BTN_GET( TEXT("main.button.play") );
	ClButton*		pBtnZoom	 	= BTN_GET( TEXT("main.button.zoom") );
	
	if( pBtnPlay ) 
	{
		pBtnPlay->SetCheckState( TRUE );
		this->ButtonTooltipSet( TEXT("main.button.play"), CLanguage::TIP_BTN_PAUSE);
	}

	if(pBtnZoom)
	{
		pBtnZoom->SetCheckState(FALSE);
		this->OnEventZoom();
	}
	
	return;
}


VOID CBBPlayerUIControl::OnEventMediaPause( VOID )
{
	ClButton* pBtnPlay = BTN_GET( TEXT("main.button.play") );
	//this->m_eMediaStatus == EVENT_MEDIA_PAUSED; 
	
	if( pBtnPlay ) 
	{
		pBtnPlay->SetCheckState( FALSE );
		this->ButtonTooltipSet( TEXT("main.button.play"), CLanguage::TIP_BTN_PLAY);
	}
	return;
}


VOID CBBPlayerUIControl::OnEventMediaStop( VOID )
{
	this->m_eMediaStatus = EVENT_MEDIA_STOPPED;
	this->SetUIState( UISTATE_MEDIA );

	ClText*		pTextFile		= TXT_GET( TEXT("main.text.file")	);
	ClButton*	pBtnPlay		= BTN_GET( TEXT("main.button.play")	);
	ClButton*	pBtnFRChange	= BTN_GET( GetLangText(TEXT("main.button.frchange")) );
	ClButton*	pBtnFMChange	= BTN_GET( m_Language.GetButtonText(TEXT("main.button.fmchange")) );

	ClButton*	pBtnFront		= BTN_GET( TEXT("main.button.front")	);
	ClButton*	pBtnRear		= BTN_GET( TEXT("main.button.rear")	);
	ClButton*	pBtnMap		= BTN_GET( TEXT("main.button.map")	);

	ClButton* 		pBtntacho1 	= BTN_GET( TEXT("main.button.tacho1") );
	ClButton* 		pBtntacho2 	= BTN_GET( TEXT("main.button.tacho2") );
	ClButton* 		pBtntacho3 	= BTN_GET( TEXT("main.button.tacho3") );
	ClButton* 		pBtntacho4 	= BTN_GET( TEXT("main.button.tacho4") );
	ClButton* 		pBtntacho5 	= BTN_GET( TEXT("main.button.tacho5") );
	ClButton* 		pBtngpspulse 	= BTN_GET( TEXT("main.button.gpspulse") );

	if(pBtntacho1) pBtntacho1->SetCheckState(FALSE);
	if(pBtntacho2) pBtntacho2->SetCheckState(FALSE);
	if(pBtntacho3) pBtntacho3->SetCheckState(FALSE);
	if(pBtntacho4) pBtntacho4->SetCheckState(FALSE);
	if(pBtntacho5) pBtntacho5->SetCheckState(FALSE);
	//if(pBtngpspulse) pBtngpspulse->SetCheckState(FALSE);
	
	for( int i = 0; i < 5; i++) {
		ClButton*	pBtnTacho = NULL;
		CString strCtrName;
		strCtrName.Format(TEXT("main.button.tacho%d"), i+1);

		pBtnTacho = BTN_GET( strCtrName.GetBuffer()	);
		if(pBtnTacho)
			pBtnTacho->SetEnable(FALSE);
	}
		
	if( m_pSldTime )	m_pSldTime->SetPos(0);

	if( m_pTextCurTime )
	{
		m_pTextCurTime->SetWindowText( TEXT("00:00:00") );

		m_pTextCurTime->Invalidate();
	}

	if( m_pTextDuration )
	{
		m_pTextDuration->SetWindowText( TEXT("00:00:00") );
		m_pTextDuration->Invalidate();
	}

	if( pTextFile )		pTextFile->SetWindowText( TEXT("") );
	if( pBtnPlay )		
	{
		pBtnPlay->SetCheckState	( FALSE );
		this->ButtonTooltipSet( TEXT("main.button.play"), CLanguage::TIP_BTN_PLAY);
	}
	
	if( pBtnFRChange ) pBtnFRChange->EnableWindow( FALSE );
	if( pBtnFMChange ) pBtnFMChange->EnableWindow( FALSE );
	
	if( pBtnFront ) pBtnFront->EnableWindow( FALSE );
	if( pBtnRear ) pBtnRear->EnableWindow( FALSE );
	if( pBtnMap ) pBtnMap->EnableWindow( FALSE );

	CGSensorView* m_pGSensorView = GSG_GET( TEXT("main.gsensor.graph") );
	if(m_pGSensorView)	
		m_pGSensorView->ErasePoint();

	ClWebBrowser*	pMap			= MAP_GET( TEXT("main.web.map") );
	if(pMap)
		pMap->DrawText();

	this->m_bTachoDisplayThreadExit = TRUE;
	::Sleep(100);
	if(m_pTachoDisplayThread)
	{
		delete m_pTachoDisplayThread;
		m_pTachoDisplayThread = NULL;
		::Sleep(1);
	}

	CDigitNum* pDigit		= DIGIT_GET(TEXT("main.digit.speed"));
	CAnalogMeter *pAMeter= AMETER_GET( TEXT("main.analogmeter.rpm") );
	if(pAMeter) pAMeter->UpdateNeedle(0.0);
	if(pDigit) pDigit->DrawDigits(0);	
	return;
}

VOID CBBPlayerUIControl::OnEventMediaProgress( VOID )
{
	static LONGLONG nPreCurrentTime;
	static LONGLONG nPreDurationTime;
	
	int nPlayState = this->m_pMultiPlayer->GetState();
	if( nPlayState == PLAY_STATE_CLOSED )
		return;

	LONGLONG nCurrentTime = this->m_pMultiPlayer->GetCurrentPosition();
	LONGLONG nDurationTime = this->m_pMultiPlayer->GetDuration();

	UserDataSynchronize((DWORD)nCurrentTime);
	
	if( m_pSldTime ) m_pSldTime->SetPos((int)nCurrentTime);

	CString str, strDurationTime, strCurrentTime;
	int nHour, nMinute, nSecond;

	nCurrentTime /= 1000;
	nDurationTime /= 1000;
	
	if(nPreCurrentTime != nCurrentTime)
	{
		if( m_pTextCurTime )
		{
			int nHour	= int(nCurrentTime / 3600);
			int nMinute = (nCurrentTime % 3600) / 60;
			int nSecond = (nCurrentTime % 3600) % 60;

			str.Format( TEXT("%02d:%02d:%02d"), nHour, nMinute, nSecond );

			m_pTextCurTime->GetWindowText(strCurrentTime);
			if( strCurrentTime != str ) m_pTextCurTime->SetWindowText(str);

			m_pTextCurTime->Invalidate();
		}
	}

	// Update duration Time	
	if(nPreDurationTime != nDurationTime)
	{
		if( m_pTextDuration )
		{
			nHour	= int(nDurationTime / 3600);
			nMinute = int ((nDurationTime % 3600) / 60);
			nSecond = int ((nDurationTime % 3600) % 60);

			str.Format( TEXT("%02d:%02d:%02d"), nHour, nMinute, nSecond );
			m_pTextDuration->GetWindowText(strDurationTime);
			if (strDurationTime != str) m_pTextDuration->SetWindowText(str);

			m_pTextDuration->Invalidate();
		}
	}

	return;
}


VOID CBBPlayerUIControl::OnEventFRChange( VOID )
{
#if (BUILD_MODEL == MODEL_RALLY_EYES)
	if(this->m_ActiveFront == ITEM_FRONT_VIDEO)
	{
		if( this->m_nStreamChannel == 1)
		{
			this->m_ActiveFront = ITEM_MAP;
			this->m_ActiveRear = ITEM_FRONT_VIDEO;
			this->m_ActiveMap = ITEM_REAR_VIDEO;
		}
		else
		{
			this->m_ActiveFront = ITEM_REAR_VIDEO;
			this->m_ActiveRear = ITEM_FRONT_VIDEO;
			this->m_ActiveMap = ITEM_MAP;
		}
	}
	else if(this->m_ActiveFront == ITEM_REAR_VIDEO)
	{
		this->m_ActiveFront = ITEM_MAP;
		this->m_ActiveRear = ITEM_FRONT_VIDEO;
		this->m_ActiveMap = ITEM_REAR_VIDEO;
	}
	else
	{
		this->m_ActiveFront = ITEM_FRONT_VIDEO;
		this->m_ActiveRear = ITEM_REAR_VIDEO;
		this->m_ActiveMap = ITEM_MAP;
	}
#else
	ClButton* pBtnFMChange = BTN_GET( m_Language.GetButtonText(TEXT("main.button.fmchange")) );
	ClButton* pBtnFRChange = BTN_GET( GetLangText(TEXT("main.button.frchange")) );
	ClVideoView*		pVideoCtrl= VID_GET( TEXT("main.video.quadctrl"));

	if(pVideoCtrl && pVideoCtrl->GetQuadViewMode()) {
		pVideoCtrl->SetQuadViewMode(FALSE);
	}
		
	if(pBtnFMChange && pBtnFMChange->GetCheckState())
		pBtnFMChange->SetCheckState(FALSE);
	
	if( pBtnFRChange->GetCheckState() )
	{	
		this->m_ActiveFront = ITEM_REAR_VIDEO;
		this->m_ActiveRear = ITEM_FRONT_VIDEO;
		this->m_ActiveMap = ITEM_MAP;
	}
	else								
	{
		this->m_ActiveFront = ITEM_FRONT_VIDEO;
		this->m_ActiveRear = ITEM_REAR_VIDEO;
		this->m_ActiveMap = ITEM_MAP;
	}
#endif
	this->SetActiveItem();
	return;
}

VOID CBBPlayerUIControl::OnEventFMChange( VOID )
{
	ClButton* pBtnFMChange = BTN_GET( m_Language.GetButtonText(TEXT("main.button.fmchange") ));
	ClButton* pBtnFRChange = BTN_GET( GetLangText(TEXT("main.button.frchange") ));
	ClVideoView*		pVideoCtrl= VID_GET( TEXT("main.video.quadctrl"));

	if(pVideoCtrl && pVideoCtrl->GetQuadViewMode()) pVideoCtrl->SetQuadViewMode(FALSE);
	
	if(pBtnFRChange->GetCheckState())
		pBtnFRChange->SetCheckState(FALSE);
	
	if( pBtnFMChange->GetCheckState() )
	{	
		this->m_ActiveFront = ITEM_MAP;
		this->m_ActiveRear = ITEM_FRONT_VIDEO;
		
		if( this->m_nStreamChannel == 1 ) 
			this->m_ActiveMap = ITEM_NONE;
		else
			this->m_ActiveMap = ITEM_REAR_VIDEO;
	}
	else								
	{
		this->m_ActiveFront = ITEM_FRONT_VIDEO;
		if( this->m_nStreamChannel == 1 ) 
		{
			this->m_ActiveRear = ITEM_NONE;
			this->m_ActiveMap = ITEM_MAP;
		}
		else
		{
			this->m_ActiveRear = ITEM_REAR_VIDEO;
			this->m_ActiveMap = ITEM_MAP;
		}
	}
	this->SetActiveItem();
	return;
}

VOID CBBPlayerUIControl::OnEventZoom( VOID )
{
	ClButton* 		pBtnZoom 	= BTN_GET( TEXT("main.button.zoom") );
	ClVideoView*		pVideoZoom = VID_GET( TEXT("main.video.zoom") );
	ClVideoView*		pVideoCtrl= VID_GET( TEXT("main.video.quadctrl"));
	ClVideoView*		pVideo = NULL;
	
	if(this->m_ActiveFront == ITEM_FRONT_VIDEO)
		pVideo = VID_GET( TEXT("main.video.front") );
	else if(this->m_ActiveFront == ITEM_REAR_VIDEO)
		pVideo = VID_GET( TEXT("main.video.rear") );
	else if( this->m_ActiveFront == ITEM_MAP)	
	{
		if(pBtnZoom)	pBtnZoom->SetCheckState(FALSE);
	}
	
	if(pBtnZoom && pVideoZoom && pVideo)
	{
		if(pBtnZoom->GetCheckState())
		{
			pVideoZoom->EnableWindow(TRUE);
			pVideoZoom->ShowWindow( SW_SHOW );
			pVideo->ShowWindow( SW_HIDE );
		}
		else
		{
			pVideoZoom->EnableWindow(FALSE);
			pVideoZoom->ShowWindow( SW_HIDE );
			pVideo->ShowWindow( SW_SHOW );
		}
	}
	return;
}

VOID CBBPlayerUIControl::OnEventMux( VOID )
{
	ClButton*		pBtnZoom = BTN_GET( TEXT("main.button.zoom") );
	
	ClVideoView*		pVideoQuidView = VID_GET( TEXT("main.video.quadview") );
	ClVideoView*		pVideoQuadCtrl = VID_GET( TEXT("main.video.quadctrl") );

	ClButton* pBtnFMChange = BTN_GET( m_Language.GetButtonText(TEXT("main.button.fmchange")) );
	ClButton* pBtnFRChange = BTN_GET( GetLangText(TEXT("main.button.frchange")) );
	

	ClVideoView*		pVideo = NULL;
	int stream_count = 1;

#if 1
	if(m_pMultiPlayer)
		stream_count = m_pMultiPlayer->GetStreamCount();
		
	if(pVideoQuadCtrl)
	{
		if(stream_count > 1 && pVideoQuadCtrl->GetQuadViewMode()){
			if(pBtnFMChange && pBtnFMChange->GetCheckState())
				pBtnFMChange->SetCheckState(FALSE);

			if(pBtnFRChange) 	pBtnFRChange->SetCheckState(FALSE);
			this->m_ActiveFront = ITEM_NONE;
			this->m_ActiveRear = ITEM_REAR_VIDEO;
			this->m_ActiveMap = ITEM_FRONT_VIDEO;
			this->SetActiveItem();
		}
		else {
			if(this->m_ActiveFront != CBBPlayerUIControl::ITEM_FRONT_VIDEO){
				if(pBtnFRChange) 	pBtnFRChange->SetCheckState(FALSE);

				pVideoQuadCtrl->SetQuadViewMode(FALSE);
				
				this->m_MaxScreenActive= this->m_ActiveFront = CBBPlayerUIControl::ITEM_FRONT_VIDEO;
				this->m_ActiveRear = CBBPlayerUIControl::ITEM_REAR_VIDEO;
				this->m_ActiveMap = CBBPlayerUIControl::ITEM_MAP;	
				this->SetActiveItem();
			}
		}		
	}
	
	pVideo = VID_GET( TEXT("main.video.rear") );
	
	if(pVideoQuadCtrl && pVideoQuidView)
	{
		if(pVideoQuadCtrl->GetQuadViewMode())
		{		
			pVideoQuidView->EnableWindow(TRUE);
			pVideoQuidView->ShowWindow( SW_SHOW );

			pVideoQuadCtrl->EnableWindow(TRUE);
			pVideoQuadCtrl->ShowWindow( SW_SHOW );
			
			if(pVideo) pVideo->ShowWindow( SW_HIDE );
			
			if(pBtnZoom) { 
				pBtnZoom->SetCheckState(FALSE);
			}
		}
		else
		{
			pVideoQuidView->EnableWindow(FALSE);
			pVideoQuidView->ShowWindow( SW_HIDE );

			pVideoQuadCtrl->EnableWindow(FALSE);
			pVideoQuadCtrl->ShowWindow( SW_HIDE );

			if(pVideo) pVideo->ShowWindow( SW_SHOW );
		}
	}
#endif

	return;
}

VOID CBBPlayerUIControl::OnEventListSort( INT nType )
{
	ClButton* pBtnList[4] = { NULL, };
	pBtnList[0] = BTN_GET( m_Language.GetButtonText(TEXT("main.button.tabnormal")) );
	pBtnList[1] = BTN_GET( m_Language.GetButtonText(TEXT("main.button.tabevent"))	);
	pBtnList[2] = BTN_GET( m_Language.GetButtonText(TEXT("main.button.tabmanual")) );
	pBtnList[3] = BTN_GET( m_Language.GetButtonText(TEXT("main.button.tabmotion")) );

	INT nCounter;
	for( nCounter = 0; nCounter < 4; nCounter ++ )
	{
		if( !pBtnList[nCounter] )
			continue;

		if( nCounter == nType )	pBtnList[nCounter]->SetCheckState( TRUE );
		else					pBtnList[nCounter]->SetCheckState( FALSE );
	}
	return;
}


VOID CBBPlayerUIControl::OnEvnetSelectModel( VOID )
{
	CString cstrModelName	= ((CBBPlayerApp*)AfxGetApp())->GetConfigure()->getModelName();
	CString cstrImagePath	= ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getImageBasePath();
	CString cstrModelImage	= cstrImagePath + TEXT("\\") + cstrModelName + TEXT(".bmp");

	ClImage* pImageTitle = IMG_GET( TEXT("main.image.title") );
	if( pImageTitle )
	{
		ClDrawImage* pDrawImage = pImageTitle->GetImage();
		//
	}

	return;
}

//================================================================================================= E. EVENT
//================================================================================================= S. OPERATION

//================================================================================================= E. OPERATION
//================================================================================================= S. CONTROL

VOID CBBPlayerUIControl::SetActiveItem( BOOL bIsGpsData )
{
	ClVideoView*		pVideoFront		= VID_GET( TEXT("main.video.front") );
	ClVideoView*		pVideoRear		= VID_GET( TEXT("main.video.rear") );
	ClWebBrowser*	pMap			= MAP_GET( TEXT("main.web.map") );
	ClWebBrowser* 	pSponsor		= MAP_GET( TEXT("main.web.sponsor") );
	ClButton*		pBtnFRChange 	= BTN_GET( GetLangText(TEXT("main.button.frchange")) );
	ClButton*		pBtnFMChange 	= BTN_GET( m_Language.GetButtonText(TEXT("main.button.fmchange")) );
	ClButton*		pBtnZoom	 	= BTN_GET( TEXT("main.button.zoom") );
	ClVideoView*		pVideoQuadView = VID_GET( TEXT("main.video.quadview"));
	ClVideoView*		pVideoQuadCtrl= VID_GET( TEXT("main.video.quadctrl"));
	ClButton*		pBtnFFlip		= BTN_GET(TEXT("main.button.frontflip"));
	ClButton*		pBtnFMirror		= BTN_GET(TEXT("main.button.frontmirror"));
	ClButton*		pBtnRFlip		= BTN_GET(TEXT("main.button.rearflip"));
	ClButton*		pBtnRMirror		= BTN_GET(TEXT("main.button.rearmirror"));

	ClButton*	pBtnFront		= BTN_GET( TEXT("main.button.front")	);
	ClButton*	pBtnRear		= BTN_GET( TEXT("main.button.rear")	);
	ClButton*	pBtnMap		= BTN_GET( TEXT("main.button.map")	);
	
	ClListCtrl* 		pFileList 		= LST_GET( TEXT("main.listbox.filelist") );

	ClText* 			pTextLatitude	= TXT_GET( TEXT("main.text.latitude"));
	ClText* 			pTextLongitude	= TXT_GET( TEXT("main.text.longitude"));
	//ClText* 			pTextAltitude	= TXT_GET( TEXT("main.text.altitude"));
	ClText* 			pTextGpsSpeed	= TXT_GET( TEXT("main.text.gps_speed"));

	BOOL bFrontVideoShow = FALSE;
	BOOL bRearVideoShow = FALSE;
	BOOL bMapShow = TRUE;
	
	CRect crectMap;
	unsigned int ulGeo = PRSGEO_WEB_MAP;
			
	BOOL bIsInternetConnected = FALSE;
	if( (pMap && pMap->IsInternetConnected() && pMap->IsGPSData()) || bIsGpsData)
		bIsInternetConnected = TRUE;


	CDigitNum* pDigit		= DIGIT_GET(TEXT("main.digit.speed"));
	CDigitNum* pLat		= DIGIT_GET(TEXT("main.digit.latitude"));
	CDigitNum* pLnt		= DIGIT_GET(TEXT("main.digit.longitude"));

	if(pVideoFront) pVideoFront->ModifyStyle( SS_NOTIFY, 0);
	if(pVideoRear) pVideoRear->ModifyStyle( SS_NOTIFY, 0);
	
	if(pMap) 
	{
		pMap->SetUiText(pLat ? (CStatic*)pLat : pTextLatitude, pLnt ? (CStatic*)pLnt : pTextLongitude, pDigit ? (CStatic*)pDigit : pTextGpsSpeed, NULL);
	}
	
	if(pBtnZoom)
	{
		pBtnZoom->SetCheckState(FALSE);
		this->OnEventZoom();
	}
	
	if( this->m_nCurMode != 0 )
	{		
		//if( this->m_eMediaStatus != EVENT_MEDIA_PLAY ||(  (this->m_nStreamChannel==1) && bIsInternetConnected == FALSE) )
		//{
		//	if( pBtnFRChange ) pBtnFRChange->EnableWindow( FALSE );
		//	if( pBtnFMChange ) pBtnFMChange->EnableWindow( FALSE );

		//	return;
		//}
#if (BUILD_MODEL == MODEL_RALLY_EYES)
		if( pBtnFRChange && this->m_nStreamChannel > 1 || bIsInternetConnected) 	pBtnFRChange->EnableWindow( TRUE );
#else
		if( pBtnFRChange && this->m_nStreamChannel > 1 ) 	pBtnFRChange->EnableWindow( TRUE );
		if( pBtnFMChange && bIsInternetConnected ) 			pBtnFMChange->EnableWindow( TRUE );

		if( pBtnFront ) pBtnFront->EnableWindow( TRUE );
		
		if(this->m_nStreamChannel > 1){
			if( pBtnRear ) pBtnRear->EnableWindow( TRUE );
		}
		else {
			if( pBtnRear ) pBtnRear->EnableWindow( FALSE );
		}

		if(bIsInternetConnected){
			if( pBtnMap ) pBtnMap->EnableWindow( TRUE );
		}
		else {
			if( pBtnMap ) pBtnMap->EnableWindow( FALSE );
		}
#endif		
	
		if( this->m_MaxScreenActive == ITEM_FRONT_VIDEO )
		{
			if(pBtnFFlip) pBtnFFlip->ShowWindow( SW_SHOW);
			if(pBtnFMirror) pBtnFMirror->ShowWindow( SW_SHOW );

			if(pBtnRFlip) pBtnRFlip->ShowWindow( SW_HIDE);
			if(pBtnRMirror) pBtnRMirror->ShowWindow( SW_HIDE );

			bFrontVideoShow = TRUE;
			bMapShow = FALSE;  // 160628 jun
			
			if( pBtnFRChange ) pBtnFRChange->SetCheckState( FALSE );
			if( pBtnFMChange ) pBtnFMChange->SetCheckState( FALSE );
		}
		else if( this->m_MaxScreenActive == ITEM_REAR_VIDEO && this->m_nStreamChannel==2)
		{	
			if(pBtnFFlip) pBtnFFlip->ShowWindow( SW_HIDE);
			if(pBtnFMirror) pBtnFMirror->ShowWindow( SW_HIDE );

			if(pBtnRFlip) pBtnRFlip->ShowWindow( SW_SHOW);
			if(pBtnRMirror) pBtnRMirror->ShowWindow( SW_SHOW );

			bRearVideoShow = TRUE;
			bMapShow = FALSE;  // 160628 jun

			if( pBtnFRChange ) pBtnFRChange->SetCheckState( TRUE );
			if( pBtnFMChange ) pBtnFMChange->SetCheckState( FALSE );
		}
		else if( this->m_MaxScreenActive == ITEM_MAP && bIsInternetConnected)
		{
			RECT rFront;
			if(pBtnFFlip) pBtnFFlip->ShowWindow( SW_HIDE);
			if(pBtnFMirror) pBtnFMirror->ShowWindow( SW_HIDE );

			if(pBtnRFlip) pBtnRFlip->ShowWindow( SW_HIDE);
			if(pBtnRMirror) pBtnRMirror->ShowWindow( SW_HIDE );

			if(pVideoFront) pVideoFront->GetWindowRect(&rFront);

			bMapShow = TRUE;

			if( pMap ) {
				crectMap.left = rFront.left - this->m_rWindowRect.left;
				crectMap.top = rFront.top - this->m_rWindowRect.top;
				crectMap.right = (this->m_rWindowRect.right - this->m_rWindowRect.left) - crectMap.left;
				crectMap.bottom = (this->m_rWindowRect.bottom - this->m_rWindowRect.top)  - crectMap.top;
			}
			
			if( pBtnFRChange ) pBtnFRChange->SetCheckState( FALSE );
			if( pBtnFMChange ) pBtnFMChange->SetCheckState( TRUE );
		}
		else
		{
			bMapShow = FALSE;
			this->m_ActiveFront = this->m_MaxScreenActive = ITEM_FRONT_VIDEO;
			bFrontVideoShow = TRUE;
			if( pBtnFRChange ) pBtnFRChange->SetCheckState( FALSE );
			if( pBtnFMChange ) pBtnFMChange->SetCheckState( FALSE );
		}

		if(pSponsor)  pSponsor->MoveWindow( 0, 0, 0, 0 );
	}
	else
	{

		if(pSponsor)  pSponsor->MoveWindow(  this->m_geoPreserved[PRSGEO_WEB_SPONSOR].x, this->m_geoPreserved[PRSGEO_WEB_SPONSOR].y,
					this->m_geoPreserved[PRSGEO_WEB_SPONSOR].width, this->m_geoPreserved[PRSGEO_WEB_SPONSOR].height );
		
		if(pBtnFFlip) pBtnFFlip->ShowWindow( SW_SHOW);
		if(pBtnFMirror) pBtnFMirror->ShowWindow( SW_SHOW );

		if(pBtnRFlip) pBtnRFlip->ShowWindow( SW_SHOW);
		if(pBtnRMirror) pBtnRMirror->ShowWindow( SW_SHOW );

#if (BUILD_MODEL == MODEL_RALLY_EYES)
		if( this->m_eMediaStatus != EVENT_MEDIA_PLAY || (this->m_nStreamChannel==1 && !bIsInternetConnected) )
#else
		if( this->m_eMediaStatus != EVENT_MEDIA_PLAY || (this->m_nStreamChannel==1) )
#endif			
		{
			if( pBtnFRChange ) pBtnFRChange->EnableWindow( FALSE );
		}
		else
		{
			if( pBtnFRChange ) pBtnFRChange->EnableWindow( TRUE );
		}

		if( bIsInternetConnected && this->m_eMediaStatus == EVENT_MEDIA_PLAY )
		{
			if(pBtnFMChange)	pBtnFMChange->EnableWindow( TRUE );
		}
		else
		{
			if(pBtnFMChange) pBtnFMChange->EnableWindow( FALSE  );
		}

		if( this->m_ActiveFront == ITEM_FRONT_VIDEO )
		{
			if( pVideoFront )
			{
				pVideoFront->MoveWindow( this->m_geoPreserved[PRSGEO_VIDEO_FRONT].x, this->m_geoPreserved[PRSGEO_VIDEO_FRONT].y,
					this->m_geoPreserved[PRSGEO_VIDEO_FRONT].width, this->m_geoPreserved[PRSGEO_VIDEO_FRONT].height );
			}
			if( pBtnFRChange ) pBtnFRChange->SetCheckState( FALSE );
		}
		else if( this->m_ActiveFront == ITEM_REAR_VIDEO )
		{
			if( pVideoRear )
			{
				pVideoRear->MoveWindow( this->m_geoPreserved[PRSGEO_VIDEO_FRONT].x, this->m_geoPreserved[PRSGEO_VIDEO_FRONT].y,
					this->m_geoPreserved[PRSGEO_VIDEO_FRONT].width, this->m_geoPreserved[PRSGEO_VIDEO_FRONT].height );
			}
		}
		else if( this->m_ActiveFront == ITEM_MAP)
		{
			if( pMap )
			{
				ulGeo = PRSGEO_VIDEO_FRONT;
				pMap->MoveWindow( this->m_geoPreserved[PRSGEO_VIDEO_FRONT].x, this->m_geoPreserved[PRSGEO_VIDEO_FRONT].y,
					this->m_geoPreserved[PRSGEO_VIDEO_FRONT].width, this->m_geoPreserved[PRSGEO_VIDEO_FRONT].height );
			}
		}

		if( this->m_ActiveRear == ITEM_FRONT_VIDEO )
		{
			if( pVideoFront )
			{
				pVideoFront->MoveWindow( this->m_geoPreserved[PRSGEO_VIDEO_REAR].x, this->m_geoPreserved[PRSGEO_VIDEO_REAR].y,
					this->m_geoPreserved[PRSGEO_VIDEO_REAR].width, this->m_geoPreserved[PRSGEO_VIDEO_REAR].height );
			}
		}
		else if(this->m_nStreamChannel > 1 &&  this->m_ActiveRear == ITEM_REAR_VIDEO )
		{
			if( pVideoRear )
			{
				pVideoRear->ModifyStyle( 0, SS_NOTIFY);
				pVideoRear->MoveWindow( this->m_geoPreserved[PRSGEO_VIDEO_REAR].x, this->m_geoPreserved[PRSGEO_VIDEO_REAR].y,
					this->m_geoPreserved[PRSGEO_VIDEO_REAR].width, this->m_geoPreserved[PRSGEO_VIDEO_REAR].height );
			}
		}

		if(this->m_nStreamChannel > 1 )
		{
			if(pVideoFront && this->m_ActiveMap == ITEM_FRONT_VIDEO)
			{
				pVideoFront->ModifyStyle( 0, SS_NOTIFY);
				pVideoFront->MoveWindow( this->m_geoPreserved[PRSGEO_WEB_MAP].x, this->m_geoPreserved[PRSGEO_WEB_MAP].y,
					this->m_geoPreserved[PRSGEO_VIDEO_REAR].width, this->m_geoPreserved[PRSGEO_VIDEO_REAR].height );
			}
			else if(pVideoRear && this->m_ActiveMap == ITEM_REAR_VIDEO)
			{
				pVideoRear->MoveWindow( this->m_geoPreserved[PRSGEO_WEB_MAP].x, this->m_geoPreserved[PRSGEO_WEB_MAP].y,
					this->m_geoPreserved[PRSGEO_VIDEO_REAR].width, this->m_geoPreserved[PRSGEO_VIDEO_REAR].height );
			}
		}
		
		if( pMap && ( this->m_ActiveRear == ITEM_MAP || this->m_ActiveMap == ITEM_MAP) )
		{
			if( bIsInternetConnected == FALSE )
			{
#if DEF_LISTBOX_FILELIST_MOVE_USE
				if(this->m_nStreamChannel > 1) // 2ch
				{
					if(pFileList) pFileList->MoveWindow( this->m_geoPreserved[PRSGEO_WEB_MAP].x, this->m_geoPreserved[PRSGEO_WEB_MAP].y,
						this->m_geoPreserved[PRSGEO_LISTBOX_FILELIST].width, 
						(this->m_geoPreserved[PRSGEO_LISTBOX_FILELIST].y - this->m_geoPreserved[PRSGEO_WEB_MAP].y) + this->m_geoPreserved[PRSGEO_LISTBOX_FILELIST].height);
				}
				else
				{
					if(pFileList) pFileList->MoveWindow( this->m_geoPreserved[PRSGEO_VIDEO_REAR].x, this->m_geoPreserved[PRSGEO_VIDEO_REAR].y,
						this->m_geoPreserved[PRSGEO_LISTBOX_FILELIST].width, 
						(this->m_geoPreserved[PRSGEO_LISTBOX_FILELIST].y - this->m_geoPreserved[PRSGEO_VIDEO_REAR].y) + this->m_geoPreserved[PRSGEO_LISTBOX_FILELIST].height);
				}
#endif
			}
			else
			{
				if(this->m_ActiveRear == ITEM_MAP)
					ulGeo = PRSGEO_VIDEO_REAR;
				else
					ulGeo = PRSGEO_WEB_MAP;
			
				//pMap->MoveWindow( this->m_geoPreserved[PRSGEO_VIDEO_REAR].x, this->m_geoPreserved[PRSGEO_VIDEO_REAR].y,
					//this->m_geoPreserved[PRSGEO_VIDEO_REAR].width, this->m_geoPreserved[PRSGEO_VIDEO_REAR].height );
#if DEF_LISTBOX_FILELIST_MOVE_USE
				if(pFileList) pFileList->MoveWindow( this->m_geoPreserved[PRSGEO_WEB_MAP].x, this->m_geoPreserved[PRSGEO_WEB_MAP].y,
					this->m_geoPreserved[PRSGEO_LISTBOX_FILELIST].width, 
					(this->m_geoPreserved[PRSGEO_LISTBOX_FILELIST].y - this->m_geoPreserved[PRSGEO_WEB_MAP].y)  + this->m_geoPreserved[PRSGEO_LISTBOX_FILELIST].height);
#endif
			}
		}

		crectMap.left = this->m_geoPreserved[ulGeo].x;
		crectMap.top = this->m_geoPreserved[ulGeo].y;
		crectMap.right = crectMap.left + this->m_geoPreserved[ulGeo].width;
		crectMap.bottom = crectMap.top + this->m_geoPreserved[ulGeo].height;

		//pMap->MoveWindow(crectMap);

		if( this->m_ActiveFront != ITEM_FRONT_VIDEO && this->m_ActiveRear != ITEM_FRONT_VIDEO && this->m_ActiveMap!= ITEM_FRONT_VIDEO ) {
			if( pVideoFront ) pVideoFront->ShowWindow( SW_HIDE );
		}
		else {
			bFrontVideoShow = TRUE;
		}

		if((this->m_nCurMode == 1 && this->m_ActiveFront != ITEM_REAR_VIDEO) ||  \
			(this->m_ActiveFront != ITEM_REAR_VIDEO && (this->m_nCurMode == 0 && this->m_ActiveRear != ITEM_REAR_VIDEO) &&  this->m_ActiveMap != ITEM_REAR_VIDEO)) {
			if( pVideoRear ) pVideoRear->ShowWindow( SW_HIDE );
		}
		else if(this->m_nStreamChannel > 1) {
			bRearVideoShow = TRUE;
		}
	}	

	if(pVideoQuadCtrl) {

		if(pVideoQuadCtrl->GetQuadViewMode()){
			//OnEventMux();
			pVideoQuadCtrl->ShowWindow( SW_SHOW );
			if(pVideoQuadView) pVideoQuadView->ShowWindow( SW_SHOW );
			
			if(this->m_ActiveFront == ITEM_FRONT_VIDEO)
				bFrontVideoShow = FALSE;
			else if(this->m_ActiveFront == ITEM_REAR_VIDEO)
				bRearVideoShow = FALSE;

			bMapShow = FALSE;
			if(pBtnZoom) pBtnZoom->EnableWindow(FALSE);
		}
		else {
			pVideoQuadCtrl->ShowWindow( SW_HIDE );
			if(pVideoQuadView) pVideoQuadView->ShowWindow( SW_HIDE );
			if(pBtnZoom) pBtnZoom->EnableWindow(TRUE);
		}
	}
		
	if(pVideoFront && bFrontVideoShow == TRUE)
		pVideoFront->ShowWindow( SW_SHOW );
	else if(pVideoFront) 
		pVideoFront->ShowWindow( SW_HIDE );
	
	if(pVideoRear && bRearVideoShow == TRUE)
		pVideoRear->ShowWindow( SW_SHOW );
	else if(pVideoRear)
		pVideoRear->ShowWindow( SW_HIDE );

	if( pMap && (!bIsInternetConnected || bMapShow == FALSE) ) {
		if(pMap) pMap->MoveWindow( 0, 0, 0, 0 );
	}

#ifdef MAP_OFF // jun 190710 MAP OFF
//	else if (pMap){
//		pMap->SetWindowRect(&crectMap);
		//pMap->ShowWindow( SW_SHOW );
//	}

#else
	else if (pMap){
		pMap->SetWindowRect(&crectMap);
		//pMap->ShowWindow( SW_SHOW );
	}
	
#endif

	if(m_pMultiPlayer)	m_pMultiPlayer->UpdateWindow();

	return;
}


VOID CBBPlayerUIControl::ActivateInitMode( VOID )
{
	this->m_eCurUIState = UISTATE_INIT;

//	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.image.title"),		TRUE,	TRUE	);

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.setting"),		TRUE,	TRUE	);

#ifdef CAPTURE_OFF   // jun 20201104
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.capture"),	FALSE,	FALSE	);
#else
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.capture"),	TRUE,	FALSE	);
#endif


#ifdef PRINT_OFF   // jun 20201104
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.print"),	FALSE,	FALSE	);
#else
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.print"),	TRUE,	FALSE	);
#endif
	
	
#ifdef BACKUP_OFF          // jun 181217
    this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.save"),		FALSE,	FALSE	);
#else
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.save"),		TRUE,	FALSE	);
#endif

#if (BUILD_MODEL == MODEL_DAVIEW_2CH || BUILD_MODEL == MODEL_DAVIEW_2CH_L || BUILD_MODEL == MODEL_DAVIEW_2CH_F || BUILD_MODEL == MODEL_DAVIEW_2CH_S)	// jun 170405  170406	171018
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.log"),		FALSE,	FALSE	);

	if(this->m_bIsNxfsFileExist){
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.saveall"),	TRUE,	TRUE	);
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.format"),		TRUE,	TRUE	);
	}
	else {
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.saveall"),	TRUE,	FALSE	);
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.format"),		TRUE,	FALSE	);
	}
#else
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.log"),		TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.saveall"),		FALSE,	FALSE	);
#endif

#ifdef ABOUT_OFF  // jun 181217
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.about"),		FALSE,	TRUE	);
#else
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.about"),		TRUE,	TRUE	);
#endif

#ifdef BUILD_NO_LOGO
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.site"),		FALSE,	TRUE	);
#elif (BUILD_MODEL == MODEL_DAVIEW_2CH || BUILD_MODEL == MODEL_DAVIEW_2CH_L || BUILD_MODEL == MODEL_DAVIEW_2CH_F || BUILD_MODEL == MODEL_DAVIEW_2CH_S) // jun 171018
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.site"),		FALSE,	FALSE	);
#else
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.site"),		TRUE,	TRUE	);
#endif
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.min"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.exit"),		TRUE,	TRUE	);

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.max"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.nor"),		TRUE,	TRUE	);

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.prev"),		TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.play"),		TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.stop"),		TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.next"),		TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.rew"),		TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.ff"),			TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.fps"),		TRUE,	FALSE	);

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.front"),		TRUE,	FALSE	);
	
#ifdef REAR_CAM_OFF                // jun 170406
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.rear"),		FALSE,	FALSE	);
#else
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.rear"),		TRUE,	FALSE	);
#endif


#ifdef MAP_OFF    // jun 190710
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.map"),		FALSE,	FALSE	);
#else
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.map"),		TRUE,	FALSE	);
#endif
	
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, GetLangText(TEXT("main.button.open")),		TRUE,	TRUE	);
	
#ifdef REAR_CAM_OFF                // jun 170406	
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, GetLangText(TEXT("main.button.frchange")),	FALSE,	FALSE	);
#else
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, GetLangText(TEXT("main.button.frchange")),	TRUE,	FALSE	);
#endif

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_Language.GetButtonText(TEXT("main.button.fmchange")),	TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.zoom"),	TRUE,	FALSE	);

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.frontflip"),	TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.frontmirror"),	TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.rearflip"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.rearmirror"),	TRUE,	TRUE	);
	
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_Language.GetButtonText(TEXT("main.button.tabnormal")),	TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_Language.GetButtonText(TEXT("main.button.tabevent")),	TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_Language.GetButtonText(TEXT("main.button.tabmanual")),	TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_Language.GetButtonText(TEXT("main.button.tabmotion")),	TRUE,	TRUE	);

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.slider.time"),		TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.curtime"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.duration"),		TRUE,	TRUE	);

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.image.speed0"),		TRUE,	FALSE	);
//	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.image.speed1"),		TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.slider.speed"),		TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.speed"),		TRUE,	FALSE	);

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.image.bright"),		TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.slider.bright"),		TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.bright"),		TRUE,	FALSE	);
	
#ifdef MAP_OFF  // jun 190710
//	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.latitude"),		TRUE,	TRUE	);
//	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.longitude"),	TRUE,	TRUE	);
#else
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.latitude"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.longitude"),		TRUE,	TRUE	);
#endif
	//this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.altitude"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.gps_speed"),		TRUE,	TRUE	);
	
#if defined(TIME_SEARCH_USE)
 #if defined(RDF_USE)
 	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.gsensor.graph"),			TRUE, 	TRUE	);
 #else
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.gsensor.graph"),			FALSE, 	TRUE	);
 #endif
#else	
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.gsensor.graph"),			TRUE, 	FALSE	);
#endif	

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.digit.speed"),		TRUE,	TRUE	);

	//this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.digit.rpm"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.analogmeter.rpm"),		TRUE,	TRUE	);
	
 #if (BUILD_MODEL == MODEL_RALLY_EYES)
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.digit.latitude"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.digit.longitude"),		TRUE,	TRUE	);
#endif

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.tacho1"),		TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.tacho2"),		TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.tacho3"),		TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.tacho4"),		TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.tacho5"),		TRUE,	FALSE	);

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.gpspulse"),	TRUE,	FALSE	);

	this->m_pDialogManager->AdaptResourceState( this->m_nCurMode );
	return;
}


VOID CBBPlayerUIControl::ActivateSelectedDriveMode( VOID )
{
	this->m_eCurUIState = UISTATE_SELDRIVE;

//	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.image.title"),		TRUE,	TRUE	);

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.setting"),	TRUE,	TRUE	);

#ifdef CAPTURE_OFF   // jun 20201104
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.capture"),	FALSE,	FALSE	);
#else
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.capture"),	TRUE,	FALSE	);
#endif

	
#ifdef PRINT_OFF   // jun 20201104	
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.print"),	FALSE,	FALSE	);
#else
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.print"),	TRUE,	FALSE	);
#endif

	
#ifdef ABOUT_OFF  // jun 181217
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.about"),		FALSE,	TRUE	);
#else
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.about"),		TRUE,	TRUE	);
#endif
	
#ifdef BUILD_NO_LOGO
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.site"),		FALSE,	TRUE	);
#elif (BUILD_MODEL == MODEL_DAVIEW_2CH || BUILD_MODEL == MODEL_DAVIEW_2CH_L || BUILD_MODEL == MODEL_DAVIEW_2CH_F || BUILD_MODEL == MODEL_DAVIEW_2CH_S)// jun 171018
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.site"),		FALSE,	FALSE	);
#else
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.site"),		TRUE,	TRUE	);
#endif

#if (BUILD_MODEL == MODEL_DAVIEW_2CH || BUILD_MODEL == MODEL_DAVIEW_2CH_L || BUILD_MODEL == MODEL_DAVIEW_2CH_F || BUILD_MODEL == MODEL_DAVIEW_2CH_S)	// jun 170405 170406  171018
	if(this->m_bIsNxfsFileExist){
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.saveall"),	TRUE,	TRUE	);
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.format"),		TRUE,	TRUE	);
	}
	else {
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.saveall"),	TRUE,	FALSE	);
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.format"),		TRUE,	FALSE	);
	}
#else
	if(this->m_bIsLogFileExist)
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.log"),	TRUE,	TRUE	);
	else
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.log"),	TRUE,	FALSE	);
#endif

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.min"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.max"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.nor"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.exit"),		TRUE,	TRUE	);

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.prev"),		TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.play"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.stop"),		TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.next"),		TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.rew"),		TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.ff"),			TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.fps"),		TRUE,	FALSE	);

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.front"),		TRUE,	FALSE	);
	
#ifdef REAR_CAM_OFF                // jun 170406
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.rear"),		FALSE,	FALSE	);
#else
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.rear"),		TRUE,	FALSE	);
#endif

#ifdef MAP_OFF    // jun 190710
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.map"),		FALSE,	FALSE	);
#else
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.map"),		TRUE,	FALSE	);
#endif
	
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, GetLangText(TEXT("main.button.open")),		TRUE,	TRUE	);

#ifdef BACKUP_OFF    // jun 181217
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.save"),		FALSE,	FALSE	);
#else
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.save"),		TRUE,	FALSE	);
#endif

#ifdef REAR_CAM_OFF                // jun 170406	
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, GetLangText(TEXT("main.button.frchange")),	FALSE,	FALSE	);
#else
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, GetLangText(TEXT("main.button.frchange")),	TRUE,	FALSE	);
#endif

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_Language.GetButtonText(TEXT("main.button.fmchange")),	TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.zoom"),		TRUE,	FALSE	);

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.frontflip"),	TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.frontmirror"),	TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.rearflip"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.rearmirror"),	TRUE,	TRUE	);
	
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_Language.GetButtonText(TEXT("main.button.tabnormal")),	TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_Language.GetButtonText(TEXT("main.button.tabevent")),	TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_Language.GetButtonText(TEXT("main.button.tabmanual")),	TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_Language.GetButtonText(TEXT("main.button.tabmotion")),	TRUE,	TRUE	);

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.slider.time"),		TRUE,	FALSE	);

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.image.speed0"),		TRUE,	TRUE	);
//	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.image.speed1"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.slider.speed"),		TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.speed"),		TRUE,	FALSE	);

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.image.bright"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.slider.bright"),		TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.bright"),		TRUE,	FALSE	);

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.tacho1"),		TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.tacho2"),		TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.tacho3"),		TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.tacho4"),		TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.tacho5"),		TRUE,	FALSE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.gpspulse"),		TRUE,	FALSE	);


#ifdef AUDIO_VOLUME_CONTROL_USE
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.volume"),	TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.slider.volume"),		TRUE,	TRUE	);
#endif
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.image.file"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.file"),			TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.curtime"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.duration"),		TRUE,	TRUE	);

#ifdef MAP_OFF  // jun 190710
//	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.latitude"),		TRUE,	TRUE	);
//	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.longitude"),		TRUE,	TRUE	);
#else
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.latitude"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.longitude"),		TRUE,	TRUE	);
#endif
	//this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.altitude"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.gps_speed"),		TRUE,	TRUE	);

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.checkfilelistall"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.listbox.filelist"),		TRUE,	TRUE	);
#if (!defined(TIME_SEARCH_USE) || defined(RDF_USE))
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.gsensor.graph"),			TRUE, 	TRUE	);
#endif

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.digit.speed"),		TRUE,	TRUE	);
	//this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.digit.rpm"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.analogmeter.rpm"),		TRUE,	TRUE	);

 #if (BUILD_MODEL == MODEL_RALLY_EYES)
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.digit.latitude"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.digit.longitude"),		TRUE,	TRUE	);
 #endif

	this->m_pDialogManager->AdaptResourceState( this->m_nCurMode );
	return;
}


VOID CBBPlayerUIControl::ActivatePlayMode( VOID )
{
	ClVideoView*		pVideoQuadCtrl		= VID_GET( TEXT("main.video.quadctrl") );
	
	this->m_eCurUIState = UISTATE_MEDIA;

//	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.image.title"),		TRUE,	TRUE	);

	if( this->m_eMediaStatus == EVENT_MEDIA_PLAY )
	{

#ifdef CAPTURE_OFF   // jun 20201104
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.capture"),	FALSE,	TRUE	);
#else
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.capture"),	TRUE,	TRUE	);
#endif

#ifdef PRINT_OFF   // jun 20201104
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.print"),	FALSE,	TRUE	);
#else
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.print"),	TRUE,	TRUE	);
#endif

	}
	else // EVENT_MEDIA_STOP
	{
	
#ifdef CAPTURE_OFF   // jun 20201104
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.capture"),	FALSE,	FALSE	);
#else
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.capture"),	TRUE,	FALSE	);
#endif


#ifdef PRINT_OFF   // jun 20201104
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.print"),	FALSE,	FALSE	);
#else
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.print"),	TRUE,	FALSE	);
#endif

	}

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.setting"),		TRUE,	TRUE	);
	
#ifdef ABOUT_OFF  // jun 181217
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.about"),		FALSE,	TRUE	);
#else
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.about"),		TRUE,	TRUE	);
#endif
	
#ifdef BUILD_NO_LOGO
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.site"),		FALSE,	TRUE	);
#elif (BUILD_MODEL == MODEL_DAVIEW_2CH || BUILD_MODEL == MODEL_DAVIEW_2CH_L || BUILD_MODEL == MODEL_DAVIEW_2CH_F || BUILD_MODEL == MODEL_DAVIEW_2CH_S) // jun 171018
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.site"),		FALSE,	FALSE	);
#else		
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.site"),		TRUE,	TRUE	);
#endif

#if (BUILD_MODEL == MODEL_DAVIEW_2CH || BUILD_MODEL == MODEL_DAVIEW_2CH_L || BUILD_MODEL == MODEL_DAVIEW_2CH_F || BUILD_MODEL == MODEL_DAVIEW_2CH_S)	// jun 170405  170406  171018
	if(this->m_bIsNxfsFileExist){
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.saveall"),	TRUE,	TRUE	);
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.format"),		TRUE,	TRUE	);
	}
	else {
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.saveall"),	TRUE,	FALSE	);
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.format"),		TRUE,	FALSE	);
	}
#else
	if(this->m_bIsLogFileExist)
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.log"),	TRUE,	TRUE	);
	else
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.log"),	TRUE,	FALSE	);
#endif

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.min"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.exit"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.max"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.nor"),		TRUE,	TRUE	);

	if( this->m_eMediaStatus == EVENT_MEDIA_PLAY )
	{
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.prev"),		TRUE,	TRUE	);
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.play"),		TRUE,	TRUE	);
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.stop"),		TRUE,	TRUE	);
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.next"),		TRUE,	TRUE	);
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.rew"),		TRUE,	TRUE	);
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.ff"),			TRUE,	TRUE	);
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.fps"),		TRUE,	TRUE	);

		if(pVideoQuadCtrl && pVideoQuadCtrl->GetQuadViewMode())
			this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.zoom"),		TRUE,	FALSE	);
		else
			this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.zoom"),		TRUE,	TRUE	);

#ifdef BACKUP_OFF// jun 181217
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.save"),		FALSE,	TRUE	);
#else
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.save"),		TRUE,	TRUE	);
#endif
	}
	else
	{
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.prev"),		TRUE,	FALSE	);
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.play"),		TRUE,	TRUE	);
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.stop"),		TRUE,	FALSE	);
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.next"),		TRUE,	FALSE	);
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.rew"),		TRUE,	FALSE	);
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.ff"),			TRUE,	FALSE	);
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.fps"),		TRUE,	FALSE	);
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.zoom"),		TRUE,	FALSE	);

#ifdef BACKUP_OFF // jun 181217
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.save"),		FALSE,	FALSE	);
#else
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.save"),		TRUE,	FALSE	);
#endif
	}

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.tacho1"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.tacho2"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.tacho3"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.tacho4"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.tacho5"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.gpspulse"),	TRUE,	TRUE	);
	
	ClListCtrl* pFileList = LST_GET( TEXT("main.listbox.filelist") );
	BOOL isChecked = FALSE;
	if(pFileList){
		int nCount = pFileList->GetItemCount();
		for(int i = 0; i< nCount;i++) {
				if(pFileList->GetCheck(i)) {
					isChecked = TRUE;
					break;
				}
		     }
	    }
	
#ifdef BACKUP_OFF  // jun 181217
	if(/* this->m_eMediaStatus == EVENT_MEDIA_PLAY || */isChecked ){
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.save"),		FALSE,	FALSE	);  // jun 170623
		
	}
	else
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.save"),		FALSE,	FALSE	);
	

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.frontflip"),	TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.frontmirror"),	TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.rearflip"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.rearmirror"),	TRUE,	TRUE	);

#else
	if(/* this->m_eMediaStatus == EVENT_MEDIA_PLAY || */isChecked ){
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.save"),		TRUE,	TRUE	);  // jun 170623
		
	}
	else
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.save"),		TRUE,	FALSE	);
	

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.frontflip"),	TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.frontmirror"),	TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.rearflip"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.rearmirror"),	TRUE,	TRUE	);
#endif


#if (BUILD_MODEL == MODEL_RALLY_EYES)
	this->m_pDialogManager->SetResourceState(  this->m_nCurMode, TEXT("main.button.open"),		TRUE,	TRUE	);
#else
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_Language.GetButtonText(TEXT("main.button.open")),		TRUE,	TRUE	);
#endif
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_Language.GetButtonText(TEXT("main.button.tabnormal")),	TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_Language.GetButtonText(TEXT("main.button.tabevent")),	TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_Language.GetButtonText(TEXT("main.button.tabmanual")),	TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_Language.GetButtonText(TEXT("main.button.tabmotion")),	TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.slider.time"),		TRUE,	TRUE	);

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.video.zoom"),		FALSE,	FALSE	);
	if( this->m_eMediaStatus == EVENT_MEDIA_PLAY )
	{
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.video.front"),	TRUE,	TRUE	);
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.video.rear"),	TRUE,	TRUE	);
	}
	else
	{
		ClVideoView*		pVideoFront		= VID_GET( TEXT("main.video.front") );
		ClVideoView*		pVideoRear		= VID_GET( TEXT("main.video.rear") );

		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.video.quadview"),	FALSE,	FALSE	);
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.video.quadctrl"),	FALSE,	FALSE	);
		
		if(pVideoQuadCtrl){
			ClDialogManager::PCONTROL_INFO pInfo = this->m_pDialogManager->GetResource( TEXT("main.video.quadctrl") );
			if( pInfo && pInfo->m_pContainer ){
				ClGeometry ctrl_geo = pInfo->m_pContainer->GetGeometry();
				pVideoQuadCtrl->MoveWindow( ctrl_geo.x, ctrl_geo.y, ctrl_geo.width, ctrl_geo.height );
			}

			pVideoQuadCtrl->SetQuadViewMode(FALSE);
		}
			
		if( pVideoFront )
		{
			pVideoFront->MoveWindow( this->m_geoPreserved[PRSGEO_VIDEO_FRONT].x, this->m_geoPreserved[PRSGEO_VIDEO_FRONT].y,
				this->m_geoPreserved[PRSGEO_VIDEO_FRONT].width, this->m_geoPreserved[PRSGEO_VIDEO_FRONT].height );
		}

		if( pVideoRear )
		{
			pVideoRear->MoveWindow( this->m_geoPreserved[PRSGEO_VIDEO_REAR].x, this->m_geoPreserved[PRSGEO_VIDEO_REAR].y,
				this->m_geoPreserved[PRSGEO_VIDEO_REAR].width, this->m_geoPreserved[PRSGEO_VIDEO_REAR].height );
		}
		
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.video.front"),	FALSE,	FALSE	);
		this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.video.rear"),	FALSE,	FALSE	);
	}

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.image.speed0"),		TRUE,	TRUE	);
//	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.image.speed1"),		TRUE,	TRUE	);

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.slider.speed"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.speed"),		TRUE,	TRUE	);

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.image.bright"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.slider.bright"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.bright"),		TRUE,	TRUE	);

#ifdef AUDIO_VOLUME_CONTROL_USE
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.volume"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.slider.volume"),		TRUE,	TRUE	);
#endif
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.image.file"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.file"),			TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.curtime"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.duration"),		TRUE,	TRUE	);

#ifdef MAP_OFF  // jun 190710
//	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.latitude"),		TRUE,	TRUE	);
//	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.longitude"),		TRUE,	TRUE	);
#else
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.latitude"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.longitude"),		TRUE,	TRUE	);
#endif
	//this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.altitude"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.text.gps_speed"),		TRUE,	TRUE	);

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.checkfilelistall"),	TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.listbox.filelist"),	TRUE,	TRUE	);

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.front"),	TRUE,	FALSE	);
	
#ifdef REAR_CAM_OFF                // jun 170406
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.rear"),	FALSE,	FALSE	);
#else
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.rear"),	TRUE,	FALSE	);
#endif

#ifdef MAP_OFF    // jun 190710
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.map"),	FALSE,	FALSE	);
#else
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.button.map"),	TRUE,	FALSE	);
#endif
	
#ifdef REAR_CAM_OFF                // jun 170406	
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, GetLangText(TEXT("main.button.frchange")),		FALSE,	FALSE	);
#else
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, GetLangText(TEXT("main.button.frchange")),		TRUE,	FALSE	);
#endif

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, m_Language.GetButtonText(TEXT("main.button.fmchange")),		TRUE,	FALSE	);

#if (!defined(TIME_SEARCH_USE) || defined(RDF_USE))
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.gsensor.graph"),		TRUE, 	TRUE	);
#endif

	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.digit.speed"),		TRUE,	TRUE	);
	//this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.digit.rpm"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.analogmeter.rpm"),		TRUE,	TRUE	);
	
 #if (BUILD_MODEL == MODEL_RALLY_EYES)
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.digit.latitude"),		TRUE,	TRUE	);
	this->m_pDialogManager->SetResourceState( this->m_nCurMode, TEXT("main.digit.longitude"),		TRUE,	TRUE	);
#endif

	this->m_pDialogManager->AdaptResourceState( this->m_nCurMode );

	return;
}

//================================================================================================= E. CONTROL
//================================================================================================= S. GET/SET

BOOL CBBPlayerUIControl::SetBBPlayerProp( ClDialogManager* pDialogManager, CMultiPlayer* pMultiPlayer, CTextData* pSensorData )
{
	if( !pDialogManager || !pMultiPlayer || !pSensorData )
		return FALSE;

	this->m_pDialogManager	= pDialogManager;
	this->m_pMultiPlayer	= pMultiPlayer;
	this->m_pSensorData = pSensorData;
	return TRUE;
}


VOID CBBPlayerUIControl::SetStreamInfo( INT nChannel, BOOL bIsGPS, BOOL bIsGsensor )
{
	this->m_nStreamChannel	= nChannel;
	this->m_bIsStreamGPS	= bIsGPS;
	this->m_bIsStreamGSensor= bIsGsensor;

	return;
}


INT CBBPlayerUIControl::GetStreamChannel( VOID )
{
	return this->m_nStreamChannel;
}


BOOL CBBPlayerUIControl::GetStreamGPS( VOID )
{
	return this->m_bIsStreamGPS;
}


BOOL CBBPlayerUIControl::GetStreamGSensor( VOID )
{
	return this->m_bIsStreamGSensor;
}


CBBPlayerUIControl::UI_EVENT CBBPlayerUIControl::GetListTabPrevEvent( VOID )
{
	return this->m_eSelectedListTab;
}


VOID CBBPlayerUIControl::SetUIState( BBPLAYER_UISTATE state )
{
	if( this->m_eCurUIState != state )
		this->m_eCurUIState = state;
	
	switch( this->m_eCurUIState )
	{
	case UISTATE_INIT:		this->ActivateInitMode();			break;
	case UISTATE_SELDRIVE:	this->ActivateSelectedDriveMode();	break;
	case UISTATE_MEDIA:		this->ActivatePlayMode();			break;
	}

#ifdef RDF_USE
	CGSensorView* m_pGSensorView = GSG_GET( TEXT("main.gsensor.graph") );
	if(m_pGSensorView)	
	{
		m_pGSensorView->ShowWindow(SW_HIDE);
		
		if( this->m_eMediaStatus == EVENT_MEDIA_PLAY)
		{
			if(this->m_nCurMode == 0)
			{
				m_pGSensorView->MoveWindow( this->m_geoPreserved[PRSGEO_GSENSOR_GRAPH].x, this->m_geoPreserved[PRSGEO_GSENSOR_GRAPH].y,
					this->m_geoPreserved[PRSGEO_GSENSOR_GRAPH].width, this->m_geoPreserved[PRSGEO_GSENSOR_GRAPH].height );
			}
			else
			{
				return;
			}
		}
		else
		{
			if(this->m_nCurMode != 0)
			{
				ClVideoView*		pVideoFront		= VID_GET( TEXT("main.video.front") );
				if(pVideoFront)
				{
					RECT rFront;
					pVideoFront->GetWindowRect(&rFront);
					m_pGSensorView->MoveWindow(&rFront);
				}
			}
			else {
				m_pGSensorView->MoveWindow( this->m_geoPreserved[PRSGEO_VIDEO_FRONT].x, this->m_geoPreserved[PRSGEO_VIDEO_FRONT].y,
				this->m_geoPreserved[PRSGEO_VIDEO_FRONT].width, this->m_geoPreserved[PRSGEO_VIDEO_FRONT].height );
			}
		}

		m_pGSensorView->InvalidateCtrl();
//		LONGLONG nCurrentTime = this->m_pMultiPlayer->GetCurrentPosition();
//		m_pGSensorView->Synchronize( (DWORD)nCurrentTime );
	
		m_pGSensorView->ShowWindow(SW_SHOW);
	}
#endif
	return;
}

FLOAT CBBPlayerUIControl::GetSpeedFromPos( INT nPos )
{
	float fSpeed = 0.0f;

	switch(nPos)
	{
	case 1: fSpeed = 0.2f;	break;	case 2: fSpeed = 0.4f;	break;	case 3: fSpeed = 0.6f;	break;
	case 4: fSpeed = 0.8f;	break;	case 5: fSpeed = 1.0f;	break;	case 6: fSpeed = 1.25f;break;
	case 7: fSpeed = 1.5f;	break;	case 8: fSpeed = 1.75f;break;	case 9: fSpeed = 2.0f;	break;
	default: fSpeed = 1.0f;
	}

	return fSpeed;
}

VOID CBBPlayerUIControl::UserDataSynchronize(DWORD dwCurrentTime, BOOL bRedraw /* = FALSE */)
{
	ClWebBrowser* pMap = MAP_GET( TEXT("main.web.map") );
	ClButton* 		pBtnGpsPulse = BTN_GET( TEXT("main.button.gpspulse") );
	ClVideoView*		pVideoCtrl= VID_GET( TEXT("main.video.quadctrl"));
	
	DWORD dwConnectionTypes;
	BOOL internet_use = 0;

		if(/*!m_pSensorData->m_bI3FileType &&*/ dwCurrentTime > 800)
			dwCurrentTime -= 800; //offset ??? 녹화 화면하고 시간차가 발생하여 임시로 넣었습니다.

		else 
			dwCurrentTime = 0;

////++{ 20170226 PulseData add
	if( this->m_pSensorData->m_PulseList.size() > 0)
	{
		static DWORD last_cts;
		CTextData::ITER_PULSE iPULSE	= this->m_pSensorData->m_PulseList.begin();
		CTextData::ITER_PULSE iPULEnd	= this->m_pSensorData->m_PulseList.end();

		ClButton* 		pBtntacho1 	= BTN_GET( TEXT("main.button.tacho1") );
		//ClButton* 		pBtntacho2 	= BTN_GET( TEXT("main.button.tacho2") );
		//ClButton* 		pBtntacho3 	= BTN_GET( TEXT("main.button.tacho3") );
		ClButton* 		pBtntacho4 	= BTN_GET( TEXT("main.button.tacho4") );
		ClButton* 		pBtntacho5 	= BTN_GET( TEXT("main.button.tacho5") );
		
		
		for( ; iPULSE != iPULEnd; iPULSE++ )
		{
			if( dwCurrentTime <= iPULSE->m_cts)
				break;
		}

		if(iPULSE == iPULEnd && iPULSE != this->m_pSensorData->m_PulseList.begin())
			iPULSE --;

		if( this->m_pSensorData->m_bPulseSpeedEnable && iPULSE->m_bPulseState)
		{
			if( this->m_pSensorData->m_bPulseSpeedUse == FALSE ){
				if(pBtnGpsPulse) pBtnGpsPulse->SetCheckState(TRUE);
				this->m_pSensorData->m_bPulseSpeedUse = TRUE;
			}
		}
		else if(this->m_pSensorData->m_bPulseSpeedUse) {
			if(pBtnGpsPulse) pBtnGpsPulse->SetCheckState(FALSE);
			this->m_pSensorData->m_bPulseSpeedUse = FALSE;
		}			

		if(iPULSE->m_cts < dwCurrentTime){
			
			if(pBtntacho1) pBtntacho1->SetEnable(0);
			if(pBtntacho4) pBtntacho4->SetEnable(0);
			if(pBtntacho5) pBtntacho5->SetEnable(0);

			m_bTachoDisplayEnable = FALSE;
		}
		else if(((iPULSE != iPULEnd  && last_cts != iPULSE->m_cts) || bRedraw)){
			m_TachoDisplayPULSE = *iPULSE;
			m_bTachoDisplayEnable = TRUE;
			
			if(pBtntacho1) pBtntacho1->SetEnable(iPULSE->m_bBrk);
			//if(pBtntacho2) pBtntacho2->SetCheckState(iPULSE->m_bSR);
			//if(pBtntacho3) pBtntacho3->SetCheckState(iPULSE->m_bSL);
			if(pBtntacho4) 
				pBtntacho4->SetEnable(iPULSE->m_bBgr);
			if(pBtntacho5) 
				pBtntacho5->SetEnable(iPULSE->m_bTR);
			  //pBtntacho5->SetEnable(iPULSE->m_bBgr ? 0 : iPULSE->m_bTR);     // jun 170622 4번 on 시 -> 5번 off
			 

			
			CString text;

			if(this->m_pSensorData->m_bPulseSpeedUse && iPULSE->m_bPulseState) {
				CDigitNum* pDigit		= DIGIT_GET(TEXT("main.digit.speed"));
				
				if(pDigit) {
				//	pDigit->SetUnit(UNIT_ENUM_END); // pulse km/h
					pDigit->DrawDigits((int)iPULSE->m_fPulseSpeed , bRedraw);
				//	pDigit->Invalidate();
				}
			}

#if 0
			if(this->m_pSensorData->m_bPulseSpeedUse && iPULSE->m_bRpmConnectionState) {
				CDigitNum* pDigit		= DIGIT_GET(TEXT("main.digit.rpm"));
				
				if(pDigit) {
					pDigit->DrawDigits((int)iPULSE->m_iRpm , bRedraw);
				}
			}
#else
			if(this->m_pSensorData->m_bPulseSpeedUse && iPULSE->m_bRpmConnectionState) 
			{
				CAnalogMeter* pAMeter		= AMETER_GET(TEXT("main.analogmeter.rpm")); 
				
				if(pAMeter) {
					pAMeter->UpdateNeedle((double)iPULSE->m_iRpm , bRedraw);
				}
			}
#endif

#if 0// test code
			ClText* 			pTextLatitude	= TXT_GET( TEXT("main.text.latitude"));
			
			text.Format(TEXT("%d %d %d %d %d %d"), (int)iPULSE->m_fPulseSpeed,  iPULSE->m_bBrk, iPULSE->m_bSR, iPULSE->m_bSL, iPULSE->m_bBgr, iPULSE->m_bTR);
			if(pTextLatitude) pTextLatitude->SetWindowText(text);
#endif

			last_cts = iPULSE->m_cts ;
		}
	}
	else if(dwCurrentTime > 1000 && this->m_pSensorData->m_bPulseSpeedUse) {
		if(pBtnGpsPulse) pBtnGpsPulse->SetCheckState(FALSE);
		this->m_pSensorData->m_bPulseSpeedUse = FALSE;
	}			

////++}

	PlayTimeProgress(dwCurrentTime);
	
	if( this->m_nCurMode == 0 ||  this->m_ActiveFront == ITEM_MAP || this->m_MaxScreenActive == ITEM_MAP)
	{
		if( pMap && InternetGetConnectedState(&dwConnectionTypes, 0) )
		{
			if( pMap->IsInternetConnected() )
			{
				internet_use = 1;
			}
			else
			{
				DWORD dwConnectionTypes;
				if( InternetGetConnectedState(&dwConnectionTypes, 0) )
				{
					CString strPath = ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getMapPath();
					pMap->Navigate(strPath, 0,0,0,0);
					pMap->SetInternetConnected();
					internet_use = 1;
				}
			}
		}

		if(pVideoCtrl && pVideoCtrl->GetQuadViewMode()) {
			if(pMap)	pMap->MoveWindow( 0, 0, 0, 0 );
			internet_use = FALSE; // Quadview mode에서는 Map을 표시하지 않음
		}

		if(pMap) pMap->Synchronize( internet_use, dwCurrentTime, bRedraw );
	}

	CGSensorView* m_pGSensorView = GSG_GET( TEXT("main.gsensor.graph") );
	if(m_pGSensorView)	m_pGSensorView->Synchronize( dwCurrentTime );
	
}

VOID CBBPlayerUIControl::PlayTimeProgress(DWORD dwCurrentTime)
{
	ClText* pTxtFile = TXT_GET( TEXT("main.text.file") );
	if( pTxtFile )
	{
		ClListCtrl* pFileList = LST_GET( TEXT("main.listbox.filelist") );

		if(pFileList){
			CString cstrFileNo;
			CString cstrFileTime;
			int index = pFileList->m_nSelectItemNo;
#if 1			
			CTime date(pFileList->m_dwfile_time + dwCurrentTime/1000);
	//		CString strTime;  // jun 170602
	


			
	//		cstrFileTime.Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), date.GetYear(), date.GetMonth(), date.GetDay(), \
	//		date.GetHour(), date.GetMinute(), date.GetSecond());
			cstrFileTime.Format(_T("%04d/%02d/%02d"), date.GetYear(), date.GetMonth(), date.GetDay());      // jun 200805 time del
			
#else

			if( this->m_pSensorData->m_GSensorList.size() > 0)
			{
				CTextData::ITER_GSENSOR iGSensor = this->m_pSensorData->m_GSensorList.begin();
				CTextData::ITER_GSENSOR iGSensorend = this->m_pSensorData->m_GSensorList.end();

				if(dwCurrentTime > 800)
					dwCurrentTime -= 800;
				
				for( ; iGSensor != iGSensorend; iGSensor++ )

				{
					if( dwCurrentTime <= iGSensor->m_cts)
						break;
				}

				if(iGSensor != iGSensorend){
					cstrFileTime.Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), 2000 + iGSensor->m_time[0], iGSensor->m_time[1] + 1, iGSensor->m_time[2], \
						iGSensor->m_time[3], iGSensor->m_time[4], iGSensor->m_time[5]);
				}
			}
#endif

			cstrFileNo.Format(TEXT("    [%d/%d]"), index + 1,  pFileList->GetItemCount());

		
			cstrFileTime += cstrFileNo;
		
			pTxtFile->SetWindowText( cstrFileTime );
		}
	}
}

VOID CBBPlayerUIControl::Redraw(VOID)
{
#if BUILD_MODEL == MODEL_RALLY_EYES
	if( this->m_nCurMode == 0 )
	{
		ClWebBrowser* pSponsor = MAP_GET( TEXT("main.web.sponsor") );
		if(pSponsor)  pSponsor->MoveWindow(  this->m_geoPreserved[PRSGEO_WEB_SPONSOR].x, this->m_geoPreserved[PRSGEO_WEB_SPONSOR].y,
					this->m_geoPreserved[PRSGEO_WEB_SPONSOR].width, this->m_geoPreserved[PRSGEO_WEB_SPONSOR].height );
	}
#endif
}

UINT CBBPlayerUIControl::ThreadTachoDisplayProc(void* pArg)
{
	CBBPlayerUIControl* pDlg = (CBBPlayerUIControl*)pArg;
	LONG llTimeOffset = 0;
	BOOL sr = 0;
	BOOL sl = 0;

	if (pDlg == NULL)
	{
		pDlg->m_pTachoDisplayThread = NULL;
		return 0;
	}

	while(pDlg->m_bTachoDisplayThreadExit == FALSE)
	{
		int nPlayState = pDlg->m_pMultiPlayer->GetState();
		
		if( (pDlg->m_pSensorData->m_PulseList.size() > 0 && pDlg->m_bTachoDisplayEnable && nPlayState == PLAY_STATE_RUNNING) || sl || sr){
			ClButton* 		pBtntacho2 	= (ClButton*)pDlg->m_pDialogManager->GetControl(TEXT("main.button.tacho2") );
			ClButton* 		pBtntacho3 	= (ClButton*)pDlg->m_pDialogManager->GetControl(TEXT("main.button.tacho3") );

			if(llTimeOffset == 0 && pDlg->m_bTachoDisplayEnable && (pDlg->m_TachoDisplayPULSE.m_bSR || pDlg->m_TachoDisplayPULSE.m_bSL) ){
				llTimeOffset = 500 - (::GetTickCount() % 500);
			}
			
			if(((llTimeOffset + ::GetTickCount()) /500) % 2){
				sl = 0;
				sr = 0;
			}
			else if(pDlg->m_bTachoDisplayEnable){
				sr = pDlg->m_TachoDisplayPULSE.m_bSR;
				sl = pDlg->m_TachoDisplayPULSE.m_bSL;
			}

			if(pBtntacho2) pBtntacho2->SetEnable(sr);
			if(pBtntacho3) pBtntacho3->SetEnable(sl);
		}
		else {
			llTimeOffset = 0;
		}
				
		::Sleep(10);
	}

	pDlg->m_pTachoDisplayThread = NULL;
}
//================================================================================================= E. GET/SET
