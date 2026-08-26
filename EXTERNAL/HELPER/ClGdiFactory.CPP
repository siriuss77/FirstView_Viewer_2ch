//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClGdiFactory.CPP
//
//	Description: CLENAW GID/GDI+ Object Factory
//
//	Update History
// 
//		2011.10.10 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. INCLUDE

#include <stdafx.h>
#include <HELPER/ClGdiFactory.H>

//================================================================================================= E. INCLUDE
//================================================================================================= S. CON/DCON

ClGdiFactory::ClGdiFactory()
{

}

ClGdiFactory::~ClGdiFactory()
{

}

//================================================================================================= E. CON/DCON
//================================================================================================= S. INIT/DEINIT

//================================================================================================= E. INIT/DEINIT
//================================================================================================= S. OPERATION

StringFormat* ClGdiFactory::createDefStringFormat( )
{
	return ClGdiFactory::createStringFormat( ClGdiFactory::ALIGN_CENTER, ClGdiFactory::VALIGN_MIDDLE );
}


StringFormat* ClGdiFactory::createStringFormat( TEXT_ALIGN align, TEXT_VALIGN vAlign )
{
	StringAlignment stringAlign = StringAlignmentNear;
	StringFormat*	pStringFormat = new StringFormat();

	if( pStringFormat == NULL )
		return NULL;

	switch( align )
	{
	case ALIGN_LEFT		: stringAlign = StringAlignmentNear;	break;
	case ALIGN_CENTER	: stringAlign = StringAlignmentCenter;	break;
	case ALIGN_RIGHT	: stringAlign = StringAlignmentFar;		break;
	}

	pStringFormat->SetAlignment( stringAlign );

	stringAlign = StringAlignmentCenter;
	switch( vAlign )
	{
	case VALIGN_TOP		: stringAlign = StringAlignmentNear;	break;
	case VALIGN_MIDDLE	: stringAlign = StringAlignmentCenter;	break;
	case VALIGN_BOTTOM	: stringAlign = StringAlignmentFar;		break;
	}

	pStringFormat->SetLineAlignment( stringAlign );

	return pStringFormat;
}


SolidBrush* ClGdiFactory::createSolidBrush( const Color& solidColor )
{
	SolidBrush* pSolidBrush = new SolidBrush( solidColor );
	return pSolidBrush;
}


Font* ClGdiFactory::createFont( const CString& strFontName, UINT nFontSize, UINT fontStyle )
{
	/* fontStyle
	FontStyleRegular    = 0,
	FontStyleBold       = 1,
	FontStyleItalic     = 2,
	FontStyleBoldItalic = 3,
	FontStyleUnderline  = 4,
	FontStyleStrikeout  = 8
	*/

	Gdiplus::Font*	pFont;
	FontFamily		fontFamily( (LPCTSTR)strFontName );

	pFont = new Gdiplus::Font( &fontFamily, (REAL)nFontSize, fontStyle, UnitPoint );

	return pFont;
}


HFONT ClGdiFactory::createGDIFont( HDC hDC, const CString& pFaceName, INT FontSize, BOOL bBold, BOOL bItalic, BOOL bUnderLine, BOOL bStrikeOut )
{
	if( hDC == NULL ) return NULL;

	HFONT result;
	const int PixelsPerInch = GetDeviceCaps(hDC, LOGPIXELSY);
	const int PointsPerInch = 72;
	int lfHeight = - FontSize * PixelsPerInch / PointsPerInch;

	LOGFONT logfont;		
	logfont.lfHeight        = lfHeight;
	logfont.lfWidth         = 0;
	logfont.lfEscapement    = 0;
	logfont.lfOrientation   = 0;
	logfont.lfWeight        = bBold ? FW_BOLD : FW_NORMAL;
	logfont.lfItalic        = (BYTE)bItalic;
	logfont.lfUnderline     = (BYTE)bUnderLine;
	logfont.lfStrikeOut     = (BYTE)bStrikeOut;
	logfont.lfCharSet       = DEFAULT_CHARSET;
	logfont.lfOutPrecision  = OUT_DEFAULT_PRECIS;
	logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont.lfQuality       = DRAFT_QUALITY|ANTIALIASED_QUALITY;
	logfont.lfPitchAndFamily= DEFAULT_PITCH|FF_DONTCARE;

	wcscpy_s( logfont.lfFaceName, sizeof(logfont.lfFaceName)/sizeof(logfont.lfFaceName[0]), pFaceName );

	result = CreateFontIndirect( &logfont );

	return result;
}

//================================================================================================= E. OPERATION
//================================================================================================= S. CONTROL

//================================================================================================= E. CONTROL
//================================================================================================= S. GET/SET

//================================================================================================= E. GET/SET
//================================================================================================= S. STATIC

//================================================================================================= E. STATIC
//================================================================================================= EOF

