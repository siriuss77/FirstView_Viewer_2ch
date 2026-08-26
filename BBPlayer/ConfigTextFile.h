// ConfigTextFile.h: interface for the CConfigText class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIG_TEXT_FILE_H__INCLUDED_)
#define AFX_CONFIG_TEXT_FILE_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "windows.h"

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                Definition

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#define CONFIG_USE_IR_LED		0

#define CONFIG_MAX_CAMERA		2
#define CONFIG_MAX_RESOLUTION	1280 // QVGA = 320, VGA = 6400, D1 = 720, 720P = 1280, 1080P = 1920  // jun 20200908
//----------------------------------------------------------------------

#define RES_VGA_MODE_USE	0 //lucy : error

#ifndef ON
 #define 	ON		1
 #endif

 #ifndef OFF
  #define OFF 	0
 #endif
 
enum
{
#if RES_VGA_MODE_USE
	RES_QVGA = 0,
	RES_VGA,
#endif	
	RES_D1,
	RES_HD,
	RES_USER
};

//System Info
typedef struct{
	int			nPcUpdate;
    	char			cFwVersion[64];
	char			cViewerVersion[64];
	int			nMaxChannel;
	int			nMaxResolution;	
}ST_CFG_SYSINFO, *LPST_CFG_SYSINFO;
		
//----------------------------------------------------------------------
//Display
typedef enum{
 	eLANG_ENGLISH,	
	eLANG_KOREAN,

	eLANG_ENUM_END
}ENUM_LANGUAGE;

typedef enum{
	UNIT_KM_P_H,
	UNIT_MILE_P_H,
	UNIT_KNOT,
	
	UNIT_ENUM_END
}ENUM_UNIT_OF_SPEED;

typedef enum{
	DISPMODE_OFF,
		
	DISPMODE_CH1,	VIDEOOUT_FRONT 	= DISPMODE_CH1,
	DISPMODE_CH2,	VIDEOOUT_REAR	= DISPMODE_CH2,
#if 0	
	DISPMODE_CH3, 	VIDEOOUT_LEFT	= VIDEOOUT_CH3,
	DISPMODE_CH4, 	VIDEOOUT_RIGHT	= VIDEOOUT_CH4,
	DISPMODE_AUTO,
#endif	
	DISPMODE_DUAL1,
	DISPMODE_DUAL2,

#if 0	
	DISPMODE_PIP1,
	DISPMODE_PIP2,
#endif

	DISPMODE_ENUM_END
}ENUM_DISPLAYMODE;

typedef enum
{
	VIDEOTYPE_NTSC,
	VIDEOTYPE_PAL,
	VIDEOTYPE_PAL_M,
	VIDEOTYPE_PAL_COMB_N,
	VIDEOTYPE_NTSC_NOSCALEUP,
	
	VIDEOTYPE_ENUM_END
} ENUM_VIDEOTYPE;

typedef struct{
    ENUM_LANGUAGE     	eLanguage;
    INT			      	nTimeZone;
    ENUM_UNIT_OF_SPEED  eUnitOfSpeed;
    ENUM_DISPLAYMODE 	eDispMode;
	ENUM_VIDEOTYPE 		eVideoType;
}ST_CFG_DISPLAY, *LPST_CFG_DISPLAY;

//----------------------------------------------------------------------
//Recorder

typedef enum{
	RECORD_DRIVING_2CH_DISABLE,
	RECORD_DRIVING_2CH_ENABLE,

	RECORD_DRIVING_2CH_ENUM_END
}ENUM_RECORD_DRIVING_2CH;	//Driving change to two-channel

typedef enum{
	RECORD_1CH = 1,				// 1CH FRONT
#if (CONFIG_MAX_CAMERA >= 2)		
	RECORD_2CH,
#if (CONFIG_MAX_CAMERA >= 3)	
	RECORD_3CH,
#if (CONFIG_MAX_CAMERA >= 4)
	RECORD_4CH,
#endif	
#endif	
#endif

	RECORD_1CH_R,				// 1CH REAR
	RECORD_AUTO,

	RECORD_CHANNEL_ENUM_END
}ENUM_RECORD_CHANNEL;

typedef enum{
	RECORD_CONTINUOUS,
	RECORD_EVENT,			DVR_PARKING_REC_MODE = RECORD_EVENT,
	
	RECCORD_MODE_ENUM_END
}ENUM_RECORD_MODE;

