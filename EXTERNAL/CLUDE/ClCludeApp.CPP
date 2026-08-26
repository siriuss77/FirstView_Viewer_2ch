//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClCludeApp.CPP
//
//	Description: CLENA CLUDE parser for Application information
//
//	Update History
// 
//		2012.01.03 [Allen Han] Adding CLUDE packager specification
//		2011.10.10 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. INCLUDE

#include <stdafx.h>
#include <HELPER/ClLog.H>
#include <HELPER/CLUtil.H>
#include "version.h"
#include "ClCludeApp.h"

//================================================================================================= E. INCLUDE
//================================================================================================= S. CON/DCON

ClCludeApp::ClCludeApp()
{
	this->m_cstrBrand			= TEXT("");
	this->m_cstrModulePath		= TEXT("");
	this->m_cstrImagePath		= TEXT("");
	this->m_cstrScriptPath		= TEXT("");
	this->m_cstrMapPath			= TEXT("");
	this->m_cstrSponsorLinkPath 	= TEXT("");
	this->m_cstrConfigPath		= TEXT("");
	this->m_cstrTimeFilePath	= TEXT("");

	this->m_cstrDefaultFont		= TEXT("Arial");
	this->m_nDefaultFontSize	= -1;

	this->m_cstrCompanySite		= TEXT("");
	this->m_cstrUpdateSite		= TEXT("");
	this->m_cstrUpdateViewerPage = TEXT("");
	this->m_cstrUpdateFirmwarePage = TEXT("");
	this->m_cstrLogFileName	= TEXT("");
	this->m_cstrBBPCLWVersion	= VS_SW_VERSION;
	this->m_cstrBBPCLWBuildDate = VS_MAKE_TIME;

#if defined(USING_CLUDE_PACK)
	this->m_pCludePack			= NULL;
#endif
}

ClCludeApp::~ClCludeApp()
{

}

//================================================================================================= E. CON/DCON
//================================================================================================= S. OPERATION

#if defined(USING_CLUDE_PACK)
bool ClCludeApp::loadCludePack( ClCludeUnPacker* pCludePack )
{
	if( !pCludePack )
		return false;

	WCHAR wcsPath[MAX_PATH];
	::GetModuleFileName( NULL, wcsPath, MAX_PATH );
	std::wstring wtsPath = ClUtil::PATH::getPathSpecW( wcsPath );
	this->m_cstrModulePath = wtsPath.c_str();

	this->m_pCludePack = pCludePack;

	const char* lpszApplicationInfo = this->m_pCludePack->getXMLFile(APPLICATION_XML_NAME);
	if( !lpszApplicationInfo )
		return false;

	TiXmlDocument xmlDoc;
	xmlDoc.Parse( lpszApplicationInfo );

	return this->parseClude( &xmlDoc );
}
#endif


bool ClCludeApp::loadCludeFile( const std::string& appInfoXml )
{
	WCHAR wcsPath[MAX_PATH];
	::GetModuleFileName( NULL, wcsPath, MAX_PATH );
	std::wstring wtsPath = ClUtil::PATH::getPathSpecW( wcsPath );
	this->m_cstrModulePath = wtsPath.c_str();

	std::string stsAppInfo;
	if( !appInfoXml.length() )
	{
		char* lpszAppPath = new char[MAX_PATH];
		strcpy_s( lpszAppPath, MAX_PATH, CT2A(this->m_cstrModulePath) );
		stsAppInfo = lpszAppPath;
		stsAppInfo += "\\";
		stsAppInfo += CLENAW_APP_XML;
		if( lpszAppPath ) delete [] lpszAppPath;
	}
	else
	{
		stsAppInfo = appInfoXml;
	}

	XMLFILE xmlFile;
	xmlFile.m_stsFileName = stsAppInfo.c_str();
	this->m_DialogXmlList.insert( PAIR_DIALOGXML(TEXT("APPLICATION"), xmlFile) );

	FILE* fpXml = NULL;
	fopen_s( &fpXml, stsAppInfo.c_str(), "rt" );
	if( !fpXml ) return false;

	long nFileSize = 0;
	fseek( fpXml, 0, SEEK_END );
	nFileSize = ftell( fpXml );
	fseek( fpXml, 0, SEEK_SET );

	if( nFileSize <= 0 )
	{
		CLD_FL( DF_ERROR, "Can not read %sl", stsAppInfo.c_str());
		return false;
	}

	char* pFileBuffer = new char[nFileSize + 1];
	if( !pFileBuffer )
	{
		CLD_FL( DF_ERROR, "Out of memory" );
		return false;
	}

	fread( pFileBuffer, sizeof(char), nFileSize, fpXml );
	pFileBuffer[nFileSize] = '\0';
	ITER_DIALOGXML iDlgXml = this->m_DialogXmlList.find( TEXT("APPLICATION") );
	iDlgXml->second.m_stsFileCont = pFileBuffer;
	if( pFileBuffer ) delete [] pFileBuffer;
	fclose( fpXml );

	TiXmlDocument xmlDoc;
	if( !xmlDoc.Parse(iDlgXml->second.m_stsFileCont.c_str()) )
	{
		CLD_FL( DF_ERROR, "Can not find appInfoPath(%s)", stsAppInfo.c_str() );
		return false;
	}

	return this->parseClude( &xmlDoc );
}


