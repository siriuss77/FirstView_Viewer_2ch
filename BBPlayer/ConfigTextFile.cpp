// ConfigTextFile.cpp: implementation of the CTParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <TINYXML/tinyxml.h>
#include <CLUDE/version.h>
#include <HELPER/CLUtil.H>
#include "ConfigTextFile.h"
#include "TParser.h"
#include "BBoxUtil.h"


#define G_SENSOR_DEFAULT_TRIGGERLEVEL 		0.3f

#define ZONE_CFG_PRINT	1

CConfigText::CConfigText(void)
{
}

CConfigText::~CConfigText(void)
{
}

BOOL CConfigText::Load(LPST_CFG_ITEM blackbox_config)
{
	return Load(CString(BLACKBOX_CONFIG_FILE), blackbox_config);
}

BOOL CConfigText::Load(LPST_CFG_DAVIEW blackbox_config)
{
	return Load(CString(BLACKBOX_CONFIG_FILE), blackbox_config);
}

BOOL CConfigText::Load(CString cfg_file_name, LPST_CFG_ITEM blackbox_config)
{
	BOOL ret;
	CString fileName;

	if(cfg_file_name.Find(TEXT(":\\")) < 0)
	{
		fileName = CBBoxUtil::GetModuleDir();
		fileName.Append(TEXT("\\"));
	}
	
	fileName.Append(cfg_file_name);
	return CfgParserFile(fileName, blackbox_config);
}

BOOL CConfigText::Load(CString cfg_file_name, LPST_CFG_DAVIEW blackbox_config)
{
	BOOL ret;
	CString fileName;

	if(cfg_file_name.Find(TEXT(":\\")) < 0)
	{
		fileName = CBBoxUtil::GetModuleDir();
		fileName.Append(TEXT("\\"));
	}
	
	fileName.Append(cfg_file_name);
	return CfgParserFile(fileName, blackbox_config);
}

CString CConfigText::Save(LPST_CFG_ITEM spConfig)
{
	return Save(CBBoxUtil::GetModuleDir(), spConfig);
}

CString CConfigText::Save(LPST_CFG_DAVIEW spConfig)
{
	return Save(CBBoxUtil::GetModuleDir(), spConfig);
}

CString CConfigText::Save(CString cfg_file_dir, LPST_CFG_TIME spTime)
{
	CString fileName;
	CFile file;
	
	fileName = cfg_file_dir;
	fileName.Append(_T("\\"));
	fileName.Append(BLACKBOX_TIME_FILE_NAME);

	if(file.Open(fileName.GetBuffer(), CFile::modeCreate | CFile::modeReadWrite))
	{
		CfgPrintfToFile(&file, "%04d %02d %02d %02d %02d %02d", spTime->nYear, spTime->nMonth, spTime->nDate, spTime->nHour, spTime->nMinute, spTime->nSec);
		file.Close();
	}
	
	return fileName;
}

