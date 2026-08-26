// PageImageCollection.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CludePacker.h"
#include "PageCollect.h"

#include <HELPER/ClLog.H>
#include <HELPER/ClUtil.H>


// CPageImageCollection 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPageCollect, CPropertyPageEx)

CPageCollect::CPageCollect()
	: CPropertyPageEx(CPageCollect::IDD)
{

}

CPageCollect::~CPageCollect()
{
}

void CPageCollect::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageCollect, CPropertyPageEx)
	ON_BN_CLICKED(IDC_BTN_COLLECTIMAGE, &CPageCollect::OnClickCollectImage )
	ON_MESSAGE( WM_USER_LOADIMAGE, &CPageCollect::OnLoadImage )
END_MESSAGE_MAP()


// CPageImageCollection 메시지 처리기입니다.

BOOL CPageCollect::OnSetActive()
{
	CPropertySheetEx* pSheet = (CPropertySheetEx*)GetParent();
	pSheet->SetWizardButtons( 0 );

	this->m_pImageCollector->setImageRoot( this->m_pCludeApp->getImageBasePath() );

	return CPropertyPageEx::OnSetActive();
}


void CPageCollect::OnClickCollectImage()
{
	HANDLE hThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)CollectThread, this, 0, NULL );
	if( hThread != NULL )
		CloseHandle( hThread );

	GetDlgItem( IDC_BTN_COLLECTIMAGE )->EnableWindow( FALSE );

	return;
}


LRESULT CPageCollect::OnLoadImage( WPARAM wParam, LPARAM lParam )
{
	if( (int)wParam == -1 )
	{
		CStatic* pStatus = (CStatic*)GetDlgItem( IDC_TXT_STATUS );
		if( pStatus )
		{
			CString cstrStatus;
			cstrStatus.Format( TEXT("%d Images found. Image loading now..."), (int)lParam );
			pStatus->SetWindowText( cstrStatus );
		}

		CProgressCtrl* pProressLoadImage = (CProgressCtrl*)GetDlgItem( IDC_SLD_LOADIMAGE );
		if( pProressLoadImage )
		{
			pProressLoadImage->SetRange( 0, (int)lParam - 1 );
			pProressLoadImage->SetPos( 0 );
		}
	}
	else
	{
		CProgressCtrl* pProgressLoadImage = (CProgressCtrl*)GetDlgItem( IDC_SLD_LOADIMAGE );
		if( pProgressLoadImage )
			pProgressLoadImage->SetPos( (int)lParam );

		int nMin, nMax;
		pProgressLoadImage->GetRange( nMin, nMax );
		if( (int)lParam >= nMax  )
		{
			BOOL bIsSomeError = FALSE;

			CListCtrl* pImageList = (CListCtrl*)GetDlgItem( IDC_LST_IMAGELIST );
			if( pImageList )
			{
				pImageList->DeleteAllItems();
				CHeaderCtrl* pHeadCtrl = pImageList->GetHeaderCtrl();
				CONST INT nColumCount = pHeadCtrl->GetItemCount();
				INT nIndex;
				for( nIndex = 0; nIndex < nColumCount; nIndex ++ )
					pImageList->DeleteColumn( 0 );

				pImageList->SetExtendedStyle( LVS_EX_FULLROWSELECT );

				pImageList->InsertColumn( 0, TEXT("Index"), LVCFMT_LEFT, 50 );
				pImageList->InsertColumn( 1, TEXT("Path"), LVCFMT_LEFT, 380 );
				pImageList->InsertColumn( 2, TEXT("Status"), LVCFMT_LEFT, 100 );

				CONST INT nImageListSize = this->m_pImageCollector->getImageListSize();
				for( nIndex = 0; nIndex < nImageListSize; nIndex++ )
				{
					CImageCollector::PIMAGEINFO pImageInfo = this->m_pImageCollector->getImageInfo( nIndex );
					if( pImageInfo )
					{
						CString cstrIndex, cstrPath, cstrStatus;

						cstrIndex.Format( TEXT("%d"), nIndex );
						std::wstring wtsPath = ClUtil::PATH::getFileSpecW( pImageInfo->m_wtsFilePath.c_str() );
						cstrPath = wtsPath.c_str();
						if( pImageInfo->m_bIsLoad )	cstrStatus = TEXT("SUCCEEDED");
						else
						{
							cstrStatus = TEXT("FAILED");
							bIsSomeError = TRUE;
						}

						pImageList->InsertItem( nIndex, cstrIndex );
						pImageList->SetItemText( nIndex, 1, cstrPath );
						pImageList->SetItemText( nIndex, 2, cstrStatus );
					}
				}
			}

			CStatic* pStatus = (CStatic*)GetDlgItem( IDC_TXT_STATUS );
			if( pStatus )
			{
				CString cstrStatus;

				if( bIsSomeError )
					cstrStatus.Format( TEXT("%d Images loading complete. But found some errors."), (int)nMax + 1 );
				else
					cstrStatus.Format( TEXT("%d Images loading complete."), (int)nMax + 1 );

				pStatus->SetWindowText( cstrStatus );

				CPropertySheetEx* pSheet = (CPropertySheetEx*)GetParent();
				pSheet->SetWizardButtons( PSWIZB_NEXT );
			}
		}
	}

	return 0;
}


