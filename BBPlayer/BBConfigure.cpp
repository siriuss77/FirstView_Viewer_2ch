
#include "stdafx.h"
#include "BBConfigure.h"

#include <HELPER/ClLog.H>


CBBConfigure::CBBConfigure()
{
	this->m_cstrBrandName = TEXT("");
	this->m_cstrModelName = TEXT("");
	this->m_cstrFirmVersion = TEXT("");
}


CBBConfigure::~CBBConfigure()
{

}


BOOL CBBConfigure::readConfig( const CString& cstrPath )
{
	CFile		configFile;
	INT			nLen;

	if( !configFile.Open(cstrPath, CFile::modeRead | CFile::typeBinary, NULL) )
	{
		CLD_FL( DF_ERROR, "Can not open configuration file." );
		return FALSE;
	}

	this->clearConfig();

	nLen = (INT)configFile.GetLength();

	CHAR strBrandName[SZ_MAX_BRAND_NAME] = { NULL, };
	CHAR strModelName[SZ_MAX_MODEL_NAME] = { NULL, };

	configFile.Read( strBrandName, SZ_MAX_BRAND_NAME );
	configFile.Read( strModelName, SZ_MAX_MODEL_NAME );

	CString cstrName;

	int nStrLen = MultiByteToWideChar( CP_ACP, 0, strBrandName, -1, 0, 0 );
	TCHAR* lptzString = new TCHAR[nStrLen+1];
	MultiByteToWideChar( CP_ACP, 0, strBrandName, -1, lptzString, nStrLen );
	cstrName.Format( TEXT("%s"), lptzString );
	if( lptzString ) { delete [] lptzString; lptzString = NULL; }
	this->setBrandName( cstrName );

	nStrLen = MultiByteToWideChar( CP_ACP, 0, strModelName, -1, 0, 0 );
	lptzString = new TCHAR[nStrLen+1];
	MultiByteToWideChar( CP_ACP, 0, strModelName, -1, lptzString, nStrLen );
	cstrName.Format( TEXT("%s"), lptzString );
	if( lptzString ) delete [] lptzString;
	this->setModelName( cstrName );

	INT nFileSize = this->m_ConfigPool.size() * sizeof(INT);
	nFileSize += SZ_MAX_BRAND_NAME + SZ_MAX_MODEL_NAME;

	INT* pFileBuffer = new INT[this->m_ConfigPool.size()];
	if( nLen != nFileSize )
	{
		this->clearConfig();
		CLD_FL( DF_ERROR, "Configuration file size does not matched." );
		return FALSE;
	}

	configFile.Read( pFileBuffer, nLen - SZ_MAX_BRAND_NAME - SZ_MAX_MODEL_NAME );
	configFile.Close();

	INT* pOffset = pFileBuffer;
	ITER_BBCONFIG iConfig = this->m_ConfigPool.begin();
	ITER_BBCONFIG iConfigEnd = this->m_ConfigPool.end();
	for( ; iConfig != iConfigEnd; iConfig ++ )
	{
		iConfig->m_Value = *pOffset;
		pOffset ++;
	}

	if( pFileBuffer ) delete [] pFileBuffer;

//	INT nVersion;
//	if( this->getConfig( TEXT("FIRMWARE_VERSION"), NULL, &nVersion, NULL) )
//		this->setFirmVersion( nVersion );

	return TRUE;
}


BOOL CBBConfigure::writeConfig( const CString& cstrPath )
{
	CFile	configFile;
	if( !configFile.Open(cstrPath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary, NULL) )
	{
		CLD_FL( DF_ERROR, "Can not create configuration file." );
		return FALSE;
	}

	INT nConfigSize = this->m_ConfigPool.size() * sizeof(INT);

	INT*	pFileBuffer = new INT[nConfigSize];
	ZeroMemory( pFileBuffer, nConfigSize );
	INT*	pOffset = pFileBuffer;

	ITER_BBCONFIG iConfig		= this->m_ConfigPool.begin();
	ITER_BBCONFIG iConfigEnd	= this->m_ConfigPool.end();
	for( ; iConfig != iConfigEnd; iConfig++ )
	{
		*pOffset = iConfig->m_Value;
		pOffset ++;
	}

	CHAR strBrandName[SZ_MAX_BRAND_NAME] = { NULL, };
	CHAR strModelName[SZ_MAX_MODEL_NAME] = { NULL, };

	strcpy_s( strBrandName, SZ_MAX_BRAND_NAME, CT2A(this->m_cstrBrandName) );
	strcpy_s( strModelName, SZ_MAX_BRAND_NAME, CT2A(this->m_cstrModelName) );

	configFile.Write( strBrandName, SZ_MAX_BRAND_NAME );
	configFile.Write( strModelName, SZ_MAX_MODEL_NAME );
	configFile.Write( pFileBuffer, nConfigSize );
	configFile.Close();

	if( pFileBuffer ) delete [] pFileBuffer;

	return TRUE;
}