CString CConfigText::Save(CString cfg_file_dir, LPST_CFG_ITEM spConfig)
{
	CFile file;
	int ch,i;
	int ret;

	CFile *fhandle = NULL;
	CFileFind finder;
	CString fileName;

	fileName = cfg_file_dir;
	if(fileName.Find(_T("Config")) >= 0)
	{
		fileName.Append(_T("\\"));
		fileName.Append(BLACKBOX_CONFIG_FILE_NAME);
	}
	else
	{
		fileName.Append(_T("\\"));
		fileName.Append(BLACKBOX_CONFIG_FILE);
	}
	
	if(!finder.FindFile(fileName.GetBuffer()))
		CreateDirectory(CBBoxUtil::GetDir(fileName), NULL);
	//else
		//DeleteFile(fileName.GetBuffer());

	finder.Close();
	
	if(file.Open(fileName.GetBuffer(), CFile::modeCreate | CFile::modeReadWrite))
	{
		fhandle = &file;

		// 0. System Info
		CfgPrintfToFile(fhandle, "\r\n[System_Info]\r\n");
		CfgPrintfToFile(fhandle, "PC_Update          = %d\r\n", 1);
		CfgPrintfToFile(fhandle, "FW_Version         = %s\r\n", spConfig->stSysInfo.cFwVersion);
		CfgPrintfToFile(fhandle, "Viewer_Version     = %s\r\n", VS_SW_VERSION);//spConfig->stSysInfo.cViewerVersion);
		CfgPrintfToFile(fhandle, "Max_Channel        = %d\r\n", spConfig->stSysInfo.nMaxChannel);
		CfgPrintfToFile(fhandle, "Max_Resolution     = %d\r\n", spConfig->stSysInfo.nMaxResolution);

		// 1. Display Menu
		CfgPrintfToFile(fhandle, "\r\n[Display]\r\n");
		CfgPrintfToFile(fhandle, "Language           = %d ; 	0:English 1:Korean\r\n", spConfig->stDisplay.eLanguage);
		CfgPrintfToFile(fhandle, "TimeZone           = %d ; 	(+9 : Korea)\r\n", spConfig->stDisplay.nTimeZone);
		CfgPrintfToFile(fhandle, "UnitOfSpeed        = %d ; 	0:km/h	  1:mile/h	2:knot\r\n", spConfig->stDisplay.eUnitOfSpeed);
		CfgPrintfToFile(fhandle, "Disp_Mode          = %d ; 	0:OFF     1:Front Camera  2:Rear Camera  ", spConfig->stDisplay.eDispMode);
			CfgPrintfToFile(fhandle, "3:Dual Mode_1  4:Dual Mode_2\r\n");//  5:PIP Mode1  6:PIP Mode2

		CfgPrintfToFile(fhandle, "Video_Type         = %d ; 	0:NTSC    1:PAL\r\n", spConfig->stDisplay.eVideoType);
		
		// 2. Record MENU
		CfgPrintfToFile(fhandle, "\r\n\r\n[Record]\r\n");
#if (BUILD_MODEL != MODEL_KIC_M7 && BUILD_MODEL != MODEL_ICARO_I300HD && BUILD_MODEL != MODEL_DOOCO_M7 && BUILD_MODEL != MODEL_EYE_TRACKS)		
		CfgPrintfToFile(fhandle, "Driving_2ch        = %d ; 	0:Disable 1:Enable (Driving mode change to 2-channel record)\r\n", spConfig->stRecord.eDriving2Ch);
#endif
		i = 1;
#if (BUILD_MODEL == MODEL_KIC_M7 || BUILD_MODEL == MODEL_ICARO_I300HD || BUILD_MODEL == MODEL_DOOCO_M7 || BUILD_MODEL == MODEL_EYE_TRACKS)
		CfgPrintfToFile(fhandle, "Channel            = %d ; 	%d:1CH             ", spConfig->stRecord.eChannel, i++);
#else
		CfgPrintfToFile(fhandle, "Channel            = %d ; 	%d:1CH(FRONT) ", spConfig->stRecord.eChannel, i++);
#endif
#if CONFIG_MAX_CAMERA >= 2
		CfgPrintfToFile(fhandle, "%d:%dCH        ", i, i); 	i++;
#if CONFIG_MAX_CAMERA >= 3
		CfgPrintfToFile(fhandle, "%d:%dCH        ", i, i); 	i++;
#if CONFIG_MAX_CAMERA >= 4
		CfgPrintfToFile(fhandle, "%d:%dCH        ", i, i);	i++;
#endif	
#endif	
#endif
#if (BUILD_MODEL != MODEL_KIC_M7 && BUILD_MODEL != MODEL_ICARO_I300HD && BUILD_MODEL != MODEL_DOOCO_M7 && BUILD_MODEL != MODEL_EYE_TRACKS)
		CfgPrintfToFile(fhandle, "%d:1CH(REAR)  ", i++);
#endif
		//CfgPrintfToFile(fhandle, "%d:AUTO\r\n", i);
		CfgPrintfToFile(fhandle, "\r\n");

		CfgPrintfToFile(fhandle, "Mode               = %d ; 	0:CONTINUOUS 1:EVENT\r\n", spConfig->stRecord.eMode);
		CfgPrintfToFile(fhandle, "Type               = %d ; 	0:MPEG4   1:H.264\r\n", spConfig->stRecord.eCodecType);
		for( ch=0; ch < CONFIG_MAX_CAMERA; ch++ )
		{
			CfgPrintfToFile(fhandle, "Resolution_Ch%d     = %d ; 	", ch+1, spConfig->stRecord.eaResolution[ch]);
			i = 0;
#if RES_VGA_MODE_USE		
			CfgPrintfToFile(fhandle, "%d:QVGA    ", i++);
	 #if CONFIG_MAX_RESOLUTION >= 640
			CfgPrintfToFile(fhandle, "%d:VGA     ", i++);
	 #endif
#endif

#if CONFIG_MAX_RESOLUTION >= 720
			CfgPrintfToFile(fhandle, "%d:D1      ", i++);
#endif

#if CONFIG_MAX_RESOLUTION >= 1280
			CfgPrintfToFile(fhandle, "%d:HD      ", i++);
#endif

#if CONFIG_MAX_RESOLUTION >= 1920
			CfgPrintfToFile(fhandle, "%d:FULL-HD ", i++);
#endif	
	//		CfgPrintfToFile(fhandle, "%d:AUTO\r\n", i++);
			CfgPrintfToFile(fhandle, "\r\n");


			//Quality
			CfgPrintfToFile(fhandle, "Quality_ch%d        = %d ; 	0:LOWEST  1:LOW     2:NORMAL  3:HIGH    4:HIGHEST \r\n", ch+1, spConfig->stRecord.eaQuality[ch]);
		}
		CfgPrintfToFile(fhandle, "\r\n");
#if BUILD_MODEL == MODEL_HONDA_HD_2U || BUILD_MODEL == MODEL_RALLY_EYES
		CfgPrintfToFile(fhandle, "# Quality (D1 Mode Video Bitrate - Lowest:2Mbps ~ Highest:6Mbps, 1Mbps step)\r\n");
		CfgPrintfToFile(fhandle, "# Quality (HD Mode Video Bitrate - Lowest:4Mbps ~ Highest:12Mbps, 2Mbps step)\r\n");
#else
		CfgPrintfToFile(fhandle, "# Quality (D1 Mode Video Bitrate - Lowest:1Mbps ~ Highest:3Mbps, 0.5Mbps step)\r\n");
 #if (BUILD_MODEL == MODEL_KIC_M7 || BUILD_MODEL == MODEL_ICARO_I300HD || BUILD_MODEL == MODEL_DOOCO_M7 || BUILD_MODEL == MODEL_EYE_TRACKS)
		CfgPrintfToFile(fhandle, "# Quality (HD Mode Video Bitrate - Lowest:3Mbps ~ Highest:6Mbps, 0.75Mbps step)\r\n");
 #else
		CfgPrintfToFile(fhandle, "# Quality (HD Mode Video Bitrate - Lowest:3Mbps ~ Highest:8Mbps, 1.25Mbps step)\r\n");
 #endif
#endif 
		CfgPrintfToFile(fhandle, "\r\n");

		CfgPrintfToFile(fhandle, "Audio              = %d ; 	0:OFF	  1:ON\r\n", spConfig->stRecord.bAudioRecord);
	//	CfgPrintfToFile(fhandle, "Record_Time        = %d ; 	1:1 MIN	  2:2 MIN	  3:3 MIN	  4:5 MIN	  5:10 MIN\r\n", spConfig->stRecord.eRecordTime);
		CfgPrintfToFile(fhandle, "Event_Proportion   = %d ; 	0:1/2	  1:1/3 	2:1/4	  3:1/5\r\n", spConfig->stRecord.eEventProportion);

		// 3. Device	
		CfgPrintfToFile(fhandle, "\r\n\r\n[Device]\r\n");   
#if (BUILD_MODEL != MODEL_KIC_M7 && BUILD_MODEL != MODEL_ICARO_I300HD && BUILD_MODEL != MODEL_DOOCO_M7 && BUILD_MODEL != MODEL_EYE_TRACKS)		
		CfgPrintfToFile(fhandle, "Parking_Record_Alarm = %d ; 	0:OFF     1:Shock Event  2:Shock + Motion Event\r\n", (int)(spConfig->stDevice.eParkingRecordAlarm));
#endif
		CfgPrintfToFile(fhandle, "Speaker_Volume     = %d ; 	0 ~ 4\r\n", spConfig->stDevice.ulSpeakerVolume);
 #if (BUILD_MODEL != MODEL_BW_2000 && BUILD_MODEL != MODEL_KIC_M7 && BUILD_MODEL != MODEL_ICARO_I300HD && BUILD_MODEL != MODEL_DOOCO_M7 && BUILD_MODEL != MODEL_EYE_TRACKS)		
		CfgPrintfToFile(fhandle, "Usb_Mode           = %d ; 	0:MODE1   1:MODE2   2:MODE3\r\n", spConfig->stDevice.eUsbMode);
		CfgPrintfToFile(fhandle, "# USB MODE : MODE1-USB Only, MODE2-Normal USB/Event SD, MODE3-USB/SD Equal\r\n");

		CfgPrintfToFile(fhandle, "\r\n");
		CfgPrintfToFile(fhandle, "Record_Alarm       = %d ; 	0:OFF     1:RECORD  2:PARKING_RECORD 3:PARKING_SHOCK\r\n", (int)(spConfig->stDevice.eRecordAlarm));
		CfgPrintfToFile(fhandle, "# Record Alarm(EXT_MODULE_GPO Low/High control): 0 OFF(do not use), 1 RECORD(Start recording, set to High)\r\n");
		CfgPrintfToFile(fhandle, "# 2 PARKING_RECORD(When a Parking mode and event(with motion) is detected, set to High)\r\n");
		CfgPrintfToFile(fhandle, "# 3 PARKING_SHOCK(When a parking mode and shock event is detected, set to High)\r\n");
#endif		
		CfgPrintfToFile(fhandle, "\r\n[Accelerometer]\r\n");
		CfgPrintfToFile(fhandle, "Trigger_Level      = %d.%2d ;	0.1 ~ 1.0 ", (int)(spConfig->stDevice.dShockSensitive), (int)(spConfig->stDevice.dShockSensitive*100)%100); //float 값 출력 안됨
			CfgPrintfToFile(fhandle, " (0.2:HIGH 0.5:NORMAL 0.8:LOW, UNIT:g)\r\n");

		for( ch=0; ch < CONFIG_MAX_CAMERA; ch++ ) // motion front rear 분리 안됨
		{
			CfgPrintfToFile(fhandle, "\r\n[MotionDetect_Ch%d]\r\n", ch+1);
			//CfgPrintfToFile(fhandle, "\r\n[MotionDetect]\r\n");
			CfgPrintfToFile(fhandle, "Mode               = %d ;	0:OFF	  1:ON\r\n", spConfig->stDevice.staMotionSet[ch].bMode);
			CfgPrintfToFile(fhandle, "Sensitive          = %d ;	0 ~ 2", spConfig->stDevice.staMotionSet[ch].eSensitive);
				CfgPrintfToFile(fhandle, " (0:HIGH	1:NORMAL	2:LOW)\r\n");
			//CfgPrintfToFile(fhandle, "Block_Proportion   = %d ;	0 ~ 3", spConfig->stDevice.staMotionSet[ch].eBlockProportion);
			//CfgPrintfToFile(fhandle, " ( 0 : LOWER 1/2	1 : LOWER 2/3	2 : LOWER 3/4	3: FULL)\r\n");
		}
		
#if (BUILD_MODEL == MODEL_BW_2000 ||BUILD_MODEL == MODEL_KIC_M7 || BUILD_MODEL == MODEL_ICARO_I300HD || BUILD_MODEL == MODEL_DOOCO_M7 || BUILD_MODEL == MODEL_DOOCO_M7 || BUILD_MODEL == MODEL_EYE_TRACKS)	
		for( ch=0; ch < CONFIG_MAX_CAMERA; ch++ )
		{
			CfgPrintfToFile(fhandle, "\r\n[Camera_Ch%d]\r\n", ch+1);
			CfgPrintfToFile(fhandle, "Vertical_Flip      = %d ;	   0:OFF	 1:ON\r\n", spConfig->stDevice.staCameraSet[ch].eVerticalFlip);
			CfgPrintfToFile(fhandle, "Horizontal_Mirror  = %d ;	   0:OFF	 1:ON\r\n", spConfig->stDevice.staCameraSet[ch].bHorizontalMirror);
	//		CfgPrintfToFile(fhandle, "Brightness         = %d ;	   0 ~ 10\r\n", spConfig->stDevice.staCameraSet[ch].ulBrightness);
		}
#endif

		CfgPrintfToFile(fhandle, "\r\n\r\n[System]\r\n");
		CfgPrintfToFile(fhandle, "Buzzer_Mode        = %d ;	0:OFF	  1:SYSTEM	2:SYSTEM + KEY", spConfig->stSystem.eBuzzerMode);
			CfgPrintfToFile(fhandle, " 3:SYSTEM + EVENT 4:ALL ON\r\n");

		CfgPrintfToFile(fhandle, "LowVoltageMode     = %d ;	0:DUAL     1:VOLTAGE (0 DUAL:Sensing the voltage and power, 1 VOLTAGE:Sensing the voltage)\r\n", spConfig->stSystem.eLowVoltageMode);
#if (BUILD_MODEL == MODEL_BW_2000)
		CfgPrintfToFile(fhandle, "LowVoltageDetect   = %d.%d ;	11.8V ~ 12.4V(23.8 ~ 24.4V)\r\n", (int)spConfig->stSystem.fLowVoltage, (int)(spConfig->stSystem.fLowVoltage*10)%10);
#else
 #ifdef LOW_VOLTAGE_9_0_USE
 		CfgPrintfToFile(fhandle, "LowVoltageDetect   = %d.%d ;	9.0V ~ 12.6V(21.0 ~ 24.6V)\r\n", (int)spConfig->stSystem.fLowVoltage, (int)(spConfig->stSystem.fLowVoltage*10)%10);
 #else
		CfgPrintfToFile(fhandle, "LowVoltageDetect   = %d.%d ;	11.6V ~ 12.6V(23.6 ~ 24.6V)\r\n", (int)spConfig->stSystem.fLowVoltage, (int)(spConfig->stSystem.fLowVoltage*10)%10);
 #endif
#endif
		CfgPrintfToFile(fhandle, "Parking_Mode       = %d ;	0:OFF	  1:AUTO\r\n", spConfig->stSystem.eParkingMode);
		CfgPrintfToFile(fhandle, "Parking_Time       = %d ;	1 ~ 10 Minute (Parking mode entry times)\r\n", spConfig->stSystem.ulParkingTime);
#if (BUILD_MODEL != MODEL_BW_2000 && BUILD_MODEL != MODEL_KIC_M7 && BUILD_MODEL != MODEL_ICARO_I300HD && BUILD_MODEL != MODEL_DOOCO_M7 && BUILD_MODEL != MODEL_EYE_TRACKS)		
		CfgPrintfToFile(fhandle, "Parking_USB        = %d ;	0:OFF	  1:ON    	2:AUTO\r\n", spConfig->stSystem.eParkingUsb);
		CfgPrintfToFile(fhandle, "Parking_UsbOffVolt = %d.%d ;	0.2 ~ 1.0V (When setting a Parking_USB AUTO mode operation, LowVoltage + UsbOffVolt)\r\n", (int)spConfig->stSystem.fParkingUsbOffVolt, (int)(spConfig->stSystem.fParkingUsbOffVolt*10)%10);
		CfgPrintfToFile(fhandle, "Parking_LowPower   = %d ;	0:OFF	  1:ON    	2:AUTO\r\n", spConfig->stSystem.eParkingLowPower);
		CfgPrintfToFile(fhandle, "# Parking low power mode : OFF-30FPS, ON-15FPS(25%% Power save), AUTO-Event ready 15FPS / Event 30FPS\r\n");
#endif

#if CONFIG_USE_IR_LED		
		CfgPrintfToFile(fhandle, "IRLED_Mode         = %d ;	0:OFF	  1:AUTO\r\n", spConfig->stSystem.eIrLedMode);
#endif

		CfgPrintfToFile(fhandle, "\r\n\r\n[Time]\r\n");
		CfgPrintfToFile(fhandle, "Year                = %d ;\r\n", spConfig->stTime.nYear);
		CfgPrintfToFile(fhandle, "Month               = %d ;\r\n", spConfig->stTime.nMonth);
		CfgPrintfToFile(fhandle, "Date                = %d ;\r\n", spConfig->stTime.nDate);
		CfgPrintfToFile(fhandle, "Hour                = %d ;\r\n", spConfig->stTime.nHour);
		CfgPrintfToFile(fhandle, "Minute              = %d ;\r\n", spConfig->stTime.nMinute);
		CfgPrintfToFile(fhandle, "Sec                 = %d ;\r\n", spConfig->stTime.nSec);
		CfgPrintfToFile(fhandle, "Time_Set            = %d ; 	0:NOT SET 1:SET\r\n", spConfig->stTime.nTimeSet);	//spConfig->stTime.nTimeSet
		
		file.Close();
	}
	else
		DEBUGMSG(ZONE_CFG_PRINT, ("file open ERROR! (%s)\n", fileName.GetString()));
	
	return fileName;
}


