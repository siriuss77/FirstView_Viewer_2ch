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
//================================================================================================= S. INCLUDE

#include "stdafx.h"
#include "wbcTextData.h"

//================================================================================================= E. INCLUDE
//================================================================================================= S. CON/DCON

CWBCSensorData::CWBCSensorData()
{
	m_gseonsorX = 0;
	m_gseonsorY = 0;
	m_gseonsorZ = 0;
	m_cts       = 0;
}

CWBCSensorData::~CWBCSensorData()
{

}


void CWBCSensorData::setData( EGSENSORTYPE type, DWORD dwX, DWORD dwY, DWORD dwZ, ULONG ulCTS )
{
    switch( type )
    {
    case GSENSOR_OLD_8_BIT:
    case GSENSOR_8_BIT:
        {
            m_gseonsorX = (0x00ff & ~dwX) + 1 - 128;
            m_gseonsorY = (0x00ff & ~dwY) + 1 - 128;
            m_gseonsorZ = (0x00ff & ~dwZ) + 1 - 128;

        } break;

    case GSENSOR_10_BIT:
        {
            m_gseonsorX = (0x03ff & ~dwX) + 1 - 512;
            m_gseonsorY = (0x03ff & ~dwY) + 1 - 512;
            m_gseonsorZ = (0x03ff & ~dwZ) + 1 - 512;

        } break;
    }

    this->m_cts = ulCTS;
    this->m_eGSensorType = type;

    return;
}

void CWBCSensorData::operator=(CWBCSensorData &newData)
{
	this->m_gseonsorX   = newData.m_gseonsorX;
	this->m_gseonsorY   = newData.m_gseonsorY;
	this->m_gseonsorZ   = newData.m_gseonsorZ;
	this->m_cts         = newData.m_cts;
}

//================================================================================================= E. CON/DCON
//================================================================================================= S. CON/DCON

CWBCGPSData::CWBCGPSData()
{
	m_nYear     = 0;
	m_nMonth    = 0;
	m_nDay      = 0;
	m_nHour     = 0;
	m_nMinute   = 0;
	m_nSecond   = 0;
	m_bValid    = false;
	m_fLat      = 0.0;
	m_fLng      = 0.0; // +북위/동경, -남위/서경
	m_fSpeed    = 0.0;
	m_fAzimuth  = 0.0;
	
	m_cts = 0;
}


CWBCGPSData::CWBCGPSData(unsigned char * pStr, unsigned long cts)
{
	ParsingGPS(pStr, cts);
}


CWBCGPSData::~CWBCGPSData()
{

}

void CWBCGPSData::operator=(CWBCGPSData &newData)
{
	this->m_nYear       = newData.m_nYear;
	this->m_nMonth      = newData.m_nMonth;
	this->m_nDay        = newData.m_nDay;
	this->m_nHour       = newData.m_nHour;
	this->m_nMinute     = newData.m_nMinute;
	this->m_nSecond     = newData.m_nSecond;
	this->m_bValid      = newData.m_bValid;
	this->m_fLat        = newData.m_fLat;
	this->m_fLng        = newData.m_fLng; // +북위/동경, -남위/서경
	this->m_fSpeed      = newData.m_fSpeed;
	this->m_fAzimuth    = newData.m_fAzimuth;
	this->m_cts         = newData.m_cts;
}

//================================================================================================= E. CON/DCON
//================================================================================================= S. OPERATION

bool CWBCGPSData::ParsingGPS(unsigned char *pStr, unsigned long cts)
{
	char buf[1024];
	char seps[] = ",";
	char *token;

	m_cts = cts;

	//strcpy((char*)buf, (char*)pStr);
	strcpy_s((char*)buf, sizeof(buf),(char*)pStr);

	token = StrToken(buf, seps);

	for (int i = 0; i < CountOfGprmcPos; i++)
	{
		if (token == NULL)
		{
			return false;
		}

		switch(i)
		{
		case posTime:
			SetTime(token);
			break;
		case posValid:
			m_bValid = (*token == 'A');
			break;
		case posLatV:
			m_fLat = (double)atof(token);
			break;
		case posLatNS:
			if (*token != 'N')
			{
				m_fLat *= -1;
			}
			break;
		case posLngV:
			m_fLng = (double)atof(token);
			break;
		case posLngEW:
			if (*token != 'E')
			{
				m_fLng *= -1;
			}
			break;
		case posKnotSpeed:
			m_fSpeed = (float)(atof(token) *1.852);
			break;
		case posAzimuth:
			m_fAzimuth = (float)atof(token);
			break;
		case posDate:
			SetDate(token);
			break;
		default:;
		}

		token = StrToken(NULL, seps);
	}

	AdjustDateTime();

	return true;
}


