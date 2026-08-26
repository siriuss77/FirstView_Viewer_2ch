//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClCludeControl.H
//
//	Description: CLENA CLUDE Parser for control
//
//	Update History
// 
//		2011.10.10 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. FILEID

#ifndef	__CORELOGC_CO_KR_CLENAW_CLTYPES_H__
#define	__CORELOGC_CO_KR_CLENAW_CLTYPES_H__

//================================================================================================= E. FILEID
//================================================================================================= S. INCLUDE

#include <stdafx.h>
#include <HELPER/ClGeometry.H>

//================================================================================================= E. INCLUDE
//================================================================================================= S. PREPROCESSOR

// Reserved Message range 0 ... 99
//--------------------------------------------------------------------------------------- S.MSG_RANGE
#define	CLENAW_MSG_BASE									( WM_USER +   0	)
#define	CLENAW_MSG_MAX									( WM_USER +  99	)
//--------------------------------------------------------------------------------------- E.MSG_RANGE
//--------------------------------------------------------------------------------------- S.CONTROLMESSAGE

#define WM_CLENAW_SLIDER								( CLENAW_MSG_BASE + 0 )

//--------------------------------------------------------------------------------------- E.CONTROLMESSAGE

#define CLENAW_MSG_USER									( WM_USER + 100	)	// Customer Message id range.

//================================================================================================= E. PREPROCESSOR
//================================================================================================= S. TYPEDEF

enum CONTROL_TYPE
{
	TYPE_IMAGE		= 0	,
	TYPE_TEXT			,
	TYPE_BUTTON			,
	TYPE_SLIDER			,
	TYPE_LISTBOX		,
	TYPE_VIDEO			,
	TYPE_WEB			,
	TYPE_GSENSOR		,
	TYPE_DIGIT			,
	TYPE_AMETER		,
	TYPE_INVALID
};

typedef enum GEOBASE
{
	GEOX_LEFT	= 1 << 0,
	GEOX_CENTER	= 1 << 1,
	GEOX_RIGHT	= 1 << 2,

	GEOY_TOP	= 1 << 4,
	GEOY_MIDDLE	= 1 << 5,
	GEOY_BOTTOM	= 1 << 6,

	GEO_PERCENT	= 1 << 15,
	GEOW_RATIO	= 1 << 16,
	GEOH_RATIO	= 1 << 17,
	GEOW_MARGIN	= 1 << 18,
	GEOH_MARGIN	= 1 << 19
};

typedef struct _tagMODEINFO
{
	DWORD		m_dwGeoBase;
	ClGeometry	m_geometry;
	BOOL		m_bShow;
	BOOL		m_bEnable;

} MODEINFO, *PMODEINFO;

enum _PlaybackState
{
	STATE_RUNNING = 0x00,
	STATE_PAUSED,
	STATE_STOPPED,
	STATE_CLOSED,
};

//================================================================================================= E. TYPEDEF
#endif	//__CORELOGC_CO_KR_CLENAW_CLTYPES_H__

