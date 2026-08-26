//=================================================================================================
// This confidential and proprietary software may be used only as authorized by a licensing
// agreement from Core Logic Limited.
//
// COPYRIGHT (c) Core Logic INC. ALL RIGHTS RESERVED.
//
// The entire notice above must be reproduced on all authorized copies and copies may only be made
// to the extent permitted by a licensing agreement from Core Logic Limited.
//
// Module: IniLoader.cpp
//
// Description: 
//
// Update History
// [13/8/2014 allen] create this file
//=================================================================================================
//================================================================================================= S.INCLUDE

#include "wbcIniLoader.h"

//================================================================================================= E.INCLUDE
//======V=======================================V================================================== S.PREPROCESSOR

//#define

//================================================================================================= E.PREPROCESSOR
//================================================================================================= S.CON/DCON

CWBCIniLoader::CWBCIniLoader()
{
    this->m_bIsExistIni = true;
}


CWBCIniLoader::~CWBCIniLoader()
{

}

//================================================================================================= E.CON/DCON
//================================================================================================= S.INIT/DEINIT

bool CWBCIniLoader::load( const CString& cstrIniPath )
{
    FILE *fpSource;
    char strLineBuffer[INI_MAX_STRING];
    _wfopen_s( &fpSource, (LPCTSTR)cstrIniPath, TEXT("r") );
    if( fpSource )
    {
        while( fgets(strLineBuffer, INI_MAX_STRING, fpSource) )
            this->_prase( strLineBuffer );

        fflush( fpSource );
        fclose( fpSource );
    }
    else
    {
        this->m_bIsExistIni = false;
    }

    this->_makeWorkingCopy();

    return true;
}


void CWBCIniLoader::_prase( char* pString )
{
    const char* pSeparator = " =\r\n\t";
    char strLineBuffer[INI_MAX_STRING];

    strcpy_s( strLineBuffer, INI_MAX_STRING, pString );

    char* pContext = NULL;
    char* pToken = strtok_s( strLineBuffer, pSeparator, &pContext );
    int nTokenCount = 0;
    int nItemIndex = -1;
    std::string stsItemName;
    bool bSucceeded = true;

    while( bSucceeded && pToken )
    {
        switch( nTokenCount )
        {
        case 0:
            {
                if( this->isExistItem( pToken, &nItemIndex ) )
                {
                    stsItemName = pToken;
                }
                else
                {
                    printf("INI ITEM <%s> is not registered! Ignore it.\n", pToken );
                    bSucceeded = false;
                }

            } break;

        case 1:
            {
                this->__setFileItemValue( nItemIndex, pToken );

            } break;

        default:;
        }

        nTokenCount ++;
        pToken = strtok_s( NULL, pSeparator, &pContext );
    }

    return;
}


bool CWBCIniLoader::__setFileItemValue( int nIndex, const std::string& stsValue )
{
    ITER_INIITEM iIni = this->m_IniList.begin();
    std::advance( iIni, nIndex );

    if( iIni == this->m_IniList.end() )
        return false;

    iIni->m_stsOrigin = stsValue;

    return true;
}

//================================================================================================= E.INIT/DEINIT
//================================================================================================= S.OPERATION

bool CWBCIniLoader::compare( const std::string& stsItemName )
{
    int nIndex;
    if( ! this->isExistItem(stsItemName, &nIndex) )
        return false;

    return this->compare( nIndex );
}


bool CWBCIniLoader::compare( int nIndex )
{
    if( (nIndex < 0) || ((int)this->m_IniList.size() <= nIndex) )
        return false;

    ITER_INIITEM iItem = this->m_IniList.begin();
    std::advance( iItem, nIndex );

    return iItem->m_stsOrigin == iItem->m_stsCurValue;
}


bool CWBCIniLoader::compareAll( void )
{
    if( this->m_IniList.empty() )
        return true;

    if( ! this->m_bIsExistIni )
        return false;

    ITER_INIITEM iItem = this->m_IniList.begin();
    ITER_INIITEM iEndItem = this->m_IniList.end();
    for( ; iItem != iEndItem; iItem ++ )
    {
        if( ! iItem->m_stsCurValue.compare(iItem->m_stsOrigin) )
            return false;
    }

    return true;
}


