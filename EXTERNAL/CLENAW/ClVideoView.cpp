//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClVideoView.CPP
//
//	Description: CLENA-W Video view control
//
//	Update History
// 
//		2011.11.23 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. INCLUDE

#include <stdafx.h>
#include "ClVideoView.h"

#include <CLENAW/ClDialog.h>

//================================================================================================= E. INCLUDE
//================================================================================================= S. CON/DCON
IMPLEMENT_DYNAMIC(ClVideoView, CStatic)

#define ZOOM_SCALE_VALUE_DEFAULT		2.0
#define ZOOM_SCALE_VALUE_MAX			15.0
#define ZOOM_SCALE_VALUE_MIN			2.0

ClVideoView::ClVideoView()
{
	this->m_bIsHover	= FALSE;

	m_ZoomScale = 1.0;
	m_bMouseClick = FALSE;
	m_cpZoomMDown.x = 0;
	m_cpZoomMDown.y = 0;
	m_cpZoomBase.x = 0;
	m_cpZoomBase.y = 0;

	m_bQuadCtrlMode = FALSE;
	m_iQuadCtrlChannel = 0;
}

ClVideoView::~ClVideoView()
{
	
}

//================================================================================================= E. CON/DCON
//================================================================================================= S. MESSAGE

BEGIN_MESSAGE_MAP(ClVideoView, CStatic)
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

BOOL ClVideoView::OnEraseBkgnd( CDC* pDC )
{
	UNREFERENCED_PARAMETER( pDC );
	
	return FALSE;
}

//================================================================================================= E. MESSAGE
//================================================================================================= S. VIRTUAL

void ClVideoView::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	UNREFERENCED_PARAMETER( lpDrawItemStruct );

	return;
}

//================================================================================================= E. VIRTUAL
//================================================================================================= S. OPERATION

VOID ClVideoView::TransferBackground( CDC* pDC )
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


// mux view control
void ClVideoView::DrawGuidline(CDC* pDC)
{
	CRect  rectClient;
	CPen *oldPen ;
	CPen solidPen(PS_SOLID, 2, RGB(  200, 200, 200));
	CPen solidPenSel(PS_SOLID, 2, RGB(  121, 189, 239 ));
	
	GetClientRect(rectClient) ;
	
	// draw the plot base rectangle
	oldPen = pDC->SelectObject (&solidPen) ; 
	pDC->MoveTo (rectClient.left , rectClient.top) ;
	pDC->LineTo (rectClient.right - 1, rectClient.top + 1) ;
	pDC->LineTo (rectClient.right - 1, rectClient.bottom - 1) ;
	pDC->LineTo (rectClient.left + 1, rectClient.bottom - 1) ;
	pDC->LineTo (rectClient.left + 1, rectClient.top + 1) ;

	// draw the plot
	pDC->MoveTo (rectClient.left + 1, rectClient.top + 1 + rectClient.Height() / 2) ;
	pDC->LineTo (rectClient.right - 1, rectClient.top + 1 + rectClient.Height() / 2) ;

	pDC->MoveTo (rectClient.left + 1 + rectClient.top + 1 + rectClient.Width() / 2, rectClient.top + 1) ;
	pDC->LineTo (rectClient.left + 1 + rectClient.top + 1 + rectClient.Width() / 2, rectClient.bottom - 1) ;
		

	// draw the plot selected camera channel rectangle
	if(m_iQuadCtrlChannel == 0){
		rectClient.right -= rectClient.Width() / 2 - 3;
		rectClient.bottom -= rectClient.Height() / 2 - 2;
	}else if(m_iQuadCtrlChannel == 1){
		rectClient.left += rectClient.Width() / 2 + 1;
		rectClient.bottom -= rectClient.Height() / 2 - 2;
	}
	else if(m_iQuadCtrlChannel == 2){
		rectClient.top += rectClient.Height() / 2;
		rectClient.right -= rectClient.Width() / 2 - 3; 
	}
	else {
		rectClient.left += rectClient.Width() / 2 + 1;
		rectClient.top += rectClient.Height() / 2;
	}

	oldPen = pDC->SelectObject (&solidPenSel) ; 
	pDC->MoveTo (rectClient.left, rectClient.top) ;
	pDC->LineTo (rectClient.right - 1, rectClient.top + 1) ;
	pDC->LineTo (rectClient.right - 1, rectClient.bottom - 1) ;
	pDC->LineTo (rectClient.left + 1, rectClient.bottom - 1) ;
	pDC->LineTo (rectClient.left + 1, rectClient.top + 1) ;

	pDC->SelectObject (oldPen) ;
}

