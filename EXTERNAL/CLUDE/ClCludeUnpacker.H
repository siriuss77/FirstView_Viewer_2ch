//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClCludeUnpacker.H
//
//	Description: CLENA CLUDE Unpacker
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
#include <list>

#include <CLUDE/version.h>
#include <HELPER/ClHashedString.H>

//================================================================================================= E. INCLUDE
//================================================================================================= S. PREPROCESSOR
#if (BUILD_MODEL == MODEL_DAVIEW_2CH)
 #if (BUILD_LANGUAGE == LANGUAGE_KOREAN)
 	#define	CLUDE_FILE_NAME												"Daview.skn"
 #else
 	#define	CLUDE_FILE_NAME												"Daview_eng.skn"
 #endif
#elif (BUILD_MODEL == MODEL_DAVIEW_2CH_L)	// jun 170405
 	#define	CLUDE_FILE_NAME												"Daview_eng.skn"
#elif (BUILD_MODEL == MODEL_DAVIEW_2CH_F)	// jun 170406
 	#define	CLUDE_FILE_NAME												"Daview_eng.skn"
#elif (BUILD_MODEL == MODEL_DAVIEW_2CH_S)	// jun 171018
 	#define	CLUDE_FILE_NAME												"Daview_eng.skn"
#elif (BUILD_MODEL == MODEL_BW_2000)
	#define	CLUDE_FILE_NAME												"Blackwing.skn"
#elif (BUILD_MODEL == MODEL_Q_BLACK)
	#define 	CLUDE_FILE_NAME												"QBlack.skn"
#elif (BUILD_MODEL == MODEL_KIC_M7)
	#define CLUDE_FILE_NAME 												"Infobox.skn"
#elif (BUILD_MODEL == MODEL_HONDA_HD_2U)
	#define CLUDE_FILE_NAME												"Honda_hd_2u.skn"
#elif (BUILD_MODEL == MODEL_SAMPLE)
	#define CLUDE_FILE_NAME 												"Sample.skn"
#elif (BUILD_MODEL == MODEL_ITSG_HD_2U)
	#define CLUDE_FILE_NAME 												"itsg_hd_2u.skn"
#elif (BUILD_MODEL == MODEL_TMPD_HD_2U)
	#define CLUDE_FILE_NAME 												"tmpd_hd_2u.skn"
#elif (BUILD_MODEL == MODEL_ICARO_I300HD)
	#define CLUDE_FILE_NAME												"icaro_i300hd.skn"
#elif (BUILD_MODEL == MODEL_VIEW_I_2U)
	#define CLUDE_FILE_NAME												"view_i.skn"
#elif (BUILD_MODEL == MODEL_DOOCO_M7)
	#define CLUDE_FILE_NAME												"InfoboxDooco.skn"
#elif (BUILD_MODEL == MODEL_RALLY_EYES)
	#define CLUDE_FILE_NAME												"RallyEyes.skn"
#elif (BUILD_MODEL == MODEL_EYE_TRACKS)
	#define CLUDE_FILE_NAME												"EyeTracks.skn"
#else
#define	CLUDE_FILE_NAME												"CLUDE.PACK"
#endif
//================================================================================================= E. PREPROCESSOR
//================================================================================================= S. CLASS

class ClCludeUnPacker
{

public:

	ClCludeUnPacker();
	~ClCludeUnPacker();

	//-------------------------------------------------------------------------------------- S. TYPEDEF

	typedef struct _tagHEADER
	{
		CHAR		m_strName[5];		// 'CLUDE'
		INT			m_nVersion;			// 1
		INT			m_nFormat;			// 0 : Normal,  1 : Compressed
		INT			m_nXMLCounter;
		INT			m_nImageCounter;

	} HEADER, *PHEADER;

	typedef struct _tagXMLHEADER
	{
		CHAR		m_strName[MAX_PATH];
		INT			m_nSize;

	} XMLHEADER, *PXMLHEADER;

	typedef std::list<XMLHEADER>						XMLHEADER_POOL;
	typedef XMLHEADER_POOL::iterator					ITER_XMLHEADER;

	typedef struct _tagIMAGEHEADER
	{
		CHAR		m_strName[MAX_PATH];
		INT			m_nSize;

	} IMAGEHEADER, *PIMAGEHEADER;

	typedef std::list<IMAGEHEADER>						IMAGEHEADER_POOL;
	typedef IMAGEHEADER_POOL::iterator					ITER_IMAGEHEADER;

	typedef struct _tagXMLFILE
	{
		std::string		m_stsFileName;
		char*			m_pFileContent;

	} XMLFILE, *PXMLFILE;

	typedef std::list<XMLFILE>							XMLFILE_POOL;
	typedef XMLFILE_POOL::iterator						ITER_XMLFILE;

	typedef struct _tagIMAGEFILE
	{
		std::wstring	m_wtsFilePath;
		INT				m_nSize;
		void*			m_pBuffer;

	} IMAGEFILE, *PIMAGEFILE;

	typedef std::list<IMAGEFILE>						IMAGEFILE_POOL;
	typedef IMAGEFILE_POOL::iterator					ITER_IMAGEFILE;

	//-------------------------------------------------------------------------------------- E. TYPEDEF

public:

		BOOL						init				( const char* lpszCludePath = NULL );
		VOID						deinit				( VOID );


public:

		BOOL						isEmptyXMLFileList	( VOID );
		INT							getXMLFileListSize	( VOID );
		const char*					getXMLFile			( const char* lpszFileName );
		VOID						clearXmlFileList	( VOID );

		BOOL						isEmptyImageFileList( VOID );
		INT							getImageFileListSize( VOID );
		PVOID						getImageFile		( INT nIndex, HashedString& hashedPath, INT* pSize );
		VOID						clearImageFileList	( VOID );

private:

		HEADER						m_CludeHeader;
		XMLHEADER_POOL				m_XMLHeaderList;
		IMAGEHEADER_POOL			m_ImageHeaderList;

		XMLFILE_POOL				m_XMLFileList;
		IMAGEFILE_POOL				m_ImageFileList;

		CString						m_cstrModulePath;
};

//================================================================================================= E. CLASS


