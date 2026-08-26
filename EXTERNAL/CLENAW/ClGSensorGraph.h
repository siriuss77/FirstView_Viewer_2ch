//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClGSensorGraph.h
//
//	Description: G-Sensor graph control
//
//	Update History
// 
//		2011.10.10 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. FILEID

#pragma once

//================================================================================================= E. FILEID
//================================================================================================= S. INCLUDE

#include <stdafx.h>
#include <CLENAW/ClIBGTransfer.h>
#include <HELPER/ClDrawImage.H>
#include <BLACKBOX/TextData.H>

//================================================================================================= E. INCLUDE
//================================================================================================= S. CLASS

class ClGSensorGraph : public CStatic, IBGTransfer
{
	DECLARE_DYNAMIC(ClGSensorGraph)

public:
	ClGSensorGraph();
	virtual ~ClGSensorGraph();

	//----------------------------------------------------------------------------------- S.TYPEDEF

	typedef struct _tagSTLINE
	{
		Color	m_color;
		INT		m_nWidth;

	} LINE_INFO, *PLINEINFO;

	enum LINE_TYPE
	{
		LINE_X		= 0	,
		LINE_Y			,
		LINE_Z			,
		LINE_TIME		,
		LINE_INVALID
	};
	//----------------------------------------------------------------------------------- E.TYPEDEF

protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual	void					DrawItem			( LPDRAWITEMSTRUCT lpDrawItemStruct );
	virtual	void					TransferBackground	( CDC* pDC );

			VOID					SetCTS				( DWORD dwCTS );
			BOOL					SetSensorData		( CTextData* pSensorData );

			BOOL					SetLine				( LINE_TYPE type, Color color, INT nWidth );
			VOID					SetTimeLineProperty	( INT nStartPos, INT nEndPos, INT nTop, INT nHeight );

			BOOL					SetBackground		( const HashedString& imagePath, Color color = Color((ARGB)Color::White),
															BOOL bUseTpColor = FALSE, Color tpColor = Color((ARGB)Color::Black),
															ClDrawImage::DRAW_MODE drawMode = ClDrawImage::MODE_STRETCH,
															const ClGeometry& geoImage = GEO_FULL_SIZE, const ClGeometry& geoSubImage = GEO_FULL_SIZE,
															DWORD dwGeoBase = GEOX_LEFT | GEOY_TOP );
			ClDrawImage*			GetBackground		( VOID );

protected:

			void					DrawTimeLine		( CDC * pDC, DWORD dwCTS );
			void					DrawGraph			( CDC* pDC );

protected:

			CDC						m_dcGraph;
			ClDrawImage*			m_pBackground;
			LINE_INFO				m_LineInfo[LINE_INVALID];

			INT						m_nTimeLineProp[4];

			DWORD					m_dwCTS;
			CTextData*				m_pSensorData;
};
//================================================================================================= E. CLASS
