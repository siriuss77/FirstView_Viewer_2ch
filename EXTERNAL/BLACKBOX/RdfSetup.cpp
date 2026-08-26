/**************************************************************************************************
 *
 *      File Name       :   RdfSetup.c
 *      Description     :  rdf_setup.cfg file parser
 *
 *      Creator         :   tony ( icaro Co., Ltd )
 *      Create Date     :   2013/10/01
 *      Update History  :   
 *
 *************************************************************************************************/

#include "stdafx.h"
#include <CLUDE/version.h>
#include <HELPER/CLUtil.H>
#include "TParser.h"
#include "BBoxUtil.h"
#include "RdfSetup.h"

 
#define ZONE_CFG_PRINT	1
#define ZONE_CFG_FUNC	1
#define ZONE_CFG_ERROR	1

#define fabs(x) ((x) < 0 ?(-(x)) : (x))

CRdfSetup::CRdfSetup(void)
{
}

CRdfSetup::~CRdfSetup(void)
{
}

BOOL CRdfSetup::Load(LPST_RDFCFG spConfig)
{
	return Load(CString(RDF_CONFIG_FILE), spConfig);
}

BOOL CRdfSetup::Load(CString cfg_file_name, LPST_RDFCFG spConfig)
{
	CString fileName;

	if(cfg_file_name.Find(TEXT(":\\")) < 0)
	{
		fileName = CBBoxUtil::GetModuleDir();
		fileName.Append(TEXT("\\"));
	}
	
	fileName.Append(cfg_file_name);
		
	return ParserFile(fileName, spConfig);
}

BOOL CRdfSetup::PrintfToFile(CFile *fhandle, char *fmt, ... )
{
	BOOL bRet = FALSE;
   	va_list argP;
   	char	string[255];
   
   	va_start(argP, fmt);
   	vsprintf(string, fmt, argP);
   
	if(fhandle)
	{
		fhandle->Write(string, strlen(string));
		bRet = TRUE;
	}
	
	va_end(argP);

	return bRet;
}

CString CRdfSetup::Save(LPST_RDFCFG spConfig)
{
	return Save(CBBoxUtil::GetModuleDir(), spConfig);
}

CString CRdfSetup::Save(CString cfg_file_dir, LPST_RDFCFG spConfig)
{
	CFile file;
	int ch,i;
	int ret;

	CFile *fhandle = NULL;
	CFileFind finder;
	CString fileName;

	fileName = cfg_file_dir;
	if(fileName.Find(_T("Config")) >= 0)
	{
		fileName.Append(_T("\\"));
		fileName.Append(RDF_CONFIG_FILE_NAME);
	}
	else
	{
		fileName.Append(_T("\\"));
		fileName.Append(RDF_CONFIG_FILE);
	}
	
	if(!finder.FindFile(fileName.GetBuffer()))
		CreateDirectory(CBBoxUtil::GetDir(fileName), NULL);
	//else
		//DeleteFile(fileName.GetBuffer());

	finder.Close();
	
	if(!file.Open(fileName.GetBuffer(), CFile::modeCreate | CFile::modeReadWrite))
	{
		DEBUGMSG(ZONE_CFG_ERROR, (" Rdf Config File OPEN Error!!!\n"));
		return FALSE;
	}

	fhandle = &file;

	// 0. Info
	PrintfToFile(fhandle, "\r\n[Info]\r\n");
	PrintfToFile(fhandle, "PC_Update          = %d\r\n", 1);

	// 1. Header
	PrintfToFile(fhandle, "\r\n[Header]\r\n");
	PrintfToFile(fhandle, "Header             = ");
	for(i = 0; i < RDF_HEADER_STRING_SIZE; i++) {
		PrintfToFile(fhandle, "%c", spConfig->header[i]);
	}
	PrintfToFile(fhandle, " ; ASCII 4 Character\r\n");

	PrintfToFile(fhandle, "Vehicle_Id         = %d ; 0 ~ 999999999\r\n", spConfig->vehicle_id);
	PrintfToFile(fhandle, "Driver_Id          = %d ; 0 ~ 999999999\r\n", spConfig->driver_id);

	// 2. G-Sensor Offset
	PrintfToFile(fhandle, "\r\n[G_Offset]\r\n");

	PrintfToFile(fhandle, "X                  = %0.2f ; Unit : g\r\n", spConfig->g_offset.x);
	PrintfToFile(fhandle, "Y                  = %0.2f ; Unit : g\r\n", spConfig->g_offset.y);
	PrintfToFile(fhandle, "Z                  = %0.2f ; Unit : g\r\n", spConfig->g_offset.z);

	// 3. Event Level
	PrintfToFile(fhandle, "\r\n[G_Event]\r\n");
	PrintfToFile(fhandle, "Trigger_Level_x    = %0.2f ; Unit : g\r\n", spConfig->evt_level.f_r);
	PrintfToFile(fhandle, "Trigger_Level_y    = %0.2f ; Unit : g\r\n", spConfig->evt_level.l_r);
	PrintfToFile(fhandle, "Trigger_Level_z    = %0.2f ; Unit : g\r\n", spConfig->evt_level.u_d);

	PrintfToFile(fhandle, "\r\n[Speed_Event]\r\n");
	PrintfToFile(fhandle, "Event_Speed        = %d ; Unit : Km/h\r\n", spConfig->evt_level.speed);

	
	file.Close();

	DEBUGMSG(ZONE_CFG_FUNC, ("--%s()\n", __func__));
	return fileName;
}

