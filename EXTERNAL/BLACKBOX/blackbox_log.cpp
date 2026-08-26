/**************************************************************************************************
 *
 *      File Name       :  blackbox_log.c
 *      Description     :  Blackbox log function
 *
 *      Creator         :   tony ( icaro Co., Ltd )
 *      Create Date     :   2011/08/05
 *      Update History  :   
 *
 *************************************************************************************************/

#include "stdafx.h"
#include <HELPER/CLUtil.H>
#include "BBoxUtil.h"
#include "BBPlayer.h"
#include "blackbox_log.h"

//#define LOG_TEST



/**********************************************************
 * log local definitions
 **********************************************************/
 #ifndef __func__
 #define __func__			__FUNCTION__
 #endif //__func__
 
#define 	AKLOG_ERROR			1
#define 	AKLOG_INIT			1
#define 	AKLOG_FUNC			1
#define 	AKLOG_ENTRY			0

#define		LOG_VERSION			000

#define		POS_LOG_HDR			(0)
#define		POS_LOG_START		(0) //(POS_LOG_HDR + l_spi_log.LogHdrSize) // (POS_LOG_HDR + sizeof(ST_LOG_INFOHDR))

#define LOG_DATA_FILE_SIZE						(LOG_MAX_SIZE)//( 256 * 1024)


#define AK_LOG_DATA_FILE_NAME					(L"system.log")


//romSz,    secNum, secSz,   		blkNum, 	pageSz,     man_id, 	dev_id
/*    {2*MB,      16,        4*KB,           32,         256,          0xEF,         0x14      },//w25Q16
    {4*MB,      16, 		4*KB,           64, 		256,          0xEF, 		0x15      },//w25Q32
    {8*MB,      16, 		4*KB,           128, 		256,          0xEF, 		0x16      },//w25Q64
*/
#define KB		(1024)
#define MB		(1024*1024)
 #define LOG_SECTOR_SIZE	(4*KB)
 #define LOG_PAGE_SIZE		(256)
 #define LOG_MAX_SIZE		(SYSTEMLOG_MAX_COUNT * sizeof(ST_LOG_ITEM)) // 184KB (4M SPI Flash use)
 #define SYSTEMLOG_MAX_COUNT		6400
 
/**********************************************************
 * log local structures & variables
 **********************************************************/

	typedef enum
	{
		RTC_SRC_GPS = 0,
		RTC_SRC_SETUP_SD,
		RTC_SRC_SETUP_APP,
		RTC_SRC_SETUP_SERVER,

		RTC_SRC_MAX
	} RTC_UPDATE_SOURCE;

	typedef enum tag_battery_state{
		eBB_BATT_STATE_USB_NORMAL = 0,
		eBB_BATT_STATE_USB_FALL,

		eBB_BATT_STATE_NORMAL,	// acc on
		eBB_BATT_STATE_PARKING, // acc 0ff
		eBB_BATT_STATE_LOW,
		eBB_BATT_STATE_EMER_LOW,
		eBB_BATT_STATE_FALL,

		eBB_BATT_STATE_POWER_SWTCH_OFF,
		eBB_BATT_STATE_MAX
	} eBB_BATT_STATE;

///////////////////////////////////////////////////////////
	typedef struct tagLogInformationHeader // 32 byte
	{
		u32		stsize;
		u32		ver;
		u32		maxitemcount;
		u32		itemsize;
		u32		posbegin;
		u32		posend;
		u8		reserved[4];
		u32		checksum;
	} ST_LOG_INFOHDR, * LPST_LOG_INFOHDR;

	static BOOL				l_download 	= FALSE;
	static BOOL 			l_systemstarted = FALSE;
	
	static ST_LOG_INFOHDR		l_log_ihdr 	= {0,};
	static BOOL				l_log_init 	= FALSE;


	u8				*l_cpLogBuffer = NULL;

/**********************************************************
 * log local macro & functions
 **********************************************************/
	#define		MACROLOG_ITEM_SIZE				(sizeof(ST_LOG_ITEM)) // flash k9f1208 : 512 is 1page size

	#define		MACROLOG_ISEMPTY(x)			(x.posbegin == x.maxitemcount && x.posend == x.maxitemcount)
	#define		MACROLOG_GEPOS(pos)			( POS_LOG_START + ((pos)*MACROLOG_ITEM_SIZE) )

