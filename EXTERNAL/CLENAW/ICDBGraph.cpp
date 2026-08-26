// ICDBGraph.cpp : implementation file
//

#include "stdafx.h"
#include "ICDBGraph.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CICDBGraph
#define		_TEXT_COLOR		RGB(255, 255, 255)
#define 		_BG_COLOR			RGB(35, 35, 35)
#define 		_SELECTED_COLOR	RGB(0, 150, 250)

CICDBGraph::CICDBGraph()
{
	int i = 0;

	__m_btGraphID = 0;
	__m_nGraphCount = 1;
	__m_nGraphWidth = 600;
	__m_nGraphHeight = 20;
	
	__m_nItemCount	= 0;
	__m_nSelectPos	= 0;
	for( i=0; i<MAX_COLOR_TABLE; i++ )
		__m_ColorTable[i] = RGB(128, 128, 128);

	memset(__m_ItemTable, 0x00, MAX_ITEM_TABLE*MAX_ITEM_DATA*sizeof(ITEM));

	__m_ColorBack = _BG_COLOR;

	__m_isAdd	= TRUE;
	__m_btValue	= 0xFF;

	__m_isEnableControl = TRUE;
}

CICDBGraph::~CICDBGraph()
{
}


BEGIN_MESSAGE_MAP(CICDBGraph, CStatic)
	//{{AFX_MSG_MAP(CICDBGraph)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CICDBGraph message handlers

void CICDBGraph::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here

	CRect		rcctrl;
	CRect		rect;
	COLORREF	color;

	GetClientRect(&rcctrl);

	// brush
	dc.FillSolidRect(&rcctrl, __m_ColorBack);
	__m_nGraphWidth = rcctrl.Width();
	__m_nGraphHeight = rcctrl.Height();

	if( __m_nGraphWidth > 0 && __m_nItemCount > 0 )
	{
		int			itemwidth = __m_nGraphWidth / __m_nItemCount;
		// font
		CFont		font;
		CFont *		fontold;
		int			fontwidth = 0;
		int			fontheight = 0;
		int			bkmode = dc.SetBkMode(TRANSPARENT);

		fontwidth = __m_nGraphWidth / __m_nItemCount;
		fontheight = __m_nGraphHeight /2 - 4;

		if( fontwidth > 20 )
			fontwidth = (fontwidth - 4) /2;
		else
			fontwidth =  (fontwidth - 2) /2;

		if(fontheight > (fontwidth * 4))
			fontheight = fontwidth * 4;
		
		font.CreateFont(fontheight, fontwidth,
						0, 0,
						FW_LIGHT,
						FALSE,			// bItalic
						FALSE,			// bUnderline
						FALSE,			// cStrikOut
						DEFAULT_CHARSET,
						OUT_STROKE_PRECIS,	//OUT_DEFAULT_PRECIS,
						CLIP_STROKE_PRECIS,	//CLIP_DEFAULT_PRECIS,
						DEFAULT_QUALITY,	//DRAFT_QUALITY,
						VARIABLE_PITCH | FF_ROMAN,
						L"Arial");

		fontold = (CFont*)dc.SelectObject(&font);

		CString str;
		int i = 0, j = 0;
		
		for( i=0; i<__m_nItemCount; i++ )
		{
			if( __m_isAdd )
			{
				if( i >= __m_btValue )
					str.Format(L"%2d", i+1);
				else
					str.Format(L"%2d", i);
			}
			else
			{
				if( i >= __m_btValue )
					str.Format(L"%2d", i-1);
				else
					str.Format(L"%2d", i);
			}
			dc.SetTextColor(_TEXT_COLOR);
			dc.TextOut(i*itemwidth+1, 0, str);
		}

		dc.SelectObject(fontold);
		dc.SetBkMode(bkmode);

		// brush
		
		int graph_height = ((__m_nGraphHeight -  (fontheight + 1)) / __m_nGraphCount);
		for( i=0; i<__m_nGraphCount; i++ )
		{
			int small_line_top= 0, small_line_bottom = 0;
			
			if(i)
				small_line_top = graph_height/4 * 3;
			else if(__m_nGraphCount > 1)
				small_line_bottom = graph_height/4 * 3;
			
			for( j=0; j<__m_nItemCount; j++ )
			{
				color = __m_ColorBack;

				if( __m_ItemTable[i][j].set )
					color = __m_ColorTable[__m_ItemTable[i][j].color];

				rect.left	= j*itemwidth;
				rect.right	= (j+1)*itemwidth;
				
				rect.top	= fontheight + 1 + graph_height*i;
				rect.bottom = small_line_bottom + rect.top + graph_height - 2;//rcctrl.Height()
				rect.top	+= small_line_top;

				dc.FillSolidRect(&rect, color);
			}
		}

		// rectangle
		CBrush		brush;
		CBrush *	brushold;

		brush.CreateSolidBrush(_SELECTED_COLOR);
		brushold = (CBrush*)dc.SelectObject(&brush);

		rect.left	= __m_nSelectPos*itemwidth;
		rect.top	= 0;
		rect.right	= (__m_nSelectPos+1)*itemwidth;
		rect.bottom	= rcctrl.Height();

		dc.FrameRect(&rect, &brush);
		dc.SelectObject(brushold);
	}

	// Do not call CStatic::OnPaint() for painting messages
}

