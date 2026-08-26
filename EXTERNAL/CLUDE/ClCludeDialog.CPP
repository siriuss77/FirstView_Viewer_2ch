//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClCludeDialog.CPP
//
//	Description: CLENA CLUDE Parser for control
//
//	Update History
// 
//		2011.10.10 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. INCLUDE

#include <stdafx.h>
#include <BBPlayer.h>
#include "ClCludeDialog.h"

#include <HELPER/CLUtil.H>
#include <HELPER/ClLog.H>

#include <CLENAW/ClButton.H>
#include <CLENAW/ClImage.H>
#include <CLENAW/ClText.H>
#include <CLENAW/ClSlider.H>
#include <CLENAW/ClListCtrl.H>
#include <CLENAW/ClVideoView.H>
#include <CLENAW/ClWebBrowser.H>
#include <CLENAW/GSensorView.H>
#include <CLENAW/ClDialog.H>
#include <CLENAW/DigitNum.H>
#include <CLENAW/AnalogMeter.H>

//================================================================================================= E. INCLUDE
#define FROMRGB(qRgb)	(RGB(qRgb.rgbRed, qRgb.rgbGreen, qRgb.rgbBlue ))
//================================================================================================= S. CON/DCON

ClCludeDialog::ClCludeDialog()
{
	this->m_pDialogManager	= NULL;
	this->m_pControlFactory	= NULL;
}


ClCludeDialog::~ClCludeDialog()
{
	if( this->m_pControlFactory )	delete this->m_pControlFactory;
}

//================================================================================================= E. CON/DCON
//================================================================================================= S. OPERATION

BOOL ClCludeDialog::openCLUDE( const std::string& xmlFilePath, ClDialogManager* pDialogManager )
{
	if( !xmlFilePath.length() || !pDialogManager ) return FALSE;

	this->m_pDialogManager = pDialogManager;
	this->m_pControlFactory = new ClControlFactory();
	if( !this->m_pControlFactory )
	{
		CLD_FL( DF_ERROR, "Out of memory" );
		return FALSE;
	}

	TiXmlDocument xmlDoc;
	if( !xmlDoc.LoadFile(xmlFilePath.c_str()) )
		return FALSE;

	TiXmlElement* pRoot = xmlDoc.RootElement();
	if( !this->parseDIALOG(pRoot) )
		return FALSE;

	TiXmlElement* pEleProperty = pRoot->FirstChildElement( "property" );
	if( !pEleProperty ) return FALSE;

	TiXmlElement* pEleControls = pRoot->FirstChildElement( "controls" );
	if( !pEleControls ) return FALSE;

	if( !this->parseDIALOG(pEleProperty) )	return FALSE;
	this->parseControls( pEleControls );

	return TRUE;
}


BOOL ClCludeDialog::openCLUDEBuffer( const char* pBuffer, ClDialogManager* pDialogManager )
{
	if( !pBuffer || !pDialogManager )	return FALSE;

	this->m_pDialogManager = pDialogManager;
	this->m_pControlFactory = new ClControlFactory();
	if( !this->m_pControlFactory )
	{
		CLD_FL( DF_ERROR, "Out of memory" );
		return FALSE;
	}

	TiXmlDocument xmlDoc;
	xmlDoc.Parse( pBuffer );

	TiXmlElement* pRoot = xmlDoc.RootElement();
	if( !this->parseDIALOG(pRoot) )
		return FALSE;

	TiXmlElement* pEleProperty = pRoot->FirstChildElement( "property" );
	if( !pEleProperty ) return FALSE;

	TiXmlElement* pEleControls = pRoot->FirstChildElement( "controls" );
	if( !pEleControls ) return FALSE;

	if( !this->parseDIALOG(pEleProperty) )	return FALSE;
	this->parseControls( pEleControls );

	return TRUE;
}


BOOL ClCludeDialog::parseDIALOG( TiXmlElement* pEleProperty )
{
	if( !pEleProperty )	return FALSE;

	TiXmlElement* pEleTitle		= pEleProperty->FirstChildElement( "title" );
	TiXmlElement* pEleGeometry	= pEleProperty->FirstChildElement( "geometry" );
	TiXmlElement* pEleBgImages	= pEleProperty->FirstChildElement( "bgimgs" );

	if( pEleTitle )
		this->m_pDialogManager->SetTitle( pEleTitle->GetText() );

	if( pEleGeometry )
	{
		ClGeometry geoDialog;
		pEleGeometry->Attribute( "x", &geoDialog.x );
		pEleGeometry->Attribute( "y", &geoDialog.y );
		pEleGeometry->Attribute( "w", &geoDialog.width );
		pEleGeometry->Attribute( "h", &geoDialog.height );

//20160514 화면 사이즈 문제 Fixed
#if 0
		int ScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
  		int ScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);

		if(geoDialog.width > ScreenWidth)
			geoDialog.width  = ScreenWidth;
		if(geoDialog.height > ScreenHeight)
			geoDialog.height  = ScreenHeight;
#endif

		this->m_pDialogManager->SetGeometry( geoDialog );
	}

	if( pEleBgImages )
	{
		TiXmlElement* pEleBgImage = pEleBgImages->FirstChildElement( "bgimg" );
		while( pEleBgImage )
		{
			int nMode = 0;
			pEleBgImage->Attribute( "mode", &nMode );

			TiXmlElement* pEleImage = pEleBgImage->FirstChildElement( "image" );
			while( pEleImage )
			{
				HashedString hashedImagePath;
				ClGeometry	geoChildImage		= GEO_FULL_SIZE;
				ClGeometry	geoSubImage			= GEO_FULL_SIZE;
				RGBQUAD		shadeColor			= {255,255,255,255};
				RGBQUAD		tpColor				= {255,255,255,255};
				BOOL		bUseTpColor			= FALSE;
				ClDrawImage::DRAW_MODE drawMode = ClDrawImage::MODE_STRETCH;
				DWORD		dwGeoBase			= 0;

				this->parseImageProp( pEleImage, hashedImagePath, &shadeColor, &bUseTpColor, &tpColor, &drawMode, geoChildImage, geoSubImage, &dwGeoBase );
				this->m_pDialogManager->SetBackground( nMode, hashedImagePath,
					Color(shadeColor.rgbReserved, shadeColor.rgbRed, shadeColor.rgbGreen, shadeColor.rgbBlue),
					bUseTpColor, Color(tpColor.rgbReserved, tpColor.rgbRed, tpColor.rgbGreen, tpColor.rgbBlue),
					drawMode, geoChildImage, geoSubImage, dwGeoBase );

				pEleImage = pEleImage->NextSiblingElement( "image" );
			}

			pEleBgImage = pEleBgImage->NextSiblingElement( "bgimg" );
		}
	}

	return TRUE;
}


