// TParser.cpp: implementation of the CTParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TParser.h"

#define TPARSER_ERROR_MSG "	[ERROR] TParser : "

CTParser::CTParser(void)
{
}

CTParser::~CTParser(void)
{
}

DICTIONARY *CTParser::Load(TCHAR *FileName)
{
	HANDLE handle;
	
	char line    [ASCIILINESZ + 1];
    char section [ASCIILINESZ + 1];
    char key     [ASCIILINESZ + 1];
    char tmp     [ASCIILINESZ + 1];
    char val     [ASCIILINESZ + 1];

	int  last=0;
    int  len;
    int  lineno = 0;
    int  errs = 0;

	DICTIONARY *dict;
	
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

	memset(line, 0x0, ASCIILINESZ);
    memset(section, 0x0, ASCIILINESZ);
    memset(key, 0x0, ASCIILINESZ);
    memset(val, 0x0, ASCIILINESZ);
	last = 0 ;

	while (GetString(line + last, ASCIILINESZ - last, handle) != NULL)
    {
        lineno++ ;
        len = (INT)strlen(line)-1;
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
            case LINE_EMPTY:
            case LINE_COMMENT:
                break;

            case LINE_SECTION:
                errs = DictSet(dict, section, NULL);
                break;

            case LINE_VALUE:
                sprintf(tmp, "%s:%s", section, key);
                errs = DictSet(dict, tmp, val) ;
                break;

            case LINE_ERROR:
                printf("%s syntax error\r\n", TPARSER_ERROR_MSG);
                break;

            default:
                break ;
        }
		
        memset(line, 0, ASCIILINESZ);

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

DICTIONARY *CTParser::Load(BYTE *pBuf, UINT bufSize)
{
	char line    [ASCIILINESZ + 1];
    char section [ASCIILINESZ + 1];
    char key     [ASCIILINESZ + 1];
    char tmp     [ASCIILINESZ + 1];
    char val     [ASCIILINESZ + 1];

	UINT readPos = 0;
	int  last=0;
    int  len;
    int  lineno = 0;
    int  errs = 0;

	DICTIONARY *dict;
	
	dict = NewDict(0);

	if(!dict)
    {
    	printf("%sCreate a new dictionary object error!\r\n", TPARSER_ERROR_MSG);
        return NULL ;
    }

	memset(line, 0x0, ASCIILINESZ);
    memset(section, 0x0, ASCIILINESZ);
    memset(key, 0x0, ASCIILINESZ);
    memset(val, 0x0, ASCIILINESZ);
	last = 0 ;

	while (GetString(line + last, ASCIILINESZ - last, pBuf, bufSize, &readPos) != NULL)
    {
        lineno++ ;
        len = (INT)strlen(line)-1;
        /* Safety check against buffer overflows */
        if (len < 0)
        {
            break;
        }
        
        if (line[len] != '\r' && line[len] != '\n')
        {
        	printf("len = %d\n", len);
            printf("%sinput line too long\r\n", TPARSER_ERROR_MSG);
            DelDict(dict);
            return NULL;
        }
        /* Get rid of \n and spaces at end of line */
        while((len >= 0) && ((line[len] == '\r') || (line[len] == '\n') || (line[len] == ' ')))
        {
            line[len] = 0;
            len--;
        }
        /* Detect multi-line */
        if (line[len] == '\\')
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
            case LINE_EMPTY:
            case LINE_COMMENT:
                break;

            case LINE_SECTION:
                errs = DictSet(dict, section, NULL);
                break;

            case LINE_VALUE:
                sprintf(tmp, "%s:%s", section, key);
                errs = DictSet(dict, tmp, val) ;
                break;

            case LINE_ERROR:
                printf("%s syntax error\r\n", TPARSER_ERROR_MSG);
                break;

            default:
                break ;
        }
		
        memset(line, 0, ASCIILINESZ);

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

char *CTParser::GetString(char *str, int size, HANDLE handle)
{
    int i = 0;
    int status = 0;
	DWORD ReadnSize;
    for (i = 0; i < size; i++)
    {
    	status = ReadFile(handle, str + i, sizeof(char), (LPDWORD)&ReadnSize, NULL);
        if (status < 0)
        {
            return NULL;
        }

        if (str[i] == 0xa /*|| str[i] == '\r'*/)
        {
            str[i] = 0;
            return str;
        }        
    }
    return str;
}

char *CTParser::GetString(char *str, int size, BYTE * pbuf, UINT bufSize, UINT * readPos)
{
    int i = 0;
    int status = 0;
    for (i = 0; i < size; i++)
    {
    	if ((*readPos) >= bufSize)
        {
            return NULL;
        }
    	memcpy(str + i, pbuf + (*readPos), sizeof(char));
		*readPos = (*readPos + 1);
		
        if (str[i] == 0xa /*|| str[i] == '\r'*/)
        {
            str[i] = 0;
            return str;
        }        
    }
    return str;
}

UINT CTParser::DictHash(char *key)
{
	INT	    len;
	UINT    hash;
	INT		i;

	len = strlen(key);
	
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

char *CTParser::GetDict(DICTIONARY *d, char *key, char *def)
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
			if (!strcmp(key, d->key[i]))
            {
            	printf("key = %s		d->key[i]= %s\r\n", key, d->key[i] );
				return d->val[i];
		    }
		}
	}
	
	return def ;
}