BOOL CPageCollect::SetCludeApp( ClCludeApp* pCludeApp )
{
	if( !pCludeApp )
		return FALSE;

	this->m_pCludeApp = pCludeApp;

	return TRUE;
}


ClCludeApp* CPageCollect::GetCludeApp( VOID )
{
	return this->m_pCludeApp;
}


BOOL CPageCollect::SetImageCollector( CImageCollector* pImageCollector )
{
	if( !pImageCollector )
		return FALSE;

	this->m_pImageCollector = pImageCollector;

	return TRUE;
}


CImageCollector* CPageCollect::GetImageCollector( VOID )
{
	return this->m_pImageCollector;
}


DWORD WINAPI CPageCollect::CollectThread( LPVOID lpArg )
{
	CPageCollect*	pPageCollect	= (CPageCollect*)lpArg;
	ClCludeApp*		pCludeApp		= pPageCollect->GetCludeApp();
	CImageCollector* pImageCollector= pPageCollect->GetImageCollector();

	TiXmlDocument xmlDoc;

	CONST INT nFileCount = pCludeApp->getXmlFileListSize();
	INT nIndex;
	for( nIndex = 0; nIndex < nFileCount; nIndex++ )
	{
		const char* pBuffer = pCludeApp->getXml( nIndex );
		if( !pBuffer )
			continue;

		xmlDoc.Clear();
		xmlDoc.Parse( pBuffer );

		TiXmlElement* pRoot = xmlDoc.RootElement();
		CPageCollect::parseDialog( pRoot, pImageCollector );

		TiXmlElement* pEleControls = pRoot->FirstChildElement( "controls" );
		if( !pEleControls )
			continue;

		TiXmlElement* pEleControl = pEleControls->FirstChildElement( "control" );
		while( pEleControl )
		{
			const char* lpszControlType = pEleControl->Attribute( "type" );
			if( lpszControlType )
			{
				switch( lpszControlType[0] )
				{
				case 'b':
					if(!strcmp(lpszControlType, "button"))
						CPageCollect::parseButton( pEleControl, pImageCollector );
					else
						CLD_FL( DF_ERROR, "Unrecognized control(%s)", lpszControlType );
					break;

				case 'd':
					if(!strcmp(lpszControlType, "digit"))
						CPageCollect::parseDigit( pEleControl, pImageCollector );
					else
						CLD_FL( DF_ERROR, "Unrecognized control(%s)", lpszControlType );
					break;
					
				case 'i':
					if(!strcmp(lpszControlType, "image"))
						CPageCollect::parseImage( pEleControl, pImageCollector );
					else
						CLD_FL( DF_ERROR, "Unrecognized control(%s)", lpszControlType );
					break;

				case 's':
					if(!strcmp(lpszControlType, "slider"))
						CPageCollect::parseSlider( pEleControl, pImageCollector );
					else
						CLD_FL( DF_ERROR, "Unrecognized control(%s)", lpszControlType );
					break;

				case 't':
					if(!strcmp(lpszControlType, "text"))
						CPageCollect::parseText( pEleControl, pImageCollector );
					else
						CLD_FL( DF_ERROR, "Unrecognized control(%s)", lpszControlType );
					break;

				case 'l':
					if(!strcmp(lpszControlType, "list"))
						CPageCollect::parseListCtrl( pEleControl, pImageCollector );
					else
						CLD_FL( DF_ERROR, "Unrecognized control(%s)", lpszControlType );
					break;

				case 'g':
					if(!strcmp(lpszControlType, "gsensor"))
						CPageCollect::parseGSensor( pEleControl, pImageCollector );
					else
						CLD_FL( DF_ERROR, "Unrecognized control(%s)", lpszControlType );
					break;
					
				case 'a': 
					if(!strcmp(lpszControlType, "analogmeter"))		
						CPageCollect::parseGSensor( pEleControl, pImageCollector );
					else
						CLD_FL( DF_ERROR, "Unrecognized control(%s)", lpszControlType );
					break;

				default: CLD_FL( DF_ERROR, "Unrecognized control(%s)", lpszControlType );
				}
			}

			pEleControl = pEleControl->NextSiblingElement( "control" );
		}
	}

	pPageCollect->PostMessage( WM_USER_LOADIMAGE, (WPARAM)-1, (WPARAM)pImageCollector->getImageListSize() );

	CONST INT nListCount = pImageCollector->getImageListSize();
	for( nIndex = 0; nIndex < nListCount; nIndex ++ )
	{
		CImageCollector::PIMAGEINFO pImageInfo = pImageCollector->getImageInfo( nIndex );
		if( !pImageInfo )
		{
			pPageCollect->PostMessage( WM_USER_LOADIMAGE, (WPARAM)-2, (WPARAM)nIndex );
			continue;
		}

		if( ClUtil::PATH::isExistFileW(pImageInfo->m_wtsFilePath.c_str()) )
		{
			FILE* fpImage = _wfopen( pImageInfo->m_wtsFilePath.c_str(), TEXT("rb") );
			if( !fpImage )
			{
				pPageCollect->PostMessage( WM_USER_LOADIMAGE, (WPARAM)-2, (WPARAM)nIndex );
				continue;
			}

			fseek( fpImage, 0L, SEEK_END );
			int nFileSize = ftell( fpImage );
			fseek( fpImage, 0L, SEEK_SET );

			pImageInfo->m_pBuffer = new BYTE[nFileSize];
			if( !pImageInfo->m_pBuffer )
			{
				CLD_FL( DF_ERROR, "Out of memory" );
				ASSERT(1);
			}

			pImageInfo->m_nSize = nFileSize;

			fread( pImageInfo->m_pBuffer, nFileSize, 1, fpImage );
			fclose( fpImage );
			pImageInfo->m_bIsLoad = TRUE;
		}
		else
		{
			pPageCollect->PostMessage( WM_USER_LOADIMAGE, (WPARAM)-2, (WPARAM)nIndex );
			continue;
		}

		pPageCollect->PostMessage( WM_USER_LOADIMAGE, (WPARAM)0, (WPARAM)nIndex );
	}

	return 0;
}