BOOL ClCludeDialog::parseControls( TiXmlElement* pEleDlgControls )
{
	TiXmlElement* pEleControl = pEleDlgControls->FirstChildElement();
	while( pEleControl )
	{
		ClContainer* pContainer =
			this->m_pControlFactory->CreateControl( pEleControl->Attribute( "type" ) );

		if( pContainer )
		{
			if( this->parseEssential(pEleControl, pContainer) )
			{
				const char* lpszControlType = pEleControl->Attribute( "type" );
				if( lpszControlType )
				{
					switch( lpszControlType[0] )
					{
					case 'b': if(!strcmp(lpszControlType, "button"))	this->parseBUTTON( pEleControl, pContainer );	break;
					case 'i': if(!strcmp(lpszControlType, "image"))		this->parseIMAGE( pEleControl, pContainer );	break;
					case 't': if(!strcmp(lpszControlType, "text"))		this->parseTEXT( pEleControl, pContainer );		break;
					case 's': if(!strcmp(lpszControlType, "slider"))	this->parseSLIDER( pEleControl, pContainer );	break;
					case 'v': if(!strcmp(lpszControlType, "video"))		this->parseVIDEO( pEleControl, pContainer );	break;
					case 'l': if(!strcmp(lpszControlType, "list"))		this->parseLISTVIEW( pEleControl, pContainer );	break;
					case 'g': if(!strcmp(lpszControlType, "gsensor"))	this->parseGSENSOR( pEleControl, pContainer );	break;
					case 'w': if(!strcmp(lpszControlType, "web"))		this->parseWEB( pEleControl, pContainer );		break;
					case 'd': if(!strcmp(lpszControlType, "digit"))		this->parseDigitNumber( pEleControl, pContainer );		break;
					case 'a': if(!strcmp(lpszControlType, "analogmeter"))		this->parseAnalogMeter( pEleControl, pContainer );		break;
					default: CLD_FL( DF_ERROR, "Unrecognized control(%s)", lpszControlType );
					}
				}
			}
			else
			{
				if( pContainer ) delete pContainer;
				pContainer = NULL;
				CLD_FL( DF_ERROR, "Can not parse essential data of control." );
			}
		}

		if( pContainer ) this->m_pDialogManager->AddContainer( pContainer );

		pEleControl = pEleControl->NextSiblingElement();
	}

	return TRUE;
}


BOOL ClCludeDialog::parseEssential( TiXmlElement* pEleControl, ClContainer* pContainer )
{
	if( !pEleControl || !pContainer ) return FALSE;

	const char* lpszControlID = pEleControl->Attribute( "id" );
	if( !lpszControlID ) goto PARSE_FAILED;
	pContainer->SetControlID( lpszControlID );

	TiXmlElement* pEleMode = pEleControl->FirstChildElement( "mode" );
	if( !pEleMode )	return FALSE;
	while( pEleMode )
	{
		INT			nMode		= 0;
		DWORD		dwGeoBase	= 0;
		ClGeometry	geoMode		= GEO_ZERO_SIZE;

		const char* lpszText = pEleMode->GetText();
		if( lpszText )
			nMode = atoi(lpszText);

		lpszText = pEleMode->Attribute( "geobase" );
		if( lpszText )
			ClUtil::CLUDE::parseModeGeoBase( lpszText, &dwGeoBase );

		pEleMode->Attribute("x", &geoMode.x );
		pEleMode->Attribute("y", &geoMode.y );
		pEleMode->Attribute("w", &geoMode.width );
		pEleMode->Attribute("h", &geoMode.height );

		pContainer->AddModeInfo( nMode, geoMode, dwGeoBase );

		pEleMode = pEleMode->NextSiblingElement( "mode" );
	}

	return TRUE;

PARSE_FAILED:
	CLD_FL( DF_ERROR, "Can not parse essential information of control" );
	return FALSE;
}