// zoom control
void ClVideoView::OnMouseHover(UINT nFlags, CPoint point)
{
	if( this->m_bIsHover )
		return;

	this->m_bIsHover = TRUE;

	CClientDC aDC(this);


	
	CStatic::OnMouseHover(nFlags, point);
}


void ClVideoView::OnMouseLeave()
{
	if( !this->m_bIsHover )
		return;

	this->m_bIsHover = FALSE;

	CClientDC aDC(this);


	
	CStatic::OnMouseLeave();
}


void ClVideoView::OnMouseMove(UINT nFlags, CPoint point)
{
	if(!m_bQuadCtrlMode){
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE|TME_HOVER;
		tme.dwHoverTime = 1;
		TrackMouseEvent(&tme);

		//printf("OnMouseMove : %d (x:%d, y:%d) scale %d, ch %d\r\n", nFlags, point.x, point.y, m_ZoomScale, m_iQuadCtrlChannel);

		m_bMouseClick = FALSE;
		m_cpMouse = point;
		
		if(nFlags == MK_LBUTTON)
		{
			m_cpZoomBase += (m_cpZoomMDown - point);
			m_cpZoomMDown = point;
			this->ZoomRedraw();
		}
		else if(nFlags == MK_RBUTTON || nFlags == (MK_SHIFT | MK_LBUTTON) )
		{
			if(m_cpZoomMDown.x - 20 > point.x || m_cpZoomMDown.y - 20 > point.y)
			{
				SetZoomScale(-0.5);
				m_cpZoomMDown = point;
			}
			else if(m_cpZoomMDown.x + 20 < point.x || m_cpZoomMDown.y + 20 < point.y)
			{
				SetZoomScale(0.5);
				m_cpZoomMDown = point;
			}
		}
	}
	CStatic::OnMouseMove(nFlags, point);
}


void ClVideoView::OnLButtonUp(UINT nFlags, CPoint point)
{
	 if(m_bMouseClick)
	{
		m_bMouseClick = FALSE;
		SetZoomScale(1.0);
	}
	
	CStatic::OnLButtonUp(nFlags, point);
}


void ClVideoView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rectDest(0,0,0,0);
	this->GetClientRect( rectDest );

	m_iQuadCtrlChannel = 0;

	if(point.x > rectDest.Width()/2)
		m_iQuadCtrlChannel |= 1;
	if(point.y > rectDest.Height()/2)
		m_iQuadCtrlChannel |= 2;

	printf("OnLButtonUp : %d (x:%d, y:%d) %d x %d, ch %d\r\n", nFlags, point.x, point.y, rectDest.Width(), rectDest.Height(), m_iQuadCtrlChannel);
	

	if(!m_bQuadCtrlMode) {
		m_bMouseClick = TRUE;
		m_cpZoomMDown = point;
	}
	
	CStatic::OnLButtonDown(nFlags, point);
}

void ClVideoView::OnRButtonUp(UINT nFlags, CPoint point)
{
	if(m_bMouseClick)
	{
		m_bMouseClick = FALSE;
		SetZoomScale(-1.0);
	}

	
	CStatic::OnRButtonUp(nFlags, point);
}


void ClVideoView::OnRButtonDown(UINT nFlags, CPoint point)
{
	if(!m_bQuadCtrlMode){
		m_bMouseClick = TRUE;
		m_cpZoomMDown = point;
	}
	CStatic::OnLButtonDown(nFlags, point);
}

void ClVideoView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if(!m_bQuadCtrlMode)
		SetZoomScale(1.0);
	
	CStatic::OnLButtonDblClk(nFlags, point);
}

void ClVideoView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	if(!m_bQuadCtrlMode)
		SetZoomScale(-1.0);
	
	CStatic::OnRButtonDblClk(nFlags, point);
}

BOOL ClVideoView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if(m_bQuadCtrlMode)
		return TRUE;
	
	printf("OnMouseWheel : %d %d (x:%d, y:%d)\r\n", nFlags,zDelta, pt.x, pt.y);

	if(zDelta > 0)
	{
		SetZoomScale(1.0, TRUE);
	}
	else
	{
		SetZoomScale(-1.0, TRUE);
	}

	return TRUE;
	//return CStatic::OnMouseWheel(nFlags, zDelta, pt);
}

