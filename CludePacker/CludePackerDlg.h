
// CludePackerDlg.h : header file
//

#pragma once


// CCludePackerDlg dialog
class CCludePackerDlg : public CDialog
{
// Construction
public:
	CCludePackerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CLUDEPACKER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedDetail();
	afx_msg void OnBnClickedDetailXml();
	afx_msg void OnBnClickedDetailImage();
	afx_msg void OnBnClickedDetailError();
	afx_msg void OnBnClickedProcess();
	afx_msg void OnNMDblclkDetailList(NMHDR *pNMHDR, LRESULT *pResult);

private:
	BOOL		m_bIsDetail;
};
