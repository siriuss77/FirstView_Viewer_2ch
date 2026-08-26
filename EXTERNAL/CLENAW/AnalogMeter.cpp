// AnalogMeter.cpp : implementation file
//

#include "stdafx.h"
#include <math.h>
#include <CLENAW/ClDialog.h>
#include <HELPER/ClLog.H>

#include "AnalogMeter.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ROUND(x) (int)((x) + 0.5 - (double)((x) < 0))

/////////////////////////////////////////////////////////////////////////////
// CAnalogMeter

IMPLEMENT_DYNCREATE(CAnalogMeter, CCmdTarget)

CAnalogMeter::CAnalogMeter()
{
	m_dPI = 4.0*atan(1.0) ;  // for trig calculations
	
	// initialized rectangle locations, will be modified on first drawing
	m_rectDraw = CRect(0, 0, 0, 0) ;
	m_nRectWidth = 0;
	m_nRectHeight = 0;

	// draw the whole thing the first time 
	m_boolForceRedraw = TRUE ;

	m_boolUseGrid = FALSE; //
	m_boolUseBG = TRUE; //
	
	m_dRadiansPerValue = 0.0 ;  // will be modified on first drawing

	// FALSE if we are printing
	m_boolUseBitmaps = TRUE ;

	m_dLimitAngleDeg =135.0 ;	//45.0 ;  // this specifies the width of the pie slice
	
	// default titles, scaling and needle position
	m_dMinScale = -10.0 ;       
	m_dMaxScale = 10.0 ; 
	m_dNeedlePos = 0.0 ;
	m_strTitle.Format(_T("Volts")) ;

	// for numerical values
	m_nRangeDecimals = 1 ;
	m_nValueDecimals = 3 ;

	//
	m_nMeterScale = 1000;
	
	// grid color
	m_colorGrid = RGB(128, 128, 128) ;
	// current numerical value color
	m_colorValue = RGB(255, 255, 255) ;
	// needle color
	m_colorNeedle = RGB(0, 255, 0) ;

	// background color brushes (for erasing)
	//m_brushErase.CreateSolidBrush(RGB(255, 255, 255)) ;
	//m_penErase.CreatePen(PS_SOLID, 0, RGB(255, 255, 255)) ;
	m_brushErase.CreateSolidBrush(RGB(0, 0, 0)) ;
	m_penErase.CreatePen(PS_SOLID, 0, RGB(0, 0, 0)) ;


	this->m_pBackground		= NULL;
}

CAnalogMeter::~CAnalogMeter()
{
	if(m_boolUseGrid)
	{
		m_dcGrid.SelectObject(m_pbitmapOldGrid) ;
		m_dcGrid.DeleteDC() ;

		m_bitmapGrid.DeleteObject() ;
	}

	m_dcNeedle.SelectObject(m_pbitmapOldNeedle) ;
	m_dcNeedle.DeleteDC() ;

	m_bitmapNeedle.DeleteObject() ;

	if( this->m_pBackground )	delete this->m_pBackground;
}


BEGIN_MESSAGE_MAP(CAnalogMeter, CCmdTarget)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnalogMeter message handlers
VOID CAnalogMeter::TransferBackground( CDC* pDC )
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

//	INT nCount;
//	for( nCount = (INT)SKIN_DIGIT; nCount < (INT)SKIN_INVALID; nCount++ )
//		if( this->m_pSkinList[nCount] ) this->m_pSkinList[nCount]->setUpdate();
	if( this->m_pBackground ) this->m_pBackground->setUpdate();
	
	return;
}

void CAnalogMeter::OnPaint()
{
	CRect rectBorder;
	CPaintDC dc(this);
	GetClientRect(rectBorder);	

	// draw the new needle image
	DrawNeedle () ;

	// combine the needle with the grid and display the result
	ShowMeterImage (&dc) ;
	return;
}


BOOL CAnalogMeter::OnEraseBkgnd( CDC* pDC )
{
	UNREFERENCED_PARAMETER( pDC );

	return FALSE;
}

/////////////////////////////////////////
BOOL CAnalogMeter::SetBackground( const HashedString& imagePath, Color color, BOOL bUseTpColor, Color tpColor,
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

	this->m_pBackground = pNewBackground;

	return TRUE;
}


ClDrawImage* CAnalogMeter::GetBackground( void )
{
	return this->m_pBackground;
}




