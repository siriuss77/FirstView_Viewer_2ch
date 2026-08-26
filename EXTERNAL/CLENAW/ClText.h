//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClText.H
//
//	Description: CLENA-W Text control
//
//	Update History
// 
//		2011.11.23 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. FILEID

#pragma once

//================================================================================================= E. FILEID
//================================================================================================= S. INCLUDE

#include <CLENAW/ClIBGTransfer.h>
#include <HELPER/ClDrawText.H>

//================================================================================================= E. INCLUDE
//================================================================================================= S. CLASS

class ClText : public CStatic, IBGTransfer
{
	DECLARE_DYNAMIC(ClText)

public:
	ClText();
	virtual ~ClText();


public:
			BOOL					SetDrawText			( const CString& cstrString, const CString& cstrFontFace, INT nFontSize,
															DWORD dwformat = ClDrawText::FORMAT_CENTER|ClDrawText::FORMAT_VCENTER|ClDrawText::FORMAT_WORDELLIPSIS,
															const Color& color = Color((ARGB)Color::White) );
			ClDrawText*				GetDrawText			( VOID );

public:

	virtual void					DrawItem			( LPDRAWITEMSTRUCT lpDrawItemStruct );
	virtual VOID					TransferBackground	( CDC* pDC );


protected:

	DECLARE_MESSAGE_MAP()

protected:

			ClDrawText*				m_pText;
};

//================================================================================================= E. CALSS
