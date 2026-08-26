// BBConfigure.h : header file
//

#pragma once

//================================================================================================= S.INCLUDE

#include "stdafx.h"

#include <HELPER/ClHashedString.H>
#include <list>

//================================================================================================= E.INCLUDE
//================================================================================================= S.PREPROCESSOR

#define	BBCONFIG_FILE									TEXT("setup.cfg")
#define	SZ_MAX_MODEL_NAME								32
#define	SZ_MAX_BRAND_NAME								32

//================================================================================================= E.PREPROCESSOR
//================================================================================================= S.TYPEDEF

//================================================================================================= E.TYPEDEF
//================================================================================================= S.CLASS

class CBBConfigure
{
public:
	CBBConfigure();
	~CBBConfigure();

	//----------------------------------------------------------------------------------- S.TYPEDEF
	
	enum BBCONFIG_TYPE
	{
		TYPE_INTEGER	= 0	,
		TYPE_BOOLEAN		,
		TYPE_FLOAT10		,
		TYPE_FLOAT100		,
		TYPE_FLOAT1000		,
	};

	typedef struct _tagBBCONFIG
	{
		HashedString	m_hashedName;
		BBCONFIG_TYPE	m_type;
		INT				m_Value;
		INT				m_DefaultValue;

	} BBCONFIG, &PBBCONFIG;

	typedef std::list<BBCONFIG>									BBCONFIG_POOL;
	typedef BBCONFIG_POOL::iterator								ITER_BBCONFIG;

	//----------------------------------------------------------------------------------- E.TYPEDEF

public:

			BOOL					readConfig			( const CString& cstrPath );
			BOOL					writeConfig			( const CString& cstrPath );
			BOOL					cloneConfig			( CBBConfigure& configure );

			BOOL					isEmptyConfig		( VOID );
			INT						getConfigListSize	( VOID );
			VOID					addConfig			( const HashedString& configName, BBCONFIG_TYPE type, INT nValue, INT defaultValue );
			BOOL					getConfig			( const HashedString& configName, BBCONFIG_TYPE* pType, INT* pValue, INT* pDefaultValue );
			BOOL					getConfig			( int nIndex, BBCONFIG* pConfig );
			BOOL					setConfig			( const HashedString& configName, INT nValue );
			BOOL					removeConfig		( const HashedString& configName );
			VOID					clearConfig			( VOID );

			BOOL					setBrandName		( const CString& cstrBrandName );
			const CString&			getBrandName		( VOID );

			BOOL					setModelName		( const CString& cstrBrandName );
			const CString&			getModelName		( VOID );

			VOID					setFirmVersion		( const CString& cstrFirmVersion);
			const CString&			getFirmVersion		( VOID );

public:

	static	BOOL					writeTime			( const CString& cstrTimePath, SYSTEMTIME time );

private:

			BBCONFIG_POOL			m_ConfigPool;
			CString					m_cstrBrandName;
			CString					m_cstrModelName;
			CString					m_cstrFirmVersion;
};

//================================================================================================= E.CLASS
