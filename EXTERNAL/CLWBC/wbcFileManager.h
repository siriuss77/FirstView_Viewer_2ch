//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: wbcFileManger.h
//
//	Description: BBPlayer dedicated file manager class
//
//	Update History
// 
//      2014.07.14 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. FILEID

#pragma once

//================================================================================================= E. FILEID
//================================================================================================= S. INCLUDE

#include "stdafx.h"

#include <CLENAW/ClListCtrl.h>
#include <BLACKBOX/MultiPlayer.h>

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
#include <vector>

//================================================================================================= E. INCLUDE
//================================================================================================= S. PREPROCESSOR

//================================================================================================= E. PREPROCESSOR
//================================================================================================= S. CLASS

class CWBCFileManager
{
public:
    CWBCFileManager();
    ~CWBCFileManager();

public:

    typedef enum _eDRIVEMODES
    {
        DRIVE_MODE_REMOVAL      = 0 ,

#if defined(__CLBB_NXFS_SUPPORT__)
        DRIVE_MODE_REMOVAL_NXFS     ,
#endif

        DRIVE_MODE_FIXED            ,
        DRIVE_MODE_INVALID

    } EDRIVEMODE, *PEDRIVEMODE;

    typedef struct _tagDRIVEINFO
    {
        int             m_nDriveIndex;
        CString         m_cstrComboFace;
        char            m_strPysical[64];
        EDRIVEMODE      m_eDriveMode;

    } SDRIVEINFO, *PSDRIVEINFO;

    typedef struct _tagDIRINFO
    {
        CString         m_cstrDirName;
#if defined(__CLBB_NXFS_SUPPORT__)
        char            m_strNxFSDirName[MAX_DIR_NAME_STR_SIZE];
#endif
        int             m_nFiles;

    } SDIRINFO, *PSDIRINFO;

    typedef struct _tagFILEINFO
    {
        int             m_dirIndex;
#if defined(__CLBB_NXFS_SUPPORT__)
        int             m_fdNxFS;
        char            m_strNxFSFileName[MAX_FILE_NAME_SIZE];
#endif
        CString         m_cstrFileName;
        CString         m_cstrFullPath;
        FILETIME        m_ftFileTime;
        SYSTEMTIME      m_stSystemTime;
        INT64           m_llFileSize;

    } SFILEINFO, *PSFILEINFO;

    typedef struct _tagDEDICATEDDIRS
    {
        CString         m_cstrDirName;
#if defined(__CLBB_NXFS_SUPPORT__)
        char            m_strNxFSDirName[MAX_DIR_NAME_STR_SIZE];
#endif

    } SDEDDIR, *PSDEDDIR;

    typedef enum _eFILELISTSORTMETHOD
    {
        FM_SORT_NAME_TIME_SIZE_ASC  = 0 ,
        FM_SORT_TIME_NAME_SIZE_ASC      ,
        FM_SORT_NAME_TIME_SIZE_DEC      ,
        FM_SORT_TIME_NAME_SIZE_DEC      ,
        FM_SORT_INVALID                 ,

    } EFILEMANAGER_SORT, *PEFILEMANAGER_SORT;

    typedef enum _eFILECOPYRESULT
    {
        FILE_COPY_SUCCEEDED         = 0 ,
        FILE_COPY_FAILED                ,
        FILE_COPY_CANCELD               ,
        FILE_COPY_INVALID

    } EFILECOPY_RESULT, *PEFILECOPY_RESULT;

    typedef std::list<SDRIVEINFO>                           DRIVEINFO_LIST;
    typedef DRIVEINFO_LIST::iterator                        ITER_DRIVEINFO;

    typedef std::list<SDIRINFO>                             DIRINFO_LIST;
    typedef DIRINFO_LIST::iterator                          ITER_DIRINFO;

    typedef std::vector<SFILEINFO>                          FILEINFO_VEC;
    typedef FILEINFO_VEC::iterator                          ITER_FILEINFO;

    typedef std::list<PSFILEINFO>                           LISTUP_LIST;
    typedef LISTUP_LIST::iterator                           ITER_LISTUP;

    typedef std::list<SDEDDIR>                              DEDDIR_LIST;
    typedef DEDDIR_LIST::iterator                           ITER_DEDDIR;

    typedef std::list<CString>                              EXTFILTER_LIST;
    typedef EXTFILTER_LIST::iterator                        ITER_EXTFILTER;

	 typedef std::list<CString>                              	AVI_FILE_LIST;
    typedef AVI_FILE_LIST::iterator                        ITER_AVI_FILE;

protected:

    static  bool                __ASC_Name_Time_Size        ( const SFILEINFO& lhs, const SFILEINFO& rhs );
    static  bool                __ASC_Time_Name_Size        ( const SFILEINFO& lhs, const SFILEINFO& rhs );
    static  bool                __DEC_Name_Time_Size        ( const SFILEINFO& lhs, const SFILEINFO& rhs );
    static  bool                __DEC_Time_Name_Size        ( const SFILEINFO& lhs, const SFILEINFO& rhs );
    static  int CALLBACK        __BrowseCallbackProc        ( HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData );

    static  bool                m_sUseDirSort;

// Operation
public:
		 bool 			fileMerge(CString strDestPath, bool is_Nxfs);
	     bool 			fileSeparate(CString saveCurrentFileName, CString saveFileExt, bool is_Nxfs);
	     bool 	    		BrowseFolder(CString& strFolder, const TCHAR* lpszTitle, HWND hWnd);
            int                 openDrive                   ( void );
            void                dropDrive                   ( void );

            bool                selectDrive                 ( int nDriveIndex );

            int                 listupFiles                 ( const CString& cstrDirName, ClListCtrl* pListbox );
            int                 listupFiles                 ( int nDirIndex, ClListCtrl* pListbox );
            int                 listupFiles                 ( ClListCtrl* pListbox );

           EFILECOPY_RESULT  copy2Desktop( int nListupIndex, LPPROGRESS_ROUTINE lpProgressRoutine , LPVOID lpData);
	    EFILECOPY_RESULT copy2Desktop( CString& cstrNewFileFullPath, PSFILEINFO pFileInfo, LPPROGRESS_ROUTINE lpProgressRoutine, LPVOID lpData);

protected:
#if defined(__CLBB_NXFS_SUPPORT__)
            bool                _TestNXFSDrive              ( int nDiskIndex, PSDRIVEINFO pDriveInfo );
#endif

            bool                _selectNxFS                 ( const PSDRIVEINFO pDriveInfo );
            bool                _selectRemovalDrive         ( const PSDRIVEINFO pDriveInfo );
            bool                _selectLocalPath            ( void );

            bool                __openDirectory             ( PSDIRINFO pDirInfo, int nDirIndex, bool bRecursive = true );
#if defined(__CLBB_NXFS_SUPPORT__)
            bool                __openNxFSDirectory         ( PSDIRINFO pDirInfo, int nDirIndex );
            int                 __getPhysicalDriveIndex     ( char cDriveName );
#endif
            bool                __isValidExtension          ( const CString& cstrFilePath );


// Control
public:
		int 			ListboxAddFiles(ITER_FILEINFO iFile, ClListCtrl* pListbox );
            bool                addDedicatedDir             ( const CString& cstrDedDirName );

            bool                addExtFilter                ( const CString& cstrExtension );
            bool                removeExtFilter             ( int nIndex );
            bool                removeExtFilter             ( const CString& cstrExtension );
            void                clearExtFilter              ( void );
protected:
            void                _clearDrives                ( void );
            void                _clearDirectories           ( void );
            void                _clearFiles                 ( void );
            void                _sort                       ( void );

// Get/Set
public:
            bool                setParentWindow             ( HWND hParentWnd );
            bool                setSelectDirString          ( const CString& cstrSelDir );
            bool                setFindDesktopFilesString   ( const CString& cstrFindDesktop );

            void                setRecursiveScan            ( bool bRemoval, bool bDesktop );

#if defined(__CLBB_NXFS_SUPPORT__)
            bool                setNxFS2DesktopExt          ( const CString& cstrNxFS2DesktopExt );

            DEV_HANDLE*         getNxFSHandle               ( void );
#endif

            EDRIVEMODE          getCurDriveMode             ( void );
            const PSDRIVEINFO   getCurDriveInfo             ( void );

            int                 getDrives                   ( void );
            int                 getDirectories              ( void );
            int                 getFiles                    ( void );

            const PSDRIVEINFO   getDriveInfo                ( int nIndex );
            const PSDIRINFO     getDirectoryInfo            ( int nIndex );
            const PSFILEINFO    getFileInfo                 ( int nIndex );

            int                 getListupFiles              ( void );
            const PSFILEINFO    getListupFileInfo           ( int nIndex );

            int                 getExtFilterListsize        ( void );
            bool                getExtFilter                ( int nIndex, CString& cstrExtension );

            void                setSortMethod               ( EFILEMANAGER_SORT eSortMethod );
            void                setUseDirSort               ( bool bUseDirSort );

// Member
	     CString 		*m_pcstrMp4SavePath;
			AVI_FILE_LIST m_listAviFilePath;
private:

            HWND                m_hParentHwnd;
            EDRIVEMODE          m_eDriveMode;

            DIRINFO_LIST        m_dirList;
            FILEINFO_VEC        m_fileVec;
            DRIVEINFO_LIST      m_driveList;
            DEDDIR_LIST         m_dedicatedDirList;
            LISTUP_LIST         m_ListupFileList;
            EXTFILTER_LIST      m_extFilterList;

            bool                m_bRemovalRecvScan;
            bool                m_bDesktopRecvScan;

#if defined(__CLBB_NXFS_SUPPORT__)
            DEV_HANDLE*         m_pNxFSHandle;
#endif

            EFILEMANAGER_SORT   m_eSortMethod;
            int                 m_nCurSelDrive;

            CString             m_cstrSelectDir;
            CString             m_cstrFindLocalFile;
            CString             m_cstrNxFS2DesktopExt;
};

//================================================================================================= E. CLASS

//__WWW_CORELOGIC_CO_KR_BBPLAYER_FILEMANAGER_H__
//================================================================================================= EOF
