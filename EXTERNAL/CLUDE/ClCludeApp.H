//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClCludeApp.H
//
//	Description: CLENA CLUDE parser for Application information
//
//	Update History
// 
//		2012.01.03 [Allen Han] Adding CLUDE packager specification
//		2011.10.10 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. FILEID

#pragma once

//================================================================================================= E. FILEID
//================================================================================================= S. INCLUDE

#include <TINYXML/tinyxml.h>
#include <HELPER/ClHashedString.H>

#if defined(USING_CLUDE_PACK)
#	include <CLUDE/ClCludeUnpacker.H>
#endif
#include <CLUDE/version.h>
#include <map>
#include <list>

//================================================================================================= E. INCLUDE
//================================================================================================= S. PREPROCESSOR

#define	CLENAW_APP_XML									APPLICATION_XML_NAME //"application.xml"

#define NOTIFY_ERR_NO_SRCFILTER							TEXT("error-no-srcfilter")
#define NOTIFY_ERR_NO_VIDFILTER							TEXT("error-no-vidfilter")
#define NOTIFY_ERR_REMOVE_DISK							TEXT("error-remove-disk")
#define NOTIFY_ERR_INVALID_STREAM						TEXT("error-invalid-stream")
#define NOTIFY_ERR_NO_INTERNET							TEXT("error-no-internet")
#define NOTIFY_ERR_NO_SELECTFILE						TEXT("error-no-selectfile")
#define NOTIFY_ERR_COPY_FILE							TEXT("error-copy-file")
#define NOTIFY_QUES_DEDICATE_DISK						TEXT("question-dedicate-disk")
#define NOTIFY_QUES_WANT_QUIT							TEXT("question-want-quit")
#define NOTIFY_QUES_WANT_SETTIME						TEXT("question-want-settime")

//================================================================================================= E. PREPROCESSOR
//================================================================================================= S. CLASS

class ClCludeApp
{
public:
	ClCludeApp();
	~ClCludeApp();

	//-------------------------------------------------------------------------------------- S.TYPEDEF

	enum SDCARD_DIR
	{
		DIR_NORMAL	= 0	,
		DIR_EVENT		,
		DIR_MANUAL		,
		DIR_MOTION		,
		DIR_INVALID
	};

	typedef std::map<INT,std::wstring>							SDDIRINFO_POOL;
	typedef SDDIRINFO_POOL::iterator							ITER_SDDIRINFO;
	typedef std::pair<INT,std::wstring>							PAIR_SDDIRINFO;

	typedef struct _tagXMLFILE
	{
		std::string		m_stsFileName;
		std::string		m_stsFileCont;

	} XMLFILE, *PXMLFILE;

	typedef std::map<HashedString,XMLFILE>						DIALOGXML_POOL;
	typedef DIALOGXML_POOL::iterator							ITER_DIALOGXML;
	typedef std::pair<HashedString,XMLFILE>						PAIR_DIALOGXML;

	typedef struct _tagNOTIFYMESSAGE
	{
		std::wstring	m_wtsTitle;
		std::wstring	m_wtsComment;

	} NOTIFYMESSAGE, *PNOTIFYMESSAGE;

	typedef std::map<HashedString,NOTIFYMESSAGE>				MESSAGE_POOL;
	typedef MESSAGE_POOL::iterator								ITER_MESSAGE;
	typedef std::pair<HashedString,NOTIFYMESSAGE>				PAIR_MESSAGE;

	typedef std::list<CString>									MODEL_POOL;
	typedef MODEL_POOL::iterator								ITER_MODEL;

	typedef std::list<std::wstring>								EXTENTION_POOL;
	typedef EXTENTION_POOL::iterator							ITER_EXTENSION;

	//-------------------------------------------------------------------------------------- E.TYPEDEF

public:

			bool					loadCludeFile		( const std::string& appInfoXml );

#if defined(USING_CLUDE_PACK)
			bool					loadCludePack		( ClCludeUnPacker* pCludePack );
#endif

private:
			bool					parseClude			( TiXmlDocument* pXmlDoc );
			bool					parseEssentialSetting( TiXmlElement* pEleSetting );
			bool					parseDialogXmlFile	( TiXmlElement* pEleXml );
			bool					parseMessages		( TiXmlElement* pEleMessages );

			void					registerMessageID	( void );


public:
			const CString&			getModulePath		( void );
			const CString&			getImageBasePath	( void );
			const CString&			getScriptBasePath	( void );
			const CString&			getMapPath			( void );
			const CString& 			getSponsorLinkPath	(void);
			const CString&			getConfigPath		( void );
			const CString&			getTimeFilePath		( void );
			const CString&			getCompanySite		( void );
			const CString&			getUpdateSite		( void );
			const CString&			getUpdateViewerSite		( void );
			const CString&			getUpdateFirmwareSite	( void );
			const CString&			getBBPCLWVersion	( void );
			const CString&			getBBPCLWBuildDate	( void );

			const CString&			getDefaultFont		( void );
			int						getDefaultFontSize	( void );

			bool					isEmptyXmlFileList	( void );
			int						getXmlFileListSize	( void );
			const char*				getXml				( int nIndex );
			const char*				getXml				( const HashedString& dialogID );
			bool					getXmlFileInfo		( int nIndex, HashedString& dialogID, std::string& xmlFileName );

			bool					isEmptyMessageList	( void );
			int						getMessageListSize	( void );
			bool					addMessage			( const HashedString& messageID, const char* lpszTitle, const char* lpszComment );
			const PNOTIFYMESSAGE	getMessage			( int nIndex );
			const PNOTIFYMESSAGE	getMessage			( const HashedString& messageID );

			BOOL					isEmptySDDirInfoList( VOID );
			INT						getSDDirInfoListSize( VOID );
			const std::wstring		getSDDirInfo		( INT nIndex );
			const std::wstring		getSDDirInfo		( SDCARD_DIR idSDDir );
			VOID					addSDDirInfo		( SDCARD_DIR dirType, WCHAR* lpszDir );

			BOOL					isEmptyModelList	( VOID );
			INT						getModelListSize	( VOID );
			const CString&			getModelName		( INT nIndex );
			const CString&			getBrandName		( VOID );

			BOOL					isEmptyExtensionList( VOID );
			INT						getExtensionListSize( VOID );
			BOOL					isExistExtension( const CString& cstrExtension );
			BOOL 					isExistLogFile( const CString& cstrFileName );

private:

#if defined(USING_CLUDE_PACK)
			ClCludeUnPacker*		m_pCludePack;
#endif

			CString					m_cstrBrand;
			MODEL_POOL				m_modelList;

			CString					m_cstrModulePath;
			CString					m_cstrImagePath;
			CString					m_cstrScriptPath;
			CString					m_cstrMapPath;
			CString					m_cstrSponsorLinkPath;
			CString					m_cstrConfigPath;
			CString					m_cstrTimeFilePath;

			CString					m_cstrBBPCLWVersion;
			CString					m_cstrBBPCLWBuildDate;

			CString					m_cstrDefaultFont;
			int						m_nDefaultFontSize;

			CString					m_cstrCompanySite;
			CString					m_cstrUpdateSite;
			CString					m_cstrUpdateViewerPage;
			CString					m_cstrUpdateFirmwarePage;
			CString					m_cstrLogFileName;
			HashedString			m_iconPath;

			DIALOGXML_POOL			m_DialogXmlList;
			MESSAGE_POOL			m_MessageList;
			SDDIRINFO_POOL			m_SDDirectoryList;

			EXTENTION_POOL			m_ExtensionList;
};

//================================================================================================= E. CLASS


