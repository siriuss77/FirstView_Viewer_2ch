// TParser.cpp: implementation of the CTParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TParserUni.h"

#define TPARSER_ERROR_MSG "	[ERROR] TParserUni : "

CTParserUni::CTParserUni(void)
{
}

CTParserUni::~CTParserUni(void)
{
}

DICTIONARY_UNI *CTParserUni::Load(TCHAR *FileName)
{
	HANDLE handle;
	
	TCHAR line    [UNICODELINESZ + 1];
    TCHAR section [UNICODELINESZ + 1];
    TCHAR key     [UNICODELINESZ + 1];
    TCHAR tmp     [UNICODELINESZ + 1];
    TCHAR val     [UNICODELINESZ + 1];

	int  last=0;
    int  len;
    int  lineno = 0;
    int  errs = 0;

	DICTIONARY_UNI *dict;
	
	handle = CreateFile(FileName,GENERIC_READ,NULL,NULL,OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,NULL);
	if(handle == INVALID_HANDLE_VALUE)
	{
		_tprintf(_T(" 	[ERROR] TParser : %s : cannot open !\r\n"), FileName);
		return FALSE;
	}

	dict = NewDict(0);

	if(!dict)
    {
    	printf("%sCreate a new dictionary object error!\r\n", TPARSER_ERROR_MSG);
        CloseHandle(handle);
        return NULL ;
    }

	memset(line, 0x0, UNICODELINESZ);
    memset(section, 0x0, UNICODELINESZ);
    memset(key, 0x0, UNICODELINESZ);
    memset(val, 0x0, UNICODELINESZ);
	last = 0 ;

	while (GetString(line + last, UNICODELINESZ - last, handle) != NULL)
    {
        lineno++ ;
        len = (INT)lstrlen(line)-1;
        /* Safety check against buffer overflows */
        if (len < 0)
        {
            break;
        }
        
        if (line[len] != '\r' && line[len] != '\n')
        {
        	printf("len = %d [%c %x]\n", len, line[len], line[len]);
            printf("%sinput line too long\r\n", TPARSER_ERROR_MSG);
            DelDict(dict);
            CloseHandle(handle);
            return NULL;
        }
        /* Get rid of \n and spaces at end of line */
        while((len >= 0) && ((line[len] == '\r') || (line[len] == '\n') || (line[len] == ' ')))
        {
            line[len] = 0;
            len--;
        }
        /* Detect multi-line */

        if (len && line[len-1] == ' ' && line[len] == '\\')
        {
         /* Multi-line value */
            last = len;
            continue;
        }
        else
        {
            last = 0;
        }
        switch(Line(line, section, key, val))
        {
            case LINE_EMPTY_UNI:
            case LINE_COMMENT_UNI:
                break;

            case LINE_SECTION_UNI:
                errs = DictSet(dict, section, NULL);
                break;

            case LINE_VALUE_UNI:
                wsprintf(tmp, TEXT("%s:%s"), section, key);
                errs = DictSet(dict, tmp, val) ;
                break;

            case LINE_ERROR_UNI:
                printf("%s syntax error\r\n", TPARSER_ERROR_MSG);
                break;

            default:
                break ;
        }
		
        memset(line, 0, UNICODELINESZ);

        last=0;

        if(errs < 0)
        {
            printf("%s memory allocation failure\n", TPARSER_ERROR_MSG);

            break;
        }
    }
    
    if(errs)
    {
        DelDict(dict);
        printf("%s ERROR\n", TPARSER_ERROR_MSG);
        dict = NULL;
    }
    CloseHandle(handle);

	printf("dict(n = %d)\n", dict->n);
	printf("dict(size = %d)\n", dict->size);
    return dict;	
}