void CAnalogMeter::ShowMeter(void)
{
	CRect rectBorder;
	CDC * pDC;
	
	//CClientDC dc(this) ;  
	CPaintDC dc(this);

	pDC = &dc;
	GetClientRect(rectBorder);	

	// check for a new meter or a resize of the old one.
	// (if the rectangles have changed, then redraw from scratch).
	// If we are printing, always draw from scratch without bitmaps.

	//if (m_rectOwner != rectBorder) 
		m_boolForceRedraw = TRUE ;

	if (m_boolForceRedraw || (pDC->IsPrinting()))
	{
		m_boolForceRedraw = FALSE ;
		// first store the rectangle for the owner
		// and determine the rectangle to draw to
		m_rectOwner = rectBorder ;
		if (pDC->IsPrinting())  
		{
			m_boolUseBitmaps = FALSE ;
			m_rectDraw = m_rectOwner ;  // draw directly to the owner
		}
		else  
		{
			m_boolUseBitmaps = TRUE ;
			m_rectDraw.left = 0 ;       // draw to a bitmap rectangle
			m_rectDraw.top = 0 ;
			m_rectDraw.right = rectBorder.Width() ;
			m_rectDraw.bottom = rectBorder.Height() ;
		}
		m_nRectWidth = m_rectDraw.Width() ;
		m_nRectHeight = m_rectDraw.Height() ;

		// if we already have a memory dc, destroy it 
		// (this occurs for a re-size of the meter)
		if(m_boolUseGrid && m_dcGrid.GetSafeHdc())
		{
			m_dcGrid.SelectObject(m_pbitmapOldGrid) ;
			m_dcGrid.DeleteDC() ;

			m_bitmapGrid.DeleteObject() ;
		}
		
		if (m_dcNeedle.GetSafeHdc())
		{
			m_dcNeedle.SelectObject(m_pbitmapOldNeedle) ;
			m_dcNeedle.DeleteDC() ;
			
			m_bitmapNeedle.DeleteObject() ;
		}

		if (m_boolUseBitmaps)  
		{
			if(m_boolUseGrid)
			{
				// create a memory based dc for drawing the grid
				m_dcGrid.CreateCompatibleDC(pDC) ;
				m_bitmapGrid.CreateCompatibleBitmap(pDC, m_nRectWidth, m_nRectHeight) ;
				m_pbitmapOldGrid = m_dcGrid.SelectObject(&m_bitmapGrid) ;
			}
			
			// create a memory based dc for drawing the needle
			m_dcNeedle.CreateCompatibleDC(pDC) ;
			m_bitmapNeedle.CreateCompatibleBitmap(pDC, m_nRectWidth, m_nRectHeight) ;

	       m_pbitmapOldNeedle = m_dcNeedle.SelectObject(&m_bitmapNeedle) ;
		}
		else  // no bitmaps, draw to the destination
		{
			if(m_boolUseGrid)
			{
				// use the destination dc for the grid
				m_dcGrid.m_hDC = pDC->m_hDC ;
				m_dcGrid.m_hAttribDC = pDC->m_hAttribDC ;
			}
			// use the destination dc for the grid
			m_dcNeedle.m_hDC = pDC->m_hDC ;
			m_dcNeedle.m_hAttribDC = pDC->m_hAttribDC ;
		}

		CalculationValue();
		
		// draw the grid in the to the "grid dc"
		if(m_boolUseGrid)
			DrawGrid () ;           
		// draw the needle in the "needle dc" 
		DrawNeedle () ;    
	}

	// display the new image, combining the needle with the grid
	if (m_boolUseBitmaps)
		ShowMeterImage(pDC);
} // end ShowMeter


