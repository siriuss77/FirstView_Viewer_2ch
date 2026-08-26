//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: wbcVideoView.CPP
//
//	Description: Blackbox dedicated windows control - VIDEO VIEW
//
//	Update History
// 
//		2011.11.23 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. INCLUDE

#include "stdafx.h"
#include "wbcVideoView.h"

//================================================================================================= E. INCLUDE
//================================================================================================= S. CON/DCON
IMPLEMENT_DYNAMIC(CWBCVideoView, CStatic)

CWBCVideoView::CWBCVideoView()
{

}

CWBCVideoView::~CWBCVideoView()
{

}

//================================================================================================= E. CON/DCON
//================================================================================================= S. MESSAGE

BEGIN_MESSAGE_MAP(CWBCVideoView, CStatic)
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


BOOL CWBCVideoView::OnEraseBkgnd( CDC* pDC )
{
    UNREFERENCED_PARAMETER( pDC );

    return FALSE;
}

//================================================================================================= E. MESSAGE
//================================================================================================= S. VIRTUAL

void CWBCVideoView::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
    UNREFERENCED_PARAMETER( lpDrawItemStruct );

    return;
}

//================================================================================================= E. VIRTUAL
//================================================================================================= S. OPERATION

//================================================================================================= E. OPERATION
