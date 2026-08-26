#pragma once

class AFX_EXT_CLASS CICMonthCalCtrl : public CMonthCalCtrl
{
	DECLARE_DYNAMIC(CICMonthCalCtrl)

public:
	CICMonthCalCtrl();
	virtual ~CICMonthCalCtrl() {}

	void SetClrBackGround(COLORREF clr, bool bRedraw = true)
	{
		m_clrBackGround = clr;
		if(bRedraw )	Invalidate();
	}
	void SetClrText(COLORREF clr, bool bRedraw = true)
	{
		m_clrText = clr;
		if(bRedraw )	Invalidate();
	}
	virtual void Init();
protected:
	COLORREF m_clrBackGround;
	COLORREF m_clrText;

	 // Add a CBrush* to store the new background brush for edit controls.
         CBrush* m_brBackGnd;


	// Generated message map functions
       //{{AFX_MSG(CEditDialog)

//	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) ;
//	afx_msg BOOL OnEraseBkgnd(CDC* pDC) ;
//	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
