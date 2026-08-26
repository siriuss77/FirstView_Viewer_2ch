//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//  Module: wbcSpeedo.cpp
//
//  Description: Blackbox dedicated windows control - Speedometer
//
//  Update History
// 
//      2014.11.10 [Allen Han] Initial Release v1.0.00, The last one, Good-bye
//=================================================================================================
//================================================================================================= S. INCLUDE

#include "wbcSpeedo.h"
#include "wbcMemDC.h"

//================================================================================================= E. INCLUDE
IMPLEMENT_DYNAMIC(CWBCSpeedomter, CStatic)
//================================================================================================= S. CON/DCON

CWBCSpeedomter::CWBCSpeedomter()
{
    this->m_eMode   = MODE_SPEEDOMETER;
    this->m_dwFlags = FLAG_REDRAW | FLAG_SHOW_NEEDLE | FLAG_SHOW_TEXT;

    for( int nIndex = SKIN_NEEDLE; nIndex < SKIN_INVALID; nIndex ++ )
        this->m_pImageRender[nIndex] = NULL;

    this->m_crTimeline = Color(255,255,255);
    this->m_nTimelineThick = 2;
}


CWBCSpeedomter::~CWBCSpeedomter()
{

}

//================================================================================================= E. CON/DCON
//================================================================================================= S. VIRTUAL

void CWBCSpeedomter::TransferBackground( CDC* pDC )
{
    this->m_dcBackground.DeleteDC();

    CBitmap bmp, *pOldBitmap;
    CRect rtWindow;
    GetWindowRect(rtWindow);
    GetParent()->ScreenToClient(rtWindow);

    this->m_dcBackground.CreateCompatibleDC( pDC );
    bmp.CreateCompatibleBitmap( pDC, rtWindow.Width(), rtWindow.Height() );
    pOldBitmap = this->m_dcBackground.SelectObject(&bmp);
    this->m_dcBackground.BitBlt( 0, 0, rtWindow.Width(), rtWindow.Height(), pDC, rtWindow.left, rtWindow.top, SRCCOPY );
    bmp.DeleteObject();

    return;
}


void CWBCSpeedomter::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
    CDC drawItemDC;
    drawItemDC.Attach( lpDrawItemStruct->hDC );

    CRect rect;
    GetClientRect(rect);
    CWBCMemDC memDC( &drawItemDC, &rect );

    if( this->m_eMode == MODE_SPEEDOMETER )
    {
        REAL angle = 0;
        if( this->m_pSensorData )
        {
            int nAdvance = this->m_dwCTS / 1000;
            if( nAdvance < (int)this->m_pSensorData->m_GPSList.size() )
            {
                CWBCTextData::ITER_GPS iGPSData = this->m_pSensorData->m_GPSList.begin();
                std::advance( iGPSData, nAdvance );
                if( iGPSData != this->m_pSensorData->m_GPSList.end() )
                    angle = (REAL)((this->m_fMinAngle + (iGPSData->m_fSpeed / this->m_fMaxSpeed) * (this->m_fMaxAngle - this->m_fMinAngle)));
            }
        }

        if( this->m_pImageRender[SKIN_BACKGROUND] )
        {
            this->m_pImageRender[SKIN_BACKGROUND]->setUpdate();
            this->m_pImageRender[SKIN_BACKGROUND]->prepareImage( &memDC, rect, &this->m_dcBackground, TRUE );
            this->m_pImageRender[SKIN_BACKGROUND]->drawImage( &memDC, rect );
        }

        if( this->m_pImageRender[SKIN_NEEDLE] )
        {
            this->m_pImageRender[SKIN_NEEDLE]->setRotateAngel( angle );
            this->m_pImageRender[SKIN_NEEDLE]->setUpdate();
            this->m_pImageRender[SKIN_NEEDLE]->prepareImage( &memDC, rect, &this->m_dcBackground, TRUE );
            this->m_pImageRender[SKIN_NEEDLE]->drawImage( &memDC, rect );
        }
    }
    else if( this->m_eMode == MODE_GRAPH )
    {

    }
    else
    {
        ;;
    }

    return;
}

