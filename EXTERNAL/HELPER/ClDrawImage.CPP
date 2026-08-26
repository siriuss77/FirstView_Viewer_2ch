//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClDrawImage.CPP
//
//	Description: CLENAW GID+ image drawing class
//
//	Update History
// 
//		2011.10.10 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. INCLUDE

#include <stdafx.h>
#include "ClDrawImage.H"

#include <HELPER/ClLog.H>

//================================================================================================= E. INCLUDE
//================================================================================================= S. CON/DCON

ClDrawImage::ClDrawImage()
{
	this->m_pImage			= NULL;

	this->m_bUseMask		= FALSE;

	this->m_color			= Color((ARGB)Color::White);
	this->m_drawMode		= MODE_STRETCH;
	this->m_geoImage		= GEO_FULL_SIZE;
	this->m_geoSubImage		= GEO_FULL_SIZE;

	this->m_bUseColorKey	= false;
	this->m_ColorKey		= Color(0,0,0);
	this->m_bIsPrepared		= FALSE;
}


ClDrawImage::~ClDrawImage()
{
	this->deinit();
}

//================================================================================================= E. CON/DCON
//================================================================================================= S. INIT/DEINIT

void ClDrawImage::deinit( void )
{
	if( this->m_pImage ) ClImagePool::getInstance()->unloadImage( this->m_ImagePath );

	this->m_dcImage.DeleteDC();

	return;
}

//================================================================================================= E. INIT/DEINIT
//================================================================================================= S. OPERATION

BOOL ClDrawImage::loadImage( const HashedString& imagePath, Color color, bool bUseTpColor, Color tpColor, DRAW_MODE drawMode, 
								const ClGeometry& geoImage, const ClGeometry& geoSubImage, DWORD dwGeoBase )
{
	this->deinit();

	this->m_ImagePath = imagePath;
	if( !this->m_ImagePath.isNULL() )
	{
		this->m_pImage = ClImagePool::getInstance()->loadImage( this->m_ImagePath );
		if( !this->m_pImage )
			return false;
	}

	this->m_color			= color;
	this->m_bUseColorKey	= bUseTpColor;
	this->m_ColorKey		= tpColor;
	this->m_drawMode		= drawMode;
	this->m_geoImage		= geoImage;
	this->m_geoSubImage		= geoSubImage;
	this->m_dwGeoBase		= dwGeoBase;

	return true;
}


