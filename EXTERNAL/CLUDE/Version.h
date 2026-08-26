
#pragma once

/**************************************************************************************************
 *
 *      Define Version of ICARO BB HD2
 *
 *************************************************************************************************/

#define MODEL_DAVIEW_2CH		0
#define MODEL_DAVIEW_2CH_L		1	// jun 170405
#define MODEL_DAVIEW_2CH_F		2	// jun 170406
#define MODEL_DAVIEW_2CH_S		3	// jun 171018
#define MODEL_NH_200A			4	
#define MODEL_BW_2000			5
#define MODEL_Q_BLACK			6
#define MODEL_KIC_M7			7
#define MODEL_HONDA_HD_2U		8
#define MODEL_SAMPLE			9
#define MODEL_ITSG_HD_2U		10
#define MODEL_TMPD_HD_2U		11	// 경시청 경찰차
#define MODEL_ICARO_I300HD		12
#define MODEL_VIEW_I_2U		    13
#define MODEL_DOOCO_M7		    14
#define MODEL_RALLY_EYES	    15
#define MODEL_EYE_TRACKS		16

#define LANGUAGE_ENGLISH	0
#define LANGUAGE_KOREAN	1
#define LANGUAGE_JAPANESE	2


#define BUILD_MODEL 		MODEL_DAVIEW_2CH
//#define BUILD_MODEL 		MODEL_DAVIEW_2CH_S      // Firstview Secret Version    jun 171018
//#define BUILD_MODEL 		MODEL_DAVIEW_2CH_F     // Firstview Front Only    jun 170406
//#define BUILD_MODEL 		MODEL_DAVIEW_2CH_L	    // Firstview Line Camera   jun 170405
//#define BUILD_MODEL 			MODEL_NH_200A
//#define BUILD_MODEL 			MODEL_BW_2000	//mobidic
//#define BUILD_MODEL			MODEL_Q_BLACK	//qdis
//#define BUILD_MODEL			MODEL_KIC_M7
//#define BUILD_MODEL			MODEL_HONDA_HD_2U
//#define BUILD_MODEL 			MODEL_SAMPLE
//#define BUILD_MODEL			MODEL_ITSG_HD_2U
//#define BUILD_MODEL			MODEL_TMPD_HD_2U
//#define BUILD_MODEL			MODEL_ICARO_I300HD
//#define BUILD_MODEL			MODEL_VIEW_I_2U
//#define BUILD_MODEL			MODEL_DOOCO_M7
//#define BUILD_MODEL			MODEL_RALLY_EYES	//rally eyes
//#define BUILD_MODEL			MODEL_EYE_TRACKS	//TNA


#if (BUILD_MODEL == MODEL_DAVIEW_2CH)
//	#define BUILD_LANGUAGE		LANGUAGE_KOREAN
//	#define BUILD_LANGUAGE		LANGUAGE_ENGLISH
	#define BUILD_LANGUAGE		LANGUAGE_JAPANESE

	//#define FORMAT_LABEL					TEXT("LUCYNXFS")
	#define FORMAT_LABEL					TEXT("SD")
 #if (BUILD_LANGUAGE== LANGUAGE_KOREAN)	
	#define APPLICATION_XML_NAME			"Daview.xml"
 #else
 	#define APPLICATION_XML_NAME			"Daview_eng.xml"
 #endif


 //#if (BUILD_LANGUAGE == LANGUAGE_JAPANESE)
 	#define LOW_VOLTAGE_9_0_USE
 //#endif
 //	#define BUILD_STREETLIGHT			//가로등 블랙박스
 	#define BUILD_STREETLIGHT_JAPAN	//
 	#define BUILD_ISIN					//Ext 트리거를 사용하는 아이신용 블랙박스 
 //	#define TIME_SEARCH_USE
 	#define SPEED_DIGIT_PONT_USE		//
//	#define BUILD_NO_LOGO
//	#define AUDIO_VOLUME_CONTROL_USE

