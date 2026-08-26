
// NXFSFormatterDlg.h : header file
//

#pragma once
#include "afxwin.h"

#if defined(__CLBB_NXFS_SUPPORT__)

#   include <fcntl.h>
#   include <BLACKBOX/nxfs_feature.h>

#   ifdef __cplusplus
extern "C" {
#   endif

#   pragma pack(push, 1)
#       include <BLACKBOX/nxfs_api.h>
#   pragma pack(pop)

#   ifdef __cplusplus
}
#   endif

#endif
#include <list>
#include "afxcmn.h"
#include <CLENAW/ClDialog.h>
#include <BLACKBOX/Language.h>
#include <BBConfigure.h>
#include <ConfigTextFile.h>

#define UID_FORMAT_PROGRESS                                 (WM_USER + 0x2000)
#define WM_FORMAT_USER_DRIVECHANGED                                (WM_USER + 0x2001)

#define BYTES_PER_SEC                                       512
#define DEFAULT_FAT_PARTITION_NUM                           0
#define CAP_FAT_DEFAULT_MB                                  200

#define NXFS_MAX_DIRS                                       6

#define NXFS_DIR_NAME_0                                     TEXT("INFINITE")
#define NXFS_DIR_NAME_1                                     TEXT("PARK");
#define NXFS_DIR_NAME_2                                     TEXT("EVENT");
#define NXFS_DIR_NAME_3                                     TEXT("MANUAL");
#define NXFS_DIR_NAME_4                                     TEXT("CAPTURE");
#define NXFS_DIR_NAME_5                                     TEXT("");

#define NXFS_DIR_CAP_PERCENT_0                              90
#define NXFS_DIR_CAP_PERCENT_1                              0
#define NXFS_DIR_CAP_PERCENT_2                              10
#define NXFS_DIR_CAP_PERCENT_3                              0
#define NXFS_DIR_CAP_PERCENT_4                              0
#define NXFS_DIR_CAP_PERCENT_5                              0

#define NXFS_DIR_DEF_FILES                                  10000

#define TID_INITIAL_START                                   100
#define TMDUR_INITIAL_START                                 10

// CNXFSFormatterDlg dialog
class CNXFSFormatterDlg : public CDialog
{
// Construction
public:
	CNXFSFormatterDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_NXFSFORMATTER_DIALOG };

    typedef enum _eDRIVEMODES
    {
        DRIVE_MODE_REMOVAL      = 0 ,
        DRIVE_MODE_REMOVAL_NXFS     ,
        DRIVE_MODE_FIXED            ,
        DRIVE_MODE_INVALID

    } EDRIVEMODE, *PEDRIVEMODE;

    typedef struct _tagDRIVEINFO
    {
        int             m_nDriveIndex;
		CString			m_cstrDrivePath;
        CString         m_cstrComboFace;
        char            m_strPysical[64];
        EDRIVEMODE      m_eDriveMode;
        int             m_CapacityKB;

    } SDRIVEINFO, *PSDRIVEINFO;
    typedef std::list<SDRIVEINFO>                           DRIVEINFO_LIST;
    typedef DRIVEINFO_LIST::iterator                        ITER_DRIVEINFO;

    typedef enum _eNXFSDIRTYPE
    {
        NXFSDIR_UNSPECIFIED     = 0 ,
        NXFSDIR_AUDIO               ,
        NXFSDIR_VIDEO               ,
        NXFSDIR_AUDIO_VIDEO         ,
        NXFSDIR_IMAGE               ,
        NXFSDIR_ARCHIVE             ,
        NXFSDIR_TXT                 ,
        NXFSDIR_RSVD1               ,
        NXFSDIR_RSVD2               ,
        NXFSDIR_INVALID

    } ENXFSDIR_TYPE, *PENXFSDIR_TYPE;

    typedef enum _ePARTITIONS
    {
        PART_FAT                = 0 ,
        PART_NXFS                   ,
        PART_INVALID

    } EPARTION_INFO, *PEPARTION_INFO;

    enum _ePROGRESSSTEPS
    {
        PROGRESS_FORMAT_START   = 0 ,
        PROGRESS_DELETE_PARTITION   ,
        PROGRESS_CREATE_FATFS       ,
        PROGRESS_CREATE_NXFS        ,
        PROGRESS_FORMAT_NXFS        ,
        PROGRESS_FORMAT_FAT_READY   ,
        PROGRESS_FORMAT_FAT         ,
        PROGRESS_FAILED             ,
        PROGRESS_INVALID
    };

    enum _EPROGRESSFAILED
    {
        FAILED_CHECK_INFO       = 0 ,
        FAILED_DELETE_PART          ,
        FAILED_CREATE_FAT           ,
        FAILED_CREATE_NXFS          ,
        FAILED_FORMAT_NXFS          ,
        FAILED_INVALID
    };

    typedef struct _tagNXFSDIR
    {
        CString         m_cstrDirName;
        int             m_nDirAttribute;
        int             m_nCapPercent;
        CString         m_cstrCapPercent;
        int             m_nDirCapMB;
        CString         m_cstrDirCapMB;
        int             m_nFiles;
        CSpinButtonCtrl m_spnDirCap;

    } SNXFS_DIR, *PSNXFS_DIR;

    typedef struct _tagPARTITION
    {
        CButton         m_chkUseThis;
        CSpinButtonCtrl m_spnCapacity;
        CString         m_cstrCapPercent;
        CString         m_cstrCapMB;
        int             m_nCapMB;
        float           m_fCapPercent;

    } SPARTION_INFO, *PSPARTION_INFO;