void CPageCollect::parseDialog( TiXmlElement* pEleDialog, CImageCollector* pImageCollector )
{
	if( !pEleDialog || !pImageCollector )
		return;

	TiXmlElement* pEleProperty = pEleDialog->FirstChildElement( "property" );
	if( !pEleProperty )
		return;

	TiXmlElement* pEleBgImages = pEleProperty->FirstChildElement( "bgimgs" );
	if( !pEleBgImages )
		return;

	TiXmlElement* pEleBgImage = pEleBgImages->FirstChildElement( "bgimg" );
	while( pEleBgImage )
	{
		TiXmlElement* pEleImage = pEleBgImage->FirstChildElement( "image" );
		while( pEleImage )
		{
			const char* lpszString = pEleImage->GetText();
			if( lpszString )
			{
				WCHAR* pwzPath = NULL;
				if( ClUtil::STRING::convertUTF8ToUnicode(lpszString, &pwzPath) )
					pImageCollector->addImagePath( pwzPath );
				if( pwzPath ) delete [] pwzPath;
			}

			pEleImage = pEleImage->NextSiblingElement( "image" );
		}

		pEleBgImage = pEleBgImage->NextSiblingElement( "bgimg" );
	}

	return;
}


void CPageCollect::parseButton( TiXmlElement* pEleButton, CImageCollector* pImageCollector )
{
	if( !pEleButton || !pImageCollector )
		return;

	TiXmlElement* pEleSkins = pEleButton->FirstChildElement( "skins" );
	if( !pEleSkins )
		return;

	TiXmlElement* pEleSkin = pEleSkins->FirstChildElement( "skin" );
	while( pEleSkin )
	{
		const char* lpszString = pEleSkin->GetText();
		if( lpszString )
		{
			WCHAR* pwzPath = NULL;
			if( ClUtil::STRING::convertUTF8ToUnicode(lpszString, &pwzPath) )
				pImageCollector->addImagePath( pwzPath );
			if( pwzPath ) delete [] pwzPath;
		}

		pEleSkin = pEleSkin->NextSiblingElement( "skin" );
	}

	return;
}