bool ClCludeApp::parseClude( TiXmlDocument* pXmlDoc )
{
	if( !pXmlDoc )
		return false;

	TiXmlElement* pRoot = pXmlDoc->RootElement();
	if( !pRoot )
	{
		CLD_FL( DF_ERROR, "Given XML file has no root element" );
		return false;
	}

	TiXmlElement* pEssential = pRoot->FirstChildElement( "essential" );
	if( !pEssential )
	{
		CLD_FL( DF_ERROR, "Can not find 'essential' element" );
		return false;
	}

	TiXmlElement* pDialogs = pRoot->FirstChildElement( "scripts" );
	if( !pDialogs )
	{
		CLD_FL( DF_ERROR, "Can not find 'scripts' element" );
		return false;
	}

	this->registerMessageID();
	if( !this->parseEssentialSetting(pEssential) )
	{
		CLD_FL( DF_ERROR, "Can not parse default setting value." );
		return false;
	}

	if( !this->parseDialogXmlFile(pDialogs) )
	{
		CLD_FL( DF_ERROR, "Can not parse dialog xml." );
		return false;
	}

	TiXmlElement* pEleMessages = pRoot->FirstChildElement( "messages" );
	this->parseMessages( pEleMessages );

	return true;
}


bool ClCludeApp::parseEssentialSetting( TiXmlElement* pEleEssential )
{
	// Parsing about Brand name (Essential)
	TiXmlElement* pEleBrand = pEleEssential->FirstChildElement( "brand" );
	if( !pEleBrand )
	{
		CLD_FL( DF_WARNING, "Can not find brand setting" );
		return false;
	}

	const char* lpszText = pEleBrand->GetText();
	if( !lpszText )
	{
		CLD_FL( DF_WARNING, "Can not find brand setting" );
		return false;
	}

	if( !ClUtil::STRING::convertUTF8ToCString(lpszText, this->m_cstrBrand) )
	{
		CLD_FL( DF_ERROR, "Can not copy string %s", lpszText );
		return false;
	}

	// Parsing about Model names (Essential)
	TiXmlElement* pEleModels = pEleEssential->FirstChildElement( "models" );
	if( !pEleModels )
	{
		CLD_FL( DF_WARNING, "Can not find models setting" );
		return false;
	}

	TiXmlElement* pEleModel = pEleModels->FirstChildElement( "model" );
	while( pEleModel )
	{
		lpszText = pEleModel->GetText();
		if( lpszText )
		{
			CString cstrModel = TEXT("");
			if( ClUtil::STRING::convertUTF8ToCString( lpszText, cstrModel) )
				this->m_modelList.push_back( cstrModel );
			else
				CLD_FL( DF_ERROR, "Can not copy string %s", lpszText );
		}

		pEleModel = pEleModel->NextSiblingElement( "model" );
	}

	if( !this->m_modelList.size() )
	{
		CLD_FL( DF_ERROR, "model name is empty." );
		return false;
	}

	// Parsing about default font information (Essential)
	TiXmlElement* pFontFace = pEleEssential->FirstChildElement( "font" );
	if( !pFontFace )
	{
		CLD_FL( DF_WARNING, "Can not find default font setting" );
		return false;
	}

	pFontFace->Attribute( "size", &this->m_nDefaultFontSize );
	if( this->m_nDefaultFontSize < 0 ) return false;

	if( !ClUtil::STRING::convertUTF8ToCString(pFontFace->GetText(), this->m_cstrDefaultFont) )
	{
		CLD_FL( DF_ERROR, "Can not copy string %s", pFontFace->GetText() );
		return false;
	}

	// Parsing about stream information (Essential)
	TiXmlElement* pEleExtension = pEleEssential->FirstChildElement( "extension" );
	if( !pEleExtension )
	{
		CLD_FL( DF_WARNING, "Can not find extension setting" );
		return false;
	}

	lpszText = pEleExtension->GetText();
	if( lpszText )
	{
		const char* pDelim	= "|\r\n, ";
		char*	lpszNewText	= NULL;
		char*	lpszNext	= NULL;
		char*	lpszToken	= NULL;

		int nLen = strlen( lpszText );
		lpszNewText = new char[nLen+1];
		strncpy_s( lpszNewText, nLen+1, lpszText, nLen );
		lpszNewText[nLen] = '\0';
		if( lpszNewText )
		{
			lpszToken = strtok_s( lpszNewText, pDelim, &lpszNext );
			while( lpszToken )
			{
				WCHAR* wtsExtension = NULL;
				if( ClUtil::STRING::convertUTF8ToUnicode(lpszToken, &wtsExtension) )
				{
					this->m_ExtensionList.push_back( wtsExtension );
					if( wtsExtension )
						delete [] wtsExtension;
				}
				lpszToken = strtok_s( NULL, pDelim, &lpszNext );
			}

			if( lpszNewText )
				delete [] lpszNewText;
		}
	}

	// Parsing log file name (Additional)
	TiXmlElement* pEleLogFileName = pEleEssential->FirstChildElement( "logfile" );
	if( pEleLogFileName )
	{
		lpszText = pEleLogFileName->GetText();
		if( !ClUtil::STRING::convertUTF8ToCString(lpszText, this->m_cstrLogFileName) )
		{
			CLD_FL( DF_ERROR, "Can not find log file name." );
		}
	}
	else
	{
		CLD_FL( DF_ERROR, "Can not find file log file name." );
	}

	// Parsing about site information (Additional)
	TiXmlElement* pEleWebSite = pEleEssential->FirstChildElement( "site" );
	if( pEleWebSite )
	{
		lpszText = pEleWebSite->GetText();
		if( !ClUtil::STRING::convertUTF8ToCString(lpszText, this->m_cstrCompanySite) )
		{
			CLD_FL( DF_ERROR, "Can not find site information. You can not open company's website." );
		}
	}
	else
	{
		CLD_FL( DF_ERROR, "Can not find site information. You can not open company's website." );
	}

	// Parsing about update site information (Additional)
	TiXmlElement* pEleUpdateSite = pEleEssential->FirstChildElement( "updatesite" );
	if( pEleUpdateSite )
	{
		lpszText = pEleUpdateSite->GetText();
		if( !ClUtil::STRING::convertUTF8ToCString(lpszText, this->m_cstrUpdateSite) )
		{
			CLD_FL( DF_ERROR, "Can not find site information. You can not open update website." );
		}
	}
	else
	{
		CLD_FL( DF_ERROR, "Can not find update site information. You can not open update website." );
	}

	// Parsing about update viewer site information (Additional)
	TiXmlElement* pEleUpdateViewer = pEleEssential->FirstChildElement( "updateviewer" );
	if( pEleUpdateViewer )
	{
		lpszText = pEleUpdateViewer->GetText();
		if( !ClUtil::STRING::convertUTF8ToCString(lpszText, this->m_cstrUpdateViewerPage) )
		{
			CLD_FL( DF_ERROR, "Can not find site information. You can not open update viewer website." );
		}
	}
	else
	{
		CLD_FL( DF_ERROR, "Can not find update site information. You can not open update viewer website." );
	}

	// Parsing about update viewer site information (Additional)
	TiXmlElement* pEleUpdateFirmware = pEleEssential->FirstChildElement( "updatefirmware" );
	if( pEleUpdateFirmware )
	{
		lpszText = pEleUpdateFirmware->GetText();
		if( !ClUtil::STRING::convertUTF8ToCString(lpszText, this->m_cstrUpdateFirmwarePage) )
		{
			CLD_FL( DF_ERROR, "Can not find site information. You can not open update firmware website." );
		}
	}
	else
	{
		CLD_FL( DF_ERROR, "Can not find update site information. You can not open update firmware website." );
	}
	
	// Parsing path information
	TiXmlElement* pElePath = pEleEssential->FirstChildElement( "path" );
	if( !pElePath )
	{
		CLD_FL( DF_ERROR, "Can not find path information." );
		return false;
	}

	// Parsing script path
	TiXmlElement* pEleScript = pElePath->FirstChildElement( "script" );
	if( !pEleScript )
	{
		CLD_FL( DF_WARNING, "Can not find script path information." );
		return false;
	}
	lpszText = pEleScript->GetText();
	if( !lpszText )
	{
		CLD_FL( DF_WARNING, "Can not find script path information." );
		return false;
	}

	CString cstrScriptPath = TEXT("");
	ClUtil::STRING::convertUTF8ToCString( lpszText, cstrScriptPath );
	ClUtil::PATH::makeValidPath( this->m_cstrModulePath + TEXT("\\") + cstrScriptPath, this->m_cstrScriptPath );
	if( !this->m_cstrScriptPath.GetLength() )
	{
		CLD_FL( DF_WARNING, "Given script path is invalid." );
		return false;
	}

	// Parsing image path
	TiXmlElement* pEleImage = pElePath->FirstChildElement( "image" );
	if( !pEleImage )
	{
		CLD_FL( DF_WARNING, "Can not find image path information." );
		return false;
	}
	lpszText = pEleImage->GetText();
	if( !lpszText )
	{
		CLD_FL( DF_WARNING, "Can not find image path information." );
		return false;
	}

	CString cstrImagePath = TEXT("");
	ClUtil::STRING::convertUTF8ToCString( lpszText, cstrImagePath );
	ClUtil::PATH::makeValidPath( this->m_cstrModulePath + TEXT("\\") + cstrImagePath, this->m_cstrImagePath );
	if( !this->m_cstrImagePath.GetLength() )
	{
		CLD_FL( DF_WARNING, "Given image path is invalid." );
		return false;
	}
	
	// Parsing map path
	TiXmlElement* pEleMap = pElePath->FirstChildElement( "map" );
	if( !pEleMap )
	{
		CLD_FL( DF_WARNING, "Can not find map path." );
		return false;
	}
	lpszText = pEleMap->GetText();
	if( !lpszText )
	{
		CLD_FL( DF_WARNING, "Can not find map path." );
		return false;
	}

	CString cstrMap = TEXT("");
	ClUtil::STRING::convertUTF8ToCString( lpszText, cstrMap );
	ClUtil::PATH::makeValidPath( this->m_cstrModulePath + TEXT("\\") + cstrMap, this->m_cstrMapPath );
	if( !this->m_cstrMapPath.GetLength() )
	{
		CLD_FL( DF_WARNING, "Given map path is invalid." );
		return false;
	}

	// Parsing Sponsor Link path
	TiXmlElement* pEleSponsorLink = pElePath->FirstChildElement( "sponsor" );
	if( !pEleSponsorLink )
	{
		CLD_FL( DF_WARNING, "Can not find Sponsor link path." );
//		return false;
	}
	else
	{
		lpszText = pEleSponsorLink->GetText();
		if( !lpszText )
		{
			CLD_FL( DF_WARNING, "Can not find sponsor link path." );
//			return false;
		}

		CString cstrSponsorLink = TEXT("");
		ClUtil::STRING::convertUTF8ToCString( lpszText, cstrSponsorLink );
		if(cstrSponsorLink.Find(TEXT("http")) < 0 && cstrSponsorLink.Find(TEXT("HTTP"))  < 0)
			ClUtil::PATH::makeValidPath( this->m_cstrModulePath + TEXT("\\") + cstrSponsorLink, this->m_cstrSponsorLinkPath);
		else
			this->m_cstrSponsorLinkPath = cstrSponsorLink;
			
		if( !this->m_cstrSponsorLinkPath.GetLength() )
		{
			CLD_FL( DF_WARNING, "Given sponsor link path is invalid." );
//			return false;
		}
	}
	
	// Parsing config path
	TiXmlElement* pEleConfig = pElePath->FirstChildElement( "config" );
	if( !pEleConfig )
	{
		CLD_FL( DF_WARNING, "Can not config path." );
		return false;
	}
	lpszText = pEleConfig->GetText();
	if( !lpszText )
	{
		CLD_FL( DF_WARNING, "Can not config path." );
		return false;
	}

	ClUtil::STRING::convertUTF8ToCString( lpszText, this->m_cstrConfigPath );
	if( !this->m_cstrConfigPath.GetLength() )
	{
		CLD_FL( DF_WARNING, "Given config path is invalid." );
		return false;
	}

	// Parsing time file path
	TiXmlElement* pEleTimePath = pElePath->FirstChildElement( "time" );
	if( !pEleTimePath )
	{
		CLD_FL( DF_WARNING, "Can not time path." );
		return false;
	}
	lpszText = pEleTimePath->GetText();
	if( !lpszText )
	{
		CLD_FL( DF_WARNING, "Can not time path." );
		return false;
	}

	ClUtil::STRING::convertUTF8ToCString( lpszText, this->m_cstrTimeFilePath );
	if( !this->m_cstrTimeFilePath.GetLength() )
	{
		CLD_FL( DF_WARNING, "Given config path is invalid." );
		return false;
	}

	// Parsing sd-card information
	TiXmlElement* pEleSDCard = pEleEssential->FirstChildElement( "sdcard" );
	if( !pEleSDCard )
	{
		CLD_FL( DF_ERROR, "Can not find path information." );
		return false;
	}

	TiXmlElement* pEleSDDir = pEleSDCard->FirstChildElement( "dir" );
	while( pEleSDDir )
	{
		SDCARD_DIR	eDir	= DIR_INVALID;
		WCHAR*	lpwzDir		= NULL;

		const char* lpszText = pEleSDDir->Attribute( "id" );
		if( lpszText )
		{
			switch( lpszText[0] )
			{
			case 'e':	if( !strcmp(lpszText,"event") )		eDir = DIR_EVENT;	break;
			case 'm':	if( !strcmp(lpszText,"manual") )	eDir = DIR_MANUAL;
						else if( !strcmp(lpszText,"motion"))eDir = DIR_MOTION;	break;
			case 'n':	if( !strcmp(lpszText,"normal"))		eDir = DIR_NORMAL;	break;
			}
		}

		lpszText = pEleSDDir->GetText();
		if( lpszText )
		{
			if( !ClUtil::STRING::convertUTF8ToUnicode(lpszText, &lpwzDir) )
			{
				CLD_FL( DF_ERROR, "Can not convert to unicode(%s)", lpszText );
				if( lpwzDir )
				{
					delete [] lpwzDir;
					lpwzDir = NULL;
				}
			}
		}

		this->addSDDirInfo( eDir, lpwzDir );

		if( lpwzDir )		delete [] lpwzDir;

		pEleSDDir = pEleSDDir->NextSiblingElement( "dir" );
	}

	return true;
}


