//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//  Module: wbcConfig.h
//
//  Description: Viewer config class
//
//  Update History
// 
//      2014.10.27 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. INCLUDE

#include "stdafx.h"
#include <TinyXML/tinyxml.h>
#include <map>
#include <list>
#include "CLWBC/wbcUtil.h"

//================================================================================================= E. INCLUDE
//================================================================================================= S. PREPROCESSOR

#define PATH_CONFIG_DEFAULT                                 TEXT("viewerconfig.cfg")

//================================================================================================= E. PREPROCESSOR
//================================================================================================= S. TYPEDEF

//================================================================================================= E. TYPEDEF
//================================================================================================= S. CLASS

class CWBCConfigure
{

private:
    CWBCConfigure();
    ~CWBCConfigure();

public:
    typedef enum _eLANGUAGES
    {
        LANGUAGE_ENGLISH    = 0 ,
        LANGUAGE_KOREAN         ,
        LANGUAGE_JAPANESE       ,
        LANGUAGE_CHINESE_S      ,
        LANGUAGE_CHINESE_T      ,
        LANGUAGE_RUSSIAN        ,
        LANGUAGE_INVALID

    } ELANGUAGES_ID, *PELANGUAGES_ID;

    typedef enum _eDIRTYPES
    {
        DIR_TYPE_NORMAL     = 0 ,
        DIR_TYPE_EVENT          ,
        DIR_TYPE_MOTION         ,
        DIR_TYPE_MANUAL         ,
        DIR_TYPE_IMAGE          ,
        DIR_TYPE_INVALID

    } EDIR_TYPE, *PEDIR_TYPE;

    enum _eFLAGS
    {
        FLAG_REMOVAL_RECVSCAN   = 1 << 0,
        FLAG_DESKTOP_RECVSCAN   = 1 << 1,
        FLAG_CAPTURE_FILETIME   = 1 << 2,
        FLAG_CAPTURE_ADDPOS     = 1 << 3,
    };

    typedef struct _tagDIRITEMINFO
    {
        EDIR_TYPE       m_eDirType;
        CString         m_cstrDirName;
        std::string     m_stsDirName;
        int             m_nNxfsPercent;

    } SDIR_ITEM, *PSDIR_ITEM;

    typedef std::map<HashedLID,CString>                     LSTRING_MAP;
    typedef LSTRING_MAP::iterator                           ITER_LSTRING;
    typedef std::pair<HashedLID,CString>                    PAIR_LSTRING;

    typedef std::map<HashedLID,ELANGUAGES_ID>               LID_MAP;
    typedef LID_MAP::iterator                               ITER_LID;
    typedef std::pair<HashedLID,ELANGUAGES_ID>              PAIR_LID;

    typedef std::list<CString>                              EXTENSION_LIST;
    typedef EXTENSION_LIST::iterator                        ITER_EXTENSION;

    typedef std::list<SDIR_ITEM>                            DIRITEM_LIST;
    typedef DIRITEM_LIST::iterator                          ITER_DIRITEM;

public:
    inline static CWBCConfigure* getInstance()
    {
        ClAutoMutex _l( &CWBCConfigure::m_ViewerConfigCS );

        if( CWBCConfigure::m_pViewerConfig == NULL )
            CWBCConfigure::m_pViewerConfig = new CWBCConfigure();

        return CWBCConfigure::m_pViewerConfig;
    }

    inline static void releaseInstance()
    {
        ClAutoMutex _l( &CWBCConfigure::m_ViewerConfigCS );

        if( CWBCConfigure::m_pViewerConfig != NULL )
        {
            CWBCConfigure::m_pViewerConfig->deInit();
            delete CWBCConfigure::m_pViewerConfig;
            CWBCConfigure::m_pViewerConfig = NULL;
        }
    }

    static  CWBCConfigure*  m_pViewerConfig;
    static  ClMutex         m_ViewerConfigCS;
private:


public:
            bool            init                            ( const CString& cstrCFGName = PATH_CONFIG_DEFAULT );
            void            deInit                          ( void );
protected:
            bool            _loadConfig                     ( const std::string& stsFilePath );
            bool            __parseConfigs                  ( TiXmlElement* pEleConfigs );
            bool            __parseLStrings                 ( TiXmlElement* pEleLStrings );


public:
protected:
            bool            __addDirInfo                    ( const char* lpszDirName, const char* lpszDirType, int nNXFSPercent );
            bool            __addExtension                  ( const char* lpszExtension );

            void            _addLanguageString              ( const char* pLSID, const char* lpszLanguageType, int mode, const char* pUTF8String );
            ELANGUAGES_ID   __getLID                        ( const char* pLanguage, int mode );
            bool            __convertUTF8ToCString          ( const char* pUTF8String, CString& cstrTarget );
            bool            __replaceEscape                 ( const char* pSource, char** pDest );

public:
            int             getDirInfoListSize              ( void );
            int             getExtensionListSize            ( void );
            bool            getDirInfo                      ( int nIndex, PSDIR_ITEM pDirItem );
            bool            getExtension                    ( int nIndex, CString& cstrExtension );

            bool            isRemovalRecursiveScan          ( void );
            bool            isDesktopRecursiveScan          ( void );
            bool            isCaptureUseFileTime            ( void );
            bool            isCaptureUseAddPos              ( void );

            bool            setLString                      ( const std::string& stsLSID, ELANGUAGES_ID eLID, const CString& cstrLString );
            bool            getLString                      ( const std::string& stsLSID, ELANGUAGES_ID eLID, CString& cstrLString );
            bool            getLString                      ( const std::string& stsLSID, CString& cstrLString );

            bool            setCurLanguage                  ( ELANGUAGES_ID languageID );
            ELANGUAGES_ID   getCurLanguage                  ( void );
protected:


public:
protected:
            ELANGUAGES_ID   m_curLanguageID;

            EXTENSION_LIST  m_extensionList;
            DIRITEM_LIST    m_dirItemList;

            LID_MAP         m_LIDMap;
            LSTRING_MAP     m_stringMap;

            DWORD           m_dwFlag;
};

//================================================================================================= E. CLASS
//viewer.cfg
