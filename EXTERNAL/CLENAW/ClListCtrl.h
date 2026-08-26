//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClListCtrl.H
//
//	Description: CLENA-W List Control
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
#include <HELPER/ClDrawText.H>
#include <HELPER/ClDrawImage.H>
#include <HELPER/ClGeometry.H>
#include <HELPER/ClHashedString.H>
#include <HELPER/ClGdiFactory.H>

#include <map>

//================================================================================================= E. INCLUDE
//================================================================================================= S. CLASS

class ClListCtrl : public CListCtrl, IBGTransfer
{
	DECLARE_DYNAMIC(ClListCtrl)

public:
	ClListCtrl();
	virtual ~ClListCtrl();

	enum TEXT_TYPE
	{
		TEXT_NORMAL	= 0	,
		TEXT_SELECTED	,
		TEXT_INVALID
	};

	enum SORT_TYPE
	{
		SORT_NO  = 0,
		SORT_FILENAME,
		SORT_SIZE ,
		SORT_PATH,
		SORT_FILENAME2, //event first
		SORT_STR_DATE,

		SORT_END
	};

	enum SORT_MODE
	{
		SORT_DOWN	= 0	,
		SORT_UP	,
		SORT_UPDOWN	,
		SORT_INVALID
	};
	//-------------------------------------------------------------------------------------- S.TYPEDEF

	typedef std::map<INT,INT>							COLUMNINFO_POOL;
	typedef COLUMNINFO_POOL::iterator					ITER_COLUMNINFO;
	typedef std::pair<INT,INT>							PAIR_COLUMNINFO;

	//-------------------------------------------------------------------------------------- E.TYPEDEF

public:

			BOOL					IsColumnInfoListEmpty( VOID );
			INT						GetColumnInfoListSize( VOID );
			VOID					AddColumnInfo		( INT nIndex, INT nWidth );
			INT						GetColumnInfo		( INT nIndex );
			BOOL					RemoveColumnInfo	( INT nIndex );
			VOID					ClearColumnInfo		( VOID );

			VOID 					SetHeaderCheck		( BOOL bFlag );
			BOOL					SetDrawText			( const CString& cstrFontFace, INT nFontSize,
															DWORD dwformat = ClDrawText::FORMAT_CENTER|ClDrawText::FORMAT_VCENTER|ClDrawText::FORMAT_WORDELLIPSIS,
															const Color& color = Color((ARGB)Color::White), const Color& selColor = Color((ARGB)Color::White), const Color& evtColor = Color((ARGB)Color::White), const Color& manColor = Color((ARGB)Color::White), const Color& motColor = Color((ARGB)Color::White) );// jun 170511

			BOOL					SetBackground		( const HashedString& ImagePath, const Color& color = Color((ARGB)Color::White),
															BOOL bUseTpColor = FALSE, const Color& tpColor = Color((ARGB)Color::Black),
															ClDrawImage::DRAW_MODE drawMode = ClDrawImage::MODE_STRETCH,
															const ClGeometry& geoImage = GEO_FULL_SIZE,
															const ClGeometry& geoSubImage = GEO_FULL_SIZE,
															DWORD dwGeoBase = GEOX_LEFT | GEOY_TOP );
			ClDrawImage*			GetBackground		( VOID );

			VOID					SetItemHeight		( INT nHeight );
			INT						GetItemHeight		( VOID );

			VOID					AdaptItemHeight	( VOID );
			VOID					AdaptColumn		( VOID );

			BOOL 					Sort				(int nSubItem = 0, int nUpDown = 0 ) ;
			static int 			CALLBACK SortFunc0	(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) ;
			static int 			CALLBACK SortString	(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) ;
			static int 			CALLBACK SortFunc2	(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) ;
protected:
	DECLARE_MESSAGE_MAP()

public:

	virtual VOID					TransferBackground	( CDC* pDC );
	virtual void					DrawItem				( LPDRAWITEMSTRUCT lpDrawItemStruct );

	afx_msg BOOL				OnEraseBkgnd		( CDC* pDC );
	afx_msg BOOL 				OnMouseWheel		(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void 					OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	INT						m_nSelectItemNo;
	INT						m_nStartItemNo;   // jun 191023 shift choice
	DWORD					m_dwfile_time;
	INT 						m_nSortItem;
	
	CImageList				m_cbImageList;
	BOOL					m_bIsAllCheck;
protected:

			COLUMNINFO_POOL			m_ColumnInfoList;

			ClDrawImage*			m_pBackground;

			CImageList				m_imgList;
			INT						m_nItemHeight;

			CString					m_cstrFont;
			INT						m_nFontSize;
			Color					m_color[5]; //20170523 3 => 5
			DWORD					m_dwFormat;
			HFONT					m_hFont;

			// tony add
			INT						m_nSortUpDown;
			INT						m_nitemID;
};

//================================================================================================= E. CLASS
