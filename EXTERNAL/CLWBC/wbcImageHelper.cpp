//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//  Module: renderHelper.h
//
//  Description: WBC image rendering helper
//
//  Update History
// 
//      2014.10.30 [Allen Han] Initial Release v1.0.00, The last one, Good-bye
//=================================================================================================
//================================================================================================= S. INCLUDE

#include "stdafx.h"
#include "wbcImageHelper.h"

//================================================================================================= E. INCLUDE
//================================================================================================= S. CON/DCON

CWBCImageHelper::CWBCImageHelper()
{
    this->m_pImage          = NULL;
    this->m_color           = Color(255,255,255);
    this->m_bUseColorKey    = FALSE;
    this->m_ColorKey        = Color(255,255,255);
    this->m_RotateAngle     = 0.f;
    this->m_dwGeoMan        = GEOMAN_LEFT | GEOMAN_TOP;
    this->m_bIsPrepared     = false;
}


CWBCImageHelper::~CWBCImageHelper()
{
    this->deinit();
}

//================================================================================================= E. CON/DCON
//================================================================================================= S. INIT/DEINIT

void CWBCImageHelper::deinit( void )
{
    if( this->m_pImage )
        delete this->m_pImage;

    if( this->m_dcImage.m_hDC )
        this->m_dcImage.DeleteDC();

    return;
}

//================================================================================================= E. INIT/DEINIT
//================================================================================================= S. OPERATION

bool CWBCImageHelper::loadImage( UINT nResourceID, const Color& color, bool bUseTpColor, const Color& tpColor,
                              const ClGeometry& geoImage, const ClGeometry& geoSubImage, DWORD dwGeoMan )
{
    if( this->m_pImage != NULL )
        delete this->m_pImage;

    this->m_pImage = Bitmap::FromResource( (HINSTANCE)GetModuleHandle(NULL), MAKEINTRESOURCE(nResourceID) );

    this->m_color           = color;
    this->m_bUseColorKey    = bUseTpColor;
    this->m_ColorKey        = tpColor;
    this->m_geoImage        = geoImage;
    this->m_geoSubImage     = geoSubImage;
    this->m_dwGeoMan        = dwGeoMan;
    this->m_bIsPrepared     = false;

    return true;
}


void CWBCImageHelper::drawImage( CDC* pDC, const CRect& rtClient, ClGeometry* pGeoTarget, ClGeometry* pGeoSource )
{
    if( !this->m_bIsPrepared )
        return;

    ClGeometry geoTarget;
    ClGeometry geoSource;

    vwutil_manGeometry( this->m_geoImage, this->m_dwGeoMan, rtClient.Width(), rtClient.Height(), geoTarget );
    geoSource = ClGeometry( 0, 0, geoTarget.width, geoTarget.height );

    if( pGeoTarget )    geoTarget = *pGeoTarget;
    if( pGeoSource )    geoSource = *pGeoSource;

    if( this->m_bUseColorKey )
    {
        pDC->TransparentBlt( geoTarget.x, geoTarget.y, geoTarget.width, geoTarget.height,
            &this->m_dcImage, geoSource.x, geoSource.y, geoSource.width, geoSource.height,
            RGB(this->m_ColorKey.GetR(), this->m_ColorKey.GetG(), this->m_ColorKey.GetB()) );
    }
    else
    {
        if( pGeoTarget || pGeoSource )
        {
            pDC->StretchBlt( geoTarget.x, geoTarget.y, geoTarget.width, geoTarget.height,
                &this->m_dcImage, geoSource.x, geoSource.y, geoSource.width, geoSource.height, SRCCOPY );
        }
        else
        {
            pDC->BitBlt( geoTarget.x, geoTarget.y, geoTarget.width, geoTarget.height,
                &this->m_dcImage, geoSource.x, geoSource.y, SRCCOPY );
        }
    }

    return;
}


void CWBCImageHelper::prepareImage( CDC* pDC, const CRect& rtClient, CDC* pBackground, BOOL bUseMask )
{
    if( !pDC || !pDC->m_hDC )
        return;

    if( this->m_bIsPrepared )
        return;

    CBitmap memBitmap;
    CDC memDC;
    ClGeometry geoMan;

    vwutil_manGeometry( this->m_geoImage, this->m_dwGeoMan, rtClient.Width(), rtClient.Height(), geoMan );

    memDC.CreateCompatibleDC( pDC );
    memBitmap.CreateCompatibleBitmap( pDC, geoMan.width, geoMan.height );
    memDC.SelectObject( &memBitmap );

    if( !this->m_dcImage.m_hDC )
    {
        if( pBackground )
            memDC.BitBlt( 0, 0, geoMan.width, geoMan.height, pBackground, this->m_geoImage.x, this->m_geoImage.y, SRCCOPY );

        Graphics graphics( memDC );
        Rect rtDest = Rect( 0, 0, geoMan.width, geoMan.height );

        if( this->m_pImage )
        {
            ImageAttributes imageAttributes;
            Rect imageSubRect;

            REAL alpha  = (REAL)( this->m_color.GetAlpha()) / 255;
            REAL red    = (REAL)( this->m_color.GetRed())   / 255;
            REAL green  = (REAL)( this->m_color.GetGreen()) / 255;
            REAL blue   = (REAL)( this->m_color.GetBlue())  / 255;

            ColorMatrix BitmapMatrix =
            {
                red, 0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, green, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, blue, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, alpha, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f, 1.0f
            };

            imageSubRect = Rect( this->m_geoSubImage.x, this->m_geoSubImage.y, this->m_geoSubImage.width, this->m_geoSubImage.height );

            if( imageSubRect.Width == GEO_MAX ) imageSubRect.Width = this->m_pImage->GetWidth();
            if( imageSubRect.Height == GEO_MAX )imageSubRect.Height = this->m_pImage->GetHeight();

            imageAttributes.SetColorMatrix( &BitmapMatrix, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap );
            if( this->m_bUseColorKey )
                imageAttributes.SetColorKey( this->m_ColorKey, this->m_ColorKey, ColorAdjustTypeBitmap );

            if( this->m_RotateAngle != 0.f )
            {
                Matrix rotateMatrix;
                rotateMatrix.RotateAt( this->m_RotateAngle, PointF((REAL)rtClient.Width()/2, (REAL)rtClient.Height()/2) );
                graphics.SetTransform( &rotateMatrix );
            }

            graphics.DrawImage( this->m_pImage, rtDest, imageSubRect.X, imageSubRect.Y, imageSubRect.Width, imageSubRect.Height,
                                UnitPixel, &imageAttributes );
        }
        else
        {
            SolidBrush whitebrush( this->m_color );
            graphics.FillRectangle( &whitebrush, rtDest );
        }

        CBitmap compBitmap, *pOldBitmap;
        this->m_dcImage.CreateCompatibleDC( pDC );
        compBitmap.CreateCompatibleBitmap( pDC, geoMan.width, geoMan.height );
        pOldBitmap = this->m_dcImage.SelectObject( &compBitmap );
        this->m_dcImage.BitBlt( 0, 0, geoMan.width, geoMan.height, &memDC, 0, 0, SRCCOPY );
        memBitmap.DeleteObject();
        memDC.DeleteDC();
    }

    this->m_bIsPrepared = TRUE;

    return;
}


void CWBCImageHelper::setUpdate( void )
{
    this->m_dcImage.DeleteDC();
    this->m_bIsPrepared = FALSE;

    return;
}


void CWBCImageHelper::setRotateAngel( REAL fAngle )
{
    this->m_RotateAngle = fAngle;

    this->setUpdate();

    return;
}

//================================================================================================= E. OPERATION
