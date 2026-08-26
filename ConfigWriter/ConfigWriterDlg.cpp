
// ConfigWriterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ConfigWriter.h"
#include "ConfigWriterDlg.h"

#include <HELPER/CLUtil.H>
#include <CLUDE/version.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CConfigWriterDlg dialog

CConfigWriterDlg::CConfigWriterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigWriterDlg::IDD, pParent)
{
	this->m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	this->m_bIsShowDetail	= FALSE;
	this->m_nOriginHeight	= 0;
	this->m_bIsNeedFind		= FALSE;
}


void CConfigWriterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CConfigWriterDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_CBN_DROPDOWN(IDC_COMBO_DRIVE,		&CConfigWriterDlg::OnCbnDropdownDrive	)
	ON_BN_CLICKED(IDC_BTN_DETAIL,			&CConfigWriterDlg::OnClickDetail		)
	ON_BN_CLICKED(IDC_BTN_APPPATH,			&CConfigWriterDlg::OnClickAppPath		)
	ON_CBN_SELCHANGE(IDC_COMBO_SELMODEL,	&CConfigWriterDlg::OnCbnSelchangeSelModel)
	ON_BN_CLICKED(IDC_BTN_WRITETIME,		&CConfigWriterDlg::OnClickWritetime		)
	ON_BN_CLICKED(IDC_BTN_WRITECONFIG,		&CConfigWriterDlg::OnClickWriteConfig	)
END_MESSAGE_MAP()


// CConfigWriterDlg message handlers

BOOL CConfigWriterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	WCHAR wcsPath[MAX_PATH];
	::GetModuleFileName( NULL, wcsPath, MAX_PATH );
	std::wstring wtsPath = ClUtil::PATH::getPathSpecW( wcsPath );
	CString cstrModulePath = wtsPath.c_str();
	cstrModulePath += TEXT("\\");
	cstrModulePath += TEXT(APPLICATION_XML_NAME);

	CEdit* pEditAppPath = (CEdit*)GetDlgItem( IDC_EDIT_APPPATH );
	CButton* pBtnAppPath = (CButton*)GetDlgItem( IDC_BTN_APPPATH );
	if( ClUtil::PATH::isExistFileW(cstrModulePath) )
	{
		pEditAppPath->SetWindowText( cstrModulePath );

		char* lpszAppPath = new char[MAX_PATH];
		strcpy_s( lpszAppPath, MAX_PATH, CT2A(cstrModulePath) );
		this->m_stsAppPath = lpszAppPath;
		if( lpszAppPath ) delete [] lpszAppPath;

		pBtnAppPath->SetWindowText( TEXT("Load") );
	}
	else
	{
		pEditAppPath->SetWindowText( TEXT("You have to select 'application.xml' path.") );
		pBtnAppPath->SetWindowText( TEXT("Find") );
		this->m_bIsNeedFind = TRUE;
	}

	CRect rect;
	this->GetWindowRect( rect );
	this->m_nOriginHeight = rect.Height();

	int nScreenWidth	= GetSystemMetrics( SM_CXSCREEN );
	int nScreenHeight	= GetSystemMetrics( SM_CYSCREEN );
	int x = (nScreenWidth - rect.Width()) / 2;
	int y = (nScreenHeight - rect.Height()) / 2;

	this->MoveWindow( x, y, rect.Width(), SZ_WINDOW_MINI, TRUE );
	this->GetWindowRect( rect );

	this->OnCbnDropdownDrive();

	return TRUE;
}


void CConfigWriterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}


HCURSOR CConfigWriterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CConfigWriterDlg::OnCbnDropdownDrive()
{
	CComboBox* pComboDrive = (CComboBox*)GetDlgItem( IDC_COMBO_DRIVE );
	pComboDrive->ResetContent();

	CString strDrivePath = TEXT(" :\\");
	DWORD drive_info = ::GetLogicalDrives();

	for(int i=0; drive_info; i++)
	{
		if(drive_info & 1)
		{
			strDrivePath.Format( TEXT("%c:\\"),'A' + i );

			switch(::GetDriveType(strDrivePath))
			{
			case DRIVE_REMOVABLE:
				pComboDrive->AddString(strDrivePath);
				break;
			case DRIVE_FIXED:
			case DRIVE_CDROM:
			default: break;
			}
		}
		drive_info >>=1;
	}

	pComboDrive->SetCurSel(0);

	return;
}


void CConfigWriterDlg::OnClickWriteConfig()
{
	UpdateData( TRUE );

	CString cstrDrive;
	this->GetComboString( IDC_COMBO_DRIVE, cstrDrive, SZ_MAX_MODEL_NAME );

	cstrDrive += this->m_CludeApp.getConfigPath();
	CString cstrConfigPath;
	ClUtil::PATH::makeValidPath( cstrDrive, cstrConfigPath );

	if( !this->m_Configure.writeConfig(cstrDrive) )
	{
		AfxMessageBox( TEXT("Can not write Configuration data."), MB_OK|MB_ICONSTOP );
		return;
	}

	AfxMessageBox( TEXT("Ok, Configuration file has been written."), MB_OK|MB_ICONINFORMATION );

	return;
}


