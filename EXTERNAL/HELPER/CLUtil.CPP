//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClUtil.CPP
//
//	Description: CLENAW Utility class
//
//	Update History
// 
//		2011.10.10 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. INCLUDE

#include <stdafx.h>
#include "CLUtil.H"

#include <CLENAW/ClTypes.H>

//================================================================================================= E. INCLUDE
//================================================================================================= S.GEOMETRY

void ClUtil::GEOMETRY::recalcGeometry( ClGeometry& geometry, int width, int height, DWORD dwFlag )
{
	ClGeometry preGeometry = geometry;
	
	if( dwFlag & GEO_PERCENT )
	{
		ClGeometry realGeometry;
		realGeometry.x		= width * geometry.x / 100;
		realGeometry.y		= height * geometry.y / 100;
		realGeometry.width	= width * geometry.width / 100;
		realGeometry.height	= height * geometry.height / 100;

		geometry = realGeometry;
		return;
	}
	else if( dwFlag & GEOW_RATIO )
	{
		ClGeometry realGeometry;
		int realWidth = width * geometry.width / 100;

		realGeometry.width	= realWidth;
		realGeometry.height	= realWidth * geometry.height / 100;
		realGeometry.x		= (width - realGeometry.width) / 2;
		realGeometry.y		= (height - realGeometry.height) / 2;

		geometry = realGeometry;
		return;
	}
	else if( dwFlag & GEOH_RATIO )
	{
		ClGeometry realGeometry;
		int realHeight = height * geometry.height / 100;

		realGeometry.width	= realHeight * geometry.width / 100;
		realGeometry.height	= realHeight;
		realGeometry.x		= (width - realGeometry.width) / 2;
		realGeometry.y		= (height - realGeometry.height) / 2;

		geometry = realGeometry;
		return;
	}

	if( dwFlag & GEOX_LEFT )
		geometry.x = preGeometry.x;
	
	if( dwFlag & GEOX_CENTER )
		geometry.x = preGeometry.x + width / 2;

	if( dwFlag & GEOX_RIGHT )
		geometry.x = width - preGeometry.x;

	if( dwFlag & GEOY_TOP )
		geometry.y = preGeometry.y;

	if( dwFlag & GEOY_MIDDLE )
		geometry.y = preGeometry.y + height / 2;

	if( dwFlag & GEOY_BOTTOM )
		geometry.y = height - preGeometry.y;

	if( dwFlag & GEOW_MARGIN )
		geometry.width = width - preGeometry.width;

	if( dwFlag & GEOH_MARGIN )
		geometry.height = height - preGeometry.height;

	if( geometry.width == GEO_MAX )		geometry.width = width;
	if( geometry.height == GEO_MAX )	geometry.height = height;

	if(geometry.width > width)
		geometry.width = width;

	if(geometry.height > height)
		geometry.height = height;
	return;
}

//================================================================================================= E.GEOMETRY
//================================================================================================= S.PATH
std::string ClUtil::PATH::getExtSpec( const char* sourcePath )
{
	if( !sourcePath ) return "";
	std::string stsPath = sourcePath;

	size_t found = stsPath.find_last_of( '.' );
	if( found == std::string::npos )
		return "";

	return stsPath.substr( found + 1, stsPath.length() - found - 1 );
}


std::string ClUtil::PATH::getFileSpec( const char* sourcePath )
{
	if( !sourcePath ) return "";
	std::string stsPath = sourcePath;

	size_t found = stsPath.find_last_of( '\\' );
	if( found == std::string::npos )
		return "";

	return stsPath.substr( found + 1, stsPath.length() - found - 1 );
}


std::string ClUtil::PATH::getFileNameSpec( const char* sourcePath )
{
	if( !sourcePath ) return "";
	std::string stsPath = sourcePath;

	size_t posSlash = stsPath.find_last_of( '\\' );
	if( posSlash == std::string::npos )
		return "";

	size_t posDot = stsPath.find_last_of( '.' );
	if( posDot == std::string::npos )
		return "";

	if( posSlash > (posDot - 1) )
		return "";

	return stsPath.substr( posSlash + 1, posDot - posSlash - 1 );
}


