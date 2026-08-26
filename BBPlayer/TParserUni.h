// TParserUni.h: interface for the CTParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TPARSERUNI_H__INCLUDED_)
#define AFX_TPARSERUNI_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "windows.h"

#define UNICODELINESZ         (1024)
#define TXT_INVALID_KEY_UNI     ((TCHAR*)-1)
#define DICTMINSZ_UNI	        128
#define OBJECT_LEN_UNI          64
#define ITEM_LEN_UNI            64	//32

#define FILE_NAME_LEN_UNI		32

typedef enum {
	    LINE_UNPROCESSED_UNI,
	    LINE_ERROR_UNI,
	    LINE_EMPTY_UNI,
	    LINE_COMMENT_UNI,
	    LINE_SECTION_UNI,
	    LINE_VALUE_UNI
	} LINE_STATUS_UNI ;

	typedef struct {
		UINT	n;		    /** Number of entries in DICTIONARY_UNI */
		UINT	size;	    /** Storage size */
		TCHAR 	**val;	    /** List of string values */
		TCHAR 	**key;	    /** List of string keys */
		UINT	*hash;	    /** List of hash values for keys */
	} DICTIONARY_UNI ;

class CTParserUni  
{
public:

	
	CTParserUni(void);
	virtual ~CTParserUni(void);

	DICTIONARY_UNI *Load(TCHAR *FileName);
	DICTIONARY_UNI *Load(BYTE *pBuf, UINT size);
	void DelDict(DICTIONARY_UNI *d);
	TCHAR *GetStr(DICTIONARY_UNI *d, TCHAR *key, TCHAR *def);
	INT GetInt(DICTIONARY_UNI * d, TCHAR * key, int notfound);
	bool GetBool(DICTIONARY_UNI * d, TCHAR * key,bool notfound);
	double GetDouble(DICTIONARY_UNI *d, TCHAR *key, double notfound);
protected:
	TCHAR *GetString(TCHAR *str, int size, HANDLE handle);
	TCHAR *GetString(TCHAR *str, int size, BYTE * pbuf, UINT bufSize, UINT * readPos);
	UINT DictHash(TCHAR *key);
	TCHAR *GetDict(DICTIONARY_UNI *d, TCHAR *key, TCHAR *def);
	static TCHAR *Strlwc(const TCHAR *exchCapital);
	static void *MemDouble(void *ptr, UINT size);
	static TCHAR *XStrdup(TCHAR *s);
	INT DictSet(DICTIONARY_UNI *d, TCHAR *key, TCHAR *val);
	static TCHAR *StrStrip(TCHAR *str);
	static LINE_STATUS_UNI Line(TCHAR *input_line, TCHAR *section, TCHAR *key, TCHAR *value);
	DICTIONARY_UNI *NewDict(UINT size);
	void Remove(TCHAR **str, INT cnt);
};

#endif // !defined(AFX_TPARSERUNI_H__INCLUDED_)
