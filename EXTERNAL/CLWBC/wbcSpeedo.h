//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//  Module: wbcSpeedo.h
//
//  Description: Blackbox dedicated windows control - Speedometer
//
//  Update History
// 
//      2014.11.10 [Allen Han] Initial Release v1.0.00, The last one, Good-bye
//=================================================================================================
//================================================================================================= S. INCLUDE

#include "stdafx.h"
#include "wbcTextData.h"
#include "wbcIBackground.h"
#include "wbcImageHelper.h"

//================================================================================================= E. INCLUDE
//================================================================================================= S. CLASS

class CWBCSpeedomter : public CStatic, IWBCBackground
{
    DECLARE_DYNAMIC(CWBCSpeedomter)

public:

    CWBCSpeedomter();
    ~CWBCSpeedomter();

    typedef enum _eMODES
    {
        MODE_SPEEDOMETER    = 0 ,
        MODE_GRAPH              ,
        MODE_INVALID

    } EMODE;

    typedef enum _eSKINS
    {
        SKIN_NEEDLE         = 0 ,
        SKIN_BACKGROUND         ,
        SKIN_INVALID

    } ESKIN;

    enum _eFLAGS
    {
        FLAG_REDRAW         = 1 << 0,
        FLAG_DRAW_TIMELINE  = 1 << 1,
        FLAG_SHOW_NEEDLE    = 1 << 2,
        FLAG_SHOW_TEXT      = 1 << 3,
        FLAG_SHOW_TIMELINE  = 1 << 4,
    };

public:
protected:

public:

    virtual VOID                    TransferBackground  ( CDC* pDC );
    virtual void                    DrawItem            ( LPDRAWITEMSTRUCT lpDrawItemStruct );

protected:

    DECLARE_MESSAGE_MAP()

public:
            EMODE                   getMode             ( void );
            bool                    setMode             ( EMODE eMode );

            bool                    setSkin             ( ESKIN eSkin, UINT nResourceID, const Color& color = Color((ARGB)Color::White), bool bUseTpColor = false,
                                                            const Color& tpColor = Color((ARGB)Color::Black), const ClGeometry& geoImage = GEO_FULL_SIZE,
                                                            const ClGeometry& geoSubImage = GEO_FULL_SIZE, DWORD dwGeoBase = GEOMAN_LEFT | GEOMAN_TOP );

            bool                    setRange            ( float fMaxSpeed, float fMinAngle, float fMaxAngle );
            bool                    setSensorData       ( CWBCTextData* pTextData );
            void                    setTimeLineStyle    ( const Color& crTimeLine, int nThick = 2 );

            void                    setRedraw           ( void );
            void                    setCTS              ( DWORD dwCTS );
            void                    showNeedle          ( bool bShowNeedle );
            void                    showText            ( bool bShowText );
            void                    showTimeline        ( bool bShowTimeline );

            void                    synchronize         ( DWORD dwCTS );

protected:


protected:
            EMODE                   m_eMode;
            DWORD                   m_dwFlags;
            ClGeometry              m_geoText;

            CWBCImageHelper*        m_pImageRender[SKIN_INVALID];

            DWORD                   m_dwCTS;
            CWBCTextData*           m_pSensorData;

            Color                   m_crTimeline;
            int                     m_nTimelineThick;

            float                   m_fMaxSpeed;
            float                   m_fMinAngle;
            float                   m_fMaxAngle;
};

//================================================================================================= E. CLASS
//================================================================================================= EOF