void CConfigWriterDlg::OnClickDetail()
{
	CRect rect;
	this->GetWindowRect( rect );

	if( m_bIsShowDetail )
	{
		this->MoveWindow( rect.left, rect.top, rect.Width(), SZ_WINDOW_NORMAL );
		this->m_bIsShowDetail = FALSE;
	}
	else
	{
		this->MoveWindow( rect.left, rect.top, rect.Width(), this->m_nOriginHeight );
		this->m_bIsShowDetail = TRUE;
	}

	return;
}


void CConfigWriterDlg::OnClickAppPath()
{
	if( this->m_bIsNeedFind )
	{
		CString strFilter = TEXT("xml (*.xml)|*.xml||");
		CFileDialog dlg( TRUE, NULL, NULL, OFN_HIDEREADONLY, TEXT("xml file (*.xml)|*.xml||"), this );

		if( !dlg.DoModal() == IDOK )
			return;

		CString cstrDstFile = dlg.GetPathName();
		if( ClUtil::PATH::isExistFileW(cstrDstFile) )
		{
			CEdit* pEditAppPath = (CEdit*)GetDlgItem( IDC_EDIT_APPPATH );
			pEditAppPath->SetWindowText( cstrDstFile );

			CButton* pBtnAppPath = (CButton*)GetDlgItem( IDC_BTN_APPPATH );
			pBtnAppPath->SetWindowText( TEXT("Load") );

			char* lpszAppPath = new char[MAX_PATH];
			strcpy_s( lpszAppPath, MAX_PATH, CT2A(cstrDstFile) );
			this->m_stsAppPath = lpszAppPath;
			if( lpszAppPath ) delete [] lpszAppPath;

			this->m_bIsNeedFind = FALSE;
		}
	}
	else
	{
		if( !this->m_CludeApp.loadCludeFile(this->m_stsAppPath) )
		{
			AfxMessageBox( TEXT("Can not load application information(Application.xml)."), MB_OK | MB_ICONSTOP );
			return;
		}

		CString cstrBrandName = this->m_CludeApp.getBrandName();
		if( !cstrBrandName.GetLength() )
		{
			AfxMessageBox( TEXT("Can not find brand name. You have to add brand name(Application.xml)."), MB_OK | MB_ICONSTOP );
			return;
		}

		CEdit* pEditBrandName = (CEdit*)GetDlgItem( IDC_EDIT_BRANDNAME );
		pEditBrandName->SetWindowText( cstrBrandName );

		CEdit* pEditSubPath = (CEdit*)GetDlgItem( IDC_EDIT_SUBPATH );
		pEditSubPath->SetWindowText( this->m_CludeApp.getConfigPath() );

		CComboBox* pWndComboModel = (CComboBox*)GetDlgItem( IDC_COMBO_SELMODEL );
		if( pWndComboModel )
		{
			pWndComboModel->ResetContent();
			CONST INT nModelListSize = this->m_CludeApp.getModelListSize();
			if( nModelListSize > 0 )
			{
				INT nIndex;
				for( nIndex = 0; nIndex < nModelListSize; nIndex ++ )
				{
					CString cstrModel = this->m_CludeApp.getModelName( nIndex );
					pWndComboModel->AddString( cstrModel );
				}
				pWndComboModel->SetCurSel( 0 );
			}
			else
			{
				AfxMessageBox( TEXT("Can not find model name. You have to add model name(Application.xml)."), MB_OK | MB_ICONSTOP );
				return;
			}
		}

		TCHAR* lptzModel = new TCHAR[SZ_MAX_MODEL_NAME];
		pWndComboModel->GetLBText( pWndComboModel->GetCurSel(), lptzModel );
		CString cstrModelName = lptzModel;
		if( lptzModel ) delete [] lptzModel;

		if( !this->m_Configure.setModelName(cstrModelName) )
		{
			CString cstrMessage;
			cstrMessage.Format( TEXT("Given model name(%s) is not registered. See 'CBBConfigure' class."), cstrModelName );
			AfxMessageBox( cstrMessage, MB_OK | MB_ICONSTOP );
			return;
		}

		this->FillConfigList();

		CButton* pBtnAppPath = (CButton*)GetDlgItem( IDC_BTN_APPPATH );
		pBtnAppPath->EnableWindow( FALSE );

		CRect rect;
		this->GetWindowRect( rect );

		int nScreenWidth	= GetSystemMetrics( SM_CXSCREEN );
		int nScreenHeight	= GetSystemMetrics( SM_CYSCREEN );
		int x = (nScreenWidth - rect.Width()) / 2;
		int y = (nScreenHeight - rect.Height()) / 2;

		this->MoveWindow( rect.left, rect.top, rect.Width(), SZ_WINDOW_NORMAL );

		UpdateData( FALSE );
	}

	return;
}


