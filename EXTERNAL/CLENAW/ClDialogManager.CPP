//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClDialogManager.cpp
//
//	Description: CLENAW Dialog Manager
//
//	Update History
// 
//		2011.10.10 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. INCLUDE

#include <stdafx.h>
#include "ClDialogManager.h"

#include <HELPER/ClLog.H>
#include <CLENAW/ClDialog.H>
#include <CLENAW/ClButton.H>

//================================================================================================= E. INCLUDE
//================================================================================================= S. CON/DCON

ClDialogManager::ClDialogManager()
{
	this->m_pParentDialog	= NULL;

	this->m_wtsTitle		= TEXT("CLENAW Application");

	this->m_alpha			= 255;
	this->m_tpColor			= 0;
	this->m_bUseTPColor		= FALSE;
	this->m_geoDialog		= GEO_ZERO_SIZE;
}


ClDialogManager::~ClDialogManager()
{
	this->ClearBackground();
	this->ClearResource();
}

//================================================================================================= E. CON/DCON
//================================================================================================= S. INIT/DEINIT

//================================================================================================= E. INIT/DEINIT
//================================================================================================= S. OPERATION

BOOL ClDialogManager::SetResourceState( INT nMode, const HashedString& resourceName, BOOL bShow, BOOL bEnable )
{
	ITER_CONTROLINFO iRes = this->m_resourceList.find( resourceName );
	if( iRes == this->m_resourceList.end() )
	{
		CLDW_FL( DF_ERROR, TEXT("Given resource name is invalid(%s)"), resourceName.c_str() );
		return FALSE;
	}

	if( !iRes->second.m_pContainer )
	{
		CLDW_FL( DF_ERROR, TEXT("Given resource name is unregistered(%s)"), resourceName.c_str() );
		return FALSE;
	}

	PMODEINFO pModeInfo = iRes->second.m_pContainer->GetModeInfo( nMode );
	if( !pModeInfo )
		return FALSE;

	pModeInfo->m_bShow		= bShow;
	pModeInfo->m_bEnable	= bEnable;

	return TRUE;
}


VOID ClDialogManager::AdaptResourceState( INT nMode )
{
	ITER_CONTROLINFO iRes		= this->m_resourceList.begin();
	ITER_CONTROLINFO iResEnd	= this->m_resourceList.end();
	for( ; iRes != iResEnd; iRes ++ )
	{
		if( iRes->second.m_pContainer )
			iRes->second.m_pContainer->AdaptMode( nMode );
	}

	return;
}


VOID ClDialogManager::TransferBackground( CDC* pDC )
{
	ITER_CONTROLINFO iRes		= this->m_resourceList.begin();
	ITER_CONTROLINFO iResEnd	= this->m_resourceList.end();
	for( ; iRes != iResEnd; iRes ++ )
	{
		if( iRes->second.m_pContainer && iRes->second.m_pContainer->GetBgInterface() )
			iRes->second.m_pContainer->GetBgInterface()->TransferBackground( pDC );
	}

	return;
}

//================================================================================================= E. OPERATION
//================================================================================================= S. CONTROL

BOOL ClDialogManager::AddResource( const HashedString& resourceName, CONTROL_TYPE type, UINT nID )
{
	if( resourceName.isNULL() || (type==TYPE_INVALID) || !nID )
		return FALSE;

	ITER_CONTROLINFO iRes = this->m_resourceList.find( resourceName );
	if( iRes != this->m_resourceList.end() )
	{
		CLD_FL( DF_ERROR, "Given resource name has been already registered." );
		return FALSE;
	}

	CONTROL_INFO info;
	info.m_type = type;
	info.m_nID = nID;
	info.m_pContainer = NULL;
	this->m_resourceList.insert( PAIR_CONTROLINFO(resourceName, info) );

	return TRUE;
}


BOOL ClDialogManager::AddContainer( ClContainer* pContainer )
{
	ITER_CONTROLINFO iRes = this->m_resourceList.find( pContainer->GetControlID() );
	if( iRes == this->m_resourceList.end() )
	{
		CLDW_FL( DF_WARNING, TEXT("Unknown control id found(%s)"), pContainer->GetControlID().c_str() );
		return FALSE;
	}

	pContainer->SetResourceID( iRes->second.m_nID );
	pContainer->SetType( iRes->second.m_type );
	iRes->second.m_pContainer = pContainer;

	return TRUE;
}


BOOL ClDialogManager::RemoveResource( const HashedString& resourceName )
{
	if( resourceName.isNULL() )
		return FALSE;

	ITER_CONTROLINFO iRes = this->m_resourceList.find( resourceName );
	if( iRes == this->m_resourceList.end() )
		return FALSE;

	this->m_resourceList.erase( iRes );

	return TRUE;
}


VOID ClDialogManager::ClearResource( VOID )
{
	if( this->m_resourceList.empty() )
		return;

	ITER_CONTROLINFO iRes		= this->m_resourceList.begin();
	ITER_CONTROLINFO iResEnd	= this->m_resourceList.end();
	for( ; iRes != iResEnd; iRes ++ )
	{
		if( iRes->second.m_pContainer )
			delete iRes->second.m_pContainer;
	}

	this->m_resourceList.clear();
}