BOOL ClCludeDialog::parseBUTTON( TiXmlElement* pEleControl, ClContainer* pContainer )
{
	if( !pEleControl || !pContainer ) return FALSE;

	ClButton* pButton = (ClButton*)pContainer->GetControl();
	ClButton::OPERATION_MODE mode = ClButton::MODE_PUSH;
	BOOL bIndicate = FALSE;

	const char* lpszText = pEleControl->Attribute( "mode" );
	if( lpszText )
	{
		INT nMode = 0;
		ClUtil::CLUDE::parseButtonMode( lpszText, &nMode );
		if( nMode == (INT)ClButton::MODE_INVALID )
		{
			CLD_FL( DF_ERROR, "Can not parse button mode(%d)", lpszText );
			return FALSE;
		}

		if(nMode == ClButton::MODE_INDICATE){
			nMode = ClButton::MODE_CHECK;
			bIndicate = TRUE;
		}

		mode = (ClButton::OPERATION_MODE)nMode;
	}
	pButton->SetMode( mode, bIndicate );

	TiXmlElement* pEleText = pEleControl->FirstChildElement( "text" );
	if( pEleText )
	{
		ClGeometry	geoText		= GEO_FULL_SIZE;
		CString		cstrFontFace= TEXT("");
		CString		cstrText	= TEXT("");
		int			nFontSize	= -1;
		RGBQUAD		color		= {255,255,255,255};
		DWORD		dwForamt	= 0;

		this->parseTextProp( pEleText, geoText, cstrText, cstrFontFace, &nFontSize, &color, &dwForamt );
		pButton->SetText( geoText, cstrText, cstrFontFace, nFontSize, dwForamt,
							Color(color.rgbReserved, color.rgbRed, color.rgbGreen, color.rgbBlue) );
	}

	TiXmlElement* pEleSkins = pEleControl->FirstChildElement( "skins" );
	if( pEleSkins )
	{
		TiXmlElement* pEleSkin = pEleSkins->FirstChildElement( "skin" );
		while( pEleSkin )
		{
			ClButton::SKIN_TYPE type;
			HashedString hashedImagePath;
			ClGeometry	geoImage			= GEO_FULL_SIZE;
			ClGeometry	geoSubImage			= GEO_FULL_SIZE;
			RGBQUAD		shadeColor			= {255,255,255,255};
			RGBQUAD		tpColor				= {255,0,0,0};
			BOOL		bUseTpColor			= FALSE;
			ClDrawImage::DRAW_MODE drawMode = ClDrawImage::MODE_STRETCH;
			DWORD		dwGeoBase			= 0;

			INT nType = (INT)ClButton::SKIN_INVALID;
			lpszText = pEleSkin->Attribute( "type" );
			if( lpszText ) ClUtil::CLUDE::parseButtonSkinType( lpszText, &nType );
			if( nType == (INT)ClButton::SKIN_INVALID )
			{
				CLD_FL( DF_ERROR, "Can not parse button skin type(%d)", pEleSkin->Attribute("type") );
				return FALSE;
			}
			type = (ClButton::SKIN_TYPE)nType;

			if( this->parseImageProp(pEleSkin, hashedImagePath, &shadeColor, &bUseTpColor, &tpColor, &drawMode, geoImage, geoSubImage, &dwGeoBase ) )
				pButton->SetSkin( type, hashedImagePath, Color(shadeColor.rgbReserved, shadeColor.rgbRed, shadeColor.rgbGreen, shadeColor.rgbBlue),
									bUseTpColor, Color(tpColor.rgbReserved, tpColor.rgbRed, tpColor.rgbGreen, tpColor.rgbBlue),
									drawMode, geoImage, geoSubImage, dwGeoBase );

			pEleSkin = pEleSkin->NextSiblingElement( "skin" );
		}
	}

	return TRUE;
}


BOOL ClCludeDialog::parseIMAGE( TiXmlElement* pEleControl, ClContainer* pContainer )
{
	if( !pEleControl || !pContainer ) return FALSE;

	ClImage* pImage = (ClImage*)pContainer->GetControl();

	HashedString hashedImagePath;
	ClGeometry	geoImage			= GEO_FULL_SIZE;
	ClGeometry	geoSubImage			= GEO_FULL_SIZE;
	RGBQUAD		shadeColor			= {255,255,255,255};
	RGBQUAD		tpColor				= {255,0,0,0};
	BOOL		bUseTpColor			= FALSE;
	ClDrawImage::DRAW_MODE drawMode = ClDrawImage::MODE_STRETCH;
	DWORD		dwGeoBase			= 0;

	TiXmlElement* pEleImage = pEleControl->FirstChildElement( "image" );
	if( !pEleImage ) return FALSE;

	if( !this->parseImageProp(pEleImage, hashedImagePath, &shadeColor, &bUseTpColor, &tpColor, &drawMode, geoImage, geoSubImage, &dwGeoBase) )
		return FALSE;

	pImage->SetImage( hashedImagePath, Color(shadeColor.rgbReserved, shadeColor.rgbRed, shadeColor.rgbGreen, shadeColor.rgbBlue),
						bUseTpColor, Color(tpColor.rgbReserved, tpColor.rgbRed, tpColor.rgbGreen, tpColor.rgbBlue),
						drawMode, geoImage, geoSubImage, dwGeoBase );

	return TRUE;
}


BOOL ClCludeDialog::parseTEXT( TiXmlElement* pEleControl, ClContainer* pContainer )
{
	if( !pEleControl || !pContainer ) return FALSE;

	ClText* pText = (ClText*)pContainer->GetControl();

	ClGeometry	GEONOTUSED	= GEO_FULL_SIZE;
	CString		cstrFontFace= TEXT("");
	CString		cstrText	= TEXT("");
	int			nFontSize	= -1;
	RGBQUAD		color		= {255,255,255,255};
	DWORD		dwForamt	= 0;

	TiXmlElement* pEleText = pEleControl->FirstChildElement( "text" );
	if( !pEleText ) return FALSE;

	if( !this->parseTextProp( pEleText, GEONOTUSED, cstrText, cstrFontFace, &nFontSize, &color, &dwForamt) )
		return FALSE;

	pText->SetDrawText( cstrText, cstrFontFace, nFontSize, dwForamt,
						Color(color.rgbReserved, color.rgbRed, color.rgbGreen, color.rgbBlue) );

	return TRUE;
}


