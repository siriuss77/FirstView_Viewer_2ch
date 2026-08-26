//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//  Module: wbcConfig.cpp
//
//  Description: Viewer config class
//
//  Update History
// 
//      2014.10.27 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. INCLUDE

#include "stdafx.h"
#include "wbcConfig.h"

//================================================================================================= E. INCLUDE

CWBCConfigure*  CWBCConfigure::m_pViewerConfig = NULL;
ClMutex         CWBCConfigure::m_ViewerConfigCS;

//================================================================================================= S. CON/DCON

CWBCConfigure::CWBCConfigure()
{
    this->m_curLanguageID           = LANGUAGE_ENGLISH;
    this->m_dwFlag                  = 0;
}


CWBCConfigure::~CWBCConfigure()
{

}

//================================================================================================= E. CON/DCON
//================================================================================================= S. INIT/DEINIT

bool CWBCConfigure::init( const CString& cstrCFGName )
{
    WCHAR wcsPath[MAX_PATH];
    long nFileSize = 0;

    ::GetModuleFileName( NULL, wcsPath, MAX_PATH );
    CString cstrPath = wcsPath;
    int nPos = cstrPath.ReverseFind( TEXT('\\') );
    cstrPath = cstrPath.Left( nPos + 1 );
    cstrPath += cstrCFGName;

    //   PAIR_LID(HashedLID(language name in XML, language sub mode), language enumeration)
    this->m_LIDMap.insert( PAIR_LID(HashedLID("english",    0), LANGUAGE_ENGLISH    ) );
    this->m_LIDMap.insert( PAIR_LID(HashedLID("korean",     0), LANGUAGE_KOREAN     ) );
    this->m_LIDMap.insert( PAIR_LID(HashedLID("japanese",   0), LANGUAGE_JAPANESE   ) );
    this->m_LIDMap.insert( PAIR_LID(HashedLID("chinese",    0), LANGUAGE_CHINESE_S  ) );
    this->m_LIDMap.insert( PAIR_LID(HashedLID("chinese",    1), LANGUAGE_CHINESE_T  ) );
    this->m_LIDMap.insert( PAIR_LID(HashedLID("russian",    0), LANGUAGE_RUSSIAN    ) );
    // New language add here...

    USES_CONVERSION;
    std::string stsPath = T2A(cstrPath);

    return this->_loadConfig( stsPath );
}


void CWBCConfigure::deInit( void )
{
    this->m_stringMap.clear();

    return;
}


bool CWBCConfigure::_loadConfig( const std::string& stsFilePath )
{
    TiXmlDocument doc;
    TiXmlElement* pRoot;
    TiXmlElement* pEleConfigs;
    TiXmlElement* pEleLStrings;
    bool bReturn = false;

    if( !doc.LoadFile(stsFilePath.c_str()) )
    {
        printf( "%s(line:%d) Can not configuration file\n" );
        goto PARSE_FAILED;
    }

    pRoot = doc.RootElement();
    if( pRoot == NULL )
    {
        printf( "%s(line:%d) xml document has no root element\n" );
        goto PARSE_FAILED;
    }

    pEleConfigs = pRoot->FirstChildElement( "configs" );
    if( pEleConfigs == NULL )
    {
        printf( "%s(line:%d) xml document has no configs element\n" );
        goto PARSE_FAILED;
    }

    pEleLStrings = pRoot->FirstChildElement( "lstrings" );
    if( pEleLStrings == NULL )
    {
        printf( "%s(line:%d) xml document has no lstrings element\n" );
        goto PARSE_FAILED;
    }

    if( ! this->__parseConfigs(pEleConfigs) )
    {
        printf( "%s(line:%d) _parseConfig failed\n" );
        goto PARSE_FAILED;
    }

    if( ! this->__parseLStrings(pEleLStrings) )
    {
        printf( "%s(line:%d) _parseLString failed\n" );
        goto PARSE_FAILED;
    }

    bReturn = true;

PARSE_FAILED:

    return bReturn;
}


