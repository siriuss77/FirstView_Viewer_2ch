//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClGdiFactroy.h
//
//	Description: CLENAW GID/GDI+ Object factroy
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

//================================================================================================= E. INCLUDE
//================================================================================================= S. PREPROCESSOR

//================================================================================================= E. PREPROCESSOR
//================================================================================================= S. TYPEDEF

//================================================================================================= E. TYPEDEF
//================================================================================================= S. CLASS

class ClGdiFactory
{

private:
		ClGdiFactory();
		~ClGdiFactory();

public:

	enum TEXT_ALIGN
	{
		ALIGN_LEFT,
		ALIGN_CENTER,
		ALIGN_RIGHT
	};

	enum TEXT_VALIGN
	{
		VALIGN_TOP,
		VALIGN_MIDDLE,
		VALIGN_BOTTOM
	};

public:

	static	StringFormat*			createDefStringFormat( void );
	static	StringFormat*			createStringFormat	( TEXT_ALIGN align, TEXT_VALIGN vAlign );

	static	SolidBrush*				createSolidBrush	( const Color& solidColor );
	static	Font*					createFont			( const CString& strFontName, UINT nFontSize, UINT fontStyle );

	static	HFONT					createGDIFont		( HDC hDC, const CString& pFaceName, INT FontSize,
															BOOL bBold, BOOL bItalic, BOOL bUnderLine, BOOL bStrikeOut );

private:

	
};

//================================================================================================= E. CLASS