BOOL ClCludeDialog::parseSLIDER( TiXmlElement* pEleControl, ClContainer* pContainer )
{
	if( !pEleControl || !pContainer ) return FALSE;

	ClSlider* pSlider = (ClSlider*)pContainer->GetControl();

	const char* lpszText = pEleControl->Attribute( "mode" );
	if( lpszText )
	{
		if( !strcmp(lpszText, "vertical") )
			pSlider->SetVertical();
	}

	TiXmlElement* pEleTrack = pEleControl->FirstChildElement( "track" );
	if( pEleTrack )
	{
		int nMin	= 0;
		int nMax	= 10;
		int nPos	= 0;
		int nPage	= 1;
		int mLeft, mRight, mTop, mBottom;
		mLeft = mRight = mTop = mBottom = 0;

		ClGeometry geoTrack = ClGeometry( 0, 0, pContainer->GetGeometry().width, pContainer->GetGeometry().height );

		pEleTrack->Attribute( "min",	&nMin );
		pEleTrack->Attribute( "max",	&nMax );
		pEleTrack->Attribute( "page",	&nPage );

		pEleTrack->Attribute( "mleft",	&mLeft );
		pEleTrack->Attribute( "mtop",	&mTop );
		pEleTrack->Attribute( "mright",	&mRight );
		pEleTrack->Attribute( "mbottom",&mBottom );

		lpszText = pEleTrack->GetText();
		if( lpszText )
			nPos = atoi( lpszText );

		pSlider->cludeSetProperty( nMin, nMax, nPos, nPage, CRect(mLeft,mTop,mRight,mBottom) );
	}

	TiXmlElement* pEleSkins = pEleControl->FirstChildElement( "skins" );
	if( !pEleSkins ) return FALSE;

	TiXmlElement* pEleSkin = pEleSkins->FirstChildElement( "skin" );
	while( pEleSkin )
	{
		HashedString hashedImagePath;
		ClGeometry	geoImage			= GEO_FULL_SIZE;
		ClGeometry	geoSubImage			= GEO_FULL_SIZE;
		RGBQUAD		shadeColor			= {255,255,255,255};
		RGBQUAD		tpColor				= {255,0,0,0};
		BOOL		bUseTpColor			= FALSE;
		ClDrawImage::DRAW_MODE drawMode = ClDrawImage::MODE_STRETCH;
		ClSlider::SKIN_TYPE skinType	= ClSlider::SKIN_INVALID;
		DWORD		dwGeoBase			= 0;

		INT nType = (INT)ClSlider::SKIN_INVALID;
		const char* lpszText = pEleSkin->Attribute( "type" );
		if( lpszText ) ClUtil::CLUDE::parseSliderSkinType( lpszText, &nType );
		if( nType == (INT)ClSlider::SKIN_INVALID )
		{
			CLD_FL( DF_ERROR, "Can not parse button skin type(%d)", pEleSkin->Attribute("id") );
			return FALSE;
		}

		skinType = (ClSlider::SKIN_TYPE)nType;

		if( this->parseImageProp(pEleSkin, hashedImagePath, &shadeColor, &bUseTpColor, &tpColor, &drawMode, geoImage, geoSubImage, &dwGeoBase) )
			pSlider->SetSkin( skinType, hashedImagePath, Color(shadeColor.rgbReserved, shadeColor.rgbRed, shadeColor.rgbGreen, shadeColor.rgbBlue ),
								bUseTpColor, Color(tpColor.rgbReserved, tpColor.rgbRed, tpColor.rgbGreen, tpColor.rgbBlue ),
								drawMode, geoImage, geoSubImage, dwGeoBase );

		pEleSkin = pEleSkin->NextSiblingElement( "skin" );
	}

	TiXmlElement* pEleBG = pEleControl->FirstChildElement( "bgimg" );
	while( pEleBG )
	{
		HashedString hashedImagePath;
		ClGeometry	geoImage			= GEO_FULL_SIZE;
		ClGeometry	geoSubImage			= GEO_FULL_SIZE;
		RGBQUAD		shadeColor			= {255,255,255,255};
		RGBQUAD		tpColor				= {255,0,0,0};
		BOOL		bUseTpColor			= FALSE;
		ClDrawImage::DRAW_MODE drawMode = ClDrawImage::MODE_STRETCH;
		DWORD		dwGeoBase			= 0;

		if( this->parseImageProp(pEleBG, hashedImagePath, &shadeColor, &bUseTpColor, &tpColor, &drawMode, geoImage, geoSubImage, &dwGeoBase) )
			pSlider->SetBackground( hashedImagePath, Color(shadeColor.rgbReserved, shadeColor.rgbRed, shadeColor.rgbGreen, shadeColor.rgbBlue ),
									bUseTpColor, Color(tpColor.rgbReserved, tpColor.rgbRed, tpColor.rgbGreen, tpColor.rgbBlue ),
									drawMode, geoImage, geoSubImage, dwGeoBase );

		pEleBG = pEleBG->NextSiblingElement( "bgimg" );
	}

	return TRUE;
}


BOOL ClCludeDialog::parseWEB( TiXmlElement* pEleControl, ClContainer* pContainer )
{
	if( !pEleControl || !pContainer ) return FALSE;

	return TRUE;
}


BOOL ClCludeDialog::parseVIDEO( TiXmlElement* pEleControl, ClContainer* pContainer )
{
	if( !pEleControl || !pContainer ) return FALSE;

	return TRUE;
}


