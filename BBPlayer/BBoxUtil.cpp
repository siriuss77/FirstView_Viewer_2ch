#include <stdafx.h>
#include <HELPER/CLUtil.H>
#include "BBoxUtil.h"

CBBoxUtil::CBBoxUtil(void)
{
}

CBBoxUtil::~CBBoxUtil(void)
{
}

int CBBoxUtil::GetFileData(CString FullPath, char *data, size_t size, int offset)
{
	CT2A ascii(FullPath.GetBuffer());
	const char * path = (const char * )ascii.m_psz;
	
	int ret = 0;
	FILE *fp = fopen(path, "rb");

	if(fp){
		fseek(fp, offset , SEEK_SET);
		
		ret = fread((void *)data, 1, size, fp);				

		if (ret != (int)size) {
			DEBUGMSG(1, ( "read failed: %d(%s) , at(%d) ret = %d : %d\n", errno, strerror(errno), offset, ret, size));
		}
		fclose(fp);
		
	} else {
		DEBUGMSG(1, ( "%s open failed: %d(%s)\n", path, errno, strerror(errno)));
	}

	//DEBUGMSG(1, ( " ####\n write : %s , at(%d) ret = %d : %d (%s)\n ####\n", path, offset, ret, size, data));
	return ret;
}
int CBBoxUtil::ReplaceFileData(CString FullPath, const char *data, size_t size, int offset)
{
	CT2A ascii(FullPath.GetBuffer());
	const char * path = (const char * )ascii.m_psz;
	
	int ret = 0;
	FILE *fp = fopen(path, "rb+");

	if(fp){
		fseek(fp, offset , SEEK_SET);
		
		ret = fwrite(data, 1, size, fp);				

		if (ret != (int)size) {
			DEBUGMSG(1, ( "write failed: %d(%s) , at(%d) ret = %d : %d\n", errno, strerror(errno), offset, ret, size));
		}
		fclose(fp);
		
	} else {
		DEBUGMSG(1, ( "%s open failed: %d(%s)\n", path, errno, strerror(errno)));
	}

	//DEBUGMSG(1, ( " ####\n write : %s , at(%d) ret = %d : %d (%s)\n ####\n", path, offset, ret, size, data));
	return ret;
}

CString CBBoxUtil::SetAtFileName(CString FullPath, int iChar, TCHAR * str)
{
	TCHAR cDir[_MAX_DIR];
	TCHAR cDrive[_MAX_DRIVE];
	TCHAR Name[200];
	TCHAR cFileExt[20];
	CString cFileName;


	_tsplitpath(FullPath, cDrive, cDir, Name, cFileExt);
	_tcsncpy(&Name[iChar],str, _tcslen(str));

	cFileName.Format(_T("%s%s%s%s"),cDrive, cDir, Name, cFileExt );
	return cFileName;
}

CString CBBoxUtil::SetAtFileName(CString FullPath, TCHAR *findStr, TCHAR * changStr)
{
	TCHAR cDir[_MAX_DIR];
	TCHAR cDrive[_MAX_DRIVE];
	TCHAR Name[200];
	TCHAR cFileExt[20];
	CString cFileName;
	int index = -1;


	_tsplitpath(FullPath, cDrive, cDir, Name, cFileExt);

	cFileName.Format(_T("%s"),Name);
	index = cFileName.Find(CString(findStr));
	if( index >= 0)
		_tcsncpy(&Name[index],changStr, _tcslen(changStr));
	
	cFileName.Format(_T("%s%s%s%s"),cDrive, cDir, Name, cFileExt );
	return cFileName;
}

CString CBBoxUtil::GetFileName(CString FullPath)
{
	TCHAR cDir[_MAX_DIR];
	TCHAR cDrive[_MAX_DRIVE];
	TCHAR Name[200];
	TCHAR cFileExt[20];
	CString cFileName;


	_tsplitpath(FullPath, cDrive, cDir, Name, cFileExt);
	cFileName.Format(_T("%s%s"), Name, cFileExt );
	return cFileName;
}