/**********************************************************
 * log interface functions
 **********************************************************/

/**
	\addtogroup	ak_log
	\{
*/
#define ROM_LOG_HEADER_SIZE (l_spi_log.SecSz * 2) //8K

typedef struct {
	bool	init;
	u32 	SecSz;
	u32		PageSz;
	u32 	CfgMaxSize;
	u32 	LogMaxSize;
	
	u32		itemLastSavePage;
	u32		hdrLastSavePage;

	u32 	LogHdrSize;
	u32		maxitemcount;
	u32		posend;
		
} ST_SPIROM_DATA;

typedef enum {
	LOG_ITEM = 0,
	LOG_HDR,
} LOG_TYPE;

ST_SPIROM_DATA l_spi_log = { 0, };

BOOL __ak_log_user_data_write(u32 pos, u8 *pData, u32 length)
{
	if(l_cpLogBuffer && pData && (pos + length) <= LOG_DATA_FILE_SIZE)
	{
		memcpy((void *)&l_cpLogBuffer[pos], (void *)pData, length);
		return TRUE;
	}
	else
		DEBUGMSG(AKLOG_ERROR, ("%s(): ERROR !\r\n", __func__)); 

	return FALSE;
}

BOOL __ak_log_user_data_read(u32 pos, u8 *pData, u32 length)
{
	if(l_cpLogBuffer && pData && (pos + length) <= LOG_DATA_FILE_SIZE)
	{
		memcpy(pData, &l_cpLogBuffer[pos], length);
		return TRUE;
	}
	else
		DEBUGMSG(AKLOG_ERROR, ("%s(): ERROR !\r\n", __func__)); 
	
	return FALSE;
}

BOOL __ak_log_spirom_hd_read(LPST_LOG_INFOHDR hdr)
{
	if(l_spi_log.init)
	{
		u32 pos	= ((l_spi_log.posend + (l_spi_log.maxitemcount - 1)) % l_spi_log.maxitemcount) * l_spi_log.PageSz; // sizeof(ST_LOG_INFOHDR);

		DEBUGMSG(AKLOG_FUNC, ("	[%s] : pos %d\r\n", __func__, pos)); 
		
		if(__ak_log_user_data_read(pos, (u8 *)hdr, sizeof(ST_LOG_INFOHDR)))
		//if(__ak_log_spirom_read( pos, (u8 *)hdr, sizeof(ST_LOG_INFOHDR)))
		{
			DEBUGMSG(AKLOG_FUNC, ("%s(): pos:%d\n", __func__, pos)); 
			return TRUE;
		}
	}

	return FALSE;
}

int ak_log_load(CString cstrLogFilePath, void * logBuf)
{
    int dwLength;
    unsigned char type;
	unsigned int *fileHandle;
	
	DEBUGMSG(AKLOG_INIT, (" %s()\n", __func__));

	if(logBuf == NULL)
		logBuf = l_cpLogBuffer;
		
	if(logBuf)
	{
		CFile		configFile;
		INT			nLen;

		if( !configFile.Open(cstrLogFilePath, CFile::modeRead | CFile::typeBinary, NULL) )
		{
			DEBUGMSG(AKLOG_ERROR, ( "Log File Open Error!!!" ));
			return FALSE;
		}

		nLen = (INT)configFile.GetLength();
		if(nLen != LOG_MAX_SIZE)
			DEBUGMSG(AKLOG_ERROR, ("Log File Size Error!!!(size:%d)", nLen ));
		
		configFile.Read( logBuf, nLen );
		configFile.Close();
		
		return nLen;
	}
	
	return FALSE;
}
	
