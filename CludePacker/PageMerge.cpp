// PageCludeMerge.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CludePacker.h"
#include "PageMerge.h"

#include <HELPER/CLUtil.H>

// CPageCludeMerge 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPageMerge, CPropertyPageEx)

CPageMerge::CPageMerge()
	: CPropertyPageEx(CPageMerge::IDD)
{
	this->m_bIsCompress	= FALSE;
}

CPageMerge::~CPageMerge()
{
}

void CPageMerge::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageMerge, CPropertyPageEx)
	ON_BN_CLICKED(IDC_BTN_MERGE, &CPageMerge::OnClickMerge)
END_MESSAGE_MAP()


// CPageCludeMerge 메시지 처리기입니다.

BOOL CPageMerge::OnSetActive()
{
	CPropertySheetEx* pSheet = (CPropertySheetEx*)GetParent();
	pSheet->SetWizardButtons( 0 );

	CComboBox* pComboVersion = (CComboBox*)GetDlgItem( IDC_COMBO_VERSION );
	if( pComboVersion )
	{
		pComboVersion->ResetContent();
		pComboVersion->AddString( TEXT("CLUDE ver 1.0") );
		pComboVersion->SetCurSel( 0 );
	}

	CComboBox* pComboFormat = (CComboBox*)GetDlgItem( IDC_COMBO_FORMAT );
	if( pComboFormat )
	{
		pComboFormat->ResetContent();
		pComboFormat->AddString( TEXT("NON-COMPRESSED") );
		//pComboFormat->AddString( TEXT("COMPRESSED(LZMA)") );
		pComboFormat->SetCurSel( 0 );
	}

	XMLHEADER xmlHeader;
	HashedString hashedDialogID;
	std::string stsFileName;
	CONST INT nXmlListSize = this->m_pCludeApp->getXmlFileListSize();
	INT nIndex;
	for( nIndex = 0; nIndex < nXmlListSize; nIndex ++ )
	{
		if( this->m_pCludeApp->getXmlFileInfo(nIndex, hashedDialogID, stsFileName) )
		{
			std::string stsFileSpec = ClUtil::PATH::getFileSpec( stsFileName.c_str() );
			strcpy_s( xmlHeader.m_strName, MAX_PATH, stsFileSpec.c_str() );
			xmlHeader.m_nSize = strlen( this->m_pCludeApp->getXml(nIndex) );
			this->m_XMLHeaderList.push_back( xmlHeader );
		}
	}

	IMAGEHEADER imageHeader;
	CONST INT nImageListSize = this->m_pImageCollector->getImageListSize();
	CImageCollector::PIMAGEINFO pImageInfo;
	for( nIndex = 0; nIndex < nImageListSize; nIndex ++ )
	{
		imageHeader.m_nSize = 0;
		memset( imageHeader.m_strName, 0, MAX_PATH );

		pImageInfo = this->m_pImageCollector->getImageInfo( nIndex );
		if( pImageInfo )
		{
			HashedString hashedImage;

			imageHeader.m_nSize = pImageInfo->m_nSize;
			if( this->m_pImageCollector->getImageHash(nIndex, hashedImage) )
				WideCharToMultiByte( CP_ACP, 0, hashedImage.c_str(), -1, imageHeader.m_strName, MAX_PATH, NULL, NULL );

			this->m_ImageHeaderList.push_back( imageHeader );
		}
	}

	CStatic* pTxtInfor = (CStatic*)GetDlgItem( IDC_TXT_FILEINFO );
	if( pTxtInfor )
	{
		CString cstrTextInfo;
		cstrTextInfo.Format( TEXT("%d XML files(including Application.xml) and %d Image files collected."),
			this->m_XMLHeaderList.size(), this->m_ImageHeaderList.size() );
		pTxtInfor->SetWindowText( cstrTextInfo );
	}

	return CPropertyPageEx::OnSetActive();
}


void CPageMerge::OnClickMerge()
{
	HEADER	header;

	FILE* fpCLUDE;
	fopen_s( &fpCLUDE, FILENAME_CLUDE, "wb+" );
	if( !fpCLUDE )
	{
		AfxMessageBox( TEXT("Creating CLUDE file(CLUDE.PAK) failed."), MB_OK | MB_ICONSTOP );
		return;
	}

	header.m_strName[0]		= 'C';
	header.m_strName[1]		= 'L';
	header.m_strName[2]		= 'U';
	header.m_strName[3]		= 'D';
	header.m_strName[4]		= 'E';
	header.m_nVersion		= 1;
	header.m_nFormat		= 0;
	header.m_nXMLCounter	= this->m_XMLHeaderList.size();
	header.m_nImageCounter	= this->m_ImageHeaderList.size();

	fwrite( &header, sizeof(HEADER), 1, fpCLUDE );

	ITER_XMLHEADER iXmlHeader = this->m_XMLHeaderList.begin();
	ITER_XMLHEADER iXmlHeaderEnd = this->m_XMLHeaderList.end();
	for( ; iXmlHeader != iXmlHeaderEnd; iXmlHeader ++ )
	{
		fwrite( &(*iXmlHeader), sizeof(XMLHEADER), 1, fpCLUDE );
	}

	ITER_IMAGEHEADER iImageHeader = this->m_ImageHeaderList.begin();
	ITER_IMAGEHEADER iImageHeaderEnd = this->m_ImageHeaderList.end();
	for( ; iImageHeader != iImageHeaderEnd; iImageHeader ++ )
	{
		fwrite( &(*iImageHeader), sizeof(IMAGEHEADER), 1, fpCLUDE );
	}

	CONST INT nXmlListSize = this->m_pCludeApp->getXmlFileListSize();
	INT nIndex;
	for( nIndex = 0; nIndex < nXmlListSize; nIndex ++ )
	{
		const char* lpszXmlContents = this->m_pCludeApp->getXml( nIndex );
		int nSize = strlen(lpszXmlContents);
		fwrite( lpszXmlContents, nSize, 1, fpCLUDE );
	}

	CONST INT nImageListSize = this->m_pImageCollector->getImageListSize();
	for( nIndex = 0; nIndex < nImageListSize; nIndex ++ )
	{
		CImageCollector::PIMAGEINFO pImageInfo = this->m_pImageCollector->getImageInfo( nIndex );
		fwrite( pImageInfo->m_pBuffer, pImageInfo->m_nSize, 1, fpCLUDE );
	}

	fclose( fpCLUDE );

	CProgressCtrl* pProgress = (CProgressCtrl*)GetDlgItem( IDC_SLD_MERGE );
	if( pProgress )
	{
		pProgress->SetRange( 0, 100 );
		pProgress->SetPos( 100 );
	}

	CStatic* pTextStatus = (CStatic*)GetDlgItem( IDC_TXT_STATUS );
	if( pTextStatus )
		pTextStatus->SetWindowText( TEXT("Merging complete.") );

	CPropertySheetEx* pSheet = (CPropertySheetEx*)GetParent();
	pSheet->SetWizardButtons( PSWIZB_FINISH );

	return;
}


BOOL CPageMerge::SetImageCollector( CImageCollector* pImageCollector )
{
	if( !pImageCollector )
		return FALSE;

	this->m_pImageCollector = pImageCollector;

	return TRUE;
}


BOOL CPageMerge::SetCludeApp( ClCludeApp* pCludeApp )
{
	if( !pCludeApp )
		return FALSE;

	this->m_pCludeApp = pCludeApp;

	return TRUE;
}


DWORD WINAPI CPageMerge::MergeThread( LPVOID lpArg )
{
	return 0;
}