//	#define BUILD_REPORT_SPEED      //20171226 Add Report Speed 
//   #define CARNUMBER      // jun 200812 CARNUMBER Delete
   #define DRIVERCODE      // jun 221017 DRIVERCODE Delete



//	#define BUILD_PASSWORD_USE		//20170829
//	#define REAR_CAM_OFF                 // 1ch View Only, 1ch Backup Only
//	#define MAP_OFF                 // 일본 keio                 jun 190710 MAP창과 버튼 미사용
//	#define BACKUP_OFF           // jun 181217 백업 미사용
//	#define CAPTURE_OFF         // jun 20201104
//	#define PRINT_OFF             // jun 20201104
//	#define ABOUT_OFF            // jun 181217 업그레이드 미사용
   #define DEF_AVI_FILE_MERGE   // jun 20201125 백업시 1개파일로 머지 





#elif (BUILD_MODEL == MODEL_DAVIEW_2CH_S)  // jun 171018
//	#define BUILD_LANGUAGE		LANGUAGE_KOREAN
//	#define BUILD_LANGUAGE		LANGUAGE_ENGLISH
	#define BUILD_LANGUAGE		LANGUAGE_JAPANESE
	
	//#define FORMAT_LABEL					TEXT("LUCYNXFS")
	#define FORMAT_LABEL					TEXT("SD")
 #if (BUILD_LANGUAGE== LANGUAGE_KOREAN)	
	#define APPLICATION_XML_NAME			"Daview.xml"
 #else
 	#define APPLICATION_XML_NAME			"Daview_eng.xml"
 #endif

 
 	#define LOW_VOLTAGE_9_0_USE
 	#define BUILD_STREETLIGHT_JAPAN	//
 	#define BUILD_ISIN					//Ext 트리거를 사용하는 아이신용 블랙박스 
 	#define SPEED_DIGIT_PONT_USE		//


	#define BUILD_PASSWORD_USE		//20170829
 	#define MAP_OFF                 // 일본 keio                 jun 190710 MAP창과 버튼 미사용
//	#define BACKUP_OFF           // jun 181217 백업 미사용
//	#define CAPTURE_OFF         // jun 20201104
//	#define PRINT_OFF             // jun 20201104
//   #define ABOUT_OFF            // jun 181217 업그레이드 미사용
//   #define DEF_AVI_FILE_MERGE   // jun 20201125 백업시 1개파일로 머지        



#elif (BUILD_MODEL == MODEL_DAVIEW_2CH_F)	// jun 170406
//	#define BUILD_LANGUAGE		LANGUAGE_KOREAN
//	#define BUILD_LANGUAGE		LANGUAGE_ENGLISH
	#define BUILD_LANGUAGE		LANGUAGE_JAPANESE
	
	//#define FORMAT_LABEL					TEXT("LUCYNXFS")
	#define FORMAT_LABEL					TEXT("SD")
 #if (BUILD_LANGUAGE== LANGUAGE_KOREAN)	
	#define APPLICATION_XML_NAME			"Daview.xml"
 #else
 	#define APPLICATION_XML_NAME			"Daview_eng.xml"
 #endif

 
 	#define LOW_VOLTAGE_9_0_USE
 	#define BUILD_STREETLIGHT_JAPAN	//
 	#define BUILD_ISIN					//Ext 트리거를 사용하는 아이신용 블랙박스 
 	#define SPEED_DIGIT_PONT_USE		//
 	

      #define REAR_CAM_OFF
 	#define MAP_OFF                 // jun 190710 MAP창과 버튼 미사용
// 	#define BACKUP_OFF           // jun 181217 백업 미사용
//	#define CAPTURE_OFF         // jun 20201104
//	#define PRINT_OFF             // jun 20201104
 	#define ABOUT_OFF            // jun 181217 업그레이드 미사용
//   #define DEF_AVI_FILE_MERGE   // jun 20201125 백업시 1개파일로 머지 



