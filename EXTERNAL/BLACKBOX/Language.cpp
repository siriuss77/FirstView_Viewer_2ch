
// AkLanguage.cpp: Make for CAkSkin class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include <HELPER/CLUtil.H>

#include "Language.h"
#include "BBPlayer.h"

#define MAXKEYLENGTH	64
#define FIND_LANG_DIRECTORY	TEXT("\\language\\")
#define FIND_LANG_FORMAT 		TEXT("\\language\\*.ini")

typedef struct
{
    TCHAR text[MAXKEYLENGTH];
} ST_KEY_TEXT, *PST_KEY_TEXT;

ST_KEY_TEXT ucLanguageKey[]= {
/*INFO_LANGUAGE*/					{TEXT("info:language")},
/*INFO_GLOBALFONTNAME*/		{TEXT("info:globalfontname")},
/*INFO_LANG_VER*/					{TEXT("info:langver")},

/*TEXT_LANGUAGE*/					{TEXT("text:language")},
/*TEXT_VERSION*/					{TEXT("text:version")},
/*TEXT_TYPE*/							{TEXT("text:type")},
/*TEXT_DATE*/							{TEXT("text:date")},
/*TEXT_RECORDINGTIME*/		{TEXT("text:recordingtime")},
/*TEXT_FILENAME*/					{TEXT("text:filename")},
/*TEXT_FILELIST*/					{TEXT("text:filelist")},
/*TEXT_PLAYLIST*/					{TEXT("text:playlist")},
/*TEXT_INFO*/							{TEXT("text:info")},
/*TEXT_INFOVERSION*/							{TEXT("text:infoversion")},
/*TEXT_PASSWORD*/					{TEXT("text:password")},
/*TEXT_NEWPASSWORD*/			{TEXT("text:newpassword")},
/*TEXT_VERIFYPASSWORD*/		{TEXT("text:verifypassword")},
/*TEXT_CAPTURECOMPLETE*/	{TEXT("text:capturecomplete")},
/*TEXT_FILESAVECOMPLETE*/	{TEXT("text:filesavecomplete")},
/*TEXT_WRONGPASSWORD*/		{TEXT("text:wrongpassword")},
/*TEXT_INTERNETDISCONNECT*/	{TEXT("text:internetdisconnect")},
/*TEXT_CAPTURE*/					{TEXT("text:capture")},
/*TEXT_FILE*/							{TEXT("text:file")},
/*TEXT_SAVE*/							{TEXT("text:save")},
/*TEXT_SAVEDIRECTORY*/		{TEXT("text:savedirectory")},
/*TEXT_SETTING*/					{TEXT("text:setting")},
/*TEXT_CAPTURESCREEN*/		{TEXT("text:capturescreen")},
                        	
/*TEXT_ACCIDENTSENS*/			{TEXT("text:accidentsensitivity")},
/*TEXT_ACCIDENTCYLINDERS*/		{TEXT("text:accidentcylinder")},  // jun 20201202
/*TEXT_CYLINDERSLOW*/		{TEXT("text:cylinderslow")},  // jun 20201202
/*TEXT_CYLINDERSHIGH*/		{TEXT("text:cylindershigh")},  // jun 20201202
/*TEXT_SENSITIVE*/				{TEXT("text:sensitive")},
/*TEXT_SENSIBILITY*/			{TEXT("text:sensibility")},
/*TEXT_NORMALITY*/				{TEXT("text:normality")},
/*TEXT_INSENSIBILITY*/		{TEXT("text:insensibility")},
/*TEXT_ANNOUNCEMENT*/			{TEXT("text:announcement")},
/*TEXT_TIMEZONE*/					{TEXT("text:timezone")},
/*TEXT_UNITOFSPEED*/			{TEXT("text:unitofspeed")},
/*TEXT_NUMBEROFVEHICLE*/	{TEXT("text:numberofvehicle")},
/*TEXT_RECORDER*/					{TEXT("text:recorder")},
/*TEXT_RECORDING*/				{TEXT("text:recording")},
/*TEXT_CHANNEL*/					{TEXT("text:channel")},
/*TEXT_MODE*/							{TEXT("text:mode")},
/*TEXT_RESOLUTION*/				{TEXT("text:resolution")},
/*TEXT_QUALITY*/					{TEXT("text:quality")},
/*TEXT_AUDIO*/						{TEXT("text:audio")},
/*TEXT_EVENTFILEOVERWRITE*/	{TEXT("text:eventfileoverwrite")},
/*TEXT_MOTIONDETECT*/			{TEXT("text:motiondetect")},
/*TEXT_MEMORY*/						{TEXT("text:memory")},
/*TEXT_AREA*/							{TEXT("text:area")},
/*TEXT_FULL*/							{TEXT("text:full")},
/*TEXT_VIDEO*/						{TEXT("text:video")},
/*TEXT_OUTPUT*/						{TEXT("text:output")},
/*TEXT_TIMESETTING*/			{TEXT("text:timesetting")},
/*TEXT_PULSERESET*/			    {TEXT("text:pulsereset")},
/*TEXT_BRAKE*/					{TEXT("text:brake")},
/*TEXT_INPUT1*/					{TEXT("text:input1")},
/*TEXT_INPUT2*/					{TEXT("text:input2")},
/*TEXT_CARNUMBER*/					{TEXT("text:carnumber")},   // jun 200805
/*TEXT_DRIVERCODE*/					{TEXT("text:drivercode")},   // jun 221017
/*TEXT_OSDSPEED*/					{TEXT("text:osdspeed")},   // jun 200813

/*TEXT_YEAR*/							{TEXT("text:year")},
/*TEXT_MONTH*/						{TEXT("text:month")},
/*TEXT_HOUR*/							{TEXT("text:hour")},
/*TEXT_MINUTE*/						{TEXT("text:minute")},
/*TEXT_SEC*/							{TEXT("text:sec")},
/*TEXT_SETUP*/						{TEXT("text:setup")},
/*TEXT_DISPLAY*/					{TEXT("text:display")},
/*TEXT_DEVICE*/						{TEXT("text:device")},
/*TEXT_SYSTEM*/ 					{TEXT("text:system")},
/*TEXT_SELECTALL*/				{TEXT("text:selectall")},
/*TEXT_EMERGENCY*/				{TEXT("text:emergency")},
/*TEXT_SHOCK*/						{TEXT("text:shock")},
/*TEXT_MOTION*/						{TEXT("text:motion")},
/*TEXT_INFINITY*/					{TEXT("text:infinity")},
/*TEXT_EVENT*/						{TEXT("text:event")},
/*TEXT_VOLUME*/						{TEXT("text:volume")},
/*TEXT_PLAYSPEED*/				{TEXT("text:playspeed")},

/*TEXT_KOREAN*/						{TEXT("text:korean")},
/*TEXT_ENGLISH*/					{TEXT("text:english")},
/*TEXT_JAPANESE*/					{TEXT("text:Japanese")},
/*TEXT_FRONTCAMERA*/			{TEXT("text:frontcamera")},
/*TEXT_REARCAMERA*/				{TEXT("text:rearcamera")},
/*TEXT_BLACKBOX*/					{TEXT("text:blackbox")},
/*TEXT_NOTCONNECTED*/			{TEXT("text:notconnected")},
/*TEXT_OFFSET*/						{TEXT("text:offset")},
/*TEXT_OFF*/							{TEXT("text:off")},
/*TEXT_ON*/								{TEXT("text:on")},
/*TEXT_AUTO*/							{TEXT("text:auto")},
/*TEXT_COMPRESSED*/				{TEXT("text:compressed")},
/*TEXT_UNIT*/							{TEXT("text:unit")},
/*TEXT_FLIP*/							{TEXT("text:flip")},
/*TEXT_MIRROR*/						{TEXT("text:mirror")},
/*TEXT_RECORDING_ALARM*/	{TEXT("text:recordingalarm")},
/*TEXT_VOICE_GUIDANCE*/		{TEXT("text:voiceguidance")},
/*TEXT_BUZZER*/						{TEXT("text:buzzer")},
/*TEXT_BUTTONSOUND*/			{TEXT("text:buttonsound")},
/*TEXT_EVENTSOUND*/				{TEXT("text:eventsound")},
/*TEXT_LOWVOLTAGE*/				{TEXT("text:lowvoltage")},
/*TEXT_BATTERY*/					{TEXT("text:battery")},
/*TEXT_PARKING*/					{TEXT("text:parking")},
/*TEXT_SDFORMAT*/ 				{TEXT("text:sdformat")},
/*TEXT_UPDATE*/						{TEXT("text:update")},
/*TEXT_SETDEFAULT*/				{TEXT("text:setdefault")},

/*TEXT_REALLYFORMAT*/			{TEXT("text:reallyformat")},
/*TEXT_ALLCONFIGRESET*/		{TEXT("text:allconfigreset")},
/*TEXT_SELECTSDCARD*/			{TEXT("text:selectsdcard")},
/*TEXT_PARKIGN_END_CHANGE_2CH*/ {TEXT("text:parkingendchange2ch")},
/*TEXT_SENSING_DUAL*/			{TEXT("text:sensingdual")},
/*TEXT_SENSING_VOLTAGE*/	{TEXT("text:sensingvoltage")},
/*TEXT_FILEERROR*/				{TEXT("text:fileerror")},
/*TEXT_FILEOPENERROR*/		{TEXT("text:fileopenerror")},
/*TEXT_OK*/						{TEXT("text:ok")},
/*TEXT_CANCEL*/						{TEXT("text:cancel")},
/*TEXT_COPYING_FILES*/		{TEXT("text:copyingfiles")},

/*TEXT_EVENT_REPORT_WRITING*/	{TEXT("text:eventreportwriting")},
/*TEXT_USER_INPUT*/							{TEXT("text:userinput")},
/*TEXT_DESCRIPTION_ADDUP*/			{TEXT("text:descriptionaddup")},
/*TEXT_PRINT*/									{TEXT("text:print")},
/*TEXT_EVENT_REPORT*/					{TEXT("text:eventreport")},
/*TEXT_REPORT*/					        {TEXT("text:report")},           // jun 170405
/*TEXT_EVENT_DATE*/							{TEXT("text:eventdate")},
/*TEXT_EVENT_LOC*/							{TEXT("text:eventloc")},
/*TEXT_G_SENSOR*/							{TEXT("text:g_sensor")},
/*TEXT_DESCRIPTION*/						{TEXT("text:description")},
/*TEXT_HIGH*/								{TEXT("text:high")},
/*TEXT_MIDDLE*/							{TEXT("text:middle")},
/*TEXT_LOW*/								{TEXT("text:low")},
/*TEXT_EVENT_CAPACITY*/		{TEXT("text:eventcapacity")},
/*TEXT_OLD_FILE_DELETE*/		{TEXT("text:oldfiledelete")},
/*TEXT_VIEWER*/							{TEXT("text:viewer")},
/*TEXT_FIRMWARE*/					{TEXT("text:firmware")},
/*TEXT_LASTVERSION*/				{TEXT("text:lastversion")},
/*TEXT_UPDATENEED*/				{TEXT("text:updateneed")},
/*TEXT_CHECKING*/					{TEXT("text:checking")},
/*TEXT_SERVER_ERROR*/			{TEXT("text:servererror")},
/*TEXT_STORAGE_INFO*/			{TEXT("text:storageinfo")},
/*TEXT_RELOAD*/			{TEXT("text:reload")},
/*TEXT_STORAGE_CAPACITY*/			{TEXT("text:storagecapacity")},
/*TEXT_CLOSE*/			{TEXT("text:close")},
/*TEXT_SETUPNOTFIND*/			{TEXT("text:setupnotfind")},
/*TEXT_FORMATCONFIRM*/			{TEXT("text:formatconfirm")},

/*TIP_LOGO*/							{TEXT("tooltip:logo")},
/*TIP_BTN_EXTEND*/				{TEXT("tooltip:extend")},
/*TIP_BTN_MAXIMIZE*/			{TEXT("tooltip:maximize")},
/*TIP_BTN_MINIMIZE*/			{TEXT("tooltip:minimize")},
/*TIP_BTN_CLOSE*/					{TEXT("tooltip:close")},
/*TIP_BTN_FRCHANGE*/			{TEXT("tooltip:frchange")},
/*TIP_BTN_FMCHANGE*/			{TEXT("tooltip:fmchange")},
/*TIP_BTN_ZOOM*/					{TEXT("tooltip:zoom")},
/*TIP_BTN_MUX*/						{TEXT("tooltip:mux")},
/*TIP_BTN_MAP*/						{TEXT("tooltip:map")},
/*TIP_BTN_INFO*/					{TEXT("tooltip:info")},
/*TIP_BTN_SETUP*/					{TEXT("tooltip:setup")},
/*TIP_BTN_CAPTURE*/				{TEXT("tooltip:capture")},
/*TIP_BTN_PRINT*/					{TEXT("tooltip:print")},
/*TIP_BTN_SAVE*/					{TEXT("tooltip:save")},
/*TIP_BTN_SAVEALL*/				{TEXT("tooltip:saveall")},
/*TIP_BTN_LOG*/						{TEXT("tooltip:log")},
/*TIP_BTN_PREV*/					{TEXT("tooltip:previous")},
/*TIP_BTN_PLAY*/					{TEXT("tooltip:play")},
/*TIP_BTN_PAUSE*/					{TEXT("tooltip:pause")},
/*TIP_BTN_STOP*/					{TEXT("tooltip:Stop")},
/*TIP_BTN_NEXT*/					{TEXT("tooltip:next")},
/*TIP_BTN_SEEK*/					{TEXT("tooltip:seek")},
/*TIP_BTN_SEEK_N*/				{TEXT("tooltip:seek_n")},
/*TIP_BTN_SEEK_P*/				{TEXT("tooltip:seek_p")},

/*TIP_BTN_FRONT*/				{TEXT("tooltip:front")},
/*TIP_BTN_REAR*/				{TEXT("tooltip:rear")},

/*TIP_BTN_DELALL*/				{TEXT("tooltip:deleteall")},
/*TIP_BTN_FILEOPEN*/			{TEXT("tooltip:fileopen")},
/*TIP_BTN_ALLFILE*/				{TEXT("tooltip:allfile")},
/*TIP_BTN_INFINITY*/			{TEXT("tooltip:infinity")},
/*TIP_BTN_MOTION*/				{TEXT("tooltip:motion")},
/*TIP_BTN_GSENSOR*/				{TEXT("tooltip:gsensor")},
/*TIP_BTN_EMERGENC*/			{TEXT("tooltip:emergenc")},
/*TIP_BTN_FORMAT*/				{TEXT("tooltip:format")},
/*TIP_BTN_GPSPULSE*/				{TEXT("tooltip:gpspulse")},


/*TIP_SPEED*/							{TEXT("tooltip:groundspeed")},
/*TIP_DIRECTION*/					{TEXT("tooltip:direction")},
/*TIP_GSENSORGRAPH*/			{TEXT("tooltip:gsensorgraph")},
/*TIP_PROGVOLUME*/				{TEXT("tooltip:volume")},
/*TIP_PROGPLAYTIME*/			{TEXT("tooltip:playtime")},
/*TIP_PROGPLAYSPEED*/			{TEXT("tooltip:playspeed")},
/*TIP_BRIGHTNESS*/						{TEXT("tooltip:brightness")},

/*TIP_BTN_SELDIR*/				{TEXT("tooltip:selectdirectory")},

/*ENTRIES_LAST*/ 
};

