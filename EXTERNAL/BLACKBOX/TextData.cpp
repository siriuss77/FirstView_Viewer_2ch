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

#include <stdafx.h>
#include "TextData.h"

//================================================================================================= E. INCLUDE
//================================================================================================= S. CON/DCON

CGSensorData::CGSensorData()
{
	m_gseonsorX	= 0;
	m_gseonsorY 	= 0;
	m_gseonsorZ 	= 0;
	m_speed		= 0;
	m_cts 		= 0;
}

CGSensorData::CGSensorData(gsensor_acc *pAccel, float *pSpeed, unsigned long cts, unsigned char * time)
{
	float			 sensitivity = 256.0; //3 2g sensitivity 256 , 4g = 128, 8g =  64

	m_gseonsorX = (float)pAccel->x / sensitivity; 
	m_gseonsorY = (float)pAccel->y / sensitivity;
	m_gseonsorZ = (float)pAccel->z / sensitivity;

	if(pSpeed)
		m_speed		= *pSpeed;
	
	m_cts = cts;
	memcpy((void *)m_time, time, 6);
}

CGSensorData::~CGSensorData()
{

}

void CGSensorData::operator=(CGSensorData &newData)
{
	this->m_gseonsorX 	= newData.m_gseonsorX;
	this->m_gseonsorY 	= newData.m_gseonsorY;
	this->m_gseonsorZ 	= newData.m_gseonsorZ;
	this->m_speed		= newData.m_speed;
	this->m_cts 			= newData.m_cts;
	memcpy((void *)this->m_time, newData.m_time, sizeof(this->m_time));
}

//================================================================================================= E. CON/DCON
//================================================================================================= S. CON/DCON

CGPSData::CGPSData()
{
	m_nYear = 0;
	m_nMonth = 0;
	m_nDay = 0;
	m_nHour = 0;
	m_nMinute = 0;
	m_nSecond = 0;
	m_bNotDegree = FALSE;
	m_bValid = FALSE;

	m_sp_state = 0;
	m_fLat = 0.0;
	m_fLng = 0.0; // +북위/동경, -남위/서경
	m_fSpeed = 0.0;
	m_fAzimuth = 0.0;
	m_fPdop = 0.0;
	m_nCog = 0;
	
	m_cts = 0;
}


CGPSData::CGPSData(unsigned char * pStr, unsigned long cts)
{
	m_bValid = FALSE;
	m_bNotDegree = FALSE;
	if ( strncmp( (const char *)pStr, "$GPRMC", 6) == 0 )
		ParsingGPRMC(pStr, cts);
	else if ( strncmp( (const char *)pStr, "PRMC", 4) == 0 )
		ParsingGPRMC(pStr, cts);
	else if(strncmp( (const char *)pStr, "$RDFGP", 6) == 0)
		ParsingRDFGP(pStr, cts);
		
}


CGPSData::~CGPSData()
{

}

void CGPSData::operator=(CGPSData &newData)
{
	this->m_nYear = newData.m_nYear;
	this->m_nMonth = newData.m_nMonth;
	this->m_nDay = newData.m_nDay;
	this->m_nHour = newData.m_nHour;
	this->m_nMinute = newData.m_nMinute;
	this->m_nSecond = newData.m_nSecond;
	this->m_bNotDegree = newData.m_bNotDegree;
	this->m_bValid = newData.m_bValid;
	this->m_sp_state = newData.m_sp_state;
	this->m_fLat = newData.m_fLat;
	this->m_fLng = newData.m_fLng; // +북위/동경, -남위/서경
	this->m_fSpeed = newData.m_fSpeed;
	this->m_fAzimuth = newData.m_fAzimuth;
	this->m_fPdop = newData.m_fPdop;
	this->m_nCog = newData.m_nCog;
	this->m_cts = newData.m_cts;
}

//================================================================================================= E. CON/DCON
//================================================================================================= S. OPERATION

bool CGPSData::ParsingGPRMC(unsigned char *pStr, unsigned long cts)
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
			m_fSpeed = (float)atof(token);
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

