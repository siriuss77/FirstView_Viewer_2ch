//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClButton.CPP
//
//	Description: CLENA-W Button control
//
//	Update History
// 
//		2011.11.23 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. INCLUDE

#include <stdafx.h>
#include "ClButton.h"
#include "ClDialog.H"

#include <HELPER/ClLog.H>
#include <HELPER/ClMemDC.H>

//================================================================================================= E. INCLUDE
//================================================================================================= S. CON/DCON
IMPLEMENT_DYNAMIC(ClButton, CButton)

ClButton::ClButton()
{
	INT nCount;
	for( nCount = (INT)SKIN_NORMAL; nCount < (INT)SKIN_INVALID; nCount++ )
		this->m_DrawImageList[nCount] = NULL;

	this->m_pDrawText	= NULL;
	this->m_bIsHover	= FALSE;
	this->m_bIsChecked	= FALSE;
	this->m_bIndicate		= FALSE;
	this->m_Mode		= MODE_PUSH;

	// No tooltip created
	m_ToolTip.m_hWnd = NULL;
	

}

ClButton::~ClButton()
{
	INT nCount;
	for( nCount = (INT)SKIN_NORMAL; nCount < (INT)SKIN_INVALID; nCount++ )
		if( this->m_DrawImageList[nCount] ) delete this->m_DrawImageList[nCount];

	if( this->m_pDrawText ) delete this->m_pDrawText;
}

//================================================================================================= E. CON/DCON
//================================================================================================= S. VIRTUAL

void ClButton::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	if( lpDrawItemStruct->itemAction != ODA_DRAWENTIRE )
		return;

	SKIN_TYPE curType = SKIN_INVALID;

	if( lpDrawItemStruct->itemState & ODS_FOCUS )
	{
		if( lpDrawItemStruct->itemState & ODS_SELECTED )
		{
			if( (this->m_Mode == MODE_CHECK) && this->m_bIsChecked )
				curType = SKIN_PUSHED2;
			else
				curType = SKIN_PUSHED;
		}
		else
		{
			if( (this->m_Mode == MODE_CHECK) && this->m_bIsChecked )
				curType = this->m_bIsHover ? SKIN_HOVER2 : SKIN_CHECKED;
			else
				curType = this->m_bIsHover ? SKIN_HOVER : SKIN_NORMAL;
		}
	}
	else if( lpDrawItemStruct->itemState & ODS_DISABLED )	curType = SKIN_DISABLED;
	else
	{
		if( (this->m_Mode == MODE_CHECK) && this->m_bIsChecked )
			curType = SKIN_CHECKED;
		else
			curType = SKIN_NORMAL;
	}

	CDC drawItemDC;
	drawItemDC.Attach( lpDrawItemStruct->hDC );
	this->DrawSkin( curType, &drawItemDC );

	return;
}

BOOL ClButton::PreTranslateMessage(MSG* pMsg) 
{
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);
	
	return CButton::PreTranslateMessage(pMsg);
} // End of PreTranslateMessage

void ClButton::TransferBackground( CDC* pDC )
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

	INT nCount;
	for( nCount = (INT)SKIN_NORMAL; nCount < (INT)SKIN_INVALID; nCount++ )
		if( this->m_DrawImageList[nCount] ) this->m_DrawImageList[nCount]->setUpdate();

	return;
}

//================================================================================================= E. VIRTUAL
//================================================================================================= S. OPERATION

VOID ClButton::DrawSkin( SKIN_TYPE type, CDC* pDC )
{
	CRect rect;
	GetClientRect(rect);
	ClMemDC memDC( pDC, &rect, TRUE );

	if(memDC.m_hDC)
	{
		if( this->m_DrawImageList[(INT)type] )
		{
			this->m_DrawImageList[(INT)type]->prepareImage( &memDC, rect, &this->m_dcBackground );
			this->m_DrawImageList[(INT)type]->drawImage( &memDC, rect );
		}

		if( this->m_pDrawText )
			this->m_pDrawText->drawText( &memDC, rect );
	}

	return;
}

//================================================================================================= E. OPERATION

BEGIN_MESSAGE_MAP(ClButton, CButton)
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

//================================================================================================= S. MESSAGE

BOOL ClButton::OnEraseBkgnd( CDC* pDC )
{
	UNREFERENCED_PARAMETER( pDC );

	return FALSE;
}


void ClButton::OnMouseHover(UINT nFlags, CPoint point)
{	
	if( this->m_bIsHover )
		return;

	this->m_bIsHover = TRUE;

	CClientDC aDC(this);

	if( this->m_Mode == MODE_CHECK && this->m_bIsChecked )
		this->DrawSkin( SKIN_HOVER2, &aDC );
	else
		this->DrawSkin( SKIN_HOVER, &aDC );

	CButton::OnMouseHover(nFlags, point);
}