void CAnalogMeter::ShowMeterImage(CDC *pDC)
{
	// this function is only used when the needle and grid
	// have been drawn to bitmaps and they need to be combined
	// and sent to the destination
	if (!m_boolUseBitmaps)  
		return ;

	if (!m_dcNeedle.GetSafeHdc())
		return ;
	
	if(m_boolUseGrid)
	{
		CDC memDC ;
		CBitmap memBitmap ;
		CBitmap* oldBitmap ; // bitmap originally found in CMemDC

		// to avoid flicker, establish a memory dc, draw to it 
		// and then BitBlt it to the destination "pDC"
		memDC.CreateCompatibleDC(pDC) ;
		memBitmap.CreateCompatibleBitmap(pDC, m_nRectWidth, m_nRectHeight) ;
		oldBitmap = (CBitmap *)memDC.SelectObject(&memBitmap) ;

		// make sure we have the bitmaps
		if (!m_dcGrid.GetSafeHdc())
			return ;
		
		if (memDC.GetSafeHdc() != NULL)
		{
			// draw the inverted grid
			memDC.BitBlt(0, 0, m_nRectWidth, m_nRectHeight, &m_dcGrid, 0, 0, NOTSRCCOPY) ;
			// merge the needle image with the grid
			memDC.BitBlt(0, 0, m_nRectWidth, m_nRectHeight, &m_dcNeedle, 0, 0, SRCINVERT) ;
			// copy the resulting bitmap to the destination
			pDC->BitBlt(m_rectOwner.left, m_rectOwner.top, m_nRectWidth, m_nRectHeight, 
							    &memDC, 0, 0, SRCCOPY) ;
		}

		memDC.SelectObject(oldBitmap) ;
	}
	else
	{
		pDC->BitBlt(m_rectOwner.left, m_rectOwner.top, m_nRectWidth, m_nRectHeight, 
					&m_dcNeedle, 0, 0, SRCCOPY) ;
	}
#if 1
	CFont *oldFont ;
	COLORREF oldColor;
	COLORREF oldBkColor;
	CString tempString;
	int oldTextAlign;
	
	oldFont = pDC->SelectObject(&m_fontValue) ;
	oldColor = pDC->GetTextColor();
	oldBkColor = pDC->GetBkColor();
	
	pDC->SetTextColor(m_colorValue) ;
	pDC->SetBkColor(m_colorGrid);
	
	oldTextAlign = pDC->GetTextAlign();
	pDC->SetTextAlign(TA_CENTER|TA_BASELINE) ;
	
	tempString.Format(_T("%.*f"), m_nValueDecimals, m_dNeedlePos) ;
	pDC->TextOut (m_rectOwner.left + (m_rectValue.Width()/2), 
			                 m_rectOwner.bottom-m_nTextBaseSpacing, tempString) ;

	pDC->SetTextAlign(oldTextAlign);
	pDC->SetTextColor(oldColor) ;
	pDC->SetBkColor(oldBkColor) ;
	pDC->SelectObject(oldFont) ;
#endif	
	//InvalidateRect(GetForegroundWindow(), m_rectOwner ,FALSE); // window active 안되면 갱신 안됨
} // end ShowMeterImage


//////////////////////////////////////////////////////
void CAnalogMeter::UpdateNeedle(double dPos, bool bRedraw)
{
	// do not support updates if we are not working with 
	// bitmaps images
	if (!m_boolUseBitmaps)
		return ;

	// must have created the grid if we are going to 
	// update the needle (the needle locations are 
	// calculateed based on the grid)
	if(m_boolUseGrid)
	{
		if (!m_dcGrid.GetSafeHdc())
			return ;
	}

	// if the needle hasn't changed, don't bother updating
	if (m_dNeedlePos == dPos && !bRedraw)
		return ;

	// store the position in the member variable 
	// for availability elsewhere
	m_dNeedlePos = dPos ;

	//InvalidateRect( NULL );
	Invalidate();

} // end UpdateNeedle

