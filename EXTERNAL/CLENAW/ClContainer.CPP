//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClContainer.CPP
//
//	Description: CLENAW Window control container
//
//	Update History
// 
//		2011.10.10 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. INCLUDE

#include <stdafx.h>
#include "resource.h"
#include "ClContainer.H"

#include <CLENAW/ClImage.H>
#include <CLENAW/ClText.H>
#include <CLENAW/ClButton.H>
#include <CLENAW/ClSlider.H>
#include <CLENAW/ClListCtrl.H>
#include <CLENAW/ClVideoView.H>
#include <CLENAW/ClWebBrowser.H>
#include <CLENAW/DigitNum.H>
#include <CLENAW/AnalogMeter.H>

#include <HELPER/ClLog.H>

//================================================================================================= E. INCLUDE
//================================================================================================= S. CON/DCON

ClContainer::ClContainer()
{
	this->m_resourceID		= 0;
	this->m_pControl		= NULL;
	this->m_pBgInterface	= NULL;
}


ClContainer::~ClContainer()
{
	if( this->m_pControl )
		delete this->m_pControl;
}

//================================================================================================= E. CON/DCON
//================================================================================================= S. INIT/DEINIT

//================================================================================================= E. INIT/DEINIT
//================================================================================================= S. OPERATION

BOOL ClContainer::CreateControl( CWnd* pParent )
{
	this->m_pControl->SubclassDlgItem( this->m_resourceID, pParent );

	switch( m_type )
	{
	case TYPE_IMAGE:
		{
			ClImage* pImage = (ClImage*)this->m_pControl;
			this->m_pBgInterface = (IBGTransfer*)pImage;
		}
		break;

	case TYPE_TEXT:
		{
			ClText* pText = (ClText*)this->m_pControl;
			this->m_pBgInterface = (IBGTransfer*)pText;
			pText->SetWindowText(pText->GetDrawText()->getText());
		}
		break;

	case TYPE_BUTTON:
		{
			ClButton* pButton = (ClButton*)this->m_pControl;
			this->m_pBgInterface = (IBGTransfer*)pButton;
		}
		break;

	case TYPE_SLIDER:
		{
			ClSlider* pSlider = (ClSlider*)this->m_pControl;
			this->m_pBgInterface = (IBGTransfer*)pSlider;
		}
		break;

	case TYPE_LISTBOX:
		{
			ClListCtrl* pListview = (ClListCtrl*)this->m_pControl;

			this->m_pBgInterface = (IBGTransfer*)pListview;
			pListview->AdaptColumn();
			pListview->ModifyStyle( 0, LVS_NOCOLUMNHEADER | LVS_SINGLESEL | LVS_OWNERDRAWFIXED );
			//pListview->SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);
			pListview->SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES | LVS_EX_SUBITEMIMAGES );
			pListview->SetTextBkColor( CLR_NONE );
			pListview->AdaptItemHeight();

#if 0
			static CBitmap cbBitmap;
          cbBitmap.LoadBitmap( IDB_BITMAP_CHECK );
          pListview->m_cbImageList.Create( 25, 25, ILC_COLOR24|ILC_MASK, 20, 2 );
          pListview->m_cbImageList.Add( &cbBitmap, RGB( 255, 255, 255 ) );

			static CBitmap cbBitmapSel;
          cbBitmapSel.LoadBitmap( IDB_BITMAP_CHECKSEL );
          pListview->m_cbImageList.Add( &cbBitmapSel, RGB( 255, 255, 255 ) );

          pListview->GetHeaderCtrl()->SetImageList( &pListview->m_cbImageList );

          pListview->SetHeaderCheck( pListview->m_bIsAllCheck );
#endif					
		}
		break;

	case TYPE_VIDEO:
		{
			ClVideoView* pVideoView = (ClVideoView*)this->m_pControl;
			this->m_pBgInterface = (IBGTransfer*)pVideoView;
		}
		break;

	case TYPE_WEB:
		{
			ClWebBrowser* pMap = (ClWebBrowser*)this->m_pControl;
			this->m_pBgInterface = (IBGTransfer*)pMap;
		}
		break;
	case TYPE_DIGIT:
		{
			CDigitNum* pDigit = (CDigitNum*)this->m_pControl;
			this->m_pBgInterface = (IBGTransfer*)pDigit;
		}
		break;
	case TYPE_AMETER:
		{
			CAnalogMeter* pAMeter = (CAnalogMeter*)this->m_pControl;
			this->m_pBgInterface = (IBGTransfer*)pAMeter;
		}
		break;
	}

	this->AdaptMode( 0 );

	return TRUE;
}


