//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClDrawImage.h
//
//	Description: CLENAW GID+ image drawing class
//
//	Update History
// 
//		2011.10.10 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. FILEID

#pragma once

//================================================================================================= E. FILEID
//================================================================================================= S. INCLUDE

#include <CLENAW/ClMacro.H>
#include <CLENAW/ClTypes.H>
#include <HELPER/ClImagePool.H>
#include <HELPER/ClGeometry.H>
#include <HELPER/ClMemDC.H>

#include <list>

//================================================================================================= E. INCLUDE

//================================================================================================= S. PREPROCESSOR

//================================================================================================= E. PREPROCESSOR
//================================================================================================= S. TYPEDEF

//================================================================================================= E. TYPEDEF
//================================================================================================= S. CLASS

class ClDrawImage
{

public:
		ClDrawImage();
		~ClDrawImage();

		enum DRAW_MODE
		{
			MODE_STRETCH	= 0	,
			MODE_PATTERN		,
			MODE_BUTTONFACE		,
			MODE_MULTIPLE	,
			MODE_INVALID
		};


// operation
public:

			VOID					deinit				( VOID );


// operation
public:

			BOOL					loadImage			( const HashedString& imagePath, Color color = Color((ARGB)Color::White),
															bool bUseTpColor = FALSE, Color tpColor = Color((ARGB)Color::Black),
															DRAW_MODE drawMode = MODE_STRETCH, const ClGeometry& geoImage = GEO_FULL_SIZE,
															const ClGeometry& geoSubImage = GEO_FULL_SIZE, DWORD dwGeoBase = GEOX_LEFT | GEOY_TOP );
			VOID					drawImage			( CDC* pDC, const CRect& rtClient, ClGeometry* pGeoTarget = NULL, ClGeometry* pGeoSource = NULL );
			VOID					prepareImage		( CDC* pDC, const CRect& rtClient, CDC* pBackground = NULL, BOOL bUseMask = FALSE );


// get/set
public:

			BOOL					setGeometry			( const ClGeometry& geoImage );
			BOOL					setGeometry			( INT x, INT y, INT width, INT height );
			const ClGeometry&		getGeometry			( VOID );
			DWORD					getGeoBase			( VOID );

			CDC*					getImageDC			( VOID );
			VOID					setUpdate			( VOID );

protected:

			Image*					m_pImage;

			CDC						m_dcImage;
			BOOL					m_bIsPrepared;

			BOOL					m_bUseMask;

			Color					m_color;
			DRAW_MODE				m_drawMode;
			ClGeometry				m_geoImage;
			ClGeometry				m_geoSubImage;
			DWORD					m_dwGeoBase;
			HashedString			m_ImagePath;

			BOOL					m_bUseColorKey;
			Color					m_ColorKey;
};

//================================================================================================= E. CLASS