#elif (BUILD_MODEL == MODEL_DAVIEW_2CH_L)	// jun 170405
//	#define BUILD_LANGUAGE		LANGUAGE_KOREAN
//	#define BUILD_LANGUAGE		LANGUAGE_ENGLISH
	#define BUILD_LANGUAGE		LANGUAGE_JAPANESE
	
	//#define FORMAT_LABEL					TEXT("LUCYNXFS")
	#define FORMAT_LABEL					TEXT("SD")
 #if (BUILD_LANGUAGE== LANGUAGE_KOREAN)	
	#define APPLICATION_XML_NAME			"Daview.xml"
 #else
 	#define APPLICATION_XML_NAME			"Daview_eng.xml"
 #endif

 
 	#define LOW_VOLTAGE_9_0_USE
 	#define BUILD_STREETLIGHT_JAPAN	//
 	#define BUILD_ISIN					//Ext 트리거를 사용하는 아이신용 블랙박스 
 	#define SPEED_DIGIT_PONT_USE		//
 	
	
 	#define ABOUT_OFF            // jun 181217 업그레이드 미사용
//   #define DEF_AVI_FILE_MERGE   // jun 20201125 백업시 1개파일로 머지 



#elif (BUILD_MODEL == MODEL_NH_200A)
	#define BUILD_LANGUAGE		LANGUAGE_KOREAN
//	#define BUILD_LANGUAGE		LANGUAGE_ENGLISH
	#define FORMAT_LABEL					TEXT("NIGHTHAWK")
	#define APPLICATION_XML_NAME			"Nighthawk.xml"
#elif (BUILD_MODEL == MODEL_BW_2000)
//	#define BUILD_LANGUAGE		LANGUAGE_KOREAN
//	#define BUILD_LANGUAGE		LANGUAGE_JAPANESE
	#define BUILD_LANGUAGE		LANGUAGE_ENGLISH
	#define FORMAT_LABEL					TEXT("BLACKWING")
	#define APPLICATION_XML_NAME			"Blackwing.xml"
#elif (BUILD_MODEL == MODEL_Q_BLACK)
	//#define BUILD_LANGUAGE		LANGUAGE_KOREAN
	#define BUILD_LANGUAGE		LANGUAGE_JAPANESE
	#define FORMAT_LABEL					TEXT("Q-BLACK")
	#define APPLICATION_XML_NAME			"QBlack.xml"
#elif (BUILD_MODEL == MODEL_KIC_M7)
	#define BUILD_LANGUAGE 	LANGUAGE_KOREAN
	#define FORMAT_LABEL					TEXT("INFOBOX")
	#define APPLICATION_XML_NAME			"Infobox.xml"
	#define LOW_VOLTAGE_9_0_USE
#elif(BUILD_MODEL == MODEL_HONDA_HD_2U)
	#define BUILD_LANGUAGE 				LANGUAGE_JAPANESE
	#define FORMAT_LABEL					TEXT("HONDA-2U")
	#define APPLICATION_XML_NAME			"Honda_hd_2u.xml"
	#define LOW_VOLTAGE_9_0_USE
	#define TIME_SEARCH_USE
#elif(BUILD_MODEL == MODEL_SAMPLE)
	#define FORMAT_LABEL					TEXT("SAMPLE")
	#define BUILD_LANGUAGE					LANGUAGE_ENGLISH
	#define APPLICATION_XML_NAME			"Sample.xml"
#elif(BUILD_MODEL == MODEL_ITSG_HD_2U)
	#define FORMAT_LABEL					TEXT("ITSG-2U")
	#define BUILD_LANGUAGE					LANGUAGE_JAPANESE
	#define APPLICATION_XML_NAME			"itsg_hd_2u.xml"
	#define LOW_VOLTAGE_9_0_USE
	#define TIME_SEARCH_USE
	#define RDF_USE // 주행정보 표시