bool ClCludeApp::parseDialogXmlFile( TiXmlElement* pEleXml )
{
	TiXmlElement* pDialog = pEleXml->FirstChildElement( "script" );

#if !defined(USING_CLUDE_PACK)
	char* lpszScriptPath = new char[MAX_PATH];
#endif

	while( pDialog )
	{
		TiXmlAttribute* pTarget	= NULL;
		const char* lpszTarget	= NULL;
		HashedString hashedTarget;

		pTarget = pDialog->FirstAttribute();
		if( !pTarget || strcmp(pTarget->Name(),"target") ) continue;

		lpszTarget	= pTarget->Value();
		
		ClUtil::STRING::convertUTF8ToHashedString( lpszTarget, hashedTarget );
		const char* lpszPath = pDialog->GetText();

#if defined(USING_CLUDE_PACK)

		const char* lpszXMLContent = this->m_pCludePack->getXMLFile( lpszPath );
		if( lpszXMLContent )
		{
			XMLFILE xmlFile;
			xmlFile.m_stsFileName = lpszPath;
			xmlFile.m_stsFileCont = lpszXMLContent;
			this->m_DialogXmlList.insert( PAIR_DIALOGXML(hashedTarget, xmlFile) );
		}
#else
		CString cstrPath;
		ClUtil::STRING::convertUTF8ToCString( lpszPath, cstrPath );
		CString cstrBasePath = this->m_cstrScriptPath + TEXT("\\") + cstrPath;
		ClUtil::PATH::makeValidPath( cstrBasePath, cstrPath );

		memset( lpszScriptPath, 0, MAX_PATH );
		strcpy_s( lpszScriptPath, MAX_PATH, CT2A(cstrBasePath) );

		XMLFILE xmlFile;
		xmlFile.m_stsFileName = lpszScriptPath;
		ClUtil::STRING::lowerString(lpszScriptPath);
		this->m_DialogXmlList.insert( PAIR_DIALOGXML(hashedTarget, xmlFile) );

		FILE* fpXml = NULL;
		fopen_s( &fpXml, lpszScriptPath, "rt" );
		if( fpXml )
		{
			long nFileSize = 0;
			fseek( fpXml, 0, SEEK_END );
			nFileSize = ftell( fpXml );
			fseek( fpXml, 0, SEEK_SET );
			if( nFileSize > 0 )
			{
				char* pFileBuffer = new char[nFileSize + 1];
				if( pFileBuffer )
				{
					fread( pFileBuffer, sizeof(char), nFileSize, fpXml );
					pFileBuffer[nFileSize] = '\0';
					ITER_DIALOGXML iDlgXml = this->m_DialogXmlList.find( hashedTarget );
					iDlgXml->second.m_stsFileCont = pFileBuffer;
					if( pFileBuffer ) delete [] pFileBuffer;
					CLD( DF_REPORT, "Open xml file %s succeeded", lpszScriptPath );
				}
				else
				{
					CLD_FL( DF_ERROR, "Out of memory while read %s", lpszScriptPath );
				}
			}

			fclose( fpXml );
		}
		else
		{
			CLD_FL( DF_ERROR, "Can not open %s", lpszScriptPath );
		}
#endif
		pDialog = pDialog->NextSiblingElement( "script" );
	}

#if !defined(USING_CLUDE_PACK)
	if( lpszScriptPath ) delete [] lpszScriptPath;
#endif

	return true;
}


