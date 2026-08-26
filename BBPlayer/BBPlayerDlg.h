// BBPlayerDlg.h : header file
//

#pragma once

//================================================================================================= S.INCLUDE
#include <atlimage.h>
#include <CLUDE/version.h>
#include <CLENAW/ClDialog.h>
#include <BLACKBOX/MultiPlayer.h>
#include <BLACKBOX/TextData.h>
#include <BLACKBOX/Config.h>
#include "BLACKBOX/RdfParser.h"
#include <ConfigTextFile.h>

//#include "GSensorDlg.h"
#include "CLENAW/GSensorView.h"
#include "CLENAW/ICDBGraph.h"
#include "CLENAW/ICMonthCalCtrl.h"
#include "CLENAW/ICDateTimeCtrl.h"
//#include "CLWBC/wbcFileManager.h"
#include "CLWBC/wbcTextData.h"
#include "CLWBC/wbcVideoView.h"
#include "CLWBC/wbcMapView.h"
#include "CLWBC/wbcSlider.h"
#include "CLWBC/wbcText.h"
#include "CLWBC/wbcButton.h"
#include "CLWBC/wbcFileManager.h"

#include "BBPlayerUIControl.h"
#include "BBConfigure.h"


//================================================================================================= E.INCLUDE
//================================================================================================= S.PREPROCESSOR

#define TEXT_DATA_THREAD_USE		1

#define MAINDLG_TID_PROGRESS								100
#define MAINDLG_TID_ZOOMREDRAW							101
#define MAINDLG_TID_MAINREDRAW							102
#define MAINDLG_TID_ACTIVEDRAW							103
#define MAINDLG_TID_MUXREDRAW							104

#define	MAINDLG_TMDUR_PROGRESS							50

#define CM_GSENSOR_VIEW_SEEK_FRAME          	WM_USER + 0x100			// wParam = cts , lParame =   pause flag
#define CM_ZOOM_REDRAW					WM_USER + 0x101			//
#define CM_MAIN_REDRAW					WM_USER + 0x102			// must value (WM_USER + 0x102)
#define CM_MUX_REDRAW						WM_USER + 0x103			//
//================================================================================================= E.PREPROCESSOR
//================================================================================================= S.TYPEDEF

//================================================================================================= E.TYPEDEF
//================================================================================================= S.CLASS

class CBBPlayerDlg : public ClDialog
{

public:
	CBBPlayerDlg(CWnd* pParent = NULL);
	virtual ~CBBPlayerDlg();

	//----------------------------------------------------------------------------------- S.TYPEDEF

	enum { IDD = IDD_MAIN_DLG };

	//----------------------------------------------------------------------------------- E.TYPEDEF

public:


protected:

	virtual void					DoDataExchange		( CDataExchange* pDX );
	virtual BOOL					PreTranslateMessage	( MSG* pMsg );
	virtual	VOID					InitResourceID		( VOID );

			BOOL					OpenFile			( CString strFilename , DWORD init_indexno = 0);
			CString					GetPathFromPIDL		( DWORD pidl );
			VOID					CheckFilters		( VOID );

			BOOL 					OpenDisk				(BOOL init = FALSE);
			BOOL					OpenDirectory		( CString cstrDir , BOOL bSubFolderSearch = TRUE);
			VOID					UpdateIndexFileList(INT index);
			BOOL					FileAddToFileList	(CString cstrDir, WIN32_FIND_DATA *pFindData);
			VOID					StopMedia			( VOID );
			VOID 					SetVolume			(INT nVolume); // max 100
			BOOL 					StartPlay			(void);
			BOOL 					PlayerFileListIndex		(INT index);
			static UINT				FileSaveThreadWork		(LPVOID pParam);
			VOID					SetUnitOfSpeed		(VOID);
			VOID 					SetUnitOfSpeed(ENUM_UNIT_OF_SPEED eUnitOfSpeed, BOOL bWriteConfig = FALSE);
#ifdef TIME_SEARCH_USE
			BOOL 					UpdateGraphTime(CTime * pTime=NULL, BOOL isForce=FALSE);
			BOOL 					UpdateGraphDB(CTime * pTime=NULL, BOOL isForce=FALSE, BOOL isFileTimeSet =FALSE);
			BOOL 					UpdateRdfData(CTime *pTime = NULL);
			INT	 					UpdateFileListFromTime(CTime *pTime = NULL);
			VOID 					ShowWindowTimeSearch(int nCmdShow);
#endif
			void 					FileListSorting(int nSortupDown = 0);
// Implementation
protected:

