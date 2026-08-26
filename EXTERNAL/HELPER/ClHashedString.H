//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: HashedString.h
//
//	Description: CLENA Image Pool
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
//================================================================================================= S. CLASS

class HashedString
{
public:
	const WCHAR* c_str( void ) const { return m_Value.c_str(); }
	int length( void ) const { return m_Value.length(); }
	HashedString( void ) : m_Hash(calcHashCode(TEXT(""))) {}
	HashedString( WCHAR* pString ) : m_Value(pString),m_Hash(calcHashCode(pString)) {}
	HashedString( const std::wstring String ) : m_Value(String),m_Hash(calcHashCode(String.c_str())) {}
	HashedString( const HashedString& crHashedString ) :
	m_Value(crHashedString.m_Value),m_Hash(crHashedString.m_Hash),m_IsNULL(crHashedString.m_IsNULL) {}
	const HashedString& operator=(WCHAR* pString)
	{
		m_Value = pString;
		m_Hash  = calcHashCode(pString);
		return *this;
	}
	const HashedString& operator=(const std::wstring String)
	{
		m_Value = String;
		m_Hash  = calcHashCode(String.c_str());
		return *this;
	}
	const HashedString& operator=(const HashedString& crHashedString)
	{
		m_Value = crHashedString.m_Value;
		m_Hash  = crHashedString.m_Hash;
		m_IsNULL = crHashedString.m_IsNULL;
		return *this;
	}
	bool operator==( WCHAR* rhs ) const
	{
		return (m_Value.compare(rhs) == 0);
	}
	bool operator==( const HashedString& rhs ) const
	{
		if(m_Hash!=rhs.m_Hash) return false;
		else				   return (m_Value==rhs.m_Value);
	}
	bool operator!=( WCHAR* rhs ) const
	{
		return (m_Value.compare(rhs) != 0);
	}
	bool operator<( const HashedString& rhs ) const
	{
		return this->m_Value < rhs.m_Value;
	}
	bool operator!=( const HashedString& rhs ) const
	{
		if(m_Hash!=rhs.m_Hash) return true;
		else				   return (m_Value!=rhs.m_Value);
	}
	bool isNULL( void ) const { return m_IsNULL; }
	void clear( void )
	{
		m_Value.clear();
		calcHashCode(TEXT(""));
	}
	unsigned int getHashCode( void ){ return this->m_Hash; }

private:

	unsigned int calcHashCode( const WCHAR* pString )
	{
		unsigned int result = 5381;
		m_IsNULL = ((*pString)==0);
		while( *pString )
		{
			result = ((result<< 5) + result) + (unsigned int)(*pString);
			pString++;
		}
		return result;
	}

private:

	std::wstring	m_Value;
	unsigned int	m_Hash;
	bool			m_IsNULL;
};

//================================================================================================= E. CLASS