typedef enum{
	VIDEO_CODEC_TYPE_MPEG4,
	VIDEO_CODEC_TYPE_H264,
	VIDEO_CODEC_TYPE_MJPEG,
	
    VIDEO_CODEC_ENUM_END
}ENUM_VIDEO_CODEC;

typedef enum{
#if RES_VGA_MODE_USE	
	VIDEO_RESOL_QVGA,		//QVGA 320x240
 #if	CONFIG_MAX_RESOLUTION >= 640
    VIDEO_RESOL_VGA,		//VGA 640x480
 #endif
#endif

#if CONFIG_MAX_RESOLUTION >= 720    
    VIDEO_RESOL_D1,			//D1 720x480
#if CONFIG_MAX_RESOLUTION >= 1280
    VIDEO_RESOL_720P,		//HD 1280x720
#if CONFIG_MAX_RESOLUTION >= 1920
	VIDEO_RESOL_1080P,		//FULL HD 1920x1080
#endif	
#endif	
#endif

	VIDEO_RESOL_AUTO,
	
    VIDEO_RESOL_ENUM_END
}ENUM_VIDEO_RESOLUTION;

#if BUILD_MODEL == MODEL_HONDA_HD_2U || BUILD_MODEL == MODEL_RALLY_EYES
	#define VIDEO_BITRATE_HD_MIN			4.0f
	#define VIDEO_BITRATE_HD_INTERVAL		2.0f
	
	#define VIDEO_BITRATE_D1_MIN			2.0f
	#define VIDEO_BITRATE_D1_INTERVAL		1.0f
#else
	#define VIDEO_BITRATE_HD_MIN			3.0f
 #if (BUILD_MODEL == MODEL_KIC_M7 || BUILD_MODEL == MODEL_ICARO_I300HD || BUILD_MODEL == MODEL_DOOCO_M7 || BUILD_MODEL == MODEL_EYE_TRACKS)
	#define VIDEO_BITRATE_HD_INTERVAL		0.75f
 #else
	#define VIDEO_BITRATE_HD_INTERVAL		1.25f
 #endif
	#define VIDEO_BITRATE_D1_MIN			1.0f
	#define VIDEO_BITRATE_D1_INTERVAL		0.5f
#endif


typedef enum{
	VIDEO_QUALITY_LOW = 0,
	VIDEO_QUALITY_NORMAL,
	VIDEO_QUALITY_HIGH,

	VIDEO_QUALITY_ENUM_END
}ENUM_VIDEO_QUALITY;

typedef enum{
	AUDIO_REC_OFF,
	AUDIO_REC_ON,

	AUDIO_REC_ENUM_END
}ENUM_AUDIO_REC;

typedef enum{
    REC_TIME1 = 1,
    REC_TIME2,
    REC_TIME3,
    REC_TIME5,
    REC_TIME10,
    REC_TIME_ENUM_END
}ENUM_REC_TIME;

typedef enum{
	EVENT_PROPORTION_1P2,		// 1/2
	EVENT_PROPORTION_1P3,		// 1/3
	EVENT_PROPORTION_1P4,		// 1/4
	EVENT_PROPORTION_1P5,		// 1/5
	
	EVENT_PROPORTION_ENUM_END
}ENUM_EVENT_PROPORTION;

typedef struct{
    ENUM_RECORD_DRIVING_2CH eDriving2Ch;
    ENUM_RECORD_CHANNEL    	eChannel;
    ENUM_RECORD_MODE	    eMode;
    ENUM_VIDEO_CODEC  		eCodecType;
    ENUM_VIDEO_RESOLUTION	eaResolution[CONFIG_MAX_CAMERA];
	ENUM_VIDEO_QUALITY		eaQuality[CONFIG_MAX_CAMERA];
	int						iEventType;
	BOOL					bAudioRecord;
	ENUM_REC_TIME			eRecordTime;
	ENUM_EVENT_PROPORTION	eEventProportion;
}ST_CFG_RECORD, *LPST_CFG_RECORD;

//----------------------------------------------------------------------
//Device
typedef enum{
	SENSITIVE_HIGH,
    SENSITIVE_NORMAL,
	SENSITIVE_LOW,
	
    SENSITIVE_ENUM_END
}ENUM_SENSITIVE;