std::string ClUtil::PATH::getPathSpec( const char* sourcePath )
{
	if( !sourcePath ) return "";
	std::string stsPath = sourcePath;

	size_t posSlash = stsPath.find_last_of( '\\' );
	if( posSlash == std::string::npos )
		return "";

	return stsPath.substr( 0, posSlash );
}


std::wstring ClUtil::PATH::getExtSpecW( const WCHAR* sourcePath )
{
	if( !sourcePath ) return TEXT("");
	std::wstring wtsPath = sourcePath;

	size_t found = wtsPath.find_last_of( TEXT('.') );
	if( found == std::string::npos )
		return TEXT("");

	return wtsPath.substr( found + 1, wtsPath.length() - found - 1 );
}


std::wstring ClUtil::PATH::getFileSpecW( const WCHAR* sourcePath )
{
	if( !sourcePath ) return TEXT("");
	std::wstring wtsPath = sourcePath;

	size_t found = wtsPath.find_last_of( L'\\' );
	if( found == std::string::npos )
		return TEXT("");

	return wtsPath.substr( found + 1, wtsPath.length() - found - 1 );
}


std::wstring ClUtil::PATH::getFileNameSpecW( const WCHAR* sourcePath )
{
	if( !sourcePath ) return TEXT("");
	std::wstring wtsPath = sourcePath;

	size_t posSlash = wtsPath.find_last_of( L'\\' );
	if( posSlash == std::string::npos )
		posSlash = 0;
	else
		posSlash++;

	size_t posDot = wtsPath.find_last_of( L'.' );
	if( posDot == std::string::npos )
		return TEXT("");

	if( posSlash >  posDot )
		return TEXT("");

	return wtsPath.substr( posSlash, posDot - posSlash);
}


std::wstring ClUtil::PATH::getPathSpecW( const WCHAR* sourcePath )
{
	if( !sourcePath ) return TEXT("");
	std::wstring wtsPath = sourcePath;

	size_t posSlash = wtsPath.find_last_of( '\\' );
	if( posSlash == std::string::npos )
		return TEXT("");

	return wtsPath.substr( 0, posSlash );
}


