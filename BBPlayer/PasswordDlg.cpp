// PasswordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BBPlayer.h"
#include "PasswordDlg.h"
#include <CLUDE/version.h>
#include <CLENAW/ClImage.h>
#include <CLENAW/ClText.h>
#include <CLENAW/ClButton.h>
#include <CLENAW/ClSlider.h>
#include <HELPER/ClLog.H>
#include <HELPER/CLUtil.H>

//#include "SelDriveDlg.h"
#include "NotifyDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg dialog


CPasswordDlg::CPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPasswordDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPasswordDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_csPassword[0].Empty();
	m_csPassword[1].Empty();
	m_csPassword[2].Empty();
	m_pCLanguage = NULL;
}


void CPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPasswordDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_PASSWORD, m_cePassword);
}


BEGIN_MESSAGE_MAP(CPasswordDlg, CDialog)
	//{{AFX_MSG_MAP(CPasswordDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg message handlers

void CPasswordDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	static int count = 0;

	CString csPw;
	m_cePassword.GetWindowText(csPw);
	
	if(m_csPassword[0].Compare(csPw) == 0 || m_csPassword[1].Compare(csPw) == 0 || m_csPassword[2].Compare(csPw) == 0)
	{
		OnOK();
		return;
	}
	else
		AfxMessageBox(_T("password is wrong"), MB_ICONERROR);

	m_cePassword.SetWindowText(L"");
	m_cePassword.SetFocus();

	if(count++ > 2)
		OnCancel();
}

void CPasswordDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

BOOL CPasswordDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	
	CFont font;
	LOGFONT lf; 
	memset(&lf, 0, sizeof(LOGFONT));
    lf.lfHeight = 18;
    wcscpy_s(lf.lfFaceName, L"Tahoma");
    font.CreateFontIndirect(&lf); 
	
	m_cePassword.SetLimitText(6);
	m_cePassword.SetFont(&font, 1);

	OnLanguageChange();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPasswordDlg::SetPassword(TCHAR *password, TCHAR *password2, TCHAR *password3)
{
	m_csPassword[0].Format(L"%s", password);
	if(password2)
		m_csPassword[1].Format(L"%s", password2);

	if(password3)
		m_csPassword[2].Format(L"%s", password3);
}

void CPasswordDlg::OnLanguageChange()
{
//	SetItemText(IDD_PASSWORD, CLanguage::TEXT_PASSWORD);
	CString str_password;
	this->m_pCLanguage->GetLenguageText(&str_password , CLanguage::TEXT_PASSWORD);
	SetWindowText(str_password.GetBuffer(str_password.GetLength()));
	
	
	SetItemText(IDC_PASS_PASSWORD, CLanguage::TEXT_PASSWORD);

	SetItemText(IDOK, CLanguage::TEXT_OK);
	SetItemText(IDCANCEL, CLanguage::TEXT_CANCEL);
}

void  CPasswordDlg::SetItemText(int nIDDlgItem, UINT resource, CString index_str)
{
	CString strText;
	if(m_pCLanguage)
	{
		if(this->m_pCLanguage->GetLenguageText(&strText , resource))
			this->GetDlgItem(nIDDlgItem)->SetWindowText(index_str + strText);
	}
}