typedef enum {
	MD_BLOCK_LOWER1P2,
	MD_BLOCK_LOWER2P3,
	MD_BLOCK_LOWER3P4,	
	MD_BLOCK_FULL,
	
	MD_BLOCK_END
} eMD_BLOCK_PROPORTION;

typedef struct{
	//Motion detect
    BOOL               		bMode; 
    ENUM_SENSITIVE   		eSensitive; 
	eMD_BLOCK_PROPORTION	eBlockProportion;
}ST_MOTION_SET, *LPST_MOTION_SET;

typedef enum{
	VERITCAL_FLIP_OFF,
    VERITCAL_FLIP_ON,
//	VERITCAL_FLIP_AUTO,
	
    VERITCAL_FLIP_ENUM_END
}ENUM_VERITCAL_FLIP;

typedef struct{
    ENUM_VERITCAL_FLIP	eVerticalFlip; 
    BOOL               	bHorizontalMirror;
    UINT               	ulBrightness;
}ST_CAMERA_SET, *LPST_CAMERA_SET;

typedef enum{
	USB_MODE1,		//무조건 USB 우선 저장
	USB_MODE2,  	// USB 상시 SD 이벤트 저장
	USB_MODE3, 		// USB와 SD 중 여유 공간이 많은 메모리에 저장

	USB_MODE_ENUM_END
}ENUM_USB_MODE;

//Ext GPIO On/Off control
typedef enum{ 
	RECORD_ALARM_OFF,				// Off (Low) : do not use
	RECORD_ALARM_RECORD,			// 녹화시 (High)
	RECORD_ALARM_PARKING_RECORD, 	// 주차중 녹화시 High
	RECORD_ALARM_PARKING_SHOCK,		DEFAULT_RECORD_ALARM = RECORD_ALARM_PARKING_SHOCK, // 주차중 충격 녹화만 High

	RECORD_ALARM_ENUM_END
}ENUM_RECORD_ALARM_MODE;

typedef enum{
	PARKING_RECORD_ALARM_OFF,
	PARKING_RECORD_ALARM_SHOCK,		//충격 이벤트
	PARKING_RECORD_ALARM_EVENT,		// 모션 + 충격 이벤트
	
	PARKING_RECORD_ALARM_ENUM_END
}ENUM_PARKING_RECORD_ALARM;

typedef struct{
	//G-sensor
    double   				dShockSensitive; 
	
	//Motion detect
   	ST_MOTION_SET			staMotionSet[CONFIG_MAX_CAMERA];

	// Camera
	ST_CAMERA_SET			staCameraSet[CONFIG_MAX_CAMERA];

	// Usb memory
	ENUM_USB_MODE			eUsbMode;

	// EXT_MODULE_GPO On/Off (Record Alarm)
	ENUM_RECORD_ALARM_MODE	eRecordAlarm;

	// Speaker Volume
	UINT					ulSpeakerVolume;	// 0 ~ 4
	// Parking Record Alarm
	ENUM_PARKING_RECORD_ALARM	eParkingRecordAlarm;
}ST_CFG_DEVICE, *LPST_CFG_DEVICE;

//----------------------------------------------------------------------
//System
typedef enum{
	BUZZER_TYPE_SYSTEM,
	BUZZER_TYPE_BUTTON,
	BUZZER_TYPE_EVENT,

	BUZZER_TYPE_MAX
}ENUM_BUZZER_TYPE;

typedef enum{
	BUZZER_DISABLE,
	BUZZER_SYSTEM,
	BUZZER_SYSTEM_KEY,
	BUZZER_SYSTEM_EVENT,
	BUZZER_ALL_ENABLE,		
	
    BUZZER_MODE_ENUM_END
}ENUM_BUZZER_MODE;

#if  (BUILD_MODEL == MODEL_BW_2000 || BUILD_MODEL == MODEL_KIC_M7 || BUILD_MODEL == MODEL_ICARO_I300HD || BUILD_MODEL == MODEL_DOOCO_M7 || BUILD_MODEL == MODEL_EYE_TRACKS)
#define DEFAULT_BUZZER_MODE  BUZZER_ALL_ENABLE
#else
#define DEFAULT_BUZZER_MODE  BUZZER_SYSTEM_KEY
#endif

