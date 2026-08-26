
// AkLanguage.cpp: Make for CAkSkin class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include <CLUDE/version.h>
#include <HELPER/CLUtil.H>

#include "Config.h"
#include "BBPlayer.h"

#define CONFIG_FILE_USE		0

#define MAXKEYLENGTH	64
#if CONFIG_FILE_USE

#define CONFIG_FILE_NAME 		TEXT("\\config\\ConfigViewer.ini")

typedef struct
{
    char text[MAXKEYLENGTH];
} ST_KEY_TEXT, *PST_KEY_TEXT;

ST_KEY_TEXT ucConfigKey[]= {
/*PATH_LAST*/					{"path:last"},

/*ENTRIES_LAST*/ 
};
#else //registry use
	#define REGISTRY_KEY		HKEY_CURRENT_USER
 	#define REGISTRY_QUERY_OPEN_PATH  	TEXT("path")
	#define REGISTRY_QUERY_FILE_SAVE_PATH  	TEXT("save_path") // for mp4 file save as
	#define REGISTRY_QUERY_CAPTURE_SAVE_PATH  	TEXT("capture_path") // for image capture 
	#define REGISTRY_QUERY_CAMERA  	TEXT("camera") // bit 0: front flip, bit 1: front mirror, bit 2:rear flip, bit 3:rear mirror;
	#define REGISTRY_QUERY_UNIT_OF_SPEED  	TEXT("speed") //
	#define REGISTRY_QUERY_PASSWORD 	TEXT("login_password") //20170829
	
  #if (BUILD_MODEL == MODEL_DAVIEW_2CH)
	#define REGISTRY_SUBKEY  	TEXT("Software\\Daview\\2HD")
  #elif (BUILD_MODEL == MODEL_DAVIEW_2CH_L)	                      // jun 170405
	#define REGISTRY_SUBKEY  	TEXT("Software\\Daview\\2HD-L")
  #elif (BUILD_MODEL == MODEL_DAVIEW_2CH_F)	                      // jun 170406
	#define REGISTRY_SUBKEY  	TEXT("Software\\Daview\\2HD-F")
  #elif (BUILD_MODEL == MODEL_DAVIEW_2CH_S)	                      // jun 171018
	#define REGISTRY_SUBKEY  	TEXT("Software\\Daview\\2HD-S")
  #elif  (BUILD_MODEL == MODEL_NH_200A)
  	#define REGISTRY_SUBKEY  	TEXT("Software\\Nighthawk\\NH-200A")
  #elif  (BUILD_MODEL == MODEL_BW_2000)
  	#define REGISTRY_SUBKEY  	TEXT("Software\\HyundaiDLS\\BW-2000")
  #elif (BUILD_MODEL == MODEL_Q_BLACK)
  	#define REGISTRY_SUBKEY	TEXT("Software\\QDIS\\Q-Black")
  #elif (BUILD_MODEL == MODEL_KIC_M7)
  	#define REGISTRY_SUBKEY	TEXT("Software\\KIC\\M7")
  #elif (BUILD_MODEL == MODEL_SAMPLE || BUILD_MODEL == MODEL_HONDA_HD_2U)
  	#define REGISTRY_SUBKEY	TEXT("Software\\Blackbox_\\Sample")
  #elif (BUILD_MODEL == MODEL_ITSG_HD_2U)
  	#define REGISTRY_SUBKEY	TEXT("Software\\ITSG\\HD-2U")  
  #elif (BUILD_MODEL == MODEL_TMPD_HD_2U)
  	#define REGISTRY_SUBKEY	TEXT("Software\\TMPD\\HD-2U")  
  #elif (BUILD_MODEL == MODEL_ICARO_I300HD)
 	#define REGISTRY_SUBKEY	TEXT("Software\\ICARO\\I300HD")
  #elif (BUILD_MODEL == MODEL_VIEW_I_2U)
	#define REGISTRY_SUBKEY	TEXT("Software\\VIEW_I\\HD-2U")
  #elif (BUILD_MODEL == MODEL_DOOCO_M7)
  	#define REGISTRY_SUBKEY	TEXT("Software\\DOOCO\\M7")
  #elif (BUILD_MODEL == MODEL_RALLY_EYES)
  	#define REGISTRY_SUBKEY	TEXT("Software\\RallyEyes\\HD-2U")
  #elif (BUILD_MODEL == MODEL_EYE_TRACKS)
  	#define REGISTRY_SUBKEY	TEXT("Software\\EyeTracks\\I300HD")
  #endif