BOOL ClContainer::AdaptMode( INT nMode )
{
	if( nMode < 0 ) return FALSE;

	PMODEINFO pModeInfo = this->GetModeInfo( nMode );
	if( !pModeInfo )
	{
		if( this->m_type == TYPE_WEB )	this->m_pControl->MoveWindow( 0, 0, 0, 0 );
		else							this->m_pControl->ShowWindow( SW_HIDE );

		return TRUE;
	}

	BOOL bInvalidate = FALSE;

	CRect rtParent;
	this->m_pControl->GetParent()->GetClientRect( rtParent );
	ClGeometry geoControl = pModeInfo->m_geometry;
	ClUtil::GEOMETRY::recalcGeometry( geoControl, rtParent.Width(), rtParent.Height(), pModeInfo->m_dwGeoBase );

	CRect rtWindow;
	this->m_pControl->GetClientRect( rtWindow );
	if(		(geoControl.x != rtWindow.left)
		||	(geoControl.y != rtWindow.top)
		||	(geoControl.width != rtWindow.Width())
		||	(geoControl.height != rtWindow.Height()) )
	{
		this->m_pControl->MoveWindow( geoControl.x, geoControl.y, geoControl.width, geoControl.height, FALSE );
		bInvalidate = TRUE;
	}

	if( !pModeInfo->m_bShow )
	{
		if( this->m_type == TYPE_WEB )
		{
			this->m_pControl->MoveWindow( 0, 0, 0, 0 );
		}
		else
		{
			this->m_pControl->ShowWindow( SW_HIDE );
			this->m_pControl->EnableWindow( FALSE );
		}
	}
	else
	{
		if( pModeInfo->m_bEnable != this->m_pControl->IsWindowEnabled() )
		{
			this->m_pControl->EnableWindow( pModeInfo->m_bEnable );
			if( pModeInfo->m_bEnable )	bInvalidate = TRUE;
		}

		if( pModeInfo->m_bShow != this->m_pControl->IsWindowVisible() )
		{
			this->m_pControl->ShowWindow( pModeInfo->m_bShow ? SW_SHOW : SW_HIDE );
			bInvalidate = TRUE;
		}
		if( bInvalidate ) this->m_pControl->Invalidate();
	}

	pModeInfo->m_bEnable= FALSE;
	pModeInfo->m_bShow	= FALSE;

	return TRUE;
}

//================================================================================================= E. OPERATION
//================================================================================================= S. CONTROL

//================================================================================================= E. CONTROL
//================================================================================================= S. GET/SET

BOOL ClContainer::SetControl( CWnd* pControl )
{
	if( !pControl )	return FALSE;

	this->m_pControl = pControl;
	return TRUE;
}


CWnd* ClContainer::GetControl( VOID )
{
	return this->m_pControl;
}


IBGTransfer* ClContainer::GetBgInterface( VOID )
{
	return this->m_pBgInterface;
}


BOOL ClContainer::SetGeometry( const ClGeometry& geoControl )
{
	if( !GEO_VALID_SIZE(geoControl) )	return FALSE;

	ITER_MODEINFO iModeInfo = this->m_modeInfoList.find( 0 );

	iModeInfo->second.m_geometry	= geoControl;

	return TRUE;
}


const ClGeometry& ClContainer::GetGeometry( void )
{
	ITER_MODEINFO iModeInfo = this->m_modeInfoList.find( 0 );

	return iModeInfo->second.m_geometry;
}


UINT ClContainer::GetResourceID( VOID )
{
	return this->m_resourceID;
}


VOID ClContainer::SetResourceID( UINT nResourceID )
{
	if( !nResourceID ) return;
	this->m_resourceID = nResourceID;
}


const HashedString& ClContainer::GetControlID( VOID )
{
	return this->m_controlID;
}


BOOL ClContainer::SetControlID( const HashedString& controlID )
{
	if( controlID.isNULL() ) return FALSE;
	this->m_controlID = controlID;

	return TRUE;
}


BOOL ClContainer::SetControlID( const char* lpszControlID )
{
	WCHAR* lpwzName = NULL;
	ClUtil::STRING::convertUTF8ToUnicode( lpszControlID, &lpwzName );
	HashedString hashedName = lpwzName;
	if( lpwzName ) delete [] lpwzName;

	return this->SetControlID( hashedName );
}


CONTROL_TYPE ClContainer::GetType( VOID )
{
	return this->m_type;
}


BOOL ClContainer::SetType( CONTROL_TYPE type )
{
	this->m_type = type;
	return TRUE;
}


BOOL ClContainer::AddModeInfo( INT nMode, const ClGeometry& geometry, DWORD dwGeoBase )
{
	if( nMode < 0 ) return FALSE;

	ITER_MODEINFO iModeInfo = this->m_modeInfoList.find( nMode );
	if( iModeInfo == this->m_modeInfoList.end() )
	{
		MODEINFO modeInfo;
		modeInfo.m_dwGeoBase	= dwGeoBase;
		modeInfo.m_geometry		= geometry;
		modeInfo.m_bShow		= FALSE;
		modeInfo.m_bEnable		= FALSE;

		this->m_modeInfoList.insert( PAIR_MODEINFO(nMode,modeInfo) );
	}
	else
	{
		CLDW_FL( DF_WARNING,
			TEXT("Duplicate mode found(resource name : %s, mode : %d). Previous mode's information will be deleted."),
			this->m_controlID.c_str(), nMode );

		iModeInfo->second.m_dwGeoBase	= dwGeoBase;
		iModeInfo->second.m_geometry	= geometry;
		iModeInfo->second.m_bShow		= FALSE;
		iModeInfo->second.m_bEnable		= FALSE;
	}

	return TRUE;
}


PMODEINFO ClContainer::GetModeInfo( INT nMode )
{
	ITER_MODEINFO iModeInfo = this->m_modeInfoList.find( nMode );
	if( iModeInfo == this->m_modeInfoList.end() )
		return NULL;

	return &iModeInfo->second;
}

//================================================================================================= E. GET/SET
//================================================================================================= EOF
