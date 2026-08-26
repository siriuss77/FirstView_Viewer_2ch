//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClImage.CPP
//
//	Description: CLENA-W Image control
//
//	Update History
// 
//		2011.11.23 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. INCLUDE

#include <stdafx.h>
#include "ClImage.h"
#include <CLENAW/ClDialog.h>
#include <HELPER/ClLog.H>

//================================================================================================= E. INCLUDE
//================================================================================================= S. CON/DCON
IMPLEMENT_DYNAMIC(ClImage, CStatic)

ClImage::ClImage()
{
	this->m_pImage		= NULL;
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;
}

ClImage::~ClImage()
{
	if( this->m_pImage ) delete this->m_pImage;
}

//================================================================================================= E. CON/DCON
//================================================================================================= S. MESSAGE

BEGIN_MESSAGE_MAP(ClImage, CStatic)
END_MESSAGE_MAP()

//================================================================================================= E. MESSAGE
//================================================================================================= S. VIRTUAL

void ClImage::InitToolTip( VOID )
{
	if (m_ToolTip.m_hWnd == NULL)
	{
		// Create ToolTip control
		m_ToolTip.Create(this);
		// Create inactive
		m_ToolTip.Activate(FALSE);
		// Enable multiline
		m_ToolTip.SendMessage(TTM_SETMAXTIPWIDTH, 0, 400);
	} // if
} // End of InitToolTip

// This function sets the text to show in the button tooltip.
//
// Parameters:
//		[IN]	lpszText
//				Pointer to a null-terminated string containing the text to show.
//		[IN]	bActivate
//				If TRUE the tooltip will be created active.
//
void ClImage::SetTooltipText(LPCTSTR lpszText, BOOL bActivate)
{
	// We cannot accept NULL pointer
	if (lpszText == NULL) return;

	// Initialize ToolTip
	InitToolTip();

	// If there is no tooltip defined then add it
	if (m_ToolTip.GetToolCount() == 0)
	{
		CRect rectBtn; 
		GetClientRect(rectBtn);
		m_ToolTip.AddTool(this, lpszText, rectBtn, 1);
	} // if

	// Set text for tooltip
	m_ToolTip.UpdateTipText(lpszText, this, 1);
	m_ToolTip.Activate(bActivate);
} // End of SetTooltipText

// This function enables or disables the button tooltip.
//
// Parameters:
//		[IN]	bActivate
//				If TRUE the tooltip will be activated.
//
void ClImage::ActivateTooltip(BOOL bActivate)
{
	// If there is no tooltip then do nothing
	if (m_ToolTip.GetToolCount() == 0) return;

	// Activate tooltip
	m_ToolTip.Activate(bActivate);
} // End of EnableTooltip

void ClImage::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	CDC drawItemDC;
	drawItemDC.Attach( lpDrawItemStruct->hDC );

	CRect rect;
	GetClientRect(rect);
	ClMemDC memDC( &drawItemDC, &rect );

	if(memDC.m_hDC)
	{
		memDC.BitBlt( 0, 0, rect.Width(), rect.Height(), &this->m_dcBackground, 0, 0, SRCCOPY );

		if( this->m_pImage )
		{
			this->m_pImage->prepareImage( &memDC, rect, &this->m_dcBackground );
			this->m_pImage->drawImage( &memDC, rect );
		}
	}

	return;
}

BOOL ClImage::PreTranslateMessage(MSG* pMsg) 
{
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);
	
	return CStatic::PreTranslateMessage(pMsg);
} // End of PreTranslateMessage

VOID ClImage::TransferBackground( CDC* pDC )
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

BOOL ClImage::SetImage( const HashedString& imagePath, const Color& color, BOOL bUseTpColor, const Color& tpColor,
					   ClDrawImage::DRAW_MODE drawMode, const ClGeometry& geoImage, const ClGeometry& geoSubImage,
					   DWORD dwGeoBase )
{
	if( !this->m_pImage )
	{
		this->m_pImage = new ClDrawImage();
		if( !this->m_pImage )
		{
			CLD_FL( DF_ERROR, "Out of memory" );
			return FALSE;
		}
	}

	if( !this->m_pImage->loadImage(imagePath, color, !!bUseTpColor, tpColor, drawMode, geoImage, geoSubImage, dwGeoBase ) )
		return FALSE;

	return TRUE;
}

ClDrawImage* ClImage::GetImage( VOID )
{
	return this->m_pImage;
}

//================================================================================================= E. GET/SET

