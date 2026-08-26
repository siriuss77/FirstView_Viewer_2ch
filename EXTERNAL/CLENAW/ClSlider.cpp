//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClSlider.CPP
//
//	Description: CLENA-W Slider control
//
//	Update History
// 
//		2011.11.23 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. INCLUDE

#include <stdafx.h>
#include "ClSlider.h"

#include <CLENAW/ClDialog.h>
#include <HELPER/ClLog.H>

//================================================================================================= E. INCLUDE

IMPLEMENT_DYNAMIC(ClSlider, CStatic)

//================================================================================================= S. CON/DCON

ClSlider::ClSlider()
{
	this->m_nPos		= 0;
	this->m_nMin		= 0;
	this->m_nMax		= 100;
	this->m_nPage		= 20;
	this->m_dbPagePixel	= 0.0f;

	this->m_bIsVertical	= FALSE;

	this->m_rtMargin	= CRect( 0, 0, 0, 0 );
	this->m_rtPrevRect	= CRect( 0, 0, -1, -1);
	this->m_nMouseOffset= 0;
	this->m_geoThumb	= ClGeometry( 0, 0, 0, 0);

	this->m_bIsButtonDown = FALSE;

	INT nSkinIndex;
	for( nSkinIndex = (INT)SKIN_TICS ; nSkinIndex < SKIN_INVALID; nSkinIndex ++ )
		this->m_pSkinList[nSkinIndex] = NULL;

	// No tooltip created
	m_ToolTip.m_hWnd = NULL;
}


ClSlider::~ClSlider()
{
	INT nSkinIndex;
	for( nSkinIndex = (INT)SKIN_TICS ; nSkinIndex < SKIN_INVALID; nSkinIndex ++ )
	{
		if( this->m_pSkinList[nSkinIndex] )
			delete this->m_pSkinList[nSkinIndex];
	}
}

//================================================================================================= E. CON/DCON

BEGIN_MESSAGE_MAP(ClSlider, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

//================================================================================================= S. MESSAGE

void ClSlider::InitToolTip( VOID )
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
void ClSlider::SetTooltipText(LPCTSTR lpszText, BOOL bActivate)
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
void ClSlider::ActivateTooltip(BOOL bActivate)
{
	// If there is no tooltip then do nothing
	if (m_ToolTip.GetToolCount() == 0) return;

	// Activate tooltip
	m_ToolTip.Activate(bActivate);
} // End of EnableTooltip

void ClSlider::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(rect);
	ClMemDC memDC( &dc, &rect, TRUE );

	if(memDC.m_hDC)
	{
		memDC.BitBlt( 0, 0, rect.Width(), rect.Height(), &this->m_dcBackground, 0, 0, SRCCOPY );

		ITER_BG iBG = this->m_BackgroundList.begin();
		ITER_BG iBGEnd = this->m_BackgroundList.end();
		for( ; iBG != iBGEnd; iBG++ )
		{
			(*iBG)->prepareImage( &memDC, rect );
			(*iBG)->drawImage( &memDC, rect );
		}

		if( this->m_pSkinList[(INT)SKIN_TICS] )
		{
			this->m_pSkinList[(INT)SKIN_TICS]->prepareImage( &memDC, rect, NULL, TRUE );
			this->m_pSkinList[(INT)SKIN_TICS]->drawImage( &memDC, rect );
		}

		ClGeometry geoBackTrack;
		this->GetTrackArea( geoBackTrack );

		if( !rect.EqualRect(this->m_rtPrevRect) )
		{
			if( this->m_pSkinList[(INT)SKIN_BTRACK] )
			{
				this->m_pSkinList[(INT)SKIN_BTRACK]->setUpdate();
				this->m_pSkinList[(INT)SKIN_BTRACK]->setGeometry( geoBackTrack );
			}

			if( this->m_pSkinList[(INT)SKIN_TRACK] )
			{
				this->m_pSkinList[(INT)SKIN_TRACK]->setUpdate();
				this->m_pSkinList[(INT)SKIN_TRACK]->setGeometry( geoBackTrack );
			}

			this->m_rtPrevRect = rect;

			if( this->m_bIsVertical )
				this->m_dbPagePixel = (double)rect.Height() / (this->m_nMax - this->m_nMin) * this->m_nPage;
			else
				this->m_dbPagePixel = (double)rect.Width() / (this->m_nMax - this->m_nMin) * this->m_nPage;
		}

		if( this->m_pSkinList[(INT)SKIN_BTRACK] )
		{
			this->m_pSkinList[(INT)SKIN_BTRACK]->prepareImage( &memDC, rect, NULL, TRUE );
			this->m_pSkinList[(INT)SKIN_BTRACK]->drawImage( &memDC, rect, &geoBackTrack );
		}

		ClGeometry geoTrack, geoSrcTrack, geoThumb;
		this->CalcGeometry( geoTrack, geoSrcTrack, geoThumb );

		if( this->m_pSkinList[(INT)SKIN_TRACK] )
		{
			this->m_pSkinList[(INT)SKIN_TRACK]->prepareImage( &memDC, rect, NULL, TRUE );
			this->m_pSkinList[(INT)SKIN_TRACK]->drawImage( &memDC, rect, &geoTrack, &geoSrcTrack );
		}

		if( this->m_pSkinList[(INT)SKIN_THUMB] )
		{
			this->m_pSkinList[(INT)SKIN_THUMB]->prepareImage( &memDC, rect, NULL, TRUE );
			this->m_pSkinList[(INT)SKIN_THUMB]->drawImage( &memDC, rect, &geoThumb );
		}
	}

	return;
}


