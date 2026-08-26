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
//================================================================================================= S. FILEID

#pragma once
//================================================================================================= E. FILEID
//================================================================================================= S. INCLUDE

#include "stdafx.h"

//================================================================================================= E. INCLUDE
//================================================================================================= S. CLASS

class CWBCVideoView : public CStatic
{
	DECLARE_DYNAMIC(CWBCVideoView)

public:
	CWBCVideoView();
	virtual ~CWBCVideoView();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg BOOL                    OnEraseBkgnd        ( CDC* pDC );
    virtual void                    DrawItem            ( LPDRAWITEMSTRUCT lpDrawItemStruct );

protected:

};

//================================================================================================= E. CLASS
//================================================================================================= EOF