CString CConfigText::Save(CString cfg_file_dir, LPST_CFG_DAVIEW spConfig)
{
	CFile file;
	int ch;
	int ret;

	bool		b;
	int		i;
	double		d;
	const char*		c;
	
	CFile *fhandle = NULL;
	CFileFind finder;
	CString fileName;

	fileName = cfg_file_dir;
	if(fileName.Find(_T("Config")) >= 0)
	{
		fileName.Append(_T("\\"));
		fileName.Append(BLACKBOX_CONFIG_FILE_NAME);
	}
	else
	{
		fileName.Append(_T("\\"));
		fileName.Append(BLACKBOX_CONFIG_FILE);
	}

	TiXmlDocument xmlDoc;
	CT2A ascii(fileName);

	xmlDoc.LoadFile(ascii.m_psz);

	TiXmlElement* pSetup = xmlDoc.FirstChildElement("setup");

	if(!pSetup) return FALSE;

	TiXmlElement* pEleItem = pSetup->FirstChildElement("item");
	while( pEleItem )
	{
		const char* lpszId = pEleItem->Attribute( "id" );

		if(!strcmp(lpszId, "Name")){
			c = pEleItem->Attribute( "value" );
			if(c) strcpy(spConfig->strName, c);
		}
		else if(!strcmp(lpszId, "FWVersion")){
			c = pEleItem->Attribute( "value" );
			if(c) strcpy(spConfig->strFWVersion, c);
		}
		else if(!strcmp(lpszId, "CarNumber")){
			pEleItem->SetAttribute( "value", spConfig->strCarNumber);
		}  // jun 200805
		else if(!strcmp(lpszId, "DriverCode")){
			CStringA UTF8Str=CW2A(spConfig->strDriverCode,CP_UTF8);
			pEleItem->SetAttribute( "value", UTF8Str);
		}  // jun 221017
		else if(!strcmp(lpszId, "GSensorSensi")){
			pEleItem->SetAttribute( "value", spConfig->iGsensorSensi);
		}
		else if(!strcmp(lpszId, "GMT")){
			pEleItem->SetAttribute( "value", spConfig->iGmt);
		}
		else if(!strcmp(lpszId, "AudioRecEnable")){
			if(spConfig->iAudioRecEnable)
				pEleItem->SetAttribute( "value", "TRUE");
			else
				pEleItem->SetAttribute( "value", "FALSE");
		}
		else if(!strcmp(lpszId, "SpeakerVol")){
			pEleItem->SetAttribute( "value", spConfig->iSpeakerVol);
		}
		// jun 20201202
		else if(!strcmp(lpszId, "EngineCylinders")){
			pEleItem->SetAttribute( "value", spConfig->iEngineCylinders);
		}
		else if(!strcmp(lpszId, "VideoQuality")){
			if(spConfig->iVideoQuality==0)
				pEleItem->SetAttribute( "value", "HIGH" );
			else if(spConfig->iVideoQuality==2)
				pEleItem->SetAttribute( "value", "LOW" );
			else
				pEleItem->SetAttribute( "value", "MIDDLE" );
		}
		else if(!strcmp(lpszId, "EventMode")){
			if(spConfig->iEventMode == 0)
				pEleItem->SetAttribute( "value", "A" );
			else if(spConfig->iEventMode == 1)
				pEleItem->SetAttribute( "value", "B" );
			else 
				pEleItem->SetAttribute( "value", "C" );
		}
		else if(!strcmp(lpszId, "AutoPmEnable")){
			pEleItem->QueryBoolAttribute( "value", &spConfig->bAutoPmEnable);
		}
		else if(!strcmp(lpszId, "AutoPmEnterTime")){
			pEleItem->QueryIntAttribute( "value", &spConfig->iAutoPmEnterTime);
		}
		else if(!strcmp(lpszId, "PmImpactSensi")){
			pEleItem->QueryIntAttribute( "value", &spConfig->iPmImpactSensi);
		}
		else if(!strcmp(lpszId, "PmMotionEnable")){
			pEleItem->QueryBoolAttribute( "value", &spConfig->bPmMotionEnable);
		}
		else if(!strcmp(lpszId, "PmMotionSensi")){
			pEleItem->QueryIntAttribute( "value", &spConfig->iPmMotionSensi);
		}
		else if(!strcmp(lpszId, "PmMotionSensi2")){
			pEleItem->QueryIntAttribute( "value", &spConfig->iPmMotionSensi2);
		}
		else if(!strcmp(lpszId, "CarBatVoltage")){
			c = pEleItem->Attribute( "value" );
			if(c) {
				if(!strcmp(c, "24V")) spConfig->iCarBatVoltage = 1;
				else spConfig->iCarBatVoltage = 0;
			}
		}
		else if(!strcmp(lpszId, "CarBatVoltCalib")){
			pEleItem->QueryIntAttribute( "value", &spConfig->iCarBatVoltageCalib);
		}
		else if(!strcmp(lpszId, "CarBatSafeEnable")){
			pEleItem->QueryBoolAttribute( "value", &spConfig->bCarBatSafeEnable);
		}
		else if(!strcmp(lpszId, "CarBatSafeVoltage")){
			pEleItem->QueryIntAttribute( "value", &spConfig->iCarBatSafeVoltage);
		}
		else if(!strcmp(lpszId, "TempSafeEnable")){
			pEleItem->QueryBoolAttribute( "value", &spConfig->bTempSafeEnable);
		}
		else if(!strcmp(lpszId, "TempSafeValue")){
			pEleItem->QueryIntAttribute( "value", &spConfig->iTempSafeValue);
		}
		else if(!strcmp(lpszId, "TempStableValue")){
			pEleItem->QueryIntAttribute( "value", &spConfig->iTempStableValue);
		}
		else if(!strcmp(lpszId, "SecurityLEDEnable")){
			pEleItem->QueryBoolAttribute( "value", &spConfig->bSecurityLEDEnable);
		}
		else if(!strcmp(lpszId, "SecurityLEDPeriod")){
			pEleItem->QueryIntAttribute( "value", &spConfig->iSecurityLEDPeriod);
		}
		else if(!strcmp(lpszId, "SecurityLEDMDWarning")){
			pEleItem->QueryBoolAttribute( "value", &spConfig->bSecurityLEDMDWarning);
		}
		else if(!strcmp(lpszId, "SecurityLEDOperating")){
			pEleItem->QueryIntAttribute( "value", &spConfig->iSecurityLEDOperating);
		}
		else if(!strcmp(lpszId, "LcdOffTime")){
			pEleItem->QueryIntAttribute( "value", &spConfig->iLcdOffTime);
		}
		else if(!strcmp(lpszId, "VideoOut")){
			c = pEleItem->Attribute( "value" );
			if(c) {
				if(!strcmp(c, "LCD")) spConfig->iVideoOut = 1;
				else spConfig->iVideoOut = 0;
			}
		}
		else if(!strcmp(lpszId, "FactoryReset")){
			if(spConfig->bFactoryReset)
				pEleItem->SetAttribute( "value", "TRUE");
			else
				pEleItem->SetAttribute( "value", "FALSE");
		}
		else if(!strcmp(lpszId, "PulseReset")){
			if(spConfig->iPulseReset)
				pEleItem->SetAttribute( "value", "TRUE");
			else
				pEleItem->SetAttribute( "value", "FALSE");
		}
		else if(!strcmp(lpszId, "BRAKE")){
			if(spConfig->iPulseTacho1LowHigh)
				pEleItem->SetAttribute( "value", "1");
			else
				pEleItem->SetAttribute( "value", "0");
		}
		else if(!strcmp(lpszId, "INPUT1")){
			if(spConfig->iPulseTacho4LowHigh)
				pEleItem->SetAttribute( "value", "1");
			else
				pEleItem->SetAttribute( "value", "0");
		}
		else if(!strcmp(lpszId, "INPUT2")){
			if(spConfig->iPulseTacho5LowHigh)
				pEleItem->SetAttribute( "value", "1");
			else
				pEleItem->SetAttribute( "value", "0");
		}
		else if(!strcmp(lpszId, "OSDSpeed")){          // jun 200813
			if(spConfig->iOsdSpeed)
				pEleItem->SetAttribute( "value", "1");
			else
				pEleItem->SetAttribute( "value", "0");
		}
		else if(!strcmp(lpszId, "LastSetupTime")){
			pEleItem->SetAttribute( "value", spConfig->strLastSetupTime);
		}
		
		pEleItem = pEleItem->NextSiblingElement();
	}

	xmlDoc.SaveFile(ascii.m_psz);
	xmlDoc.Clear();
	
	return fileName;
}

