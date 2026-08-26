//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClText.CPP
//
//	Description: CLENA-W Text control
//
//	Update History
// 
//		2011.11.23 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. INCLUDE

#include <stdafx.h>
#include "ClText.h"

#include <CLENAW/ClDialog.h>
#include <HELPER/ClLog.H>

//================================================================================================= E. INCLUDE
//================================================================================================= S. CON/DCON
IMPLEMENT_DYNAMIC(ClText, CStatic)

ClText::ClText()
{
	this->m_pText = NULL;
}

ClText::~ClText()
{
	if( this->m_pText )
		delete this->m_pText;
}

//================================================================================================= E. CON/DCON
//================================================================================================= S. MESSAGE

BEGIN_MESSAGE_MAP(ClText, CStatic)
END_MESSAGE_MAP()

//================================================================================================= E. MESSAGE
//================================================================================================= S. VIRTUAL

void ClText::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	CDC drawItemDC;
	drawItemDC.Attach( lpDrawItemStruct->hDC );

	CRect rect;
	GetClientRect(rect);
	ClMemDC memDC( &drawItemDC, &rect );

	if(memDC.m_hDC)
	{
		memDC.BitBlt( 0, 0, rect.Width(), rect.Height(), &this->m_dcBackground, 0, 0, SRCCOPY );

		if( this->m_pText )
		{
			CString cstrText;
			this->GetWindowText( cstrText );
			this->m_pText->setText( cstrText );
			this->m_pText->drawText( &memDC, rect );
		}
	}
	return;
}


VOID ClText::TransferBackground( CDC* pDC )
{
	this->m_dcBackground.DeleteDC();

	CBitmap bmp, *pOldBitmap;
	CRect rtWindow;
	GetWindowRect(rtWindow);
	GetParent()->ScreenToClient(rtWindow);

	m_dcBackground.CreateCompatibleDC( pDC );
	bmp.CreateCompatibleBitmap( pDC, rtWindow.Width(), rtWindow.Height() );
	pOldBitmap = m_dcBackground.SelectObject(&bmp);
	m_dcBackground.BitBlt( 0, 0, rtWindow.Width(), rtWindow.Height(), pDC, rtWindow.left, rtWindow.top, SRCCOPY );
	bmp.DeleteObject();

	return;
}

//================================================================================================= E. VIRTUAL
//================================================================================================= S. OPERATION

//================================================================================================= E. OPERATION
//================================================================================================= S. CONTROL

//================================================================================================= E. CONTROL
//================================================================================================= S. GET/SET

BOOL ClText::SetDrawText( const CString& cstrString, const CString& cstrFontFace, INT nFontSize, DWORD dwformat, const Color& color )
{
	if( !this->m_pText )
	{
		this->m_pText = new ClDrawText();
		if( !this->m_pText )
		{
			CLD_FL( DF_ERROR, "Out of memory" );
			return FALSE;
		}
	}

	this->m_pText->setText( cstrString );
	this->m_pText->setFont( cstrFontFace, nFontSize, dwformat, color );

	return TRUE;
}


ClDrawText* ClText::GetDrawText( VOID )
{
	return this->m_pText;
}

//================================================================================================= E. GET/SET
