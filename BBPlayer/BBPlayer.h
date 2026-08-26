//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: BBPlayer.H
//
//	Description:
//
//	Update History
// 
//		2012.01.12 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S.FILEID

#pragma once

//================================================================================================= E.FILEID

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

//================================================================================================= S.INCLUDE

#include "resource.h"		// main symbols

#include <CLENAW/ClTypes.H>
#include <CLUDE/ClCludeApp.H>
#include "BBConfigure.h"

//================================================================================================= E.INCLUDE
//================================================================================================= S.PREPROCESSOR

#define	WM_USER_DRIVECHANGED								( CLENAW_MSG_USER + 100 )
#define	WM_USER_MEDIAFORCESTOP								( CLENAW_MSG_USER + 101 )
#define	WM_USER_ADDFILE											( CLENAW_MSG_USER + 102 )

#define STR_MUTEX_NAME										TEXT("CORELOGIC.CLENAW.BBPLAYER")
#define STR_CLASS_NAME										TEXT("CORELOGIC.CLENAW.BBPLAYER")		// WARNING : STR_CLASS_NAME must be same with 'CLASS' property in CBBPlayer.rc.

//================================================================================================= E.PREPROCESSOR
//================================================================================================= S.CLASS

class CBBPlayerApp : public CWinApp
{
public:
	CBBPlayerApp();

	public:
	virtual BOOL			InitInstance		( VOID );
	virtual int				ExitInstance		( VOID );
	ClCludeApp*				GetGlobalSetting	( VOID );
	CBBConfigure*			GetConfigure		( VOID );

	DECLARE_MESSAGE_MAP()

private:

	ULONG_PTR				m_gdiplusToken;
	ClCludeApp				m_AppGlobalSetting;
	CBBConfigure			m_Configure;
};

extern CBBPlayerApp theApp;

//================================================================================================= E.CLASS
