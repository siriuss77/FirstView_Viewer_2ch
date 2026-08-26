// DlgReport.cpp : implementation file
//

#include "stdafx.h"
#include "DlgReport.h"
#include <CLUDE/version.h>
#include <CLENAW/DigitNum.h>

// CDlgReport dialog

#pragma warning (disable:4996)

IMPLEMENT_DYNAMIC(CDlgReport, CDialog)

CDlgReport::CDlgReport(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgReport::IDD, pParent)
{

	m_pCLanguage = NULL;
	memset(&m_PrintInfo, 0, sizeof(PRINTINFO));

}

CDlgReport::~CDlgReport()
{
}

void CDlgReport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_PREVIEW, m_staticPreview);
}


BEGIN_MESSAGE_MAP(CDlgReport, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_INSERT_MEMO, &CDlgReport::OnBnClickedBtnInsertMemo)
	ON_BN_CLICKED(IDC_BTN_PRINT, &CDlgReport::OnBnClickedBtnPrint)
	ON_BN_CLICKED(IDC_CANCEL, &CDlgReport::OnBnClickedCancel)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CDlgReport message handlers

BOOL CDlgReport::OnInitDialog()
{
	CDialog::OnInitDialog();

	 if(m_pCLanguage){
	 	CString strText;
	 	if(m_pCLanguage->GetLenguageText(&strText, CLanguage::TEXT_EVENT_REPORT_WRITING))
	 		SetWindowText(strText); 
		
		if(m_pCLanguage->GetLenguageText(&strText, CLanguage::TEXT_USER_INPUT))
			SetDlgItemText(IDC_STATIC_REPORT, strText); 

		if(m_pCLanguage->GetLenguageText(&strText, CLanguage::TEXT_DESCRIPTION_ADDUP))
			SetDlgItemText(IDC_BTN_INSERT_MEMO, strText); 

		if(m_pCLanguage->GetLenguageText(&strText, CLanguage::TEXT_PRINT))
			SetDlgItemText(IDC_BTN_PRINT, strText); 

		if(m_pCLanguage->GetLenguageText(&strText, CLanguage::TEXT_CANCEL))
			SetDlgItemText(IDC_CANCEL, strText);
	 }

/*
	m_btnExit.Create(NULL, NULL, WS_CHILD|WS_VISIBLE, CRect(0, 0, 0, 0), this, IDB_BMP_EXIT_U, NULL);
	//m_btnExit.SetBitmaps(IDB_EXIT_U, IDB_EXIT_D, IDB_EXIT_X, IDB_EXIT_O);
	m_btnExit.SetBitmaps(IDB_BMP_POPUP_EXIT_U, IDB_BMP_POPUP_EXIT_D, IDB_BMP_POPUP_EXIT_X, IDB_BMP_POPUP_EXIT_O);
	CRect clientRect;
	GetClientRect(&clientRect);
	m_btnExit.MoveWindow(clientRect.Width()-CX_SYSTEMMENU_BTN, 1, CX_SYSTEMMENU_BTN, CY_SYSTEMMENU_BTN);
*/

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgReport::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	RECT rcClient;
	m_staticPreview.GetWindowRect(&rcClient);
	ScreenToClient(&rcClient);

	CString str;

if(m_pCLanguage){
	 	CString strText;
		
#if (BUILD_MODEL == MODEL_DAVIEW_2CH_L)                       // jun 170405

           if(m_pCLanguage->GetLenguageText(&strText,CLanguage::TEXT_REPORT))
		
#else
           if(m_pCLanguage->GetLenguageText(&strText,CLanguage::TEXT_EVENT_REPORT))

#endif
			wsprintf(m_PrintInfo.tzTitle, _T("%s"), strText);


		/* patch by hwangbu - 20120116 */
		//wsprintf(m_PrintInfo.tzBottom, _T("%s"), us.GetDateTimeString(time(0)-m_Timezone.GetTimeZone()));
		CTime t = CTime::GetCurrentTime();
		
		str.Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), t.GetYear(), t.GetMonth(), t.GetDay(), t.GetHour(), t.GetMinute(), t.GetSecond());
		_tcscpy(m_PrintInfo.tzBottom, str);


		if(m_pCLanguage->GetLenguageText(&strText, CLanguage::TEXT_EVENT_DATE))