typedef enum{
	PARKING_OFF,
//	PARKING_ON,
    PARKING_AUTO,
    
    PARKING_ENUM_END
}ENUM_PARKING_MODE;

#define DEFAULT_PARKING_TIME		3 // Min
typedef enum{
	PARKING_USB_OFF,
	PARKING_USB_ON,			DEFAULT_PARKING_USB = PARKING_USB_ON,
    PARKING_USB_AUTO,
    
    PARKING_USB_ENUM_END
}ENUM_PARKING_USB;

#define DEFAULT_PARKING_USB_OFF_VOLT		0.3 // volt
typedef enum{
	PARKING_LOW_POWER_OFF,		// 30 FPS
	PARKING_LOW_POWER_ON,		// 15 FPS
    	PARKING_LOW_POWER_AUTO,		DEFAULT_PARKING_LOW_POWER = PARKING_LOW_POWER_AUTO,	// Event Ready 15 FPS, Event 30 FPS
    
    	PARKING_LOW_POWER_ENUM_END
}ENUM_PARKING_LOW_POWER;

typedef enum{
	LOW_VOLTAGE_MODE_DUAL, 	DEFAULT_LOW_VOLTAGE_MODE = LOW_VOLTAGE_MODE_DUAL, // Sensing the voltage and power
	LOW_VOLTAGE_MODE_VOLTAGE, 		// Sensing the voltage
	
	LOW_VOLTAGE_MODE_END
}ENUM_LOW_VOLTAGE_MODE;

typedef enum{
	IRLED_OFF,
//	IRLED_ON,
    IRLED_AUTO,
    
    IRLED_ENUM_END
}ENUM_IRLED_MODE;

#if  (BUILD_MODEL == MODEL_BW_2000)
#define LOW_VOLTAGE_MAX_STEP			6
#define LOW_VOLTAGE_MIN_LEVEL			11.8
#else
  #ifdef LOW_VOLTAGE_9_0_USE
  	#define LOW_VOLTAGE_MAX_STEP				16
  	#define LOW_VOLTAGE_MIN_LEVEL				9.0		// 9.0, 9.5, 10.0, 10.5, 11.0, 11.5, 11.6 ~ 12.6
	#define LOW_VOLTAGE_HIGH_LEVEL			12.6
	#define LOW_VOLTAGE_0_1_INTERVAL_START	11.6
	#define LOW_VOLTAGE_INTERVAL				0.5
  #else
	#define LOW_VOLTAGE_MAX_STEP			10
	#define LOW_VOLTAGE_MIN_LEVEL			11.6
 #endif
#endif

#if (BUILD_MODEL == MODEL_KIC_M7 || BUILD_MODEL == MODEL_ICARO_I300HD || BUILD_MODEL == MODEL_DOOCO_M7 || BUILD_MODEL == MODEL_EYE_TRACKS)
#define DEFAULT_LOW_VOLTAGE_DETECTED	12.5
#else
#define DEFAULT_LOW_VOLTAGE_DETECTED	12.2
#endif

typedef struct{
    ENUM_BUZZER_MODE    			eBuzzerMode;
	ENUM_LOW_VOLTAGE_MODE		eLowVoltageMode;
   	float							fLowVoltage;
	ENUM_PARKING_MODE			eParkingMode;
	UINT						ulParkingTime;	//parking enter time
	ENUM_PARKING_USB 			eParkingUsb;
	float							fParkingUsbOffVolt;
	ENUM_PARKING_LOW_POWER	eParkingLowPower;
    ENUM_IRLED_MODE     			eIrLedMode;
}ST_CFG_SYSTEM, *LPST_CFG_SYSTEM;

//----------------------------------------------------------------------
//Time
typedef struct{
    int    	nYear;
    int    	nMonth;
    int		nDate;
    int     nHour;
    int     nMinute;
    int     nSec;
	int		nTimeSet;
}ST_CFG_TIME, *LPST_CFG_TIME;

//-----------------------------------------------------------------------
typedef struct{
	ST_CFG_SYSINFO		stSysInfo;
   	ST_CFG_DISPLAY    	stDisplay;
   	ST_CFG_RECORD		stRecord;
	ST_CFG_DEVICE		stDevice;
    	ST_CFG_SYSTEM     	stSystem;
	ST_CFG_TIME			stTime;
}ST_CFG_ITEM, *LPST_CFG_ITEM;