bool CWBCIniLoader::write( const CString& cstrPath )
{
    FILE *fpDest;
    _wfopen_s( &fpDest, (LPCTSTR)cstrPath, TEXT("w+") );
    if( !fpDest )
        return false;

    char strLineBuffer[INI_MAX_STRING];

    ITER_INIITEM iItem = this->m_IniList.begin();
    ITER_INIITEM iEndItem = this->m_IniList.end();
    for( ; iItem != iEndItem; iItem ++ )
    {
        sprintf_s( strLineBuffer, INI_MAX_STRING, "%s=%s\r\n", iItem->m_stsName.c_str(), iItem->m_stsCurValue );
        printf( "%s", strLineBuffer );
        fputs( strLineBuffer, fpDest );
    }

    fflush( fpDest );
    fclose( fpDest );

    return true;
}


bool CWBCIniLoader::_makeWorkingCopy( void )
{
    ITER_INIITEM iItem = this->m_IniList.begin();
    ITER_INIITEM iEndItem = this->m_IniList.end();
    for( ; iItem != iEndItem; iItem ++ )
        iItem->m_stsCurValue = iItem->m_stsOrigin;

    return true;
}

//================================================================================================= E.OPERATION
//================================================================================================= S.MESSAGING

//CIniLoader::

//================================================================================================= E.MESSAGING
//================================================================================================= S.CONTROL

bool CWBCIniLoader::addItem( const std::string& stsItemName, int nDefault, int nMin, int nMax )
{
    if( (stsItemName.empty()) || this->isExistItem(stsItemName) )
        return false;

    char strOrigin[INI_MAX_VALUE];
    sprintf_s( strOrigin, INI_MAX_VALUE, "%d", nDefault );

    SINI_ITEM iniItem;
    iniItem.m_eDataType     = DATA_TYPE_INTEGER;
    iniItem.m_stsName       = stsItemName;
    iniItem.m_stsOrigin     = strOrigin;
    iniItem.m_stsCurValue   = "";
    iniItem.m_nMin          = nMin;
    iniItem.m_nMax          = nMax;

    this->m_IniList.push_back( iniItem );

    return true;
}


bool CWBCIniLoader::addItem( const std::string& stsItemName, float fDefault, float fMin, float fMax )
{
    if( (stsItemName.empty()) || this->isExistItem(stsItemName) )
        return false;

    char strOrigin[INI_MAX_VALUE];
    sprintf_s( strOrigin, INI_MAX_VALUE, "%f", fDefault );

    SINI_ITEM iniItem;
    iniItem.m_eDataType     = DATA_TYPE_FLOAT;
    iniItem.m_stsName       = stsItemName;
    iniItem.m_stsOrigin     = strOrigin;
    iniItem.m_stsCurValue   = "";
    iniItem.m_nMin          = int(fMin * 1000);
    iniItem.m_nMax          = int(fMax * 1000);

    return true;
}


bool CWBCIniLoader::addItem( const std::string& stsItemName, const std::string& stsValue )
{
    if( (stsItemName.empty()) || this->isExistItem(stsItemName) )
        return false;

    SINI_ITEM iniItem;
    iniItem.m_eDataType     = DATA_TYPE_STRING;
    iniItem.m_stsName       = stsItemName;
    iniItem.m_stsOrigin     = stsValue;
    iniItem.m_stsCurValue   = "";
    iniItem.m_nMin          = 0;
    iniItem.m_nMax          = 0;

    return true;
}


bool CWBCIniLoader::addItem( const std::string& stsItemName, PSDATE_TIME pDateTime )
{
    if( (stsItemName.empty()) || this->isExistItem(stsItemName) )
        return false;

    char strOrigin[INI_MAX_VALUE];
    sprintf_s( strOrigin, INI_MAX_VALUE, "%d%02d%02d%02d%02d%02d",
        pDateTime->m_year, pDateTime->m_month, pDateTime->m_day, pDateTime->m_hour, pDateTime->m_min, pDateTime->m_sec );

    SINI_ITEM iniItem;
    iniItem.m_eDataType     = DATA_TYPE_DATETIME;
    iniItem.m_stsName       = stsItemName;
    iniItem.m_stsOrigin     = strOrigin;
    iniItem.m_stsCurValue   = "";
    iniItem.m_nMin          = 0;
    iniItem.m_nMax          = 0;

    return true;
}


bool CWBCIniLoader::removeItem( const std::string& stsItemName )
{
    int nIndex;
    if( ! this->isExistItem(stsItemName, &nIndex) )
        return false;

    return this->removeItem( nIndex );
}


bool CWBCIniLoader::removeItem( int nIndex )
{
    if( (nIndex < 0) || ((int)this->m_IniList.size() <= nIndex) )
        return false;

    ITER_INIITEM iItem = this->m_IniList.begin();
    std::advance( iItem, nIndex );

    this->m_IniList.erase( iItem );

    return true;
}