void CPageCollect::parseDigit( TiXmlElement* pEleDigit, CImageCollector* pImageCollector )
{
	if( !pEleDigit || !pImageCollector )
		return;

	TiXmlElement* pEleSkins = pEleDigit->FirstChildElement( "skins" );
	if( !pEleSkins )
		return;

	TiXmlElement* pEleSkin = pEleSkins->FirstChildElement( "skin" );
	while( pEleSkin )
	{
		const char* lpszString = pEleSkin->GetText();
		if( lpszString )
		{
			WCHAR* pwzPath = NULL;
			if( ClUtil::STRING::convertUTF8ToUnicode(lpszString, &pwzPath) )
				pImageCollector->addImagePath( pwzPath );
			if( pwzPath ) delete [] pwzPath;
		}

		pEleSkin = pEleSkin->NextSiblingElement( "skin" );
	}

	return;
}


void CPageCollect::parseImage( TiXmlElement* pEleImage, CImageCollector* pImageCollector )
{
	if( !pEleImage || !pImageCollector )
		return;

	TiXmlElement* pEleImagePath = pEleImage->FirstChildElement( "image" );
	const char* lpszString = pEleImagePath->GetText();
	if( lpszString )
	{
		WCHAR* pwzPath = NULL;
		if( ClUtil::STRING::convertUTF8ToUnicode(lpszString, &pwzPath) )
			pImageCollector->addImagePath( pwzPath );
		if( pwzPath ) delete [] pwzPath;
	}

	return;
}


void CPageCollect::parseSlider( TiXmlElement* pEleSlider, CImageCollector* pImageCollector )
{
	if( !pEleSlider || !pImageCollector )
		return;

	TiXmlElement* pEleSkins = pEleSlider->FirstChildElement( "skins" );
	if( !pEleSkins )
		return;

	TiXmlElement* pEleSkin = pEleSkins->FirstChildElement( "skin" );
	while( pEleSkin )
	{
		const char* lpszString = pEleSkin->GetText();
		if( lpszString )
		{
			WCHAR* pwzPath = NULL;
			if( ClUtil::STRING::convertUTF8ToUnicode(lpszString, &pwzPath) )
				pImageCollector->addImagePath( pwzPath );
			if( pwzPath ) delete [] pwzPath;
		}

		pEleSkin = pEleSkin->NextSiblingElement( "skin" );
	}

	return;
}


void CPageCollect::parseText( TiXmlElement* pEleText, CImageCollector* pImageCollector )
{
	if( !pEleText || !pImageCollector )
		return;

	return;
}


void CPageCollect::parseListCtrl( TiXmlElement* pEleListCtrl, CImageCollector* pImageCollector )
{
	if( !pEleListCtrl || !pImageCollector )
		return;

	return;
}


void CPageCollect::parseGSensor( TiXmlElement* pEleGSensor, CImageCollector* pImageCollector )
{
	if( !pEleGSensor || !pImageCollector )
		return;

	TiXmlElement* pEleBgImg = pEleGSensor->FirstChildElement( "bgimg" );
	const char* lpszString = pEleBgImg->GetText();
	if( lpszString )
	{
		WCHAR* pwzPath = NULL;
		if( ClUtil::STRING::convertUTF8ToUnicode(lpszString, &pwzPath) )
			pImageCollector->addImagePath( pwzPath );
		if( pwzPath ) delete [] pwzPath;
	}

	return;
}

