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
#include <CLWBC/wbcTextData.h>

//================================================================================================= E. INCLUDE
//================================================================================================= S. CLASS

typedef struct  {
		short x, /**< holds x-axis acceleration data sign extended. Range -512 to 511. */
			  y, /**< holds y-axis acceleration data sign extended. Range -512 to 511. */
			  z; /**< holds z-axis acceleration data sign extended. Range -512 to 511. */
} gsensor_acc;

class CGSensorData
{
public:
	CGSensorData();
	CGSensorData(gsensor_acc *pAccel, float *pSpeed, unsigned long cts, unsigned char * time);
	virtual ~CGSensorData();

	float				m_gseonsorX;
	float				m_gseonsorY;
	float				m_gseonsorZ;

	float				m_speed; //knots
	unsigned long	m_cts;
	unsigned char 		m_time[6]; //년월일시분초


	void operator=(CGSensorData &newData);
};

class CGPSData
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

	enum RDFGPPOS 
	{
		rdfPosHead,
		rdfPosDate,
		rdfPosTime,
		rdfPosMode,
		rdfPosKmhSpeed,
		rdfPosPdop,
		rdfPosEvent,
		rdfPosCourse,
		rdfPosLatV,
		rdfPosLatNS,
		rdfPosLngV,
		rdfPosLngEW,
		
		CountOfRdfgpPos
	};

public:
	CGPSData();
	CGPSData(unsigned char * pStr, unsigned long cts);
	virtual ~CGPSData();

	int			m_nYear, m_nMonth, m_nDay, m_nHour, m_nMinute, m_nSecond;
	BOOL   		m_bNotDegree;
	BOOL		m_bValid;

	unsigned char m_sp_state;	//위성 상태 (0 = 2 = 2D, 3 = 3D) 
	double		m_fLat, m_fLng; // +북위/동경, -남위/서경
	double		m_fSpeed; //Knots
	double		m_fAzimuth;
	double		m_fPdop;	//PDOP
	int			m_nCog;		//방위 (0 ~ 360 × 100)  Course Over Ground (degree) //진북
	
	//CTime		m_time;
	unsigned long	m_cts;

	void operator=(CGPSData &newData);
	bool ParsingGPRMC(unsigned char *pStr, unsigned long cts);
	bool ParsingRDFGP(unsigned char *pStr, unsigned long cts);
	char * StrToken(char * s1, const char * delimit);
	void SetTime(char * pStr);
	void SetDate(char * pStr);
	void AdjustDateTime(void);
	double FromDegree(double dbValue);
	double GetLat(void);
	double GetLng(void);
};


class CPulseData
{
public:
	CPulseData();
	CPulseData(unsigned char * pStr, unsigned long cts);
	virtual ~CPulseData();

	BOOL m_bGpsConnectionState; //물리적 연결 여부 (1: 연결 / 0: 연결되어 있지 않음)
	BOOL m_bGpsSignalState; //GPS 신호 연결여부 (1: 수신 / 0: 수신불가 또는 수신 시도중)
	double m_fGpsSpeed; //km/h

	BOOL m_bPulseState; //'1' - 펄스 당 속도 계산이 완료된 상태 (이 이후에는 Pulse Speed 내용을 참조하면 된다) '0' - 펄스 당 속도 계산이 아직 완료되지 않은 상태
	double m_fPulseSpeed; //km/h

	int m_iPulseSec;  //현재, 초당 펄스 수
	double m_iSpdPulse; //Pulse Set이 완료되었을 때 계산된 펄스 당 속도

	BOOL m_bBrk; //브레이크 여부 (한 번이라도 밟았으면 '1', 안 밟았으면 '0')
	BOOL m_bSR;  //오른쪽 방향지시등 점멸 여부 (켰으면 '1', 안 켰으면 '0')
	BOOL m_bSL; //왼쪽 방향지시등 점멸 여부 (켰으면 '1', 안 켰으면 '0')
	BOOL m_bBgr; //후진기어 여부 (후진 켰으면'1', 안 켰으면 '0')
	BOOL m_bTR; //트리거 IN 신호가 감지되면 '1' (아니면 '0')	

	int 	 	m_iRpmPulseSec;	//현재, 초당 펄스 수
	int 	 	m_iRpm;					//RPM
	BOOL 	m_bRpmConnectionState; //물리적 연결 여부 (1: 연결 / 0: 연결되어 있지 않음)
	
	int			m_nYear, m_nMonth, m_nDay, m_nHour, m_nMinute, m_nSecond;
	
	unsigned long	m_cts;


	bool ParsingPulseData(unsigned char *pStr, unsigned long cts);
	char * StrToken(char * s1, const char * delimit);
	int StrToInt(char * s1, int delimit);
	void SetTime(char * pStr);
	void operator=(CPulseData &newData);
};

class CTextData
{
public:
	CTextData();
	virtual ~CTextData();

	//====================================================================================== S. TYPEDEF

	typedef std::list<CGSensorData>								GSENSOR_POOL;
	typedef GSENSOR_POOL::iterator								ITER_GSENSOR;

	typedef std::list<CGPSData>									GPS_POOL;
	typedef GPS_POOL::iterator									ITER_GPS;

	typedef std::list<CPulseData>									PULSE_POOL;
	typedef PULSE_POOL::iterator									ITER_PULSE;
	//====================================================================================== E. TYPEDEF

	GSENSOR_POOL		m_GSensorList;
	GPS_POOL			m_GPSList;

	PULSE_POOL			m_PulseList;

	BOOL				m_bPulseSpeedUse;
	BOOL				m_bPulseSpeedEnable;

	BOOL				m_bI3FileType;


	int ParseTextData(unsigned char * pTextData, unsigned long cts, CWBCSensorData::EGSENSORTYPE type = CWBCSensorData::GSENSOR_OLD_8_BIT);
};

//================================================================================================= E. CLASS