void ClButton::OnMouseLeave()
{
	if( !this->m_bIsHover )
		return;

	this->m_bIsHover = FALSE;

	CClientDC aDC(this);

	if( this->GetState() == BST_PUSHED )
	{
		if( this->m_Mode == MODE_CHECK && this->m_bIsChecked )
			this->DrawSkin( SKIN_PUSHED2, &aDC );
		else
			this->DrawSkin( SKIN_PUSHED, &aDC );
	}
	else
	{
		if( this->m_Mode == MODE_CHECK && this->m_bIsChecked )
			this->DrawSkin( SKIN_CHECKED, &aDC );
		else
			this->DrawSkin( SKIN_NORMAL, &aDC );
	}

	this->GetParent()->SetFocus(); // 20131014 wheel 동작 문제로 추가
	CButton::OnMouseLeave();
}


void ClButton::OnMouseMove(UINT nFlags, CPoint point)
{
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.hwndTrack = m_hWnd;
	tme.dwFlags = TME_LEAVE|TME_HOVER;
	tme.dwHoverTime = 1;
	TrackMouseEvent(&tme);

	CButton::OnMouseMove(nFlags, point);
}


void ClButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	CRect rtButton;

	if( !this->m_bIndicate){
		this->GetClientRect( rtButton );
		if( rtButton.PtInRect(point) )
		{
			this->m_bIsChecked = !this->m_bIsChecked;
		}
	}
	CButton::OnLButtonUp(nFlags, point);
}

//================================================================================================= E. MESSAGE
//================================================================================================= S. GET/SET

void ClButton::InitToolTip( VOID )
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
void ClButton::SetTooltipText(LPCTSTR lpszText, BOOL bActivate)
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
void ClButton::ActivateTooltip(BOOL bActivate)
{
	// If there is no tooltip then do nothing
	if (m_ToolTip.GetToolCount() == 0) return;

	// Activate tooltip
	m_ToolTip.Activate(bActivate);
} // End of EnableTooltip

BOOL ClButton::SetSkin( SKIN_TYPE type, const HashedString& imagePath, const Color& color, BOOL bUseTpColor, const Color& tpColor,
					   ClDrawImage::DRAW_MODE drawMode, const ClGeometry& geoImage, const ClGeometry& geoSubImage, DWORD dwGeoBase )
{
	if( (type == SKIN_INVALID) || imagePath.isNULL() )
		return FALSE;

	if( this->m_DrawImageList[type] == NULL )
	{
		this->m_DrawImageList[type] = new ClDrawImage();
		if( !this->m_DrawImageList[type] )
			return FALSE;
	}

	return this->m_DrawImageList[type]->loadImage( imagePath, color,
		!!bUseTpColor, tpColor, drawMode, geoImage, geoSubImage, dwGeoBase ) ? TRUE : FALSE;
}


ClDrawImage* ClButton::GetSkin( SKIN_TYPE type )
{
	if( type == SKIN_INVALID ) return FALSE;

	return this->m_DrawImageList[type];
}


BOOL ClButton::SetText( const ClGeometry& geometry, const CString& cstrString, const CString& cstrFontFace, INT nFontSize, DWORD dwformat, Color color )
{
	if( !this->m_pDrawText )
	{
		this->m_pDrawText = new ClDrawText();
		if( !this->m_pDrawText ) return FALSE;
	}

	this->m_pDrawText->SetGeometry( geometry );
	this->m_pDrawText->setText( cstrString );
	this->m_pDrawText->setFont( cstrFontFace, nFontSize, dwformat, color );

	return TRUE;
}


ClButton::OPERATION_MODE ClButton::GetMode( VOID )
{
	return this->m_Mode;
}


VOID ClButton::SetMode( ClButton::OPERATION_MODE mode , BOOL bIsIndicate /* = FALSE */)
{
	if( mode == MODE_INVALID ) return;

	this->m_Mode = mode;
	this->m_bIndicate = bIsIndicate;

	return;
}


VOID ClButton::SetCheckState( BOOL bCheck )
{
	this->m_bIsChecked = bCheck;
	this->Invalidate();

	return;
}


BOOL ClButton::GetCheckState( VOID )
{
	return this->m_bIsChecked;
}

VOID ClButton::SetEnable( BOOL bEnable )
{
	//EnableWindow(bEnable);
	SetCheckState(bEnable);
	return ;
}

//================================================================================================= E. GET/SET
