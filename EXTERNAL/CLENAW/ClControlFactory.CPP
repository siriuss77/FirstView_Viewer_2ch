//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClControlFactory.CPP
//
//	Description: CLENA UI Control Factory
//
//	Update History
// 
//		2011.10.10 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. INCLUDE

#include <stdafx.h>
#include "ClControlFactory.h"

#include <CLENAW/ClDialog.H>
#include <CLENAW/ClButton.H>
#include <CLENAW/ClText.H>
#include <CLENAW/ClImage.H>
#include <CLENAW/ClSlider.H>
#include <CLENAW/ClListCtrl.H>
#include <CLENAW/ClVideoView.H>
#include <CLENAW/ClWebBrowser.H>
#include <CLENAW/GSensorView.H>
#include <CLENAW/DigitNum.H>
#include <CLENAW/AnalogMeter.H>

#include <HELPER/ClLog.H>

//================================================================================================= E. INCLUDE
//================================================================================================= S. CON/DCON

ClControlFactory::ClControlFactory()
{
	this->RegisterFactory( TEXT("button"),	&ClControlFactory::createButtonControl	);
	this->RegisterFactory( TEXT("image"),	&ClControlFactory::createImageControl	);
	this->RegisterFactory( TEXT("text"),	&ClControlFactory::createTextControl	);
	this->RegisterFactory( TEXT("slider"),	&ClControlFactory::createSliderControl	);
	this->RegisterFactory( TEXT("map"),		&ClControlFactory::createWebControl		);
	this->RegisterFactory( TEXT("list"),	&ClControlFactory::createListboxControl	);
	this->RegisterFactory( TEXT("video"),	&ClControlFactory::createVideoControl	);
	this->RegisterFactory( TEXT("gsensor"),	&ClControlFactory::createGSensorGraph	);
	this->RegisterFactory( TEXT("digit"),	&ClControlFactory::createDigitControl	);
	this->RegisterFactory( TEXT("analogmeter"),	&ClControlFactory::createAnalogMeterControl );
}


ClControlFactory::~ClControlFactory()
{
	this->clearFactoryPool();
}

//================================================================================================= E. CON/DCON
//================================================================================================= S. OPERATION
//================================================================================================= E. OPERATION
//================================================================================================= S. CONTROL

bool ClControlFactory::RegisterFactory( const HashedString& name, LPFN_CREATECONTROL lpfnCreateControl )
{
	if( name.isNULL() ) return false;

	ITER_FACTORY iFactory = this->m_FactoryPool.find( name );
	if( iFactory == this->m_FactoryPool.end() )
	{
		CFactoryItem aFactoryItem;
		if( lpfnCreateControl )	aFactoryItem.m_createControlFn = lpfnCreateControl;

		this->m_FactoryPool.insert( PAIR_FACTROY(name, aFactoryItem) );
	}
	else
	{
		if( lpfnCreateControl )	iFactory->second.m_createControlFn = lpfnCreateControl;
	}

	return true;
}


ClContainer* ClControlFactory::CreateControl( const HashedString& name )
{
	ITER_FACTORY iFactory = this->m_FactoryPool.find( name );
	if( iFactory == this->m_FactoryPool.end() )
		return NULL;

	if( iFactory->second.m_createControlFn == NULL )
		return NULL;

	return iFactory->second.m_createControlFn();
}


ClContainer* ClControlFactory::CreateControl( const char* lpszName )
{
	if( !lpszName ) return NULL;

	int nLen = strlen(lpszName);
	char* lpszNewName = new char[nLen+1];
	strncpy_s( lpszNewName, nLen+1, lpszName, nLen );
	lpszNewName[nLen] = '\0';

	ClUtil::STRING::lowerString( lpszNewName );
	WCHAR* lpwzName = NULL;
	if( !ClUtil::STRING::convertUTF8ToUnicode(lpszName, &lpwzName) )
	{
		CLD_FL( DF_ERROR, "Can not copy string (UTF8 to UNICODE" );
		return NULL;
	}
	HashedString hashedName = lpwzName;
	if( lpwzName ) delete [] lpwzName;
	if( lpszNewName ) delete [] lpszNewName;

	return this->CreateControl( hashedName );
}


void ClControlFactory::clearFactoryPool( void )
{
	if( !this->m_FactoryPool.empty() )
		this->m_FactoryPool.clear();

	return;
}

//================================================================================================= E. CONTROL
//================================================================================================= S. GET/SET
//================================================================================================= E. GET/SET
//================================================================================================= S. STATIC

ClContainer* ClControlFactory::createButtonControl( void )
{
	ClContainer* pButtonContainer = new ClContainer();
	ClButton* pButton = new ClButton();
	pButtonContainer->SetControl( pButton );

	return pButtonContainer;
}


ClContainer* ClControlFactory::createImageControl( void )
{
	ClContainer* pImageContainer = new ClContainer();
	ClImage* pImage = new ClImage();
	pImageContainer->SetControl( pImage );

	return pImageContainer;
}


ClContainer* ClControlFactory::createTextControl( void )
{
	ClContainer* pTextContainer = new ClContainer();
	ClText* pText = new ClText();
	pTextContainer->SetControl( pText );

	return pTextContainer;
}


ClContainer* ClControlFactory::createSliderControl( void )
{
	ClContainer* pSliderContainer = new ClContainer();
	ClSlider* pSlider = new ClSlider();
	pSliderContainer->SetControl( pSlider );

	return pSliderContainer;
}


ClContainer* ClControlFactory::createListboxControl( void )
{
	ClContainer* pListViewContainer = new ClContainer();
	ClListCtrl* pListView = new ClListCtrl();
	pListViewContainer->SetControl( pListView );

	return pListViewContainer;
}


ClContainer* ClControlFactory::createVideoControl( void )
{
	ClContainer* pVideoContainer = new ClContainer();
	ClVideoView* pVideoView = new ClVideoView();
	pVideoContainer->SetControl( pVideoView );

	return pVideoContainer;
}


ClContainer* ClControlFactory::createWebControl( void )
{
	ClContainer* pWebContainer = new ClContainer();
	ClWebBrowser* pWebBrowser = new ClWebBrowser();
	pWebContainer->SetControl( pWebBrowser );

	return pWebContainer;
}


ClContainer* ClControlFactory::createGSensorGraph( void )
{
	ClContainer* pGSensorGraphContainer = new ClContainer();
	CGSensorView* pGSensorGraph = new CGSensorView();
	pGSensorGraphContainer->SetControl( pGSensorGraph );

	return pGSensorGraphContainer;
}

ClContainer* ClControlFactory::createDigitControl( void )
{
	ClContainer* pDigitControlContainer = new ClContainer();
	CDigitNum* pDigitNum = new CDigitNum();
	pDigitControlContainer->SetControl( pDigitNum );

	return pDigitControlContainer;
}

ClContainer* ClControlFactory::createAnalogMeterControl( void )
{
	ClContainer* pAnalogMeterControlContainer = new ClContainer();
	CAnalogMeter* pAnalogMeter = new CAnalogMeter();
	pAnalogMeterControlContainer->SetControl( pAnalogMeter );

	return pAnalogMeterControlContainer;
}
//================================================================================================= E. STATIC
//================================================================================================= EOF