BOOL ClSlider::OnEraseBkgnd( CDC* pDC )
{
	UNREFERENCED_PARAMETER( pDC );

	return FALSE;
}


void ClSlider::OnLButtonDown( UINT nFlags, CPoint point )
{
	this->m_bIsButtonDown = TRUE;
	SetCapture();

	this->m_nMouseOffset = 0;
	if( this->m_geoThumb.isInGeometry(ClPoint(point.x,point.y)) )
	{
		if( this->m_bIsVertical )
			this->m_nMouseOffset = (this->m_geoThumb.y + this->m_geoThumb.height / 2) - point.y;
		else
			this->m_nMouseOffset = (this->m_geoThumb.x + this->m_geoThumb.width / 2) - point.x;
	}
	OnMouseMove( nFlags, point );

	CStatic::OnLButtonDown( nFlags, point );
}


void ClSlider::OnMouseMove( UINT nFlags, CPoint point )
{
	if( !this->m_bIsButtonDown )
		return;

	INT nPixel = 0;
	INT nPos = 0;
	ClGeometry geoTrack;
	this->GetTrackArea( geoTrack );

	if( this->m_bIsVertical )
	{
		nPixel = point.y - geoTrack.y;

		if( nPixel < 0 )
			nPixel = 0;
		if( nPixel > geoTrack.height )
			nPixel = geoTrack.height;

		nPos = nPixel * (this->m_nMax - this->m_nMin) / geoTrack.height;
	}
	else
	{
		nPixel = point.x - geoTrack.x;

		if( nPixel < 0 )
			nPixel = 0;
		if( nPixel > geoTrack.width )
			nPixel = geoTrack.width;

		nPos = nPixel * (this->m_nMax - this->m_nMin) / geoTrack.width;
	}

	nPos += this->m_nMin;

	if( this->GetPos() != nPos )
	{
		this->SetPos( nPos );
		this->GetParent()->PostMessage( WM_CLENAW_SLIDER, (WPARAM)this, (LPARAM)m_nPos );
	}

	CStatic::OnMouseMove( nFlags, point );
}


void ClSlider::OnLButtonUp( UINT nFlags, CPoint point )
{
	this->m_bIsButtonDown = FALSE;
	this->m_nMouseOffset = 0;
	ReleaseCapture();

	CStatic::OnLButtonUp( nFlags, point );
}


void ClSlider::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	CStatic::OnKeyDown( nChar, nRepCnt, nFlags );
}

//================================================================================================= E. MESSAGE
//================================================================================================= S. VIRTUAL

void ClSlider::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	UNREFERENCED_PARAMETER( lpDrawItemStruct );

	return;
}

BOOL ClSlider::PreTranslateMessage(MSG* pMsg) 
{
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);
	
	return CStatic::PreTranslateMessage(pMsg);
} // End of PreTranslateMessage