bool ClCludeApp::parseMessages( TiXmlElement* pEleMessages )
{
	if( !pEleMessages )	return false;

	TiXmlElement* pEleMessage = pEleMessages->FirstChildElement( "message" );
	while( pEleMessage )
	{
		const char* lpszID		= NULL;
		const char* lpszTitle	= NULL;
		const char* lpszComment	= NULL;
		HashedString hashedID;

		lpszID = pEleMessage->Attribute( "id" );
		if( lpszID ) ClUtil::STRING::convertUTF8ToHashedString( lpszID, hashedID );

		if( !hashedID.isNULL() )
		{
			TiXmlElement* pEleTitle = pEleMessage->FirstChildElement( "title" );
			if( pEleTitle ) lpszTitle = pEleTitle->GetText();

			TiXmlElement* pEleComment = pEleMessage->FirstChildElement( "comment" );
			if( pEleComment ) lpszComment = pEleComment->GetText();

			this->addMessage( hashedID, lpszTitle, lpszComment );
		}

		pEleMessage = pEleMessage->NextSiblingElement( "message" );
	}

	return true;
}

//================================================================================================= E. OPERATION
//================================================================================================= S. CONTROL

void ClCludeApp::registerMessageID( void )
{
	this->addMessage( HashedString(NOTIFY_ERR_NO_SRCFILTER),	NULL, NULL );
	this->addMessage( HashedString(NOTIFY_ERR_NO_VIDFILTER),	NULL, NULL );
	this->addMessage( HashedString(NOTIFY_ERR_REMOVE_DISK),		NULL, NULL );
	this->addMessage( HashedString(NOTIFY_ERR_INVALID_STREAM),	NULL, NULL );
	this->addMessage( HashedString(NOTIFY_ERR_NO_INTERNET),		NULL, NULL );
	this->addMessage( HashedString(NOTIFY_ERR_NO_SELECTFILE),	NULL, NULL );
	this->addMessage( HashedString(NOTIFY_ERR_COPY_FILE),		NULL, NULL );
	this->addMessage( HashedString(NOTIFY_QUES_DEDICATE_DISK),	NULL, NULL );
	this->addMessage( HashedString(NOTIFY_QUES_WANT_QUIT),		NULL, NULL );
	this->addMessage( HashedString(NOTIFY_QUES_WANT_SETTIME),	NULL, NULL );

	return;
}

