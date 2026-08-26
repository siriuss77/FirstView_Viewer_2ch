//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClGSensorGraph.CPP
//
//	Description: G-Sensor graph control
//
//	Update History
// 
//		2011.10.10 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. INCLUDE

#include "stdafx.h"
#include "BBPlayer.h"
#include "ClGSensorGraph.h"

//================================================================================================= E. INCLUDE
//================================================================================================= S. CON/DCON
IMPLEMENT_DYNAMIC(ClGSensorGraph, CStatic)

ClGSensorGraph::ClGSensorGraph()
{
	this->m_pBackground		= NULL;
	this->m_dwCTS			= 0;
	this->m_pSensorData		= NULL;

	for( int lCount = 0; lCount < (int)LINE_INVALID; lCount++ )
	{
		this->m_LineInfo[lCount].m_color = Color(255,255,255);
		this->m_LineInfo[lCount].m_nWidth = 1;
	}

	this->m_nTimeLineProp[0] = 0;
	this->m_nTimeLineProp[1] = 0;
	this->m_nTimeLineProp[2] = 0;
	this->m_nTimeLineProp[3] = 0;
}


ClGSensorGraph::~ClGSensorGraph()
{
	if( this->m_pBackground )		delete this->m_pBackground;
	if( this->m_dcGraph )			this->m_dcGraph.DeleteDC();
}

//================================================================================================= S. MESSAGE

BEGIN_MESSAGE_MAP(ClGSensorGraph, CStatic)
END_MESSAGE_MAP()

//================================================================================================= E. MESSAGE
//================================================================================================= S. VIRTUAL

void ClGSensorGraph::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	CDC drawItemDC;
	drawItemDC.Attach( lpDrawItemStruct->hDC );

	CRect rect;
	GetClientRect(rect);
	ClMemDC memDC( &drawItemDC, &rect, TRUE );

	if(memDC.m_hDC)
	{
		if( this->m_dcGraph.m_hDC == NULL )
		{
			if( this->m_pBackground )
			{
				this->m_pBackground->prepareImage( &memDC, rect, &this->m_dcBackground );
				this->m_pBackground->drawImage( &memDC, rect );
			}

			this->DrawGraph( &memDC );

			CBitmap compBitmap;
			this->m_dcGraph.CreateCompatibleDC( &drawItemDC );
			compBitmap.CreateCompatibleBitmap( &drawItemDC, rect.Width(), rect.Height() );
			this->m_dcGraph.SelectObject( &compBitmap );
			this->m_dcGraph.BitBlt( 0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY );
			compBitmap.DeleteObject();
		}
		else
		{
			memDC.BitBlt( 0, 0, rect.Width(), rect.Height(), &this->m_dcGraph, 0, 0, SRCCOPY );
			this->DrawTimeLine( &memDC, this->m_dwCTS );
		}
	}

	return;
}

//================================================================================================= E. VIRTUAL
//================================================================================================= S. OPERATION

void ClGSensorGraph::TransferBackground( CDC* pDC )
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


void ClGSensorGraph::DrawTimeLine( CDC * pDC, DWORD dwCTS )
{
	CRect rectGraph( this->m_nTimeLineProp[0], this->m_nTimeLineProp[2],
		this->m_nTimeLineProp[1], this->m_nTimeLineProp[2] + this->m_nTimeLineProp[3] );

	int nBaseLineX = rectGraph.left;
	double fRateX = rectGraph.Width()/(60*1000.0); // width / 1 msec 

	if (dwCTS > 0)
	{
		CPen pen, *oldPen;
		COLORREF color = RGB( this->m_LineInfo[LINE_TIME].m_color.GetR(),
								this->m_LineInfo[LINE_TIME].m_color.GetG(),
								this->m_LineInfo[LINE_TIME].m_color.GetB() );
		pen.CreatePen(PS_SOLID, this->m_LineInfo[LINE_TIME].m_nWidth, color );
		oldPen = pDC->SelectObject(&pen);

		int x = int(nBaseLineX + (dwCTS * fRateX));

		pDC->MoveTo(x, rectGraph.top);
		pDC->LineTo(x, rectGraph.bottom);
		pDC->SelectObject(oldPen);
	}

	return;
}