CLanguage::CLanguage(void)
{
	m_eLangId = LANG_ID_ENGLISH;
}
CLanguage::~CLanguage(void)
{
}
	
UINT CLanguage::GetLanguageCount(VOID)
{
	if(this->m_LanguageList.empty())
	{
		CString cstrLang;
		CString cstrDir;
		CString cstrPath;
		WIN32_FIND_DATA	findData;
		HANDLE			hDir = NULL;

		
		cstrDir = ((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getModulePath();
		cstrPath = cstrDir ;
		cstrPath +=  FIND_LANG_FORMAT;
		
		hDir = FindFirstFileEx( cstrPath, FindExInfoStandard, &findData, FindExSearchNameMatch, NULL, 0 );
		if( hDir == INVALID_HANDLE_VALUE )
			return FALSE;

		do
		{
			if( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				if( !_tcscmp(findData.cFileName, TEXT(".")) || !_tcscmp(findData.cFileName, TEXT("..")) )
					continue;
			}
			else
			{
				cstrPath = cstrDir ;
				cstrPath +=FIND_LANG_DIRECTORY;
				cstrPath +=  findData.cFileName;
				if(GetLenguageName(cstrPath, &cstrLang))
				{
					CLanguageList Lan(cstrPath, cstrLang);
					this->m_LanguageList.push_back(Lan);
				}
			}

		} while( FindNextFile(hDir, &findData) );

		FindClose(hDir);
	}
	
	return this->m_LanguageList.size() ;
}

BOOL CLanguage::GetLenguage(CString * str, UINT resource) // 0 is Korean, 1 is English
{
	if(this->m_LanguageList.empty())
		this->LoadLenguage(0);
	
	if( this->m_LanguageList.size() > 0 &&  resource < this->m_LanguageList.size() )
	{
		ITER_LANGLIST iLang		= this->m_LanguageList.begin();
		std::advance( iLang, resource );

		if( iLang == this->m_LanguageList.end())
			return FALSE;
		
		*str = iLang->m_acsLang[CLanguageList::LANGUAGE];

		return TRUE;
	}

	return FALSE;
}

BOOL CLanguage::LoadLenguage(UINT nIndex)
{
	BOOL result = FALSE;
	
	if(this->m_LanguageList.empty())
		GetLanguageCount();

	if(!this->m_LanguageText.empty())
		this->m_LanguageText.clear();

	if(nIndex >= this->m_LanguageList.size())
		nIndex = this->m_LanguageList.size() - 1;
	
	if(this->m_LanguageList.size() > 0)
	{
		int nText;
		DICTIONARY_UNI	*p_CfgDt;
		CTParserUni	cfgTPar;

		ITER_LANGLIST iLang = this->m_LanguageList.begin();
		std::advance( iLang, nIndex );

		if( iLang == this->m_LanguageList.end() )
		return FALSE;
		
		p_CfgDt = cfgTPar.Load(iLang->m_acsLang[CLanguageList::FILE_NAME] .GetBuffer());

		if(!p_CfgDt) {
			DEBUGMSG(1, ("GetLenguageName(): Load ERROR!!\n"));
			return FALSE;
		}

		for( nText = 0; nText < ENTRIES_LAST; nText++)
		{
			CString strText;
			result = this->GetLenguageText(p_CfgDt, ucLanguageKey[nText].text, &strText);
			this->m_LanguageText.push_back(strText);
		}

		cfgTPar.DelDict(p_CfgDt);

		m_eLangId = (LANG_ID)nIndex;

	}
	
	return result;
}

WCHAR* CLanguage::GetButtonText(WCHAR* text)
{
	static CString strText;
	
	switch(m_eLangId)
	{
		case CLanguage::LANG_ID_KOREAN:
			strText.Format(TEXT("%s_kor"), text);
			break;
		case CLanguage::LANG_ID_JAPANESE:
			strText.Format(TEXT("%s_jpn"), text);
			break;
		default:
			strText.Format(TEXT("%s_eng"), text);
			break;
	}
	return strText.GetBuffer();
}

BOOL CLanguage::GetLenguageText(CString * str, UINT resource)
{
	if(this->m_LanguageText.empty())
		this->LoadLenguage(0);
	
	if(resource < this->m_LanguageText.size())
	{
		ITER_TEXT iLang = this->m_LanguageText.begin();
		std::advance( iLang, resource );

		if( iLang == this->m_LanguageText.end() )
			return FALSE;
		
		*str = *iLang;
		return TRUE;
	}
	return FALSE;
}

BOOL CLanguage::GetLenguageText(DICTIONARY_UNI	*p_CfgDt, TCHAR *key, CString *retText)
{
	CTParserUni	cfgTPar;
	TCHAR*		c;
	c = cfgTPar.GetStr(p_CfgDt, key, NULL);
	if(c != NULL)
	{
#if 0	
		WCHAR* lpwzName = NULL;
		ClUtil::STRING::convertUTF8ToUnicode( c, &lpwzName );
		if( lpwzName )
		{
			retText->Format( _T("%s"),  lpwzName);

			 delete [] lpwzName;
			 return TRUE;
		}
#elif 1
		//TCHAR lpwzName[256];
		//if(ClUtil::STRING::convertChar2Wchar(lpwzName, c, 256))
		{
			retText->Format( _T("%s"),  c);
			return TRUE;
		}
#else
		retText->Format(_T("%s"),  c);
		return TRUE;
#endif
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
BOOL CLanguage::GetLenguageName(CString strFileName, CString* strName)
{
	BOOL result = FALSE;
	DICTIONARY_UNI	*p_CfgDt;
	CTParserUni	cfgTPar;
	
	p_CfgDt = cfgTPar.Load(strFileName.GetBuffer());

	if(!p_CfgDt) {
		DEBUGMSG(1, ("GetLenguageName(): Load ERROR!!\n"));
		return FALSE;
	}

	result = GetLenguageText(p_CfgDt, ucLanguageKey[INFO_LANGUAGE].text, strName);
		
	cfgTPar.DelDict(p_CfgDt);


	return result;
}
	
#undef MAXKEYLENGTH

