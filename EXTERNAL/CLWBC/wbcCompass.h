//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//  Module: wbcCompass.h
//
//  Description: Blackbox dedicated windows control - COMPASS
//
//  Update History
// 
//      2014.11.10 [Allen Han] Initial Release v1.0.00, The last one, Good-bye
//=================================================================================================
//================================================================================================= S. FILEID
#pragma once
//================================================================================================= E. FILEID
//================================================================================================= S. INCLUDE

#include "stdafx.h"
#include "wbcImageHelper.h"
#include "wbcIBackground.h"
#include "wbcTextData.h"

//================================================================================================= E. INCLUDE
//================================================================================================= S. CLASS

class CWBCCompass : public CStatic, IWBCBackground
{
    DECLARE_DYNAMIC(CWBCCompass)

public:

    CWBCCompass();
    ~CWBCCompass();


public:

    typedef enum _eSKINTYPES
    {
        SKIN_NEEDLE         = 0 ,
        SKIN_METER              ,
        SKIN_INVALID

    } ESKIN_TYPE;

    typedef enum _eACTIONS
    {
        ACTION_ROTATE_NEEDLE = 0,
        ACTION_ROTATE_METER     ,
        ACTION_INVALID

    } EACTION;


public:

    virtual VOID                    TransferBackground  ( CDC* pDC );
    virtual void                    DrawItem            ( LPDRAWITEMSTRUCT lpDrawItemStruct );

protected:

    DECLARE_MESSAGE_MAP()


public:
            EACTION                 getAction           ( void );
            bool                    setAction           ( EACTION eAction );

            bool                    setSkin             ( ESKIN_TYPE eSkinType, UINT uResourceID, const Color& color = Color((ARGB)Color::White), bool bUseTpColor = false,
                                                            const Color& tpColor = Color((ARGB)Color::Black), const ClGeometry& geoImage = GEO_FULL_SIZE,
                                                            const ClGeometry& geoSubImage = GEO_FULL_SIZE, DWORD dwGeoBase = GEOMAN_LEFT | GEOMAN_TOP );

            void                    synchronize         ( DWORD dwCTS );
            BOOL                    SetSensorData       ( CWBCTextData* pSensorData );
protected:


protected:
            CWBCImageHelper*           m_pImageRender[SKIN_INVALID];
            EACTION                 m_eCurAction;

            DWORD                   m_dwCTS;
            CWBCTextData*           m_pSensorData;
};

//================================================================================================= E. CLASS
//================================================================================================= EOF
