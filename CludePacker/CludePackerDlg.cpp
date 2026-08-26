
// CludePackerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CludePacker.h"
#include "CludePackerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CCludePackerDlg dialog




CCludePackerDlg::CCludePackerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCludePackerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	this->m_bIsDetail = FALSE;
}

void CCludePackerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCludePackerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_DETAIL, &CCludePackerDlg::OnBnClickedDetail)
	ON_BN_CLICKED(IDC_DETAIL_XML, &CCludePackerDlg::OnBnClickedDetailXml)
	ON_BN_CLICKED(IDC_DETAIL_IMAGE, &CCludePackerDlg::OnBnClickedDetailImage)
	ON_BN_CLICKED(IDC_DETAIL_ERROR, &CCludePackerDlg::OnBnClickedDetailError)
	ON_BN_CLICKED(IDC_PROCESS, &CCludePackerDlg::OnBnClickedProcess)
	ON_NOTIFY(NM_DBLCLK, IDC_DETAIL_LIST, &CCludePackerDlg::OnNMDblclkDetailList)
END_MESSAGE_MAP()


// CCludePackerDlg message handlers

BOOL CCludePackerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	this->MoveWindow( 0, 0, 486, 225 );
	this->CenterWindow();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCludePackerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCludePackerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCludePackerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CCludePackerDlg::OnBnClickedDetail()
{
	CRect rt;
	this->GetWindowRect( rt );
	CButton* pBtnDetail = ((CButton*)GetDlgItem(IDC_DETAIL));

	if( this->m_bIsDetail )
	{
		this->MoveWindow( rt.left, rt.top, 486, 225 );
		this->m_bIsDetail = FALSE;
	}
	else
	{
		this->MoveWindow( rt.left, rt.top, 486, 491 );
		this->m_bIsDetail = TRUE;
	}

	return;
}

void CCludePackerDlg::OnBnClickedDetailXml()
{

}

void CCludePackerDlg::OnBnClickedDetailImage()
{

}

void CCludePackerDlg::OnBnClickedDetailError()
{

}

void CCludePackerDlg::OnBnClickedProcess()
{

}

void CCludePackerDlg::OnNMDblclkDetailList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	*pResult = 0;
}