bool CGPSData::ParsingRDFGP(unsigned char *pStr, unsigned long cts)
{
	char buf[1024];
	char seps[] = ",";
	char *token;

	m_cts = cts;

	//strcpy((char*)buf, (char*)pStr);
	strcpy_s((char*)buf, sizeof(buf),(char*)pStr);

	token = StrToken(buf, seps);

	for (int i = 0; i < CountOfRdfgpPos; i++)
	{
		if (token == NULL)
		{
			return false;
		}

		switch(i)
		{
		case rdfPosDate:
			SetDate(token);
			break;
		case rdfPosTime:
			SetTime(token);
			break;
		case rdfPosMode:
			m_sp_state = (unsigned char)atoi(token);
			break;
		case rdfPosKmhSpeed:
			m_fSpeed = (float)atof(token)  / 1.853184; //Km/h to Knots
			break;
		case rdfPosPdop:
			m_fPdop = (float)atof(token);
			break;
		case rdfPosEvent:
			break;
		case rdfPosCourse:
			m_nCog = atoi(token);
			break;
		case rdfPosLatV:
			m_fLat = (double)atof(token);
			break;
		case rdfPosLatNS:
			if (*token != 'N')
			{
				m_fLat *= -1;
			}
			break;
		case rdfPosLngV:
			m_fLng = (double)atof(token);
			break;
		case rdfPosLngEW:
			if (*token != 'E')
			{
				m_fLng *= -1;
			}
			break;
		default:;
		}

		token = StrToken(NULL, seps);
	}

	if(m_sp_state >= 2 && m_fLat != 0.0 && m_fLng != 0.0)
		m_bValid = TRUE;
	
	AdjustDateTime();

	return true;
}

char * CGPSData::StrToken(char * s1, const char * delimit)
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

void CGPSData::SetTime(char * pStr)
{
	char buf[3] = {0};

	strncpy_s(buf, pStr, 2);
	m_nHour = atoi(buf) + 9;
	strncpy_s(buf, pStr+2, 2);
	m_nMinute = atoi(buf);
	strncpy_s(buf, pStr+4, 2);
	m_nSecond = atoi(buf);
}

void CGPSData::SetDate(char * pStr)
{
	char buf[3] = {0};

	strncpy_s(buf, pStr, 2);
	m_nDay = atoi(buf);
	strncpy_s(buf, pStr+2, 2);
	m_nMonth = atoi(buf);
	strncpy_s(buf, pStr+4, 2);
	m_nYear = atoi(buf);
}

void CGPSData::AdjustDateTime(void)
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


double CGPSData::FromDegree(double dbValue)
{
	double d = (int)dbValue;
	double m = ((dbValue - d) * 100.0);

	return d + m / 60.0;
}


double CGPSData::GetLat(void)
{
	if(m_bNotDegree)
		return m_fLat;
	
	return FromDegree(m_fLat * 0.01);
}

double CGPSData::GetLng(void)
{
	if(m_bNotDegree)
		return m_fLng;
	
	return FromDegree(m_fLng * 0.01); 
}
//================================================================================================= E. OPERATION
//================================================================================================= S. CON/DCON

CPulseData::CPulseData()
{
	m_nYear = 0;
	m_nMonth = 0;
	m_nDay = 0;
	m_nHour = 0;
	m_nMinute = 0;
	m_nSecond = 0;
	m_bGpsConnectionState = 0; //물리적 연결 여부 (1: 연결 / 0: 연결되어 있지 않음)
	m_bGpsSignalState = 0; //GPS 신호 연결여부 (1: 수신 / 0: 수신불가 또는 수신 시도중)
	m_fGpsSpeed = 0.0; //km/h

	m_bPulseState = 0; //'1' - 펄스 당 속도 계산이 완료된 상태 (이 이후에는 Pulse Speed 내용을 참조하면 된다) '0' - 펄스 당 속도 계산이 아직 완료되지 않은 상태
	m_fPulseSpeed = 0.0; //km/h

	this->m_iPulseSec = 0;
	this->m_iSpdPulse = 0.0;
	
	this->m_bBrk = 0;
	this->m_bSR = 0;
	this->m_bSL = 0;
	this->m_bBgr = 0;
	this->m_bTR = 0;
	m_cts = 0;
}