VOID ClVideoView::SetZoomScale(double scale, BOOL bMousePosCenter)
{
	BITMAP bmpinfo;
	double scaleValue;
	CRect rScaleSize;
	CRect rNewScaleSize;

	if(m_zoomBitmap.GetObjectType() == NULL)
		return;
	
	if(scale < ZOOM_SCALE_VALUE_MIN && scale > -ZOOM_SCALE_VALUE_MIN)
		scaleValue = m_ZoomScale + scale;
	else
		scaleValue = scale;
	
	if(scaleValue > ZOOM_SCALE_VALUE_MAX) // max
		scaleValue = ZOOM_SCALE_VALUE_MAX;
	else if(scaleValue < ZOOM_SCALE_VALUE_MIN) //min
		scaleValue = ZOOM_SCALE_VALUE_MIN;

	m_zoomBitmap.GetBitmap(&bmpinfo);

	if(bmpinfo.bmWidth < 1 || scaleValue == m_ZoomScale)
		return;

	rScaleSize.right 		= (LONG)(bmpinfo.bmWidth/m_ZoomScale);
	rScaleSize.bottom 		= (LONG)(bmpinfo.bmHeight/m_ZoomScale);

	rNewScaleSize.right 	= (LONG)(bmpinfo.bmWidth/scaleValue);
	rNewScaleSize.bottom 	= (LONG)(bmpinfo.bmHeight/scaleValue);

	if(bMousePosCenter)
	{
		CRect rectDest(0,0,0,0);
		double moveScaleX, moveScaleY;

		this->GetClientRect( rectDest );
		if(m_cpMouse.x)
		{
			moveScaleX= rectDest.Width();
			moveScaleX /=  m_cpMouse.x ;	
			m_cpZoomBase.x += ((rScaleSize.right - rNewScaleSize.right) /moveScaleX);
		}

		if(m_cpMouse.y)
		{
			moveScaleY =rectDest.Height();
			moveScaleY /=  m_cpMouse.y;
			m_cpZoomBase.y += ((rScaleSize.bottom - rNewScaleSize.bottom)/moveScaleY);
		}
	}
	else
	{
		m_cpZoomBase.x += ((rScaleSize.right - rNewScaleSize.right) /2);
		m_cpZoomBase.y += ((rScaleSize.bottom - rNewScaleSize.bottom)/2);
	}
	
	m_ZoomScale = scaleValue;
	this->ZoomRedraw();
}

VOID ClVideoView::SetQuadView(HBITMAP hBitmap, int channel) // 0 ~ 3, 4is all view
{
	m_bQuadCtrlMode= TRUE;
	
	if(channel == 4){
		m_ZoomScale = 1.0;
	}
	else
		m_ZoomScale = 2.0;

	if(hBitmap)
	{	
		BITMAP bmpinfo;
		
		this->m_zoomBitmap.Detach();
		this->m_zoomBitmap.Attach(hBitmap);
		//this->m_zoomBitmap.CreateBitmapIndirect(hBitmap);

		m_zoomBitmap.GetBitmap(&bmpinfo);

		if(bmpinfo.bmWidth < 1)
			return;
		
		if(channel == 0 || channel >= 4){
			m_cpZoomBase.x = 0;
			m_cpZoomBase.y = 0;
		}
		else if(channel == 1){
			m_cpZoomBase.x = bmpinfo.bmWidth/m_ZoomScale;
			m_cpZoomBase.y = 0;
		}
		else if(channel == 2){
			m_cpZoomBase.x = 0;
			m_cpZoomBase.y = bmpinfo.bmHeight/m_ZoomScale;
		}
		else if(channel == 3){
			m_cpZoomBase.x = bmpinfo.bmWidth/m_ZoomScale;
			m_cpZoomBase.y = bmpinfo.bmHeight/m_ZoomScale;
		}
		
		

#if 1
		if(m_bQuadCtrlMode && channel == 4) {
			CClientDC dc(this);
			CDC memDC ;
			CBitmap memBitmap ;
			CBitmap* oldBitmap ; // bitmap originally found in CMemDC
			CRect rectDest(0,0,0,0);
			
			this->GetClientRect( rectDest );
			
			memDC.CreateCompatibleDC(&dc) ;
			memBitmap.CreateCompatibleBitmap(&dc, rectDest.Width(), rectDest.Height()) ;
			oldBitmap = (CBitmap *)memDC.SelectObject(&memBitmap) ;
			
			ZoomRedraw(&memDC);
			DrawGuidline(&memDC);

			dc.StretchBlt(0, 0, rectDest.Width(), rectDest.Height(), &memDC, 0, 0, rectDest.Width(), rectDest.Height(), SRCCOPY);
			
			if(oldBitmap)
			{
				memDC.SelectObject(oldBitmap);
				memDC.DeleteDC();
			}
		}
		else {
			this->ZoomRedraw();
		}
		//InvalidateRect(rectClient) ;	
#endif
	}
}

