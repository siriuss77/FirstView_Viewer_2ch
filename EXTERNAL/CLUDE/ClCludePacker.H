//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClCludePacker.H
//
//	Description: CLENA CLUDE Packer
//
//	Update History
// 
//		2011.10.10 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. FILEID

#pragma once

//================================================================================================= E. FILEID
//================================================================================================= S. INCLUDE

#include <string>
#include <CLUDE/ClCludeApp.H>

//================================================================================================= E. INCLUDE
//================================================================================================= S. PREPROCESSOR


//================================================================================================= E. PREPROCESSOR
//================================================================================================= S. CLASS

class ClCludePacker
{

public:
	ClCludePacker();
	~ClCludePacker();


public:

			bool					Init				( void );
			void					Deinit				( void );


private:

			ClCludeApp*				m_pCludeApp;
};

//================================================================================================= E. CLASS