void ClGSensorGraph::DrawGraph( CDC* pDC )
{
	if( this->m_pSensorData->m_GSensorList.size() > 0)
	{
		CRect rectGraph( this->m_nTimeLineProp[0], this->m_nTimeLineProp[2],
			this->m_nTimeLineProp[1], this->m_nTimeLineProp[2] + this->m_nTimeLineProp[3] );

		int nBaseLineX = rectGraph.left;
		int nBaseLineY = rectGraph.top + rectGraph.Height()/2;

		double fRateX = rectGraph.Width()/(60*1000.0);
		double fRateY = (rectGraph.Height()/4.0) ;  // gsensor +-2.0g setting

		CPen pen, *oldPen;
		COLORREF color = RGB( this->m_LineInfo[LINE_X].m_color.GetR(),
								this->m_LineInfo[LINE_X].m_color.GetG(),
								this->m_LineInfo[LINE_X].m_color.GetB() );
		pen.CreatePen( PS_SOLID, this->m_LineInfo[LINE_X].m_nWidth, color );
		oldPen = pDC->SelectObject(&pen);
		pDC->MoveTo(nBaseLineX, nBaseLineY);

		CTextData::ITER_GSENSOR iGSensor = this->m_pSensorData->m_GSensorList.begin();
		CTextData::ITER_GSENSOR iGSensorend = this->m_pSensorData->m_GSensorList.end();
		for( ; iGSensor != iGSensorend; iGSensor++ )
		{
			if( iGSensor->m_cts <= 60*1000)
			{
				int x = int(nBaseLineX + (iGSensor->m_cts * fRateX));
				int y = int(nBaseLineY - (iGSensor->m_gseonsorX * fRateY));

				pDC->LineTo(x, y);
			}
		}

		pen.DeleteObject();
		color = RGB( this->m_LineInfo[LINE_Y].m_color.GetR(),
			this->m_LineInfo[LINE_Y].m_color.GetG(),
			this->m_LineInfo[LINE_Y].m_color.GetB() );
		pen.CreatePen( PS_SOLID, this->m_LineInfo[LINE_Y].m_nWidth, color );
		pDC->SelectObject(&pen);
		pDC->MoveTo(nBaseLineX, nBaseLineY);
		iGSensor = this->m_pSensorData->m_GSensorList.begin();
		for( ; iGSensor != iGSensorend; iGSensor++ )
		{
			if( iGSensor->m_cts <= 60*1000 )
			{
				int x = int(nBaseLineX + (iGSensor->m_cts * fRateX));
				int y = int(nBaseLineY - (iGSensor->m_gseonsorY * fRateY));

				pDC->LineTo(x, y);
			}

		}

		pen.DeleteObject();
		color = RGB( this->m_LineInfo[LINE_Z].m_color.GetR(),
			this->m_LineInfo[LINE_Z].m_color.GetG(),
			this->m_LineInfo[LINE_Z].m_color.GetB() );
		pen.CreatePen( PS_SOLID, this->m_LineInfo[LINE_Z].m_nWidth, color );
		pDC->SelectObject(&pen);
		pDC->MoveTo(nBaseLineX, nBaseLineY);
		iGSensor = this->m_pSensorData->m_GSensorList.begin();
		for( ; iGSensor != iGSensorend; iGSensor++ )
		{
			if( iGSensor->m_cts <= 60*1000 )
			{
				int x = int(nBaseLineX + (iGSensor->m_cts * fRateX));
				int y = int(nBaseLineY - (iGSensor->m_gseonsorZ * fRateY));

				pDC->LineTo(x, y);
			}

		}

		pDC->SelectObject(oldPen);
	}

	return;
}

//================================================================================================= E. OPERATION
//================================================================================================= S. GET/SET

void ClGSensorGraph::SetCTS( DWORD dwCTS )
{
	this->m_dwCTS = dwCTS;
	this->InvalidateRect( NULL );
}


BOOL ClGSensorGraph::SetSensorData( CTextData* pSensorData )
{
	if( !pSensorData ) return FALSE;

	this->m_pSensorData = pSensorData;
	this->m_dcGraph.DeleteDC();

	return TRUE;
}


BOOL ClGSensorGraph::SetLine( LINE_TYPE type, Color color, INT nWidth )
{
	if( type == LINE_INVALID )
		return FALSE;

	this->m_LineInfo[type].m_color	= color;
	this->m_LineInfo[type].m_nWidth	= nWidth;

	return TRUE;
}


BOOL ClGSensorGraph::SetBackground( const HashedString& imagePath, Color color, BOOL bUseTpColor, Color tpColor,
									ClDrawImage::DRAW_MODE drawMode, const ClGeometry& geoImage, const ClGeometry& geoSubImage,
									DWORD dwGeoBase )
{
	if( !this->m_pBackground )
	{
		this->m_pBackground = new ClDrawImage();
		if( !this->m_pBackground )
			return FALSE;
	}

	if( !this->m_pBackground->loadImage(imagePath, color, !!bUseTpColor, tpColor, drawMode, geoImage, geoSubImage, dwGeoBase) )
	{
		delete this->m_pBackground;
		this->m_pBackground = NULL;
		return FALSE;
	}

	return TRUE;
}


ClDrawImage* ClGSensorGraph::GetBackground( VOID )
{
	return this->m_pBackground;
}


VOID ClGSensorGraph::SetTimeLineProperty( INT nStartPos, INT nEndPos, INT nTop, INT nHeight )
{
	if( nEndPos < nStartPos )
		return;

	this->m_nTimeLineProp[0] = nStartPos;
	this->m_nTimeLineProp[1] = nEndPos;
	this->m_nTimeLineProp[2] = nTop;
	this->m_nTimeLineProp[3] = nHeight;

	return;
}

//================================================================================================= E. GET/SET