void CAnalogMeter::CalculationValue () 
{
	int nFontHeight ;
	double dTemp ;
	// determine the angular scaling
	m_dLimitAngleRad = m_dLimitAngleDeg*m_dPI/180.0 ;
	m_dRadiansPerValue = (2.0*m_dLimitAngleRad)/(m_dMaxScale-m_dMinScale) ;

	// determine the center point
	m_nCXPix = (m_rectDraw.left+m_rectDraw.right)/2 ;
	m_nCYPix = m_rectDraw.bottom - m_nRectHeight/2 ; //5 ;
	
	// determine the size and location of the meter "pie"
	m_nRadiusPix = m_nRectWidth*60/170 ; //100 ;
	m_nHalfBaseWidth = m_nRadiusPix/30 ; // 40
	dTemp = m_nCXPix - m_nRadiusPix*sin(m_dLimitAngleRad) ;
	m_nLeftLimitXPix = ROUND(dTemp) ;
	dTemp = m_nCYPix - m_nRadiusPix*cos(m_dLimitAngleRad) ;
	m_nLeftLimitYPix = ROUND(dTemp) ;

	dTemp = m_nCXPix + m_nRadiusPix*sin(m_dLimitAngleRad) ;
	m_nRightLimitXPix = ROUND(dTemp) ;
	m_nRightLimitYPix = m_nLeftLimitYPix ;

	// determine the placement of the current value text
	m_rectValue.left = m_rectDraw.left+1 ;
	m_rectValue.top = m_rectDraw.top+1 ;
	m_rectValue.right = m_rectDraw.right-1 ;
	if (m_boolUseBitmaps)
		m_rectValue.bottom = m_rectDraw.top+m_nCYPix - m_nRadiusPix - 1 ;
	else
		m_rectValue.bottom = m_nCYPix - m_nRadiusPix - 1 ;

	// determine the placement of the minimum value
	m_rectMinValue.left = m_rectDraw.left+1 ;
	m_rectMinValue.top = m_nCYPix - m_nRectHeight*3/20 ;
	m_rectMinValue.right = m_nLeftLimitXPix + 3*(m_nCXPix-m_nLeftLimitXPix)/4 ;
	m_rectMinValue.bottom = m_nCYPix + m_nRectHeight/20 ;

	// determine the placement of the maximum value
	m_rectMaxValue.right = m_rectDraw.right-1 ;
	m_rectMaxValue.top = m_nCYPix - m_nRectHeight*3/20 ;
	m_rectMaxValue.left = m_nRightLimitXPix - 3*(m_nRightLimitXPix-m_nCXPix)/4 ;
	m_rectMaxValue.bottom = m_nCYPix + m_nRectHeight/20 ;

	// create a font based on these sizes
	nFontHeight = m_rectMaxValue.Height()*3/5 ;  // modify the fraction to adjust
	m_fontValue.DeleteObject() ;
	m_fontValue.CreateFont (nFontHeight, 0, 0, 0, 400,
													FALSE, FALSE, 0, ANSI_CHARSET,
													OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
													DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, _T("Arial")) ;

	// grab the font and set the text color
	m_nTextBaseSpacing = m_rectMinValue.Height();

	
}
//////////////////////////////////////////
void CAnalogMeter::DrawGrid () 
{
	int nLeftBoundX, nRightBoundX,
			nLeftBoundY, nRightBoundY ;
	
	double dX, dY ;
	CPen penSolid, *oldPen ;
	CBrush brushSolid, *oldBrush ;
	CFont *oldFont ;
	CString tempString ;

	if(m_boolUseBG && m_pBackground){
		m_pBackground->prepareImage( &m_dcGrid, m_rectDraw );
		m_pBackground->drawImage( &m_dcGrid, m_rectDraw );
	}
	else
	{
		// draw the boundary rectangle and 
		// fill the entire area with the background color
		penSolid.CreatePen(PS_SOLID, 0, RGB(0, 0, 0)) ;
		oldPen = m_dcGrid.SelectObject(&penSolid) ;
		oldBrush = m_dcGrid.SelectObject(&m_brushErase) ;
		
		m_dcGrid.Rectangle(m_rectDraw) ;
	
		m_dcGrid.SelectObject(oldBrush) ;
		m_dcGrid.SelectObject(oldPen) ;
	}

	// grab the font and set the text color
	oldFont = m_dcGrid.SelectObject(&m_fontValue) ;
	m_dcGrid.SetTextColor(m_colorGrid) ;

	// show the title
	m_dcGrid.SetTextAlign(TA_CENTER|TA_BOTTOM) ;
	m_dcGrid.TextOut ((m_rectDraw.left+m_rectDraw.right)/2,
		                 m_rectDraw.bottom-1, m_strTitle) ;

	// show the max and min (limit) values
	m_dcGrid.SetTextAlign(TA_CENTER|TA_BASELINE) ;
	tempString.Format(_T("%.*f"), m_nRangeDecimals, m_dMinScale) ;
	m_dcGrid.TextOut ((m_rectMinValue.left+m_rectMinValue.right)/2, 
		                 m_rectMinValue.bottom-m_nTextBaseSpacing, tempString) ;
	tempString.Format(_T("%.*f"), m_nRangeDecimals, m_dMaxScale) ;
	m_dcGrid.TextOut ((m_rectMaxValue.left+m_rectMaxValue.right)/2, 
		                 m_rectMaxValue.bottom-m_rectMaxValue.Height()/4, tempString) ;

	// restore the font
	m_dcGrid.SelectObject(oldFont) ;

	// create the pen and brush for drawing
	penSolid.DeleteObject() ;
	penSolid.CreatePen(PS_SOLID, 0, m_colorGrid) ;
	// grab the pen
	oldPen = m_dcGrid.SelectObject(&penSolid) ;
	
	// determine the bounding rectangle for the pie slice
	// and draw it
	nLeftBoundX = m_nCXPix - m_nRadiusPix ;
	nRightBoundX = m_nCXPix + m_nRadiusPix ;
	nLeftBoundY = m_nCYPix - m_nRadiusPix ;
	nRightBoundY = m_nCYPix + m_nRadiusPix ;
	m_dcGrid.Pie(nLeftBoundX, nLeftBoundY, nRightBoundX+1, nRightBoundY+1,
						   m_nRightLimitXPix, m_nRightLimitYPix, m_nLeftLimitXPix, m_nLeftLimitYPix) ;

	// center tick mark
	m_dcGrid.MoveTo (m_nCXPix, m_nCYPix-m_nRadiusPix) ;
	m_dcGrid.LineTo (m_nCXPix, m_nCYPix-46*m_nRadiusPix/50) ;

	// left tick mark
	dX = m_nCXPix - m_nRadiusPix*sin(m_dLimitAngleRad/2) ;
	dY = m_nCYPix - m_nRadiusPix*cos(m_dLimitAngleRad/2) ;
	m_dcGrid.MoveTo(ROUND(dX), ROUND(dY)) ;
	dX = m_nCXPix - 46*m_nRadiusPix*sin(m_dLimitAngleRad/2)/50 ;
	dY = m_nCYPix - 46*m_nRadiusPix*cos(m_dLimitAngleRad/2)/50 ;
	m_dcGrid.LineTo(ROUND(dX), ROUND(dY)) ;

	// right tick mark
	dX = m_nCXPix + m_nRadiusPix*sin(m_dLimitAngleRad/2) ;
	dY = m_nCYPix - m_nRadiusPix*cos(m_dLimitAngleRad/2) ;
	m_dcGrid.MoveTo(ROUND(dX), ROUND(dY)) ;
	dX = m_nCXPix + 46*m_nRadiusPix*sin(m_dLimitAngleRad/2)/50 ;
	dY = m_nCYPix - 46*m_nRadiusPix*cos(m_dLimitAngleRad/2)/50 ;
	m_dcGrid.LineTo(ROUND(dX), ROUND(dY)) ;


	// draw circle at the bottom
	brushSolid.CreateSolidBrush (m_colorGrid) ;
	oldBrush = m_dcGrid.SelectObject(&brushSolid) ;
	m_dcGrid.Ellipse (m_nCXPix-m_nHalfBaseWidth, m_nCYPix-m_nHalfBaseWidth,
		                m_nCXPix+m_nHalfBaseWidth+1, m_nCYPix+m_nHalfBaseWidth+1) ;

	m_dcGrid.SelectObject(oldBrush) ;
	m_dcGrid.SelectObject(oldPen) ;

} // end DrawGrid

