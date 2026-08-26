// ================================================================================================
// This confidential and proprietary software may be used only as authorized by a licensing
// agreement from Core Logic Limited.
//
// COPYRIGHT (c) Core Logic INC. ALL RIGHTS RESERVED.
//
// The entire notice above must be reproduced on all authorized copies and copies may only be made
// to the extent permitted by a licensing agreement from Core Logic Limited.
//
// Module: wbcIniLoader.h
//
// Description: 
//
// Update History
// [13/8/2014 allen] create this file
//=================================================================================================
//================================================================================================= S.FILEID
#pragma once
//================================================================================================= E.FILEID
//================================================================================================= S.INCLUDE

#include "stdafx.h"

#include <string>
#include <list>

//================================================================================================= E.INCLUDE
//======V=======================================V================================================== S.PREPROCESSOR

//#define

//================================================================================================= E.PREPROCESSOR
//==V=======V===================V===========================V====================================== S.PROTOTYPE

#define     INI_MAX_STRING                                  (64 + 64)
#define     INI_MAX_VALUE                                   64

//================================================================================================= E.PROTOTYPE
//==V=======V===================V===========================V====================================== S.CLASS

class CWBCIniLoader
{

public:

    CWBCIniLoader();
    virtual ~CWBCIniLoader();

    typedef enum _eDATATYPES
    {
        DATA_TYPE_INTEGER   = 0 ,
        DATA_TYPE_FLOAT         ,   // min, max will divided by 1000
        DATA_TYPE_STRING        ,
        DATA_TYPE_DATETIME      ,
        DATA_TYPE_INVALID

    } EDATA_TYPE, *PEDATA_TYPE;

    typedef struct _tagINIITEM
    {
        EDATA_TYPE      m_eDataType;
        std::string     m_stsName;
        std::string     m_stsOrigin;
        std::string     m_stsCurValue;
        long            m_nMin;
        long            m_nMax;

    } SINI_ITEM, *PSINI_ITEM;

    typedef struct _tagDATETIME
    {
        int             m_year;
        int             m_month;
        int             m_day;
        int             m_hour;
        int             m_min;
        int             m_sec;

    } SDATE_TIME, *PSDATE_TIME;

    typedef std::list<SINI_ITEM>                            INIITEM_LIST;
    typedef INIITEM_LIST::iterator                          ITER_INIITEM;


// INIT/DEINIT
public:
            bool                load                        ( const CString& cstrIniPath );
protected:
            void                _prase                      ( char* pString );
            bool                __setFileItemValue          ( int nIndex, const std::string& stsValue );


// OPERATION
public:
            bool                compare                     ( const std::string& stsItemName );
            bool                compare                     ( int nIndex );
            bool                compareAll                  ( void );

            bool                write                       ( const CString& cstrPath );
protected:
            bool                _makeWorkingCopy            ( void );


// MESSAGING
public:
protected:


// CONTROL
public:
            bool                addItem                     ( const std::string& stsItemName, int nDefault, int nMin, int nMax );
            bool                addItem                     ( const std::string& stsItemName, float fDefault, float fMin, float fMax );
            bool                addItem                     ( const std::string& stsItemName, const std::string& stsValue );
            bool                addItem                     ( const std::string& stsItemName, PSDATE_TIME pDateTime );
            bool                removeItem                  ( const std::string& stsItemName );
            bool                removeItem                  ( int nIndex );
            void                clearItems                  ( void );

            bool                restore                     ( const std::string& stsItemName );
            bool                restore                     ( int nIndex );
            void                restoreAll                  ( void );

protected:


// GET/SET
public:
            bool                isExistFile                 ( void );
            bool                isExistItem                 ( const std::string& stsItemName, int* pIndex = NULL );
            int                 getItemCount                ( void );

            PSINI_ITEM          getItem                     ( int nIndex );
            PSINI_ITEM          getItem                     ( const std::string& stsItemName );

            EDATA_TYPE          getItemType                 ( const std::string& stsItemName );

            bool                getItemRange                ( const std::string& stsItemName, int* pMin, int* pMax );
            bool                getItemRange                ( int nIndex, int* pMin, int* pMax );

            bool                getItemValue                ( const std::string& stsItemName, int* pValue );
            bool                getItemValue                ( const std::string& stsItemName, float* pValue );
            bool                getItemValue                ( const std::string& stsItemName, std::string& stsValue );
            bool                getItemValue                ( const std::string& stsItemName, PSDATE_TIME pDateTime );

            bool                setItemValue                ( int nIndex, int nValue );
            bool                setItemValue                ( int nIndex, float fValue );
            bool                setItemValue                ( int nIndex, const std::string& stsValue );
            bool                setItemValue                ( int nIndex, PSDATE_TIME pDateTime );
protected:


// MEMBER
protected:
private:

            CString             m_cstrIniPath;
            bool                m_bIsExistIni;
            INIITEM_LIST        m_IniList;
};

//================================================================================================= E.CLASS
//================================================================================================= EOF