#endif
CConfig::CConfig(void)
{
	m_bLoaded = FALSE;
}

CConfig::CConfig(CString * strFileName)
{
	m_bLoaded = FALSE;
	LoadConfig(strFileName);
}

CConfig::~CConfig(void)
{
}
	
	
BOOL CConfig::LoadConfig(CString * strFileName)
{
#if CONFIG_FILE_USE
	DICTIONARY	*p_CfgDt;
	CTParser	cfgTPar;

	CString cstrDir;
	CString cstrPath;
	
	char *c = NULL;
	
	cstrDir = ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getModulePath();
	cstrPath = cstrDir ;

	if(strFileName == NULL)
		cstrPath +=  CONFIG_FILE_NAME;
	else 
		cstrPath += *strFileName;
		
	
	p_CfgDt = cfgTPar.Load(cstrPath.GetBuffer());

	if(!p_CfgDt) {
		DEBUGMSG(1, ("LoadConfig(): Load ERROR!!\n"));
		return FALSE;
	}

	c = cfgTPar.GetStr(p_CfgDt, ucConfigKey[CConfig::PATH_LAST].text,  "C:\\");
	
	TCHAR lpwzName[256];
	if(ClUtil::STRING::convertChar2Wchar(lpwzName, c, 256))
	{
		m_LastPath.Format( _T("%s"),  lpwzName);
	}
	
	cfgTPar.DelDict(p_CfgDt);
	m_bLoaded = TRUE;
#else

	HKEY	hSubKey;
	DWORD	dwType = REG_SZ;

	TCHAR szBuffer[512] = {'\0' ,};
	DWORD	dwSize = sizeof(szBuffer);
	
	LONG lRtn = RegOpenKeyEx(REGISTRY_KEY,
							 REGISTRY_SUBKEY,
							 0,
							 KEY_READ,
							 &hSubKey);
	
	if(ERROR_SUCCESS == lRtn)
	{
		lRtn = RegQueryValueEx(hSubKey,
							   REGISTRY_QUERY_OPEN_PATH,
							   NULL,
							   NULL,
							   NULL,
							   &dwSize);

		if( lRtn != ERROR_SUCCESS)
		{
			RegCloseKey(hSubKey);
			return lRtn;
		}		
	}
	else
	{
		return lRtn;
	}


	if(ERROR_SUCCESS == lRtn) 
	{
		dwSize = sizeof(szBuffer);
		lRtn = RegQueryValueEx(hSubKey,
							   REGISTRY_QUERY_OPEN_PATH,
							   NULL,
							   &dwType,
							   (LPBYTE)szBuffer,
							   &dwSize);
		
		if(ERROR_SUCCESS == lRtn) 
			m_LastPath.Format( _T("%s"),  szBuffer);
		else
			m_LastPath = _T("");


		dwSize = sizeof(szBuffer);
		lRtn = RegQueryValueEx(hSubKey,
							   REGISTRY_QUERY_FILE_SAVE_PATH,
							   NULL,
							   &dwType,
							   (LPBYTE)szBuffer,
							   &dwSize);

		dwSize = sizeof(szBuffer);
		lRtn = RegQueryValueEx(hSubKey,
							   REGISTRY_QUERY_FILE_SAVE_PATH,
							   NULL,
							   &dwType,
							   (LPBYTE)szBuffer,
							   &dwSize);
		
		if(ERROR_SUCCESS == lRtn) 
			m_LastFileSavePath.Format( _T("%s"),  szBuffer);
		else
			m_LastFileSavePath = _T("");

		//
		dwSize = sizeof(szBuffer);
		lRtn = RegQueryValueEx(hSubKey,
							   REGISTRY_QUERY_CAPTURE_SAVE_PATH,
							   NULL,
							   &dwType,
							   (LPBYTE)szBuffer,
							   &dwSize);
		
		if(ERROR_SUCCESS == lRtn) 
			m_LastJpgSavePath.Format( _T("%s"),  szBuffer);
		else
			m_LastJpgSavePath = _T("");
		
		// password
		dwSize = sizeof(szBuffer);
		lRtn = RegQueryValueEx(hSubKey,
							   REGISTRY_QUERY_PASSWORD,
							   NULL,
							   &dwType,
							   (LPBYTE)szBuffer,
							   &dwSize);
		
		if(ERROR_SUCCESS == lRtn)
			m_Password.Format( _T("%s"),  szBuffer);
		else
			m_Password = DEFAULT_PASSWORD;

		
		//camera flip, mirror
		dwSize = sizeof(szBuffer);
		lRtn = RegQueryValueEx(hSubKey,
							   REGISTRY_QUERY_CAMERA,
							   NULL,
							   &dwType,
							  (LPBYTE)szBuffer,
							   &dwSize);
		
		if(ERROR_SUCCESS == lRtn) 
			m_ucCamera = _tstoi(szBuffer);
		else
			m_ucCamera = 0;

		// UNIT OF SPEED
		dwSize = sizeof(szBuffer);
		lRtn = RegQueryValueEx(hSubKey,
							   REGISTRY_QUERY_UNIT_OF_SPEED,
							   NULL,
							   &dwType,
							  (LPBYTE)szBuffer,
							   &dwSize);
		
		if(ERROR_SUCCESS == lRtn) 
			m_ucUnitOfSpeed = _tstoi(szBuffer);
		else
			m_ucUnitOfSpeed = 0;
		
		RegCloseKey(hSubKey);
		m_bLoaded = TRUE;
	}

#endif
	
	return m_bLoaded;
}