CString CBBoxUtil::GetDir(CString FullPath)
{
	TCHAR cDir[_MAX_DIR];
	TCHAR cDrive[_MAX_DRIVE];
	TCHAR Name[200];
	TCHAR cFileExt[20];
	CString cFileName;
	
	_tsplitpath(FullPath, cDrive, cDir, Name, cFileExt);
	
	cFileName.Format(_T("%s%s"), cDrive, cDir);
	return 	cFileName;
}

CString CBBoxUtil::GetModuleDir(HMODULE hInstance)
{
#if 0
	WCHAR FullPath[_MAX_PATH];
	char cDir[_MAX_DIR];
	char cDrive[_MAX_DRIVE];
	char Name[200];
	char cFileExt[20];
	CString CfileName;

	
	GetModuleFileName(hInstance, FullPath, 255);

	_splitpath(CT2A(FullPath), cDrive, cDir, Name, cFileExt);

	CfileName.Format(_T("%s%s"), cDrive, cDir);
	return CfileName;
#else
	WCHAR wcsPath[MAX_PATH];
	::GetModuleFileName( hInstance, wcsPath, MAX_PATH );
	std::wstring wtsPath = ClUtil::PATH::getPathSpecW( wcsPath );
	return wtsPath.c_str();
#endif
}

int CBBoxUtil::GetEventType(CString FullPath)
{


/*	int type = 0;
	CString CFileName;
	CFileName = CBBoxUtil::GetFileName(FullPath);
	
	 if(CFileName.Find(_T("C")) >= 0 || CFileName.Find(_T("I") ) >= 0 )
		 type = BB_EVENT_CONTINUOUS;
	 else if(CFileName.Find(_T("E")) >= 0 || CFileName.Find(_T("A")) >= 0 || CFileName.Find(_T("B")) >= 0 )     // jun 201021
		 type = BB_EVENT_GSENSOR;
	// else if(CFileName.Find(_T("M")) >= 0 )            // jun 170511
	 else if(CFileName.Find(_T("H")) >= 0 )              // jun 170511
		 type = BB_EVENT_MOTION;
	// else if(CFileName.Find(_T("B")) >= 0 )            // jun 170511
	 else if(CFileName.Find(_T("G")) >= 0 )              // jun 170511
		 type = BB_EVENT_BUTTON;
	 else
		 type = BB_EVENT_CONTINUOUS;

	 return type;
}
*/





 int type = 0;
    CString CFileName;
    CFileName = CBBoxUtil::GetFileName(FullPath);

    // 첫 번째와 두 번째 언더스코어 위치 찾기
    int firstUnderscorePos = CFileName.Find(_T('_'));
    int secondUnderscorePos = CFileName.Find(_T('_'), firstUnderscorePos + 1);

    // 두 번째 언더스코어 이후 문자열 찾기
    if (secondUnderscorePos != -1) {
        CString remainingSection = CFileName.Mid(secondUnderscorePos + 1);

        // 언더스코어 전에 나타나는 첫 번째 알파벳만 찾기
        for (int i = 0; i < remainingSection.GetLength(); i++) {
            if (_istalpha(remainingSection[i])) {
                TCHAR firstChar = remainingSection[i];

                // 해당 문자에 따라 이벤트 타입 설정
                if (firstChar == _T('C') || firstChar == _T('I'))
                    type = BB_EVENT_CONTINUOUS;
                else if (firstChar == _T('E') || firstChar == _T('A') || firstChar == _T('B'))
                    type = BB_EVENT_GSENSOR;
                else if (firstChar == _T('H'))
                    type = BB_EVENT_MOTION;
                else if (firstChar == _T('G'))
                    type = BB_EVENT_BUTTON;
                else
                    type = BB_EVENT_CONTINUOUS;
                
                break;  // 첫 번째 알파벳을 찾으면 종료
            }
        }
    }

    return type;
}