VOID ClSlider::TransferBackground( CDC* pDC )
{
	this->m_dcBackground.DeleteDC();

	CBitmap bmp, *pOldBitmap;
	CRect rtWindow;
	GetWindowRect(rtWindow);
	this->GetParent()->ScreenToClient( rtWindow );

	m_dcBackground.CreateCompatibleDC( pDC );
	bmp.CreateCompatibleBitmap( pDC, rtWindow.Width(), rtWindow.Height() );
	pOldBitmap = m_dcBackground.SelectObject( &bmp );
	m_dcBackground.BitBlt( 0, 0, rtWindow.Width(), rtWindow.Height(), pDC, rtWindow.left, rtWindow.top, SRCCOPY );
	bmp.DeleteObject();

	return;
}

//================================================================================================= E. VIRTUAL
//================================================================================================= S. CONTROL

BOOL ClSlider::CalcGeometry( ClGeometry& geoTrack, ClGeometry& geoSrcTrack, ClGeometry& geoThumb )
{
	int nLength = 0;

	this->GetTrackArea( geoTrack );

	if( this->m_bIsVertical )
	{
		nLength = geoTrack.height * (this->m_nPos - this->m_nMin) / (this->m_nMax - this->m_nMin);

		geoTrack.y = geoTrack.getRight() - nLength;
		geoTrack.width = nLength;
		geoSrcTrack = ClGeometry( 0, geoTrack.y, geoTrack.width, geoTrack.height );

		if( this->m_pSkinList[(INT)SKIN_THUMB] )
		{
			geoThumb = this->m_pSkinList[(INT)SKIN_THUMB]->getGeometry();
			geoThumb.y = geoThumb.y + nLength + this->m_nMouseOffset;
			this->m_geoThumb = geoThumb;
		}
	}
	else
	{
		nLength = geoTrack.width * (this->m_nPos - this->m_nMin) / (this->m_nMax - this->m_nMin);

		geoTrack.width = nLength;
		geoSrcTrack = ClGeometry( 0, 0, geoTrack.width, geoTrack.height );

		if( this->m_pSkinList[(INT)SKIN_THUMB] )
		{
			geoThumb = this->m_pSkinList[(INT)SKIN_THUMB]->getGeometry();
			geoThumb.x = geoThumb.x + nLength + this->m_nMouseOffset;
			this->m_geoThumb = geoThumb;
		}
	}

	return TRUE;
}


VOID ClSlider::GetTrackArea( ClGeometry& geoTrack )
{
	CRect rect;
	this->GetClientRect( rect );

	geoTrack = ClGeometry( this->m_rtMargin.left, this->m_rtMargin.top,
		rect.Width() - (this->m_rtMargin.left + this->m_rtMargin.right),
		rect.Height() - (this->m_rtMargin.top + this->m_rtMargin.bottom) );

	return;
}

//================================================================================================= E. CONTROL
//================================================================================================= S. GET/SET

void ClSlider::GetRange( int &nMin, int &nMax )
{
	nMin = m_nMin;
	nMax = m_nMax;

	return;
}


void ClSlider::SetRange( int nMin, int nMax, BOOL bRedraw )
{
	SetRangeMin( nMin, FALSE );
	SetRangeMax( nMax, bRedraw );

	return;
}


int ClSlider::GetRangeMin( void )
{
	return m_nMin;
}


void ClSlider::SetRangeMin( int nMin, BOOL bRedraw )
{
	m_nMin = nMin;

	if( bRedraw ) InvalidateRect( NULL );

	return;
}


int ClSlider::GetRangeMax( void )
{
	return m_nMax;
}


void ClSlider::SetRangeMax( int nMax, BOOL bRedraw )
{
	m_nMax = nMax;

	if( bRedraw ) InvalidateRect( NULL );

	return;
}


int ClSlider::GetPos( void )
{
	return m_nPos;
}


void ClSlider::SetPos( int nPos )
{
	m_nPos = nPos;

	if( m_nPos > m_nMax ) m_nPos = m_nMax;
	if( m_nPos < m_nMin ) m_nPos = m_nMin;

	Invalidate();

	return;
}


int ClSlider::GetPageSize( void )
{
	return this->m_nPage;
}


int ClSlider::SetPageSize( int nSize )
{
	int nRet = m_nPage;
	m_nPage = nSize;

	return nRet;
}


