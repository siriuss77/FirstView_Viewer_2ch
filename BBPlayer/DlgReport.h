#pragma once

#include "resource.h"
#include <BLACKBOX/Language.h>
#include "ReportPreview.h"

// CDlgReport dialog

class CDlgReport : public CDialog
{
	DECLARE_DYNAMIC(CDlgReport)

private:
	PRINTINFO			m_PrintInfo;
	REPORT_ITEM			m_ReportItem;

public:
	CDlgReport(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgReport();

	void SetReportItem(PREPORT_ITEM pItem);

// Dialog Data
	enum { IDD = IDD_REPORT };

private:
	CStatic m_staticPreview;
	CLanguage * 			m_pCLanguage;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedBtnInsertMemo();
	afx_msg void OnBnClickedBtnPrint();
	afx_msg void OnBnClickedCancel();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	void							SetLanguage				( CLanguage * cLang) 	{m_pCLanguage = cLang;}
};

