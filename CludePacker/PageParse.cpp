// PageLoadAppXml.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CludePacker.h"
#include "PageParse.h"

#include <HELPER/CLUtil.H>
#include <CLUDE/version.h>

// CPageLoadAppXml 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPageParse, CPropertyPageEx)

CPageParse::CPageParse() : CPropertyPageEx(CPageParse::IDD)
{
	this->m_pCludeApp	= NULL;
	this->m_bIsNeedFind	= FALSE;
}

CPageParse::~CPageParse()
{

}

void CPageParse::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageParse, CPropertyPageEx)
	ON_BN_CLICKED(IDC_BTN_APPPATH, &CPageParse::OnClickAppPath)
END_MESSAGE_MAP()


BOOL CPageParse::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	WCHAR wcsPath[MAX_PATH];
	::GetModuleFileName( NULL, wcsPath, MAX_PATH );
	this->m_stsXmlPath = ClUtil::PATH::getPathSpecW( wcsPath );
	CString cstrModulePath = this->m_stsXmlPath.c_str();
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

	return TRUE;
}


void CPageParse::OnClickAppPath()
{
	if( this->m_bIsNeedFind )
	{
		CString strFilter = TEXT("xml (*.xml)|*.xml||");
		CFileDialog dlg( TRUE, NULL, NULL, OFN_HIDEREADONLY, TEXT("xml file (*.xml)|*.xml||"), this );

		if( !dlg.DoModal() == IDOK )
			return;

		CString cstrDstFile = dlg.GetPathName();
		this->m_stsXmlPath = ClUtil::PATH::getPathSpecW( cstrDstFile );
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
		if( !this->m_pCludeApp->loadCludeFile(this->m_stsAppPath) )
		{
			AfxMessageBox( TEXT("Can not load application information(Application.xml)."), MB_OK | MB_ICONSTOP );
			return;
		}

		BOOL bIsNotExitedFile = FALSE;
		CListCtrl* pWndConfigList = (CListCtrl*)GetDlgItem( IDC_LST_XMLFILES );
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
			pWndConfigList->InsertColumn( 1, TEXT("Type"), LVCFMT_LEFT, 150 );
			pWndConfigList->InsertColumn( 2, TEXT("XML File"), LVCFMT_LEFT, 200 );
			pWndConfigList->InsertColumn( 3, TEXT("Exist"), LVCFMT_LEFT, 50 );

			CONST INT nXmlListSize = this->m_pCludeApp->getXmlFileListSize();
			for( nIndex = 0; nIndex < nXmlListSize; nIndex++ )
			{
				HashedString hashedDialogID;
				std::string stsXmlFile;

				if( this->m_pCludeApp->getXmlFileInfo(nIndex, hashedDialogID, stsXmlFile) )
				{
					CString cstrIndex, cstrType, cstrXmlFile, cstrExist;
					BOOL bIsExist = FALSE;

					cstrIndex.Format( TEXT("%d"), nIndex );
					cstrType = hashedDialogID.c_str();
					std::string stsFileName = ClUtil::PATH::getFileSpec( stsXmlFile.c_str() );
					ClUtil::STRING::convertUTF8ToCString( stsFileName.c_str(), cstrXmlFile );
					if( !(bIsExist = ClUtil::PATH::isExistFile(stsXmlFile.c_str())) )
						bIsNotExitedFile = TRUE;

					cstrExist = bIsExist ? TEXT("YES") : TEXT("NO");

					pWndConfigList->InsertItem( nIndex, cstrIndex );
					pWndConfigList->SetItemText( nIndex, 1, cstrType );
					pWndConfigList->SetItemText( nIndex, 2, cstrXmlFile );
					pWndConfigList->SetItemText( nIndex, 3, cstrExist );
				}
			}
		}

		CWnd* pWnd = (CButton*)GetDlgItem( IDC_BTN_APPPATH );
		pWnd->EnableWindow( FALSE );

		if( bIsNotExitedFile )
		{
			GetDlgItem( IDC_TXT_REPORT )->SetWindowText( TEXT("Error Report : Found a file that does not exist.") );
		}
		else
		{
			GetDlgItem( IDC_TXT_REPORT )->SetWindowText( TEXT("Report : No error found.") );

			CPropertySheetEx* pSheet = (CPropertySheetEx*)GetParent();
			pSheet->SetWizardButtons( PSWIZB_NEXT );
		}
	}

	return;
}


BOOL CPageParse::OnSetActive()
{
	CPropertySheetEx* pSheet = (CPropertySheetEx*)GetParent();
	pSheet->SetWizardButtons( 0 );

	return CPropertyPageEx::OnSetActive();
}


BOOL CPageParse::SetCludeApp( ClCludeApp* pCludeApp )
{
	if( pCludeApp == NULL )
		return FALSE;

	this->m_pCludeApp = pCludeApp;

	return TRUE;
}