char * CWBCGPSData::StrToken(char * s1, const char * delimit)
{
	static char *lastToken = NULL;
	char *tmp;

	if (s1 == NULL)
	{
		s1 = lastToken;
		if (s1 == NULL)
		{
			return NULL;
		}
	}
	else 
	{
		s1 += strspn(s1, delimit);
	}

	tmp = strpbrk(s1, delimit);
	if (tmp)
	{
		*tmp = '\0';
		lastToken = tmp + 1;
	}
	else
	{
		lastToken = NULL;
	}

	return s1;
}

void CWBCGPSData::SetTime(char * pStr)
{
	char buf[3] = {0};

	strncpy_s(buf, pStr, 2);
	m_nHour = atoi(buf) + 9;
	strncpy_s(buf, pStr+2, 2);
	m_nMinute = atoi(buf);
	strncpy_s(buf, pStr+4, 2);
	m_nSecond = atoi(buf);
}

void CWBCGPSData::SetDate(char * pStr)
{
	char buf[3] = {0};

	strncpy_s(buf, pStr, 2);
	m_nDay = atoi(buf);
	strncpy_s(buf, pStr+2, 2);
	m_nMonth = atoi(buf);
	strncpy_s(buf, pStr+4, 2);
	m_nYear = atoi(buf);
}

void CWBCGPSData::AdjustDateTime(void)
{
	struct tm tmThis;

	tmThis.tm_year = 70+((m_nYear+30)%100);
	tmThis.tm_mon = m_nMonth-1;
	tmThis.tm_mday = m_nDay;
	tmThis.tm_hour = m_nHour;
	tmThis.tm_min = m_nMinute;
	tmThis.tm_sec = m_nSecond;

	//m_time = CTime(mktime(&tmThis));
}


double CWBCGPSData::FromDegree(double dbValue)
{
	double d = (int)dbValue;
	double m = ((dbValue - d) * 100.0);

	return d + m / 60.0;
}


double CWBCGPSData::GetLat(void)
{
	return FromDegree(m_fLat * 0.01);
}

double CWBCGPSData::GetLng(void)
{
	return FromDegree(m_fLng * 0.01);
}
//================================================================================================= E. OPERATION
//================================================================================================= S. CON/DCON

CWBCTextData::CWBCTextData()
{
}

CWBCTextData::~CWBCTextData()
{
}

//================================================================================================= E. CON/DCON
//================================================================================================= S. OPERATION

int CWBCTextData::ParseTextData(unsigned char * pTextData, unsigned long cts)
{
    DWORD dwX, dwY, dwZ;

	if( pTextData[0] == 'M' )	// G-Sensor Data
	{
        CWBCSensorData data;

        dwX = pTextData[1];
        dwY = pTextData[2];
        dwZ = pTextData[3];
		
        data.setData( CWBCSensorData::GSENSOR_OLD_8_BIT, dwX, dwY, dwZ, cts );

		m_GSensorList.push_back(data);
	}
    else if( pTextData[0] == 'S' )
    {
        CWBCSensorData data;
        unsigned short* pData = (unsigned short*)&pTextData[2];

        dwX = *pData ++;
        dwY = *pData ++;
        dwZ = *pData;

        if( pTextData[1] == 8 )         data.setData( CWBCSensorData::GSENSOR_8_BIT, dwX, dwY, dwZ, cts );
        else if( pTextData[1] == 10 )   data.setData( CWBCSensorData::GSENSOR_10_BIT, dwX, dwY, dwZ, cts );

        m_GSensorList.push_back(data);
    }
	else if( pTextData[0] == 'G' )
	{
		CWBCGPSData data(pTextData, cts);

		m_GPSList.push_back(data);
	}

	return 0;
}

//================================================================================================= S. OPERATION
