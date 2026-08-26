//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: TextData.h
//
//	Description: Blackbox Sensor data class
//
//	Update History
// 
//		2011.10.10 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. FILEID

#pragma once

//================================================================================================= E. FILEID
//================================================================================================= S. INCLUDE

#include <list>

//================================================================================================= E. INCLUDE
//================================================================================================= S. CLASS

class CWBCSensorData
{
public:
	CWBCSensorData();
	~CWBCSensorData();

    typedef enum _eGSENSORTYPE
    {
        GSENSOR_OLD_8_BIT   = 0 ,
        GSENSOR_8_BIT           ,
        GSENSOR_10_BIT	,

	GSENSOR_AVIC_8_BIT,

	GSENSOR_TYPE_END

    } EGSENSORTYPE;

    void            setData( EGSENSORTYPE type, DWORD dwX, DWORD dwY, DWORD dwZ, ULONG ulCTS );

	int				m_gseonsorX;
	int				m_gseonsorY;
	int				m_gseonsorZ;
    EGSENSORTYPE    m_eGSensorType;

	unsigned long	m_cts;

	void operator=(CWBCSensorData &newData);
};

class CWBCGPSData
{
	
	enum GPRMCPOS 
	{
		posHead,
		posTime,
		posValid,
		posLatV,
		posLatNS,
		posLngV,
		posLngEW,
		posKnotSpeed,
		posAzimuth,
		posDate,
		CountOfGprmcPos
	};

public:
	CWBCGPSData();
	CWBCGPSData(unsigned char * pStr, unsigned long cts);
	virtual ~CWBCGPSData();

	int			m_nYear, m_nMonth, m_nDay, m_nHour, m_nMinute, m_nSecond;
	BOOL		m_bValid;
	double		m_fLat, m_fLng; // +북위/동경, -남위/서경
	double		m_fSpeed;
	double		m_fAzimuth;
	//CTime		m_time;
	unsigned long	m_cts;

	void operator=(CWBCGPSData &newData);
	bool ParsingGPS(unsigned char *pStr, unsigned long cts);
	char * StrToken(char * s1, const char * delimit);
	void SetTime(char * pStr);
	void SetDate(char * pStr);
	void AdjustDateTime(void);
	double FromDegree(double dbValue);
	double GetLat(void);
	double GetLng(void);
};


class CWBCTextData
{
public:
	CWBCTextData();
	virtual ~CWBCTextData();

	//====================================================================================== S. TYPEDEF

	typedef std::list<CWBCSensorData>                           GSENSOR_POOL;
	typedef GSENSOR_POOL::iterator								ITER_GSENSOR;

	typedef std::list<CWBCGPSData>								GPS_POOL;
	typedef GPS_POOL::iterator									ITER_GPS;

	//====================================================================================== E. TYPEDEF

	GSENSOR_POOL		m_GSensorList;
	GPS_POOL			m_GPSList;

	int ParseTextData(unsigned char * pTextData, unsigned long cts);
};

//================================================================================================= E. CLASS