///////////////////////////////////		
void CAnalogMeter::DrawNeedle()
{
	CPoint pPoints[6] ;
	CString tempString ;
//	CFont *oldFont ;
	CPen *oldPen, solidPen ;
	CBrush *oldBrush, solidBrush ;
	double dAngleRad, dX, dY ;
	double dCosAngle, dSinAngle ;

	if (!m_dcNeedle.GetSafeHdc())
		return ;

	if (m_boolUseBitmaps)
	{
		if(m_pBackground){
			m_pBackground->prepareImage( &m_dcNeedle, m_rectDraw );
			m_pBackground->drawImage( &m_dcNeedle, m_rectDraw );
		}
		else
		{
			oldPen = m_dcNeedle.SelectObject(&m_penErase) ;
			oldBrush = m_dcNeedle.SelectObject(&m_brushErase) ;

			m_dcNeedle.Rectangle(m_rectDraw) ;

			m_dcNeedle.SelectObject(oldBrush) ;
			m_dcNeedle.SelectObject(oldPen) ;
		}
	}
#if 0
	oldFont = m_dcNeedle.SelectObject(&m_fontValue) ;

	m_dcNeedle.SetTextAlign(TA_CENTER|TA_BASELINE) ;
	m_dcNeedle.SetTextColor(m_colorValue) ;
	tempString.Format("%.*f", m_nValueDecimals, m_dNeedlePos) ;
	m_dcNeedle.TextOut ((m_rectValue.right+m_rectValue.left)/2, 
			                 m_rectValue.bottom-m_nTextBaseSpacing, tempString) ;

	m_dcNeedle.SelectObject(oldFont) ;
#endif

	dAngleRad = ((m_dNeedlePos / m_nMeterScale) - m_dMinScale)*m_dRadiansPerValue 
			        - m_dLimitAngleRad ;
	dAngleRad = max(dAngleRad, -m_dLimitAngleRad) ;
	dAngleRad = min(dAngleRad, m_dLimitAngleRad) ;
	dCosAngle = cos(dAngleRad) ;
	dSinAngle = sin(dAngleRad) ;

	// tip
	dX = m_nCXPix + m_nRadiusPix*dSinAngle ;
	dY = m_nCYPix - m_nRadiusPix*dCosAngle ;
	pPoints[0].x = ROUND(dX) ;
	pPoints[0].y = ROUND(dY) ;

	// left base
	dX = m_nCXPix - m_nHalfBaseWidth*dCosAngle ;
	dY = m_nCYPix - m_nHalfBaseWidth*dSinAngle ;
	pPoints[1].x = ROUND(dX) ;
	pPoints[1].y = ROUND(dY) ;

	// right base
	pPoints[2].x = m_nCXPix + (m_nCXPix-pPoints[1].x) ;
	pPoints[2].y = m_nCYPix + (m_nCYPix-pPoints[1].y) ;

	// tip
	pPoints[3].x = pPoints[0].x ;
	pPoints[3].y = pPoints[0].y ;

	solidPen.CreatePen (PS_SOLID, 0, m_colorNeedle) ;
	solidBrush.CreateSolidBrush (m_colorNeedle) ;
	oldPen = m_dcNeedle.SelectObject(&solidPen) ;
	oldBrush = m_dcNeedle.SelectObject(&solidBrush) ;

	// draw the needle pointer
	m_dcNeedle.Polygon(pPoints, 4) ;

	// draw the circle at the bottom of the needle
	CPen solidPenEl ;
	CBrush solidBrushEl ;
	solidPenEl.CreatePen (PS_SOLID, 0, RGB(150,150,150)) ;
	solidBrushEl.CreateSolidBrush (RGB(220,220,220)) ;
	m_dcNeedle.SelectObject(&solidPenEl);
	m_dcNeedle.SelectObject(&solidBrushEl);
	m_dcNeedle.Ellipse (m_nCXPix-(m_nHalfBaseWidth+2), m_nCYPix-(m_nHalfBaseWidth+2),
		                  m_nCXPix+m_nHalfBaseWidth+4, m_nCYPix+m_nHalfBaseWidth+4) ;

	m_dcNeedle.SelectObject(oldPen) ;
	m_dcNeedle.SelectObject(oldBrush) ;
} // end DrawNeedle

