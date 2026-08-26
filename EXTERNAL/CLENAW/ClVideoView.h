//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClVideoView.H
//
//	Description: CLENA-W Video View control
//
//	Update History
// 
//		2011.11.23 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. FILEID

#pragma once

//================================================================================================= E. FILEID
//================================================================================================= S. INCLUDE

#include <atlimage.h>
#include <CLENAW/ClIBGTransfer.H>

//================================================================================================= E. INCLUDE
//================================================================================================= S. CLASS

class ClVideoView : public CStatic, IBGTransfer
{
	DECLARE_DYNAMIC(ClVideoView)

public:
	ClVideoView();
	virtual ~ClVideoView();

public:

	virtual	VOID					TransferBackground	( CDC* pDC );


protected:

	DECLARE_MESSAGE_MAP()


public:

	virtual void					DrawItem			( LPDRAWITEMSTRUCT lpDrawItemStruct );
	afx_msg BOOL				OnEraseBkgnd		( CDC* pDC );
	afx_msg	void					OnMouseHover		( UINT nFlags, CPoint point );
	afx_msg	void					OnMouseLeave		();
	afx_msg	void					OnMouseMove			( UINT nFlags, CPoint point );
	afx_msg void					OnLButtonUp			( UINT nFlags, CPoint point );
	afx_msg void 					OnLButtonDown		(UINT nFlags, CPoint point);
	afx_msg void					OnLButtonDblClk		(UINT nFlags, CPoint point);
	afx_msg void					OnRButtonUp			( UINT nFlags, CPoint point );
	afx_msg void 					OnRButtonDown		(UINT nFlags, CPoint point);
	afx_msg void					OnRButtonDblClk		(UINT nFlags, CPoint point);
	afx_msg BOOL 				OnMouseWheel		(UINT nFlags, short zDelta, CPoint pt);

	void 						DrawGuidline(CDC* pDC);
	VOID 						SetQuadView(HBITMAP hBitmap, int channel = 0); // 0 ~ 3, 4is all view
	VOID 						SetQuadView(HBITMAP hBitmap, double scale, int pos_x = 0, int pos_y = 0);
	VOID						SetQuadViewMode(BOOL enable) { m_bQuadCtrlMode = enable; m_ZoomScale = 1.0; };
	BOOL						GetQuadViewMode(VOID) { return m_bQuadCtrlMode; };
	VOID						SetQuadCtrlChannel(INT ch) { m_iQuadCtrlChannel = ch; };
	INT							GetQuadCtrlChannel(VOID) { return m_iQuadCtrlChannel; };
	VOID 						SetZoomScale(double scale, BOOL bMousePosCenter = NULL);
	VOID						SetZoomImage(HBITMAP hBitmap, double scale = 0.0);
	VOID 						ZoomRedraw(CDC* pDC = NULL, DWORD dwRop = SRCCOPY);
private:
	BOOL					m_bIsHover;

	// tony, added by zoom control
	double					m_ZoomScale;
	BOOL					m_bMouseClick;
	CPoint					m_cpMouse;
	CPoint					m_cpZoomMDown;
	CPoint					m_cpZoomBase;
	CBitmap					m_zoomBitmap;
	CImage 					m_zoomImage; 

	//
	BOOL					m_bQuadCtrlMode;
	INT						m_iQuadCtrlChannel;
};

//================================================================================================= E. CLASS