#elif(BUILD_MODEL == MODEL_TMPD_HD_2U)
	#define BUILD_LANGUAGE 				LANGUAGE_JAPANESE
	#define FORMAT_LABEL					TEXT("TMPD-2U")
	#define APPLICATION_XML_NAME			"Tmpd_hd_2u.xml"
	#define LOW_VOLTAGE_9_0_USE
	#define TIME_SEARCH_USE
#elif(BUILD_MODEL == MODEL_ICARO_I300HD)
//	#define BUILD_LANGUAGE 				LANGUAGE_JAPANESE
	#define BUILD_LANGUAGE 				LANGUAGE_ENGLISH
	#define FORMAT_LABEL					TEXT("I300HD")
	#define APPLICATION_XML_NAME			"Icaro_i300hd.xml"
	#define LOW_VOLTAGE_9_0_USE
	#define TIME_SEARCH_USE
	#define BUILD_NO_LOGO
#elif (BUILD_MODEL == MODEL_VIEW_I_2U)
	#define BUILD_LANGUAGE 				LANGUAGE_JAPANESE
//	#define BUILD_LANGUAGE 				LANGUAGE_ENGLISH
	#define FORMAT_LABEL					TEXT("VIEW-i")
	#define APPLICATION_XML_NAME			"view_i_2u.xml"
	#define LOW_VOLTAGE_9_0_USE
	#define TIME_SEARCH_USE
#elif (BUILD_MODEL == MODEL_DOOCO_M7)
	#define BUILD_LANGUAGE 	LANGUAGE_KOREAN
	#define FORMAT_LABEL					TEXT("INFOBOX")
	#define APPLICATION_XML_NAME			"InfoboxDooco.xml"
	#define LOW_VOLTAGE_9_0_USE	
#elif (BUILD_MODEL == MODEL_RALLY_EYES)
	#define BUILD_LANGUAGE 	LANGUAGE_JAPANESE
	#define FORMAT_LABEL					TEXT("HONDA-2U") //TEXT("DAKAR")
	#define APPLICATION_XML_NAME			"RallyEyes.xml"
	#define LOW_VOLTAGE_9_0_USE
	#define SPEED_DIGIT_PONT_USE
#elif(BUILD_MODEL == MODEL_EYE_TRACKS)
	#define BUILD_LANGUAGE 				LANGUAGE_KOREAN
//	#define BUILD_LANGUAGE 				LANGUAGE_JAPANESE
//	#define BUILD_LANGUAGE 				LANGUAGE_ENGLISH
	#define FORMAT_LABEL					TEXT("I300HD")
	#define APPLICATION_XML_NAME			"eyetracks_i300hd.xml"
	#define LOW_VOLTAGE_9_0_USE
	#define TIME_SEARCH_USE
#endif