void	ak_log_init( CString cstrLogFilePath)
{
	BOOL	isInitDefault	= FALSE;
	u32		maxitemcount	= 0;
	u32		addr = 0, size = 0;
	
	int dummy;
	unsigned int pBytesRead;

	if(l_cpLogBuffer == NULL)
	{
		l_cpLogBuffer = new BYTE[LOG_DATA_FILE_SIZE];

		if(l_cpLogBuffer == NULL)
		{
			DEBUGMSG(AKLOG_ERROR, ("%s(): mv_MemAlloc FAIL !\r\n", __func__)); 
			return;
		}

		if(ak_log_load(cstrLogFilePath, l_cpLogBuffer))
		{
			l_log_init = TRUE;
		}
		else
		{
			ak_log_release(0);
			return;
		}
	}

	ST_LOG_ITEM *p_log;
	u32 current_log_no = 0;
	u32 last_log_no = SYSTEMLOG_MAX_COUNT - 1;
	
	for(int i = 0; i < SYSTEMLOG_MAX_COUNT; i++){
		p_log = (ST_LOG_ITEM *)&l_cpLogBuffer[i*sizeof(ST_LOG_ITEM)];

		if(p_log->log_no && p_log->log_no < last_log_no)
			last_log_no = p_log->log_no;
		
		if(p_log->log_no > current_log_no)
			current_log_no = p_log->log_no;
		else
			break;
	}

	l_log_ihdr.posbegin= last_log_no - 1;
	l_log_ihdr.posend = current_log_no - 1;
	l_log_ihdr.maxitemcount = SYSTEMLOG_MAX_COUNT;

	DEBUGMSG(AKLOG_FUNC, ("%s(): INIT OK (posbigin=%d, posend=%d)\r\n", __func__, l_log_ihdr.posbegin, l_log_ihdr.posend)); 
	l_log_init = TRUE;
}

void	ak_log_release( BOOL diskBackup )
{
	if(l_log_init)
	{
		if(l_spi_log.init == TRUE)
		{
		}
		l_log_init = FALSE;

		DEBUGMSG(AKLOG_INIT, ("ak_log_release(): RELEASE OK (posbigin=%d, posend=%d)\r\n", l_log_ihdr.posbegin, l_log_ihdr.posend)); 
	}

	if(l_cpLogBuffer)
	{
		delete []l_cpLogBuffer;
		l_cpLogBuffer = NULL;
	}
}


u32		ak_log_get_itemcount( void )
{
	if( !l_log_init )	
	{
		return FALSE;
	}
	
	if( MACROLOG_ISEMPTY(l_log_ihdr) )
		return 0;

	if( l_log_ihdr.posbegin > l_log_ihdr.posend )
		return l_log_ihdr.maxitemcount - l_log_ihdr.posbegin + l_log_ihdr.posend + 1;

	return l_log_ihdr.posend - l_log_ihdr.posbegin + 1;
}

u32	ak_log_get_item(u32 pos, u32 count, LPST_LOG_ITEM pitemarray)
{
	u32 _startpos	= 0;
	u32 _readcount	= 0;
	u32 _itemcount 	= 0;
	//u8	i			= 0;
	u32 arraypos	= 0;
	
	if( !l_log_init )	
	{
		return FALSE;
	}

	if( MACROLOG_ISEMPTY(l_log_ihdr) )
		return FALSE;

	_itemcount = ak_log_get_itemcount();
	
	if( _itemcount - pos >= count )
		_readcount	= count;
	else
		_readcount	= _itemcount - pos;

	if(l_log_ihdr.posbegin + pos + _readcount <= l_log_ihdr.maxitemcount)
		_startpos = l_log_ihdr.posbegin + pos;//(l_log_ihdr.maxitemcount - 1) - (pos + _readcount - l_log_ihdr.posend);
	else if( l_log_ihdr.posend >= (_readcount-1) )
		_startpos = l_log_ihdr.posend - (_readcount - 1);
	else
		_startpos = l_log_ihdr.posbegin + pos;//(l_log_ihdr.maxitemcount - 1) - (pos + _readcount - l_log_ihdr.posend);


	DEBUGMSG(0, ("pos=%d, _itemcount=%d, _startpos=%d, _readcount=%d, count = %d (posbigin=%d, posend=%d)\r\n", pos, _itemcount, _startpos, _readcount, count, l_log_ihdr.posbegin, l_log_ihdr.posend)); 

	if( (_startpos + _readcount) > l_log_ihdr.maxitemcount )
	{
		arraypos = l_log_ihdr.maxitemcount - _startpos;
		ak_log_get_item(	pos,
							arraypos,
							pitemarray);
		ak_log_get_item(	pos + arraypos,
							_readcount-arraypos,
							&pitemarray[arraypos] ); //pitemarray );//&pitemarray[ITEM_SIZE-arraypos] );
	}
	else
	{
		if( !__ak_log_user_data_read( MACROLOG_GEPOS(_startpos), (u8*)pitemarray, _readcount * MACROLOG_ITEM_SIZE) )
			return 0;
	}
	DEBUGMSG(0, ("_startpos=%d, _readcount=%d (posbigin=%d, posend=%d)\r\n", _startpos, _readcount, l_log_ihdr.posbegin, l_log_ihdr.posend)); 
//	DEBUGMSG(1, (" pos:%d, get log itime Time:%d \r\n",pos, pitemarray->time));
	return _readcount;
}

