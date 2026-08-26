//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: BBPlayerUIControl.H
//
//	Description: BBPlayer UI controller
//
//	Update History
// 
//		2011.11.23 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. FILEID

#pragma once

//================================================================================================= E. FILEID
//================================================================================================= S.INCLUDE

#include "resource.h"
#include <CLENAW/ClSlider.h>
#include <CLENAW/ClText.h>
#include <CLENAW/DigitNum.h>
#include <CLENAW/AnalogMeter.h>
#include <CLENAW/ClDialogManager.H>
#include <BLACKBOX/MultiPlayer.h>
#include <BLACKBOX/Language.h>
#include <BLACKBOX/TextData.h>
#include <HELPER/ClGeometry.H>
//================================================================================================= E.INCLUDE
//================================================================================================= S.PREPROCESSOR

#define	WND_GET(name)					this->m_pDialogManager->GetControl(name)

#define TXT_GET(name)					(ClText*)this->m_pDialogManager->GetControl(name)
#define IMG_GET(name)					(ClImage*)this->m_pDialogManager->GetControl(name)
#define BTN_GET(name)					(ClButton*)this->m_pDialogManager->GetControl(name)
#define SLD_GET(name)					(ClSlider*)this->m_pDialogManager->GetControl(name)
#define LST_GET(name)					(ClListCtrl*)this->m_pDialogManager->GetControl(name)
#define VID_GET(name)					(ClVideoView*)this->m_pDialogManager->GetControl(name)
#define GSG_GET(name)					(CGSensorView*)this->m_pDialogManager->GetControl(name)
#define MAP_GET(name)					(ClWebBrowser*)this->m_pDialogManager->GetControl(name)
#define DIGIT_GET(name)					(CDigitNum*)this->m_pDialogManager->GetControl(name)
#define AMETER_GET(name)					(CAnalogMeter*)this->m_pDialogManager->GetControl(name)
//================================================================================================= E.PREPROCESSOR
//================================================================================================= S.CLASS

class CBBPlayerUIControl
{

public:
	CBBPlayerUIControl(void);
	~CBBPlayerUIControl(void);

	enum BBPLAYER_UISTATE
	{
		UISTATE_INIT		= 0	,
		UISTATE_SELDRIVE		,
		UISTATE_MEDIA			,
		UISTATE_INVALID
	};

	enum PRESERVED_GEOMETRY
	{
		PRSGEO_VIDEO_FRONT	= 0	,
		PRSGEO_VIDEO_REAR		,
		PRSGEO_WEB_MAP		,
		PRSGEO_LISTBOX_FILELIST	,
		PRSGEO_GSENSOR_GRAPH	,
		PRSGEO_WEB_SPONSOR	,
		PRSGEO_BTN_FIRST		,
		PRSGEO_BTN_SECOND	,
		PRSGEO_INVALID
	};

	enum UI_EVENT
	{
		EVENT_INIT			= 0	,
		EVENT_MODE_CHANGE		,	// INIT, SELDRIVE, RUNNING
		EVENT_SEL_DRIVE			,	// INIT
		EVENT_DROP_DRIVE		,	// SELDRIVE, RUNNING

		EVENT_MEDIA_PLAY		,	// MEDIA
		EVENT_MEDIA_REPLAY		,	// MEDIA
		EVENT_MEDIA_PAUSED		,	// MEDIA
		EVENT_MEDIA_STOPPED		,	// MEDIA
		EVENT_MEDIA_PROGRESS	,	// MEDIA
		
		EVENT_FRCHANGE			,
		EVENT_FMCHANGE		,

		EVENT_ZOOM			,
		EVENT_MUX			,


		EVENT_LIST_NORMAL		,
		EVENT_LIST_EVENT		,
		EVENT_LIST_MANUAL		,
		EVENT_LIST_MOTION		,

		EVENT_SEL_MODEL
	};

    typedef enum _eDIRTYPE
    {
        DIR_TYPE_REC    = 0 ,
        DIR_TYPE_EVENT      ,
        DIR_TYPE_MOTION     ,
        DIR_TYPE_MANUAL     ,
        DIR_TYPE_INVALID

    } EDIR_TYPE, *PEDIR_TYPE;

	enum ACTIVE_ITEM
	{
		ITEM_FRONT_VIDEO	= 0	,
		ITEM_REAR_VIDEO		,
		ITEM_MAP				,
		ITEM_MUXVIEW_VIDEO	,
		ITEM_MUXCTRL_VIDEO	,
		
		ITEM_NONE
	};


public:

			BOOL					SetBBPlayerProp		( ClDialogManager* pDialogManager, CMultiPlayer* pMultiPlayer, CTextData* pSensorData );
			VOID 					InitTooltip		( VOID );
			VOID 					InitLanguage				(VOID);
			VOID					InitExchangeGeometry( VOID );
			VOID					InitBBPlayerRes		( VOID );

public:

			VOID					SendUIEvent			( UI_EVENT event );
			VOID					SetStreamInfo			( INT nChannel, BOOL bIsGPS, BOOL bIsGsensor );
			VOID					SetLogFileExist			(BOOL exist) { this->m_bIsLogFileExist = exist; }
			VOID					SetNxfsFileExist			(BOOL exist) { this->m_bIsNxfsFileExist = exist; }
			FLOAT					GetSpeedFromPos	( INT nPos );
			VOID 					UserDataSynchronize(DWORD dwCurrentTime, BOOL bRedraw = FALSE);
			VOID					SetActiveItem		( BOOL IsGpsData = FALSE );
			VOID 					PlayTimeProgress(DWORD dwCurrentTime);
			VOID					Redraw				( VOID );
			static UINT	ThreadTachoDisplayProc(void* pArg);
protected:

			VOID					OnEventInit			( VOID );
			VOID					OnEventModeChange	( VOID );
			VOID					OnEventSelectDrive	( VOID );
			VOID					OnEventDropDrive	( VOID );

			VOID					OnEventMediaPlay	( VOID );
			VOID					OnEventMediaReplay	( VOID );
			VOID					OnEventMediaPause	( VOID );
			VOID					OnEventMediaStop	( VOID );
			VOID					OnEventMediaProgress( VOID );

			VOID					OnEventFrontInFront	( VOID );
			VOID					OnEventFrontInRear	( VOID );
			VOID					OnEventFrontInMap	( VOID );
			VOID					OnEventRearInFront	( VOID );
			VOID					OnEventRearInRear	( VOID );
			VOID					OnEventRearInMap	( VOID );
			VOID					OnEventFRChange		( VOID );
			VOID					OnEventFMChange		( VOID );
			VOID					OnEventZoom				( VOID );
			VOID					OnEventMux				( VOID );
			
			VOID					OnEventListSort		( INT nType );

			VOID					OnEvnetSelectModel	( VOID );

			VOID					SetUIState			( BBPLAYER_UISTATE state );
			VOID					ActivateInitMode	( VOID );
			VOID					ActivateSelectedDriveMode( VOID );
			VOID					ActivatePlayMode	( VOID );

public:
			INT						GetStreamChannel	( VOID );
			BOOL					GetStreamGPS		( VOID );
			BOOL					GetStreamGSensor	( VOID );

			UI_EVENT				GetListTabPrevEvent	( VOID );
			UI_EVENT				GetMediaStatus(VOID) { return m_eMediaStatus; };

protected:
			VOID 					ButtonTooltipSet( const WCHAR* pButtonName, INT Index);
			VOID 					SliderTooltipSet( const WCHAR* pSliderName, INT Index) ;
			VOID 					ImageTooltipSet( const WCHAR* pImageName, INT Index) ;


public:
			ACTIVE_ITEM				m_ActiveFront;
			ACTIVE_ITEM				m_ActiveRear;
			ACTIVE_ITEM				m_ActiveMap;
			ACTIVE_ITEM				m_MaxScreenActive;
			CLanguage				m_Language;

			CPulseData			m_TachoDisplayPULSE;
			BOOL 					m_bTachoDisplayEnable;

			CRect					m_rWindowRect;
private:

			ClDialogManager*		m_pDialogManager;
			CMultiPlayer*			m_pMultiPlayer;
			CTextData* 			m_pSensorData;
			INT						m_nStreamChannel;
			INT						m_nPreStreamChannel;
			BOOL					m_bIsStreamGPS;
			BOOL					m_bIsStreamGSensor;
			BOOL					m_bIsLogFileExist;
			BOOL					m_bIsNxfsFileExist;
			INT						m_nCurMode;
			BBPLAYER_UISTATE		m_eCurUIState;

			ClSlider* 				m_pSldTime;
			ClText* 					m_pTextDuration;
			ClText* 					m_pTextCurTime;
			ClText* 					m_pTextCurFileTime;
			
			UI_EVENT				m_eSelectedListTab;
			UI_EVENT				m_eMediaStatus;

			ClGeometry				m_geoPreserved[PRSGEO_INVALID];

			BOOL					m_bTachoDisplayThreadExit;
			CWinThread* 			m_pTachoDisplayThread;

};
//================================================================================================= E.CLASS