#if BUILD_MODEL == MODEL_DAVIEW_2CH || BUILD_MODEL == MODEL_HONDA_HD_2U
//	#define 	_SW_VERSION				"FIRSTVIEW_PC_0001"
//	#define 	_SW_VERSION				"FIRSTVIEW_V1.1.0"
//	#define 	_SW_VERSION				"V1.0.0"
//  #define 	_SW_VERSION				"V1.1.0"
//	#define 	_SW_VERSION				"DA210_V2.0.0"           //170705 V2HD Start
//	#define 	_SW_VERSION				"DA210_V2.1.0"           //170829 Google Script Version Change
//	#define 	_SW_VERSION				"DA210_V2.2.0"           //170831 server version Down Error 수정  // 171018 Add Secret Version
//	#define 	_SW_VERSION				"DA210_V2.2.1"           //171212 Add Report Speed
//	#define 	_SW_VERSION				"DA210_V2.2.2"           //171226 Google version V1 dowm 
//	#define 	_SW_VERSION				"DA210_V2.2.5"           // jun 180212 미러수정
//	#define 	_SW_VERSION				"DA210_V2.2.6"           // jun 181019 OSM맵 변경
//	#define 	_SW_VERSION				"DA210_V2.2.7"           // jun 190930 GMT에러수정,G센서=2
//	#define 	_SW_VERSION				"DA210_V2.3.0"           // 멀티체크
//	#define 	_SW_VERSION				"DA210_V2.3.1"           // jun 200805 Main time(Hour,minute,second) delete, CarNumber add
//	#define 	_SW_VERSION				"DA210_V2.3.2"           // jun 200813 SPEED OSD add
//	#define 	_SW_VERSION				"DA210_V2.4.0"           // jun 201006 패치수정,서버변경
//	#define 	_SW_VERSION				"DA210_V2.4.1"           // jun 201021 이벤트항목추가, 실행시 Sort가능 
//	#define 	_SW_VERSION				"DA210_V2.4.2"           // jun 201028 fvfs추가,RPM추가,백업여러파일1개로,ffmpeg업데이트,4채널뷰어추가 
//	#define 	_SW_VERSION				"DA210_V2.4.3"           // jun 201126 DLL추가, Patch변경, 인스톨러삭제, 레지스트리 2HD로 변경 
//	#define 	_SW_VERSION				"DA210_V2.4.4"           // jun 201202 RPM 셋팅추가 
//	#define 	_SW_VERSION				"DA210_V2.4.5"           // jun 210104 fvfs 수정(재생시 40초NG)
//	#define 	_SW_VERSION				"DA210_V2.4.6"           // jun 210108 fvfs 추가 수정 (재생시 40초 개선)  
//	#define 	_SW_VERSION				"DA210_V2.4.7"           // jun 210114 보안관련 코드수정   
//	#define 	_SW_VERSION				"DA210_V2.4.8"           // jun 210128 Network 코드추가    
//	#define 	_SW_VERSION				"DA210_V2.4.9"           // jun 210203 Nxfs Setup Read Error  , Setting Default 변경 
//	#define 	_SW_VERSION				"DA210_V2.4.10"           // jun 210224 coreaac,launcher,Lavfilter 응용폴더로 이동 
//	#define 	_SW_VERSION				"DA210_V2.4.11"           // jun 210226 DLL3종 삭제 
//	#define 	_SW_VERSION				"DA210_V2.4.12"           // jun 210416 게이오 폴더 open시 SSD검색삭제, toto자동차 네트워크 위치 안보임 문제로 레지스트리 추가
//	#define 	_SW_VERSION				"DA210_V2.4.13"           // jun 211019 마지막파일 G센서 오류 수정 
//	#define 	_SW_VERSION				"DA210_V2.4.14"           // jun 211020 G-Sensor그래프에 Speed추가 
//	#define 	_SW_VERSION				"DA210_V2.4.15"           // jun 220111 Open Layer Map 버전 v5.2.0 ->v6.1.1 
//	#define 	_SW_VERSION				"DA210_V2.4.16"           // jun 220112 map const -> bar로 변경 
//	#define 	_SW_VERSION				"DA210_V2.4.17"           // jun 220112 ffmpeg 32bit 4.3.1로 변경 
//	#define 	_SW_VERSION				"DA210_V2.4.18"           // jun 220121 Nxfs와 Fvfs에 따라 ffmpeg버전을 나눔
//	#define 	_SW_VERSION				"DA210_V2.4.19"           // jun 220629 ol map 스크립트 에러로 map html 수정 
//	#define 	_SW_VERSION				"DA210_V2.4.20"           // jun 220824 뷰어실행시 GPS 속도 0값으로 변경시 펄스속도  순간적으로 0값 디스플레이되는 오류 수정
//	#define 	_SW_VERSION				"DA210_V2.4.21"           // jun 221017 셋업 운전자정보 추가
//	#define 	_SW_VERSION				"DA210_V2.4.22"           // jun 221110 셋업 NAME,FWVersion의 id,value값 위치 변경시 에러 수정 
//	#define 	_SW_VERSION				"DA210_V2.4.23"           // jun 241007 이벤트 파일 색상 및 Sort 수정 
//	#define 	_SW_VERSION				"DA210_V2.4.24"           // jun 260507 Map 스프립트 에러 수정