BOOL ClCludeDialog::parseLISTVIEW( TiXmlElement* pEleControl, ClContainer* pContainer )
{
	if( !pEleControl || !pContainer ) return FALSE;

	ClListCtrl* pListView = (ClListCtrl*)pContainer->GetControl();

	TiXmlElement* pEleBG = pEleControl->FirstChildElement( "bgimg" );
	if( pEleBG )
	{
		HashedString hashedImagePath;
		ClGeometry	geoImage			= GEO_FULL_SIZE;
		ClGeometry	geoSubImage			= GEO_FULL_SIZE;
		RGBQUAD		shadeColor			= {255,255,255,255};
		RGBQUAD		tpColor				= {255,0,0,0};
		BOOL		bUseTpColor			= FALSE;
		ClDrawImage::DRAW_MODE drawMode	= ClDrawImage::MODE_STRETCH;
		DWORD		dwGeoBase			= 0;

		if( this->parseImageProp(pEleBG, hashedImagePath, &shadeColor, &bUseTpColor, &tpColor, &drawMode, geoImage, geoSubImage, &dwGeoBase) )
			pListView->SetBackground( hashedImagePath, Color(shadeColor.rgbReserved, shadeColor.rgbRed, shadeColor.rgbGreen, shadeColor.rgbBlue),
										bUseTpColor, Color(tpColor.rgbReserved, tpColor.rgbRed, tpColor.rgbGreen, tpColor.rgbBlue),
										drawMode, geoImage, geoSubImage, dwGeoBase );
	}

	TiXmlElement* pEleColumns = pEleControl->FirstChildElement( "columns" );
	if( !pEleColumns )
	{
		CLD_FL( DF_ERROR, "Control type list must have column element." );
		return FALSE;
	}

	ClGeometry	GEONOTUSED;
	CString		CSTRNOTUSED			= TEXT("");
	CString		cstrDefaultFontFace	= ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getDefaultFont();
	INT			nDefaltFontSize		= ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getDefaultFontSize();
	RGBQUAD		defaultNormalColor	= {255,255,255,255};
	RGBQUAD		defaultSelectColor	= {255,255,255,255};
	RGBQUAD		defaultEventColor	= {255,255,255,255};
	RGBQUAD		defaultManualColor	= {255,255,255,255};    // jun 170511
	RGBQUAD		defaultMotionColor	= {255,255,255,255};    // jun 170511
	DWORD		defaultTextFormat	= 0L;
	INT			nItemHeight			= 10;

	pEleColumns->Attribute( "height", &nItemHeight );
	this->parseTextProp( pEleColumns, GEONOTUSED, CSTRNOTUSED, cstrDefaultFontFace, &nDefaltFontSize, &defaultNormalColor, &defaultTextFormat );

	const char* lpszText = pEleColumns->Attribute( "selcolor" );
	if( lpszText ) ClUtil::CLUDE::parseColor( lpszText, &defaultSelectColor.rgbReserved, &defaultSelectColor.rgbRed,
												&defaultSelectColor.rgbGreen, &defaultSelectColor.rgbBlue);
	lpszText = pEleColumns->Attribute( "evtcolor" );

	if( lpszText ) ClUtil::CLUDE::parseColor( lpszText, &defaultEventColor.rgbReserved, &defaultEventColor.rgbRed,
												&defaultEventColor.rgbGreen, &defaultEventColor.rgbBlue);
	lpszText = pEleColumns->Attribute( "mancolor" );                                                                    // jun 170511
	if( lpszText ) ClUtil::CLUDE::parseColor( lpszText, &defaultManualColor.rgbReserved, &defaultManualColor.rgbRed,    // jun 170511
												&defaultManualColor.rgbGreen, &defaultManualColor.rgbBlue);             // jun 170511
	lpszText = pEleColumns->Attribute( "motcolor" );                                                                    // jun 170511
	if( lpszText ) ClUtil::CLUDE::parseColor( lpszText, &defaultMotionColor.rgbReserved, &defaultMotionColor.rgbRed,    // jun 170511
												&defaultMotionColor.rgbGreen, &defaultMotionColor.rgbBlue);             // jun 170511
	
	pListView->SetItemHeight( nItemHeight );
	pListView->SetDrawText( cstrDefaultFontFace, nDefaltFontSize, defaultTextFormat,
							Color(defaultNormalColor.rgbReserved, defaultNormalColor.rgbRed, defaultNormalColor.rgbGreen, defaultNormalColor.rgbBlue),
							Color(defaultSelectColor.rgbReserved, defaultSelectColor.rgbRed, defaultSelectColor.rgbGreen, defaultSelectColor.rgbBlue),
							Color(defaultEventColor.rgbReserved, defaultEventColor.rgbRed, defaultEventColor.rgbGreen, defaultEventColor.rgbBlue),
							Color(defaultManualColor.rgbReserved, defaultManualColor.rgbRed, defaultManualColor.rgbGreen, defaultManualColor.rgbBlue),   // jun 170511
							Color(defaultMotionColor.rgbReserved, defaultMotionColor.rgbRed, defaultMotionColor.rgbGreen, defaultMotionColor.rgbBlue));   // jun 170511


	TiXmlElement* pEleCol = pEleColumns->FirstChildElement( "column" );
	while( pEleCol )
	{
		INT nIndex				= -1;
		INT nWidth				= -1;

		lpszText = pEleCol->Attribute( "index" );
		if( lpszText ) nIndex = atoi(lpszText);

		lpszText = pEleCol->Attribute( "width" );
		if( lpszText ) nWidth = atoi(lpszText);
		pListView->AddColumnInfo( nIndex, nWidth );

		pEleCol = pEleCol->NextSiblingElement( "column" );
	}

	return TRUE;
}