BOOL CBBConfigure::cloneConfig( CBBConfigure& srcConfigure )
{
	if( srcConfigure.m_ConfigPool.empty() )
		return FALSE;

	this->clearConfig();
	BBCONFIG config;

	CONST INT nListSize = srcConfigure.getConfigListSize();
	INT nIndex;
	for( nIndex = 0; nIndex < nListSize; nIndex ++ )
	{
		if( !srcConfigure.getConfig(nIndex, &config) )
		{
			CLD_FL( DF_ERROR, "configuration cloning failed." );
			return FALSE;
		}

		this->addConfig( config.m_hashedName, config.m_type, config.m_Value, config.m_DefaultValue );
	}

	return TRUE;
}


BOOL CBBConfigure::isEmptyConfig( VOID )
{
	return this->m_ConfigPool.empty();
}


INT CBBConfigure::getConfigListSize( VOID )
{
	return this->m_ConfigPool.size();
}


VOID CBBConfigure::addConfig( const HashedString& configName, BBCONFIG_TYPE type, INT value, INT defaultValue )
{
	ITER_BBCONFIG iConfig = this->m_ConfigPool.begin();
	ITER_BBCONFIG iConfigEnd = this->m_ConfigPool.end();
	for( ; iConfig != iConfigEnd; iConfig++ )
		ASSERT( iConfig->m_hashedName != configName );

	BBCONFIG config;
	config.m_hashedName		= configName;
	config.m_type			= type;
	config.m_Value			= value;
	config.m_DefaultValue	= defaultValue;

	this->m_ConfigPool.push_back( config );

	return;
}


BOOL CBBConfigure::getConfig( const HashedString& configName, BBCONFIG_TYPE* pType, INT* pValue, INT* pDefaultValue )
{
	ITER_BBCONFIG iConfig = this->m_ConfigPool.begin();
	ITER_BBCONFIG iConfigEnd = this->m_ConfigPool.end();
	for( ; iConfig != iConfigEnd; iConfig++ )
	{
		if( iConfig->m_hashedName == configName )
			break;
	}

	if( iConfig == iConfigEnd )
	{
		CLD_FL( DF_ERROR, "Can not find configuration item by given configuration name." );
		return FALSE;
	}

	if( pType )			*pType	= iConfig->m_type;
	if( pValue )		*pValue = iConfig->m_Value;
	if( pDefaultValue )	*pDefaultValue = iConfig->m_DefaultValue;

	return TRUE;
}


BOOL CBBConfigure::getConfig( int nIndex, BBCONFIG* pConfig )
{
	if( (nIndex < 0) || !pConfig )
		return FALSE;

	ITER_BBCONFIG iConfig = this->m_ConfigPool.begin();
	std::advance( iConfig, nIndex );

	if( iConfig == this->m_ConfigPool.end() )
		return FALSE;

	pConfig->m_hashedName	= iConfig->m_hashedName;
	pConfig->m_type			= iConfig->m_type;
	pConfig->m_Value		= iConfig->m_Value;
	pConfig->m_DefaultValue	= iConfig->m_DefaultValue;

	return TRUE;
}


BOOL CBBConfigure::setConfig( const HashedString& configName, INT nValue )
{
	ITER_BBCONFIG iConfig = this->m_ConfigPool.begin();
	ITER_BBCONFIG iConfigEnd = this->m_ConfigPool.end();
	for( ; iConfig != iConfigEnd; iConfig++ )
	{
		if( iConfig->m_hashedName == configName )
			break;
	}

	if( iConfig == iConfigEnd )
	{
		CLD_FL( DF_ERROR, "Can not find configuration item by given configuration name." );
		return FALSE;
	}

	iConfig->m_Value = nValue;

	return TRUE;
}


BOOL CBBConfigure::removeConfig( const HashedString& configName )
{
	ITER_BBCONFIG iConfig = this->m_ConfigPool.begin();
	ITER_BBCONFIG iConfigEnd = this->m_ConfigPool.end();
	for( ; iConfig != iConfigEnd; iConfig++ )
	{
		if( iConfig->m_hashedName == configName )
			break;
	}

	if( iConfig == iConfigEnd )
	{
		CLD_FL( DF_ERROR, "Can not find configuration item by given configuration name." );
		return FALSE;
	}

	this->m_ConfigPool.erase( iConfig );

	return TRUE;
}


VOID CBBConfigure::clearConfig( VOID )
{
	this->m_ConfigPool.clear();
}