//	#define 	_SW_VERSION				"DA210_V2.4.25"
//	#define 	_SW_VERSION				"DA210S_V2.4.25"    // toto 자동차 
//	#define 	_SW_VERSION				"DA210F_V2.4.25"    // 일본 택시조합  
//	#define 	_SW_VERSION				"DA210T_V2.4.25"    // 도쿄 전력   
//	#define 	_SW_VERSION				"DA300_V2.4.25"      // pai-r
	#define 	_SW_VERSION				"DA300E_V2.4.25"    // Entetsu
//	#define 	_SW_VERSION				"DA300T_V2.4.25"    // 도쿄 전력 
//	#define 	_SW_VERSION				"DA300K1_V2.4.25"   //keio 본사용 
//	#define 	_SW_VERSION				"DA300K2_V2.4.25"   //keio 영업용 Front Only




#elif BUILD_MODEL == MODEL_DAVIEW_2CH_S	                               // jun 171018
//	#define 	_SW_VERSION				"DA210_V2.2.0_S"       //171018 Add Secret Version
//  #define 	_SW_VERSION				"DA210_V2.2.5_S"       //180626 미러수정, 백업키 삭제
//	#define 	_SW_VERSION				"DA210_V2.2.6_S"       // jun 181024 OSM맵 변경
//	#define 	_SW_VERSION				"DA210_V2.2.7_S"       // jun 190930 GMT에러수정,G센서=2
//	#define 	_SW_VERSION				"DA210_V2.3.0_S"       // 멀티체크
//	#define 	_SW_VERSION				"DA210S_V2.4.0"         // jun 201006 패치수정,서버변경
//	#define 	_SW_VERSION				"DA210S_V2.4.1"         // jun 201021 이벤트항목추가, 실행시 Sort가능 
//	#define 	_SW_VERSION				"DA210S_V2.4.2"         // jun 201028 fvfs추가,RPM추가,백업여러파일1개로,ffmpeg업데이트,4채널뷰어추가  
	#define 	_SW_VERSION				"DA210S_V2.4.3"         // jun 201126 DLL추가, Patch변경, 인스톨러삭제, 레지스트리 2HD로 변경 


	
#elif BUILD_MODEL == MODEL_DAVIEW_2CH_F	                           // jun 170406
//	#define 	_SW_VERSION				"DA210_V2.0.0_F"
//	#define 	_SW_VERSION				"DA210_V2.1.0_F" 
//	#define 	_SW_VERSION				"DA210_V2.2.0_F"  	
//	#define 	_SW_VERSION				"DA210_V2.2.1_F"  
//	#define 	_SW_VERSION				"DA210_V2.2.2_F"  
//	#define 	_SW_VERSION				"DA210_V2.2.3_F"        // jun 180205 백업시 원본파일 다운
//	#define 	_SW_VERSION				"DA210_V2.2.4_F"        // jun 180212 미러수정
//	#define 	_SW_VERSION				"DA210_V2.2.5_F"        // jun 180219 미러수정, 백업시 원본파일 삭제
//	#define 	_SW_VERSION				"DA210_V2.2.6_F"        // jun 181024 OSM맵 변경
//	#define 	_SW_VERSION				"DA210_V2.2.7_F"        // jun 190930 GMT에러수정,G센서=2
//	#define 	_SW_VERSION				"DA210_V2.3.0_F"        // 멀티체크 
//	#define 	_SW_VERSION				"DA210F_V2.4.0"          // jun 201006 패치수정,서버변경
//	#define 	_SW_VERSION				"DA210F_V2.4.1"          // jun 201021 이벤트항목추가, 실행시 Sort가능 
//	#define 	_SW_VERSION				"DA210F_V2.4.2"          // jun 201028 fvfs추가,RPM추가,백업여러파일1개로,ffmpeg업데이트,4채널뷰어추가 
	#define 	_SW_VERSION				"DA210F_V2.4.3"          // jun 201126 DLL추가, Patch변경, 인스톨러삭제, 레지스트리 2HD로 변경 
	