void CWBCIniLoader::clearItems( void )
{
    if( this->m_IniList.empty() )
        return;

    this->m_IniList.clear();

    return;
}


bool CWBCIniLoader::restore( const std::string& stsItemName )
{
    int nIndex;

    if( ! this->isExistItem(stsItemName, &nIndex) )
        return NULL;

    return this->restore( nIndex );
}


bool CWBCIniLoader::restore( int nIndex )
{
    if( (nIndex < 0) || ((int)this->m_IniList.size() <= nIndex) )
        return false;

    ITER_INIITEM iItem = this->m_IniList.begin();
    std::advance( iItem, nIndex );

    iItem->m_stsCurValue = iItem->m_stsOrigin;

    return true;
}


void CWBCIniLoader::restoreAll( void )
{
    ITER_INIITEM iItem = this->m_IniList.begin();
    ITER_INIITEM iEndItem = this->m_IniList.end();
    for( ; iItem != iEndItem; iItem ++ )
        iItem->m_stsCurValue = iItem->m_stsOrigin;

    return;
}

//================================================================================================= E.CONTROL
//================================================================================================= S.GET/SET

bool CWBCIniLoader::isExistFile( void )
{
    return this->m_bIsExistIni;
}


bool CWBCIniLoader::isExistItem( const std::string& stsItemName, int* pIndex )
{
    bool bReturn = false;

    ITER_INIITEM iItem = this->m_IniList.begin();
    ITER_INIITEM iEndItem = this->m_IniList.end();

    for( ; iItem != iEndItem; iItem ++ )
    {
        if( ! iItem->m_stsName.compare(stsItemName) )
            break;
    }

    if( iItem != iEndItem )
    {
        if( pIndex )
            *pIndex = std::distance( this->m_IniList.begin(), iItem );

        bReturn = true;
    }

    return bReturn;
}


int CWBCIniLoader::getItemCount( void )
{
    return this->m_IniList.size();
}


CWBCIniLoader::PSINI_ITEM CWBCIniLoader::getItem( const std::string& stsItemName )
{
    int nIndex;

    if( ! this->isExistItem(stsItemName, &nIndex) )
        return NULL;

    return this->getItem( nIndex );
}


CWBCIniLoader::PSINI_ITEM CWBCIniLoader::getItem( int nIndex )
{
    if( (nIndex < 0) || ((int)this->m_IniList.size() <= nIndex) )
        return NULL;

    ITER_INIITEM iItem = this->m_IniList.begin();
    std::advance( iItem, nIndex );

    return &(*iItem);
}


CWBCIniLoader::EDATA_TYPE CWBCIniLoader::getItemType( const std::string& stsItemName )
{
    int nIndex;

    if( ! this->isExistItem(stsItemName, &nIndex) )
        return DATA_TYPE_INVALID;

    ITER_INIITEM iItem = this->m_IniList.begin();
    std::advance( iItem, nIndex );

    if( iItem == this->m_IniList.end() )
        return DATA_TYPE_INVALID;

    return iItem->m_eDataType;
}


bool CWBCIniLoader::getItemRange( const std::string& stsItemName, int* pMin, int* pMax )
{
    int nIndex;

    if( ! this->isExistItem(stsItemName, &nIndex) )
        return false;

    return this->getItemRange( nIndex, pMin, pMax );
}


bool CWBCIniLoader::getItemRange( int nIndex, int* pMin, int* pMax )
{
    ITER_INIITEM iItem = this->m_IniList.begin();
    std::advance( iItem, nIndex );

    if(     (iItem->m_eDataType != DATA_TYPE_INTEGER)
        &&  (iItem->m_eDataType != DATA_TYPE_FLOAT) )
    {
        return false;
    }

    if( pMin ) *pMin = iItem->m_nMin;
    if( pMax ) *pMax = iItem->m_nMax;

    return true;
}


bool CWBCIniLoader::getItemValue( const std::string& stsItemName, int* pValue )
{
    int nIndex;
    if( ! this->isExistItem(stsItemName, &nIndex) )
        return false;

    ITER_INIITEM iIni = this->m_IniList.begin();
    std::advance( iIni, nIndex );

    if( iIni == this->m_IniList.end() )
        return false;

    if( iIni->m_eDataType != DATA_TYPE_INTEGER )
        return false;

    *pValue = atoi( iIni->m_stsCurValue.c_str() );

    return true;
}