//================================================================================================= E. CONTROL
//================================================================================================= S. GET/SET

BOOL ClDialogManager::SetParentDialog( ClDialog* pParentDialog )
{
	if( !pParentDialog ) return FALSE;

	this->m_pParentDialog = pParentDialog;

	return TRUE;
}


BOOL ClDialogManager::SetTitle( const char* lpszTitle )
{
	if( !lpszTitle ) return FALSE;

	WCHAR* lpwzTitle = NULL;
	if( !ClUtil::STRING::convertUTF8ToUnicode(lpszTitle, &lpwzTitle) )
	{
		CLD_FL( DF_ERROR, "Can not copy string(UTF8 to UNICODE)" );
		return FALSE;
	}
	this->m_wtsTitle = lpwzTitle;
	if( lpwzTitle ) delete [] lpwzTitle;

	return TRUE;
}


const WCHAR* ClDialogManager::GetTitle( VOID )
{
	return this->m_wtsTitle.c_str();
}


BOOL ClDialogManager::SetGeometry( const ClGeometry& geoDialog )
{
	if( !GEO_VALID_SIZE(geoDialog) )
		return FALSE;

	this->m_geoDialog = geoDialog;
	return TRUE;
}


BOOL ClDialogManager::SetBackground( INT nMode, const HashedString& ImagePath, const Color& color, BOOL bUseTpColor, const Color& tpColor,
									ClDrawImage::DRAW_MODE drawMode, const ClGeometry& geoImage, const ClGeometry& geoSubImage, DWORD dwGeoBase )
{
	if( nMode < 0 ) return FALSE;

	BGINFO bgInfo = { nMode, NULL };

	bgInfo.m_pBgImage = new ClDrawImage();
	if( !bgInfo.m_pBgImage )
	{
		CLD_FL( DF_ERROR, "Out of memory" );
		return FALSE;
	}

	if( !bgInfo.m_pBgImage->loadImage(ImagePath, color, !!bUseTpColor, tpColor, drawMode, geoImage, geoSubImage, dwGeoBase) )
	{
		delete bgInfo.m_pBgImage;
		return FALSE;
	}

	this->m_BgList.push_back( bgInfo );

	return TRUE;
}


ClDrawImage* ClDialogManager::GetBackground( INT nMode, INT nIndex )
{
	INT nCounter = 0;

	ITER_BG iBg = this->m_BgList.begin();
	ITER_BG iBgEnd = this->m_BgList.end();
	for( ; iBg != iBgEnd; iBg++ )
	{
		if( iBg->m_nMode == nMode )
		{
			if( nCounter == nIndex ) break;
			nCounter++;
		}
	}

	if( iBg == iBgEnd )
		return NULL;

	return iBg->m_pBgImage;
}


INT ClDialogManager::GetModeBgCount( INT nMode )
{
	INT nCounter = 0;

	ITER_BG iBg = this->m_BgList.begin();
	ITER_BG iBgEnd = this->m_BgList.end();
	for( ; iBg != iBgEnd; iBg++ )
	{
		if( iBg->m_nMode == nMode )
			nCounter++;
	}

	return nCounter;
}


VOID ClDialogManager::ClearBackground( VOID )
{
	if( this->m_BgList.empty() )
		return;

	ITER_BG iBg = this->m_BgList.begin();
	ITER_BG iBgEnd = this->m_BgList.end();
	for( ; iBg != iBgEnd; iBg++ )
	{
		if( iBg->m_pBgImage != NULL )
			delete iBg->m_pBgImage;
	}

	return;
}


BOOL ClDialogManager::IsEmptyResourceList( VOID )
{
	return this->m_resourceList.empty();
}


INT ClDialogManager::GetResourceListSize( VOID )
{
	return this->m_resourceList.size();
}


ClDialogManager::PCONTROL_INFO ClDialogManager::GetResource( const HashedString& resourceName )
{
	ITER_CONTROLINFO iRes = this->m_resourceList.find( resourceName );
	if( iRes == this->m_resourceList.end() )
	{
		CLDW_FL( DF_WARNING, TEXT("Given Resource Name is invalid(%s)"), resourceName.c_str() );
		return 0;
	}

	return &iRes->second;
}


CWnd* ClDialogManager::GetControl( const HashedString& resourceName )
{
	PCONTROL_INFO pInfo = this->GetResource( resourceName );
	if( pInfo == NULL || pInfo->m_pContainer == NULL )
	{
		CLDW_FL( DF_WARNING, TEXT("Given Resource Name is invalid(%s)"), resourceName.c_str() );
		return NULL;
	}

	return pInfo->m_pContainer->GetControl();
}


ClDialogManager::PCONTROL_INFO ClDialogManager::GetResource( INT nIndex )
{
	if( nIndex < 0 ) return NULL;

	ITER_CONTROLINFO iRes = this->m_resourceList.begin();
	std::advance( iRes, nIndex );

	return &iRes->second;
}

//================================================================================================= E. GET/SET
//================================================================================================= EOF