VOID ClVideoView::SetQuadView(HBITMAP hBitmap, double scale, int pos_x, int pos_y)
{
	if(scale >= 1.0)
		m_ZoomScale = scale;
	else
		m_ZoomScale = ZOOM_SCALE_VALUE_DEFAULT;

	if(hBitmap)
	{	
		this->m_zoomBitmap.Detach();
		this->m_zoomBitmap.Attach(hBitmap);
		//this->m_zoomBitmap.CreateBitmapIndirect(hBitmap);

		m_cpZoomBase.x = pos_x;
		m_cpZoomBase.y = pos_y;
		
		this->ZoomRedraw();
	}
}

VOID ClVideoView::SetZoomImage(HBITMAP hBitmap, double scale)
{
	if(hBitmap)
	{	
		this->m_zoomBitmap.Detach();
		this->m_zoomBitmap.Attach(hBitmap);
		//this->m_zoomBitmap.CreateBitmapIndirect(hBitmap);

		if(scale != 0.0)
			this->SetZoomScale(scale);
		else if(m_ZoomScale < ZOOM_SCALE_VALUE_DEFAULT)
			this->SetZoomScale(ZOOM_SCALE_VALUE_DEFAULT);
		else
			this->ZoomRedraw();
	}
}
VOID ClVideoView::ZoomRedraw(CDC* pDC, DWORD dwRop)
{

	if(m_zoomBitmap.GetObjectType()) {
		CClientDC aDC(this);
		CDC 					ImageDC;
		CBitmap 					*pOldBitmapImage = NULL;
		
		CRect rectDest(0,0,0,0);
		CRect rectSrc(0,0,0,0);
		BITMAP bmpinfo;

		if(pDC == NULL)
			pDC = &aDC;
		
		m_zoomBitmap.GetBitmap(&bmpinfo);

		if(bmpinfo.bmWidth < 1)
			return;
		
		if(m_ZoomScale > 1)
		{
			rectSrc.right 		= bmpinfo.bmWidth/m_ZoomScale;
			rectSrc.bottom 	= bmpinfo.bmHeight/m_ZoomScale;
		}
		else
		{
			rectSrc.right = bmpinfo.bmWidth;
			rectSrc.bottom = bmpinfo.bmHeight;
		}
		
		if(m_cpZoomBase.x < 0)
			m_cpZoomBase.x = 0;
		else if(m_cpZoomBase.x > (bmpinfo.bmWidth - rectSrc.right))
			m_cpZoomBase.x = bmpinfo.bmWidth - rectSrc.right;

		if(m_cpZoomBase.y < 0)
			m_cpZoomBase.y = 0;
		else if(m_cpZoomBase.y > (bmpinfo.bmHeight - rectSrc.bottom))
			m_cpZoomBase.y = bmpinfo.bmHeight - rectSrc.bottom;

		this->GetClientRect( rectDest );
		
		//m_zoomImage.Draw(aDC.m_hDC, rectDest, rectSrc);

		ImageDC.CreateCompatibleDC(this->GetDC());
		pOldBitmapImage = (CBitmap *)ImageDC.SelectObject(&this->m_zoomBitmap); 

		pDC->SetStretchBltMode(COLORONCOLOR);
		
		if(m_bQuadCtrlMode && m_ZoomScale <= 1.0)
			pDC->StretchBlt(0, 0, rectDest.Width(), rectDest.Height(), &ImageDC, m_cpZoomBase.x, m_cpZoomBase.y,rectSrc.right, rectSrc.bottom, dwRop);
		else
			pDC->StretchBlt(0, 0, rectDest.Width(), rectDest.Height(), &ImageDC, m_cpZoomBase.x + 9, m_cpZoomBase.y,rectSrc.right - 9, rectSrc.bottom - 9, dwRop);

		
		if(pOldBitmapImage)
		{
			ImageDC.SelectObject(pOldBitmapImage);
			ImageDC.DeleteDC();
		}
	}
}

//================================================================================================= E. OPERATION
