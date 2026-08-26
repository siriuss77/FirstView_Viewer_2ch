//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClIBGTransfer.H
//
//	Description: CLENA-W Background transfer interface
//
//	Update History
// 
//		2011.11.23 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. FILEID

#pragma once

//================================================================================================= E. FILEID
//================================================================================================= S. INCLUDE

#include <stdafx.h>

//================================================================================================= E. INCLUDE
//================================================================================================= S. CLASS

interface IBGTransfer
{

public:

	virtual	void					TransferBackground	( CDC* pDC ) = 0;

protected:

			CDC						m_dcBackground;
};

//================================================================================================= E. CLASS