CString CConfigText::SaveVersion(CString cfg_file_dir, char * strVersion)
{
	CString fileName;
	CFile file;
	
	fileName = cfg_file_dir;
	fileName.Append(_T("\\"));
	fileName.Append(_T("Version.lst"));

	if(file.Open(fileName.GetBuffer(), CFile::modeCreate | CFile::modeReadWrite))
	{
		CfgPrintfToFile(&file, "%s,Firstview_setup.msi,UNregister.bat,Uninstall.bat,\r\n", strVersion);
		file.Close();
	}
	
	return fileName;
}

BOOL CConfigText::CfgPrintfToFile(CFile *fhandle, char *fmt, ... )
{
	BOOL bRet = FALSE;
   	va_list argP;
   	char	string[255];
   
   	va_start(argP, fmt);
   	vsprintf(string, fmt, argP);
   
	if(fhandle)
	{
		fhandle->Write(string, strlen(string));
		bRet = TRUE;
	}
	
	va_end(argP);

	return bRet;
}


BOOL CConfigText::CfgParserFile(CString cfg_file_name, LPST_CFG_DAVIEW spConfig)
{
	int ch;
	
	/* Some temporary variables to hold query results */
	bool		b;
	int		i;
	double		d;
	const char*		c;

	TiXmlDocument xmlDoc;
	CT2A ascii(cfg_file_name);

	xmlDoc.LoadFile(ascii.m_psz);

	TiXmlElement* pSetup = xmlDoc.FirstChildElement("setup");

	if(!pSetup) return FALSE;
	
	DEBUGMSG(ZONE_CFG_PRINT, ("-------------------------------\n"));

	TiXmlElement* pEleItem = pSetup->FirstChildElement("item");
	while( pEleItem )
	{
		const char* lpszId = pEleItem->Attribute( "id" );

		if(!strcmp(lpszId, "Name")){
			c = pEleItem->Attribute( "value" );
			if(c) strcpy(spConfig->strName, c);
		}
		else if(!strcmp(lpszId, "FWVersion")){
			c = pEleItem->Attribute( "value" );
			if(c) strcpy(spConfig->strFWVersion, c);
		}
		else if(!strcmp(lpszId, "CarNumber")){
			c = pEleItem->Attribute( "value" );
			if(c) strcpy(spConfig->strCarNumber, c);
		}  // jun 200805
		else if(!strcmp(lpszId, "DriverCode")){
			c = pEleItem->Attribute( "value" );
			if(c){
			CString UTF8Str=CA2W(c,CP_UTF8);
				wcscpy(spConfig->strDriverCode, UTF8Str);
				}
		}  // jun 221017
		else if(!strcmp(lpszId, "GSensorSensi")){
			pEleItem->QueryIntAttribute( "value", &spConfig->iGsensorSensi);
		}
		else if(!strcmp(lpszId, "GMT")){
			pEleItem->QueryIntAttribute( "value", &spConfig->iGmt);
		}
		else if(!strcmp(lpszId, "AudioRecEnable")){
			pEleItem->QueryBoolAttribute( "value", &b);
			spConfig->iAudioRecEnable = (int)b;
		}
		else if(!strcmp(lpszId, "SpeakerVol")){
			pEleItem->QueryIntAttribute( "value", &spConfig->iSpeakerVol);
		}
		// jun 20201202
		else if(!strcmp(lpszId, "EngineCylinders")){
			pEleItem->QueryIntAttribute( "value", &spConfig->iEngineCylinders);
		}
		else if(!strcmp(lpszId, "VideoQuality")){
			c = pEleItem->Attribute( "value" );
			if(c) {
				if(!strcmp(c, "LOW")) spConfig->iVideoQuality = 2;
				else if(!strcmp(c, "HIGH")) spConfig->iVideoQuality = 0;
				else  spConfig->iVideoQuality = 1;
			}
		}
		else if(!strcmp(lpszId, "EventMode")){
			c = pEleItem->Attribute( "value" );
			if(c) {
				if(!strcmp(c, "A")) spConfig->iEventMode = 0;
				else if(!strcmp(c, "C")) spConfig->iEventMode = 2;
				else spConfig->iEventMode = 1;
			}
		}
		else if(!strcmp(lpszId, "AutoPmEnable")){
			pEleItem->QueryBoolAttribute( "value", &spConfig->bAutoPmEnable);
		}
		else if(!strcmp(lpszId, "AutoPmEnterTime")){
			pEleItem->QueryIntAttribute( "value", &spConfig->iAutoPmEnterTime);
		}
		else if(!strcmp(lpszId, "PmImpactSensi")){
			pEleItem->QueryIntAttribute( "value", &spConfig->iPmImpactSensi);
		}
		else if(!strcmp(lpszId, "PmMotionEnable")){
			pEleItem->QueryBoolAttribute( "value", &spConfig->bPmMotionEnable);
		}
		else if(!strcmp(lpszId, "PmMotionSensi")){
			pEleItem->QueryIntAttribute( "value", &spConfig->iPmMotionSensi);
		}
		else if(!strcmp(lpszId, "PmMotionSensi2")){
			pEleItem->QueryIntAttribute( "value", &spConfig->iPmMotionSensi2);
		}
		else if(!strcmp(lpszId, "CarBatVoltage")){
			c = pEleItem->Attribute( "value" );
			if(c) {
				if(!strcmp(c, "24V")) spConfig->iCarBatVoltage = 1;
				else spConfig->iCarBatVoltage = 0;
			}
		}
		else if(!strcmp(lpszId, "CarBatVoltCalib")){
			pEleItem->QueryIntAttribute( "value", &spConfig->iCarBatVoltageCalib);
		}
		else if(!strcmp(lpszId, "CarBatSafeEnable")){
			pEleItem->QueryBoolAttribute( "value", &spConfig->bCarBatSafeEnable);
		}
		else if(!strcmp(lpszId, "CarBatSafeVoltage")){
			pEleItem->QueryIntAttribute( "value", &spConfig->iCarBatSafeVoltage);
		}
		else if(!strcmp(lpszId, "TempSafeEnable")){
			pEleItem->QueryBoolAttribute( "value", &spConfig->bTempSafeEnable);
		}
		else if(!strcmp(lpszId, "TempSafeValue")){
			pEleItem->QueryIntAttribute( "value", &spConfig->iTempSafeValue);
		}
		else if(!strcmp(lpszId, "TempStableValue")){
			pEleItem->QueryIntAttribute( "value", &spConfig->iTempStableValue);
		}
		else if(!strcmp(lpszId, "SecurityLEDEnable")){
			pEleItem->QueryBoolAttribute( "value", &spConfig->bSecurityLEDEnable);
		}
		else if(!strcmp(lpszId, "SecurityLEDPeriod")){
			pEleItem->QueryIntAttribute( "value", &spConfig->iSecurityLEDPeriod);
		}
		else if(!strcmp(lpszId, "SecurityLEDMDWarning")){
			pEleItem->QueryBoolAttribute( "value", &spConfig->bSecurityLEDMDWarning);
		}
		else if(!strcmp(lpszId, "SecurityLEDOperating")){
			pEleItem->QueryIntAttribute( "value", &spConfig->iSecurityLEDOperating);
		}
		else if(!strcmp(lpszId, "LcdOffTime")){
			pEleItem->QueryIntAttribute( "value", &spConfig->iLcdOffTime);
		}
		else if(!strcmp(lpszId, "VideoOut")){
			c = pEleItem->Attribute( "value" );
			if(c) {
				if(!strcmp(c, "LCD")) spConfig->iVideoOut = 1;
				else spConfig->iVideoOut = 0;
			}
		}
		else if(!strcmp(lpszId, "FactoryReset")){
			pEleItem->QueryBoolAttribute( "value", &spConfig->bFactoryReset);
		}
		else if(!strcmp(lpszId, "PulseReset")){
			bool bReset = (bool)spConfig->iPulseReset;
			pEleItem->QueryBoolAttribute( "value", &bReset);
			spConfig->iPulseReset = (int)bReset;
		}
		else if(!strcmp(lpszId, "BRAKE")){
			bool bValue = (bool)spConfig->iPulseTacho1LowHigh;
			pEleItem->QueryBoolAttribute( "value", &bValue);
			spConfig->iPulseTacho1LowHigh = (int)bValue;
		}
		else if(!strcmp(lpszId, "INPUT1")){
			bool bValue = (bool)spConfig->iPulseTacho4LowHigh;
			pEleItem->QueryBoolAttribute( "value", &bValue);
			spConfig->iPulseTacho4LowHigh = (int)bValue;
		}
		else if(!strcmp(lpszId, "INPUT2")){
			bool bValue = (bool)spConfig->iPulseTacho5LowHigh;
			pEleItem->QueryBoolAttribute( "value", &bValue);
			spConfig->iPulseTacho5LowHigh = (int)bValue;
		}
		else if(!strcmp(lpszId, "OSDSpeed")){        // jun 200813
			bool bValue = (bool)spConfig->iOsdSpeed;
			pEleItem->QueryBoolAttribute( "value", &bValue);
			spConfig->iOsdSpeed = (int)bValue;
		}
		else if(!strcmp(lpszId, "LastSetupTime")){
			c = pEleItem->Attribute( "value" );
			if(c) strcpy(spConfig->strLastSetupTime, c);
		}
		pEleItem = pEleItem->NextSiblingElement();
	}

	xmlDoc.Clear();
	return TRUE;

}