//================================================================================================= E. VIRTUAL

BEGIN_MESSAGE_MAP(CWBCSpeedomter, CStatic)
END_MESSAGE_MAP()

//================================================================================================= S. MESSAGE

//================================================================================================= E. MESSAGE
//================================================================================================= S. INIT/DEINIT

//================================================================================================= E. INIT/DEINIT
//================================================================================================= S. OPERATION

void CWBCSpeedomter::synchronize( DWORD dwCTS )
{
    this->m_dwCTS = dwCTS;

    this->InvalidateRect( NULL );

    return;
}

//================================================================================================= E. OPERATION
//================================================================================================= S. GET/SET

CWBCSpeedomter::EMODE CWBCSpeedomter::getMode( void )
{
    return this->m_eMode;
}


bool CWBCSpeedomter::setMode( EMODE eMode )
{
    if( eMode == MODE_INVALID )
        return false;

    this->m_eMode = eMode;

    return true;
}


bool CWBCSpeedomter::setSkin( ESKIN eSkin, UINT uResourceID, const Color& color, bool bUseTpColor,
                                const Color& tpColor, const ClGeometry& geoImage, const ClGeometry& geoSubImage, DWORD dwGeoBase )
{
    bool bSucceeded = false;

    if( eSkin == SKIN_INVALID )
        return bSucceeded;

    if( this->m_pImageRender[eSkin] != NULL )
        delete this->m_pImageRender[eSkin];

    this->m_pImageRender[eSkin] = new CWBCImageHelper();
    if( this->m_pImageRender[eSkin] == NULL )
        return bSucceeded;

    bSucceeded = this->m_pImageRender[eSkin]->loadImage( uResourceID, color, bUseTpColor, tpColor, geoImage, geoSubImage, dwGeoBase );

    if( ! bSucceeded )
    {
        delete this->m_pImageRender[eSkin];
        this->m_pImageRender[eSkin] = NULL;
    }

    return bSucceeded;
}


bool CWBCSpeedomter::setRange( float fMaxSpeed, float fMinAngle, float fMaxAngle )
{
    if(     (fMaxSpeed < 0.f)
        ||  (fMinAngle < -359.9f)
        ||  (fMaxAngle > 359.9f)
        ||  (fMinAngle > fMaxAngle) )
    {
        return false;
    }

    this->m_fMaxSpeed = fMaxSpeed;
    this->m_fMinAngle = fMinAngle;
    this->m_fMaxAngle = fMaxAngle;

    return true;
}


bool CWBCSpeedomter::setSensorData( CWBCTextData* pTextData )
{
    if( pTextData == NULL )
        return false;

    this->m_pSensorData = pTextData;

    return true;
}


void CWBCSpeedomter::setTimeLineStyle( const Color& crTimeLine, int nThick )
{
    this->m_crTimeline = crTimeLine;
    this->m_nTimelineThick = nThick;

    return;
}


void CWBCSpeedomter::setRedraw( void )
{
    this->m_dwFlags |= FLAG_REDRAW;

    return;
}


void CWBCSpeedomter::setCTS( DWORD dwCTS )
{
    if( this->m_dwFlags & FLAG_SHOW_TIMELINE )
        this->m_dwFlags |= FLAG_DRAW_TIMELINE;

    this->m_dwCTS = dwCTS;

    this->Invalidate();

    return;
}


void CWBCSpeedomter::showNeedle( bool bShowNeedle )
{
    this->m_dwFlags |= FLAG_SHOW_NEEDLE;

    return;
}


void CWBCSpeedomter::showText( bool bShowText )
{
    this->m_dwFlags |= FLAG_SHOW_TEXT;

    return;
}


void CWBCSpeedomter::showTimeline( bool bShowTimeline )
{
    this->m_dwFlags |= FLAG_SHOW_TIMELINE;

    return;
}

//================================================================================================= E. GET/SET
//================================================================================================= EOF
