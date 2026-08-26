//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClUtil.H
//
//	Description: CLENAW Utility class
//
//	Update History
// 
//		2011.10.10 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. FILEID

#pragma once

//================================================================================================= E. FILEID
//================================================================================================= S. INCLUDE

#include <HELPER/ClHashedString.H>
#include <HELPER/ClGeometry.H>

//================================================================================================= E. INCLUDE
//================================================================================================= S. PREPROCESSOR

//================================================================================================= E. PREPROCESSOR
//================================================================================================= S. CLASS

class ClUtil
{
private: ClUtil(){}; ~ClUtil(){};
public:

	class GEOMETRY		{	private:	GEOMETRY(){}; ~GEOMETRY(){};
							public:		static void	recalcGeometry( ClGeometry& geometry, int width, int height, DWORD dwFlag );

	};

	class PATH			{	private:	PATH(){}; ~PATH(){};
							public:		static std::string getExtSpec( const char* sourcePath );
							public:		static std::string getFileSpec( const char* sourcePath );
							public:		static std::string getFileNameSpec( const char* sourcePath );
							public:		static std::string getPathSpec( const char* sourcePath );
							public:		static std::wstring getExtSpecW( const WCHAR* sourcePath );
							public:		static std::wstring getFileSpecW( const WCHAR* sourcePath );
							public:		static std::wstring getFileNameSpecW( const WCHAR* sourcePath );
							public:		static std::wstring getPathSpecW( const WCHAR* sourcePath );
							public:		static bool isExistFile( const char* pFilePath );
							public:		static bool isExistFileW( const WCHAR* pFilePath );
							public:		static void makeValidPath( const CString cstrBasePath, CString& cstrValidPath );
	};

	class STRING		{	private:	STRING(){}; ~STRING(){};
							public:		static char* upperString( char* pString );
							public:		static char* lowerString( char* pString );
							public:		static bool convertWchar2Char(char* pDest, TCHAR* pSrc, int nDestStrLen);
							public:		static bool convertChar2Wchar(TCHAR* pDest, char* pSrc, int nDestStrLen);
							public:		static bool convertUTF8ToUnicode( const char* pUTF8, WCHAR** ppUnicode );
							public:		static bool convertUTF8ToCString( const char* pUTF8, CString& cstrTarget );
							public:		static bool convertUTF8ToHashedString( const char* pUTF8, HashedString& hashedTarget );
							public:		static CString extractSubString(CString str, CString startKey,  CString endKey );
	};

	class CLUDE			{	private:	CLUDE(){}; ~CLUDE(){};
							public:		static bool parseColor( const char* lpszColor, BYTE* pAlpha, BYTE* pRed, BYTE* pGreen, BYTE* pBlue );
							public:		static bool parseImageDrawMode( const char* lpszMode, INT* pMode );
							public:		static bool parseTextFormat( const char* lpszFormat, DWORD* pDwFormat );
							public:		static bool parseButtonSkinType( const char* lpszSkinType, INT* pType );
							public:		static bool parseButtonMode( const char* lpszMode, INT* pMode );
							public:		static bool parseSliderSkinType( const char* lpszMode, INT* pMode );
							public:		static bool parseModeGeoBase( const char* lpszGeobase, DWORD* pDwGeoBase );
							public:		static bool parseBoolean( const char* lpszBoolean, BOOL* pBoolean );
	};

	class ClMutex		{	private:	CRITICAL_SECTION	m_CS;
							public:		ClMutex(){ InitializeCriticalSection(&m_CS); }
							public:		~ClMutex(){ DeleteCriticalSection(&m_CS); }
							public:		void enter( void ){ EnterCriticalSection(&m_CS); }
							public:		void leave( void ){ LeaveCriticalSection(&m_CS); }
	};

	class ClAutoMutex	{	private:	ClMutex* m_pMutex;
							public:		ClAutoMutex() { m_pMutex = NULL; }
							public:		ClAutoMutex(ClMutex* pMutex):m_pMutex(pMutex){ if(m_pMutex != NULL) m_pMutex->enter(); }
							public:		~ClAutoMutex(){ leave(); }
							public:		void leave(){ if(m_pMutex!=NULL) {m_pMutex->leave(); m_pMutex=NULL;} }
	};
};

//================================================================================================= E. CLASS