//================================================================================================= E. CONTROL
//================================================================================================= S. GET/SET

const CString& ClCludeApp::getModulePath( void )
{
	return this->m_cstrModulePath;
}


const CString& ClCludeApp::getImageBasePath( void )
{
	return this->m_cstrImagePath;
}


const CString& ClCludeApp::getScriptBasePath( void )
{
	return this->m_cstrScriptPath;
}


const CString& ClCludeApp::getMapPath( void )
{
	return this->m_cstrMapPath;
}

const CString& ClCludeApp::getSponsorLinkPath(void)
{
	return this->m_cstrSponsorLinkPath;
}

const CString& ClCludeApp::getConfigPath( void )
{
	return this->m_cstrConfigPath;
}


const CString& ClCludeApp::getTimeFilePath( void )
{
	return this->m_cstrTimeFilePath;
}


const CString& ClCludeApp::getDefaultFont( void )
{
	return this->m_cstrDefaultFont;
}


int ClCludeApp::getDefaultFontSize( void )
{
	return this->m_nDefaultFontSize;
}


bool ClCludeApp::isEmptyXmlFileList( void )
{
	return this->m_DialogXmlList.empty();
}


int ClCludeApp::getXmlFileListSize( void )
{
	return this->m_DialogXmlList.size();
}


