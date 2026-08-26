#include "stdafx.h"
#include "ICDateTimeCtrl.h"
#include <commctrl.h>

#define		_TEXT_COLOR		RGB(255, 255, 255)
#define		_TEXT_COLOR2		RGB(150, 150, 150)

#define 		_BG_COLOR			RGB(48, 48, 48)
#define 		_SELECTED_COLOR	RGB(0, 150, 250)

IMPLEMENT_DYNAMIC(CICDateTimeCtrl,CDateTimeCtrl)

CICDateTimeCtrl::CICDateTimeCtrl(void)
: CDateTimeCtrl(), m_clrBackGround(_BG_COLOR), m_clrText(_TEXT_COLOR)
{
	m_bkgd_brush = new CBrush(_BG_COLOR);
}

CICDateTimeCtrl::~CICDateTimeCtrl(void)
{
	delete m_bkgd_brush;
}


void CICDateTimeCtrl::Init()
{
	SetMonthCalColor(MCSC_BACKGROUND,_BG_COLOR);
	SetMonthCalColor(MCSC_TEXT ,_TEXT_COLOR);

	SetMonthCalColor(MCSC_TITLEBK ,_BG_COLOR);
	SetMonthCalColor(MCSC_TITLETEXT,_TEXT_COLOR);

	SetMonthCalColor(MCSC_MONTHBK,_BG_COLOR);
	SetMonthCalColor(MCSC_TRAILINGTEXT,_TEXT_COLOR2);
	
//	ModifyStyleEx(NULL,DTS_SHOWNONE);	
}

BOOL CICDateTimeCtrl::OnEraseBkgnd(CDC* pDC) 
{
#if 0
	CBrush * old_brush = pDC->SelectObject(m_bkgd_brush);
	CRect rect;
	pDC->GetClipBox(&rect);
	pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
	pDC->SelectObject(old_brush);
	
	pDC->SetTextColor(m_clrText);
	return TRUE;
#else
	return CDateTimeCtrl::OnEraseBkgnd(pDC);
#endif
}


void CICDateTimeCtrl::OnPaint()
{
#if 0
	CDateTimeCtrl::OnPaint();
	//CPaintDC dc(this);
	CDC* pDC = CDateTimeCtrl::GetDC();
	CRect rct;
	GetClientRect(&rct);
//	rct.right = rct.right - rct.Height();
	pDC->FillSolidRect (&rct,m_clrBackGround);

	CString strText;
	GetWindowText(strText);
	CFont * f = GetFont();
	CFont * pf = (CFont*)pDC->SelectObject(f);

	rct.DeflateRect(2,2);
	pDC->SetTextColor(m_clrText);
	CRect arText = rct;
	pDC->DrawText(strText,&arText,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_CALCRECT);
	
	//pDC->Rectangle(&arText);
	pDC->DrawText(strText,&rct,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
	//pDC->DrawText(strText,&arText,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
	
	pDC->SelectObject(pf);
#else
//	CDC* pDC = CDateTimeCtrl::GetDC();

//	pDC->SetBkColor(_BG_COLOR);
//	pDC->SetTextColor(_TEXT_COLOR);
	CDateTimeCtrl::OnPaint();
#endif
}

BEGIN_MESSAGE_MAP(CICDateTimeCtrl,CDateTimeCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()