DICTIONARY_UNI *CTParserUni::Load(BYTE *pBuf, UINT bufSize)
{
	TCHAR line    [UNICODELINESZ + 1];
	TCHAR section [UNICODELINESZ + 1];
	TCHAR key     [UNICODELINESZ + 1];
	TCHAR tmp     [UNICODELINESZ + 1];
	TCHAR val     [UNICODELINESZ + 1];

	UINT readPos = 0;
	int  last=0;
	int  len;
	int  lineno = 0;
	int  errs = 0;

	DICTIONARY_UNI *dict;
	
	dict = NewDict(0);

	if(!dict)
    {
    	printf("%sCreate a new dictionary object error!\r\n", TPARSER_ERROR_MSG);
        return NULL ;
    }

	memset(line, 0x0, UNICODELINESZ);
    memset(section, 0x0, UNICODELINESZ);
    memset(key, 0x0, UNICODELINESZ);
    memset(val, 0x0, UNICODELINESZ);
	last = 0 ;

	while (GetString(line + last, UNICODELINESZ - last, pBuf, bufSize, &readPos) != NULL)
    {
        lineno++ ;
        len = (INT)lstrlen(line)-1;
        /* Safety check against buffer overflows */
        if (len < 0)
        {
            break;
        }
        
        if (line[len] !=TEXT('\r') && line[len] != TEXT('\n'))
        {
        	printf("len = %d\n", len);
            printf("%sinput line too long\r\n", TPARSER_ERROR_MSG);
            DelDict(dict);
            return NULL;
        }
        /* Get rid of \n and spaces at end of line */
        while((len >= 0) && ((line[len] == TEXT('\r')) || (line[len] == TEXT('\n')) || (line[len] == TEXT(' '))))
        {
            line[len] = 0;
            len--;
        }
        /* Detect multi-line */
        if (line[len] == TEXT('\\'))
        {
         /* Multi-line value */
            last = len;
            continue;
        }
        else
        {
            last = 0;
        }
        switch(Line(line, section, key, val))
        {
            case LINE_EMPTY_UNI:
            case LINE_COMMENT_UNI:
                break;

            case LINE_SECTION_UNI:
                errs = DictSet(dict, section, NULL);
                break;

            case LINE_VALUE_UNI:
                wsprintf(tmp, TEXT("%s:%s"), section, key);
                errs = DictSet(dict, tmp, val) ;
                break;

            case LINE_ERROR_UNI:
                printf("%s syntax error\r\n", TPARSER_ERROR_MSG);
                break;

            default:
                break ;
        }
		
        memset(line, 0, UNICODELINESZ * sizeof(TCHAR));

        last=0;

        if(errs < 0)
        {
            printf("%s memory allocation failure\n", TPARSER_ERROR_MSG);

            break;
        }
    }
    
    if(errs)
    {
        DelDict(dict);
        printf("%s ERROR\n", TPARSER_ERROR_MSG);
        dict = NULL;
    }

	printf("dict(n = %d)\n", dict->n);
	printf("dict(size = %d)\n", dict->size);
    return dict;	
}

TCHAR *CTParserUni::GetString(TCHAR *str, int size, HANDLE handle)
{
    int i = 0;
    int status = 0;
	DWORD ReadnSize;
    for (i = 0; i < size; i++)
    {
    	status = ReadFile(handle, str + i, sizeof(TCHAR), (LPDWORD)&ReadnSize, NULL);
        if (status < 0)
        {
            return NULL;
        }

        if (str[i] == TEXT('\n') /*|| str[i] == TEXT('\r')*/)
        {
            str[i] = 0;
            return str;
        }        
    }
    return str;
}

TCHAR *CTParserUni::GetString(TCHAR *str, int size, BYTE * pbuf, UINT bufSize, UINT * readPos)
{
    int i = 0;
    int status = 0;
    TCHAR *pStrBuf = (TCHAR *)pbuf;
	
    for (i = 0; i < size; i++)
    {
    	if ((*readPos) >= bufSize)
        {
            return NULL;
        }
    	memcpy(str + i, pStrBuf + (*readPos), sizeof(TCHAR));
		*readPos = (*readPos + 1);
		
        if (str[i] == TEXT('\n') /*|| str[i] == TEXT('\r')*/)
        {
            str[i] = 0;
            return str;
        }        
    }
    return str;
}