const char* ClCludeApp::getXml( int nIndex )
{
	if( nIndex < 0 )
		return NULL;

	ITER_DIALOGXML iXml = this->m_DialogXmlList.begin();
	std::advance( iXml, nIndex );
	if( iXml == this->m_DialogXmlList.end() )
		return NULL;

	return iXml->second.m_stsFileCont.c_str();
}


const char* ClCludeApp::getXml( const HashedString& dialogID )
{
	ITER_DIALOGXML iXML = this->m_DialogXmlList.find( dialogID );
	if( iXML == this->m_DialogXmlList.end() )
		return NULL;

	return iXML->second.m_stsFileCont.c_str();
}


bool ClCludeApp::getXmlFileInfo( int nIndex, HashedString& dialogID, std::string& xmlFileName )
{
	if( nIndex < 0 )
		return false;

	ITER_DIALOGXML iXml = this->m_DialogXmlList.begin();
	std::advance( iXml, nIndex );
	if( iXml == this->m_DialogXmlList.end() )
		return false;

	dialogID = iXml->first;
	xmlFileName = iXml->second.m_stsFileName;

	return true;
}


bool ClCludeApp::isEmptyMessageList( void )
{
	return this->m_MessageList.empty();
}


int ClCludeApp::getMessageListSize( void )
{
	return this->m_MessageList.size();
}


