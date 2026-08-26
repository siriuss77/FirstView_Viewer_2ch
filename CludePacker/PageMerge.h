#pragma once
#include <CLUDE/version.h>
#include <CLUDE/ClCludeApp.H>
#include "ImageCollector.h"

#if (BUILD_MODEL == MODEL_DAVIEW_2CH)
 #if (BUILD_LANGUAGE == LANGUAGE_KOREAN)
 	#define	FILENAME_CLUDE												"Daview.skn"
 #else
 	#define	FILENAME_CLUDE												"Daview_eng.skn"
 #endif
#elif (BUILD_MODEL == MODEL_DAVIEW_2CH_L)	// jun 170405
 	#define	FILENAME_CLUDE												"Daview_eng.skn"
#elif (BUILD_MODEL == MODEL_DAVIEW_2CH_F)	// jun 170406
 	#define	FILENAME_CLUDE												"Daview_eng.skn"
#elif (BUILD_MODEL == MODEL_DAVIEW_2CH_S)	// jun 171018
 	#define	FILENAME_CLUDE												"Daview_eng.skn"
#elif (BUILD_MODEL == MODEL_BW_2000)
	#define	FILENAME_CLUDE												"Blackwing.skn"
#elif (BUILD_MODEL == MODEL_Q_BLACK)
	#define 	FILENAME_CLUDE								"QBlack.skn"
#elif (BUILD_MODEL == MODEL_KIC_M7)
	#define 	FILENAME_CLUDE								"Infobox.skn"
#elif (BUILD_MODEL == MODEL_HONDA_HD_2U)
	#define FILENAME_CLUDE									"Honda_hd_2u.skn"
#elif (BUILD_MODEL == MODEL_SAMPLE)
	#define 	FILENAME_CLUDE								"Sample.skn"
#elif (BUILD_MODEL == MODEL_ITSG_HD_2U)
	#define 	FILENAME_CLUDE								"itsg_hd_2u.skn"
#elif (BUILD_MODEL == MODEL_TMPD_HD_2U)
	#define 	FILENAME_CLUDE								"tmpd_hd_2u.skn"
#elif (BUILD_MODEL == MODEL_ICARO_I300HD)				
	#define 	FILENAME_CLUDE								"Icaro_i300hd.skn"
#elif (BUILD_MODEL == MODEL_VIEW_I_2U)	
	#define 	FILENAME_CLUDE								"view_i.skn"
#elif (BUILD_MODEL == MODEL_DOOCO_M7)
	#define 	FILENAME_CLUDE								"InfoboxDooco.skn"	
#elif (BUILD_MODEL == MODEL_RALLY_EYES)
	#define FILENAME_CLUDE									"RallyEyes.skn"
#elif (BUILD_MODEL == MODEL_EYE_TRACKS)
	#define FILENAME_CLUDE									"EyeTracks.skn"
#else
	#define	FILENAME_CLUDE									"CLUDE.PACK"
#endif

class CPageMerge : public CPropertyPageEx
{
	DECLARE_DYNAMIC(CPageMerge)

public:
	CPageMerge();
	virtual ~CPageMerge();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MERGE_PAGE };

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


public:

			BOOL					SetImageCollector	( CImageCollector* pImageCollector );
			BOOL					SetCludeApp			( ClCludeApp* pCludeApp );


public:

	afx_msg void					OnClickMerge		();
	virtual	BOOL					OnSetActive			();


protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:

	static	DWORD WINAPI			MergeThread			( LPVOID lpArg );


private:

			CImageCollector*		m_pImageCollector;
			ClCludeApp*				m_pCludeApp;
			BOOL					m_bIsCompress;

			XMLHEADER_POOL			m_XMLHeaderList;
			IMAGEHEADER_POOL		m_ImageHeaderList;
};