//////////////////////////////////////////////////////
void CAnalogMeter::SetMeterScale(int nDivision)
{
	m_nMeterScale = nDivision;
}

//////////////////////////////////////////////////////
void CAnalogMeter::SetColor(COLORREF grid, COLORREF value, COLORREF needle)
{
	// grid color
	m_colorGrid = grid;
	// current numerical value color
	m_colorValue = value ;
	// needle color
	m_colorNeedle = needle;
}

//////////////////////////////////////////////////////
void CAnalogMeter::SetRange(double dMin, double dMax)
{
	// Note, this only changes the plotting range. 
	// It does NOT force the re-drawing of the meter.
	// The owner must explicitly call the ShowMeter function
	// to get the new range values to display.
	m_dMinScale = dMin ;
	m_dMaxScale = dMax ;
	m_boolForceRedraw = TRUE ;

}

//////////////////////////////////////////////////////
void CAnalogMeter::SetRangeDecimals(int nRangeDecimals)
{
	m_nRangeDecimals = nRangeDecimals;
	m_boolForceRedraw = TRUE ;

}

//////////////////////////////////////////////////////
void CAnalogMeter::SetValueDecimals(int nValueDecimals)
{
	m_nValueDecimals = nValueDecimals;
	m_boolForceRedraw = TRUE ;

}

//////////////////////////////////////////////////////
void CAnalogMeter::SetTitle(CString strTitle)
{
	m_strTitle = strTitle ;
	m_boolForceRedraw = TRUE ;

}
