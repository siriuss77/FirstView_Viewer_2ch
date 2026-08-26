//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: wbcText.CPP
//
//	Description: CLENA-W Text control
//
//	Update History
// 
//		2011.11.23 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. INCLUDE

#include "stdafx.h"
#include "wbcText.h"
#include "wbcMemDC.h"

//================================================================================================= E. INCLUDE
//================================================================================================= S. CON/DCON
IMPLEMENT_DYNAMIC(CWBCText, CStatic)

CWBCText::CWBCText()
{
    this->m_crTextColor     = RGB(0,0,0);
    this->m_cstrFontFace    = TEXT("Arial");
    this->m_nFontSize       = 10;
}

CWBCText::~CWBCText()
{
}

//================================================================================================= E. CON/DCON
//================================================================================================= S. MESSAGE

BEGIN_MESSAGE_MAP(CWBCText, CStatic)
END_MESSAGE_MAP()

//================================================================================================= E. MESSAGE
//================================================================================================= S. VIRTUAL

void CWBCText::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
    CDC drawItemDC;
    drawItemDC.Attach( lpDrawItemStruct->hDC );

    CRect rect;
    GetClientRect(rect);
    CWBCMemDC memDC( &drawItemDC, &rect );

    CString szText;
    GetWindowText(szText);

    memDC.BitBlt( 0, 0, rect.Width(), rect.Height(), &this->m_dcBackground, 0, 0, SRCCOPY );

    CFont font;
    CFont *pOldFont;
    int newFontSize = 0 - MulDiv( this->m_nFontSize, GetDeviceCaps(lpDrawItemStruct->hDC, LOGPIXELSY), 72 );

    font.CreateFont(
        newFontSize,                // nHeight
        0,                          // nWidth
        0,                          // nEscapement
        0,                          // nOrientation
        FW_NORMAL,                  // nWeight
        FALSE,                      // bItalic
        FALSE,                      // bUnderline
        0,                          // cStrikeOut
        ANSI_CHARSET,               // nCharSet
        OUT_DEFAULT_PRECIS,         // nOutPrecision
        CLIP_DEFAULT_PRECIS,        // nClipPrecision
        DEFAULT_QUALITY,            // nQuality
        DEFAULT_PITCH | FF_SWISS,   // nPitchAndFamily
        this->m_cstrFontFace);      // lpszFacename)

    pOldFont = memDC.SelectObject( &font );
    DWORD dwStyle = GetStyle();
    DWORD dwText = 0;

    memDC.SetTextColor( this->m_crTextColor );
    memDC.SetBkMode( TRANSPARENT );
    memDC.DrawText( szText, rect, dwText );
    memDC.SelectObject( pOldFont );

    return;
}


VOID CWBCText::TransferBackground( CDC* pDC )
{
    this->m_dcBackground.DeleteDC();

    CBitmap bmp, *pOldBitmap;
    CRect rtWindow;
    GetWindowRect(rtWindow);
    GetParent()->ScreenToClient(rtWindow);

    this->m_dcBackground.CreateCompatibleDC( pDC );
    bmp.CreateCompatibleBitmap( pDC, rtWindow.Width(), rtWindow.Height() );
    pOldBitmap = this->m_dcBackground.SelectObject(&bmp);
    this->m_dcBackground.BitBlt( 0, 0, rtWindow.Width(), rtWindow.Height(), pDC, rtWindow.left, rtWindow.top, SRCCOPY );
    bmp.DeleteObject();

    return;
}


void CWBCText::SetTextColor( COLORREF crTextColor )
{
    this->m_crTextColor = crTextColor;

    this->Invalidate();

    return;
}


void CWBCText::SetFont( const CString& cstrFontFace, int nFontSize )
{
    this->m_cstrFontFace = cstrFontFace;
    this->m_nFontSize = nFontSize;

    return;
}

//================================================================================================= E. VIRTUAL
//================================================================================================= S. OPERATION

//================================================================================================= E. OPERATION
//================================================================================================= S. CONTROL

//================================================================================================= E. CONTROL
//================================================================================================= S. GET/SET

//================================================================================================= E. GET/SET