bool CWBCConfigure::__parseConfigs( TiXmlElement* pEleConfigs )
{
    const char* lpszString = NULL;

    // parse "language"
    TiXmlElement* pEleLanguage = pEleConfigs->FirstChildElement( "language" );
    if( pEleLanguage != NULL )
    {
        int mode = 0;
        ELANGUAGES_ID eLID;

        lpszString = pEleLanguage->GetText();
        pEleLanguage->Attribute( "mode", &mode );
        eLID = this->__getLID( lpszString, mode );
        if( eLID != LANGUAGE_INVALID )
            this->m_curLanguageID = eLID;
    }

    // parse "dirs"
    TiXmlElement* pEleDirs = pEleConfigs->FirstChildElement( "dirs" );
    TiXmlElement* pEleDir = pEleDirs->FirstChildElement( "dir" );
    while( pEleDir )
    {
        int nNXFSPercent;
        const char* lpszDirName = NULL;
        lpszString = pEleDir->Attribute( "type" );
        lpszDirName = pEleDir->GetText();
        pEleDir->Attribute( "nxfsp", &nNXFSPercent );
        this->__addDirInfo( lpszDirName, lpszString, nNXFSPercent );

        pEleDir = pEleDir->NextSiblingElement( "dir" );
    }

    TiXmlElement* pEleExtensions = pEleConfigs->FirstChildElement( "extensions" );
    TiXmlElement* pEleExtension = pEleExtensions->FirstChildElement( "extension" );
    while( pEleExtension )
    {
        lpszString = pEleExtension->GetText();
        this->__addExtension( lpszString );

        pEleExtension = pEleExtension->NextSiblingElement( "extension" );
    }

    TiXmlElement* pEleRecursive = pEleConfigs->FirstChildElement( "recursive" );
    if( pEleRecursive )
    {
        bool bRemovalRecursiveScan = false;
        bool bDesktopRecursiveScan = false;

        pEleRecursive->QueryBoolAttribute( "removal", &bRemovalRecursiveScan );
        pEleRecursive->QueryBoolAttribute( "desktop", &bDesktopRecursiveScan );

        if( bRemovalRecursiveScan ) this->m_dwFlag |= FLAG_REMOVAL_RECVSCAN;
        if( bDesktopRecursiveScan ) this->m_dwFlag |= FLAG_DESKTOP_RECVSCAN;
    }

    TiXmlElement* pEleCapture = pEleConfigs->FirstChildElement( "capture" );
    if( pEleCapture )
    {
        bool bCaptureUseFileTime = false;
        bool bCaptureUseAddPos = false;

        pEleCapture->QueryBoolAttribute( "filetime", &bCaptureUseFileTime );
        pEleCapture->QueryBoolAttribute( "addpos", &bCaptureUseAddPos );

        if( bCaptureUseFileTime ) this->m_dwFlag |= FLAG_CAPTURE_FILETIME;
        if( bCaptureUseAddPos ) this->m_dwFlag |= FLAG_CAPTURE_ADDPOS;
    }

    return true;
}


bool CWBCConfigure::__parseLStrings( TiXmlElement* pEleLStrings )
{
    TiXmlElement* pEleLString;
    const char* lpszLSID = NULL;

    pEleLString = pEleLStrings->FirstChildElement( "lstring" );
    while( pEleLString )
    {
        lpszLSID = pEleLString->Attribute( "id" );
        if( lpszLSID != NULL )
        {
            const char* lpszLanguageType = NULL;
            const char* pUTF8String = NULL;
            int mode = 0;

            TiXmlElement* pEleLanguage = pEleLString->FirstChildElement( "language" );
            while( pEleLanguage )
            {
                lpszLanguageType = pEleLanguage->Attribute( "type" );
                pEleLanguage->Attribute( "mode", &mode );
                pUTF8String = pEleLanguage->GetText();

                if( (lpszLanguageType != NULL) && (pUTF8String != NULL) )
                    this->_addLanguageString( lpszLSID, lpszLanguageType, mode, pUTF8String );

                pEleLanguage = pEleLanguage->NextSiblingElement( "language" );
            }
        }

        pEleLString = pEleLString->NextSiblingElement();
    }

    return true;
}

//================================================================================================= E. INIT/DEINIT
//================================================================================================= S. OPERATION