#elif BUILD_MODEL == MODEL_DAVIEW_2CH_L	                               // jun 170405
//	#define 	_SW_VERSION				"DA210_V2.0.0_L"
//	#define 	_SW_VERSION				"DA210_V2.1.0_L" 
//	#define 	_SW_VERSION				"DA210_V2.2.0_L" 
//	#define 	_SW_VERSION				"DA210_V2.2.1_L" 
//  #define 	_SW_VERSION				"DA210_V2.2.5_L"        // jun 180212 미러수정
//	#define 	_SW_VERSION				"DA210_V2.2.6_L"        // jun 181024 OSM맵 변경
//	#define 	_SW_VERSION				"DA210_V2.2.7_L"        // jun 190930 GMT에러수정,G센서=2
//	#define 	_SW_VERSION				"DA210_V2.3.0_L"        // 멀티체크
//	#define 	_SW_VERSION				"DA210L_V2.4.0"          // jun 201006 패치수정,서버변경
//	#define 	_SW_VERSION				"DA210L_V2.4.1"          // jun 201021 이벤트항목추가, 실행시 Sort가능 
//	#define 	_SW_VERSION				"DA210L_V2.4.2"          // jun 201028 fvfs추가,RPM추가,백업여러파일1개로,ffmpeg업데이트,4채널뷰어추가 
	#define 	_SW_VERSION				"DA210L_V2.4.3"           // jun 201126 DLL추가, Patch변경, 인스톨러삭제, 레지스트리 2HD로 변경 



#elif BUILD_MODEL == MODEL_NH_200A
	#define 	_SW_VERSION				"NH-200A_PC_0008"
#elif BUILD_MODEL == MODEL_BW_2000
	#define 	_SW_VERSION				"BW-2000_PC_0008"
#elif BUILD_MODEL == MODEL_Q_BLACK
	#define 	_SW_VERSION				"Q-BLACK_PC_0008"
#elif (BUILD_MODEL == MODEL_KIC_M7 || BUILD_MODEL == MODEL_DOOCO_M7)
	#define 	_SW_VERSION				"M7_PC_0011"
#elif BUILD_MODEL == MODEL_SAMPLE
	#define 	_SW_VERSION				"SAMPLE_PC_0008"
#elif BUILD_MODEL == MODEL_ITSG_HD_2U
	#define 	_SW_VERSION				"ITSG_PC_0010"
#elif BUILD_MODEL == MODEL_TMPD_HD_2U
	#define 	_SW_VERSION				"TMPD_PC_0010"
#elif BUILD_MODEL == MODEL_ICARO_I300HD
	#define 	_SW_VERSION				"I300HD_PC_0010"
#elif BUILD_MODEL == MODEL_VIEW_I_2U
	#define 	_SW_VERSION				"VIEW_i_2U_PC_0010"
#elif BUILD_MODEL == MODEL_RALLY_EYES
	#define 	_SW_VERSION				"RALLY_EYES_PC_0010"
#elif BUILD_MODEL == MODEL_EYE_TRACKS
	#define 	_SW_VERSION				"EYE_TRACKS_PC_0011"
#else
	<< ERRROR! MODEL type >>
#endif

#define VS_MAKE_TIME 			"2016.05.08"
#define VS_SW_VERSION 			_SW_VERSION


#define DEFAULT_PASSWORD		_T("00000")// 사용안함 레지스트리에서 변경
#define DEFAULT_MASTERKEY		_T("181010")

/*******************  ICARO BB HD2 Revision Note                     ******************************

1	revision : DAVIEW_PC_0001 20160410  ==> revision : DAVIEW_PC_0002 
	1)




**************************************************************************************/