//			wsprintf(m_PrintInfo.tzLine1, _T("%s (yyyy/mm/dd hh:MM:ss)"), strText);
			wsprintf(m_PrintInfo.tzLine1, _T("%s "), strText);
		_tcscpy(m_PrintInfo.tzLine1C, m_ReportItem.strDateTime);

		if(m_pCLanguage->GetLenguageText(&strText, CLanguage::TEXT_EVENT_LOC))
			wsprintf(m_PrintInfo.tzLine2, _T("%s"), strText);

		str.Format(_T("%.06f,  "), m_ReportItem.fNGps);
		_tcscpy(m_PrintInfo.tzLine2C1, str);

		str.Format(_T("%.06f"), m_ReportItem.fEGps);
		_tcscpy(m_PrintInfo.tzLine2C2, str);

#ifdef BUILD_REPORT_SPEED

		switch(m_ReportItem.nSpeedMode)
		{
		case CDigitNum::UNIT_MI: 	str.Format(TEXT("%0d Mile"), (int)(m_ReportItem.pDigit * 0.621371));	break;
		case CDigitNum::UNIT_KN: 	str.Format(TEXT("%0d Knot"), (int)(m_ReportItem.pDigit * 0.539957));	break;		
		
		case CDigitNum::UNIT_KM: 	
		default: 		str.Format(_T("%.00f KM"), m_ReportItem.pDigit);	break;
		}
		
		_tcscpy(m_PrintInfo.tzLine2C3, str);

#endif

		if(m_pCLanguage->GetLenguageText(&strText, CLanguage::TEXT_G_SENSOR))
			wsprintf(m_PrintInfo.tzLine3, _T("%s"), strText);
		str.Format(_T("X: %.4f,  Y: %.4f,  Z: %.4f"), m_ReportItem.fXSensor, m_ReportItem.fYSensor, m_ReportItem.fZSensor);
		_tcscpy(m_PrintInfo.tzLine3C, str);

		if(m_pCLanguage->GetLenguageText(&strText, CLanguage::TEXT_DESCRIPTION))
			wsprintf(m_PrintInfo.tzLine4, _T("%s"), strText);
	
	}


	CReportPreview  preview;
	preview.GetPrintOutDC(&dc, &rcClient, &m_ReportItem, &m_PrintInfo);


	CDC dcMemory;
	CDC dcMemory2;
	CBitmap bmp;
	CRect rect;
	this->GetClientRect(&rect);
	bmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());

	dcMemory.CreateCompatibleDC(&dc);
	dcMemory2.CreateCompatibleDC(&dc);

	//2. 상단 중앙
	dcMemory.StretchBlt(0, 0, rect.Width(), 34, &dcMemory2, 0, 0, 1, 34, SRCCOPY);

	dcMemory.SetBkMode(TRANSPARENT);
	dcMemory.SetTextColor(RGB(0xC2,0xC2,0xC2));

	if(m_pCLanguage){
	 	CString strText;

#if (BUILD_MODEL == MODEL_DAVIEW_2CH_L)                       // jun 170405

	 	if(m_pCLanguage->GetLenguageText(&strText, CLanguage::TEXT_REPORT))
#else

	 	if(m_pCLanguage->GetLenguageText(&strText, CLanguage::TEXT_EVENT_REPORT))
			
#endif
			dcMemory.TextOutW( 9, 9, strText, 6 );
	}

	dc.BitBlt(0, 0, rect.Width(), 34, &dcMemory, 0, 0, SRCCOPY);

	// 에지
	dc.MoveTo(0,0);
	dc.LineTo(0,rect.Height());
	dc.MoveTo(0,rect.Height()-1);
	dc.LineTo(rect.Width(),rect.Height()-1);
	dc.MoveTo(rect.Width()-1,rect.Height()-1);
	dc.LineTo(rect.Width()-1,0);
	dc.MoveTo(rect.Width()-1,0);
	dc.LineTo(0,0);

	bmp.DeleteObject();

	dcMemory.DeleteDC();
	dcMemory2.DeleteDC();
}

