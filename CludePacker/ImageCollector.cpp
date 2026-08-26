
#include "StdAfx.h"
#include "ImageCollector.h"
#include <HELPER/CLUtil.H>


CImageCollector::CImageCollector(void)
{
}


CImageCollector::~CImageCollector(void)
{
	this->clearImageInfo();
}


BOOL CImageCollector::isEmptyImageList( VOID )
{
	return this->m_ImageList.empty();
}


INT CImageCollector::getImageListSize( VOID )
{
	return this->m_ImageList.size();
}


BOOL CImageCollector::addImagePath( const HashedString& hashedPath )
{
	if( hashedPath.isNULL() )
		return FALSE;

	ITER_COLLECTIMAGE iImage = this->m_ImageList.find( hashedPath );
	if( iImage == this->m_ImageList.end() )
	{
		CString cstrPath = this->m_cstrImageRoot + hashedPath.c_str();
		CString cstrValidPath;
		ClUtil::PATH::makeValidPath( cstrPath, cstrValidPath );

		IMAGEINFO imageInfo;
		imageInfo.m_wtsFilePath	= cstrValidPath;
		imageInfo.m_nSize		= 0;
		imageInfo.m_bIsLoad		= FALSE;
		imageInfo.m_pBuffer		= NULL;

		this->m_ImageList.insert( PAIR_COLLECTIMAGE(hashedPath,imageInfo) );
	}

	return TRUE;
}


CImageCollector::PIMAGEINFO CImageCollector::getImageInfo( INT nIndex )
{
	if( nIndex < 0 )
		return NULL;

	ITER_COLLECTIMAGE iImage = this->m_ImageList.begin();
	std::advance( iImage, nIndex );
	if( iImage == this->m_ImageList.end() )
		return NULL;

	return &iImage->second;
}


BOOL CImageCollector::getImageHash( INT nIndex, HashedString& imageHash )
{
	if( nIndex < 0 )
		return FALSE;

	ITER_COLLECTIMAGE iImage = this->m_ImageList.begin();
	std::advance( iImage, nIndex );
	if( iImage == this->m_ImageList.end() )
		return FALSE;

	imageHash = iImage->first;

	return TRUE;
}


BOOL CImageCollector::removeImageInfo( const HashedString& hashedPath )
{
	if( hashedPath.isNULL() )
		return FALSE;

	ITER_COLLECTIMAGE iImage = this->m_ImageList.find( hashedPath );
	if( iImage == this->m_ImageList.end() )
		return false;

	if( iImage->second.m_pBuffer )
		delete [] iImage->second.m_pBuffer;

	this->m_ImageList.erase( iImage );

	return TRUE;
}


VOID CImageCollector::clearImageInfo( VOID )
{
	if( this->m_ImageList.empty() )
		return;

	ITER_COLLECTIMAGE iImage = this->m_ImageList.begin();
	ITER_COLLECTIMAGE iImageEnd = this->m_ImageList.end();
	for( ; iImage != iImageEnd; iImage ++ )
	{
		if( iImage->second.m_pBuffer )
			delete [] iImage->second.m_pBuffer;
	}

	this->m_ImageList.clear();

	return;
}


BOOL CImageCollector::setImageRoot( const CString& cstrImageRoot )
{
	this->m_cstrImageRoot = cstrImageRoot;
	this->m_cstrImageRoot += TEXT("\\");

	return TRUE;
}