BOOL CConfigText::CfgParserFile(CString cfg_file_name, LPST_CFG_ITEM spConfig)
{
	DICTIONARY	*p_CfgDt;
	CTParser	cfgTPar;
	ST_CFG_ITEM	stCfgDefault;
	char szKey[OBJECT_LEN] = {0};
	int ch;
	
	/* Some temporary variables to hold query results */
	UINT32		b;
	UINT32		i;
	double		d;
	char*		c;

	p_CfgDt = cfgTPar.Load(cfg_file_name.GetBuffer());
	
	if(!p_CfgDt) {
		DEBUGMSG(ZONE_CFG_PRINT, ("CfgParserFile(): Load ERROR!!\n"));
		return FALSE;
	}
	
	CfgDefaultSet(&stCfgDefault);
	
	DEBUGMSG(ZONE_CFG_PRINT, ("-------------------------------\n"));

// 0. System Info
	DEBUGMSG(ZONE_CFG_PRINT, ("\n*******	system  ******\n"));
	c = cfgTPar.GetStr(p_CfgDt, "system_info:fw_version", stCfgDefault.stSysInfo.cFwVersion);
	if(c != NULL)
		strcpy(spConfig->stSysInfo.cFwVersion, c);
	else
		sprintf(spConfig->stSysInfo.cFwVersion, " -");
	
	DEBUGMSG(ZONE_CFG_PRINT, ("F/W Version : %s\n", c));

	c = cfgTPar.GetStr(p_CfgDt, "system_info:viewer_version", stCfgDefault.stSysInfo.cViewerVersion);
	if(c != NULL)
		strcpy(spConfig->stSysInfo.cViewerVersion, c);
	else
		sprintf(spConfig->stSysInfo.cViewerVersion, " -");
	
	DEBUGMSG(ZONE_CFG_PRINT, ("Viewer Version : %s\n", c));

	i = cfgTPar.GetInt(p_CfgDt, "system_info:max_channel", stCfgDefault.stSysInfo.nMaxChannel);
	spConfig->stSysInfo.nMaxChannel = i;
	DEBUGMSG(ZONE_CFG_PRINT, ("Max_Channel:				[%d]\n", i));	 

	i = cfgTPar.GetInt(p_CfgDt, "system_info:max_resolution", stCfgDefault.stSysInfo.nMaxResolution);
	spConfig->stSysInfo.nMaxResolution= i;
	DEBUGMSG(ZONE_CFG_PRINT, ("Max_Resolution:				[%d]\n", i));	 

// 1. Display
	DEBUGMSG(ZONE_CFG_PRINT, ("\n*******	Display  ******\n"));
	i = cfgTPar.GetInt(p_CfgDt, "display:language", stCfgDefault.stDisplay.eLanguage);
	spConfig->stDisplay.eLanguage = (ENUM_LANGUAGE)i;
	DEBUGMSG(ZONE_CFG_PRINT, ("Language:				[%d]\n", i));	 

	i = cfgTPar.GetInt(p_CfgDt, "display:timezone", stCfgDefault.stDisplay.nTimeZone);
	spConfig->stDisplay.nTimeZone = i;
	DEBUGMSG(ZONE_CFG_PRINT, ("TimeZone:				[%d]\n", i)); 

	i = cfgTPar.GetInt(p_CfgDt, "display:unitofspeed", stCfgDefault.stDisplay.eUnitOfSpeed);
	spConfig->stDisplay.eUnitOfSpeed = (ENUM_UNIT_OF_SPEED)i;
	DEBUGMSG(ZONE_CFG_PRINT, ("UnitOfSpeed: 			   [%d]\n", i)); 
	

	i = cfgTPar.GetInt(p_CfgDt, "display:disp_mode", stCfgDefault.stDisplay.eDispMode);
#if (BUILD_MODEL == MODEL_BW_2000 || BUILD_MODEL == MODEL_Q_BLACK || BUILD_MODEL == MODEL_KIC_M7 || BUILD_MODEL == MODEL_ICARO_I300HD || BUILD_MODEL == MODEL_DOOCO_M7 || BUILD_MODEL == MODEL_EYE_TRACKS)
	if(i > DISPMODE_DUAL1)
		i = DISPMODE_DUAL1;
#endif
	spConfig->stDisplay.eDispMode = (ENUM_DISPLAYMODE)i;
	DEBUGMSG(ZONE_CFG_PRINT, ("Dideo_Mode:				 [%d]\n", i));

	i = cfgTPar.GetInt(p_CfgDt, "display:video_type", stCfgDefault.stDisplay.eVideoType);
	spConfig->stDisplay.eVideoType = (ENUM_VIDEOTYPE)i;
	DEBUGMSG(ZONE_CFG_PRINT, ("Video_Type:				 [%d]\n", i));

// 2. Record
	DEBUGMSG(ZONE_CFG_PRINT, ("\n*******	Record  ******\n"));

	i = cfgTPar.GetInt(p_CfgDt, "record:driving_2ch", stCfgDefault.stRecord.eDriving2Ch);
	if( i <= RECORD_DRIVING_2CH_ENUM_END)
		spConfig->stRecord.eDriving2Ch = (ENUM_RECORD_DRIVING_2CH)i;
	else
		spConfig->stRecord.eDriving2Ch = RECORD_DRIVING_2CH_DISABLE;
	DEBUGMSG(ZONE_CFG_PRINT, ("Driving 2ch: 				[%d]\n", i));
	
	i = cfgTPar.GetInt(p_CfgDt, "record:channel", stCfgDefault.stRecord.eChannel);

	if( i < RECORD_CHANNEL_ENUM_END)
		spConfig->stRecord.eChannel = (ENUM_RECORD_CHANNEL)i;
	else
		spConfig->stRecord.eChannel = (ENUM_RECORD_CHANNEL)CONFIG_MAX_CAMERA;	
	DEBUGMSG(ZONE_CFG_PRINT, ("Channel: 				[%d]\n", i));
	
	i = cfgTPar.GetInt(p_CfgDt, "record:mode", stCfgDefault.stRecord.eMode);
	spConfig->stRecord.eMode = (ENUM_RECORD_MODE)i;
	DEBUGMSG(ZONE_CFG_PRINT, ("Mode:					[%d]\n", i));
	
	i = cfgTPar.GetInt(p_CfgDt, "record:type", stCfgDefault.stRecord.eCodecType);  //VIDEO_CODEC_TYPE_H264
	if(i) i = VIDEO_CODEC_TYPE_H264;
	spConfig->stRecord.eCodecType = (ENUM_VIDEO_CODEC)i;
	DEBUGMSG(ZONE_CFG_PRINT, ("Type:					[%d]\n", i));

	for ( ch=0; ch < CONFIG_MAX_CAMERA; ch++ )
	{
		sprintf((char*)szKey, "record:resolution_ch%d", ch+1);
		i = cfgTPar.GetInt(p_CfgDt, szKey, stCfgDefault.stRecord.eaResolution[ch]);
		spConfig->stRecord.eaResolution[ch] = (ENUM_VIDEO_RESOLUTION)i;
		DEBUGMSG(ZONE_CFG_PRINT, ("Resolution Ch%d:				[%d]\n", ch+1, i));

		sprintf((char*)szKey, "record:quality_ch%d", ch+1);
		i = cfgTPar.GetInt(p_CfgDt, szKey, stCfgDefault.stRecord.eaQuality[ch]);
		spConfig->stRecord.eaQuality[ch] = (ENUM_VIDEO_QUALITY)i;
		DEBUGMSG(ZONE_CFG_PRINT, ("Quality Ch%d:				[%d]\n", ch+1, i));
	}

	b = cfgTPar.GetBool(p_CfgDt, "record:audio", stCfgDefault.stRecord.bAudioRecord);
	spConfig->stRecord.bAudioRecord = b;
	DEBUGMSG(ZONE_CFG_PRINT, ("Audio:					[%d]\n", b));

	i = cfgTPar.GetInt(p_CfgDt, "record:record_time", stCfgDefault.stRecord.eRecordTime);
	spConfig->stRecord.eRecordTime = (ENUM_REC_TIME)i;
	DEBUGMSG(ZONE_CFG_PRINT, ("Record Time: 			[%d]\n\n", i));  

	i = cfgTPar.GetInt(p_CfgDt, "record:event_proportion", stCfgDefault.stRecord.eEventProportion);
	if(i < 0 || i >= EVENT_PROPORTION_ENUM_END)
		i = stCfgDefault.stRecord.eEventProportion;
    spConfig->stRecord.eEventProportion = (ENUM_EVENT_PROPORTION)i;
    DEBUGMSG(ZONE_CFG_PRINT, ("Event file proportion:   [%ld]\n", i));
	

// 3. Device
	DEBUGMSG(ZONE_CFG_PRINT, ("\n*******	USB MODE  ******\n"));   
	i = cfgTPar.GetInt(p_CfgDt, "device:usb_mode", stCfgDefault.stDevice.eUsbMode);
	spConfig->stDevice.eUsbMode = (ENUM_USB_MODE)i;
	 DEBUGMSG(ZONE_CFG_PRINT, ("Usb Mode:   [%ld]\n", i));

	DEBUGMSG(ZONE_CFG_PRINT, ("\n*******	Event Alarm Mode  ******\n"));   
	i = cfgTPar.GetInt(p_CfgDt, "device:parking_record_alarm", stCfgDefault.stDevice.eParkingRecordAlarm);
	spConfig->stDevice.eParkingRecordAlarm = (ENUM_PARKING_RECORD_ALARM)i;
	 DEBUGMSG(ZONE_CFG_PRINT, ("Parking Record Alarm :   [%ld]\n", i));

	 DEBUGMSG(ZONE_CFG_PRINT, ("\n*******	Speaker Volume  ******\n"));   
	i = cfgTPar.GetInt(p_CfgDt, "device:speaker_volume", stCfgDefault.stDevice.ulSpeakerVolume);
	spConfig->stDevice.ulSpeakerVolume = i;
	 DEBUGMSG(ZONE_CFG_PRINT, ("Speaker Volume:   [%ld]\n", i));
	 
// 3. Device - Alarm
	DEBUGMSG(ZONE_CFG_PRINT, ("\n*******	Alarm Mode  ******\n"));   
	i = cfgTPar.GetInt(p_CfgDt, "device:record_alarm", stCfgDefault.stDevice.eRecordAlarm);
	if( i >= RECORD_ALARM_ENUM_END)
		i = DEFAULT_RECORD_ALARM;
	spConfig->stDevice.eRecordAlarm = (ENUM_RECORD_ALARM_MODE)i;
	DEBUGMSG(ZONE_CFG_PRINT, ("Record Alarm: 			[%d]\n", i));
	
// 3. Device - Accelerometer
	DEBUGMSG(ZONE_CFG_PRINT, ("\n*******	Accelerometer  ******\n"));   
	d = cfgTPar.GetDouble(p_CfgDt, "accelerometer:trigger_level", stCfgDefault.stDevice.dShockSensitive);
	if(d < 0.05) d = 0.05;
	else if (d > 1.0) d = 1.0;
	spConfig->stDevice.dShockSensitive = d;
	DEBUGMSG(ZONE_CFG_PRINT, ("Trigger Level:			[%d.%2d]\n", (int)d, (int)(d*100)%100));

	for ( ch=0; ch < CONFIG_MAX_CAMERA; ch++ )
	{
// 3. Device Motion		
		DEBUGMSG(ZONE_CFG_PRINT, ("\n*****  Motion Detection %d	*****\n", ch+1));
		sprintf((char*)szKey, "motiondetect_ch%d:mode", ch+1);
		b = cfgTPar.GetBool(p_CfgDt, szKey, stCfgDefault.stDevice.staMotionSet[ch].bMode);
		spConfig->stDevice.staMotionSet[ch].bMode = b;
		DEBUGMSG(ZONE_CFG_PRINT, ("Mode:					[%d]\n", b));

		sprintf((char*)szKey, "motiondetect_ch%d:sensitive", ch+1);
		i = cfgTPar.GetInt(p_CfgDt, szKey, stCfgDefault.stDevice.staMotionSet[ch].eSensitive);
		spConfig->stDevice.staMotionSet[ch].eSensitive = (ENUM_SENSITIVE)i;
		DEBUGMSG(ZONE_CFG_PRINT, ("Sensitive:				[%d]\n", i));

		sprintf((char*)szKey, "motiondetect_ch%d:block_proportion", ch+1);
		i = cfgTPar.GetInt(p_CfgDt, szKey, stCfgDefault.stDevice.staMotionSet[ch].eBlockProportion);
		spConfig->stDevice.staMotionSet[ch].eBlockProportion = (eMD_BLOCK_PROPORTION)i;
		DEBUGMSG(ZONE_CFG_PRINT, ("Block Proportion:        [%ld]\n", i));

// 3. Device Camera 
		DEBUGMSG(ZONE_CFG_PRINT, ("\n********** Camera %d	**********\n", ch+1));
		sprintf((char*)szKey, "camera_ch%d:vertical_flip", ch+1);
		i = cfgTPar.GetInt(p_CfgDt, szKey, stCfgDefault.stDevice.staCameraSet[ch].eVerticalFlip);
		spConfig->stDevice.staCameraSet[ch].eVerticalFlip = (ENUM_VERITCAL_FLIP)i;
		DEBUGMSG(ZONE_CFG_PRINT, ("Vertical Flip:			[%d]\n", i));

		sprintf((char*)szKey, "camera_ch%d:horizontal_mirror", ch+1);
		i = cfgTPar.GetInt(p_CfgDt, szKey, stCfgDefault.stDevice.staCameraSet[ch].bHorizontalMirror);
		spConfig->stDevice.staCameraSet[ch].bHorizontalMirror = i;
		DEBUGMSG(ZONE_CFG_PRINT, ("Horizontal Mirror:		[%d]\n", i));

		sprintf((char*)szKey, "camera_ch%d:brightness", ch+1);
		i = cfgTPar.GetInt(p_CfgDt, szKey, stCfgDefault.stDevice.staCameraSet[ch].ulBrightness);
		spConfig->stDevice.staCameraSet[ch].ulBrightness = i;
		DEBUGMSG(ZONE_CFG_PRINT, ("Brightness:				[%ld]\n", i));
	}	

	

// 4. System
	DEBUGMSG(ZONE_CFG_PRINT, ("\n************  System  ***********\n"));
	i = cfgTPar.GetInt(p_CfgDt, "system:buzzer_mode", stCfgDefault.stSystem.eBuzzerMode);
	spConfig->stSystem.eBuzzerMode = (ENUM_BUZZER_MODE)i;
	DEBUGMSG(ZONE_CFG_PRINT, ("Buzzer Mode: 			[%d]\n", i));

	i = cfgTPar.GetInt(p_CfgDt, "system:lowvoltagemode", stCfgDefault.stSystem.eLowVoltageMode);
	if( i >=LOW_VOLTAGE_MODE_END )
		i = DEFAULT_LOW_VOLTAGE_MODE;
	spConfig->stSystem.eLowVoltageMode = (ENUM_LOW_VOLTAGE_MODE)i;
	DEBUGMSG(ZONE_CFG_PRINT, ("Low Voltage Mode: 		[%d]\n", i));


	d = cfgTPar.GetDouble(p_CfgDt, "system:lowvoltagedetect", stCfgDefault.stSystem.fLowVoltage);


	if(d < LOW_VOLTAGE_MIN_LEVEL || d>24.6)
		d=stCfgDefault.stSystem.fLowVoltage;
	
	spConfig->stSystem.fLowVoltage = (float)d;
	DEBUGMSG(ZONE_CFG_PRINT, ("Low Voltage Detect:		[%d.%d V]\n", (int)d, (int)(d*10)%10));

	i = cfgTPar.GetInt(p_CfgDt, "system:parking_mode", stCfgDefault.stSystem.eParkingMode);
	spConfig->stSystem.eParkingMode = (ENUM_PARKING_MODE)i;
	DEBUGMSG(ZONE_CFG_PRINT, ("Parking Mode :			[%d]\n", i));

	i = cfgTPar.GetInt(p_CfgDt, "system:parking_time", stCfgDefault.stSystem.ulParkingTime);
	if( i < 1 || i > 10)
		i = DEFAULT_PARKING_TIME;
	spConfig->stSystem.ulParkingTime = (UINT)i;
	DEBUGMSG(ZONE_CFG_PRINT, ("Parking Time :			[%d]\n", i));
	
	i = cfgTPar.GetInt(p_CfgDt, "system:parking_usb", stCfgDefault.stSystem.eParkingUsb);
	spConfig->stSystem.eParkingUsb = (ENUM_PARKING_USB)i;
	DEBUGMSG(ZONE_CFG_PRINT, ("Parking USB :			[%d]\n", i));

	d = cfgTPar.GetDouble(p_CfgDt, "system:parking_usboffvolt", stCfgDefault.stSystem.fParkingUsbOffVolt);
	if(d < 0.2 || d > 1.0 )
		d = DEFAULT_PARKING_USB_OFF_VOLT;
	spConfig->stSystem.fParkingUsbOffVolt = (float)d;
	DEBUGMSG(ZONE_CFG_PRINT, ("Parking USB Off Volt :	[%d.%d V]\n", (int)d, (int)(d*10)%10));
	
	i = cfgTPar.GetInt(p_CfgDt, "system:parking_lowpower", stCfgDefault.stSystem.eParkingLowPower);
	spConfig->stSystem.eParkingLowPower = (ENUM_PARKING_LOW_POWER)i;
	DEBUGMSG(ZONE_CFG_PRINT, ("Parking Low Power :			[%d]\n", i));
	
	i = cfgTPar.GetInt(p_CfgDt, "system:irled_mode", stCfgDefault.stSystem.eIrLedMode);
	spConfig->stSystem.eIrLedMode = (ENUM_IRLED_MODE)i;
	DEBUGMSG(ZONE_CFG_PRINT, ("IR LED Mode: 			[%d]\n", i));



// 5. Time
	DEBUGMSG(ZONE_CFG_PRINT, ("\n***********	Time  ***********\n"));
	i = cfgTPar.GetInt(p_CfgDt, "time:year", stCfgDefault.stTime.nYear);
	spConfig->stTime.nYear = i;
	DEBUGMSG(ZONE_CFG_PRINT, ("Year:					[%d]\n", i));
	
	i = cfgTPar.GetInt(p_CfgDt, "time:month", stCfgDefault.stTime.nMonth);
	spConfig->stTime.nMonth = i;
	DEBUGMSG(ZONE_CFG_PRINT, ("Month:					[%d]\n", i));
	
	i = cfgTPar.GetInt(p_CfgDt, "time:date", stCfgDefault.stTime.nDate);
	spConfig->stTime.nDate = i;
	DEBUGMSG(ZONE_CFG_PRINT, ("Date:					[%d]\n", i));
	
	i = cfgTPar.GetInt(p_CfgDt, "time:hour",stCfgDefault.stTime.nHour);
	spConfig->stTime.nHour = i;
	DEBUGMSG(ZONE_CFG_PRINT, ("Hour:					[%d]\n", i));
	
	i = cfgTPar.GetInt(p_CfgDt, "time:minute",stCfgDefault.stTime.nMinute);
	spConfig->stTime.nMinute = i;
	DEBUGMSG(ZONE_CFG_PRINT, ("Minute:					[%d]\n", i));
	
	i = cfgTPar.GetInt(p_CfgDt, "time:sec",stCfgDefault.stTime.nSec);
	spConfig->stTime.nSec = i;
	DEBUGMSG(ZONE_CFG_PRINT, ("Sec: 					[%d]\n", i));

	i = cfgTPar.GetInt(p_CfgDt, "time:time_set",stCfgDefault.stTime.nTimeSet);
	spConfig->stTime.nTimeSet = i;
	DEBUGMSG(ZONE_CFG_PRINT, ("Time Set:					[%d]\n", i));


	cfgTPar.DelDict(p_CfgDt);
	return TRUE;

}

