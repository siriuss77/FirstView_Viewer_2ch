//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//  Module: wbcImageHelper.h
//
//  Description: WBC image rendering helper
//
//  Update History
// 
//      2014.10.30 [Allen Han] Initial Release v1.0.00, The last one, Good-bye
//=================================================================================================
//================================================================================================= S. FILEID
#pragma once
//================================================================================================= E. FILEID
//================================================================================================= S. INCLUDE

#include "stdafx.h"
#include "wbcUtil.h"

//================================================================================================= E. INCLUDE
//================================================================================================= S. CLASS

class CWBCImageHelper
{

public:
    CWBCImageHelper();
    ~CWBCImageHelper();

public:
        void                    deinit              ( void );

public:
        bool                    loadImage           ( UINT nResourceID, const Color& color = Color((ARGB)Color::White), bool bUseTpColor = false,
                                                        const Color& tpColor = Color((ARGB)Color::Black), const ClGeometry& geoImage = GEO_FULL_SIZE,
                                                        const ClGeometry& geoSubImage = GEO_FULL_SIZE, DWORD dwGeoBase = GEOMAN_LEFT | GEOMAN_TOP );
        void                    drawImage           ( CDC* pDC, const CRect& rtClient, ClGeometry* pGeoTarget = NULL, ClGeometry* pGeoSource = NULL );
        void                    prepareImage        ( CDC* pDC, const CRect& rtClient, CDC* pBackground = NULL, BOOL bUseMask = FALSE );

public:
        void                    setUpdate           ( void );
        void                    setRotateAngel      ( REAL fAngle );

protected:

        Image*                  m_pImage;
        CDC                     m_dcImage;
        Color                   m_color;
        bool                    m_bUseColorKey;
        bool                    m_bIsPrepared;
        Color                   m_ColorKey;
        REAL                    m_RotateAngle;
        DWORD                   m_dwGeoMan;
        ClGeometry              m_geoImage;
        ClGeometry              m_geoSubImage;
};

//================================================================================================= E. CLASS
//================================================================================================= EOF
