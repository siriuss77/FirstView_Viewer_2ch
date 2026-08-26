#if !defined(AFX_ICDBGRAPH_H__085FBEFB_AD9F_4868_AE28_BE564948C6F2__INCLUDED_)
#define AFX_ICDBGRAPH_H__085FBEFB_AD9F_4868_AE28_BE564948C6F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ICDBGraph.h : header file
//

#include	"ICWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CICDBGraph window

class AFX_EXT_CLASS CICDBGraph : public CICWnd
{
// Construction
public:
	CICDBGraph();

	enum
	{
		MAX_COLOR_TABLE	= 5,

		MAX_ITEM_TABLE	= 4,
		MAX_ITEM_DATA	= 60
	};

	typedef struct tagItem
	{
		BYTE	set		: 1;
		BYTE	color	: 7;
	} ITEM, FAR * LPITEM;

	BOOL 	SetGraph(BYTE id, long count, long width = NULL, long height = NULL);
	BOOL	SetGraphItem(BYTE pos, BYTE count, ITEM * pItemArray);
	BOOL	SetColor(BYTE table_pos, COLORREF color);
	BOOL	SetSeletedSel(long pos);
	void	SetAddDelTextOutValue(BOOL isAdd, BYTE btValue);
	BOOL	EnableCtrl(BOOL bEnable = TRUE);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CICDBGraph)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CICDBGraph();

	// Generated message map functions
protected:
	//{{AFX_MSG(CICDBGraph)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	BYTE  	__m_btGraphID;
	long		__m_nGraphCount; 
	long		__m_nGraphWidth;
	long		__m_nGraphHeight;
	
	long		__m_nSelectPos;
	long		__m_nItemCount;
	COLORREF	__m_ColorTable[MAX_COLOR_TABLE];
	ITEM		__m_ItemTable[MAX_ITEM_TABLE][MAX_ITEM_DATA];
	COLORREF	__m_ColorBack;

	BYTE		__m_isAdd;
	BYTE		__m_btValue;

	BOOL		__m_isEnableControl;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICDBGRAPH_H__085FBEFB_AD9F_4868_AE28_BE564948C6F2__INCLUDED_)