BOOL CConfigText::CfgDefaultSet(LPST_CFG_DAVIEW pCfg)
{
	strcpy(pCfg->strName, "");
	strcpy(pCfg->strFWVersion, "");
	//pCfg->iGsensorSensi = 5;
	pCfg->iGsensorSensi = 2;     // jun 161115 5->3으로 변경   jun 190926  3->2로 변경  jun 20201125 2->3으로 변경 jun 20210216 3->2로 변
	pCfg->iGmt = 9;
	pCfg->iAudioRecEnable = 1;
	pCfg->iSpeakerVol = 4;        //jun 20210216   5->4로 변경
	pCfg->iEngineCylinders = 4;  // jun 20201202
	pCfg->iVideoQuality = 2; 	// HIGH, MIDDLE, LOW 
	pCfg->iEventMode = 1;		// A, B, C
	pCfg->bAutoPmEnable = 0;
	pCfg->iAutoPmEnterTime = 10;
	pCfg->iPmImpactSensi = 1;
	pCfg->bPmMotionEnable = 1;
	pCfg->iPmMotionSensi = 3;
	pCfg->iPmMotionSensi2 = 3;
	pCfg->iCarBatVoltage = 12;	//12V, 24V
	pCfg->iCarBatVoltageCalib = 0;
	pCfg->bCarBatSafeEnable = 1;
	pCfg->iCarBatSafeVoltage = 118;
	pCfg->bTempSafeEnable = 1;
	pCfg->iTempSafeValue = 90;
	pCfg->iTempStableValue = 60;
	pCfg->bSecurityLEDEnable = 1;
	pCfg->iSecurityLEDPeriod = 500;
	pCfg->bSecurityLEDMDWarning = 1;
	pCfg->iSecurityLEDOperating = 2;
	pCfg->iLcdOffTime = 30;
	pCfg->iVideoOut = 0;	//TV, LCD
	pCfg->bFactoryReset = 0;
	pCfg->iPulseReset = 1;
	pCfg->iPulseTacho1LowHigh = 0;
	pCfg->iPulseTacho4LowHigh = 0;
	pCfg->iPulseTacho5LowHigh = 0;
	pCfg->iOsdSpeed = 0;         // jun 200813
	strcpy(pCfg->strCarNumber, "0000");   // jun 200805
	wcscpy(pCfg->strDriverCode, TEXT(""));   // jun 221017

	CString time_string = make_time_string(time(0));
	ClUtil::STRING::convertWchar2Char(pCfg->strLastSetupTime, time_string.GetBuffer(),  time_string.GetLength()+1);
	return TRUE;
}

