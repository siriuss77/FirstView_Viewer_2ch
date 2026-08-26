// BboxUtil.h: 
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BBOXUTIL_H__INCLUDED_)
#define AFX_BBOXUTIL_H__INCLUDED_
#include "stdafx.h"

class CBBoxUtil  
{
public:
	typedef enum tagBBEventType{
	BB_EVENT_CONTINUOUS, 
	BB_EVENT_MOTION,
	BB_EVENT_GSENSOR,
	BB_EVENT_BUTTON,

	BB_EVENT_TYPE_END
	}eBB_EVENT_TYPE;

	typedef enum tagBBFileType{
	BB_FILE_DATECH, 
	BB_FILE_AVIC,

	BB_EVENT_FILE_END
	}eBB_FILE_TYPE;
	
	CBBoxUtil(void);
	virtual ~CBBoxUtil(void);

	static int GetFileData(CString FullPath, char *data, size_t size, int offset = 0);
	static int ReplaceFileData(CString FullPath, const char *data, size_t size, int offset = 0);
	static CString SetAtFileName(CString FullPath, int iChar, TCHAR * str);
	static CString SetAtFileName(CString FullPath, TCHAR *findStr, TCHAR * changStr);
	static CString GetFileName(CString FullPath);
	static CString GetDir(CString FullPath);
	static CString GetModuleDir(HMODULE hInstance = NULL);
	static int GetEventType(CString FullPath);
	static int GetFileType(CString FullPath);
	static UINT GetCheckSum32(UINT * buff, UINT length);
	static BOOL GetTimeToText(time_t time, char *str_time);
private: 

};

#endif // !defined(AFX_BBOXUTIL_H__INCLUDED_)

