//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//  Module: wbcUtil.h
//
//  Description: Viewer Utils
//
//  Update History
// 
//      2014.10.22 [Allen Han] Initial Release v1.0.00
//=================================================================================================
#pragma once
//================================================================================================= S. INCLUDE

#include <Windows.h>
#include <string>
#include <HELPER/ClGeometry.h>

//================================================================================================= E. INCLUDE
//================================================================================================= S. PREPROCESSOR

#define IS_DLG_STATE(FLAG)                          !!(this->m_dwDlgState & (FLAG))
#define SET_DLG_STATE(FLAG)                         (this->m_dwDlgState |= (FLAG))
#define RESET_DLG_STATE(FLAG)                       (this->m_dwDlgState ^= (FLAG))

#define MAKE_LSTRING_KEY(HVALUE,LID)                (((((KEYTYPE)HVALUE)<<32))|LID)
#define GET_LSKEY_HVALUE(U64KEY)                    ((unsigned long)(U64KEY))
#define GET_LSKEY_LID(U64KEY)                       ((unsigned long)(((U64KEY)>>32) & 0xffffffff))

#define GEO_VALID_SIZE(GEO)                         ((GEO.width>0)&&(GEO.height>0))

#define GEO_MAX                                     0x10000 // 65536

#define GEO_FULL_SIZE                               ClGeometry(0,0,GEO_MAX,GEO_MAX)
#define GEO_ZERO_SIZE                               ClGeometry(0,0,0,0)

//================================================================================================= E. PREPROCESSOR
//================================================================================================= S. TYPEDEF

class ClMutex       {   private:    CRITICAL_SECTION    m_CS;
                        public:     ClMutex(){ InitializeCriticalSection(&m_CS); }
                        public:     ~ClMutex(){ DeleteCriticalSection(&m_CS); }
                        public:     void enter( void ){ EnterCriticalSection(&m_CS); }
                        public:     void leave( void ){ LeaveCriticalSection(&m_CS); }
                    };

class ClAutoMutex   {   private:    ClMutex* m_pMutex;
                        public:     ClAutoMutex() { m_pMutex = NULL; }
                        public:     ClAutoMutex(ClMutex* pMutex):m_pMutex(pMutex){ if(m_pMutex) m_pMutex->enter(); }
                        public:     ~ClAutoMutex(){ leave(); }
                        public:     void leave(){ if(m_pMutex!=NULL) {m_pMutex->leave(); m_pMutex=NULL;} }
                    };

typedef unsigned __int64 KEYTYPE;
class HashedLID
{
public:
    const char* c_str( void ) const { return m_stsLSID.c_str(); }
    int length( void ) const { return m_stsLSID.length(); }
    HashedLID( void ) : m_key64(calcHashCode("",0)) {}
    HashedLID( const char* pString, unsigned int LID ) : m_stsLSID(pString),m_key64(calcHashCode(pString, LID)) {}
    HashedLID( const std::string stsString, unsigned int LID ) : m_stsLSID(stsString),m_key64(calcHashCode(stsString.c_str(),LID)) {}
    HashedLID( const HashedLID& crHashedString ) : m_stsLSID(crHashedString.m_stsLSID),m_key64(crHashedString.m_key64),m_bIsNULL(crHashedString.m_bIsNULL) {}
    const HashedLID& operator=(const HashedLID& crHashedString)
    {
        m_stsLSID   = crHashedString.m_stsLSID;
        m_key64     = crHashedString.m_key64;
        m_bIsNULL   = crHashedString.m_bIsNULL;
        return *this;
    }
    bool operator==( const HashedLID& rhs ) const
    {
        if(m_key64!=rhs.m_key64)    return false;
        else                        return (m_stsLSID==rhs.m_stsLSID);
    }
    bool operator<( const HashedLID& rhs ) const { return m_key64 < rhs.m_key64; }
    bool operator!=( const HashedLID& rhs ) const
    {
        if(m_key64!=rhs.m_key64)    return true;
        else                        return (m_stsLSID!=rhs.m_stsLSID);
    }
    bool isNULL( void ) const { return m_bIsNULL; }
    void clear( void )
    {
        m_stsLSID.clear();
        calcHashCode( "", 0 );
    }
    unsigned int getHashCode( void ){ return GET_LSKEY_HVALUE(m_key64); }
    unsigned int getLanguage(void){ return GET_LSKEY_LID(m_key64); }

private:
    KEYTYPE calcHashCode( const char* pString, unsigned int LID )
    {
        KEYTYPE key;
        unsigned int result = 5381;
        m_bIsNULL = ((*pString)==0);
        while( *pString )
        {
            result = ((result<< 5) + result) + (unsigned int)(*pString);
            pString++;
        }

        key = MAKE_LSTRING_KEY( result, LID );

        return key;
    }

private:
    std::string     m_stsLSID;
    KEYTYPE         m_key64;
    bool            m_bIsNULL;
};