protected:

	virtual void            DoDataExchange                  ( CDataExchange* pDX );

// Implementation
protected:
    virtual BOOL            OnInitDialog                    ( void );
    virtual BOOL            PreTranslateMessage             ( MSG* pMsg );

protected:
    afx_msg void            OnPaint                         ( void );
    afx_msg HCURSOR         OnQueryDragIcon                 ( void );
    afx_msg void            OnTimer                         ( UINT_PTR nIDEvent );
    afx_msg void            OnLButtonDown                   ( UINT nFlags, CPoint point );

    afx_msg void            OnCmbSelchangeSeldrive          ( void );
    afx_msg void            OnClickReloadDriveList          ( void );

    afx_msg void            OnClickUseFAT                   ( void );

    afx_msg void            OnEnChangeDirName0              ( void );
    afx_msg void            OnEnChangeDirName1              ( void );
    afx_msg void            OnEnChangeDirName2              ( void );
    afx_msg void            OnEnChangeDirName3              ( void );
    afx_msg void            OnEnChangeDirName4              ( void );
    afx_msg void            OnEnChangeDirName5              ( void );

    afx_msg void            OnVScroll                       ( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar );

    afx_msg void            OnClickFormat                   ( void );
	afx_msg void 			OnBnClickedCancel				(void);
    afx_msg LRESULT         OnFormatProgress                ( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT         OnDriveChanged                  ( WPARAM wParam, LPARAM lParam );

    DECLARE_MESSAGE_MAP()

// OPERATION
public:
			void					SetSelDrive			( CString cstrSelDrive ) { m_cstrSelDrive = cstrSelDrive;}

			void					SetLanguage				( CLanguage * cLang) { m_pCLanguage = cLang;}
protected:
            void            _selectDrive                    ( void );

            void            _reloadDriveList                ( void );
            bool            __TestNXFSDrive                 ( int nDiskIndex, PSDRIVEINFO pDriveInfo );
            int             __getPhysicalDriveIndex         ( char cDriveName );

            void            _showInterrControls             ( BOOL bShow = TRUE );
			void 			OnLanguageChange				(void);
			void  			SetItemText						(int nIDDlgItem, UINT resource, CString index_str);

// GET/SET
public:
            PSDRIVEINFO     getCurDriveInfo                 ( void );
            PSPARTION_INFO  getPartitionInfo                ( EPARTION_INFO ePartition );
            PSNXFS_DIR      getNXFSDirInfo                  ( int nIndex );
protected:

// THREAD
public:
protected:
    static  DWORD           _ThreadFormatNXFS               ( LPVOID lpArg );
    static  bool            __createPartition               ( HWND hWnd, sint8* pPhsicalDevice, uint64_t u64szFATCapMB );
    static  int             __formatPartition               ( HWND hWnd, sint8* pPhsicalDevice, NxFS_DirTbl* pDirList );

protected:

	        HICON           m_hIcon;

            CComboBox       m_comboDriveList;
            DRIVEINFO_LIST  m_driveList;
            int             m_nCurDriveIndex;

            SNXFS_DIR       m_NXFSDirInfo[NXFS_MAX_DIRS];
            SPARTION_INFO   m_Partitions[PART_INVALID];

            int             m_nTotalDiskSpaceMB;

            CString         m_cstrStorageInfo;
            CString         m_cstrWarning;

            CProgressCtrl   m_progressFormat;
            CString         m_cstrProgress;

            BOOL            m_bIsFormatting;
			CString			m_cstrSelDrive;
			CLanguage * 	m_pCLanguage;
			
			BOOL			m_bConfigOpen;
			ST_CFG_DAVIEW	m_stCfgDaview;
};
