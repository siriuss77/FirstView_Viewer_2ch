#include "stdafx.h"
#include "ICMonthCalCtrl.h"
#include <commctrl.h>

#define		_TEXT_COLOR		RGB(255, 255, 255)
#define		_TEXT_COLOR2		RGB(150, 150, 150)

#define 		_BG_COLOR			RGB(48, 48, 48)
#define 		_SELECTED_COLOR	RGB(0, 150, 250)

IMPLEMENT_DYNAMIC(CICMonthCalCtrl, CMonthCalCtrl)

CICMonthCalCtrl::CICMonthCalCtrl(void)
{
	SetClrBackGround(_BG_COLOR,false);
	SetClrText(_TEXT_COLOR,false);
	m_brBackGnd = new CBrush(_BG_COLOR);
}

BEGIN_MESSAGE_MAP(CICMonthCalCtrl,CMonthCalCtrl)
//	ON_WM_DRAWITEM()
//	ON_WM_ERASEBKGND()
//	ON_WM_PAINT()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CICMonthCalCtrl::Init()
{
	SetColor(MCSC_BACKGROUND,_BG_COLOR);
	SetColor(MCSC_TEXT ,_TEXT_COLOR);

	SetColor(MCSC_TITLEBK ,_BG_COLOR);
	SetColor(MCSC_TITLETEXT,_TEXT_COLOR);

	SetColor(MCSC_MONTHBK,_BG_COLOR);
	SetColor(MCSC_TRAILINGTEXT,_TEXT_COLOR2);
	
	ModifyStyleEx(NULL,DTS_SHOWNONE);
	
}

#if 0
void CICMonthCalCtrl::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
// 	if(nIDCtl==IDC_BTN_ACTION)         //checking for the button 
	{
		UINT state;
		CDC dc;
		RECT rctBtn;
		TCHAR buffer[MAX_PATH];           //To store the Caption of the button.
		ZeroMemory(buffer,MAX_PATH );     //Intializing the buffer to zero


		dc.Attach(lpDrawItemStruct->hDC);  // Get the Button DC to CDC
		rctBtn = lpDrawItemStruct->rcItem;     //Store the Button rect to our local rect.
		dc.Draw3dRect(&rctBtn,RGB(255,255,255),RGB(0,0,0)); 
		dc.FillSolidRect(&rctBtn,RGB(100,100,255));  //Here you can define the required color to appear on the Button.
		state = lpDrawItemStruct->itemState;   //This defines the state of the Push button either pressed or not. 
		if((state & ODS_SELECTED))
		{
			dc.DrawEdge(&rctBtn,EDGE_SUNKEN,BF_RECT);
		}
		else
		{
			dc.DrawEdge(&rctBtn,EDGE_RAISED,BF_RECT);
		}
		dc.SetBkColor(RGB(100,100,255));   //Setting the Text Background color
		dc.SetTextColor(RGB(255,0,0));     //Setting the Text Color


		::GetWindowText(lpDrawItemStruct->hwndItem,buffer,MAX_PATH); //Get the Caption of Button Window 
		dc.DrawText(buffer,&rctBtn,DT_CENTER|DT_VCENTER|DT_SINGLELINE);//Redraw the  Caption of Button Window 
		dc.Detach();  // Detach the Button DC
	}   

	CMonthCalCtrl::OnDrawItem(nIDCtl, lpDrawItemStruct);
}


#endif


#if 0
BOOL CICMonthCalCtrl::OnEraseBkgnd(CDC* pDC) 
{
	BOOL ret = CWnd::OnEraseBkgnd(pDC);
	CRect rct;
	GetClientRect(&rct);
	pDC->FillSolidRect (&rct,m_clrBackGround);

	CString strText;
	GetWindowText(strText);
	CFont * f = GetFont();
	CFont * pf = (CFont*)pDC->SelectObject(f);
		
	rct.DeflateRect(2,2);
	pDC->SetTextColor(m_clrText);
	CRect arText = rct;
	pDC->DrawText(strText,&arText,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_CALCRECT);
	pDC->SetROP2(R2_WHITE);
	pDC->Rectangle(&arText);
	//pDC->DrawText(strText,&rct,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
	//pDC->DrawText(strText,&arText,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
	pDC->SelectObject(pf);

	return false;	
//	return ret;
}
#endif

#if 0
void CICMonthCalCtrl::OnPaint()
{
	CMonthCalCtrl::OnPaint();
#if 0	
	//CPaintDC dc(this);
	CDC* pDC = CMonthCalCtrl::GetDC();
	CRect rct;
	GetClientRect(&rct);
	//rct.right = rct.right - rct.Height();
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
#endif	
}
#endif

 void CICMonthCalCtrl::OnDestroy()
{
	CMonthCalCtrl::OnDestroy();

	// Free the space allocated for the background brush
	delete m_brBackGnd;
}

