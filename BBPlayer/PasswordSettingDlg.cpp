// PasswordSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BBPlayer.h"
#include "PasswordSettingDlg.h"
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
// CPasswordSettingDlg dialog


CPasswordSettingDlg::CPasswordSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPasswordSettingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPasswordSettingDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_csOldPassword[0].Empty();
	m_csOldPassword[1].Empty();
	m_csOldPassword[2].Empty();
	m_csNewPassword.Empty();
	m_pCLanguage = NULL;
}


void CPasswordSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPasswordSettingDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_EDIT_OLD_PASSWORD, m_ceOldPassword);
	DDX_Control(pDX, IDC_EDIT_NEW_PASSWORD, m_ceNewPassword);
	DDX_Control(pDX, IDC_EDIT_CONFIRM, m_ceConfirm);
}


BEGIN_MESSAGE_MAP(CPasswordSettingDlg, CDialog)
	//{{AFX_MSG_MAP(CPasswordSettingDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPasswordSettingDlg message handlers

void CPasswordSettingDlg::SetOldPassword(TCHAR * OldPassword, TCHAR * OldPassword2, TCHAR * OldPassword3)
{
	m_csOldPassword[0].Format(L"%s", OldPassword);

	if(OldPassword2)
		m_csOldPassword[1].Format(L"%s", OldPassword2);

	if(OldPassword3)
		m_csOldPassword[2].Format(L"%s", OldPassword3);
}

void CPasswordSettingDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	static UINT count = 0;
	CString strOld, strNew, strConfirm;
	m_ceOldPassword.GetWindowText(strOld);
	
	if(strOld.Compare(m_csOldPassword[0])==0 || strOld.Compare(m_csOldPassword[1])==0 || strOld.Compare(m_csOldPassword[2])==0)
	{
		m_ceNewPassword.GetWindowText(strNew);
		m_ceConfirm.GetWindowText(strConfirm);

		if(strNew.Compare(strConfirm) == 0)
		{
			m_csNewPassword = strNew;
			OnOK();
			return;
		}
		else
		{
			AfxMessageBox(_T("New password check error"), MB_ICONERROR);
			m_ceConfirm.SetWindowText(L"");
			m_ceConfirm.SetFocus();
		}
	}
	else
	{
		AfxMessageBox(_T("Old password is wrong"), MB_ICONERROR);
		m_ceOldPassword.SetWindowText(L"");
		m_ceOldPassword.SetFocus();
	}

	if(count++ > 2)
		OnCancel();
}

void CPasswordSettingDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

BOOL CPasswordSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	CFont font;
	LOGFONT lf; 
	memset(&lf, 0, sizeof(LOGFONT));
    lf.lfHeight = 18;
    wcscpy_s(lf.lfFaceName, _T("Tahoma"));
    font.CreateFontIndirect(&lf); 

	m_ceOldPassword.SetLimitText(6);
	m_ceOldPassword.SetFont(&font, 1);

	m_ceNewPassword.SetLimitText(6);
	m_ceNewPassword.SetFont(&font, 1);

	m_ceConfirm.SetLimitText(6);
	m_ceConfirm.SetFont(&font, 1);

	OnLanguageChange();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

CString CPasswordSettingDlg::GetPassword(void)
{
	return m_csNewPassword;
}

void CPasswordSettingDlg::OnLanguageChange()
{
	CString str, str_setting, str_password;
	this->m_pCLanguage->GetLenguageText(&str_setting , CLanguage::TEXT_SETTING);
	this->m_pCLanguage->GetLenguageText(&str_password , CLanguage::TEXT_PASSWORD);
	
	str = str_password + L" " + str_setting;
	SetWindowText(str.GetBuffer(str.GetLength()));
	

	
	SetItemText(IDC_PASS_OLD, CLanguage::TEXT_PASSWORD);
	SetItemText(IDC_PASS_NEW, CLanguage::TEXT_NEWPASSWORD);
	SetItemText(IDC_PASS_CONFIRM, CLanguage::TEXT_VERIFYPASSWORD);

	SetItemText(IDOK, CLanguage::TEXT_SAVE);
	SetItemText(IDCANCEL, CLanguage::TEXT_CANCEL);
}

void  CPasswordSettingDlg::SetItemText(int nIDDlgItem, UINT resource, CString index_str)
{
	CString strText;
	if(m_pCLanguage)
	{
		if(this->m_pCLanguage->GetLenguageText(&strText , resource))
			this->GetDlgItem(nIDDlgItem)->SetWindowText(index_str + strText);
	}
}