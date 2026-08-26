//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClDrawText.h
//
//	Description: CLENAW GID+ Text drawing class
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
#include <HELPER/ClGeometry.H>

//================================================================================================= E. INCLUDE
//================================================================================================= S. PREPROCESSOR

//================================================================================================= E. PREPROCESSOR
//================================================================================================= S. CLASS

class ClDrawText
{

public:
		ClDrawText();
		~ClDrawText();

		enum TEXT_FORMAT
		{
			FORMAT_WORDBREAK	= (1<< 0),
			FORMAT_WORDELLIPSIS	= (1<< 1),
			FORMAT_LEFT			= (1<< 2),
			FORMAT_CENTER		= (1<< 3),
			FORMAT_RIGHT		= (1<< 4),
			FORMAT_TOP			= (1<< 5),
			FORMAT_VCENTER		= (1<< 6),
			FORMAT_BOTTOM		= (1<< 7),
			FORMAT_BOLD			= (1<< 8),
			FORMAT_ITALIC		= (1<< 9),
			FORMAT_UNDERLINE	= (1<< 10),
			FORMAT_STRIKEOUT	= (1<< 11),
		};

public:

				BOOL				drawText			( CDC* pDC, const CRect& rtClient );

public:
				VOID				SetGeometry			( const ClGeometry& geometry );
				const ClGeometry	GetGeometry			( VOID );

				VOID				setText				( const CString& cstrText );
				const CString&		getText				( VOID );

				VOID				setFont				( const CString& fontFace, INT nFontSize,
															DWORD dwformat = FORMAT_CENTER|FORMAT_VCENTER|FORMAT_WORDELLIPSIS,
															const Color& color = Color((ARGB)Color::White) );

				const CString&		getFontFace			( VOID );
				INT					getFontSize			( VOID );

				VOID				setColor			( const Color& color );
				const Color&		getColor			( VOID );

				VOID				setAlign			( DWORD dwAlign );
				DWORD				getFormat			( VOID );

public:

				ClGeometry			m_geoText;
				CString				m_cstrText;
				CString				m_cstrFontFace;
				INT					m_nFontSize;
				Color				m_color;

				DWORD				m_dwformat;
				HFONT				m_gdiFont;
};

//================================================================================================= E. CLASS

