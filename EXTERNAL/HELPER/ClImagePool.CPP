//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClImagePool.h
//
//	Description: CLENA Image Pool
//
//	Update History
// 
//		2011.10.10 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. INCLUDE

#include <stdafx.h>
#include "ClImagePool.H"
#include <HELPER/ClLog.H>
#include <CLENAW/ClMacro.H>

//================================================================================================= E. INCLUDE
//================================================================================================= S. GLOBAL

ClImagePool*		ClImagePool::m_pImagePool	= NULL;
ClUtil::ClMutex		ClImagePool::m_ImagePoolCS;

//================================================================================================= E. GLOBAL
//================================================================================================= S. CON/DCON

ClImagePool::ClImagePool()
{

}


ClImagePool::~ClImagePool()
{

}

//================================================================================================= E. CON/DCON
//================================================================================================= S. INIT/DEINIT

bool ClImagePool::init()
{
	return true;
}


void ClImagePool::deinit()
{
	return;
}

//================================================================================================= S. INIT/DEINIT

//================================================================================================= E. INIT/DEINIT
//================================================================================================= S. OPERATION
bool ClImagePool::loadCludePack( ClCludeUnPacker* pCludePack )
{
	if( !pCludePack )
		return false;

	INT nSize;
	HashedString hashedPath;

	CONST INT nImageListSize = pCludePack->getImageFileListSize();
	INT nIndex;
	for( nIndex = 0; nIndex < nImageListSize; nIndex ++ )
	{
		PVOID pBuffer = pCludePack->getImageFile( nIndex, hashedPath, &nSize );
		if( !pBuffer ) continue;

		IStream* pStream = NULL;
		HRESULT hr = CreateStreamOnHGlobal( NULL, TRUE, &pStream );
		if( SUCCEEDED(hr) )
		{
			ULONG act = 0;
			hr = pStream->Write( pBuffer, nSize, &act );
			if( SUCCEEDED(hr) )
			{
				CLIMAGE image = { NULL, 1 };
				image.m_pImage = Image::FromStream( pStream );
				if( !VALID_GDIPLUS_OBJECT(image.m_pImage) )
				{
					CLDW_FL( DF_ERROR, TEXT("Can not load image(%s)"), hashedPath.c_str() );
					continue;
				}
				this->m_ImagePool.insert( PAIR_IMAGE(hashedPath,image) );
			}
		}
	}

	return true;
}


Image* ClImagePool::loadImage( const HashedString& imagePath )
{
	if( imagePath.isNULL() )
		return false;

	ClUtil::ClAutoMutex _l( &this->m_ImagePoolCS );

	CString cstrValidImagePath;
	CString cstrImagePath;
	ITER_IMAGE iImage = this->m_ImagePool.find( imagePath );
	if( iImage == this->m_ImagePool.end() )
	{
		CLIMAGE image = { NULL, 1 };

		cstrImagePath = this->m_wtsImagePath.c_str();
		cstrImagePath += imagePath.c_str();

		ClUtil::PATH::makeValidPath( cstrImagePath, cstrValidImagePath );

		image.m_pImage = new Image( cstrValidImagePath );
		if( !VALID_GDIPLUS_OBJECT(image.m_pImage) )
		{
			if( image.m_pImage )
			{
				delete image.m_pImage;
				image.m_pImage = NULL;
			}
			CLDW_FL( DF_ERROR, TEXT("Can not load image(%s)"), imagePath.c_str() );
			return false;
		}

		this->m_ImagePool.insert( PAIR_IMAGE(imagePath,image) );
		return image.m_pImage;
	}

	iImage->second.m_nRefCount ++;
	return iImage->second.m_pImage;
}


bool ClImagePool::unloadImage( const HashedString& imagePath )
{
	ClUtil::ClAutoMutex _l( &this->m_ImagePoolCS );

	ITER_IMAGE iImage = this->m_ImagePool.find( imagePath );
	if( iImage == this->m_ImagePool.end() )
		return false;

	iImage->second.m_nRefCount --;
	if( iImage->second.m_nRefCount < 0 )
	{
		delete iImage->second.m_pImage;
		this->m_ImagePool.erase( iImage );
	}

	return true;
}