void CICDBGraph::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	ModifyStyle(SS_ETCHEDFRAME, SS_NOTIFY, TRUE);
	SetWindowText(L"");
	
	CStatic::PreSubclassWindow();
}

BOOL CICDBGraph::SetGraph(BYTE id, long count, long width, long height)
{
	if(count > MAX_ITEM_TABLE)
		count = MAX_ITEM_TABLE;

	__m_btGraphID = id;
	__m_nGraphCount = count;

	if(width)
		__m_nGraphWidth = width;

	if(height)
		__m_nGraphHeight = height;	

	return TRUE;
}

BOOL CICDBGraph::SetColor(BYTE table_pos, COLORREF color)
{
	if( table_pos >= MAX_COLOR_TABLE )
		return FALSE;

	__m_ColorTable[table_pos] = color;

	return TRUE;
}

BOOL CICDBGraph::SetGraphItem(BYTE pos, BYTE count, ITEM * pItemArray)
{
	if( count != 23 && count != 24 && count != 25 && count != 60 )
		return FALSE;

	__m_nItemCount = count;
	memcpy((void*)__m_ItemTable[pos], (const void*)pItemArray, count*sizeof(ITEM));

	return TRUE;
}

BOOL CICDBGraph::SetSeletedSel(long pos)
{
	if( pos < 0 || pos >= __m_nItemCount )
		return FALSE;

	long	nPrevSelectPos = __m_nSelectPos;
	CRect	rcctrl, rcinvali;

	GetClientRect(&rcctrl);

	int		itemwidth = __m_nGraphWidth / __m_nItemCount;

	__m_nSelectPos = pos;

	if( nPrevSelectPos == __m_nSelectPos )
		return TRUE;

	// remove prev selected
	rcinvali.left	= nPrevSelectPos*itemwidth;
	rcinvali.top	= 0;
	rcinvali.right	= (nPrevSelectPos+1)*itemwidth;
	rcinvali.bottom	= rcctrl.Height();
	InvalidateRect(&rcinvali, FALSE);

	// draw current select
	rcinvali.left	= __m_nSelectPos*itemwidth;
	rcinvali.top	= 0;
	rcinvali.right	= (__m_nSelectPos+1)*itemwidth;
	rcinvali.bottom	= rcctrl.Height();
	InvalidateRect(&rcinvali, FALSE);

	return TRUE;
}

BOOL CICDBGraph::EnableCtrl(BOOL bEnable/*=TRUE*/)
{
	__m_isEnableControl = bEnable;

	return TRUE;
}

void CICDBGraph::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	//CStatic::OnLButtonDown(nFlags, point);
}

void CICDBGraph::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( !__m_isEnableControl )	return;

	if( __m_nItemCount > 0 )
	{
		int	itemwidth = __m_nGraphWidth / __m_nItemCount;

		GetParent()->PostMessage(ICM_DBGRAPH, MAKELONG(ICM_DBGRAPH_W_LCLICK, __m_btGraphID), MAKELONG(__m_nSelectPos, point.x/itemwidth));
		SetSeletedSel(point.x/itemwidth);
	}

	//CStatic::OnLButtonUp(nFlags, point);
}

void CICDBGraph::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if( !__m_isEnableControl )	return;

	GetParent()->PostMessage(ICM_DBGRAPH, MAKELONG(ICM_DBGRAPH_W_LDBLCLICK, __m_btGraphID), 0);
	//CStatic::OnLButtonDblClk(nFlags, point);
}

void CICDBGraph::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	//CStatic::OnRButtonDown(nFlags, point);
}

void CICDBGraph::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	GetParent()->PostMessage(ICM_DBGRAPH, MAKELONG(ICM_DBGRAPH_W_RCLICK, __m_btGraphID), 0);

	//CStatic::OnRButtonUp(nFlags, point);
}

void CICDBGraph::SetAddDelTextOutValue(BOOL isAdd, BYTE btValue)
{
	if( !__m_isEnableControl )	return;

	__m_isAdd		= (BYTE)isAdd;
	__m_btValue		= btValue;
}
