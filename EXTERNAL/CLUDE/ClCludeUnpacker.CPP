//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClCludeUnpacker.CPP
//
//	Description: CLENA CLUDE Unpacker
//
//	Update History
// 
//		2011.10.10 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. INCLUDE

#include <stdafx.h>
#include "ClCludeUnpacker.H"

#include <HELPER/CLUtil.H>

//================================================================================================= E. INCLUDE
//================================================================================================= S. CON/DCON

ClCludeUnPacker::ClCludeUnPacker()
{

}


ClCludeUnPacker::~ClCludeUnPacker()
{
	this->deinit();
}

//================================================================================================= E. CON/DCON
//================================================================================================= S. INIT/DEINIT

BOOL ClCludeUnPacker::init( const char* lpszCludePath )
{
	FILE* fpClude;
	if( lpszCludePath )
	{
		fopen_s( &fpClude, lpszCludePath, "rb" );
	}
	else
	{
		CHAR lpszPath[MAX_PATH];
		::GetModuleFileNameA( NULL, lpszPath, MAX_PATH );
		std::string stsPath = ClUtil::PATH::getPathSpec( lpszPath );
		stsPath += "\\";
		stsPath += CLUDE_FILE_NAME;

		fopen_s( &fpClude, stsPath.c_str(), "rb" );
	}

	if( !fpClude ) return FALSE;

	LONG nFileSize = 0;
	LONG nCalcFileSize = 0;
	fseek( fpClude, 0, SEEK_END );
	nFileSize = ftell( fpClude );
	fseek( fpClude, 0, SEEK_SET );

	INT nReadSize = fread( &this->m_CludeHeader, sizeof(HEADER), 1, fpClude );
	if( !nReadSize ) return FALSE;

	if(		this->m_CludeHeader.m_strName[0] != 'C'
		||	this->m_CludeHeader.m_strName[1] != 'L'
		||	this->m_CludeHeader.m_strName[2] != 'U'
		||	this->m_CludeHeader.m_strName[3] != 'D'
		||	this->m_CludeHeader.m_strName[4] != 'E' )
	{
		return FALSE;
	}

	// Version check
	// Format check

	CONST INT XMLCounter	= this->m_CludeHeader.m_nXMLCounter;
	CONST INT ImageCounter	= this->m_CludeHeader.m_nImageCounter;

	if( XMLCounter <= 0 || ImageCounter <= 0 )
		return FALSE;

	XMLHEADER xmlHeader;
	INT nIndex;
	for( nIndex = 0; nIndex < XMLCounter; nIndex ++ )
	{
		nReadSize = fread( &xmlHeader, sizeof(XMLHEADER), 1, fpClude );
		if( !nReadSize )
			ASSERT(1);

		this->m_XMLHeaderList.push_back( xmlHeader );
		nCalcFileSize += xmlHeader.m_nSize;
	}

	IMAGEHEADER imageHeader;
	for( nIndex = 0; nIndex < ImageCounter; nIndex ++ )
	{
		nReadSize = fread( &imageHeader, sizeof(IMAGEHEADER), 1, fpClude );
		if( !nReadSize )
			ASSERT(1);

		this->m_ImageHeaderList.push_back( imageHeader );
		nCalcFileSize += imageHeader.m_nSize;
	}

	nCalcFileSize += sizeof(HEADER);
	nCalcFileSize += sizeof(XMLHEADER) * this->m_XMLHeaderList.size();
	nCalcFileSize += sizeof(IMAGEHEADER) * this->m_ImageHeaderList.size();

	if( nCalcFileSize != nFileSize )
		return FALSE;

	ITER_XMLHEADER iXMLHeader = this->m_XMLHeaderList.begin();
	ITER_XMLHEADER iXMLHeaderEnd = this->m_XMLHeaderList.end();
	for( ; iXMLHeader != iXMLHeaderEnd; iXMLHeader ++ )
	{
		XMLFILE xmlFile;
		xmlFile.m_stsFileName = iXMLHeader->m_strName;
		xmlFile.m_pFileContent = new char[iXMLHeader->m_nSize + 1];
		nReadSize = fread( xmlFile.m_pFileContent, iXMLHeader->m_nSize, 1, fpClude );
		xmlFile.m_pFileContent[iXMLHeader->m_nSize] = '\0';
		if( nReadSize )
			this->m_XMLFileList.push_back( xmlFile );
	}

	ITER_IMAGEHEADER iImageHader = this->m_ImageHeaderList.begin();
	ITER_IMAGEHEADER iImageHaderEnd = this->m_ImageHeaderList.end();
	for( ; iImageHader != iImageHaderEnd; iImageHader ++ )
	{
		IMAGEFILE imageFile;
		WCHAR* lptzName = NULL;
		ClUtil::STRING::convertUTF8ToUnicode( iImageHader->m_strName, &lptzName );
		imageFile.m_wtsFilePath = lptzName;
		imageFile.m_nSize = iImageHader->m_nSize;
		imageFile.m_pBuffer = new char[imageFile.m_nSize];
		nReadSize = fread( imageFile.m_pBuffer, imageFile.m_nSize, 1, fpClude );
		if( nReadSize )
			this->m_ImageFileList.push_back( imageFile );

		if( lptzName ) delete [] lptzName;
	}

	fclose( fpClude );

	return TRUE;
}


