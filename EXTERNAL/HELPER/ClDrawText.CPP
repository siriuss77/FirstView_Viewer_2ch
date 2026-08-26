//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClDrawText.CPP
//
//	Description: CLENAW GID+ Text drawing class
//
//	Update History
// 
//		2011.10.10 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. INCLUDE

#include <stdafx.h>
#include <HELPER/ClDrawText.H>
#include <HELPER/ClGdiFactory.H>
#include <CLENAW/ClMacro.H>
#include <HELPER/CLUtil.H>

//================================================================================================= E. INCLUDE
//================================================================================================= S. CON/DCON

ClDrawText::ClDrawText()
{
	this->m_gdiFont	= NULL;
	this->m_geoText = GEO_FULL_SIZE;
}


ClDrawText::~ClDrawText()
{
	if( this->m_gdiFont )
		DeleteObject( this->m_gdiFont );
}

//================================================================================================= E. CON/DCON
//================================================================================================= S. INIT/DEINIT

//================================================================================================= E. INIT/DEINIT
//================================================================================================= S. OPERATION

BOOL ClDrawText::drawText( CDC* pMemDC, const CRect& rtClient )
{
	if( !pMemDC || !pMemDC->m_hDC )		return FALSE;
	if( !this->m_cstrText.GetLength() )	return TRUE;

	ClGeometry geoText = this->m_geoText;
	ClUtil::GEOMETRY::recalcGeometry( geoText, rtClient.Width(), rtClient.Height(), 0 );

	RECT rtText;
	rtText.left  = geoText.x;
	rtText.top   = geoText.y;
	rtText.right = geoText.getRight();
	rtText.bottom= geoText.getBottom();

	CString cstrConvText = this->m_cstrText;

	if( this->m_dwformat & FORMAT_WORDBREAK )
	{
		cstrConvText.Replace( TEXT("\\r"), TEXT("") );
		cstrConvText.Replace( TEXT("\\n"), TEXT("\n") );
		cstrConvText.Replace( TEXT("/n"), TEXT("\n") );
	}
	else
	{
		cstrConvText.Replace( TEXT("\\r"), TEXT("\r") );
		cstrConvText.Replace( TEXT("\\n"), TEXT("\n") );
		cstrConvText.Replace( TEXT("/n"), TEXT("\n") );
	}

	if( !this->m_gdiFont )
	{
		this->m_gdiFont = ClGdiFactory::createGDIFont( pMemDC->m_hDC, this->m_cstrFontFace, this->m_nFontSize, this->m_dwformat & FORMAT_BOLD,
			this->m_dwformat & FORMAT_ITALIC, this->m_dwformat & FORMAT_UNDERLINE, this->m_dwformat & FORMAT_STRIKEOUT );
	}

	if( !this->m_gdiFont )
		return FALSE;

	HFONT hOldFont = (HFONT)pMemDC->SelectObject( this->m_gdiFont );

	UINT dwDTFormat = DT_NOPREFIX|DT_EDITCONTROL;
	int rendered_width;
	int rendered_height;
	{
		if( 0 == (this->m_dwformat & FORMAT_WORDBREAK) )
		{
			dwDTFormat = DT_SINGLELINE|DT_CALCRECT|DT_LEFT|DT_TOP|DT_NOCLIP|DT_EDITCONTROL|DT_NOPREFIX;
		}
		else
		{
			dwDTFormat = DT_WORDBREAK|DT_CALCRECT|DT_LEFT|DT_TOP|DT_NOCLIP|DT_EDITCONTROL|DT_NOPREFIX;
		}
		rtText.right = rtClient.Width();
		rtText.bottom= 0;
		pMemDC->DrawText( this->m_cstrText, -1, &rtText, dwDTFormat );
		rendered_width = rtText.right;
		rendered_height= rtText.bottom;
	}

	rtText.right = rtClient.Width();
	rtText.bottom= rtClient.Height();
	dwDTFormat = DT_NOPREFIX|DT_EDITCONTROL;

	if( 0 == (this->m_dwformat & FORMAT_WORDBREAK) )
	{
		dwDTFormat |= DT_SINGLELINE;
		if( 0 != (this->m_dwformat & FORMAT_WORDELLIPSIS) )
		{
			dwDTFormat |= DT_END_ELLIPSIS;
		}
	}
	else
	{
		dwDTFormat |= DT_WORDBREAK;
	}

	if( 0 != (this->m_dwformat & FORMAT_CENTER) )		{ dwDTFormat |= DT_CENTER ; }
	else if( 0 != (this->m_dwformat & FORMAT_RIGHT) )	{ dwDTFormat |= DT_RIGHT  ; }
	else												{ dwDTFormat |= DT_LEFT   ; }
	if( 0 != (this->m_dwformat & FORMAT_VCENTER) )    
	{
		if( 0 == (dwDTFormat & DT_WORDBREAK) )
		{
			dwDTFormat |= DT_VCENTER;
		}
		else
		{
			int offset_y = (rtClient.Height() - rendered_height)/2;
			rtText.top += offset_y;
		}
	}
	else if( 0 != (this->m_dwformat & FORMAT_BOTTOM) )
	{
		if( 0 == (dwDTFormat & DT_WORDBREAK) )
		{
			dwDTFormat |= DT_BOTTOM ;
		}
		else
		{
			int offset_y = (rtClient.Height() - rendered_height);
			rtText.top += offset_y;
		}
	}
	else
	{
		dwDTFormat |= DT_TOP;
	}

	pMemDC->SetTextColor( RGB(this->m_color.GetR(), this->m_color.GetG(), this->m_color.GetB()) );
	pMemDC->SetBkMode( TRANSPARENT );
	pMemDC->DrawText( cstrConvText, -1, &rtText, dwDTFormat );
	pMemDC->SelectObject( hOldFont );

	return TRUE;
}


