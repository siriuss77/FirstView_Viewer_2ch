// Language.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <list>
#include "TParser.h"

//Viewer config
class CConfig 
{
public:
	enum{	
	PATH_LAST		= 0, //resource [info]
	FILE_SAVE_LAST,
	JPG_SAVE_LAST,
	LOGIN_PASSWORD,
	CAMERA_FLIP_MIRROR,
	UNIT_OF_SPEED,
	ENTRIES_LAST
	};

	
public:
	CConfig(void);
	CConfig(CString * strFileName);
	virtual ~CConfig(void);
	
public:
	CString 	m_LastPath;	//PATH_LAST
	CString 	m_LastFileSavePath;
	CString 	m_LastJpgSavePath;
	CString 	m_Password;
	
	BYTE	m_ucCamera; //bit 0: front flip, bit 1: front mirror, bit 2:rear flip, bit 3:rear mirror;
	BYTE 	m_ucUnitOfSpeed;
	
	BOOL LoadConfig(CString * strFileName = NULL);
	BOOL SaveConfig(CString * strFileName = NULL);
	BOOL GetConfig(INT nConfigNo, CString *cstrValue);
	BOOL GetConfig(INT nConfigNo, BOOL *ff, BOOL *fm, BOOL *rf, BOOL *rm);
	BOOL GetConfig(INT nConfigNo, BYTE *p_ucUnitOfSpeed);
	
	BOOL SetConfig(INT nConfigNo, CString *cstrValue);
	BOOL SetConfig(INT nConfigNo, BOOL ff, BOOL fm, BOOL rf, BOOL rm);
	BOOL SetConfig(INT nConfigNo, BYTE ucUnitOfSpeed);
	
private:
	BOOL  	m_bLoaded;

	BOOL CConfig::CfgPrintfToFile(CFile *fhandle, char *fmt, ... );
};