bool ClImagePool::unloadImage( Image* pImage )
{
	if( !pImage )
		return false;

	ClUtil::ClAutoMutex _l( &this->m_ImagePoolCS );

	ITER_IMAGE iImage = this->m_ImagePool.begin();
	ITER_IMAGE iImageEnd = this->m_ImagePool.end();
	for( ; iImage != iImageEnd; iImage ++ )
	{
		if( iImage->second.m_pImage == pImage )
			break;
	}

	if( iImage == iImageEnd )
		return false;

	iImage->second.m_nRefCount --;
	if( iImage->second.m_nRefCount < 0 )
	{
		delete iImage->second.m_pImage;
		this->m_ImagePool.erase( iImage );
	}

	return true;
}

//================================================================================================= E. OPERATION
//================================================================================================= S. CONTROL

bool ClImagePool::removeImage( const HashedString& imagePath )
{
	ClUtil::ClAutoMutex _l( &this->m_ImagePoolCS );

	ITER_IMAGE iImage = this->m_ImagePool.find( imagePath );
	if( iImage == this->m_ImagePool.end() )
		return false;

	if( iImage->second.m_pImage )
		delete iImage->second.m_pImage;

	this->m_ImagePool.erase( iImage );

	return true;
}


bool ClImagePool::removeImage( int nIndex )
{
	ClUtil::ClAutoMutex _l( &this->m_ImagePoolCS );

	ITER_IMAGE iImage = this->m_ImagePool.begin();
	std::advance( iImage, nIndex );
	if( iImage == this->m_ImagePool.end() )
		return false;

	if( iImage->second.m_pImage )
		delete iImage->second.m_pImage;

	this->m_ImagePool.erase( iImage );

	return true;
}


void ClImagePool::clearImages( void )
{
	ClUtil::ClAutoMutex _l( &this->m_ImagePoolCS );

	ITER_IMAGE iImage = this->m_ImagePool.begin();
	ITER_IMAGE iImageEnd = this->m_ImagePool.end();
	for( ; iImage != iImageEnd; iImage ++ )
	{
		if( iImage->second.m_pImage )
			delete iImage->second.m_pImage;
	}

	this->m_ImagePool.clear();
	return;
}

//================================================================================================= E. CONTROL
//================================================================================================= S. GET/SET

bool ClImagePool::setBasePath( const std::wstring imageBasePath )
{
	if( !imageBasePath.length() )
		return false;

	this->m_wtsImagePath = imageBasePath;
	return true;
}


const std::wstring ClImagePool::getBasePath( void )
{
	return this->m_wtsImagePath;
}

bool ClImagePool::isEmptyImagePool( void )
{
	ClUtil::ClAutoMutex _l( &this->m_ImagePoolCS );

	return this->m_ImagePool.empty();
}


int ClImagePool::getImagePoolSize( void )
{
	ClUtil::ClAutoMutex _l( &this->m_ImagePoolCS );

	return this->m_ImagePool.size();
}


Image* ClImagePool::getImage( const HashedString& imagePath )
{
	ClUtil::ClAutoMutex _l( &this->m_ImagePoolCS );

	ITER_IMAGE iImage = this->m_ImagePool.find( imagePath );
	if( iImage == this->m_ImagePool.end() )
		return NULL;

	return iImage->second.m_pImage;
}


Image* ClImagePool::getImage( int nIndex )
{
	if( nIndex < 0 )
		return NULL;

	ClUtil::ClAutoMutex _l( &this->m_ImagePoolCS );

	ITER_IMAGE iImage = this->m_ImagePool.begin();
	std::advance( iImage, nIndex );
	if( iImage == this->m_ImagePool.end() )
		return NULL;

	return iImage->second.m_pImage;
}

//================================================================================================= E. GET/SET
//================================================================================================= EOF

