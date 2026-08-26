//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClDialog.CPP
//
//	Description: CLENAW Dialog control
//
//	Update History
// 
//		2011.11.23 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. INCLUDE

#include <stdafx.h>
#include "ClDialog.h"

#include <CLUDE/ClCludeDialog.H>
#include <HELPER/ClLog.H>

//================================================================================================= S. INCLUDE
//================================================================================================= S. CON/DCON
IMPLEMENT_DYNAMIC(ClDialog, CDialog)


ClDialog::ClDialog( UINT nIDTemplate, CWnd* pParentWnd /*=NULL*/ )
					:CDialog( nIDTemplate, pParentWnd )
{
	this->m_pDialogManager	= NULL;
	this->m_bIsBgUpdate		= TRUE;
	this->m_nCurMode		= 0;
	this->m_bEasyMove		= TRUE;
}


ClDialog::~ClDialog()
{
	if( this->m_pDialogManager )
		delete this->m_pDialogManager;

	this->m_dcBackground.DeleteDC();
}

//================================================================================================= E. CON/DCON
//================================================================================================= S. MESSAGE

BEGIN_MESSAGE_MAP(ClDialog, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


BOOL ClDialog::OnEraseBkgnd(CDC* pDC)
{
	CRect rtClient;
	this->GetClientRect( rtClient );

	if( !this->m_bIsBgUpdate && this->m_dcBackground.m_hDC )
	{
		pDC->BitBlt( 0, 0, rtClient.Width(), rtClient.Height(), &this->m_dcBackground, 0, 0, SRCCOPY );
		return TRUE;
	}

	ClMemDC memDC( pDC, &rtClient, TRUE );
	CBrush blackBrush( RGB(0,0,0) );

	if(memDC.m_hDC)
	{
		memDC->FillRect( rtClient, &blackBrush );

		INT nBgCounter = this->m_pDialogManager->GetModeBgCount( this->m_nCurMode );
		if( nBgCounter <= 1 )
		{
			ClDrawImage* pBg = this->m_pDialogManager->GetBackground( this->m_nCurMode, 0 );
			if( pBg )
			{
				pBg->prepareImage( &memDC, rtClient );
				pBg->drawImage( &memDC, rtClient );
			}
		}
		else
		{
			INT nIndex;
			for( nIndex = 0; nIndex < nBgCounter; nIndex ++ )
			{
				ClDrawImage* pBg = this->m_pDialogManager->GetBackground( this->m_nCurMode, nIndex );
				if( pBg )
				{
					pBg->prepareImage( &memDC, rtClient );
					pBg->drawImage( &memDC, rtClient );
				}
			}
		}
	
	
		if( this->m_bIsBgUpdate )
		{
			CBitmap compBitmap;

			this->m_dcBackground.DeleteDC();

			this->m_dcBackground.CreateCompatibleDC( pDC );
			compBitmap.CreateCompatibleBitmap( pDC, rtClient.Width(), rtClient.Height() );
			this->m_dcBackground.SelectObject( &compBitmap );
			this->m_dcBackground.BitBlt( 0, 0, rtClient.Width(), rtClient.Height(), &memDC, 0, 0, SRCCOPY );

			this->m_pDialogManager->TransferBackground( &memDC );
			this->m_bIsBgUpdate = FALSE;
		}
	}

	return TRUE;
}


void ClDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER( nFlags );

	if( this->m_bEasyMove )
		PostMessage( WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y) );

	CDialog::OnLButtonDown(nFlags, point);
}

//================================================================================================= E. MESSAGE
//================================================================================================= S. VIRTUAL

void ClDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


VOID ClDialog::InitResourceID( VOID )
{
	return;
}