VOID ClCludeUnPacker::deinit( VOID )
{
	this->clearXmlFileList();
	this->clearImageFileList();

	return;
}

//================================================================================================= E. INIT/DEINIT
//================================================================================================= S. OPERATION


//================================================================================================= E. OPERATION
//================================================================================================= S. CONTROL


//================================================================================================= E. CONTROL
//================================================================================================= S. GET/SET


BOOL ClCludeUnPacker::isEmptyXMLFileList( VOID )
{
	return this->m_XMLFileList.empty();
}


INT ClCludeUnPacker::getXMLFileListSize( VOID )
{
	return this->m_XMLFileList.size();
}


const char* ClCludeUnPacker::getXMLFile( const char* lpszFileName )
{
	if( !lpszFileName )
		return NULL;

	ITER_XMLFILE iXMLFile = this->m_XMLFileList.begin();
	ITER_XMLFILE iXMLFileEnd = this->m_XMLFileList.end();
	for( ; iXMLFile != iXMLFileEnd; iXMLFile ++ )
	{
		if( !iXMLFile->m_stsFileName.compare(lpszFileName) )
			break;
	}

	if( iXMLFile == iXMLFileEnd )
		return NULL;

	return iXMLFile->m_pFileContent;
}


VOID ClCludeUnPacker::clearXmlFileList( VOID )
{
	if( this->m_XMLFileList.empty() )
		return;

	ITER_XMLFILE iXMLFile = this->m_XMLFileList.begin();
	ITER_XMLFILE iXMLFileEnd = this->m_XMLFileList.end();
	for( ; iXMLFile != iXMLFileEnd; iXMLFile++ )
	{
		if( iXMLFile->m_pFileContent )
			delete [] iXMLFile->m_pFileContent;
	}

	this->m_XMLFileList.clear();
}


BOOL ClCludeUnPacker::isEmptyImageFileList( VOID )
{
	return this->m_ImageFileList.empty();
}


INT ClCludeUnPacker::getImageFileListSize( VOID )
{
	return this->m_ImageFileList.size();
}


PVOID ClCludeUnPacker::getImageFile( INT nIndex, HashedString& hashedPath, INT* pSize )
{
	if( nIndex < 0 )
		return NULL;

	ITER_IMAGEFILE iImageFile = this->m_ImageFileList.begin();
	std::advance( iImageFile, nIndex );

	if( iImageFile == this->m_ImageFileList.end() )
		return NULL;

	if( pSize ) *pSize = iImageFile->m_nSize;
	hashedPath = iImageFile->m_wtsFilePath.c_str();

	return iImageFile->m_pBuffer;
}


VOID ClCludeUnPacker::clearImageFileList( VOID )
{
	if( this->m_ImageFileList.empty() )
		return;

	ITER_IMAGEFILE iImageFile = this->m_ImageFileList.begin();
	ITER_IMAGEFILE iImageFileEnd = this->m_ImageFileList.end();
	for( ; iImageFile != iImageFileEnd; iImageFile ++ )
	{
		if( iImageFile->m_pBuffer )
			delete [] iImageFile->m_pBuffer;
	}

	this->m_ImageFileList.clear();

	return;
}

//================================================================================================= E. GET/SET
//================================================================================================= EOF