int CBBoxUtil::GetFileType(CString FullPath)
{
	CString name;
	name = CBBoxUtil::GetFileName(FullPath);
	WCHAR * cFileName = (WCHAR *)name.GetBuffer();
	int type = 0;
	if(cFileName[3] == L'_' && cFileName[10] == L'_') //avic file
		type = BB_FILE_AVIC;
	else
		type = BB_FILE_DATECH;

	return type;
}

UINT CBBoxUtil::GetCheckSum32(UINT * buff, UINT length)
{
	UINT rtn = 0;
	UINT i	= 0;

	for(i=0; i<length; i++)
		rtn += buff[i];

	return rtn;
}

BOOL CBBoxUtil::GetTimeToText(time_t time, char *str_time)
{
	struct tm   *pformated_time = NULL, formated_time;

	pformated_time = &formated_time;
	
	pformated_time = (struct tm *)localtime(&time);
	
    sprintf((char *)str_time, 
                "%04d/%02d/%02d-%02d:%02d:%02d",
                pformated_time->tm_year + 1900, pformated_time->tm_mon + 1, pformated_time->tm_mday,
                pformated_time->tm_hour, pformated_time->tm_min, pformated_time->tm_sec);
	
	return TRUE;
}

typedef struct
{
    char    *filename;
    INT  	create_time;
	INT		event_type;
} HISTORY_INFO;

time_t TimeDate2TimeT(UINT16 Time, UINT16 Date)
{
    struct tm dt;

	dt.tm_sec   =  (Time & 0x1F) << 1;
	dt.tm_min   =  (Time >> 5) & 0x3F;
	dt.tm_hour  =  (Time >> 11) & 0x1F;
    
	dt.tm_mday  =   Date & 0x1F;
	dt.tm_mon   = ((Date >> 5) & 0xF) - 1;
	dt.tm_year  = ((Date >> 9) & 0x7F) + 80;
	dt.tm_isdst = 0;

	return mktime(&dt);
}

time_t GetTimeFromFileName(char *filename)
{
    char year[8], month[4], date[4];
    char hour[4], minute[4], second[4];

    INT year_t, month_t, date_t;
    INT hour_t, minute_t, second_t;     

    UINT16 crDate, crTime;
    time_t createTime;

    memset(year, 0x0, sizeof(year));  
    memset(month, 0x0, sizeof(month));  
    memset(date, 0x0, sizeof(date));  
    memset(hour, 0x0, sizeof(hour));  
    memset(minute, 0x0, sizeof(minute));  
    memset(second, 0x0, sizeof(second)); 

    if (filename[1] == '_')
    {
        // ex) C_121127_153613.mp4
        strncpy(year, filename + 2, 2);
        strncpy(month, filename + 4, 2);
        strncpy(date, filename + 6, 2);
        strncpy(hour, filename + 9, 2);
        strncpy(minute, filename + 11, 2);
        strncpy(second, filename + 13, 2);
    }
    else
	 return 0;


    year_t = atoi((char *)year) + 2000;
    month_t = atoi((char *)month);
    date_t = atoi((char *)date);
    hour_t = atoi((char *)hour);
    minute_t = atoi((char *)minute);
    second_t = atoi((char *)second);    

    crDate = ((year_t - 1980) << 9) | (month_t << 5) | date_t;
    crTime = (hour_t << 11) | (minute_t << 5) | (second_t >> 1);
    createTime = (time_t)TimeDate2TimeT(crTime, crDate);
    createTime += (second_t & 0x01);

    return createTime;
}

char *GetCharFileName(char *filepath)
{
    int i = 0;
    int len = strlen(filepath);

    for (i = (len - 1); i >= 0; i--)
    {
        if ((filepath[i] == '/') || (filepath[i] == '\\'))
        {
            return (filepath + i + 1);
        }
    }

    return filepath;
}