char *CTParser::Strlwc(const char *exchCapital)
{
    static char line[ASCIILINESZ + 1];
    INT i;

    if (exchCapital == NULL)
    {
        return NULL ;
    }
    
    memset(line, 0, ASCIILINESZ + 1);
    i=0 ;
    
    while (exchCapital[i] && i < ASCIILINESZ)
    {
        line[i] = (char)tolower((INT)exchCapital[i]);
        i++ ;
    }
    
    line[ASCIILINESZ] = (char)0;
    
    return line;
}

char * CTParser::GetStr(DICTIONARY *d, char *key, char *def)
{
    char * lc_key ;
    char * sval ;
    if ((d == NULL) || (key == NULL))
    {
        return def ;
    }
    
    lc_key = Strlwc(key);
    sval = GetDict(d, lc_key, def);
    
    return sval ;
}

int CTParser::GetInt(DICTIONARY * d, char * key, int notfound)
{
    char    *str;
    str = GetStr(d, key, TXT_INVALID_KEY);
    if(str==TXT_INVALID_KEY)
        return notfound ;
    return (int)strtol(str, NULL, 0);
}

bool CTParser::GetBool(DICTIONARY *d, char *key, bool notfound)
{
    char    *c;
    bool 	ret;

    c = GetStr(d, key, TXT_INVALID_KEY);
    
    if(c == TXT_INVALID_KEY)
    {
        return notfound ;
    }
    
    if ((c[0] == 'y') || (c[0] == 'Y') || (c[0] == '1') || (c[0] == 't') || (c[0] == 'T'))
    {
        ret = 1 ;
    }
    
    else if ((c[0] == 'n') || (c[0] == 'N') || (c[0] == '0') || (c[0] == 'f') || (c[0] == 'F'))
    {
        ret = 0 ;
    }
    
    else
    {
        ret = notfound ;
    }
    
    return ret;
}

double CTParser::GetDouble(DICTIONARY *d, char *key, double notfound)
{
    char    *str ;

    str = GetStr(d, key, TXT_INVALID_KEY);
    
    if (str == TXT_INVALID_KEY)
    {
        return notfound ;
    }
    
    return atof(str);
}

void *CTParser::MemDouble(void *ptr, UINT size)
{
    void *newptr = NULL ;
 
    newptr = calloc(2 * size, 1);
    
    if (newptr == NULL)
    {
    	printf("MemDouble: calloc Error!\n");
        return NULL ;
    }
    
    memcpy(newptr, ptr, size);
    free(ptr);
    
    return newptr ;
}

char *CTParser::XStrdup(char *s)
{
    static char *t;
    
    if (!s)
    {
        return NULL;
    }
    
    t = (char*)malloc(strlen(s) + 1) ;
    
    if (t)
    {
        strcpy(t, s);
    }
    return t ;
}