bool ClCludeApp::addMessage( const HashedString& messageID, const char* lpszTitle, const char* lpszComment )
{
	WCHAR* lpwzTitle	= NULL;
	WCHAR* lpwzComment	= NULL;

	if( lpszTitle )
	{
		if( !ClUtil::STRING::convertUTF8ToUnicode(lpszTitle, &lpwzTitle) )
		{
			CLD_FL( DF_ERROR, "Message(%s) can not convert to UNICODE.", lpszTitle );
			return false;
		}
	}

	if( lpszComment )
	{
		if( !ClUtil::STRING::convertUTF8ToUnicode(lpszComment, &lpwzComment) )
		{
			CLD_FL( DF_ERROR, "Message(%s) can not convert to UNICODE.", lpszComment );
			return false;
		}
	}

	ITER_MESSAGE iMessage = this->m_MessageList.find( messageID );
	if( iMessage == this->m_MessageList.end() )
	{
		NOTIFYMESSAGE message;
		if( lpwzTitle )		message.m_wtsTitle	= lpwzTitle;
		if( lpwzComment )	message.m_wtsComment= lpwzComment;

		this->m_MessageList.insert( PAIR_MESSAGE(messageID,message) );
	}
	else
	{
		if( lpwzTitle )		iMessage->second.m_wtsTitle = lpwzTitle;
		if( lpwzComment )	iMessage->second.m_wtsComment = lpwzComment;
	}

	if( lpwzTitle ) delete [] lpwzTitle;
	if( lpwzComment ) delete [] lpwzComment;

	return true;
}


const ClCludeApp::PNOTIFYMESSAGE ClCludeApp::getMessage( int nIndex )
{
	ITER_MESSAGE iMessage = this->m_MessageList.begin();
	std::advance( iMessage, nIndex );
	if( iMessage == this->m_MessageList.end() )
		return NULL;

	return &iMessage->second;
}


