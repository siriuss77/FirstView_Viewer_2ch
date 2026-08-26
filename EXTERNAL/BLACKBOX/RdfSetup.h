/**************************************************************************************************
 *
 *      File Name       :   RdfSetup.h
 *      Description     :  rdf_setup.cfg file parser
 *
 *      Creator         :   tony ( icaro Co., Ltd )
 *      Create Date     :   2013/10/01
 *      Update History  :   
 *
 *************************************************************************************************/
#ifndef __RDFSETUP_H__
#define __RDFSETUP_H__
 
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                INCLUDE
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "windows.h"
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                Definition

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

#ifndef u8
typedef unsigned char     u8;
#endif /* u8 */
#ifndef u16
typedef unsigned short    u16;
#endif /* u16 */
#ifndef u32
typedef unsigned int      u32;
#endif /* u32 */

#ifndef __func__
#define __func__			__FUNCTION__
#endif //__func__
 

#define RDF_SETUP_FILE_NAME L"rdf_setup.cfg"
#define RDF_DEFAULT_HEADER	"R001"

#define RDF_HEADER_STRING_SIZE		4

typedef struct 
{
	double x;
	double y;
	double z;
}ST_G,*LPST_G;

typedef struct 
{
	double f_r; // front and rear
	double l_r;	// left and right
	double u_d;	// up and down

	u32		speed; // km/h
}ST_EVENT_LEVEL,*LPST_G_EVENT_LEVEL;

typedef struct tagST_RDFCFG
{
	char 			header[RDF_HEADER_STRING_SIZE]; 	//헤더 (고정 문자열 (ASCII)) 
	u32 			vehicle_id;
	u32				driver_id;

	ST_G			g_offset;
	ST_EVENT_LEVEL 	evt_level;
} ST_RDFCFG, *LPST_RDFCFG;
//----------------------------------------------------------------------------


#define RDF_CONFIG_FOLDER		_T("Config\\")
#define RDF_CONFIG_FILE_NAME		_T("rdf_setup.cfg")

#define RDF_CONFIG_FILE		(RDF_CONFIG_FOLDER RDF_CONFIG_FILE_NAME)

class CRdfSetup  
{
public:

	CRdfSetup(void);
	virtual ~CRdfSetup(void);

	static BOOL Load(LPST_RDFCFG spConfig);
	static BOOL Load(CString cfg_file_name, LPST_RDFCFG spConfig);
	static CString Save(LPST_RDFCFG spConfig);
	static CString Save(CString cfg_file_dir, LPST_RDFCFG spConfig);
	static BOOL DefaultSet(LPST_RDFCFG pCfg);
protected:
	static BOOL PrintfToFile(CFile *fhandle, char *fmt, ... );
	static BOOL ParserFile(CString cfg_file_name, LPST_RDFCFG spConfig);
};


#endif // __RDFSETUP_H__