BOOL ClDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	if( this->m_pDialogManager )
	{
		int nScreenWidth;
		int nScreenHeight;

		CWnd* pParent = this->GetParent();
		if( pParent )
		{
			CRect rtParent;
			pParent->GetWindowRect( rtParent );

			ClGeometry geoDialog = this->m_pDialogManager->GetGeometry();
			int x = geoDialog.x;
			int y = geoDialog.y;

			if( x < 0 ) x = (rtParent.Width()- geoDialog.width) / 2;
			if( y < 0 ) y = (rtParent.Height()- geoDialog.height) / 2;

			x += rtParent.left;
			y += rtParent.top;

			this->MoveWindow( x, y, geoDialog.width, geoDialog.height );
		}
		else
		{
			nScreenWidth	= GetSystemMetrics( SM_CXSCREEN );
			nScreenHeight	= GetSystemMetrics( SM_CYSCREEN );

			ClGeometry geoDialog = this->m_pDialogManager->GetGeometry();
			int x = geoDialog.x;
			int y = geoDialog.y;

			if( x < 0 ) x = (nScreenWidth - geoDialog.width) / 2;
			if( y < 0 ) y = (nScreenHeight - geoDialog.height) / 2;

			this->MoveWindow( x, y, geoDialog.width, geoDialog.height );
		}

		this->SetWindowText( this->m_pDialogManager->GetTitle() );

		CONST INT nResSize = this->m_pDialogManager->GetResourceListSize();
		for( INT nCount = 0; nCount < nResSize; nCount ++ )
		{
			ClDialogManager::PCONTROL_INFO pControlInfo = this->m_pDialogManager->GetResource( nCount );
			if( pControlInfo && pControlInfo->m_pContainer )
				pControlInfo->m_pContainer->CreateControl( this );
			else
				((CWnd*)GetDlgItem(pControlInfo->m_nID))->ShowWindow( SW_HIDE );
		}
	}

	return TRUE;
}

//================================================================================================= E. VIRTUAL
//================================================================================================= S. OPERATION

BOOL ClDialog::OpenClude( const std::string& cludePath )
{
	if( !cludePath.length() )
		return FALSE;

	this->m_pDialogManager = new ClDialogManager();
	if( !this->m_pDialogManager )
	{
		CLD_FL( DF_ERROR, "Out of memory" );
		return FALSE;
	}
	this->InitResourceID();

	ClCludeDialog* pClude = new ClCludeDialog();
	if( pClude )
	{
		CLD_FL( DF_ERROR, "Out of memory" );
		return FALSE;
	}

	pClude->openCLUDE( cludePath, this->m_pDialogManager );

	if( pClude ) delete pClude;

	return TRUE;
}


BOOL ClDialog::OpenCludeBuffer( const char* pBuffer )
{
	if( !pBuffer )
		return FALSE;

	this->m_pDialogManager = new ClDialogManager();
	if( !this->m_pDialogManager )
	{
		CLD_FL( DF_ERROR, "Out of memory" );
		return FALSE;
	}
	this->InitResourceID();

	ClCludeDialog* pClude = new ClCludeDialog();
	if( !pClude )
	{
		CLD_FL( DF_ERROR, "Out of memory" );
		return FALSE;
	}

	pClude->openCLUDEBuffer( pBuffer, this->m_pDialogManager );

	if( pClude ) delete pClude;

	return TRUE;
}

//================================================================================================= E. OPERATION
//================================================================================================= S. GET/SET

VOID ClDialog::SetCurrentMode( INT nMode )
{
	if( this->m_nCurMode == nMode )
		return;

	this->m_nCurMode = nMode;
	this->m_bIsBgUpdate = TRUE;

	this->InvalidateRect( NULL );

	return;
}


INT ClDialog::GetCurrentMode( VOID )
{
	return this->m_nCurMode;
}


VOID ClDialog::EnableEasyMove( BOOL bEasyMoveEnable )
{
	this->m_bEasyMove = bEasyMoveEnable;
}


BOOL ClDialog::IsEasyMove( VOID )
{
	return this->m_bEasyMove;
}


CDC* ClDialog::GetBackground( VOID )
{
	return &this->m_dcBackground;
}

//================================================================================================= E. GET/SET