bool get_item_type_string_sys(char *szbuff, u16 log_type, u8 log_sub)
{
	switch(log_type)
	{
	case LOG_INIT:
		sprintf(szbuff,"SYSTEM LOG");
		break;
	case LOG_SYSBOOT:
		sprintf(szbuff,"SYSTEM START");
		break;
	case LOG_SYSEND:
		sprintf(szbuff,"SYSTEM END");
		break;
	case LOG_EVENT_DATETIMECHANGE:
		/* sub type */
		if( log_sub == RTC_SRC_GPS )
			sprintf((char*)szbuff, "TIME SETUP(GPS)");
		else if( log_sub == RTC_SRC_SETUP_SD )
			sprintf((char*)szbuff, "TIME SETUP(SD)");
		else if( log_sub == RTC_SRC_SETUP_APP )
			sprintf((char*)szbuff, "TIME SETUP(APP)");
		else if( log_sub == RTC_SRC_SETUP_SERVER )
			sprintf((char*)szbuff, "TIME SETUP(SVR)");
		else
			sprintf((char*)szbuff, "TIME SETUP");
		
		break;
	case LOG_ERRDBOVERTIME:
		sprintf((char*)szbuff, "WRITE ERROR - OVER TIME");
		break;
	case LOG_EVENT_REC_BEGIN:
		sprintf((char*)szbuff, "RECORD(BEGIN)");
		break;
	case LOG_EVENT_REC_END:
		sprintf((char*)szbuff, "RECORD(END)");
		break;
	case LOG_EVENT_LOGINOUT:
		/* sub type */
		if( log_sub == _LOG_LOGIN )
			sprintf((char*)szbuff, "LOG IN  ");
		else if( log_sub == _LOG_LOGOUT )
			sprintf((char*)szbuff, "LOG OUT ");

		break;
	case LOG_EVENT_MEMORY_FORMAT:
		sprintf((char*)szbuff, "FORMAT");
		break;
	case LOG_EVENT_BEGIN_SLEEP:
		sprintf((char*)szbuff, "SLEEP MODE");
		break;
	case LOG_EVENT_ACC_STATE:
		sprintf((char*)szbuff, "ACC");
		break;
	case LOG_EVENT_MP4_FILE_RECOVERY:
		sprintf((char*)szbuff, "RECOVERY");
		break;
	case LOG_DISCHARGE_SLEEP :
		sprintf((char*)szbuff, "DISCHARGE");
		break;
	case LOG_EVENT_MEMORY_INFO:
		sprintf((char*)szbuff, "MEMORY INFO");
		break;
	case LOG_EVENT_SD_CD:
		sprintf((char*)szbuff, "SD CARD");
		break;
	case LOG_EVENT_USB_CD:
		sprintf((char*)szbuff, "USB MEMORY");
		break;
	case LOG_EVENT_BATTCHANGE:
		if( log_sub == eBB_BATT_STATE_NORMAL )
			sprintf((char*)szbuff, "BATT NORMAL");
		else if( log_sub == eBB_BATT_STATE_PARKING)
			sprintf((char*)szbuff, "BATT PARKING");
		else if( log_sub == eBB_BATT_STATE_LOW)
			sprintf((char*)szbuff, "BATT LOW");
		else if( log_sub == eBB_BATT_STATE_EMER_LOW)
			sprintf((char*)szbuff, "BATT EMER LOW");
		else if( log_sub == eBB_BATT_STATE_FALL)
			sprintf((char*)szbuff, "BATT FALL");
		else
			sprintf((char*)szbuff, "BATT");
		
		break;

	case LOG_EVENT_TEMPCHANGE:
		if( log_sub == eBB_TEMP_STATE_NORMAL )
			sprintf((char*)szbuff, "TEMP NORMAL");
		else if( log_sub == eBB_TEMP_STATE_HIGH)
			sprintf((char*)szbuff, "TEMP HIGH");
		else if( log_sub == eBB_TEMP_STATE_LOW)
			sprintf((char*)szbuff, "TEMP LOW");
		else
			sprintf((char*)szbuff, "TEMP");
		
		break;
		
	case LOG_EVENT_EVENT_RECORDING:
		sprintf((char*)szbuff, "EVENT");
		break;

	case LOG_EVENT_GSENSOR_ERROR:
		sprintf((char*)szbuff, "G_SENSOR");
		break;
	case LOG_EVENT_BUTTON_POWER:
		sprintf((char*)szbuff, "KEY");
		break;
	case LOG_EVENT_UPDATE:
		sprintf((char*)szbuff, "S/W UPDATE");
		break;
	
	default:
		sprintf(szbuff,"undefined(%d)", log_type);
		break;
	}

	return TRUE;
}