void CConfigWriterDlg::OnClickWritetime()
{
	CDateTimeCtrl* pDT = (CDateTimeCtrl*)GetDlgItem( IDC_DTPICKER_DEVICETIME );

	SYSTEMTIME time;
	pDT->GetTime( &time );

	CString cstrDrive;
	this->GetComboString( IDC_COMBO_DRIVE, cstrDrive, SZ_MAX_MODEL_NAME );

	CString cstrTimePath;
	ClUtil::PATH::makeValidPath( cstrDrive + TEXT("\\") + this->m_CludeApp.getTimeFilePath(), cstrTimePath );
	if( !CBBConfigure::writeTime(cstrTimePath, time) )
	{
		AfxMessageBox( TEXT("Can not write device time."), MB_OK|MB_ICONSTOP );
		return;
	}

	AfxMessageBox( TEXT("Ok, Device time has been written."), MB_OK|MB_ICONINFORMATION );

	return;
}


void CConfigWriterDlg::OnCbnSelchangeSelModel()
{
	CString cstrModelName;
	this->GetComboString( IDC_COMBO_SELMODEL, cstrModelName, MAX_PATH );

	if( !this->m_Configure.setModelName(cstrModelName) )
	{
		CString cstrMessage;
		cstrMessage.Format( TEXT("Given model name(%s) is not registered. See 'CBBConfigure' class."), cstrModelName );
		AfxMessageBox( cstrMessage, MB_OK | MB_ICONSTOP );
		return;
	}

	this->FillConfigList();

	return;
}


void CConfigWriterDlg::FillConfigList( void )
{
	CListCtrl* pWndConfigList = (CListCtrl*)GetDlgItem( IDC_LST_CONFGIS );
	if( pWndConfigList )
	{
		pWndConfigList->DeleteAllItems();
		CHeaderCtrl* pHeadCtrl = pWndConfigList->GetHeaderCtrl();
		CONST INT nColumCount = pHeadCtrl->GetItemCount();
		INT nIndex;
		for( nIndex = 0; nIndex < nColumCount; nIndex ++ )
			pWndConfigList->DeleteColumn( 0 );

		pWndConfigList->SetExtendedStyle( LVS_EX_FULLROWSELECT );

		pWndConfigList->InsertColumn( 0, TEXT("Index"), LVCFMT_LEFT, 50 );
		pWndConfigList->InsertColumn( 1, TEXT("Name"), LVCFMT_LEFT, 140 );
		pWndConfigList->InsertColumn( 2, TEXT("Type"), LVCFMT_LEFT, 60 );
		pWndConfigList->InsertColumn( 3, TEXT("Value"), LVCFMT_LEFT, 50 );
		pWndConfigList->InsertColumn( 4, TEXT("Default"), LVCFMT_LEFT, 50 );

		CONST INT nConfigListSize = this->m_Configure.getConfigListSize();
		for( nIndex = 0; nIndex < nConfigListSize; nIndex++ )
		{
			CBBConfigure::BBCONFIG config;
			if( this->m_Configure.getConfig(nIndex, &config) )
			{
				CString cstrIndex, cstrType, cstrValue, cstrDefaultValue;

				cstrIndex.Format( TEXT("%d"), nIndex );
				switch( (INT)config.m_type )
				{
				case CBBConfigure::TYPE_INTEGER	: cstrType.Format( TEXT("Integer") ); break;
				case CBBConfigure::TYPE_BOOLEAN	: cstrType.Format( TEXT("Boolean") ); break;
				case CBBConfigure::TYPE_FLOAT10	: cstrType.Format( TEXT("Float/10") ); break;
				case CBBConfigure::TYPE_FLOAT100: cstrType.Format( TEXT("Float/100") ); break;
				case CBBConfigure::TYPE_FLOAT1000: cstrType.Format( TEXT("Float/1000") ); break;
				}
				cstrValue.Format( TEXT("%d"), config.m_Value );
				cstrDefaultValue.Format( TEXT("%d"), config.m_DefaultValue );

				pWndConfigList->InsertItem( nIndex, cstrIndex );
				pWndConfigList->SetItemText( nIndex, 1, config.m_hashedName.c_str() );
				pWndConfigList->SetItemText( nIndex, 2, cstrType );
				pWndConfigList->SetItemText( nIndex, 3, cstrValue );
				pWndConfigList->SetItemText( nIndex, 4, cstrDefaultValue );
			}
		}
	}

	return;
}


BOOL CConfigWriterDlg::GetComboString( int nResID, CString& cstrComboString, int nSize )
{
	if( nSize <= 0 )
		return FALSE;

	CComboBox* pWndCombo = (CComboBox*)GetDlgItem( nResID );
	if( !pWndCombo )
		return FALSE;

	TCHAR* lptzComboString = new TCHAR[nSize];
	pWndCombo->GetLBText( pWndCombo->GetCurSel(), lptzComboString );
	cstrComboString = lptzComboString;
	if( lptzComboString ) delete [] lptzComboString;

	return TRUE;
}