	virtual BOOL					OnInitDialog		( VOID );
	afx_msg void					OnPaint				( VOID );
	afx_msg HCURSOR					OnQueryDragIcon		( VOID );

	DECLARE_MESSAGE_MAP()

public:

	afx_msg void					OnClickSetting		( VOID );
	afx_msg void					OnClickLog		( VOID );
	afx_msg void					OnClickCapture		( VOID );
	afx_msg void					OnClickAbout		( VOID );
	afx_msg void					OnClickSite			( VOID );
	afx_msg void					OnClickMinimize		( VOID );
	afx_msg void					OnClickMaximize		( VOID );
	afx_msg void					OnClickNormalize	( VOID );
	afx_msg void					OnClickExit			( VOID );
	afx_msg void					OnClickPrev			( VOID );
	afx_msg void					OnClickPlay			( VOID );
	afx_msg void					OnClickStop			( VOID );
	afx_msg void					OnClickNext			( VOID );
	afx_msg void					OnClickRew			( VOID );
	afx_msg void					OnClickFF			( VOID );
	afx_msg void					OnClickFPS			( VOID );
	afx_msg void					OnClickOpen			( VOID );
	afx_msg void					OnClickSave			( VOID );
	afx_msg void					OnClickSaveAll		( VOID );
	afx_msg void					OnClickFRChange	( VOID );
	afx_msg void					OnClickFMChange	( VOID );
	afx_msg void 					OnBnClickZoom		( VOID );
	afx_msg void					OnClickTabNormal	( VOID );
	afx_msg void					OnClickTabEvent		( VOID );
	afx_msg void					OnClickTabManual	( VOID );
	afx_msg void					OnClickTabMotion	( VOID );
	afx_msg void OnBnClickedMainBtnCamFf();
	afx_msg void OnBnClickedMainBtnCamFm();
	afx_msg void OnBnClickedMainBtnCamRf();
	afx_msg void OnBnClickedMainBtnCamRm();
	afx_msg void OnBnClickedMainBtnVolume();
	afx_msg void OnBnClickedMainBtnSpeed();
	
