//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClCludeDialog.H
//
//	Description: CLENA CLUDE Parser for control
//
//	Update History
// 
//		2011.10.10 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. FILEID

#pragma once

//================================================================================================= E. FILEID
//================================================================================================= S. INCLUDE

#include <TINYXML/tinyxml.h>
#include <CLENAW/ClControlFactory.H>
#include <CLENAW/ClDialogManager.H>
#include <HELPER/ClGeometry.H>

#include <list>
#include <string>

//================================================================================================= E. INCLUDE
//================================================================================================= S. PREPROCESSOR

//================================================================================================= E. PREPROCESSOR
//================================================================================================= S. CLASS

class ClCludeDialog
{

public:
	ClCludeDialog();
	~ClCludeDialog();


public:

			BOOL					openCLUDE			( const std::string& xmlFilePath, ClDialogManager* pDialogManager );
			BOOL					openCLUDEBuffer		( const char* pBuffer, ClDialogManager* pDialogManager );


private:
			BOOL					parseDIALOG			( TiXmlElement* pXmlDialog );
			BOOL					parseControls		( TiXmlElement* pEleDlgControls );

			BOOL					parseEssential		( TiXmlElement* pEleControl, ClContainer* pContainer );

			BOOL					parseBUTTON			( TiXmlElement* pXmlPushButton, ClContainer* pContainer );
			BOOL					parseIMAGE			( TiXmlElement* pXmlImage, ClContainer* pContainer );
			BOOL					parseTEXT			( TiXmlElement* pXmlText, ClContainer* pContainer );
			BOOL					parseSLIDER			( TiXmlElement* pXmlText, ClContainer* pContainer );
			BOOL					parseWEB			( TiXmlElement* pEleControl, ClContainer* pContainer );
			BOOL					parseVIDEO			( TiXmlElement* pEleControl, ClContainer* pContainer );
			BOOL					parseLISTVIEW		( TiXmlElement* pEleControl, ClContainer* pContainer );
			BOOL					parseGSENSOR		( TiXmlElement* pEleControl, ClContainer* pContainer );

			BOOL					parseImageProp		( TiXmlElement* pEleImageProp, HashedString& hashedImagePath, RGBQUAD* pColor,
															BOOL* pUseTpColor, RGBQUAD* pTpColor, ClDrawImage::DRAW_MODE* pDrawMode,
															ClGeometry& geoImage, ClGeometry& geoSubImage, DWORD* pDwGeoBase );
			BOOL					parseTextProp		( TiXmlElement* pEleTextProp, ClGeometry& geometry,
															CString& cstrText, CString& cstrFontFace, INT* pFontSize,
															RGBQUAD* pColor, DWORD* pDWFormat );
			BOOL 					parseDigitNumber		( TiXmlElement* pEleControl, ClContainer* pContainer );
			BOOL 					parseAnalogMeter( TiXmlElement* pEleControl, ClContainer* pContainer );
private:

			ClDialogManager*		m_pDialogManager;
			ClControlFactory*		m_pControlFactory;
};

//================================================================================================= E. CLASS