CPulseData::CPulseData(unsigned char * pStr, unsigned long cts)
{
	ParsingPulseData(pStr, cts);
}


CPulseData::~CPulseData()
{

}

void CPulseData::operator=(CPulseData &newData)
{
	this->m_nYear = newData.m_nYear;
	this->m_nMonth = newData.m_nMonth;
	this->m_nDay = newData.m_nDay;
	this->m_nHour = newData.m_nHour;
	this->m_nMinute = newData.m_nMinute;
	this->m_nSecond = newData.m_nSecond;
	this->m_bGpsConnectionState = newData.m_bGpsConnectionState;
	this->m_bGpsSignalState = newData.m_bGpsSignalState;
	this->m_fGpsSpeed = newData.m_fGpsSpeed;
	this->m_bPulseState = newData.m_bPulseState;
	this->m_fPulseSpeed = newData.m_fPulseSpeed;

	this->m_iPulseSec =  newData.m_iPulseSec;
	this->m_iSpdPulse = newData.m_iSpdPulse;
	this->m_bBrk = newData.m_bBrk;
	this->m_bSR = newData.m_bSR;
	this->m_bSL = newData.m_bSL;
	this->m_bBgr = newData.m_bBgr;
	this->m_bTR = newData.m_bTR;

	this->m_bRpmConnectionState = newData.m_bRpmConnectionState;
	this->m_iRpmPulseSec = newData.m_iRpmPulseSec;
	this->m_iRpm = newData.m_iRpm;
	
	this->m_cts = newData.m_cts;
}

bool CPulseData::ParsingPulseData(unsigned char *pStr, unsigned long cts)
{
	char buf[1024];
	char seps[] = ",";
	char *token;

	m_cts = cts;

	//strcpy((char*)buf, (char*)pStr);
	strcpy_s((char*)buf, sizeof(buf),(char*)pStr);

	token = StrToken(buf, seps);

	for (int i = 0; i < 2; i++)
	{
		if (token == NULL)
		{
			return false;
		}

		switch(i)
		{
		case 0:
			this->m_bGpsConnectionState = (BOOL)StrToInt(token + 2, 1);
			this->m_bGpsSignalState = (BOOL)StrToInt(token + 3, 1);
			this->m_fGpsSpeed = (double)StrToInt(token + 4, 3);
			this->m_bPulseState = (BOOL)StrToInt(token + 7, 1);
			this->m_fPulseSpeed = (double)StrToInt(token + 8, 3);
			this->m_iPulseSec = StrToInt(token + 11, 3);
			this->m_iSpdPulse = StrToInt(token + 14, 3) / 100.0;
			
			this->m_bBrk = (BOOL)StrToInt(token + 17, 1);
			this->m_bSR = (BOOL)StrToInt(token + 18, 1);
			this->m_bSL = (BOOL)StrToInt(token + 19, 1);
			this->m_bBgr = (BOOL)StrToInt(token + 20, 1);
			this->m_bTR = (BOOL)StrToInt(token + 21, 1);

			this->m_bRpmConnectionState = (BOOL)StrToInt(token + 22, 1);
			this->m_iRpmPulseSec = StrToInt(token + 23, 4);
			this->m_iRpm = StrToInt(token + 27, 5);
			break;
		case 1:
			SetTime(token);
			break;
		default:;
		}

		token = StrToken(NULL, seps);
	}

	return true;
}

char * CPulseData::StrToken(char * s1, const char * delimit)
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

int CPulseData::StrToInt(char * s1, int delimit)
{
	char buf[1024];

	if(delimit < sizeof(buf)){
		strncpy(buf, s1, delimit);
		buf[delimit] = 0;

		return atoi(buf);
	}

	return NULL;
}