BOOL CBBConfigure::setBrandName( const CString& cstrBrandName )
{
	if( !cstrBrandName.GetLength() || cstrBrandName.GetLength() >= SZ_MAX_MODEL_NAME )
		return FALSE;

	this->m_cstrBrandName = cstrBrandName;

	return TRUE;
}


const CString& CBBConfigure::getBrandName( VOID )
{
	return this->m_cstrBrandName;
}


const CString& CBBConfigure::getModelName( VOID )
{
	return this->m_cstrModelName;
}


VOID CBBConfigure::setFirmVersion( const CString& cstrFirmVersion )
{
	this->m_cstrFirmVersion = cstrFirmVersion;
	return;
}


const CString& CBBConfigure::getFirmVersion( VOID )
{
	return this->m_cstrFirmVersion;
}


BOOL CBBConfigure::setModelName( const CString& cstrModelName )
{
	if( !cstrModelName.GetLength() || cstrModelName.GetLength() >= SZ_MAX_MODEL_NAME )
		return FALSE;

	this->clearConfig();
	this->m_cstrModelName = cstrModelName;

	if( !this->m_cstrModelName.Compare(TEXT("CLBB-000")) )
	{
		this->addConfig( TEXT("FIRMWARE_VERSION"),		TYPE_FLOAT100,	100,	100	);	// Firmware version

		this->addConfig( TEXT("AV_VIDEO_QUALITY"),		TYPE_INTEGER,	5,		5	);	// Video Quality(1 ~ 10)// jun 20200908
		this->addConfig( TEXT("AV_AUDIO_ENABLED"),		TYPE_BOOLEAN,	1,		1	);	// Audio Enable

		this->addConfig( TEXT("PM_VIDEO_QUALITY"),		TYPE_INTEGER,	0,		0	);	// [Parking Mode Video Quality] 0 : LOW,  1 : HIGH
		this->addConfig( TEXT("PM_SENSE_IMPACT"),		TYPE_INTEGER,	5,		5	);	// [Parking Mode Impact Sensitivity] 1 ~ 10
		this->addConfig( TEXT("PM_SENSE_MOTION"),		TYPE_INTEGER,	5,		5	);	// [Parking Mode Motion Sensitivity] 1 ~ 10

		this->addConfig( TEXT("SYSTEM_SENSE_GSENSOR"),	TYPE_INTEGER,	5,		5	);	// [G-Sensor Sensitivity] 1 ~ 10
		this->addConfig( TEXT("SYSTEM_PARTITION"),		TYPE_INTEGER,	0,		0	);	// [Disk Partition option] 0 ~ 2

	}
	else if( !this->m_cstrModelName.Compare(TEXT("CLBB-001")) )
	{
		this->addConfig( TEXT("FIRMWARE_VERSION"),		TYPE_FLOAT100,	100,	100	);	// Firmware version

		this->addConfig( TEXT("AV_VIDEO_QUALITY"),		TYPE_INTEGER,	5,		5	);	// Video Quality
		this->addConfig( TEXT("AV_AUDIO_ENABLED"),		TYPE_BOOLEAN,	1,		1	);	// Audio Enable

		this->addConfig( TEXT("PM_VIDEO_QUALITY"),		TYPE_INTEGER,	0,		0	);	// [Parking Mode Video Quality] 0 : LOW,  1 : HIGH
		this->addConfig( TEXT("PM_SENSE_IMPACT"),		TYPE_INTEGER,	5,		5	);	// [Parking Mode Impact Sensitivity] 1 ~ 10
		this->addConfig( TEXT("PM_SENSE_MOTION"),		TYPE_INTEGER,	5,		5	);	// [Parking Mode Motion Sensitivity] 1 ~ 10

		this->addConfig( TEXT("SYSTEM_SENSE_GSENSOR"),	TYPE_INTEGER,	5,		5	);	// [G-Sensor Sensitivity] 1 ~ 10
		this->addConfig( TEXT("SYSTEM_PARTITION"),		TYPE_INTEGER,	0,		0	);	// [Disk Partition option] 0 ~ 2
	}
	else
	{
		CLD_FL( DF_ERROR, "Try to configure by unknown model name" );
		return FALSE;
	}

	return TRUE;
}

//================================================================================================= S. STATIC

BOOL CBBConfigure::writeTime( const CString& cstrTimePath, SYSTEMTIME time )
{
	CStdioFile	timeFile;

	if( !timeFile.Open(cstrTimePath, CFile::modeCreate | CFile::modeWrite | CFile::typeText, NULL) )
	{
		CLD_FL( DF_ERROR, "Can not open configuration file." );
		return FALSE;
	}

	CString cstrTime;
	cstrTime.Format( TEXT("%d %02d %02d %02d %02d %02d"),
		time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond );
	timeFile.WriteString( cstrTime );

	timeFile.Close();

	return TRUE;
}
