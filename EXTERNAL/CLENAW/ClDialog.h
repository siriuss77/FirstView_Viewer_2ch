//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClDialog.H
//
//	Description: CLENAW Dialog control
//
//	Update History
// 
//		2011.11.23 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. FILEID

#pragma once

//================================================================================================= E. FILEID
//================================================================================================= S. INCLUDE

#include <HELPER/ClDrawImage.H>
#include <CLENAW/ClDialogManager.H>

#include <string>

//================================================================================================= S. INCLUDE
//================================================================================================= S. PREPROCESSOR

#define	WND_GET(name)					this->m_pDialogManager->GetControl(name)

#define TXT_GET(name)					(ClText*)this->m_pDialogManager->GetControl(name)
#define IMG_GET(name)					(ClImage*)this->m_pDialogManager->GetControl(name)
#define BTN_GET(name)					(ClButton*)this->m_pDialogManager->GetControl(name)
#define SLD_GET(name)					(ClSlider*)this->m_pDialogManager->GetControl(name)
#define LST_GET(name)					(ClListCtrl*)this->m_pDialogManager->GetControl(name)
#define VID_GET(name)					(ClVideoView*)this->m_pDialogManager->GetControl(name)
#define GSG_GET(name)					(CGSensorView*)this->m_pDialogManager->GetControl(name)
#define MAP_GET(name)					(ClWebBrowser*)this->m_pDialogManager->GetControl(name)

#define RESET_RESSTATE( MODE, RES_NAME, BSHOW, BENABLE )										\
	this->m_pDialogManager->SetResourceState( MODE, RES_NAME, BSHOW, BENABLE );

//================================================================================================= E. PREPROCESSOR
//================================================================================================= S. CLASS

class ClDialog : public CDialog
{
	DECLARE_DYNAMIC(ClDialog)

public:

	ClDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	virtual ~ClDialog();


protected:

	virtual void					DoDataExchange		( CDataExchange* pDX );

	DECLARE_MESSAGE_MAP()

public:

	virtual	VOID					InitResourceID		( VOID );
			BOOL					OpenClude			( const std::string& cludePath );
			BOOL					OpenCludeBuffer		( const char* pBuffer );
			VOID					SetCurrentMode		( INT nMode );
			INT						GetCurrentMode		( VOID );
			VOID					EnableEasyMove		( BOOL bEasyMoveEnable );
			BOOL					IsEasyMove			( VOID );

			CDC*					GetBackground		( VOID );

public:

	virtual	BOOL					OnInitDialog		( VOID );

	afx_msg	void					OnLButtonDown		( UINT nFlags, CPoint point );
	afx_msg	BOOL					OnEraseBkgnd		( CDC* pDC );


protected:

			ClDialogManager*		m_pDialogManager;

			CDC						m_dcBackground;
			BOOL					m_bIsBgUpdate;

			INT						m_nCurMode;

			BOOL					m_bEasyMove;
};

//================================================================================================= E. CLASS