INT CTParser::DictSet(DICTIONARY *d, char *key, char *val)
{
	UINT		i;
	UINT	hash;
	
	if ((d == NULL) || (key == NULL))
	{
	    return -1 ;
	}
	/* Compute hash for this key */
	hash = DictHash(key);
	
	/* Find if value is already in DICTIONARY */
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
				if (!strcmp(key, d->key[i]))
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
	/* See if DICTIONARY needs to grow */
	if (d->n == d->size) 
	{
		/* Reached maximum size: reallocate DICTIONARY */
		d->val  = (char **)MemDouble(d->val,  d->size * sizeof(char*));
		d->key  = (char **)MemDouble(d->key,  d->size * sizeof(char*));
		d->hash = (UINT *)MemDouble(d->hash, d->size * sizeof(UINT));
		
        if ((d->val == NULL) || (d->key == NULL) || (d->hash == NULL))
        {
            /* Cannot grow DICTIONARY */
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

char *CTParser::StrStrip(char *str)
{
    static char line[ASCIILINESZ+1];
	char * last;
	
    if (str == NULL)
    {
        return NULL;
    }
    
	//while(isspace((UINT)*str) && *str) str++;
	while((((char)*str) == ' ') && *str) str++;
	memset(line, 0, ASCIILINESZ+1);
	strcpy(line, str);
	
	last = line + strlen(line);
	while(last > line)
	{
		//if (!isspace((UINT)*(last - 1)))
		if((char)*(last - 1) != ' ')
		{
			break;
		}
		last --;
	}
	*last = (char)0;
	
	return (char*)line;
}

LINE_STATUS CTParser::Line(char *input_line, char *section, char *key, char *value)
{   
    static LINE_STATUS     sta;
    char            line[ASCIILINESZ+1];
    INT             len;

    strcpy(line, StrStrip(input_line));
    len = (INT)strlen(line);

    sta = LINE_UNPROCESSED;
    if (len < 1)
    {
        sta = LINE_EMPTY ;         /* Empty line */
    }
    else if(line[0] == '#')
    {
        sta = LINE_COMMENT ;         /* Comment line */
    }
    else if((line[0] == '[') && (line[len-1] == ']'))
    {
        /* Section name */
        sscanf(line, "[%[^]]", section);
        strcpy(section, StrStrip(section));
        strcpy(section, Strlwc(section));
        sta = LINE_SECTION;
    }
    else if(sscanf(line, "%[^=] = \"%[^\"]\"", key, value) == 2
           ||  sscanf(line, "%[^=] = '%[^\']'", key, value) == 2
           ||  sscanf(line, "%[^=] = %[^;#]", key, value) == 2)
    {
        /* Usual key=value, with or without comments */
        strcpy(key, StrStrip(key));
        strcpy(key, Strlwc(key));
        strcpy(value, StrStrip(value));

        if(!strcmp(value, "\"\"") || (!strcmp(value, "''")))
        {
            value[0] = 0;
        }
        
        sta = LINE_VALUE;
    }
    else if(sscanf(line, "%[^=] = %[;#]", key, value) == 2
           ||  sscanf(line, "%[^=] %[=]", key, value) == 2)
    {
        /*
         * Special cases:
         * key=
         * key=;
         * key=#
         */
        strcpy(key, StrStrip(key));
        strcpy(key, Strlwc(key));
        value[0] = 0 ;
        sta = LINE_VALUE ;
    }
    else
    {
        sta = LINE_ERROR ;         /* Generate syntax error */
    }
    
    return sta ;
}

DICTIONARY * CTParser::NewDict(UINT size)
{
	DICTIONARY	*d ;

	/* If no size was specified, allocate space for DICTMINSZ */
	if(size < DICTMINSZ)
	{
	   size = DICTMINSZ ;
    }

	d = (DICTIONARY *)calloc(1, sizeof(DICTIONARY));
	if(!d)
	{
		printf("NewDict:calloc ERROR! \r\n");
		return NULL;
	}

	d->size = size;
	d->val  = (char **)calloc(size, sizeof(char*));
	d->key  = (char **)calloc(size, sizeof(char*));
	d->hash = (UINT *)calloc(size, sizeof(UINT));
	
	return d;
}

void CTParser::DelDict(DICTIONARY *d)
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

void CTParser::Remove(char **str, INT cnt)
{
	if(str)
	{
		for(cnt = 0; cnt < cnt; cnt++)
			free(str[cnt]);

		free(str);
	}
}