VOID ClDrawImage::prepareImage( CDC* pDC, const CRect& rtClient, CDC* pBackground, BOOL bUseMask )
{
	if( !pDC || !pDC->m_hDC )
		return;

	if( this->m_bIsPrepared )
		return;

	ClGeometry geoTemp = this->m_geoImage;

	if(this->m_drawMode != MODE_MULTIPLE)
		ClUtil::GEOMETRY::recalcGeometry( geoTemp, rtClient.Width(), rtClient.Height(), this->m_dwGeoBase );

	CBitmap memBitmap;
	CDC memDC;
	memDC.CreateCompatibleDC( pDC );
	memBitmap.CreateCompatibleBitmap( pDC, geoTemp.width, geoTemp.height );
	memDC.SelectObject( &memBitmap );

	if( !this->m_dcImage.m_hDC )
	{
		if( pBackground )
			memDC.BitBlt( 0, 0, geoTemp.width, geoTemp.height, pBackground, this->m_geoImage.x, this->m_geoImage.y, SRCCOPY );

		Graphics graphics( memDC );
		Rect rtDest = Rect( 0, 0, geoTemp.width, geoTemp.height );

		if( this->m_pImage )
		{
			ImageAttributes imageAttributes;
			Rect			imageSubRect;

			REAL alpha	= (REAL)( this->m_color.GetAlpha()) / 255;
			REAL red	= (REAL)( this->m_color.GetRed())	/ 255;
			REAL green	= (REAL)( this->m_color.GetGreen())	/ 255;
			REAL blue	= (REAL)( this->m_color.GetBlue())	/ 255;

			ColorMatrix BitmapMatrix =
			{
				red, 0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, green, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, blue, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, alpha, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f, 1.0f
			};

			imageSubRect = Rect( this->m_geoSubImage.x, this->m_geoSubImage.y, this->m_geoSubImage.width, this->m_geoSubImage.height );

			if( imageSubRect.Width == GEO_MAX )	imageSubRect.Width = this->m_pImage->GetWidth();
			if( imageSubRect.Height == GEO_MAX )imageSubRect.Height = this->m_pImage->GetHeight();

			imageAttributes.SetColorMatrix( &BitmapMatrix, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap );
			if( bUseMask )
			{
				this->m_bUseMask = TRUE;
			}
			else
			{
				this->m_bUseMask = FALSE;
				if( this->m_bUseColorKey ) imageAttributes.SetColorKey( this->m_ColorKey, this->m_ColorKey, ColorAdjustTypeBitmap );
			}

			switch( this->m_drawMode )
			{
			case MODE_STRETCH :
			case MODE_MULTIPLE:
				{
					graphics.DrawImage( this->m_pImage, rtDest, imageSubRect.X, imageSubRect.Y, imageSubRect.Width, imageSubRect.Height,
						UnitPixel, &imageAttributes );
				} break;

			case MODE_PATTERN :
				{
					INT nCropWidth, nCropHeight;
					nCropWidth = nCropHeight = 0;

					for( INT nXPos = rtDest.X; nXPos < rtDest.GetRight(); nXPos += (imageSubRect.Width) )
					{
						nCropWidth = nCropHeight = 0;
						if( nXPos > rtDest.GetRight() ) nCropWidth = rtDest.Width % (imageSubRect.Width);

						for( INT nYPos = rtDest.Y; nYPos < rtDest.GetBottom(); nYPos += (imageSubRect.Height) )
						{
							if( nYPos > rtDest.GetBottom() ) nCropHeight = rtDest.Width % (imageSubRect.Width);

							graphics.DrawImage( this->m_pImage, Rect( nXPos, nYPos, ((imageSubRect.Width) - nCropWidth), (imageSubRect.Height) - nCropHeight ),
								imageSubRect.X, imageSubRect.Y, imageSubRect.Width - nCropWidth, imageSubRect.Height - nCropHeight, UnitPixel, &imageAttributes );
						}
					}

				} break;

			case MODE_BUTTONFACE :
				{
					int nCornerWidth = min(rtDest.Width,imageSubRect.Width) / 2;
					int nCornerHeight = min(rtDest.Height,imageSubRect.Height) /2;

					// Draw image 4 corners
					graphics.DrawImage( this->m_pImage, Rect( rtDest.X, rtDest.Y, nCornerWidth, nCornerHeight),
						imageSubRect.X, imageSubRect.Y, nCornerWidth, nCornerHeight, UnitPixel, &imageAttributes );

					graphics.DrawImage( this->m_pImage, Rect( rtDest.X + rtDest.Width - (nCornerWidth), rtDest.Y, nCornerWidth, nCornerHeight),
						imageSubRect.X + imageSubRect.Width-nCornerWidth,imageSubRect.Y,nCornerWidth, nCornerHeight, UnitPixel, &imageAttributes );

					graphics.DrawImage( this->m_pImage, Rect( rtDest.X, rtDest.Y + rtDest.Height - nCornerHeight, nCornerWidth, nCornerHeight),
						imageSubRect.X, imageSubRect.Y + imageSubRect.Height - nCornerHeight, nCornerWidth, nCornerHeight, UnitPixel, &imageAttributes );

					graphics.DrawImage( this->m_pImage, Rect( rtDest.X + rtDest.Width - nCornerWidth, rtDest.Y + rtDest.Height - nCornerHeight, nCornerWidth, nCornerHeight),
						imageSubRect.X + imageSubRect.Width - nCornerWidth, imageSubRect.Y + imageSubRect.Height - nCornerHeight, nCornerWidth, nCornerHeight, UnitPixel, &imageAttributes );

					// Draw Middle
					graphics.DrawImage( this->m_pImage, Rect( rtDest.X + nCornerWidth, rtDest.Y, rtDest.Width-((nCornerWidth<<1)), nCornerHeight),
						imageSubRect.X + nCornerWidth, imageSubRect.Y, 1, nCornerHeight, UnitPixel, &imageAttributes );

					graphics.DrawImage( this->m_pImage, Rect( rtDest.X, rtDest.Y + nCornerHeight, nCornerWidth, rtDest.Height - ((nCornerHeight<<1))),
						imageSubRect.X, imageSubRect.Y+nCornerHeight, nCornerWidth, 1, UnitPixel, &imageAttributes );

					graphics.DrawImage( this->m_pImage, Rect( rtDest.X + nCornerWidth, rtDest.Y + nCornerHeight, rtDest.Width-(nCornerWidth<<1), rtDest.Height-((nCornerHeight<<1))),
						imageSubRect.X + nCornerWidth, imageSubRect.Y+nCornerHeight, 1, 1, UnitPixel, &imageAttributes );

					graphics.DrawImage( this->m_pImage, Rect( rtDest.X + rtDest.Width-nCornerWidth, rtDest.Y+nCornerHeight, nCornerWidth, rtDest.Height-((nCornerHeight<<1))),
						imageSubRect.X + imageSubRect.Width-nCornerWidth, imageSubRect.Y+nCornerHeight, nCornerWidth, 1, UnitPixel, &imageAttributes );

					graphics.DrawImage( this->m_pImage, Rect( rtDest.X + nCornerWidth, rtDest.Y+ rtDest.Height-nCornerHeight, rtDest.Width-((nCornerWidth<<1)), nCornerHeight),
						imageSubRect.X + nCornerWidth, imageSubRect.Y+imageSubRect.Height-nCornerHeight, 1, nCornerHeight, UnitPixel, &imageAttributes );

				} break;
			}
		}
		else
		{
			SolidBrush whitebrush( this->m_color );
			graphics.FillRectangle( &whitebrush, rtDest );
		}

		CBitmap compBitmap, *pOldBitmap;
		this->m_dcImage.CreateCompatibleDC( pDC );
		compBitmap.CreateCompatibleBitmap( pDC, geoTemp.width, geoTemp.height );
		pOldBitmap = this->m_dcImage.SelectObject( &compBitmap );
		this->m_dcImage.BitBlt( 0, 0, geoTemp.width, geoTemp.height, &memDC, 0, 0, SRCCOPY );
		memBitmap.DeleteObject();
		memDC.DeleteDC();
	}

	this->m_bIsPrepared = TRUE;

	return;
}


