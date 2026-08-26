//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//  Module: wbcIBackground.h
//
//  Description: WBC image background interface
//
//  Update History
// 
//      2014.11.10 [Allen Han] Initial Release v1.0.00, The last one, Good-bye
//=================================================================================================
//================================================================================================= S. FILEID
#pragma once
//================================================================================================= E. FILEID
//================================================================================================= S. INCLUDE

#include "stdafx.h"

//================================================================================================= E. INCLUDE
//================================================================================================= S. CLASS

interface IWBCBackground
{

public:

    virtual void                    TransferBackground  ( CDC* pDC ) = 0;

protected:

            CDC                     m_dcBackground;
};

//================================================================================================= E. CLASS
//================================================================================================= EOF