BOOL ClCludeDialog::parseGSENSOR( TiXmlElement* pEleControl, ClContainer* pContainer )
{
	if( !pEleControl || !pContainer ) return FALSE;

	CGSensorView* pGSensorGraph = (CGSensorView*)pContainer->GetControl();
	RGBQUAD	unit_text_color	= {255,255,255,255};
	
	TiXmlElement* pEleBg = pEleControl->FirstChildElement( "bgimg" );
	if( pEleBg )
	{
		RGBQUAD	bg_color	= {255,0,0,0};
		const char* lpszText = pEleBg->Attribute( "color" );
		if( lpszText ) ClUtil::CLUDE::parseColor( lpszText, &bg_color.rgbReserved, &bg_color.rgbRed, &bg_color.rgbGreen, &bg_color.rgbBlue );
		pGSensorGraph->SetBackgroundColor(FROMRGB(bg_color)) ;
	}

	TiXmlElement* pEleUnit = pEleControl->FirstChildElement( "unitx" );
	if(pEleUnit)
	{
		const char* lpszText = pEleUnit->GetText();

		pGSensorGraph->SetXUnits(CString(lpszText)) ;		
		lpszText = pEleUnit->Attribute( "color" );
		if( lpszText ) ClUtil::CLUDE::parseColor( lpszText, &unit_text_color.rgbReserved, &unit_text_color.rgbRed, &unit_text_color.rgbGreen, &unit_text_color.rgbBlue );
	}

	pEleUnit = pEleControl->FirstChildElement( "unity" );
	if(pEleUnit)
	{
		const char* lpszText = pEleUnit->GetText();
		pGSensorGraph->SetYUnits(CString(lpszText)) ;
	}
	
	TiXmlElement* pEleLine = pEleControl->FirstChildElement( "line" );
	RGBQUAD	line_color[CGSensorView::LINE_INVALID] = { {255,255,255,255},{255,255,255,255},{255,255,255,255}, {255,255,255,255},{255,255,255,255} };
	while( pEleLine )
	{
		const char* lpszText = pEleLine->GetText();
		if( lpszText )
		{
			CGSensorView::LINE_TYPE lineType = CGSensorView::LINE_INVALID;

			if( !strcmp(lpszText,"x") )		lineType = CGSensorView::LINE_X;
			if( !strcmp(lpszText,"y") )		lineType = CGSensorView::LINE_Y;
			if( !strcmp(lpszText,"z") )		lineType = CGSensorView::LINE_Z;
			if( !strcmp(lpszText,"s") )		lineType = CGSensorView::LINE_S;
			if( !strcmp(lpszText,"track") )
			{
				INT nTrackbarpos= 30;
				double dScaleX=2.0;
				lineType = CGSensorView::LINE_TRACK;
				
				pEleLine->Attribute( "trackbarpos",	&nTrackbarpos);
				pGSensorGraph->SetTrackBarPosition(nTrackbarpos);

				pEleLine->Attribute( "scale",	&dScaleX);
				pGSensorGraph->SetGridScaleX(dScaleX);
			}

			lpszText = pEleLine->Attribute( "color" );
			if( lpszText ) ClUtil::CLUDE::parseColor( lpszText, &line_color[lineType].rgbReserved, &line_color[lineType].rgbRed, &line_color[lineType].rgbGreen, &line_color[lineType].rgbBlue );
		}

		pEleLine = pEleLine->NextSiblingElement( "line" );
	}

	pGSensorGraph->SetPlotColor( FROMRGB(line_color[CGSensorView::LINE_X]), FROMRGB(line_color[CGSensorView::LINE_Y]), FROMRGB(line_color[CGSensorView::LINE_Z]), FROMRGB(line_color[CGSensorView::LINE_S])) ;

	pGSensorGraph->SetTrackBarColor( FROMRGB(line_color[CGSensorView::LINE_TRACK]));
	
	double value[4]={0.0,};
	TiXmlElement* pEleGrad = pEleControl->FirstChildElement( "gradx" );
	if(pEleGrad)
	{
		RGBQUAD	grad_color	= {255,255,255,255};
		const char* lpszText = pEleGrad->Attribute( "color" );
		if( lpszText ) ClUtil::CLUDE::parseColor( lpszText, &grad_color.rgbReserved, &grad_color.rgbRed, &grad_color.rgbGreen, &grad_color.rgbBlue );
		pGSensorGraph->SetGridColor( FROMRGB(grad_color),  FROMRGB(unit_text_color)) ;
		
		pEleGrad->Attribute( "lower", &value[0]  );
		pEleGrad->Attribute( "upper", &value[1] );
		pEleGrad->Attribute( "decimalPlaces", &value[2] );
		
		pGSensorGraph->SetXRange(value[0] ,value[1] ,(int)value[2] );	
	}
	pEleGrad = pEleControl->FirstChildElement( "grady" );
	if(pEleGrad)
	{
		pEleGrad->Attribute( "lower", &value[0]  );
		pEleGrad->Attribute( "upper", &value[1] );
		pEleGrad->Attribute( "decimalPlaces", &value[2] );
		pEleGrad->Attribute( "interval", &value[3]);

		pGSensorGraph->SetRange(value[0] ,value[1] ,(int)value[2],value[3] );	
	}
	return TRUE;
}

//================================================================================================= E. OPERATION
//================================================================================================= S. CONTROL

BOOL ClCludeDialog::parseImageProp( TiXmlElement* pEleImageProp, HashedString& hashedImagePath, RGBQUAD* pColor, BOOL* pUseTpColor, RGBQUAD* pTpColor,
									ClDrawImage::DRAW_MODE* pDrawMode, ClGeometry& geoImage, ClGeometry& geoSubImage, DWORD* pDwGeoBase )
{
	if( !pEleImageProp ) return FALSE;

	const char* lpszText = pEleImageProp->GetText();
	if( lpszText )
	{
		WCHAR* lpwzImagePath = NULL;
		if( !ClUtil::STRING::convertUTF8ToUnicode(lpszText, &lpwzImagePath) )
		{
			CLD_FL( DF_ERROR, "Can not convert to UNICODE(%s)", lpszText );
			return FALSE;
		}

		hashedImagePath = lpwzImagePath;
		if( lpwzImagePath ) delete [] lpwzImagePath;

		if( !hashedImagePath.isNULL() )
		{
			INT nMode;
			lpszText = pEleImageProp->Attribute( "drawmode" );
			if( lpszText )
			{
				ClUtil::CLUDE::parseImageDrawMode( lpszText, &nMode );
				if( nMode == (ClDrawImage::MODE_INVALID) )
				{
					CLD_FL( DF_ERROR, "Can not parse drawmode(%s)", lpszText );
					nMode = ClDrawImage::MODE_STRETCH;
				}

				*pDrawMode = (ClDrawImage::DRAW_MODE)nMode;
			}

			pEleImageProp->Attribute( "sx", &geoSubImage.x );
			pEleImageProp->Attribute( "sy", &geoSubImage.y );
			pEleImageProp->Attribute( "sw", &geoSubImage.width );
			pEleImageProp->Attribute( "sh", &geoSubImage.height );
		}

		lpszText = pEleImageProp->Attribute( "tpcolor" );
		if( lpszText )
		{
			if( ClUtil::CLUDE::parseColor(lpszText, &pTpColor->rgbReserved, &pTpColor->rgbRed,
												&pTpColor->rgbGreen, &pTpColor->rgbBlue) )
				*pUseTpColor = TRUE;
		}
	}

	pEleImageProp->Attribute( "x", &geoImage.x );
	pEleImageProp->Attribute( "y", &geoImage.y );
	pEleImageProp->Attribute( "w", &geoImage.width );
	pEleImageProp->Attribute( "h", &geoImage.height );

	lpszText = pEleImageProp->Attribute("color");
	if( lpszText )
		ClUtil::CLUDE::parseColor( lpszText, &pColor->rgbReserved, &pColor->rgbRed, &pColor->rgbGreen, &pColor->rgbBlue );

	lpszText = pEleImageProp->Attribute( "geobase" );
	if( lpszText )
		ClUtil::CLUDE::parseModeGeoBase( lpszText, pDwGeoBase );
	else
		*pDwGeoBase = GEOX_LEFT | GEOY_TOP;

	return TRUE;
}