VOID ClDrawImage::drawImage( CDC* pDC, const CRect& rtClient, ClGeometry* pGeoTarget, ClGeometry* pGeoSource )
{
	if( !this->m_bIsPrepared )
		return;

	ClGeometry geoTemp = this->m_geoImage;
	ClUtil::GEOMETRY::recalcGeometry( geoTemp, rtClient.Width(), rtClient.Height(), this->m_dwGeoBase );
	ClGeometry geoTarget = geoTemp;
	ClGeometry geoSource = ClGeometry( 0, 0, geoTemp.width, geoTemp.height );

	if( pGeoTarget )	geoTarget = *pGeoTarget;
	if( pGeoSource )	geoSource = *pGeoSource;

	if( this->m_bUseMask && this->m_bUseColorKey )
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

//================================================================================================= E. OPERATION
//================================================================================================= S. CONTROL

//================================================================================================= E. CONTROL
//================================================================================================= S. GET/SET

BOOL ClDrawImage::setGeometry( const ClGeometry& geoImage )
{
	if( this->m_geoImage == geoImage )
		return false;

	this->m_geoImage = geoImage;

	this->setUpdate();

	return true;
}


BOOL ClDrawImage::setGeometry( INT x, INT y, INT width, INT height )
{
	return this->setGeometry( ClGeometry(x,y,width,height) );
}


const ClGeometry& ClDrawImage::getGeometry( VOID )
{
	return this->m_geoImage;
}


DWORD ClDrawImage::getGeoBase( VOID )
{
	return this->m_dwGeoBase;
}


CDC* ClDrawImage::getImageDC( VOID )
{
	return &this->m_dcImage;
}


VOID ClDrawImage::setUpdate( VOID )
{
	this->m_dcImage.DeleteDC();
	this->m_bIsPrepared	= FALSE;

	return;
}

//================================================================================================= E. GET/SET
//================================================================================================= S. STATIC

//================================================================================================= E. STATIC
//================================================================================================= EOF