bool CWBCIniLoader::getItemValue( const std::string& stsItemName, float* pValue )
{
    int nIndex;
    if( ! this->isExistItem(stsItemName, &nIndex) )
        return false;

    ITER_INIITEM iIni = this->m_IniList.begin();
    std::advance( iIni, nIndex );

    if( iIni == this->m_IniList.end() )
        return false;

    if( iIni->m_eDataType != DATA_TYPE_FLOAT )
        return false;

    *pValue = (float)atof( iIni->m_stsCurValue.c_str() );

    return true;
}


bool CWBCIniLoader::getItemValue( const std::string& stsItemName, std::string& stsValue )
{
    int nIndex;
    if( ! this->isExistItem(stsItemName, &nIndex) )
        return false;

    ITER_INIITEM iIni = this->m_IniList.begin();
    std::advance( iIni, nIndex );

    if( iIni == this->m_IniList.end() )
        return false;

    if( iIni->m_eDataType != DATA_TYPE_STRING )
        return false;

    stsValue = iIni->m_stsCurValue;

    return true;
}


bool CWBCIniLoader::getItemValue( const std::string& stsItemName, PSDATE_TIME pDateTime )
{
    int nIndex;
    if( ! this->isExistItem(stsItemName, &nIndex) )
        return false;

    ITER_INIITEM iIni = this->m_IniList.begin();
    std::advance( iIni, nIndex );

    if( iIni == this->m_IniList.end() )
        return false;

    if( iIni->m_eDataType != DATA_TYPE_STRING )
        return false;

    char strValue[INI_MAX_VALUE];
    char* pSeparator = "-T:";
    strcpy_s( strValue, INI_MAX_VALUE, iIni->m_stsCurValue.c_str() );

    char* pContext = NULL;
    char* pToken = strtok_s( strValue, pSeparator, &pContext );
    int nTokenCount = 0;
    while( pToken )
    {
        switch( nTokenCount )
        {
        case 0: pDateTime->m_year   = atoi( pToken );   break;
        case 1: pDateTime->m_month  = atoi( pToken );   break;
        case 2: pDateTime->m_day    = atoi( pToken );   break;
        case 3: pDateTime->m_hour   = atoi( pToken );   break;
        case 4: pDateTime->m_min    = atoi( pToken );   break;
        case 5: pDateTime->m_sec    = atoi( pToken );   break;
        default:;
        }

        nTokenCount ++;
        pToken = strtok_s( NULL, pSeparator, &pContext );
    }

    return true;
}


bool CWBCIniLoader::setItemValue( int nIndex, int nValue )
{
    ITER_INIITEM iIni = this->m_IniList.begin();
    std::advance( iIni, nIndex );

    if( iIni == this->m_IniList.end() )
        return false;

    if( iIni->m_eDataType != DATA_TYPE_INTEGER )
        return false;

    char strValue[INI_MAX_VALUE];
    sprintf_s( strValue, INI_MAX_VALUE, "%d", nValue );

    iIni->m_stsCurValue = strValue;

    return true;
}


bool CWBCIniLoader::setItemValue( int nIndex, float fValue )
{
    ITER_INIITEM iIni = this->m_IniList.begin();
    std::advance( iIni, nIndex );

    if( iIni == this->m_IniList.end() )
        return false;

    if( iIni->m_eDataType != DATA_TYPE_FLOAT )
        return false;

    char strValue[INI_MAX_VALUE];
    sprintf_s( strValue, INI_MAX_VALUE, "%d", fValue );

    iIni->m_stsCurValue = strValue;

    return true;
}


bool CWBCIniLoader::setItemValue( int nIndex, const std::string& stsValue )
{
    ITER_INIITEM iIni = this->m_IniList.begin();
    std::advance( iIni, nIndex );

    if( iIni == this->m_IniList.end() )
        return false;

    if( iIni->m_eDataType != DATA_TYPE_STRING )
        return false;

    iIni->m_stsCurValue = stsValue;

    return true;
}


bool CWBCIniLoader::setItemValue( int nIndex, PSDATE_TIME pDateTime )
{
    ITER_INIITEM iIni = this->m_IniList.begin();
    std::advance( iIni, nIndex );

    if( iIni == this->m_IniList.end() )
        return false;

    if( iIni->m_eDataType != DATA_TYPE_DATETIME )
        return false;

    char strValue[INI_MAX_VALUE];
    sprintf_s( strValue, INI_MAX_VALUE, "%d-%02d-%02dT%02d:%02d:%02d",
        pDateTime->m_year, pDateTime->m_month, pDateTime->m_day, pDateTime->m_hour, pDateTime->m_min, pDateTime->m_sec );

    iIni->m_stsCurValue = strValue;

    return true;
}

//================================================================================================= E.GET/SET
//================================================================================================= S.STATIC

//================================================================================================= E.STATIC
//================================================================================================= EOF