void ClSlider::cludeSetProperty( int nMin, int nMax, int nPos, int nPageSize, const CRect& rtMargin )
{
	if(		(nMin >= nMax)
		||	(nPos < nMin)
		||	(nPos > nMax)
		||	(nPageSize>(nMax-nMin)) )
	{
		CLD_FL( DF_ERROR, "Given slider parameter is invalid." );
		return;
	}

	this->m_nMin	= nMin;
	this->m_nMax	= nMax;
	this->m_nPos	= nPos;
	this->m_nPage	= nPageSize;

	this->m_rtMargin= rtMargin;

	if( this->m_rtMargin.left	< 0 ) this->m_rtMargin.left		= 0;
	if( this->m_rtMargin.top	< 0 ) this->m_rtMargin.top		= 0;
	if( this->m_rtMargin.right	< 0 ) this->m_rtMargin.right	= 0;
	if( this->m_rtMargin.bottom	< 0 ) this->m_rtMargin.bottom	= 0;

	return;
}


BOOL ClSlider::SetSkin( ClSlider::SKIN_TYPE type, const HashedString& imagePath, Color color, BOOL bUseTpColor, Color tpColor,
					   ClDrawImage::DRAW_MODE drawMode, const ClGeometry& geoImage, const ClGeometry& geoSubImage,
					   DWORD dwGeoBase )
{
	if( type == SKIN_INVALID )
		return FALSE;

	if( !this->m_pSkinList[type] )
	{
		this->m_pSkinList[type] = new ClDrawImage();
		if( !this->m_pSkinList[type] )
			return FALSE;
	}

	if( !this->m_pSkinList[type]->loadImage(imagePath, color, !!bUseTpColor, tpColor, drawMode, geoImage, geoSubImage, dwGeoBase) )
	{
		delete this->m_pSkinList[type];
		this->m_pSkinList[type] = NULL;
		return FALSE;
	}

	return TRUE;
}


ClDrawImage* ClSlider::GetSkin( ClSlider::SKIN_TYPE type )
{
	if( type == SKIN_INVALID )
		return NULL;

	return this->m_pSkinList[type];
}


VOID ClSlider::ClearSkins( VOID )
{
	INT nSkinIndex;
	for( nSkinIndex = (INT)SKIN_TICS ; nSkinIndex < SKIN_INVALID; nSkinIndex ++ )
	{
		if( this->m_pSkinList[nSkinIndex] )
			delete this->m_pSkinList[nSkinIndex];
	}

	return;
}


BOOL ClSlider::SetBackground( const HashedString& imagePath, Color color, BOOL bUseTpColor, Color tpColor,
								ClDrawImage::DRAW_MODE drawMode, const ClGeometry& geoImage, const ClGeometry& geoSubImage,
								DWORD dwGeoBase )
{
	ClDrawImage* pNewBackground = new ClDrawImage();
	if( !pNewBackground )
	{
		CLD_FL( DF_ERROR, "Out of Memory");
		return FALSE;
	}

	if( !pNewBackground->loadImage(imagePath, color, !!bUseTpColor, tpColor, drawMode, geoImage, geoSubImage, dwGeoBase) )
	{
		CLDW_FL( DF_ERROR, TEXT("Cannot load background image %s"), imagePath.c_str() );
		delete pNewBackground;
		return FALSE;
	}

	this->m_BackgroundList.push_back( pNewBackground );

	return TRUE;
}


ClDrawImage* ClSlider::GetBackground( INT nIndex )
{
	if( nIndex < 0 )
		return NULL;

	ITER_BG iBG = this->m_BackgroundList.begin();
	std::advance( iBG, nIndex );
	if( iBG == this->m_BackgroundList.end() )
		return NULL;

	return (*iBG);
}


VOID ClSlider::ClearBackgrounds( VOID )
{
	ITER_BG iBG = this->m_BackgroundList.begin();
	ITER_BG iBGEnd = this->m_BackgroundList.end();
	for( ; iBG != iBGEnd; iBG++ )
		if( *iBG ) delete (*iBG);

	this->m_BackgroundList.clear();

	return;
}


void ClSlider::SetVertical( BOOL bVertical )
{
	this->m_bIsVertical = bVertical;
}


BOOL ClSlider::IsVertical( void )
{
	return this->m_bIsVertical;
}

//================================================================================================= E. GET/SET