	afx_msg void					OnTimer				( UINT_PTR nIDEvent );
	afx_msg void 					OnDropFiles			(HDROP hDropInfo);
	afx_msg	void					OnMouseMove			( UINT nFlags, CPoint point );
	afx_msg BOOL 				OnMouseWheel		(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void 					OnLButtonDblClk(UINT nFlags, CPoint point);
	
	afx_msg void					OnSize				( UINT nType, int cx, int cy );
	afx_msg BOOL 				OnCopyData( CWnd * pWnd, COPYDATASTRUCT* pcds );
	afx_msg LRESULT		 			OnAddFileFromArguments( WPARAM wParam, LPARAM lParam );
	afx_msg	LRESULT					OnDriveChanged		( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT					OnSliderScroll		( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT					OnMediaForceStop	( WPARAM wParam, LPARAM lParam );
	afx_msg void					OnNMDblclkFilelist	( NMHDR *pNMHDR, LRESULT *pResult );

	afx_msg void 					OnNMClickMainFilelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT 				OnGsensorSeekFrame(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT				OnDrawZoomImage	( WPARAM wParam = NULL, LPARAM lParam = NULL);
	afx_msg LRESULT				OnDrawMuxImage	( WPARAM wParam = NULL, LPARAM lParam = NULL);
	afx_msg LRESULT 				OnRedraw(WPARAM wParam, LPARAM lParam);
#ifdef TIME_SEARCH_USE
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnCloseupDatetimepickerDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeDatetimepickerTime(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg	long OnICDBGraph(WPARAM wParam, LPARAM lParam);
#endif

#if TEXT_DATA_THREAD_USE
	static UINT	ThreadTextDataProc(void* pArg);
#endif

protected:

	HICON					m_hIcon;

	CBBPlayerUIControl*		m_pUIControl;

	CMultiPlayer				m_multiPlayer;
	INT						m_nCurPlayIndex;
	CString					m_cstrSelectDrive;
	CString					m_cstrMp4SavePath;
	CString					m_cstrJpgSavePath;
	CString					m_cstrLogFilePath;
	CString					m_cstrPassword;
	INT						m_nVolumePosition;
	BOOL					m_bFileSaveThreadWorking;
	BOOL 					m_bFrontFlip;
	BOOL 					m_bFrontMirror;
	BOOL 					m_bRearFlip;
	BOOL 					m_bRearMirror;

	CTextData				m_textData;
	CGSensorView*			m_pGSensorView;
	CRect					m_rGSensorGraph;
	CRect					m_rVideoFront;

	CConfig					m_ConfigViewer;
	CBitmap 					m_BitmapZoom;
	CBitmap 					m_BitmapMux;
	LONGLONG				m_MuxViewCurrentTime;
	INT							m_iMuxViewChannel;

	ENUM_UNIT_OF_SPEED 		m_eUnitOfSpeed;

#ifdef TIME_SEARCH_USE
	CICDateTimeCtrl	m_ctrlTime;
	CICMonthCalCtrl	m_ctrlDate;
	CICDBGraph		m_ctrlICDBGraph[3];

	CTime	__m_ctmPrevSelected;
	CTime	__m_ctmPrevUpdateDB;
	CICDBGraph::ITEM	_m_item_h[24];
	CICDBGraph::ITEM	_m_item_m[24][60];
	CICDBGraph::ITEM	_m_item_s[24][60][60];
			
	BOOL	__m_isTimeBarEnabled;

 #ifdef RDF_USE
 	CRdfData m_cRdf;
 	CICDBGraph::ITEM	_m_rdf_h[24];
	CICDBGraph::ITEM	_m_rdf_m[24][60];
	CICDBGraph::ITEM	_m_rdf_s[24][60][60];
 #endif
#endif

#if TEXT_DATA_THREAD_USE
	CMultiPlayer	m_multiPlayerForTextRead;
	BOOL		m_bTextDataThreadWorking;
	BOOL		m_bTextDataThreadParsingStop;
	BOOL		m_bTextDataThreadExit;
	CWinThread* m_pTextDataThread;
	CString		m_cstrCurrentPlayFileName;
#endif

	//for file save thread
	CWinThread*				mp_SaveThread;
	CString 					m_cstrSaveFileExt;
	CString 					m_cstrSaveDstFile;
	CString					m_cstrSaveCurrentFileName;
	BOOL					m_bIsRemovalNxfs;
	int						m_nStreamCount;

	CWBCFileManager m_fileManager;

//// ++{ 20201026 FVFS Added *******************
	CString				m_cstrFvfsFileToAvi;
//// ++}**************************************
public:
	afx_msg void OnClickPrint();
	afx_msg void OnBnClickedMainBtnFullF();
	afx_msg void OnBnClickedMainBtnFullR();
	afx_msg void OnBnClickedMainBtnFullM();
	afx_msg void OnBnClickedMainBtnFileListAll();
	afx_msg void OnBnClickedMainBtnFormat();
	afx_msg void OnBnClickedMainBtnGpsPulse();
	afx_msg void OnStnClickedMainVideoFront();
	afx_msg void OnStnDblclickMainVideoFront();	
	afx_msg void OnStnClickedMainVideoRear();
	afx_msg void OnStnDblclickMainVideoRear();
};

//================================================================================================= S.CLASS