bool CWBCConfigure::__addDirInfo( const char* lpszDirName, const char* lpszDirType, int nNXFSPercent )
{
    if( (lpszDirName == NULL) || (lpszDirType == NULL) )
        return false;

    char strDirType[16];
    strcpy_s( strDirType, 16, lpszDirType );

    _strupr_s( strDirType, 16 );

    SDIR_ITEM dirItem;
    dirItem.m_eDirType = DIR_TYPE_INVALID;

    switch( strDirType[0] )
    {
    case 'N':
        {
            if( ! strcmp(strDirType, "NORMAL") )
                dirItem.m_eDirType = DIR_TYPE_NORMAL;

        } break;

    case 'E':
        {
            if( ! strcmp(strDirType, "EVENT") )
                dirItem.m_eDirType = DIR_TYPE_EVENT;

        } break;

    case 'M':
        {
            if( ! strcmp(strDirType, "MOTION") )
                dirItem.m_eDirType = DIR_TYPE_MOTION;

            else if( ! strcmp(strDirType, "MANUAL") )
                dirItem.m_eDirType = DIR_TYPE_MANUAL;

        } break;

    case 'I':
        {
            if( ! strcmp(strDirType, "IMAGE") )
                dirItem.m_eDirType = DIR_TYPE_IMAGE;

        } break;

    default:;
    }

    if( dirItem.m_eDirType == DIR_TYPE_INVALID )
        return false;

    ITER_DIRITEM iDir = this->m_dirItemList.begin();
    ITER_DIRITEM iEndDir = this->m_dirItemList.end();
    for( ; iDir != iEndDir; iDir ++ )
    {
        if( ! iDir->m_stsDirName.compare(lpszDirName) )
            return false;

        if( iDir->m_eDirType == dirItem.m_eDirType )
            return false;
    }

    this->__convertUTF8ToCString( lpszDirName, dirItem.m_cstrDirName );
    dirItem.m_stsDirName = lpszDirName;

    if( (nNXFSPercent < 0) || (nNXFSPercent >= 100) )
        dirItem.m_nNxfsPercent = 0;
    else
        dirItem.m_nNxfsPercent = nNXFSPercent;


    this->m_dirItemList.push_back( dirItem );

    return true;
}


bool CWBCConfigure::__addExtension( const char* lpszExtension )
{
    if( lpszExtension == NULL )
        return false;

    CString cstrExtension;
    this->__convertUTF8ToCString( lpszExtension, cstrExtension );

    ITER_EXTENSION iExt = this->m_extensionList.begin();
    ITER_EXTENSION iEndExt = this->m_extensionList.end();
    for( ; iExt != iEndExt; iExt ++ )
    {
        if( ! iExt->Compare(cstrExtension) )
            return false;
    }

    this->m_extensionList.push_back( cstrExtension );

    return true;
}


void CWBCConfigure::_addLanguageString( const char* pLSID, const char* lpszLanguageType, int mode, const char* pUTF8String )
{
    ELANGUAGES_ID eLID = this->__getLID( lpszLanguageType, mode );
    HashedLID hLID = HashedLID( pLSID,(unsigned int)eLID );
    CString cstrString;

    char* pNewText = NULL;
    this->__replaceEscape( pUTF8String, &pNewText );
    this->__convertUTF8ToCString( pNewText, cstrString );

    if( pNewText != NULL )
        delete [] pNewText;

    ITER_LSTRING iLString = this->m_stringMap.find( hLID );
    if( iLString != this->m_stringMap.end() )
    {
        wprintf( TEXT("Duplicated LSID(%s) found, ignore it.\n"), iLString->second.GetBuffer() );
        return;
    }

    this->m_stringMap.insert( PAIR_LSTRING(hLID,cstrString) );

    return;
}


CWBCConfigure::ELANGUAGES_ID CWBCConfigure::__getLID( const char* pLanguage, int mode )
{
    ITER_LID iLID = this->m_LIDMap.find( HashedLID(pLanguage, mode) );
    if( iLID == this->m_LIDMap.end() )
        return LANGUAGE_INVALID;

    return iLID->second;
}


