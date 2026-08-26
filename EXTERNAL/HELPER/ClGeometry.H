//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClGeometry.h
//
//	Description: CLENAW Geometry
//
//	Update History
// 
//		2011.10.10 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. FILEID

#pragma once

//================================================================================================= E. FILEID
//================================================================================================= S. INCLUDE

#include <string>

//================================================================================================= E. INCLUDE
//================================================================================================= S. PREPROCESSOR

#define	GEO_VALID_SIZE(GEO)								((GEO.width>0)&&(GEO.height>0))

#define GEO_MAX											0x10000	// 65536

#define GEO_FULL_SIZE									ClGeometry(0,0,GEO_MAX,GEO_MAX)
#define	GEO_ZERO_SIZE									ClGeometry(0,0,0,0)

//================================================================================================= E. PREPROCESSOR
//================================================================================================= S. TYPEDEF

//================================================================================================= E. TYPEDEF
//================================================================================================= S. CLASS

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

	const ClPoint& GetPos( void ) const { return (*((ClPoint*)this));	}
	ClSize GetSize( void ) const { return ClSize(width,height);	}
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
		x		= x < rhs.x ? rhs.x : x;
		y		= y < rhs.y ? rhs.y : y;
		width	= right - x;
		height	= bottom - y;
		x		-= oldX;
		y		-= oldY;

		return *this;
	}

	ClGeometry& operator+=(const ClGeometry& rhs)
	{
		int right = getRight() < rhs.getRight()  ? rhs.getRight()  : getRight();
		int bottom= getBottom()< rhs.getBottom() ? rhs.getBottom() : getBottom();
		x		= x < rhs.x ? x : rhs.x;
		y		= y < rhs.y ? y : rhs.y;
		width	= right - x;
		height	= bottom - y;

		return *this;
	}
};

//================================================================================================= E. CLASS