bool ClUtil::PATH::isExistFileW( const WCHAR* pFilePath )
{
	DWORD dwAttrib = GetFileAttributes(pFilePath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}


bool ClUtil::PATH::isExistFile( const char* pFilePath )
{
	DWORD dwAttrib = GetFileAttributesA(pFilePath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}


void ClUtil::PATH::makeValidPath( const CString cstrBasePath, CString& cstrValidPath )
{
	CString cstrTemp = cstrBasePath;
	cstrValidPath = TEXT("");

	bool bIgnoreNextToken = false;
	cstrTemp.MakeReverse();
	int nStart = 0;

	CString cstrToken = cstrTemp.Tokenize( TEXT("\\/"), nStart );
	while( cstrToken.GetLength() )
	{
		if( !bIgnoreNextToken )
		{
			if( cstrToken.Compare(TEXT("..")) == 0 )	bIgnoreNextToken = true;
			else 
			{
				if( cstrToken.Compare(TEXT(".")) != 0 )
				{
					cstrValidPath.Append( cstrToken );
					cstrValidPath.Append( _T("\\") );
				}
			}
		}
		else bIgnoreNextToken = false;
		cstrToken = cstrTemp.Tokenize( _T("\\/"), nStart );
	}

	cstrValidPath = cstrValidPath.Left( cstrValidPath.GetLength() - 1 );
	cstrValidPath.MakeReverse();

	return;
}

//================================================================================================= E.PATH
//================================================================================================= S.STRING

char* ClUtil::STRING::upperString( char* pString )
{
	int loop = 0;
	while( pString[loop] != '\0' )
	{
		pString[loop] = (char)toupper( pString[loop] );
		loop++;
	}

	return pString;
}


char* ClUtil::STRING::lowerString( char* pString )
{
	int loop = 0;
	while( pString[loop] != '\0' )
	{
		pString[loop] = (CHAR)tolower( pString[loop] );
		loop++;
	}

	return pString;
}

bool ClUtil::STRING::convertWchar2Char(char* pDest, TCHAR* pSrc, int nDestStrLen)
{
#if 1
	CT2A ascii(pSrc);
	strncpy(pDest, ascii.m_psz, nDestStrLen);
#else	
 #if !defined _DEBUG
   WideCharToMultiByte(CP_ACP, 0, pSrc, nDestStrLen, pDest, nDestStrLen, NULL, NULL);
 #else
    wcstombs(pDest, pSrc, nDestStrLen);
 #endif
#endif
	

	return TRUE;
}

bool ClUtil::STRING::convertChar2Wchar(TCHAR* pDest, char* pSrc, int nDestStrLen)
{
     int nSrcStrLen = 0;
     int nOutputBuffLen = 0;
     int retcode = 0;

     if(pDest == NULL || pSrc == NULL)
     {
          //SysDebug(MID_EXCEPTION, "Char2Wchar: Input Args NULL\n");
          return FALSE;
     }

     nSrcStrLen = strlen(pSrc);
     if(nSrcStrLen == 0 || nSrcStrLen >  nDestStrLen)
     {  
          //SysDebug(MID_EXCEPTION, "Char2Wchar: Strlen zero\n");
          return FALSE;
     }

     nDestStrLen = nSrcStrLen;

     memset(pDest,0,sizeof(TCHAR)*nDestStrLen);
     nOutputBuffLen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pSrc, nSrcStrLen, (LPWSTR)pDest, nDestStrLen);
 
     if (nOutputBuffLen == 0)
     {
          retcode = GetLastError();
          //Sysdebug("Char2Wchar : MultiByteToWideChar returned ERROR \n",0,0);  
          return FALSE;
     }

     pDest[nOutputBuffLen] = '\0';
     return TRUE;
}

bool ClUtil::STRING::convertUTF8ToUnicode( const char* pUTF8, WCHAR** ppUnicode )
{
	if( !pUTF8 || !ppUnicode || *ppUnicode ) return false;

	int nLen = MultiByteToWideChar( CP_UTF8, 0, pUTF8, -1, 0, 0 );
	*ppUnicode = new WCHAR[nLen+1];
	MultiByteToWideChar( CP_UTF8, 0, pUTF8, -1, *ppUnicode, nLen );

	return true;
}


bool ClUtil::STRING::convertUTF8ToCString( const char* pUTF8, CString& cstrTarget )
{
	if( !pUTF8 ) return false;

	int nLen = MultiByteToWideChar( CP_UTF8, 0, pUTF8, -1, 0, 0 );
	WCHAR* lpwzString = new WCHAR[nLen+1];
	MultiByteToWideChar( CP_UTF8, 0, pUTF8, -1, lpwzString, nLen );

	cstrTarget.Format( TEXT("%s"), lpwzString );
	if( lpwzString ) delete [] lpwzString;

	return true;
}


bool ClUtil::STRING::convertUTF8ToHashedString( const char* pUTF8, HashedString& hashedTarget )
{
	if( !pUTF8 ) return false;

	int nLen = MultiByteToWideChar( CP_UTF8, 0, pUTF8, -1, 0, 0 );
	WCHAR* lpwzString = new WCHAR[nLen+1];
	MultiByteToWideChar( CP_UTF8, 0, pUTF8, -1, lpwzString, nLen );

	hashedTarget = lpwzString;
	if( lpwzString ) delete [] lpwzString;

	return true;
}

CString ClUtil::STRING::extractSubString(CString str, CString startKey,  CString endKey )
{
	
    TCHAR *pSourceStr = str.GetBuffer();
	TCHAR * last;

	int pos = str.Find(startKey);

	if(pos >= 0)
		pSourceStr += (pos + startKey.GetLength());
	else
		return TEXT("");

	CString resultStr(pSourceStr);
	pos = resultStr.Find(endKey);

	if(pos)
		resultStr.SetAt(pos, TEXT('\0')); 
	
	return resultStr;
}

//================================================================================================= E.STRING
//================================================================================================= S.CLUDE

bool ClUtil::CLUDE::parseColor( const char* lpszColor, BYTE* pAlpha, BYTE* pRed, BYTE* pGreen, BYTE* pBlue )
{
	if( !lpszColor || !pAlpha || !pRed || !pGreen || !pBlue ) return false;

	int nLen			= 0;
	char* pNewColor		= NULL;
	char* pToken		= NULL;
	const char* pDelim	= "|\r\n, ";
	int color[4]		= {255,255,255,255};
	int nCount			= 0;

	nLen = strlen( lpszColor );
	pNewColor = new char[nLen+1];
	if( !pNewColor ) return false;

	strncpy_s( pNewColor, nLen+1, lpszColor, nLen );
	pNewColor[nLen] = '\0';

	char* lpszNext = NULL;
	pToken = strtok_s( pNewColor, pDelim, &lpszNext );
	while( pToken )
	{
		color[nCount++] = atoi( pToken );
		pToken = strtok_s( NULL, pDelim, &lpszNext );
	}

	if( nCount == 4 )
	{
		*pAlpha	= (BYTE)color[0];
		*pRed	= (BYTE)color[1];
		*pGreen	= (BYTE)color[2];
		*pBlue	= (BYTE)color[3];
	}
	else if( nCount == 3 )
	{
		*pAlpha	= (BYTE)255;
		*pRed	= (BYTE)color[0];
		*pGreen	= (BYTE)color[1];
		*pBlue	= (BYTE)color[2];
	}
	else
	{
		*pAlpha	= (BYTE)255;
		*pRed	= (BYTE)255;
		*pGreen	= (BYTE)255;
		*pBlue	= (BYTE)255;
	}

	if( pNewColor ) delete [] pNewColor;

	return true;
}


bool ClUtil::CLUDE::parseImageDrawMode( const char* lpszMode, INT* pMode )
{
	if( !lpszMode || !pMode )	return false;

	switch( lpszMode[0] )
	{
	case 'b':	if( !strcmp(lpszMode, "buttonface") )	*pMode = 2;	break;	// ClDrawImage::MODE_BUTTONFACE
	case 'p':	if( !strcmp(lpszMode, "pattern") )		*pMode = 1;	break;	// ClDrawImage::MODE_PATTERN
	case 's':	if( !strcmp(lpszMode, "stretch") )		*pMode = 0; break;	// ClDrawImage::MODE_STRETCH
	case 'm':	if( !strcmp(lpszMode, "multiple") )		*pMode = 3; break;	// ClDrawImage::MODE_MULTIPLE
	default:											*pMode = 4; break;	// ClDrawImage::MODE_INVALID
	}

	return true;
}


bool ClUtil::CLUDE::parseTextFormat( const char* lpszFormat, DWORD* pDwFormat )
{
	if( !lpszFormat || !pDwFormat )	return false;

	int nLen			= 0;
	char* pNewFormat		= NULL;
	char* pToken		= NULL;
	const char* pDelim	= "|\r\n, ";

	nLen = strlen( lpszFormat );
	pNewFormat = new char[nLen+1];
	if( !pNewFormat ) return false;

	strncpy_s( pNewFormat, nLen+1, lpszFormat, nLen );
	pNewFormat[nLen] = '\0';

	char* lpszNext = NULL;
	pToken = strtok_s( pNewFormat, pDelim, &lpszNext );
	while( pToken )
	{
		switch( pToken[0] )
		{
		case 'b':	if( !strcmp(pToken, "bottom") )			*pDwFormat |= 1 << 7;
					else if( !strcmp(pToken, "bold") )		*pDwFormat |= 1 << 8;
		case 'c':	if( !strcmp(pToken, "center"))			*pDwFormat |= 1 << 3;	break;
		case 'i':	if( !strcmp(pToken, "italic"))			*pDwFormat |= 1 << 9;	break;
		case 'l':	if( !strcmp(pToken, "left"))			*pDwFormat |= 1 << 2;	break;
		case 'r':	if( !strcmp(pToken, "right"))			*pDwFormat |= 1 << 4;	break;
		case 's':	if( !strcmp(pToken, "strikeout"))		*pDwFormat |= 1 << 11;	break;
		case 't':	if( !strcmp(pToken, "top"))				*pDwFormat |= 1 << 5;	break;
		case 'u':	if( !strcmp(pToken, "underline"))		*pDwFormat |= 1 << 10;	break;
		case 'v':	if( !strcmp(pToken, "vcenter"))			*pDwFormat |= 1 << 6;	break;
		case 'w':	if( !strcmp(pToken, "wordbreak"))		*pDwFormat |= 1 << 0;
					else if( !strcmp(pToken, "wordellipsis"))*pDwFormat |= 1 << 1;	break;
		}

		pToken = strtok_s( NULL, pDelim, &lpszNext );
	}

	if( pNewFormat ) delete [] pNewFormat;

	return true;
}


bool ClUtil::CLUDE::parseButtonSkinType( const char* lpszSkinType, INT* pType )
{
	if( !lpszSkinType || !pType )	return false;

	switch( lpszSkinType[0] )
	{
	case 'c':	if( !strcmp(lpszSkinType, "checked") )		*pType = 1;	else *pType = 7;	break;	// ClButton::SKIN_CHECKED
	case 'd':	if( !strcmp(lpszSkinType, "disabled") )		*pType = 3;	else *pType = 7;	break;	// ClButton::SKIN_DISABLED
	case 'h':	if( !strcmp(lpszSkinType, "hover") )		*pType = 4;								// ClButton::SKIN_HOVER
				else if( !strcmp(lpszSkinType, "hover2") )	*pType = 6;								// ClButton::SKIN_HOVER2
				else *pType = 7;
																							break;
	case 'n':	if( !strcmp(lpszSkinType, "normal") )		*pType = 0;	else *pType = 7;	break;	// ClButton::SKIN_NORMAL
	case 'p':	if( !strcmp(lpszSkinType, "pushed") )		*pType = 2;								// ClButton::SKIN_PUSHED
				else if( !strcmp(lpszSkinType, "pushed2") )	*pType = 5;								// ClButton::SKIN_PUSHED2
				else *pType = 7;
																							break;
	default:	*pType = 7;																			// ClButton::SKIN_INVALID
	}

	return true;
}


bool ClUtil::CLUDE::parseButtonMode( const char* lpszMode, INT* pMode )
{
	if( !lpszMode || !pMode )	return false;

	switch( lpszMode[0] )
	{
	case 'c':	if( !strcmp(lpszMode, "check") )	*pMode = 1; else *pMode = 3;	break;	// ClButton::MODE_CHECK
	case 'i':	if( !strcmp(lpszMode, "indicate") )		*pMode = 2; else *pMode = 3;	break;	// ClButton::MODE_INDICATE
	case 'p':	if( !strcmp(lpszMode, "push") )		*pMode = 0; else *pMode = 3;	break;	// ClButton::MODE_PUSH
	default:										*pMode = 3;						break;	// ClButton::MODE_INVALID
	}

	return true;
}


bool ClUtil::CLUDE::parseSliderSkinType( const char* lpszMode, INT* pMode )
{
	if( !lpszMode || !pMode )	return false;

	switch( lpszMode[0] )
	{
	case 'a':	if( !strcmp(lpszMode, "athumb") )		*pMode = 4; else *pMode = 5;	break;	// ClSlider::SKIN_ATHUMB
	case 'b':	if( !strcmp(lpszMode, "btrack") )		*pMode = 1; else *pMode = 5;	break;	// ClSlider::SKIN_BTRACK
	case 't':	if( !strcmp(lpszMode, "track") )		*pMode = 2;								// ClSlider::SKIN_TRACK
				else if( !strcmp(lpszMode, "thumb") )	*pMode = 3;								// ClSlider::SKIN_THUMB
				else if( !strcmp(lpszMode, "tics") )	*pMode = 0;								// ClSlider::SKIN_TICS
				else									*pMode = 5;						break;	// ClSlider::SKIN_INVALID
	default:											*pMode = 5;						break;	// ClSlider::SKIN_INVALID
	}

	return true;
}


bool ClUtil::CLUDE::parseModeGeoBase( const char* lpszGeobase, DWORD* pDwGeoBase )
{
	if( !lpszGeobase || !pDwGeoBase )	return false;

	int nLen			= 0;
	char* pNewGeoBase	= NULL;
	char* pToken		= NULL;
	const char* pDelim	= "|\r\n, ";

	nLen = strlen( lpszGeobase );
	pNewGeoBase = new char[nLen+1];
	if( !pNewGeoBase ) return false;

	strncpy_s( pNewGeoBase, nLen + 1, lpszGeobase, nLen );
	pNewGeoBase[nLen] = '\0';

	char* lpszNext = NULL;
	pToken = strtok_s( pNewGeoBase, pDelim, &lpszNext );
	while( pToken )
	{
		switch( pToken[0] )
		{
		case 'l':	if( !strcmp(pToken, "left") )	*pDwGeoBase |= GEOX_LEFT;	break;
		case 'c':	if( !strcmp(pToken, "center"))	*pDwGeoBase |= GEOX_CENTER;	break;
		case 'r':	if( !strcmp(pToken, "right"))	*pDwGeoBase |= GEOX_RIGHT;	break;
		case 't':	if( !strcmp(pToken, "top"))		*pDwGeoBase |= GEOY_TOP;	break;
		case 'm':	if( !strcmp(pToken, "middle"))	*pDwGeoBase |= GEOY_MIDDLE;	break;
		case 'b':	if( !strcmp(pToken, "bottom"))	*pDwGeoBase |= GEOY_BOTTOM;	break;
		case 'p':	if( !strcmp(pToken, "percent"))	*pDwGeoBase = GEO_PERCENT;	break;
			case 'w':	
			{
				switch( pToken[1])
				{
				case 'r':	if( !strcmp(pToken, "wratio"))	*pDwGeoBase = GEOW_RATIO;	break;
				case 'm':	if( !strcmp(pToken, "wmargin"))	*pDwGeoBase |= GEOW_MARGIN;	break;
				}
				break;
			}
			case 'h':	
			{
				switch( pToken[1])
				{
				case 'r':	if( !strcmp(pToken, "hratio"))	*pDwGeoBase = GEOH_RATIO;	break;
				case 'm':	if( !strcmp(pToken, "hmargin"))	*pDwGeoBase |= GEOH_MARGIN;	break;
				}
				break;
			}
		}

		pToken = strtok_s( NULL, pDelim, &lpszNext );
	}

	if( pNewGeoBase ) delete [] pNewGeoBase;

	if(		*pDwGeoBase == GEO_PERCENT
		||	*pDwGeoBase == GEOW_RATIO
		||	*pDwGeoBase == GEOH_RATIO )
		return true;

	int nShift;
	int count = 0;

	for( nShift = 0; nShift < 4; nShift ++ ) if( (*pDwGeoBase) & (1 << nShift) ) count++;

	if( count != 1 )
	{
		*pDwGeoBase &= 0xF0;
		*pDwGeoBase |= GEOX_LEFT;
	}

	count = 0;
	for( ; nShift < 8; nShift ++ ) if( (*pDwGeoBase) & (1 << nShift) ) count++;

	if( count != 1 )
	{
		*pDwGeoBase &= 0x0F;
		*pDwGeoBase |= GEOY_TOP;
	}

	return true;
}


bool ClUtil::CLUDE::parseBoolean( const char* lpszBoolean, BOOL* pBoolean )
{
	if( !lpszBoolean || !pBoolean )	return false;

	switch( lpszBoolean[0] )
	{
	case 't':	if( !strcmp(lpszBoolean,"true") )	*pBoolean = true;	break;
	case 'f':	if( !strcmp(lpszBoolean,"false") )	*pBoolean = false;	break;
	}

	return true;
}

//================================================================================================= E.CLUDE
//================================================================================================= EOF

