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
//================================================================================================= S. INCLUDE

#include "wbcCompass.h"
#include "wbcTextData.h"
#include "wbcMemDC.h"

//================================================================================================= E. INCLUDE
IMPLEMENT_DYNAMIC(CWBCCompass, CStatic)
//================================================================================================= S. CON/DCON

CWBCCompass::CWBCCompass()
{
    this->m_eCurAction = ACTION_ROTATE_NEEDLE;
    for( int nIndex = SKIN_NEEDLE; nIndex < SKIN_INVALID; nIndex ++ )
        this->m_pImageRender[nIndex] = NULL;

    this->m_pSensorData = NULL;
}


CWBCCompass::~CWBCCompass()
{
    for( int nIndex = SKIN_NEEDLE; nIndex < SKIN_INVALID; nIndex ++ )
        if( this->m_pImageRender[nIndex] != NULL )
            delete this->m_pImageRender[nIndex];

    if( this->m_dcBackground.m_hDC != NULL )
        this->m_dcBackground.DeleteDC();
}

//================================================================================================= E. CON/DCON
//================================================================================================= S. VIRTUAL

void CWBCCompass::TransferBackground( CDC* pDC )
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


void CWBCCompass::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
    CDC drawItemDC;
    drawItemDC.Attach( lpDrawItemStruct->hDC );

    CRect rect;
    GetClientRect(rect);
    CWBCMemDC memDC( &drawItemDC, &rect );

    REAL angle = 0;
    if( this->m_pSensorData )
    {
        int nAdvance = this->m_dwCTS / 1000;
        if( nAdvance < (int)this->m_pSensorData->m_GPSList.size() )
        {
            CWBCTextData::ITER_GPS iGPSData = this->m_pSensorData->m_GPSList.begin();
            std::advance( iGPSData, nAdvance );
            if( iGPSData != this->m_pSensorData->m_GPSList.end() )
                angle = (REAL)(0 - iGPSData->m_fAzimuth);
        }
    }

    if( this->m_pImageRender[SKIN_METER] )
    {
        if( this->m_eCurAction == ACTION_ROTATE_METER )
        {
            this->m_pImageRender[SKIN_METER]->setRotateAngel( 0 - angle );
            this->m_pImageRender[SKIN_METER]->setUpdate();
            this->m_pImageRender[SKIN_METER]->prepareImage( &memDC, rect, &this->m_dcBackground, TRUE );
        }

        this->m_pImageRender[SKIN_METER]->drawImage( &memDC, rect );
    }

    if( this->m_pImageRender[SKIN_NEEDLE] )
    {
        if( this->m_eCurAction == ACTION_ROTATE_NEEDLE )
        {
            this->m_pImageRender[SKIN_NEEDLE]->setRotateAngel( angle );
            this->m_pImageRender[SKIN_NEEDLE]->setUpdate();
            this->m_pImageRender[SKIN_NEEDLE]->prepareImage( &memDC, rect, &this->m_dcBackground, TRUE );
        }

        this->m_pImageRender[SKIN_NEEDLE]->drawImage( &memDC, rect );
    }

    return;
}

//================================================================================================= E. VIRTUAL

BEGIN_MESSAGE_MAP(CWBCCompass, CStatic)
END_MESSAGE_MAP()

//================================================================================================= S. MESSAGE


//================================================================================================= E. MESSAGE
//================================================================================================= S. INIT/DEINIT


//================================================================================================= E. INIT/DEINIT
//================================================================================================= S. GET/SET

CWBCCompass::EACTION CWBCCompass::getAction( void )
{
    return this->m_eCurAction;
}


bool CWBCCompass::setAction( EACTION eAction )
{
    if( eAction == ACTION_INVALID )
        return false;

    this->m_eCurAction = eAction;

    return true;
}


bool CWBCCompass::setSkin( ESKIN_TYPE eSkin, UINT uResourceID, const Color& color, bool bUseTpColor, const Color& tpColor,
                          const ClGeometry& geoImage, const ClGeometry& geoSubImage, DWORD dwGeoBase )
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


void CWBCCompass::synchronize( DWORD dwCTS )
{
    this->m_dwCTS = dwCTS;
    this->InvalidateRect( NULL );

    return;
}


BOOL CWBCCompass::SetSensorData( CWBCTextData* pSensorData )
{
    this->m_pSensorData = pSensorData;
    this->Invalidate();

    return TRUE;
}

//================================================================================================= E. GET/SET