BOOL CRdfSetup::ParserFile(CString cfg_file_name, LPST_RDFCFG spConfig)
{
	DICTIONARY	*p_CfgDt;
	CTParser	cfgTPar;
	ST_RDFCFG stCfgDefault;
	char szKey[OBJECT_LEN] = {0};
	int size;
	
	/* Some temporary variables to hold query results */
	UINT32		b;
	UINT32		i;
	double		d;
	char*		c;

	#define TextParserGetStr	cfgTPar.GetStr
	#define TextParserGetInt	cfgTPar.GetInt
	#define TextParserGetDouble cfgTPar.GetDouble
	#define TextParserDelDict cfgTPar.DelDict
	
	p_CfgDt = cfgTPar.Load(cfg_file_name.GetBuffer());

	if(!p_CfgDt) {
		DEBUGMSG(ZONE_CFG_PRINT, ("RdfCfgParserFile(): Load ERROR!!\n"));
		return FALSE;
	}

	DefaultSet(&stCfgDefault);

// 1. Header
	DEBUGMSG(ZONE_CFG_PRINT, ("\n*******	Header   ******\n"));
	c = TextParserGetStr(p_CfgDt, "header:Header", stCfgDefault.header);
	size = strlen(c);
	if(size>RDF_HEADER_STRING_SIZE)
		size = RDF_HEADER_STRING_SIZE;
	strncpy(spConfig->header, c, size);
	DEBUGMSG(ZONE_CFG_PRINT, ("Header:				[%s]\n", c));	 

	i = TextParserGetInt(p_CfgDt, "header:vehicle_id", stCfgDefault.vehicle_id);
	spConfig->vehicle_id = i;
	DEBUGMSG(ZONE_CFG_PRINT, ("Vehicle_Id:				[%d]\n", i)); 

	i = TextParserGetInt(p_CfgDt, "header:driver_id", stCfgDefault.driver_id);
	spConfig->driver_id = i;
	DEBUGMSG(ZONE_CFG_PRINT, ("Driver_Id: 			    [%d]\n", i)); 
	
	// 2. G_Offset
	DEBUGMSG(ZONE_CFG_PRINT, ("\n*******	G Offset   ******\n"));
	d = TextParserGetDouble(p_CfgDt, "g_offset:x", stCfgDefault.g_offset.x);
	spConfig->g_offset.x = d;
	DEBUGMSG(ZONE_CFG_PRINT, ("G_Offset x:				 [%d.%02d]\n", (int)d, ((int)(fabs(d)*100)%100)));
		
	d = TextParserGetDouble(p_CfgDt, "g_offset:y", stCfgDefault.g_offset.y);
	spConfig->g_offset.y = d;
	DEBUGMSG(ZONE_CFG_PRINT, ("G_Offset y:				 [%d.%02d]\n", (int)d, ((int)(fabs(d)*100)%100)));

	d = TextParserGetDouble(p_CfgDt, "g_offset:z", stCfgDefault.g_offset.z);
	spConfig->g_offset.z = d;
	DEBUGMSG(ZONE_CFG_PRINT, ("G_Offset z:				 [%d.%02d]\n", (int)d, ((int)(fabs(d)*100)%100)));
	
	// 3. G_Event
	DEBUGMSG(ZONE_CFG_PRINT, ("\n*******	G Event  ******\n"));
	d = TextParserGetDouble(p_CfgDt, "g_event:Trigger_Level_x", stCfgDefault.evt_level.f_r);
	spConfig->evt_level.f_r = d;
	DEBUGMSG(ZONE_CFG_PRINT, ("Trigger_Leves_x: 		[%d.%02d]\n", (int)d, ((int)(fabs(d)*100)%100)));

	d = TextParserGetDouble(p_CfgDt, "g_event:Trigger_Level_y", stCfgDefault.evt_level.l_r);
	spConfig->evt_level.l_r = d;
	DEBUGMSG(ZONE_CFG_PRINT, ("Trigger_Leves_y: 		[%d.%02d]\n", (int)d, ((int)(fabs(d)*100)%100)));

	
	d = TextParserGetDouble(p_CfgDt, "g_event:Trigger_Level_z", stCfgDefault.evt_level.u_d);
	spConfig->evt_level.u_d = d;
	DEBUGMSG(ZONE_CFG_PRINT, ("Trigger_Leves_z: 		[%d.%02d]\n", (int)d, ((int)(fabs(d)*100)%100)));


	// 4. Speed_Event
	DEBUGMSG(ZONE_CFG_PRINT, ("\n*******	Speed Event  ******\n"));
	i = TextParserGetInt(p_CfgDt, "speed_event:event_speed", stCfgDefault.evt_level.speed);
	spConfig->evt_level.speed = i;
	DEBUGMSG(ZONE_CFG_PRINT, ("Event_Speed: 		    [%d]\n", (int)i));

	TextParserDelDict(p_CfgDt);

	return TRUE;

}

BOOL CRdfSetup::DefaultSet(LPST_RDFCFG pCfg)
{
	int ch;
	//header
	strncpy(pCfg->header, RDF_DEFAULT_HEADER, RDF_HEADER_STRING_SIZE);
	pCfg->vehicle_id 		= 999999999;
	pCfg->driver_id			= 999999999;
	
	pCfg->g_offset.x		= 0.0;
	pCfg->g_offset.y		= 0.0;
	pCfg->g_offset.z		= 0.0;

	pCfg->evt_level.f_r		= 0.35;
	pCfg->evt_level.l_r		= 0.35;
	pCfg->evt_level.u_d		= 0.55;
	pCfg->evt_level.speed	= 120;
	
return TRUE;
}


