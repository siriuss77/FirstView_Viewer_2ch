// TParser.h: interface for the CTParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TPARSER_H__INCLUDED_)
#define AFX_TPARSER_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "windows.h"

#define ASCIILINESZ         (1024)
#define TXT_INVALID_KEY     ((char*)-1)
#define DICTMINSZ	        128
#define OBJECT_LEN          64
#define ITEM_LEN            64	//32

#define FILE_NAME_LEN		32

typedef enum {
    LINE_UNPROCESSED,
    LINE_ERROR,
    LINE_EMPTY,
    LINE_COMMENT,
    LINE_SECTION,
    LINE_VALUE
} LINE_STATUS ;

typedef struct {
	UINT	n;		    /** Number of entries in DICTIONARY */
	UINT	size;	    /** Storage size */
	char 	**val;	    /** List of string values */
	char 	**key;	    /** List of string keys */
	UINT	*hash;	    /** List of hash values for keys */
} DICTIONARY ;

typedef struct {
	char dir[128];
	char **name;
} ST_UDI_FILE, *PST_UDI_FILE;

typedef struct {
	UINT TableVer;
	char BinName[128];
	UINT ImageCount;
	UINT SoundCount;
	UINT ViewerCount;
	ST_UDI_FILE Image;
	ST_UDI_FILE Sound;
	ST_UDI_FILE Viewer;
} ST_UD_INFO, *PST_UD_INFO ;

typedef struct {
	char fName[FILE_NAME_LEN];
} ST_FNAME;

typedef enum {
    ITEM_TYPE_IMAGE,
    ITEM_TYPE_SOUND,
    ITEM_TYPE_VIEWER,

	ITEM_TYPE_END
} ITIME_TYPE ;

class CTParser  
{
public:

	ST_UD_INFO mUDI;
	
	CTParser(void);
	virtual ~CTParser(void);

	DICTIONARY *Load(TCHAR *FileName);
	DICTIONARY *Load(BYTE *pBuf, UINT size);
	void DelDict(DICTIONARY *d);
	char *GetStr(DICTIONARY *d, char *key, char *def);
	INT GetInt(DICTIONARY * d, char * key, int notfound);
	bool GetBool(DICTIONARY * d, char * key,bool notfound);
	double GetDouble(DICTIONARY *d, char *key, double notfound);
protected:
	char *GetString(char *str, int size, HANDLE handle);
	char *GetString(char *str, int size, BYTE * pbuf, UINT bufSize, UINT * readPos);
	UINT DictHash(char *key);
	char *GetDict(DICTIONARY *d, char *key, char *def);
	static char *Strlwc(const char *exchCapital);
	static void *MemDouble(void *ptr, UINT size);
	static char *XStrdup(char *s);
	INT DictSet(DICTIONARY *d, char *key, char *val);
	static char *StrStrip(char *str);
	static LINE_STATUS Line(char *input_line, char *section, char *key, char *value);
	DICTIONARY *NewDict(UINT size);
	void Remove(char **str, INT cnt);
};

#endif // !defined(AFX_TPARSER_H__INCLUDED_)
