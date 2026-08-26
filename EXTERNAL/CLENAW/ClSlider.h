//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClSlider.H
//
//	Description: CLENA-W Slider control
//
//	Update History
// 
//		2011.11.23 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. FILEID

#pragma once

//================================================================================================= E. FILEID
//================================================================================================= S. INCLUDE

#include <CLENAW/ClIBGTransfer.h>
#include <HELPER/ClGeometry.H>
#include <HELPER/ClHashedString.H>
#include <HELPER/ClDrawImage.H>
#include <HELPER/ClGeometry.H>

#include <list>

//================================================================================================= E. INCLUDE
//================================================================================================= S. PREPROCESSOR

//================================================================================================= E. PREPROCESSOR
//================================================================================================= S. CLASS

class ClSlider : public CStatic, IBGTransfer
{
	DECLARE_DYNAMIC(ClSlider)

public:
	ClSlider();
	virtual ~ClSlider();

	//----------------------------------------------------------------------------------- S. TYPEDEF

	enum SKIN_TYPE
	{
		SKIN_TICS			= 0	,
		SKIN_BTRACK				,
		SKIN_TRACK				,
		SKIN_THUMB				,
		SKIN_ATHUMB				,
		SKIN_INVALID
	};

	typedef std::list<ClDrawImage*>						BG_POOL;
	typedef BG_POOL::iterator							ITER_BG;

	//----------------------------------------------------------------------------------- E. TYPEDEF

public:

			BOOL					SetSkin				( SKIN_TYPE type, const HashedString& imagePath,
															Color color = Color((ARGB)Color::White),
															BOOL bUseTpColor = FALSE, Color tpColor = Color((ARGB)Color::Black),
															ClDrawImage::DRAW_MODE drawMode = ClDrawImage::MODE_STRETCH,
															const ClGeometry& geoImage = GEO_FULL_SIZE,
															const ClGeometry& geoSubImage = GEO_FULL_SIZE,
															DWORD dwGeoBase = GEOX_LEFT | GEOY_TOP );
			ClDrawImage*			GetSkin				( SKIN_TYPE type );
			VOID					ClearSkins			( VOID );

			BOOL					SetBackground		( const HashedString& imagePath,
															Color color = Color((ARGB)Color::White),
															BOOL bUseTpColor = FALSE, Color tpColor = Color((ARGB)Color::Black),
															ClDrawImage::DRAW_MODE drawMode = ClDrawImage::MODE_STRETCH,
															const ClGeometry& geoImage = GEO_FULL_SIZE,
															const ClGeometry& geoSubImage = GEO_FULL_SIZE,
															DWORD dwGeoBase = GEOX_LEFT | GEOY_TOP );
			ClDrawImage*			GetBackground		( INT nIndex );
			VOID					ClearBackgrounds	( VOID );

public:

	virtual void					DrawItem			( LPDRAWITEMSTRUCT lpDrawItemStruct );
	virtual VOID					TransferBackground	( CDC* pDC );
	virtual	BOOL				PreTranslateMessage(MSG* pMsg);

public:

			void					cludeSetProperty	( int nMin, int nMax, int nPos, int nPageSize, const CRect& rtMargin );

			void					GetRange			( int &nMin, int &nMax );
			void					SetRange			( int nMin, int nMax, BOOL bRedraw = FALSE );

			int						GetRangeMax			( void );
			void					SetRangeMax			( int nMax, BOOL bRedraw = FALSE );

			int						GetRangeMin			( void );
			void					SetRangeMin			( int nMin, BOOL bRedraw = FALSE);

			int						GetPos				( void );
			void					SetPos				( int nPos );

			int						SetPageSize			( int nSize );
			int						GetPageSize			( void );

			void					SetVertical			( BOOL bVertical = TRUE );
			BOOL					IsVertical			( void );
			
			VOID 					SetTooltipText			(LPCTSTR lpszText, BOOL bActivate = TRUE);
			VOID					ActivateTooltip		(BOOL bActivate);

protected:
			void 					InitToolTip			( VOID );

			BOOL					CalcGeometry		( ClGeometry& geoTrack, ClGeometry& geoSrcTrack, ClGeometry& geothumb );
			VOID					GetTrackArea		( ClGeometry& geoTrack );

public:

	afx_msg	void					OnPaint				();
	afx_msg	BOOL					OnEraseBkgnd		( CDC* pDC );
	afx_msg void					OnLButtonDown		( UINT nFlags, CPoint point );
	afx_msg void					OnLButtonUp			( UINT nFlags, CPoint point );
	afx_msg void					OnMouseMove			( UINT nFlags, CPoint point );
	afx_msg void					OnKeyDown			( UINT nChar, UINT nRepCnt, UINT nFlags );


protected:

	DECLARE_MESSAGE_MAP()


protected:
			ClDrawImage*			m_pSkinList[SKIN_INVALID];
			BG_POOL					m_BackgroundList;
			int						m_nMax;
			int						m_nMin;
			int						m_nPos;
			int						m_nPage;
			double					m_dbPagePixel;

			int						m_nInitMax, m_nInitMin, m_nInitPos, m_nInitPage;

			CRect					m_rtMargin;
			CRect					m_rtPrevRect;

			BOOL					m_bIsVertical;
			BOOL					m_bIsButtonDown;
			INT						m_nMouseOffset;
			ClGeometry				m_geoThumb;

			CToolTipCtrl 				m_ToolTip; // tony add
};

//================================================================================================= E. CLASS