void CPulseData::SetTime(char * pStr)
{
	char buf[3] = {0};

	strncpy_s(buf, pStr, 2);
	m_nDay = atoi(buf);
	strncpy_s(buf, pStr+2, 2);
	m_nMonth = atoi(buf);
	strncpy_s(buf, pStr+4, 2);
	m_nYear = atoi(buf);

	strncpy_s(buf, pStr+6, 2);
	m_nHour = atoi(buf);
	strncpy_s(buf, pStr+8, 2);
	m_nMinute = atoi(buf);
	strncpy_s(buf, pStr+10, 2);
	m_nSecond = atoi(buf);
}


//================================================================================================= E. CON/DCON

//================================================================================================= S. CON/DCON

CTextData::CTextData()
{
	m_bPulseSpeedUse = FALSE;  //20170829
	m_bPulseSpeedEnable = TRUE;
	m_bI3FileType = FALSE;
}

CTextData::~CTextData()
{
}

//================================================================================================= E. CON/DCON
//================================================================================================= S. OPERATION

int CTextData::ParseTextData(unsigned char * pTextData, unsigned long cts, CWBCSensorData::EGSENSORTYPE type)
{
	static float speed;
	
	if( m_GSensorList.size() <= 0 )
		speed = 0;
	
	if (pTextData[0] == 'M')	// G-Sensor Data
	{
#if 1//성진전자	
		gsensor_acc st_acc;
		switch( type )
	    {
#if 0	    
	    case CWBCSensorData::GSENSOR_OLD_8_BIT:
	    case CWBCSensorData::GSENSOR_8_BIT:
	        {
	            st_acc.x = ((0x00ff & ~pTextData[1]) + 1 - 128) ;
	            st_acc.y = ((0x00ff & ~pTextData[2]) + 1 - 128) ;
	            st_acc.z = ((0x00ff & ~pTextData[3]) + 1 - 128) ;

	        } break;

	    case CWBCSensorData::GSENSOR_10_BIT:
	        {
				  st_acc = *((gsensor_acc *)&pTextData[1]);
						
	            st_acc.x = (0x03ff & ~st_acc.x) + 1 - 512;
	            st_acc.y = (0x03ff & ~st_acc.y) + 1 - 512;
	            st_acc.z = (0x03ff & ~st_acc.z) + 1 - 512;

	        } break;
#else
		 case CWBCSensorData::GSENSOR_OLD_8_BIT:
	    case CWBCSensorData::GSENSOR_8_BIT:
	        {
				  signed char * pValue = (signed char *)pTextData;
					
	            st_acc.x = (-pValue[1]) * 4;
	            st_acc.y = (-pValue[2]) * 4;
	            st_acc.z = (-pValue[3]) * 4;
	        } break;

	    case CWBCSensorData::GSENSOR_10_BIT:
	        {
				  st_acc = *((gsensor_acc *)&pTextData[1]);
						
	            st_acc.x = (0x03ff & ~st_acc.x) + 1 - 512;
	            st_acc.y = (0x03ff & ~st_acc.y) + 1 - 512;
	            st_acc.z = (0x03ff & ~st_acc.z) + 1 - 512;
	        } break;

#endif
		case CWBCSensorData::GSENSOR_AVIC_8_BIT:
			{
				st_acc.x = ((0x00ff & ~pTextData[1]) + 1 - 128) * 4;
	           st_acc.y = ((0x00ff & ~pTextData[2]) + 1 - 128) * 4;
	           st_acc.z = ((0x00ff & ~pTextData[3]) + 1 - 128) * 4;
			}
			break;
	    }
		
		CGSensorData data(&st_acc, &speed, cts, &pTextData[7]);
#else		
		CGSensorData data((gsensor_acc*)&pTextData[1], (float*)&pTextData[7], cts);
#endif
		m_GSensorList.push_back(data);
	}
	else if (pTextData[0] == 'G')
	{
		CGPSData data(&pTextData[1], cts);
		
		if(data.m_bValid){
			speed = (float)data.m_fSpeed;
			m_GPSList.push_back(data);
		}
	}
	else if (pTextData[0] == 'P')
	{
		CPulseData data(pTextData, cts);
		
		m_PulseList.push_back(data);
	}
	return 0;
}

//================================================================================================= S. OPERATION
