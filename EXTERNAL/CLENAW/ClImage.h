//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClImage.H
//
//	Description: CLENA-W Image control
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
#include <HELPER/ClDrawImage.H>
#include <HELPER/ClGeometry.H>
#include <HELPER/ClHashedString.H>

//================================================================================================= E. INCLUDE
//================================================================================================= S. CLASS

class ClImage : public CStatic, IBGTransfer
{
	DECLARE_DYNAMIC(ClImage)

public:

	ClImage();
	virtual ~ClImage();

public:
			VOID 					SetTooltipText			(LPCTSTR lpszText, BOOL bActivate = TRUE);
			VOID					ActivateTooltip		(BOOL bActivate);
			BOOL					SetImage			( const HashedString& imagePath, const Color& color = Color((ARGB)Color::White),
															BOOL bUseTpColor=  FALSE, const Color& tpColor = Color((ARGB)Color::Black),
															ClDrawImage::DRAW_MODE drawMode = ClDrawImage::MODE_STRETCH,
															const ClGeometry& geoImage = GEO_FULL_SIZE,
															const ClGeometry& geoSubImage = GEO_FULL_SIZE,
															DWORD dwGeoBase = GEOX_LEFT | GEOY_TOP );

			ClDrawImage*			GetImage			( VOID );
	virtual VOID					TransferBackground	( CDC* pDC );
	virtual void					DrawItem			( LPDRAWITEMSTRUCT lpDrawItemStruct );
	virtual	BOOL				PreTranslateMessage(MSG* pMsg);

protected:
			void 					InitToolTip			( VOID );
			
protected:

	DECLARE_MESSAGE_MAP()


protected:

			ClDrawImage*			m_pImage;
			CToolTipCtrl 				m_ToolTip; // tony add
};

//================================================================================================= E. CLASS
