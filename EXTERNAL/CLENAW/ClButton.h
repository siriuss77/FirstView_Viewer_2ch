//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClButton.H
//
//	Description: CLENA-W Button control
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

#include <HELPER/ClHashedString.H>
#include <HELPER/ClGeometry.H>
#include <HELPER/ClDrawImage.H>
#include <HELPER/ClDrawText.H>

//================================================================================================= E. INCLUDE
//================================================================================================= S. CLASS

class ClButton : public CButton, IBGTransfer
{
	DECLARE_DYNAMIC(ClButton)

public:
	ClButton();
	virtual ~ClButton();

	//-------------------------------------------------------------------------------------- S. TYPEDEF
	enum SKIN_TYPE
	{
		SKIN_NORMAL		= 0	,
		SKIN_CHECKED		,
		SKIN_PUSHED			,
		SKIN_DISABLED		,
		SKIN_HOVER			,
		SKIN_PUSHED2		,
		SKIN_HOVER2			,
		SKIN_INVALID
	};

	enum OPERATION_MODE
	{
		MODE_PUSH	= 0	,
		MODE_CHECK		,
		MODE_INDICATE	,
		MODE_INVALID
	};
	//-------------------------------------------------------------------------------------- E. TYPEDEF

public:
			VOID 					SetTooltipText			(LPCTSTR lpszText, BOOL bActivate = TRUE);
			VOID					ActivateTooltip		(BOOL bActivate);
			BOOL					SetSkin				( SKIN_TYPE type, const HashedString& imagePath,
															const Color& color = Color((ARGB)Color::White),
															BOOL bUseTpColor = FALSE, const Color& tpColor = Color((ARGB)Color::Black),
															ClDrawImage::DRAW_MODE drawMode = ClDrawImage::MODE_STRETCH,
															const ClGeometry& geoImage = GEO_FULL_SIZE,
															const ClGeometry& geoSubImage = GEO_FULL_SIZE,
															DWORD dwGeoBase = GEOX_LEFT | GEOY_TOP );
			ClDrawImage*			GetSkin				( SKIN_TYPE type );

			BOOL					SetText				( const ClGeometry& geometry, const CString& cstrString, const CString& cstrFontFace, INT nFontSize,
															DWORD dwformat = ClDrawText::FORMAT_CENTER|ClDrawText::FORMAT_VCENTER|ClDrawText::FORMAT_WORDELLIPSIS,
															Color color = Color((ARGB)Color::White) );

			OPERATION_MODE			GetMode				( VOID );
			VOID					SetMode				( ClButton::OPERATION_MODE mode, BOOL bIsIndicate = FALSE );

			VOID					SetCheckState		( BOOL bCheck = TRUE );
			BOOL					GetCheckState		( VOID );

			VOID 					SetEnable( BOOL bEnable );

protected:
			void 					InitToolTip			( VOID );
			VOID					DrawSkin			( SKIN_TYPE type, CDC* pDC );

protected:

	DECLARE_MESSAGE_MAP()


public:

	afx_msg	BOOL					OnEraseBkgnd		( CDC* pDC );
	afx_msg	void					OnMouseHover		( UINT nFlags, CPoint point );
	afx_msg	void					OnMouseLeave		();
	afx_msg	void					OnMouseMove			( UINT nFlags, CPoint point );
	afx_msg void					OnLButtonUp			( UINT nFlags, CPoint point );

	virtual	void					DrawItem			( LPDRAWITEMSTRUCT lpDrawItemStruct );
	virtual	void					TransferBackground	( CDC* pDC );
	virtual	BOOL				PreTranslateMessage(MSG* pMsg);


protected:
			ClDrawImage*			m_DrawImageList[SKIN_INVALID];
			ClDrawText*				m_pDrawText;

			OPERATION_MODE			m_Mode;
			BOOL					m_bIsChecked;
			BOOL					m_bIsHover;
			BOOL					m_bIndicate;
			SKIN_TYPE				m_curSkin;

			CToolTipCtrl 				m_ToolTip; // tony add
};

//================================================================================================= E. CLASS
