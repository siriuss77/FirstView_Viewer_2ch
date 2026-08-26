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
//================================================================================================= S. FILEID

#pragma once

//================================================================================================= E. FILEID
//================================================================================================= S. INCLUDE

#include <HELPER/ClHashedString.H>
#include <HELPER/CLUtil.H>
#include <CLUDE/ClCludeUnpacker.H>

#include <map>
#include <string>
#include <GdiPlus.h>
using namespace Gdiplus;

//================================================================================================= E. INCLUDE
//================================================================================================= S. PREPROCESSOR

//================================================================================================= E. PREPROCESSOR
//================================================================================================= S. TYPEDEF

typedef struct _tagCLIMAGE
{
	Image*	m_pImage;
	int		m_nRefCount;

} CLIMAGE, *PCLIMAGE;

typedef std::map<HashedString,CLIMAGE>					IMAGE_POOL;
typedef IMAGE_POOL::iterator							ITER_IMAGE;
typedef std::pair<HashedString,CLIMAGE>					PAIR_IMAGE;

//================================================================================================= E. TYPEDEF
//================================================================================================= S. CLASS

class ClImagePool
{

public:

	ClImagePool ();
	~ClImagePool ();

public:

	inline static ClImagePool* __fastcall getInstance()
	{
		ClUtil::ClAutoMutex _l( &ClImagePool::m_ImagePoolCS );
		if( !ClImagePool::m_pImagePool )
		{
			ClImagePool::m_pImagePool = new ClImagePool();
			ClImagePool::m_pImagePool->init();
		}
		return ClImagePool::m_pImagePool;
	}

	inline static void releaseInstance()
	{
		ClUtil::ClAutoMutex _l( &ClImagePool::m_ImagePoolCS );
		if( ClImagePool::m_pImagePool )
		{
			ClImagePool::m_pImagePool->deinit();
			delete ClImagePool::m_pImagePool;
			ClImagePool::m_pImagePool = NULL;
		}
		return;
	}


public:

			bool					init				( void );
			void					deinit				( void );

public:
			bool					loadCludePack		( ClCludeUnPacker* pCludePack );
			Image*					loadImage			( const HashedString& imagePath );
			bool					unloadImage			( const HashedString& imagePath );
			bool					unloadImage			( Image* pImage );

public:

			bool					isEmptyImagePool	( void );
			int						getImagePoolSize	( void );
			Image*					getImage			( const HashedString& imagePath );
			Image*					getImage			( int nIndex );
			bool					removeImage			( const HashedString& imagePath );
			bool					removeImage			( int nIndex );
			void					clearImages			( void );

			bool					setBasePath			( const std::wstring imageBasePath );
			const std::wstring		getBasePath			( void );

public:

	static	ClImagePool*			m_pImagePool;
	static	ClUtil::ClMutex			m_ImagePoolCS;


private:

			IMAGE_POOL				m_ImagePool;
			std::wstring			m_wtsImagePath;
};

//================================================================================================= E. CLASS