BOOL ClCludeDialog::parseTextProp( TiXmlElement* pEleTextProp, ClGeometry& geometry,
								  CString& cstrText, CString& cstrFontFace, INT* pFontSize,
								  RGBQUAD* pColor, DWORD* pDWFormat )
{
	if( !pEleTextProp )	return FALSE;

	const char* lpszText = NULL;

	lpszText = pEleTextProp->GetText();
	if( lpszText ) ClUtil::STRING::convertUTF8ToCString( lpszText, cstrText );

	lpszText = pEleTextProp->Attribute("font");
	if( lpszText )
		ClUtil::STRING::convertUTF8ToCString( lpszText, cstrFontFace );
	else
		cstrFontFace = ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getDefaultFont();

	lpszText = pEleTextProp->Attribute("color");
	if( lpszText ) ClUtil::CLUDE::parseColor( lpszText, &pColor->rgbReserved, &pColor->rgbRed,
													&pColor->rgbGreen, &pColor->rgbBlue );

	lpszText = pEleTextProp->Attribute("format");
	if( lpszText ) ClUtil::CLUDE::parseTextFormat( lpszText, pDWFormat );

	lpszText = pEleTextProp->Attribute( "size" );
	if( lpszText )
		*pFontSize = atoi( lpszText );
	else
		*pFontSize = ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getDefaultFontSize();

	ClGeometry geoText = GEO_FULL_SIZE;

	pEleTextProp->Attribute( "x", &geoText.x );
	pEleTextProp->Attribute( "y", &geoText.y );
	pEleTextProp->Attribute( "w", &geoText.width );
	pEleTextProp->Attribute( "h", &geoText.height );

	geometry = geoText;

	return TRUE;
}

BOOL ClCludeDialog::parseDigitNumber( TiXmlElement* pEleControl, ClContainer* pContainer )
{
	if( !pEleControl || !pContainer ) return FALSE;

	CDigitNum* pDigitNum = (CDigitNum*)pContainer->GetControl();
	
	TiXmlElement* pEleSkins = pEleControl->FirstChildElement( "skins" );
	if( pEleSkins )
	{
		TiXmlElement* pEleBG = pEleControl->FirstChildElement( "bgimg" );
		while( pEleBG )
		{
			HashedString hashedImagePath;
			ClGeometry	geoImage			= GEO_FULL_SIZE;
			ClGeometry	geoSubImage			= GEO_FULL_SIZE;
			RGBQUAD		shadeColor			= {255,255,255,255};
			RGBQUAD		tpColor				= {255,0,0,0};
			BOOL		bUseTpColor			= FALSE;
			ClDrawImage::DRAW_MODE drawMode = ClDrawImage::MODE_STRETCH;
			DWORD		dwGeoBase			= 0;

			if( this->parseImageProp(pEleBG, hashedImagePath, &shadeColor, &bUseTpColor, &tpColor, &drawMode, geoImage, geoSubImage, &dwGeoBase) )
				pDigitNum->SetBackground( hashedImagePath, Color(shadeColor.rgbReserved, shadeColor.rgbRed, shadeColor.rgbGreen, shadeColor.rgbBlue ),
										bUseTpColor, Color(tpColor.rgbReserved, tpColor.rgbRed, tpColor.rgbGreen, tpColor.rgbBlue ),
										drawMode, geoImage, geoSubImage, dwGeoBase );

			pEleBG = pEleBG->NextSiblingElement( "bgimg" );
		}
		
		TiXmlElement* pEleSkin = pEleSkins->FirstChildElement( "skin" );
		while( pEleSkin )
		{
			const char* lpszText = NULL;
			CDigitNum::SKIN_TYPE type = CDigitNum::SKIN_INVALID;
			int 			nSkinPiece = 0;
			int 			nDecimalPlaces = 5;
			
			HashedString hashedImagePath;
			ClGeometry	geoImage			= GEO_FULL_SIZE;
			ClGeometry	geoSubImage			= GEO_FULL_SIZE;
			RGBQUAD		shadeColor			= {255,255,255,255};
			RGBQUAD		tpColor				= {255,0,0,0};
			BOOL		bUseTpColor			= FALSE;
			ClDrawImage::DRAW_MODE drawMode = ClDrawImage::MODE_STRETCH;
			DWORD		dwGeoBase			= 0;

			lpszText = pEleSkin->Attribute( "type" );
			switch( lpszText[0] )
			{
			case 'd':	if( !strcmp(lpszText, "digit") )		type = CDigitNum::SKIN_DIGIT;	else type = CDigitNum::SKIN_DOT;	break;	// ClButton::SKIN_CHECKED
			case 'u':	if( !strcmp(lpszText, "unit") )		type = CDigitNum::SKIN_UNIT;	else type = CDigitNum::SKIN_INVALID;	break;	// ClButton::SKIN_DISABLED
			default:	type = CDigitNum::SKIN_INVALID;															// ClButton::SKIN_INVALID
			}

			if( type == CDigitNum::SKIN_INVALID )
			{
				CLD_FL( DF_ERROR, "Can not parse digitnum type(%d)", type );
				return FALSE;
			}
			
			lpszText = pEleSkin->Attribute( "supplant");
			if(lpszText) 
			{
			}

			if(pEleSkin->Attribute( "decimalPlaces", &nDecimalPlaces))
				pDigitNum->SetPrecision(nDecimalPlaces);
			
			pEleSkin->Attribute( "count", &nSkinPiece );
				
			
			if( type >= (INT)CDigitNum::SKIN_INVALID )
			{
				CLD_FL( DF_ERROR, "Can not parse button skin type(%d)", pEleSkin->Attribute("type") );
				return FALSE;
			}

			if( this->parseImageProp(pEleSkin, hashedImagePath, &shadeColor, &bUseTpColor, &tpColor, &drawMode, geoImage, geoSubImage, &dwGeoBase ) )
				pDigitNum->SetSkin( type, nSkinPiece, hashedImagePath, Color(shadeColor.rgbReserved, shadeColor.rgbRed, shadeColor.rgbGreen, shadeColor.rgbBlue),
									bUseTpColor, Color(tpColor.rgbReserved, tpColor.rgbRed, tpColor.rgbGreen, tpColor.rgbBlue),
									drawMode, geoImage, geoSubImage, dwGeoBase );

			pEleSkin = pEleSkin->NextSiblingElement( "skin" );
		}
	}

	return TRUE;
}

