//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClContainer.H
//
//	Description: CLENA-W Window control container
//
//	Update History
// 
//		2011.10.10 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. FILEID

#pragma once

//================================================================================================= E. FILEID
//================================================================================================= S. INCLUDE

#include <HELPER/ClGeometry.H>
#include <HELPER/ClDrawImage.H>
#include <HELPER/ClDrawText.H>
#include <CLENAW/ClTypes.H>
#include <CLENAW/ClIBGTransfer.H>

#include <list>
#include <map>

//================================================================================================= E. INCLUDE
//================================================================================================= S. CLASS

class ClContainer
{

public:

	ClContainer();
	virtual ~ClContainer();

	//-------------------------------------------------------------------------------------- S.TYPEDEF

	typedef std::map<int,MODEINFO>						MODEINFO_POOL;
	typedef MODEINFO_POOL::iterator						ITER_MODEINFO;
	typedef std::pair<int,MODEINFO>						PAIR_MODEINFO;

	//-------------------------------------------------------------------------------------- E.TYPEDEF

public:

			BOOL					CreateControl		( CWnd* pParent );
			BOOL					AdaptMode			( INT nMode );


public:
			BOOL					SetControl			( CWnd* pControl );
			CWnd*					GetControl			( VOID );
			IBGTransfer*			GetBgInterface		( VOID );

			BOOL					SetGeometry			( const ClGeometry& geoControl );
			const ClGeometry&		GetGeometry			( VOID );

			UINT					GetResourceID		( VOID );
			VOID					SetResourceID		( UINT nResourceID );

			const HashedString&		GetControlID		( VOID );
			BOOL					SetControlID		( const HashedString& name );
			BOOL					SetControlID		( const char* lpszName );

			CONTROL_TYPE			GetType				( VOID );
			BOOL					SetType				( CONTROL_TYPE type );

			BOOL					AddModeInfo			( INT nMode, const ClGeometry& geometry, DWORD dwGeoBase );
			PMODEINFO				GetModeInfo			( INT nMode );

protected:

			CWnd*					m_pControl;
			IBGTransfer*			m_pBgInterface;


			MODEINFO_POOL			m_modeInfoList;

			UINT					m_resourceID;
			HashedString			m_controlID;
			CONTROL_TYPE			m_type;
};

//================================================================================================= E. CLASS