void CDlgReport::SetReportItem(PREPORT_ITEM pItem)
{
	memcpy (&m_ReportItem, pItem, sizeof(REPORT_ITEM));
}

void CDlgReport::OnBnClickedBtnInsertMemo()
{
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_MEMO);
	pEdit->UpdateData(TRUE);

	if (pEdit)
	{
		wsprintf(m_PrintInfo.tzMemo1, _T(""));
		wsprintf(m_PrintInfo.tzMemo2, _T(""));
		wsprintf(m_PrintInfo.tzMemo3, _T(""));
		wsprintf(m_PrintInfo.tzMemo4, _T(""));
		wsprintf(m_PrintInfo.tzMemo5, _T(""));
		wsprintf(m_PrintInfo.tzMemo6, _T(""));

		int nLineCnt = pEdit->GetLineCount();
		int nLineLen, nFindPos=0;
		TCHAR* pNull = NULL;


		if (nLineCnt >= 1)
		{									
			nLineLen = pEdit->LineLength(nFindPos);
			pEdit->GetLine(0, m_PrintInfo.tzMemo1, nLineLen);			
			nFindPos += (nLineLen + 2);

			pNull = (TCHAR*)(m_PrintInfo.tzMemo1 + nLineLen);
			wsprintf(pNull, _T(""));
		}

		if (nLineCnt >= 2)
		{	
			nLineLen = pEdit->LineLength(nFindPos);
			pEdit->GetLine(1, m_PrintInfo.tzMemo2, nLineLen);
			nFindPos += (nLineLen + 2);

			pNull = (TCHAR*)(m_PrintInfo.tzMemo2 + nLineLen);
			wsprintf(pNull, _T(""));
		}

		if (nLineCnt >= 3)
		{	
			nLineLen = pEdit->LineLength(nFindPos);
			pEdit->GetLine(2, m_PrintInfo.tzMemo3, nLineLen);
			nFindPos += (nLineLen + 2);

			pNull = (TCHAR*)(m_PrintInfo.tzMemo3 + nLineLen);
			wsprintf(pNull, _T(""));
		}

		if (nLineCnt >= 4)
		{		
			nLineLen = pEdit->LineLength(nFindPos);
			pEdit->GetLine(3, m_PrintInfo.tzMemo4, nLineLen);
			nFindPos += (nLineLen + 2);

			pNull = (TCHAR*)(m_PrintInfo.tzMemo4 + nLineLen);
			wsprintf(pNull, _T(""));
		}

		if (nLineCnt >= 5)
		{			
			nLineLen = pEdit->LineLength(nFindPos);
			pEdit->GetLine(4, m_PrintInfo.tzMemo5, nLineLen);
			nFindPos += (nLineLen + 2);

			pNull = (TCHAR*)(m_PrintInfo.tzMemo5 + nLineLen);
			wsprintf(pNull, _T(""));
		}

		if (nLineCnt >= 6)
		{			
			nLineLen = pEdit->LineLength(nFindPos);
			pEdit->GetLine(5, m_PrintInfo.tzMemo6, nLineLen);
			nFindPos += (nLineLen + 2);

			pNull = (TCHAR*)(m_PrintInfo.tzMemo6 + nLineLen);
			wsprintf(pNull, _T(""));
		}

		Invalidate(FALSE);
	}
}

