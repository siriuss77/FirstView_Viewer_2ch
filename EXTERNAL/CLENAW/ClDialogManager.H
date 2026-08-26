//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClDialogManager.h
//
//	Description: CLENAW Dialog Manager
//
//	Update History
// 
//		2011.10.10 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. FILEID

#pragma once

//================================================================================================= E. FILEID
//================================================================================================= S. INCLUDE

#include <CLENAW/ClContainer.H>
#include <CLENAW/ClTypes.H>
#include <HELPER/ClHashedString.H>

#include <string>
#include <map>

//================================================================================================= E. INCLUDE
//================================================================================================= S. PREPROCESSOR

//================================================================================================= E. PREPROCESSOR
//================================================================================================= S. TYPEDEF

//================================================================================================= E. TYPEDEF
//================================================================================================= S. CLASS

class ClDialog;

class ClDialogManager
{

public:

	ClDialogManager();
	~ClDialogManager();

	//-------------------------------------------------------------------------------------- S. TYPEDEF

	typedef struct _tagCONTROLINFO
	{
		CONTROL_TYPE	m_type;
		UINT			m_nID;
		ClContainer*	m_pContainer;

	} CONTROL_INFO, *PCONTROL_INFO;

	typedef std::map<HashedString,CONTROL_INFO>			CONTROLINFO_POOL;
	typedef CONTROLINFO_POOL::iterator					ITER_CONTROLINFO;
	typedef std::pair<HashedString,CONTROL_INFO>		PAIR_CONTROLINFO;

	typedef struct _tagBGINFO
	{
		INT				m_nMode;
		ClDrawImage*	m_pBgImage;

	} BGINFO, *PBGINFO;

	typedef std::list<BGINFO>							BG_POOL;
	typedef BG_POOL::iterator							ITER_BG;

	//-------------------------------------------------------------------------------------- E. TYPEDEF

// operation
public:

			BOOL					SetResourceState	( INT nMode, const HashedString& resourceName, BOOL bShow = TRUE, BOOL bEnable = TRUE );
			VOID					AdaptResourceState	( INT nMode );

			VOID					TransferBackground	( CDC* pDC );

// control
public:

			BOOL					AddResource			( const HashedString& resourceName, CONTROL_TYPE type, UINT nID );
			BOOL					AddContainer		( ClContainer* pContainer );
			BOOL					RemoveResource		( const HashedString& resourceName );
			VOID					ClearResource		( VOID );

// get/set
public:

			BOOL					SetParentDialog		( ClDialog* pParentDialog );

			BOOL					SetTitle			( const char* lpszTitle );
			const WCHAR*			GetTitle			( VOID );

			BOOL					SetGeometry			( const ClGeometry& geoDialog );
			const ClGeometry&		GetGeometry			( VOID )	{ return this->m_geoDialog; }

			BOOL					SetBackground		( INT nMode, const HashedString& ImagePath, const Color& color = Color((ARGB)Color::White),
															BOOL bUseTpColor = FALSE, const Color& tpColor = Color((ARGB)Color::Black),
															ClDrawImage::DRAW_MODE drawMode = ClDrawImage::MODE_STRETCH,
															const ClGeometry& geoImage = GEO_FULL_SIZE,
															const ClGeometry& geoSubImage = GEO_FULL_SIZE,
															DWORD dwGeoBase = GEOX_LEFT | GEOY_TOP );
			ClDrawImage*			GetBackground		( INT nMode, INT nIndex );
			INT						GetModeBgCount		( INT nMode );
			VOID					ClearBackground		( VOID );

			BOOL					IsEmptyResourceList	( VOID );
			INT						GetResourceListSize	( VOID );
			PCONTROL_INFO			GetResource			( CONST HashedString& resourceName );
			PCONTROL_INFO			GetResource			( INT nIndex );
			CWnd*					GetControl			( const HashedString& resourceName );


private:

			ClDialog*				m_pParentDialog;
			BG_POOL					m_BgList;

			std::wstring			m_wtsTitle;

			BYTE					m_alpha;
			COLORREF				m_tpColor;
			BOOL					m_bUseTPColor;
			ClGeometry				m_geoDialog;

			CONTROLINFO_POOL		m_resourceList;
};

//================================================================================================= E. CLASS

