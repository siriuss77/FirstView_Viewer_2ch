//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: wbcText.h
//
//	Description: 
//
//	Update History
// 
//		2011.11.23 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. FILEID

#pragma once

//================================================================================================= E. FILEID
//================================================================================================= S. INCLUDE

#include "wbcIBackground.h"

//================================================================================================= E. INCLUDE
//================================================================================================= S. CLASS

class CWBCText : public CStatic, IWBCBackground
{
    DECLARE_DYNAMIC(CWBCText)

public:
    CWBCText();
    virtual ~CWBCText();

public:

    virtual void					DrawItem			( LPDRAWITEMSTRUCT lpDrawItemStruct );
    virtual VOID					TransferBackground	( CDC* pDC );
    void                    SetTextColor        ( COLORREF crTextColor );

    void                    SetFont             ( const CString& cstrFontFace, int nFontSize );


protected:

    DECLARE_MESSAGE_MAP()

protected:

    CDC						m_dcBackground;
    COLORREF                m_crTextColor;
    CString                 m_cstrFontFace;
    int                     m_nFontSize;
};

//================================================================================================= E. CALSS