BOOL CConfig::SaveConfig(CString * strFileName)
{
	BOOL ret = FALSE;
#if CONFIG_FILE_USE
	CFile file;
	int ch,i;
	

	CFile *fhandle = NULL;

	CString cstrDir;
	CString cstrPath;
	
	
	cstrDir = ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getModulePath();
	cstrPath = cstrDir ;

	if(strFileName == NULL)
		cstrPath +=  CONFIG_FILE_NAME;
	else 
		cstrPath += *strFileName;
		
	
	if(file.Open(cstrPath.GetBuffer(), CFile::modeCreate | CFile::modeReadWrite))
	{
		fhandle = &file;


		CfgPrintfToFile(fhandle, "\r\n[Path]\r\n");
		
		//CConfig::PATH_LAST
		char strName[256];
		if(ClUtil::STRING::convertWchar2Char(strName, m_LastPath.GetBuffer(),  256))
			CfgPrintfToFile(fhandle, "Last          = %s\r\n", strName);
	
		
		file.Close();
		ret = TRUE;
	}
	else
	{
 #ifdef DEBUG	
		char strName[256];
		ClUtil::STRING::convertWchar2Char(strName, cstrPath.GetBuffer(),  256);
		DEBUGMSG(1, ("file open ERROR! (%s)\n", strName));
 #endif
	}
#else
	HKEY hSubKey;
	LONG lRtn = RegCreateKeyEx (REGISTRY_KEY,
								REGISTRY_SUBKEY,
								0,
								REG_NONE,
								REG_OPTION_NON_VOLATILE,
								KEY_WRITE,
								NULL,
								&hSubKey,
								NULL);

	if(ERROR_SUCCESS == lRtn)
	{		
		lRtn = RegSetValueEx (hSubKey,
							  REGISTRY_QUERY_OPEN_PATH,
							  0,
							  REG_SZ,
							  (LPBYTE)m_LastPath.GetBuffer(),
							  (m_LastPath.GetLength() + 1)*2);

		lRtn = RegSetValueEx (hSubKey,
							  REGISTRY_QUERY_FILE_SAVE_PATH,
							  0,
							  REG_SZ,
							  (LPBYTE)m_LastFileSavePath.GetBuffer(),
							  (m_LastFileSavePath.GetLength() + 1)*2);

		lRtn = RegSetValueEx (hSubKey,
							  REGISTRY_QUERY_CAPTURE_SAVE_PATH,
							  0,
							  REG_SZ,
							  (LPBYTE)m_LastJpgSavePath.GetBuffer(),
							  (m_LastJpgSavePath.GetLength() + 1)*2);

		lRtn = RegSetValueEx (hSubKey,
							  REGISTRY_QUERY_PASSWORD,
							  0,
							  REG_SZ,
							  (LPBYTE)m_Password.GetBuffer(),
							  (m_Password.GetLength() + 1)*2);
		
		
		lRtn = RegSetValueEx (hSubKey,
							  REGISTRY_QUERY_CAMERA,
							  0,
							  REG_SZ,
							  (LPBYTE)&m_ucCamera,
							  sizeof(BYTE));

		lRtn = RegSetValueEx (hSubKey,
							  REGISTRY_QUERY_UNIT_OF_SPEED,
							  0,
							  REG_SZ,
							  (LPBYTE)&m_ucUnitOfSpeed,
							  sizeof(BYTE));
		
		RegCloseKey(hSubKey);

		ret = TRUE;
	}
	
#endif
	return ret;
}

