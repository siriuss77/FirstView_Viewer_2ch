//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClLog.h
//
//	Description: CLENA Log class
//
//	Update History
// 
//		2011.10.10 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. FILEID

#pragma once

//================================================================================================= E. FILEID
//================================================================================================= S. INCLUDE

#include <stdio.h>

//================================================================================================= E. INCLUDE
//================================================================================================= S. PREPROCESSOR

#	define	DF_REPORT		1
#	define	DF_WARNING		1
#	define	DF_ERROR		1
#	define	DF_FATAL		1

#	define	NOTHING

#ifdef _DEBUG

#define __W(x)											L##x
#define _W(x)											__W(x)
#define __WFUNCTION__									_W(__FUNCTION__)

#	define	CLD( DFLAG, STR_MSG, ... )												\
	do{	if(DFLAG)																	\
		printf( STR_MSG"\n",##__VA_ARGS__);											\
	} while(0)

#	define	CLD_FL( DFLAG, STR_MSG, ... )											\
	do{	if(DFLAG)																	\
		printf( "%s(%d) "STR_MSG"\n",__FUNCTION__,__LINE__,##__VA_ARGS__);			\
	} while(0)

#	define	CLDW( DFLAG, WSTR_MSG, ... )											\
	do{	if(DFLAG)																	\
		wprintf( WSTR_MSG TEXT("\n"),##__VA_ARGS__);								\
	} while(0)

#	define	CLDW_FL( DFLAG, WSTR_MSG, ... )											\
	do{	if(DFLAG)																	\
		wprintf( TEXT("%s(%d) ")WSTR_MSG TEXT("\n"),__WFUNCTION__,__LINE__,##__VA_ARGS__);	\
	} while(0)

#else

#	define	CLD( DFLAG, STR_MSG, ... )					NOTHING
#	define	CLD_FL( DFLAG, STR_MSG, ... )				NOTHING
#	define	CLDW( DFLAG, WSTR_MSG, ... )				NOTHING
#	define	CLDW_FL( DFLAG, WSTR_MSG, ... )				NOTHING

#endif

//================================================================================================= E. PREPROCESSOR
//================================================================================================= S. TYPEDEF

//================================================================================================= E. TYPEDEF