const ClCludeApp::PNOTIFYMESSAGE ClCludeApp::getMessage( const HashedString& messageID )
{
	ITER_MESSAGE iMessage = this->m_MessageList.find( messageID );
	if( iMessage == this->m_MessageList.end() )
		return NULL;

	return &iMessage->second;
}


const CString& ClCludeApp::getCompanySite( VOID )
{
	return this->m_cstrCompanySite;
}


const CString& ClCludeApp::getUpdateSite( void )
{
	return this->m_cstrUpdateSite;
}

const CString& ClCludeApp::getUpdateViewerSite( void )
{
	return this->m_cstrUpdateViewerPage;
}

const CString& ClCludeApp::getUpdateFirmwareSite( void )
{
	return this->m_cstrUpdateFirmwarePage;
}

const CString& ClCludeApp::getBBPCLWVersion( void )
{
	return this->m_cstrBBPCLWVersion;
}


const CString& ClCludeApp::getBBPCLWBuildDate( void )
{
	return this->m_cstrBBPCLWBuildDate;
}


BOOL ClCludeApp::isEmptySDDirInfoList( VOID )
{
	return this->m_SDDirectoryList.empty();
}


INT ClCludeApp::getSDDirInfoListSize( VOID )
{
	return this->m_SDDirectoryList.size();
}


const std::wstring ClCludeApp::getSDDirInfo( INT nIndex )
{
	if( nIndex < 0 )
		return NULL;

	ITER_SDDIRINFO iDir = this->m_SDDirectoryList.begin();
	std::advance( iDir, nIndex );
	if( iDir == this->m_SDDirectoryList.end() )
		return NULL;

	return iDir->second;
}


const std::wstring ClCludeApp::getSDDirInfo( ClCludeApp::SDCARD_DIR idSDDir )
{
	ITER_SDDIRINFO iDir = this->m_SDDirectoryList.find( (INT)idSDDir );
	if( iDir == this->m_SDDirectoryList.end() )
		return TEXT("");

	return iDir->second;
}


VOID ClCludeApp::addSDDirInfo( SDCARD_DIR dirType, WCHAR* lpwzDir )
{
	if( dirType == DIR_INVALID )
		return;

	ITER_SDDIRINFO iDir = this->m_SDDirectoryList.find( (INT)dirType );
	if( iDir == this->m_SDDirectoryList.end() )
	{
		this->m_SDDirectoryList.insert( PAIR_SDDIRINFO((INT)dirType, lpwzDir) );
	}
	else
	{
		iDir->second = lpwzDir;
		CLD_FL( DF_WARNING, "Duplicated application::essential::sdcard::dir::id found. Anyway overwritten it." );
	}

	return;
}


BOOL ClCludeApp::isEmptyModelList( VOID )
{
	return this->m_modelList.empty();
}


INT ClCludeApp::getModelListSize( VOID )
{
	return this->m_modelList.size();
}


const CString& ClCludeApp::getModelName( INT nIndex )
{
	ITER_MODEL iModel = this->m_modelList.begin();
	std::advance( iModel, nIndex );

	return (*iModel);
}


const CString& ClCludeApp::getBrandName( VOID )
{
	return this->m_cstrBrand;
}


BOOL ClCludeApp::isEmptyExtensionList( VOID )
{
	return this->m_ExtensionList.empty();
}


INT ClCludeApp::getExtensionListSize( VOID )
{
	return this->m_ExtensionList.size();
}


BOOL ClCludeApp::isExistExtension( const CString& cstrExtension )
{
	if( !cstrExtension.GetLength() )
		return FALSE;

	ITER_EXTENSION iExt = this->m_ExtensionList.begin();
	ITER_EXTENSION iExtEnd = this->m_ExtensionList.end();
	for( ; iExt != iExtEnd; iExt ++ )
	{
		if( !cstrExtension.Compare(iExt->c_str()) )
			return TRUE;
	}

	return FALSE;
}

BOOL ClCludeApp::isExistLogFile( const CString& cstrFileName )
{
	if( !cstrFileName.GetLength() )
		return FALSE;

	if( !this->m_cstrLogFileName.Compare(cstrFileName) )
		return TRUE;

	return FALSE;
}

//================================================================================================= E. GET/SET
//================================================================================================= EOF