typedef struct{
	char		strName[128];
	char		strFWVersion[64];
	char       strCarNumber[64]; // CAR NUMBER   // jun 200805
	TCHAR       strDriverCode[64]; // DRIVER NAME   // jun 221017
	int		iGsensorSensi;
	int		iGmt;
	int		iAudioRecEnable;
	int		iSpeakerVol;
	int      	iEngineCylinders;  // jun 20201202
	int		iVideoQuality; 	// LOW, MIDDLE, HIGH 
	int		iEventMode;		// A, B, C
	bool		bAutoPmEnable;
	int		iAutoPmEnterTime;
	int		iPmImpactSensi;
	bool		bPmMotionEnable;
	int		iPmMotionSensi;
	int		iPmMotionSensi2;
	int		iCarBatVoltage;	//12V, 24V
	int		iCarBatVoltageCalib;
	bool		bCarBatSafeEnable;
	int		iCarBatSafeVoltage;
	bool		bTempSafeEnable;
	int		iTempSafeValue;
	int		iTempStableValue;
	bool		bSecurityLEDEnable;
	int		iSecurityLEDPeriod;
	bool		bSecurityLEDMDWarning;
	int		iSecurityLEDOperating;
	int		iLcdOffTime;
	int		iVideoOut;	//TV, LCD
	bool		bFactoryReset;
	int		iPulseReset;
	int		iPulseTacho1LowHigh;  //BRAKE
	int		iPulseTacho4LowHigh;  // INPUT1
	int		iPulseTacho5LowHigh; // INPUT2
	int		iOsdSpeed; // OSDSPEED  jun 200813

	char		strLastSetupTime[32]; //%4d-%02d-%02d %02d:%02d:%02d
   }ST_CFG_DAVIEW, *LPST_CFG_DAVIEW;

//------------------------------------------------------------------------

//#define BLACKBOX_CONFIG_FOLDER		_T("Config\\")
//#define BLACKBOX_CONFIG_FILE_NAME		_T("Config.ini")
#define BLACKBOX_CONFIG_FOLDER		_T("")
#define BLACKBOX_CONFIG_FILE_NAME		_T("setup.xml")
#define BLACKBOX_TIME_FILE_NAME		_T("Time.cfg")
#define BLACKBOX_FORMAT_FILE_NAME		_T("format.txt")
#define BLACKBOX_VRHD_FORMAT_FILE_NAME		_T("VideoQualit.txt")

#define BLACKBOX_CONFIG_FILE		(BLACKBOX_CONFIG_FOLDER BLACKBOX_CONFIG_FILE_NAME)

class CConfigText  
{
public:

	CConfigText(void);
	virtual ~CConfigText(void);

	static BOOL Load(LPST_CFG_ITEM blackbox_config);
	static BOOL Load(LPST_CFG_DAVIEW blackbox_config);
	
	static BOOL Load(CString cfg_file_name, LPST_CFG_ITEM blackbox_config);
	static BOOL Load(CString cfg_file_name, LPST_CFG_DAVIEW blackbox_config);
	
	static CString Save(LPST_CFG_ITEM spConfig);
	static CString Save(LPST_CFG_DAVIEW spConfig);
	static CString Save(CString cfg_file_dir, LPST_CFG_ITEM spConfig);
	static CString Save(CString cfg_file_dir, LPST_CFG_DAVIEW spConfig);
	static CString Save(CString cfg_file_dir, LPST_CFG_TIME spTime);

	static CString SaveVersion(CString cfg_file_dir, char * strVersion);
		
	static BOOL CfgDefaultSet(LPST_CFG_DAVIEW pCfg);
	static BOOL CfgDefaultSet(LPST_CFG_ITEM pCfg);
	static CString make_time_string(time_t t);
protected:
	static BOOL CfgPrintfToFile(CFile *fhandle, char *fmt, ... );
	static BOOL CfgParserFile(CString cfg_file_name, LPST_CFG_ITEM blackbox_config);
	static BOOL CfgParserFile(CString cfg_file_name, LPST_CFG_DAVIEW blackbox_config);
	
};

#endif // !defined(AFX_CONFIG_TEXT_FILE_H__INCLUDED_)