void CDlgReport::OnBnClickedBtnPrint()
{
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_MEMO);
	pEdit->UpdateData(TRUE);

	if (pEdit)
	{
		wsprintf(m_PrintInfo.tzMemo1, _T(""));
		wsprintf(m_PrintInfo.tzMemo2, _T(""));
		wsprintf(m_PrintInfo.tzMemo3, _T(""));
		wsprintf(m_PrintInfo.tzMemo4, _T(""));
		wsprintf(m_PrintInfo.tzMemo5, _T(""));
		wsprintf(m_PrintInfo.tzMemo6, _T(""));

		int nLineCnt = pEdit->GetLineCount();
		int nLineLen, nFindPos=0;
		TCHAR* pNull = NULL;


		if (nLineCnt >= 1)
		{									
			nLineLen = pEdit->LineLength(nFindPos);
			pEdit->GetLine(0, m_PrintInfo.tzMemo1, nLineLen);			
			nFindPos += (nLineLen + 2);

			pNull = (TCHAR*)(m_PrintInfo.tzMemo1 + nLineLen);
			wsprintf(pNull, _T(""));
		}

		if (nLineCnt >= 2)
		{	
			nLineLen = pEdit->LineLength(nFindPos);
			pEdit->GetLine(1, m_PrintInfo.tzMemo2, nLineLen);
			nFindPos += (nLineLen + 2);

			pNull = (TCHAR*)(m_PrintInfo.tzMemo2 + nLineLen);
			wsprintf(pNull, _T(""));
		}

		if (nLineCnt >= 3)
		{	
			nLineLen = pEdit->LineLength(nFindPos);
			pEdit->GetLine(2, m_PrintInfo.tzMemo3, nLineLen);
			nFindPos += (nLineLen + 2);

			pNull = (TCHAR*)(m_PrintInfo.tzMemo3 + nLineLen);
			wsprintf(pNull, _T(""));
		}

		if (nLineCnt >= 4)
		{		
			nLineLen = pEdit->LineLength(nFindPos);
			pEdit->GetLine(3, m_PrintInfo.tzMemo4, nLineLen);
			nFindPos += (nLineLen + 2);

			pNull = (TCHAR*)(m_PrintInfo.tzMemo4 + nLineLen);
			wsprintf(pNull, _T(""));
		}

		if (nLineCnt >= 5)
		{			
			nLineLen = pEdit->LineLength(nFindPos);
			pEdit->GetLine(4, m_PrintInfo.tzMemo5, nLineLen);
			nFindPos += (nLineLen + 2);

			pNull = (TCHAR*)(m_PrintInfo.tzMemo5 + nLineLen);
			wsprintf(pNull, _T(""));
		}

		if (nLineCnt >= 6)
		{			
			nLineLen = pEdit->LineLength(nFindPos);
			pEdit->GetLine(5, m_PrintInfo.tzMemo6, nLineLen);
			nFindPos += (nLineLen + 2);

			pNull = (TCHAR*)(m_PrintInfo.tzMemo6 + nLineLen);
			wsprintf(pNull, _T(""));
		}

		Invalidate(FALSE);
	}

	CReportPreview  preview;
	preview.PrintOut(&m_ReportItem, &m_PrintInfo);
}

void CDlgReport::OnBnClickedCancel()
{
	CDialog::OnCancel();
}

HBRUSH CDlgReport::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if(nCtlColor != CTLCOLOR_EDIT && nCtlColor != CTLCOLOR_LISTBOX){
		static CBrush s_brush(RGB(0xff, 0xff, 0xff));
		pDC->SetBkMode(TRANSPARENT);  
		pDC->SetTextColor(RGB(0x40, 0x40, 0x40));  //글자색(일괄 변경)
		pDC->SetBkColor(RGB(0xff, 0xff, 0xff));  //배경색
		return s_brush;
	}
	// TODO:  Return a different brush if the default is not desired

	return hbr;
}

BOOL CDlgReport::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	static CBrush s_brush(RGB(0xff, 0xff, 0xff));
	pDC->SetBkMode(TRANSPARENT);  
	pDC->SetTextColor(RGB(0x40, 0x40, 0x40));  //글자색(일괄 변경)
	pDC->SetBkColor(RGB(0xff, 0xff, 0xff));  //배경색

	return CDialog::OnEraseBkgnd(pDC);
}

void CDlgReport::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));

	CDialog::OnLButtonDown(nFlags, point);
}