BOOL CConfig::GetConfig(INT nConfigNo, CString *cstrValue)
{
	if(nConfigNo < CConfig::ENTRIES_LAST)
	{
		switch(	nConfigNo )
		{
			case CConfig::PATH_LAST :				*cstrValue = m_LastPath;	break;
			case CConfig::FILE_SAVE_LAST :		*cstrValue = m_LastFileSavePath;	break;
			case CConfig::JPG_SAVE_LAST :			*cstrValue = m_LastJpgSavePath;	break;
			case CConfig::LOGIN_PASSWORD:		*cstrValue = m_Password;	break;
		}
		
		return TRUE;
	}
	return FALSE;
}

BOOL CConfig::GetConfig(INT nConfigNo, BOOL *ff, BOOL *fm, BOOL *rf, BOOL *rm)
{
	if(nConfigNo == CConfig::CAMERA_FLIP_MIRROR && ff && fm && rf && rm)
	{
		*ff = (m_ucCamera & 1) ? 1 : 0;
		*fm = (m_ucCamera & 2) ? 1 : 0;
		*rf = (m_ucCamera & 4) ? 1 : 0;
		*rm = (m_ucCamera & 8) ? 1 : 0;
		return TRUE;
	}
	return FALSE;
}

BOOL CConfig::GetConfig(INT nConfigNo, BYTE *p_ucUnitOfSpeed)
{
	if(nConfigNo == CConfig::UNIT_OF_SPEED && p_ucUnitOfSpeed)
	{
		*p_ucUnitOfSpeed = m_ucUnitOfSpeed;
		return TRUE;
	}
	return FALSE;
}


BOOL CConfig::SetConfig(INT nConfigNo, CString *cstrValue)
{
	if(nConfigNo < CConfig::ENTRIES_LAST && cstrValue)
	{
		switch(	nConfigNo )
		{
			case CConfig::PATH_LAST :				m_LastPath = *cstrValue;	break;
			case CConfig::FILE_SAVE_LAST :		m_LastFileSavePath = *cstrValue;	break;
			case CConfig::JPG_SAVE_LAST :			m_LastJpgSavePath = *cstrValue;	break;
			case CConfig::LOGIN_PASSWORD:		m_Password = *cstrValue;	break;
		}
		
		return TRUE;
	}
	return FALSE;
}

BOOL CConfig::SetConfig(INT nConfigNo, BOOL ff, BOOL fm, BOOL rf, BOOL rm)
{
	if(nConfigNo == CConfig::CAMERA_FLIP_MIRROR)
	{
		m_ucCamera = (ff | (fm << 1) | (rf << 2) | (rm << 3));
		return TRUE;
	}
	return FALSE;
}

BOOL CConfig::SetConfig(INT nConfigNo, BYTE ucUnitOfSpeed)
{
	if(nConfigNo == CConfig::UNIT_OF_SPEED)
	{
		m_ucUnitOfSpeed =ucUnitOfSpeed;
		return TRUE;
	}
	return FALSE;
}

BOOL CConfig::CfgPrintfToFile(CFile *fhandle, char *fmt, ... )
{
	BOOL bRet = FALSE;
#if CONFIG_FILE_USE
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
#endif
	return bRet;
}
	
#undef MAXKEYLENGTH