BOOL CConfigText::CfgDefaultSet(LPST_CFG_ITEM pCfg)
{
	int ch;
	// System Info
	pCfg->stSysInfo.nPcUpdate								= 1;
	sprintf(pCfg->stSysInfo.cFwVersion, "-");
	sprintf(pCfg->stSysInfo.cViewerVersion, VS_SW_VERSION);

	pCfg->stSysInfo.nMaxChannel							=  CONFIG_MAX_CAMERA;
	pCfg->stSysInfo.nMaxResolution							=  CONFIG_MAX_RESOLUTION;
	
	//Display
	pCfg->stDisplay.eLanguage								= eLANG_KOREAN;	//eLANG_ENGLISH;
	pCfg->stDisplay.nTimeZone								= 9;	//+9 Korea
	pCfg->stDisplay.eUnitOfSpeed							= UNIT_KM_P_H;
	pCfg->stDisplay.eDispMode								= DISPMODE_DUAL1;	//DISPMODE_CH1;
	pCfg->stDisplay.eVideoType							= VIDEOTYPE_NTSC;
		
	//Record
	pCfg->stRecord.eDriving2Ch							= RECORD_DRIVING_2CH_DISABLE;
#if (defined(BUILD_STREETLIGHT))
	pCfg->stRecord.eChannel 								= RECORD_1CH;
#else
	pCfg->stRecord.eChannel								= (ENUM_RECORD_CHANNEL)CONFIG_MAX_CAMERA;
#endif

#if (defined(BUILD_STREETLIGHT) || defined(BUILD_ISIN))
	pCfg->stRecord.eMode									= RECORD_EVENT;
#else
	pCfg->stRecord.eMode									= RECORD_CONTINUOUS;
#endif
	pCfg->stRecord.eCodecType								= VIDEO_CODEC_TYPE_H264;

	for( ch=0; ch < CONFIG_MAX_CAMERA; ch++ )
	{
		pCfg->stRecord.eaResolution[ch]						= VIDEO_RESOL_720P;
		pCfg->stRecord.eaQuality[ch]						= VIDEO_QUALITY_HIGH; // VIDEO_QUALITY_NORMAL
	}

	pCfg->stRecord.bAudioRecord								= ON;
	pCfg->stRecord.eRecordTime								= REC_TIME1;
	pCfg->stRecord.eEventProportion							= EVENT_PROPORTION_1P3;

	//Device
#if (BUILD_MODEL == MODEL_KIC_M7 || BUILD_MODEL == MODEL_ICARO_I300HD || BUILD_MODEL == MODEL_DOOCO_M7 || BUILD_MODEL == MODEL_EYE_TRACKS)
	pCfg->stDevice.dShockSensitive							= 0.6;	// 0.3 ==> 0.6 // kic 요청
#else
	pCfg->stDevice.dShockSensitive							= 0.3;	// 0.4 ==> 0.3
#endif
	for( ch=0; ch < CONFIG_MAX_CAMERA; ch++ )
	{
		pCfg->stDevice.staMotionSet[ch].bMode				= ON;
		pCfg->stDevice.staMotionSet[ch].eSensitive			= SENSITIVE_NORMAL;
		pCfg->stDevice.staMotionSet[ch].eBlockProportion	= MD_BLOCK_FULL;

		pCfg->stDevice.staCameraSet[ch].eVerticalFlip 		= VERITCAL_FLIP_OFF;
		pCfg->stDevice.staCameraSet[ch].bHorizontalMirror 	= OFF;
		pCfg->stDevice.staCameraSet[ch].ulBrightness 		= 0;
	}

#if (defined(BUILD_STREETLIGHT))
	pCfg->stDevice.eUsbMode								= USB_MODE1;
#else
	pCfg->stDevice.eUsbMode								= USB_MODE2;
#endif
	pCfg->stDevice.eRecordAlarm							= DEFAULT_RECORD_ALARM;

#if (BUILD_MODEL == MODEL_KIC_M7 || BUILD_MODEL == MODEL_ICARO_I300HD || BUILD_MODEL == MODEL_DOOCO_M7 || BUILD_MODEL == MODEL_EYE_TRACKS)
	pCfg->stDevice.ulSpeakerVolume						= 4;
#else
	pCfg->stDevice.ulSpeakerVolume						= 3;
#endif

#if (BUILD_MODEL == MODEL_KIC_M7 || BUILD_MODEL == MODEL_ICARO_I300HD || BUILD_MODEL == MODEL_DOOCO_M7 || BUILD_MODEL == MODEL_EYE_TRACKS)
	pCfg->stDevice.eParkingRecordAlarm						= PARKING_RECORD_ALARM_OFF;
#else
	pCfg->stDevice.eParkingRecordAlarm						= PARKING_RECORD_ALARM_SHOCK;
#endif

	//System
	pCfg->stSystem.eBuzzerMode								= DEFAULT_BUZZER_MODE;
 #if (BUILD_MODEL == MODEL_HONDA_HD_2U || BUILD_MODEL == MODEL_RALLY_EYES || (defined(BUILD_STREETLIGHT)) || defined(BUILD_ISIN))	
 	pCfg->stSystem.eLowVoltageMode							= LOW_VOLTAGE_MODE_VOLTAGE;
 	pCfg->stSystem.fLowVoltage								= LOW_VOLTAGE_MIN_LEVEL;
 #else
	pCfg->stSystem.eLowVoltageMode							= DEFAULT_LOW_VOLTAGE_MODE;
 	pCfg->stSystem.fLowVoltage								= DEFAULT_LOW_VOLTAGE_DETECTED;
 #endif


#if (BUILD_MODEL == MODEL_HONDA_HD_2U || BUILD_MODEL == MODEL_RALLY_EYES || (defined(BUILD_STREETLIGHT)) || defined(BUILD_ISIN))
	pCfg->stSystem.eParkingMode								= PARKING_OFF;
#else
	pCfg->stSystem.eParkingMode								= PARKING_AUTO;
#endif

	pCfg->stSystem.ulParkingTime								= DEFAULT_PARKING_TIME;
	pCfg->stSystem.eParkingUsb								= DEFAULT_PARKING_USB;
	pCfg->stSystem.fParkingUsbOffVolt							= DEFAULT_PARKING_USB_OFF_VOLT;
#if (BUILD_MODEL == MODEL_BW_2000 || BUILD_MODEL == MODEL_KIC_M7 || BUILD_MODEL == MODEL_ICARO_I300HD || BUILD_MODEL == MODEL_DOOCO_M7 || BUILD_MODEL == MODEL_EYE_TRACKS)
	pCfg->stSystem.eParkingLowPower							= PARKING_LOW_POWER_OFF;
#else
	pCfg->stSystem.eParkingLowPower							= DEFAULT_PARKING_LOW_POWER;
#endif
	pCfg->stSystem.eIrLedMode								= IRLED_AUTO;

	//Time
	pCfg->stTime.nYear										= 2016;
	pCfg->stTime.nMonth										= 5;
	pCfg->stTime.nDate										= 7;
	pCfg->stTime.nHour										= 0;
	pCfg->stTime.nMinute										= 0;
	pCfg->stTime.nSec										= 0;
	pCfg->stTime.nTimeSet									= 0;

	return TRUE;
}

CString CConfigText::make_time_string(time_t t)
{
	CString csTime;
	struct tm *tm_t;
	tm_t = localtime(&t);
	csTime.Format(_T("%4d-%02d-%02d %02d:%02d:%02d"), tm_t->tm_year + 1900, tm_t->tm_mon + 1, tm_t->tm_mday, tm_t->tm_hour, tm_t->tm_min, tm_t->tm_sec);

	return csTime;
}