bool CWBCConfigure::__convertUTF8ToCString( const char* pUTF8String, CString& cstrTarget )
{
    if( pUTF8String == NULL )
        return false;

    int nLen = MultiByteToWideChar( CP_UTF8, 0, pUTF8String, -1, 0, 0 );
    WCHAR* lpwzString = new WCHAR[nLen+1];
    MultiByteToWideChar( CP_UTF8, 0, pUTF8String, -1, lpwzString, nLen );

    cstrTarget.Format( TEXT("%s"), lpwzString );
    if( lpwzString )
        delete [] lpwzString;

    return true;
}


bool CWBCConfigure::__replaceEscape( const char* pSource, char** pDest )
{
    if( pSource == NULL )
        return false;

    std::string stsString = pSource;
    std::string stsTarget = "\\n";
    std::string stsNewLIne = "\n";
    std::string::size_type pos = 0;

    while( (pos = stsString.find(stsTarget), pos) != std::string::npos )
    {
        stsString.replace( pos, stsTarget.length(), stsNewLIne );
        pos ++;
    }

    (*pDest) = new char[stsString.size() + 1];
    memcpy( (*pDest), stsString.c_str(), stsString.size() );
    (*pDest)[stsString.size()] = NULL;

    return true;
}

//================================================================================================= E. OPERATION
//================================================================================================= S. GET/SET

int CWBCConfigure::getDirInfoListSize( void )
{
    return this->m_dirItemList.size();
}


int CWBCConfigure::getExtensionListSize( void )
{
    return this->m_extensionList.size();
}


bool CWBCConfigure::getDirInfo( int nIndex, PSDIR_ITEM pDirItem )
{
    if( (nIndex < 0) || (nIndex >= (int)this->m_dirItemList.size()) )
        return false;

    ITER_DIRITEM iDir = this->m_dirItemList.begin();
    std::advance( iDir, nIndex );

    if( iDir == this->m_dirItemList.end() )
        return false;

    *pDirItem = *iDir;

    return true;
}


bool CWBCConfigure::getExtension( int nIndex, CString& cstrExtension )
{
    if( (nIndex < 0) || (nIndex >= (int)this->m_extensionList.size()) )
        return false;

    ITER_EXTENSION iExt = this->m_extensionList.begin();
    std::advance( iExt, nIndex );

    if( iExt == this->m_extensionList.end() )
        return false;

    cstrExtension = *iExt;

    return true;
}


bool CWBCConfigure::isRemovalRecursiveScan( void )  { return !!(this->m_dwFlag & FLAG_REMOVAL_RECVSCAN);    }
bool CWBCConfigure::isDesktopRecursiveScan( void )  { return !!(this->m_dwFlag & FLAG_DESKTOP_RECVSCAN);    }
bool CWBCConfigure::isCaptureUseFileTime( void )    { return !!(this->m_dwFlag & FLAG_CAPTURE_FILETIME);    }
bool CWBCConfigure::isCaptureUseAddPos( void )      { return !!(this->m_dwFlag & FLAG_CAPTURE_ADDPOS);      }


bool CWBCConfigure::setLString( const std::string& stsLSID, ELANGUAGES_ID eLID, const CString& cstrLString )
{
    HashedLID lID = HashedLID( stsLSID, (unsigned int)eLID );

    ITER_LSTRING iString = this->m_stringMap.find( lID );
    if( iString != this->m_stringMap.end() )
        return false;

    iString->second = cstrLString;

    return true;
}


bool CWBCConfigure::getLString( const std::string& stsLSID, CString& cstrLString )
{
    return getLString( stsLSID, this->m_curLanguageID, cstrLString );
}


bool CWBCConfigure::getLString( const std::string& stsLSID, ELANGUAGES_ID eLID, CString& cstrLString )
{
    ITER_LSTRING iString = this->m_stringMap.find( HashedLID(stsLSID,(unsigned int)eLID) );
    if( iString == this->m_stringMap.end() )
        return false;

    cstrLString = iString->second;

    return true;
}


bool CWBCConfigure::setCurLanguage( ELANGUAGES_ID LID )
{
    if( LID == LANGUAGE_INVALID )
        return false;

    this->m_curLanguageID = LID;

    return true;
}


CWBCConfigure::ELANGUAGES_ID CWBCConfigure::getCurLanguage( void )
{
    return this->m_curLanguageID;
}