//================================================================================================= E. OPERATION
//================================================================================================= S. CONTROL

//================================================================================================= E. CONTROL
//================================================================================================= S. GET/SET

VOID ClDrawText::SetGeometry( const ClGeometry& geometry )
{
	this->m_geoText = geometry;

	return;
}


const ClGeometry ClDrawText::GetGeometry( VOID )
{
	return this->m_geoText;
}


VOID ClDrawText::setText( const CString& cstrText )
{
	this->m_cstrText = cstrText;
	return;
}


const CString& ClDrawText::getText( VOID )
{
	return this->m_cstrText;
}


VOID ClDrawText::setFont( const CString& fontFace, INT nFontSize, DWORD dwformat, const Color& color )
{
	this->m_cstrFontFace	= fontFace;
	this->m_nFontSize		= nFontSize;
	this->m_dwformat		= dwformat;
	this->m_color			= color;

	DeleteObject( this->m_gdiFont );
	this->m_gdiFont = NULL;

	return;
}


const CString& ClDrawText::getFontFace( VOID )
{
	return this->m_cstrFontFace;
}


INT ClDrawText::getFontSize( VOID )
{
	return this->m_nFontSize;
}


VOID ClDrawText::setColor( const Color& color )
{
	this->m_color = color;
	return;
}


const Color& ClDrawText::getColor( VOID )
{
	return this->m_color;
}


VOID ClDrawText::setAlign( DWORD dwAlign )
{
	if( dwAlign & FORMAT_LEFT ) this->m_dwformat |= FORMAT_LEFT;
	else						this->m_dwformat &= ~FORMAT_LEFT;

	if( dwAlign & FORMAT_CENTER ) this->m_dwformat |= FORMAT_CENTER;
	else						this->m_dwformat &= ~FORMAT_CENTER;

	if( dwAlign & FORMAT_RIGHT ) this->m_dwformat |= FORMAT_RIGHT;
	else						this->m_dwformat &= ~FORMAT_RIGHT;

	if( dwAlign & FORMAT_TOP ) this->m_dwformat |= FORMAT_TOP;
	else						this->m_dwformat &= ~FORMAT_TOP;

	if( dwAlign & FORMAT_VCENTER ) this->m_dwformat |= FORMAT_VCENTER;
	else						this->m_dwformat &= ~FORMAT_VCENTER;

	if( dwAlign & FORMAT_BOTTOM ) this->m_dwformat |= FORMAT_BOTTOM;
	else						this->m_dwformat &= ~FORMAT_BOTTOM;

	return;
}


DWORD ClDrawText::getFormat( VOID )
{
	return this->m_dwformat;
}

//================================================================================================= E. GET/SET
//================================================================================================= S. STATIC

//================================================================================================= E. STATIC
//================================================================================================= EOF