BOOL ClCludeDialog::parseAnalogMeter( TiXmlElement* pEleControl, ClContainer* pContainer )
{
	if( !pEleControl || !pContainer ) return FALSE;

	CAnalogMeter* pAnalogMeter = (CAnalogMeter*)pContainer->GetControl();

	TiXmlElement* pEleBG = pEleControl->FirstChildElement( "bgimg" );
	if( pEleBG )
	{
		HashedString hashedImagePath;
		ClGeometry	geoImage			= GEO_FULL_SIZE;
		ClGeometry	geoSubImage			= GEO_FULL_SIZE;
		RGBQUAD		shadeColor			= {255,255,255,255};
		RGBQUAD		tpColor				= {255,0,0,0};
		BOOL		bUseTpColor			= FALSE;
		ClDrawImage::DRAW_MODE drawMode	= ClDrawImage::MODE_STRETCH;
		DWORD		dwGeoBase			= 0;

		if( this->parseImageProp(pEleBG, hashedImagePath, &shadeColor, &bUseTpColor, &tpColor, &drawMode, geoImage, geoSubImage, &dwGeoBase) )
			pAnalogMeter->SetBackground( hashedImagePath, Color(shadeColor.rgbReserved, shadeColor.rgbRed, shadeColor.rgbGreen, shadeColor.rgbBlue),
										bUseTpColor, Color(tpColor.rgbReserved, tpColor.rgbRed, tpColor.rgbGreen, tpColor.rgbBlue),
										drawMode, geoImage, geoSubImage, dwGeoBase );
	}
	

	TiXmlElement* pEleSetting = pEleControl->FirstChildElement( "setting" );
	if( pEleSetting )
	{
		const char* lpszText = NULL;
		int 			nDecimalPlaces = 5;
		int 			nMaxValue = 8;
		int			nDivision = 1000;
		
		HashedString hashedImagePath;
		ClGeometry	geoImage			= GEO_FULL_SIZE;
		ClGeometry	geoSubImage			= GEO_FULL_SIZE;
		RGBQUAD		shadeColor			= {255,255,255,255};
		RGBQUAD		tpColor				= {255,0,0,0};
		RGBQUAD		rgbColor				= {255,0,0,0};
		COLORREF 		colorGrid				= 0x7f7f7f;
		COLORREF 		colorNeedle			= 0xff0000;
		COLORREF 		colorValue				= 0xffffff;
		
		BOOL		bUseTpColor			= FALSE;
		ClDrawImage::DRAW_MODE drawMode = ClDrawImage::MODE_STRETCH;
		DWORD		dwGeoBase			= 0;

		pEleSetting->Attribute( "decimalPlaces", &nDecimalPlaces);
		pEleSetting->Attribute( "maxValue", &nMaxValue);
		pEleSetting->Attribute( "division", &nDivision); // 1/min x1000

		pAnalogMeter->SetRangeDecimals(0) ;
		pAnalogMeter->SetValueDecimals(nDecimalPlaces);
		pAnalogMeter->SetRange(0, nMaxValue);
		pAnalogMeter->SetMeterScale(nDivision);

		lpszText = pEleSetting->Attribute( "gridcolor" );
		if( lpszText )
		{
			if( ClUtil::CLUDE::parseColor(lpszText, &rgbColor.rgbReserved, &rgbColor.rgbRed,
												&rgbColor.rgbGreen, &rgbColor.rgbBlue) )
				colorGrid = RGB(rgbColor.rgbRed, rgbColor.rgbGreen, rgbColor.rgbBlue);
		}

		lpszText = pEleSetting->Attribute( "valuecolor" );
		if( lpszText )
		{
			if( ClUtil::CLUDE::parseColor(lpszText, &rgbColor.rgbReserved, &rgbColor.rgbRed,
												&rgbColor.rgbGreen, &rgbColor.rgbBlue) )
				colorValue = RGB(rgbColor.rgbRed, rgbColor.rgbGreen, rgbColor.rgbBlue);
		}

		lpszText = pEleSetting->Attribute( "needlecolor" );
		if( lpszText )
		{
			if( ClUtil::CLUDE::parseColor(lpszText, &rgbColor.rgbReserved, &rgbColor.rgbRed,
												&rgbColor.rgbGreen, &rgbColor.rgbBlue) )
				colorNeedle = RGB(rgbColor.rgbRed, rgbColor.rgbGreen, rgbColor.rgbBlue);
		}

		pAnalogMeter->SetColor(colorGrid, colorValue, colorNeedle);
		
	}
	
	return TRUE;
}

//================================================================================================= E. CONTROL
//================================================================================================= S. GET/SET


//================================================================================================= E. GET/SET
//================================================================================================= S. STATIC
//================================================================================================= E. STATIC
//================================================================================================= EOF

