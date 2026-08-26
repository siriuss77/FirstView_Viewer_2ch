/**************************************************************************************************
 *
 *      File Name       : RdfParser.h
 *      Description     :  주행정보 기록
 *
 *      Creator         :   tony ( icaro Co., Ltd )
 *      Create Date     :   2013/09/14
 *      Update History  :   
 *
 *************************************************************************************************/

#ifndef __RDF_PARSER_H__
#define __RDF_PARSER_H__
#include "RdfSetup.h"

///////////////////// DEFINE ////////////////////////////////////////////

///////////////////// TYPE ///////////////////////////////////////////

#define packed         //__packed
#pragma pack(push, 1)	//2012/11/01 add, do not use padding bit	

typedef enum
{
	RDF_TYPE_HEADER = 0,

	RDF_TYPE_BBOX,
	RDF_TYPE_GPS,
	RDF_TYPE_PULSE,
	RDF_TYPE_GSENSOR,

	RDF_TYPE_END
}E_RDF_TYPE;

typedef packed struct	// 20 Byte
{
	char 		header[RDF_HEADER_STRING_SIZE]; 	//헤더 (고정 문자열 (ASCII)) 
	u32			utd_time;	//기록 시작 시간 (UTC) 
	u32 		vehicle_id;
	u32			driver_id;

	char			time_zone;	//-13 ~ 12
	char			ext_signal;	//외부 차량 신호 입력의 유무 (0 = 미 연결, 1 = 연결) 
	char			reserved[2];
}ST_RDF_DATA_HEADER, *LPST_RDF_DATA_HEADER;

typedef packed struct // 16 byte
{
	u8		header[3];	// "RDF" frame start (고정 문자열 (ASCII))
	u8		status;		// 블랙박스 상태 (0 : 녹화 대기 , 1 : 상시 녹화, 2 : 모션 녹화, 3 : 충격 이벤트 녹화 , 4 : 강제녹화 , 5 : 녹화 종료)
	
	u32 	utd_time; 	//현재 시간 (UTC)

	u32		batt_volt;	// voltage x 100
	u32		etc_status;	// bit 0 : (0 :acc off, 1 : acc on), bit 1....
	
}ST_RDF_DATA_BBOX, *LPST_RDF_DATA_BBOX;
typedef packed struct	//16 Byte
{
	u8	 	sp_state;	//위성 상태 (0 = 2 = 2D, 3 = 3D) 
	u8		speed;		// 0 ~ 255
	u8		pdop;		//PDOP (× 10) 
	u8 		event;		//이벤트 정보 유무 (0 : 이벤트 없음 , 1 : 왼쪽 G의 이벤트, 2 : 오른쪽 G의 이벤트, 3 : 후 G의 이벤트 , 4 : 전 G에 의한 이벤트 , 5 : 상하 G의 이벤트, 6 : 속도에 의한 이벤트 , 7 기타

	u32		cog;		//방위 (0 ~ 360 × 100)  Course Over Ground (degree) //진북
	int		latitude;		//위도 ddmm.mmmm × 10000 (+ : 북위 - : 남위) 	-99999999 ~ 99999999 
	int		longitude;	//경도 dddmm.mmmm × 10000 (+ : 서경 - : 동경) 	-99999999 ~ 99999999  // +동경 -서경
}ST_RDF_DATA_GPS, *LPST_RDF_DATA_GPS;

typedef packed struct	// 8 Byte x 2
{
	u8		speed_pulse;	//차속 펄스 (Raw : 순간 값) 
	u8		rpm_pulse;		//엔진 회전 펄스 (Raw : 순간 값) 
	u8		reverse;		//후진 기어 (0 = OFF, 1 = ON 미 연결시 0) 
	u8		speed;			//속도 (km / h) 차속 펄스로 취득

	u16		rpm;			//엔진 회전 수 (rpm) 엔진 회전 펄스로 취득
	u16		etc;			// bit 0 ~ 15	
}ST_RDF_DATA_PULSE, *LPST_RDF_DATA_PULSE;

typedef packed struct // 6 Byte x 10
{
	short 	x;		//X 축 가속도 (1/100G) 
	short	y;		//y 축 가속도 (1/100G) 
	short 	z;		//z 축 가속도 (1/100G) 
}ST_RDF_DATA_GSENSOR, *LPST_RDF_DATA_GSENSOR;

#define ONE_SECOND_PULSE_DATA_COUNT		2
#define ONE_SECOND_G_DATA_COUNT			10
typedef packed struct
{
//	ST_RDF_DATA_HEADER 	hdr;		// 처음에만 저장 
	ST_RDF_DATA_BBOX 	bbox;
	ST_RDF_DATA_GPS		gps;
	ST_RDF_DATA_PULSE		alPulse[ONE_SECOND_PULSE_DATA_COUNT];	//0.5 초 x2 개
	ST_RDF_DATA_GSENSOR 	alG[ONE_SECOND_G_DATA_COUNT];		//0.1 초 x10 개
}ST_RDF_DATA, *LPST_RDF_DATA;

typedef struct
{
	struct tm 		tm;
	ST_RDF_DATA	data;
}ST_RDF_QUEUE_DATA, *LPST_RDF_QUEUE_DATA;

#pragma pack(pop)	//2012/11/01 add, do not use padding bit

////////////////////////////////////////////////////////////////
#define RDFDATA_COUNT_OF_DAY	(60 * 60 *24) // 1day
#define RDF_FOLDER	TEXT("RDF")

class CRdfData
{
public:
	CRdfData(struct tm *ptime = NULL) ;
	virtual ~CRdfData();

	struct tm m_time; 
	unsigned int m_rdf_data_size;
	
	ST_RDF_DATA_HEADER m_RdfHdr;
	LPST_RDF_DATA m_pRdfData;

	int GetTimezone(void);
	u32 GetDriver_id(void);
	u32 GetVehicle_id(void);
	u32 GetLocalTime(u32 time  = NULL );
	
	LPST_RDF_DATA GetRdfData(unsigned int pos);
	unsigned int CRdfData::GetLength(void);
	void ClearData(void);
	unsigned int AddRdfData(LPST_RDF_DATA pRdf);
	int Load(CString strDir, struct tm *pDate = NULL);
};

class CRdfParser  
{
public:
	typedef struct 
	{
		u32				file_index;
		CFile			file;
		CString	 		cstrFileName;

		struct tm 		tm;
	}ST_RDF_FILE, *LPST_RDF_FILE;
	
		
	CRdfParser(void);
	virtual ~CRdfParser(void);

	int CRdfParser::GetRdfDataLoad(CString strDir, struct tm *pDate, CRdfData *p_crdf_data);
	
protected:
	BOOL CRdfParser::OpenRdfFile( CString cstrDir , struct tm *pDate );

	ST_RDF_FILE m_rdf_file;
};

#endif
