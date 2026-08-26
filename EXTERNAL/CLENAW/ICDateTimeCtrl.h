#pragma once

class AFX_EXT_CLASS CICDateTimeCtrl : public CDateTimeCtrl
{
	DECLARE_DYNAMIC(CICDateTimeCtrl)

public:
	CICDateTimeCtrl();
	virtual ~CICDateTimeCtrl();

	void SetClrBackGround(COLORREF clr, bool bRedraw = true)
	{
		if (clr != m_clrBackGround) // color changed?
		{
		    m_clrBackGround = clr;
			
		    delete m_bkgd_brush; // throw away old brush
		    m_bkgd_brush = new CBrush(m_clrBackGround); // buy a new one ;-)		

		    if(bRedraw )	Invalidate();
		}
	}
	void SetClrText(COLORREF clr, bool bRedraw = true)
	{
		m_clrText = clr;
		if(bRedraw )	Invalidate();
	}
	virtual void Init();
protected:
	CBrush* m_bkgd_brush;
	COLORREF m_clrBackGround;
	COLORREF m_clrText;

	afx_msg BOOL OnEraseBkgnd(CDC* pDC) ;
	afx_msg void OnPaint();
	
	DECLARE_MESSAGE_MAP()
};