UINT CTParserUni::DictHash(TCHAR *key)
{
	INT	    len;
	UINT    hash;
	INT		i;

	len = lstrlen(key);
	
	for (hash=0, i=0; i < len; i++)
	{
		hash += (UINT)key[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	
	return hash ;
}

TCHAR *CTParserUni::GetDict(DICTIONARY_UNI *d, TCHAR *key, TCHAR *def)
{
	UINT	hash;
	INT		i;

	hash = DictHash(key);
	
	for (i=0 ; i< d->size ; i++)
	{
        if (d->key[i] == NULL)
            continue ;
        /* Compare hash */
		if (hash == d->hash[i])
		{
			if (!lstrcmp(key, d->key[i]))
	            {
	            		wprintf(TEXT("key = %s		d->key[i]= %s\r\n"), key, d->key[i] );
				return d->val[i];
		    }
		}
	}
	
	return def ;
}


TCHAR *CTParserUni::Strlwc(const TCHAR *exchCapital)
{
    static TCHAR line[UNICODELINESZ + 1];
    INT i;

    if (exchCapital == NULL)
    {
        return NULL ;
    }
    
    memset(line, 0, (UNICODELINESZ + 1) * sizeof(TCHAR));
    i=0 ;
    
    while (exchCapital[i] && i < UNICODELINESZ)
    {
        line[i] = (TCHAR)towlower((INT)exchCapital[i]);
        i++ ;
    }
    
    line[UNICODELINESZ] = (TCHAR)0;
    
    return line;
}

TCHAR * CTParserUni::GetStr(DICTIONARY_UNI *d, TCHAR *key, TCHAR *def)
{
    TCHAR * lc_key ;
    TCHAR * sval ;
    if ((d == NULL) || (key == NULL))
    {
        return def ;
    }
    
    lc_key = Strlwc(key);
    sval = GetDict(d, lc_key, def);
    
    return sval ;
}

int CTParserUni::GetInt(DICTIONARY_UNI * d, TCHAR * key, int notfound)
{
    TCHAR    *str;
    str = GetStr(d, key, TXT_INVALID_KEY_UNI);
    if(str==TXT_INVALID_KEY_UNI)
        return notfound ;
    return (int)wcstol(str, NULL, 0);
}

bool CTParserUni::GetBool(DICTIONARY_UNI *d, TCHAR *key, bool notfound)
{
    TCHAR    *c;
    bool 	ret;

    c = GetStr(d, key, TXT_INVALID_KEY_UNI);
    
    if(c == TXT_INVALID_KEY_UNI)
    {
        return notfound ;
    }
    
    if ((c[0] == TEXT('y')) || (c[0] == TEXT('Y')) || (c[0] == TEXT('1')) || (c[0] == TEXT('t')) || (c[0] == TEXT('T')))
    {
        ret = 1 ;
    }
    
    else if ((c[0] == TEXT('n')) || (c[0] == TEXT('N')) || (c[0] == TEXT('0')) || (c[0] == TEXT('f')) || (c[0] == TEXT('F')))
    {
        ret = 0 ;
    }
    
    else
    {
        ret = notfound ;
    }
    
    return ret;
}

double CTParserUni::GetDouble(DICTIONARY_UNI *d, TCHAR *key, double notfound)
{
    TCHAR    *str ;

    str = GetStr(d, key, TXT_INVALID_KEY_UNI);
    
    if (str == TXT_INVALID_KEY_UNI)
    {
        return notfound ;
    }
    
    return _wtof(str);
}

void *CTParserUni::MemDouble(void *ptr, UINT size)
{
    void *newptr = NULL ;
 
    newptr = calloc(2 * size, sizeof(TCHAR));
    
    if (newptr == NULL)
    {
    	printf("MemDouble: calloc Error!\n");
        return NULL ;
    }
    
    memcpy(newptr, ptr, size);
    free(ptr);
    
    return newptr ;
}

TCHAR *CTParserUni::XStrdup(TCHAR *s)
{
    static TCHAR *t;
    
    if (!s)
    {
        return NULL;
    }
    
    t = (TCHAR*)malloc((lstrlen(s) + 1) * sizeof(TCHAR)) ;
    
    if (t)
    {
        lstrcpy(t, s);
    }
    return t ;
}

INT CTParserUni::DictSet(DICTIONARY_UNI *d, TCHAR *key, TCHAR *val)
{
	UINT		i;
	UINT	hash;
	
	if ((d == NULL) || (key == NULL))
	{
	    return -1 ;
	}
	/* Compute hash for this key */
	hash = DictHash(key);
	
	/* Find if value is already in DICTIONARY_UNI */
	if (d->n > 0)
	{
		for (i = 0 ; i < d->size ; i++)
		{
            if (d->key[i] == NULL)
            {
                continue;
            }
            
			if (hash == d->hash[i])
			{ /* Same hash value */
				if (!lstrcmp(key, d->key[i]))
				{	 /* Same key */
					/* Found a value: modify and return */
					if (d->val[i] != NULL)
						free(d->val[i]);
                    d->val[i] = val ? XStrdup(val) : NULL ;
                    /* Value has been modified: return */
					return 0 ;
				}
			}
		}
	}
	/* Add a new value */
	/* See if DICTIONARY_UNI needs to grow */
	if (d->n == d->size) 
	{
		/* Reached maximum size: reallocate DICTIONARY_UNI */
		d->val  = (TCHAR **)MemDouble(d->val,  d->size * sizeof(TCHAR*));
		d->key  = (TCHAR **)MemDouble(d->key,  d->size * sizeof(TCHAR*));
		d->hash = (UINT *)MemDouble(d->hash, d->size * sizeof(UINT));
		
        if ((d->val == NULL) || (d->key == NULL) || (d->hash == NULL))
        {
            /* Cannot grow DICTIONARY_UNI */
            return -1;
        }
		/* Double size */
		d->size *= 2;
	}

    /* Insert key in the first empty slot */
    for (i = 0 ; i < d->size ; i++)
    { 
        if (d->key[i] == NULL)
        {
            /* Add key here */
            break;
        }
    }
	/* Copy key */
	d->key[i]  = XStrdup(key);
    d->val[i]  = val ? XStrdup(val) : NULL;
	d->hash[i] = hash;
	d->n++;
	return 0;
}

TCHAR *CTParserUni::StrStrip(TCHAR *str)
{
    static TCHAR line[UNICODELINESZ+1];
	TCHAR * last;
	
    if (str == NULL)
    {
        return NULL;
    }
    
	//while(isspace((UINT)*str) && *str) str++;
	while((((char)*str) == ' ') && *str) str++;
	memset(line, 0, (UNICODELINESZ+1) * sizeof(TCHAR));
	lstrcpy(line, str);
	
	last = line + lstrlen(line);
	while(last > line)
	{
		//if (!isspace((UINT)*(last - 1)))
		if((char)*(last - 1) != ' ')
		{
			break;
		}
		last --;
	}
	*last = (TCHAR)0;
	
	return (TCHAR*)line;
}

LINE_STATUS_UNI CTParserUni::Line(TCHAR *input_line, TCHAR *section, TCHAR *key, TCHAR *value)
{   
    static LINE_STATUS_UNI     sta;
    TCHAR            line[UNICODELINESZ+1];
    INT             len;

    lstrcpy(line, StrStrip(input_line));
    len = (INT)lstrlen(line);

    sta = LINE_UNPROCESSED_UNI;
    if (len < 1)
    {
        sta = LINE_EMPTY_UNI ;         /* Empty line */
    }
    else if(line[0] == TEXT('#'))
    {
        sta = LINE_COMMENT_UNI ;         /* Comment line */
    }
    else if((line[0] == TEXT('[')) && (line[len-1] == TEXT(']')))
    {
        /* Section name */
        swscanf(line, TEXT("[%[^]]"), section);
        lstrcpy(section, StrStrip(section));
        lstrcpy(section, Strlwc(section));
        sta = LINE_SECTION_UNI;
    }
    else if(swscanf(line, TEXT("%[^=] = \"%[^\"]\""), key, value) == 2
           ||  swscanf(line, TEXT("%[^=] = '%[^\']'"), key, value) == 2
           ||  swscanf(line, TEXT("%[^=] = %[^;#]"), key, value) == 2)
    {
        /* Usual key=value, with or without comments */
        lstrcpy(key, StrStrip(key));
        lstrcpy(key, Strlwc(key));
        lstrcpy(value, StrStrip(value));

        if(!lstrcmp(value, TEXT("\"\"")) || (!lstrcmp(value, TEXT("''"))))
        {
            value[0] = 0;
        }
        
        sta = LINE_VALUE_UNI;
    }
    else if(swscanf(line, TEXT("%[^=] = %[;#]"), key, value) == 2
           ||  swscanf(line, TEXT("%[^=] %[=]"), key, value) == 2)
    {
        /*
         * Special cases:
         * key=
         * key=;
         * key=#
         */
        lstrcpy(key, StrStrip(key));
        lstrcpy(key, Strlwc(key));
        value[0] = 0 ;
        sta = LINE_VALUE_UNI ;
    }
    else
    {
        sta = LINE_ERROR_UNI ;         /* Generate syntax error */
    }
    
    return sta ;
}

DICTIONARY_UNI * CTParserUni::NewDict(UINT size)
{
	DICTIONARY_UNI	*d ;

	/* If no size was specified, allocate space for DICTMINSZ_UNI */
	if(size < DICTMINSZ_UNI)
	{
	   size = DICTMINSZ_UNI ;
    }

	d = (DICTIONARY_UNI *)calloc(1, sizeof(DICTIONARY_UNI));
	if(!d)
	{
		printf("NewDict:calloc ERROR! \r\n");
		return NULL;
	}

	d->size = size;
	d->val  = (TCHAR **)calloc(size, sizeof(TCHAR*));
	d->key  = (TCHAR **)calloc(size, sizeof(TCHAR*));
	d->hash = (UINT *)calloc(size, sizeof(UINT));
	
	return d;
}

void CTParserUni::DelDict(DICTIONARY_UNI *d)
{
	UINT	i;

	if (d == NULL)
	{
	    return;
	}
	for (i = 0; i < d->size; i++)
	{
		if (d->key[i] != NULL)
		{
			free(d->key[i]);
        }
		if (d->val[i] != NULL)
        {
			free(d->val[i]);
		}
	}
	free(d->val);
	free(d->key);
	free(d->hash);
	free(d);
	
	return ;
}

void CTParserUni::Remove(TCHAR **str, INT cnt)
{
	if(str)
	{
		for(cnt = 0; cnt < cnt; cnt++)
			free(str[cnt]);

		free(str);
	}
}
