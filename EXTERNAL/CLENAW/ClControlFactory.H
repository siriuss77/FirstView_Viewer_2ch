//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClControlFactory.H
//
//	Description: CLENA UI Control Factory
//
//	Update History
// 
//		2011.10.10 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. FILEID

#pragma once

//================================================================================================= E. FILEID
//================================================================================================= S. INCLUDE

#include <HELPER/ClHashedString.H>
#include <map>

//================================================================================================= E. INCLUDE
//================================================================================================= S. PREPROCESSOR

//================================================================================================= E. PREPROCESSOR
//================================================================================================= S. CLASS

class ClContainer;

class ClControlFactory
{

public:
	ClControlFactory();
	~ClControlFactory();

	//----------------------------------------------------------------------------------- S. TYPEDEF

	typedef ClContainer* (*LPFN_CREATECONTROL)();
	class CFactoryItem
	{
		public:	CFactoryItem(){m_createControlFn = NULL;}
		public:	LPFN_CREATECONTROL m_createControlFn;
	};
	
	typedef std::map<HashedString,CFactoryItem>					FACTORY_POOL;
	typedef FACTORY_POOL::iterator								ITER_FACTORY;
	typedef std::pair<HashedString,CFactoryItem>				PAIR_FACTROY;
	//----------------------------------------------------------------------------------- E. TYPEDEF

public:

			bool					RegisterFactory		( const HashedString& name, LPFN_CREATECONTROL lpfnCreateControl );
			ClContainer*			CreateControl		( const HashedString& name );
			ClContainer*			CreateControl		( const char* lpszName );

public:

	static	ClContainer*			createButtonControl	( void );
	static	ClContainer*			createImageControl	( void );
	static	ClContainer*			createTextControl	( void );
	static	ClContainer*			createSliderControl	( void );
	static	ClContainer*			createListboxControl( void );
	static	ClContainer*			createVideoControl	( void );
	static	ClContainer*			createWebControl	( void );
	static	ClContainer*			createGSensorGraph	( void );
	static	ClContainer*			createDigitControl( void );
	static 	ClContainer*			createAnalogMeterControl( void );
	
private:

			void					clearFactoryPool	( void );

private:

			FACTORY_POOL			m_FactoryPool;
};

//================================================================================================= E. CLASS