#if 0
struct ClPoint
{
    int x;
    int y;

    ClPoint( void ){}
    ClPoint( int X, int Y ) : x(X), y(Y) {}
    ClPoint( const ClPoint& src ) : x(src.x), y(src.y) {}
    bool operator==(const ClPoint& rhs) const
    {
        return (x == rhs.x && y == rhs.y);
    }
    bool operator!=(const ClPoint& rhs) const
    { 
        return (x != rhs.x || y != rhs.y);
    }
};

struct ClSize
{
    int cx;
    int cy;

    ClSize( void ){}
    ClSize( int CX, int CY ) : cx(CX), cy(CY) {}
    ClSize( const ClSize& src ) : cx(src.cx), cy(src.cy) {}
    bool operator==(const ClSize& rhs) const
    {
        return (cx == rhs.cx && cy == rhs.cy);
    }
    bool operator!=(const ClSize& rhs) const
    { 
        return (cx != rhs.cx || cy != rhs.cy);
    }
};

struct ClGeometry
{
    int x;
    int y;
    int width;
    int height;

    ClGeometry( void ){}
    ClGeometry( int L, int T, int W, int H ) : x(L), y(T), width(W), height(H) {}
    ClGeometry( const ClGeometry& src ) : x(src.x), y(src.y), width(src.width), height(src.height) {}

    bool isInGeometry( const ClPoint& Point ) const
    {
        return ((x<Point.x)&&(getRight()>Point.x)&&(y<Point.y)&&(getBottom()>Point.y));
    }

    bool isInGeometry( const ClGeometry& rhs ) const
    {
        return ((x<rhs.x)&&(getRight()>rhs.getRight())&&(y<rhs.y)&&(getBottom()>rhs.getBottom()));
    }

    bool isIntersect( const ClGeometry& rhs ) const
    {
        return ((x<rhs.width)&&(width>rhs.x)&&(y<rhs.height)&&(height>rhs.y));
    }

    const ClPoint& GetPos( void ) const { return (*((ClPoint*)this));}
    ClSize GetSize( void ) const { return ClSize(width,height);}
    int getRight ( void ) const  { return x + width; }
    int getBottom( void ) const  { return y + height; }

    bool operator==(const ClGeometry& rhs) const
    {
        return ((x==rhs.x)&&(y==rhs.y)&&(width==rhs.width)&&(height==rhs.height));
    }

    bool operator!=(const ClGeometry& rhs) const
    { 
        return ((x!=rhs.x)||(y!=rhs.y)||(width!=rhs.width)||(height != rhs.height));
    }

    ClGeometry& operator-=(const ClGeometry& rhs)
    {
        int oldX = x;
        int oldY = y;

        int right = getRight() < rhs.getRight()  ? getRight()  : rhs.getRight();
        int bottom = getBottom()< rhs.getBottom() ? getBottom() : rhs.getBottom();
        x       = x < rhs.x ? rhs.x : x;
        y       = y < rhs.y ? rhs.y : y;
        width   = right - x;
        height  = bottom - y;
        x       -= oldX;
        y       -= oldY;

        return *this;
    }

    ClGeometry& operator+=(const ClGeometry& rhs)
    {
        int right = getRight() < rhs.getRight()  ? rhs.getRight()  : getRight();
        int bottom= getBottom()< rhs.getBottom() ? rhs.getBottom() : getBottom();
        x       = x < rhs.x ? x : rhs.x;
        y       = y < rhs.y ? y : rhs.y;
        width   = right - x;
        height  = bottom - y;

        return *this;
    }
};
#endif

enum _eGEOMETRYMAN
{
    GEOMAN_LEFT     = 1 << 0,   // x
    GEOMAN_RIGHT    = 1 << 1,   // x
    GEOMAN_HCENTER  = 1 << 2,   // x
    GEOMAN_TOP      = 1 << 3,   // y
    GEOMAN_BOTTOM   = 1 << 4,   // y
    GEOMAN_VCENTER  = 1 << 5,   // y

    GEOMAN_WEB      = 1 << 29,  // Special case for web control
    GEOMAN_IGNORE   = 1 << 30,  // No geometry control...
};

//================================================================================================= E. TYPEDEF
//================================================================================================= S. UTILS

void vwutil_manGeometry( const ClGeometry& geoSource, DWORD dwMan, int width, int height, ClGeometry& geoMan );

//================================================================================================= E. UTILS