bool get_item_type_string_error(char *szbuff, u16 log_type, u8 log_sub)
{
	switch(log_type)
	{
		//// error message
		case LOG_ERROR_RTC_READ_FAIL :
			sprintf((char*)szbuff, "[ERROR] RTC");
			break;
		case LOG_ERROR_SD_DISK_INIT_FAIL :
			sprintf((char*)szbuff, "[ERROR] SD");
			break;
		case LOG_ERROR_USB_DISK_INIT_FAIL:
			sprintf((char*)szbuff, "[ERROR] USB");
			break;
		case LOG_ERROR_MEDIA_ERROR :
			sprintf((char*)szbuff, "[ERROR] MEDIA");
			break;
		case LOG_ERROR_MEDIA_WARNING :
			sprintf((char*)szbuff, "[ERROR] MEDIA WARNING");
			break;
		case LOG_ERROR_INTERNAL_ERR	:
			sprintf((char*)szbuff, "[ERROR] INTERNAL");
			break;
		
		default:
			sprintf(szbuff,"undefined(%d)", log_type);
			break;
	}
	return TRUE;
}

bool get_item_type_string_comm(char *szbuff, u16 log_type, u8 log_sub)
{
	switch(log_type)
	{
		case LOG_COMM_WIFI :
			sprintf((char*)szbuff, "WIFI");
			break;
			
		case LOG_COMM_SERVER :
			sprintf((char*)szbuff, "SERVER");
			break;

		case LOG_COMM_APP :
			sprintf((char*)szbuff, "APP");
			break;
			
		default:
			sprintf(szbuff,"undefined(%d)", log_type);
			break;
	}
	return TRUE;
}

bool get_item_type_string_etc(char *szbuff, u16 log_type, u8 log_sub)
{
	switch(log_type)
	{

		default:
			sprintf(szbuff,"undefined(%d)", log_type);
			break;
	}
	return TRUE;
}

bool get_item_type_string_batt(char *szbuff, u16 log_type, u8 log_sub)
{
	switch(log_type)
	{

		default:
			sprintf(szbuff,"undefined(%d)", log_type);
			break;
	}
	return TRUE;
}


BOOL ak_log_get_item_type_string(char *szbuff, LPST_LOG_ITEM read_logitem)
{
	u8 log_sub = read_logitem->log_sub & ~LOG_ITEM_DATA_ELLIPSIS_FLAG;
	
	switch(read_logitem->type)
	{		
		case LOG_TYPE_ERROR : 		get_item_type_string_error(szbuff, read_logitem->log_type, log_sub);	break;
		case LOG_TYPE_COMM : 		get_item_type_string_comm(szbuff, read_logitem->log_type, log_sub);	break;
		case LOG_TYPE_ETC : 			get_item_type_string_etc(szbuff, read_logitem->log_type, log_sub);	break;
		
		case LOG_TYPE_BATTERY : 	//get_item_type_string_batt(szbuff, p_log->log_type, log_sub);	break;
		case LOG_TYPE_TEMPERATURE :
		case LOG_TYPE_SYSTEM : 	get_item_type_string_sys(szbuff, read_logitem->log_type, log_sub);		break;

		default: sprintf(szbuff,"%d", read_logitem->type); break;
	}
	
	return TRUE;
}

/** \} */
