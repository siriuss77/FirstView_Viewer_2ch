//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: wbcFileManger.cpp
//
//	Description: BBPlayer dedicated file manager class
//
//	Update History
// 
//      2014.07.14 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. INCLUDE

#include "stdafx.h"
#include "BBoxUtil.h"
#include <HELPER/ClLog.H>
#include "wbcFileManager.h"
#include <CLUDE/version.h>
#include <BLACKBOX/StrdParser.h>

#include <WinIoCtl.h>
#include <atlconv.h>
#include <algorithm>

//================================================================================================= E. INCLUDE

bool CWBCFileManager::m_sUseDirSort = false;

//================================================================================================= S. CON/DCON

CWBCFileManager::CWBCFileManager()
{
    this->m_hParentHwnd         = NULL;
    this->m_eDriveMode          = DRIVE_MODE_INVALID;
    this->m_eSortMethod         = FM_SORT_TIME_NAME_SIZE_ASC;

    this->m_nCurSelDrive        = -1;

    this->m_cstrSelectDir       = TEXT("Select Folder");
    this->m_cstrFindLocalFile   = TEXT("Find in My Computer");
    this->m_cstrNxFS2DesktopExt = TEXT("avi");

    this->m_bRemovalRecvScan    = false;
    this->m_bDesktopRecvScan    = false;
	 this->m_pcstrMp4SavePath = NULL;

#if defined(__CLBB_NXFS_SUPPORT__)
    this->m_pNxFSHandle     = NULL;
    CL_NxFS_Start();
#endif

}


CWBCFileManager::~CWBCFileManager()
{
#if defined(__CLBB_NXFS_SUPPORT__)
    if( this->m_pNxFSHandle != NULL )
    {
        CL_NxFS_Device_Close( this->m_pNxFSHandle );
        delete this->m_pNxFSHandle;
    }

    CL_NxFS_Stop();
#endif
}

//================================================================================================= E. CON/DCON
//================================================================================================= S. INIT/DEINIT
//================================================================================================= E. INIT/DEINIT
//================================================================================================= S. OPERATION

#define FROM_D_DRVIE    3
int CWBCFileManager::openDrive( void )
{
    this->_clearDrives();

    DWORD dwDrives = ::GetLogicalDrives();
    DWORD dwSectorsPerCluster;
    DWORD dwBytesPerSector;
    DWORD dwNumberOfFreeClusters;
    DWORD dwTotalNumberOfClusters;
    CString cstrDrivePath;

    dwDrives >>= FROM_D_DRVIE;

    int nDriveIndex;
    for( nDriveIndex = FROM_D_DRVIE; dwDrives; nDriveIndex++ )
    {
        if(dwDrives & 1)
        {
            cstrDrivePath.Format( TEXT("%c:\\"),'A' + nDriveIndex );
            SDRIVEINFO driveInfo;

            switch( ::GetDriveType(cstrDrivePath) )
            {
            case DRIVE_REMOVABLE:
                {
                    GetDiskFreeSpace( (LPCTSTR)cstrDrivePath,
                                        &dwSectorsPerCluster,
                                        &dwBytesPerSector,
                                        &dwNumberOfFreeClusters,
                                        &dwTotalNumberOfClusters );

                    driveInfo.m_nDriveIndex = nDriveIndex;
                    driveInfo.m_cstrComboFace = cstrDrivePath;
                    driveInfo.m_eDriveMode = DRIVE_MODE_REMOVAL;

                    if( dwBytesPerSector
#if defined(__CLBB_NXFS_SUPPORT__)
                        && _TestNXFSDrive(nDriveIndex, &driveInfo)
#endif
                        )
                    {
                        this->m_driveList.push_back( driveInfo );
                    }

                } break;

            default:;
            }
        }

        dwDrives >>=1;
    }

    // Add Local play drive info
    SDRIVEINFO localPlay;
    localPlay.m_cstrComboFace   = this->m_cstrFindLocalFile;
    localPlay.m_eDriveMode      = DRIVE_MODE_FIXED;

    this->m_driveList.push_back( localPlay );

    return this->m_driveList.size();
}


void CWBCFileManager::dropDrive( void )
{
    if( ! this->m_dirList.empty() )         this->m_dirList.clear();
    if( ! this->m_fileVec.empty() )         this->m_fileVec.clear();
    if( ! this->m_driveList.empty() )       this->m_driveList.clear();
    if( ! this->m_ListupFileList.empty() )  this->m_ListupFileList.clear();

	this->m_eDriveMode = DRIVE_MODE_INVALID;
	this->m_nCurSelDrive = -1;
    return;
}

int CALLBACK BrowseNotify(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{   
	if (uMsg == BFFM_INITIALIZED)
    {   
			SendMessage(hwnd, BFFM_SETSELECTION, 1, lpData);    // Set initial folder
        return 1;
    }
    return 0;
}

bool CWBCFileManager::BrowseFolder(CString& strFolder, const TCHAR* lpszTitle, HWND hWnd)
{
	::OleInitialize(NULL);

	BROWSEINFO bi;
	::ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.ulFlags = BIF_USENEWUI | BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS;
	bi.hwndOwner = hWnd;
	bi.lpszTitle = lpszTitle;
	bi.lpfn = BrowseNotify;

	if(this->m_pcstrMp4SavePath){
			bi.lParam = (LPARAM)(LPCTSTR)this->m_pcstrMp4SavePath->GetBuffer();
	}



	BOOL fRet = FALSE;
	TCHAR lpszPath[_MAX_PATH] = {0};
	LPITEMIDLIST idl = ::SHBrowseForFolder(&bi);
	if (idl != NULL)
	{
		fRet = ::SHGetPathFromIDList(idl, lpszPath) != 0;
		::CoTaskMemFree(idl);
	}

	::OleUninitialize();

	if (fRet)
		strFolder = lpszPath;

	return fRet;
}

#if defined(__CLBB_NXFS_SUPPORT__)
bool CWBCFileManager::_TestNXFSDrive( int nDiskIndex, PSDRIVEINFO pDriveInfo )
{
    HANDLE hDevice = NULL;

    nDiskIndex = this->__getPhysicalDriveIndex( 'A' + nDiskIndex );

    if( nDiskIndex <= 0 )
        return false;

    sprintf_s( pDriveInfo->m_strPysical, "\\\\.\\PhysicalDrive%d", nDiskIndex );

    hDevice = CreateFileA( pDriveInfo->m_strPysical, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL );
    if( hDevice == INVALID_HANDLE_VALUE )
        return false;

    if( CL_NxFS_Check_NxFS_partition((sint8*)pDriveInfo->m_strPysical) )
    {
        pDriveInfo->m_eDriveMode = DRIVE_MODE_REMOVAL_NXFS;
        pDriveInfo->m_cstrComboFace += TEXT(" (NXFS)");
    }
    else
    {
        pDriveInfo->m_eDriveMode = DRIVE_MODE_REMOVAL;
    }

    return true;
}


#define VOLUMEDISKSIZE (sizeof(VOLUME_DISK_EXTENTS))
int CWBCFileManager::__getPhysicalDriveIndex( char cDriveName )
{
    HANDLE hDevice;
    DWORD dwOut;
    int nDiskNumber = -1;
    char vcDriveName[ 40 ];
    VOLUME_DISK_EXTENTS* pstVolumeData;


    pstVolumeData = new VOLUME_DISK_EXTENTS;
    if( pstVolumeData == NULL )
        goto FAILED_GETPHYSICALDRIVEINDEX;

    sprintf_s( vcDriveName, 40, "\\\\?\\%c:", cDriveName );

    hDevice = CreateFileA( vcDriveName, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL );
    if( hDevice == INVALID_HANDLE_VALUE )
        goto FAILED_GETPHYSICALDRIVEINDEX;

    if( ! DeviceIoControl(hDevice, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0, pstVolumeData, VOLUMEDISKSIZE, &dwOut, NULL) )
        goto FAILED_GETPHYSICALDRIVEINDEX;

    CloseHandle( hDevice );

    if( pstVolumeData->NumberOfDiskExtents < 1 )
        goto FAILED_GETPHYSICALDRIVEINDEX;

    nDiskNumber = pstVolumeData->Extents[0].DiskNumber;


FAILED_GETPHYSICALDRIVEINDEX:

    if( pstVolumeData )
        delete pstVolumeData;

    return nDiskNumber;
}
#endif


bool CWBCFileManager::selectDrive( int nDriveIndex )
{
    if( nDriveIndex < 0 )
        return false;

    bool bReturn = false;
    ITER_DRIVEINFO iDrive;

#if defined(__CLBB_NXFS_SUPPORT__)
    if( this->m_pNxFSHandle )
    {
        CL_NxFS_Device_Close( this->m_pNxFSHandle );
        delete this->m_pNxFSHandle;
        this->m_pNxFSHandle = NULL;
    }
#endif

    iDrive = this->m_driveList.begin();
    std::advance( iDrive, nDriveIndex );
    if( iDrive == this->m_driveList.end() )
        goto FAILED_SELECTDRIVE;

    PSDRIVEINFO pDriveInfo = &(*iDrive);

    this->m_eDriveMode = iDrive->m_eDriveMode;
    
    switch( this->m_eDriveMode )
    {
    case DRIVE_MODE_REMOVAL:        bReturn = this->_selectRemovalDrive( pDriveInfo ); break;

#if defined(__CLBB_NXFS_SUPPORT__)
    case DRIVE_MODE_REMOVAL_NXFS:   bReturn = this->_selectNxFS( pDriveInfo );         break;
#endif

    case DRIVE_MODE_FIXED:          bReturn = this->_selectLocalPath();                break;
    default:;
    }

    if( bReturn )
    {
        this->_sort();
        this->m_nCurSelDrive = nDriveIndex;
    }

FAILED_SELECTDRIVE:

    return bReturn;
}

int CWBCFileManager::ListboxAddFiles(ITER_FILEINFO iFile, ClListCtrl* pListbox )
{
#if 0        
            pListbox->InsertItem( nItemCount, iFile->m_cstrFileName );

            CString cstrText;
            cstrText.Format( TEXT("%02d/%02d/%d"), iFile->m_stSystemTime.wMonth, iFile->m_stSystemTime.wDay, iFile->m_stSystemTime.wYear );
            pListbox->SetItemText( nItemCount, 1, cstrText );

            cstrText.Format(TEXT("%d,%03d"), int(iFile->m_llFileSize / 1000), int(iFile->m_llFileSize % 1000));
            pListbox->SetItemText( nItemCount, 2, cstrText );
#else
			INT nItemCount = pListbox->GetItemCount();
			CString cstrText, cstrDir;
			SYSTEMTIME stUTC, stLocal;
			TCHAR *cFileName = iFile->m_cstrFileName.GetBuffer();

			//tony, for sort	
			//20010119_071403_I2 daview
			if(cFileName[8] == L'_' && cFileName[15] == L'_')
			{
				TCHAR szTime[9];
				_tcsncpy(szTime, &cFileName[0], 8);
				szTime[8] = '\0';
				cstrText = szTime;

				_tcsncpy(szTime, &cFileName[9], 6);
				szTime[6] = '\0';
				cstrText += szTime;			

				stLocal.wYear = ((cFileName[0] - L'0') * 1000) + ((cFileName[1] - L'0') * 100) + ((cFileName[2] - L'0') * 10) + cFileName[3] - L'0';
				stLocal.wMonth = ((cFileName[4] - L'0') * 10) + cFileName[5] - L'0' ;
				stLocal.wDay = ((cFileName[6] - L'0') * 10) + cFileName[7] - L'0';

				stLocal.wHour = ((cFileName[9] - L'0') * 10) + cFileName[10] - L'0';
				stLocal.wMinute = ((cFileName[11] - L'0') * 10) + cFileName[12] - L'0';
				stLocal.wSecond = ((cFileName[13] - L'0') * 10) + cFileName[14] - L'0';	
			}
			else if(cFileName[3] == L'_' && cFileName[10] == L'_') //avic file
			{
				TCHAR szTime[9];
				int offset;
				_tcsncpy(szTime, &cFileName[4], 6);
				szTime[6] = '\0';
				cstrText = TEXT("20");
				cstrText += szTime;

				_tcsncpy(szTime, &cFileName[11], 6);
				szTime[6] = '\0';
				cstrText += szTime;			
				
				stLocal.wYear = 2000 + ((cFileName[4] - L'0') * 10)  + cFileName[5] - L'0';
				stLocal.wMonth = ((cFileName[6] - L'0') * 10) + cFileName[7] - L'0' ;
				stLocal.wDay = ((cFileName[8] - L'0') * 10) + cFileName[9] - L'0';

				stLocal.wHour = ((cFileName[11] - L'0') * 10) + cFileName[12] - L'0';
				stLocal.wMinute = ((cFileName[13] - L'0') * 10) + cFileName[14] - L'0';
				stLocal.wSecond = ((cFileName[15] - L'0') * 10) + cFileName[16] - L'0';
			}
			else
			{
				cstrText.Format( TEXT("%d%02d%02d%02d%02d%02d"), iFile->m_stSystemTime.wYear - 2000, iFile->m_stSystemTime.wMonth, iFile->m_stSystemTime.wDay,iFile->m_stSystemTime.wHour,iFile->m_stSystemTime.wMinute,iFile->m_stSystemTime.wSecond);
			}
			
			pListbox->InsertItem( nItemCount, cstrText );

			//20170523 tony, for sort 기존에는 녹화시간으로 저장하였으나 동일한 녹화시간의 파일에선 쏘팅이 안되는 문제가 있어 일런번호로 변경
			pListbox->SetItemData(nItemCount , nItemCount);

#if 0
			CString str;
			
			str.Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), time.GetYear(), time.GetMonth(), time.GetDay(), \
				time.GetHour(), time.GetMinute(), time.GetSecond());
			pListbox->SetItemText( nItemCount, 1, str);
#else
			pListbox->SetItemText( nItemCount, 1, cFileName);
#endif			

			cstrText.Format(TEXT("%d.%01dMB"), int(iFile->m_llFileSize / 1000), int(iFile->m_llFileSize % 1000 / 100));
			pListbox->SetItemText( nItemCount, 2, cstrText );

			cstrText = cstrDir + cFileName;
			pListbox->SetItemText( nItemCount, 3, cstrText );	

			//20170523 : filename2 ex) E2016...
			cstrText = cFileName;
			cstrText.Insert(0, cstrText.GetBuffer()[16]);
			pListbox->SetItemText( nItemCount, 4, cstrText);

			if(stLocal.wYear < 1970)   //jun 190926 1970년 이전 연도에러 수정
				stLocal.wYear = 1970;   // jun 190926
			//20170523 : str_date
			CTime time = CTime(stLocal);
			CString strTime;
			strTime.Format(_T("%d"), time.GetTime());
			pListbox->SetItemText(nItemCount , 5,  strTime);
#endif	
	return 0;
}

int CWBCFileManager::listupFiles( const CString& cstrDirName, ClListCtrl* pListbox )
{
    ITER_DIRINFO iDir = this->m_dirList.begin();
    ITER_DIRINFO iEndDir = this->m_dirList.end();
    for( ; iDir != iEndDir; iDir ++ )
    {
        if( ! iDir->m_cstrDirName.Compare(cstrDirName) )
            break;
    }

    if( iDir == this->m_dirList.end() )
        return -1;

    return this->listupFiles( std::distance(this->m_dirList.begin(), iDir), pListbox );
}


int CWBCFileManager::listupFiles( int nDirIndex, ClListCtrl* pListbox )
{
    this->m_ListupFileList.clear();

    pListbox->SetRedraw( FALSE );
    pListbox->DeleteAllItems();

    ITER_FILEINFO iFile = this->m_fileVec.begin();
    ITER_FILEINFO iEndFile = this->m_fileVec.end();
    INT nItemCount = 0;
    for( ; iFile != iEndFile; iFile ++ )
    {
        if( iFile->m_dirIndex == nDirIndex)
        {
			 this->ListboxAddFiles(iFile, pListbox);
            this->m_ListupFileList.push_back( &(*iFile) );

            nItemCount ++;
        }
    }

    pListbox->SetRedraw( TRUE );

    return nItemCount;
}


int CWBCFileManager::listupFiles( ClListCtrl* pListbox )
{
    INT nItemCount = 0;

    if( this->m_fileVec.empty() )
        return nItemCount;

    this->m_ListupFileList.clear();

    pListbox->SetRedraw( FALSE );
    pListbox->DeleteAllItems();

    ITER_FILEINFO iFile = this->m_fileVec.begin();
    ITER_FILEINFO iEndFile = this->m_fileVec.end();
    for( ; iFile != iEndFile; iFile ++ )
    {
		 this->ListboxAddFiles(iFile, pListbox);
		 
        this->m_ListupFileList.push_back( &(*iFile) );

        nItemCount ++;
    }

    pListbox->SetRedraw( TRUE );

    return nItemCount;
}

CWBCFileManager::EFILECOPY_RESULT CWBCFileManager::copy2Desktop( CString& cstrNewFileFullPath, PSFILEINFO pFileInfo, LPPROGRESS_ROUTINE lpProgressRoutine, LPVOID lpData)
{
	  char strNxfsPath[MAX_DIR_NAME_STR_SIZE + MAX_FILE_NAME_SIZE + 4];
	  FILE *fpDest = NULL;
      int szReadBuffer = 256 * 1024;
      int szReadout = 0;
	   INT64 transferredBytes = 0;
      BYTE* pBuffer = NULL;
		bool bLastFileDelete = false;
			
        int fdNew = -1;
        PSDIRINFO pDirInfo = this->getDirectoryInfo( pFileInfo->m_dirIndex );
        if( pDirInfo == NULL )
            return FILE_COPY_FAILED;

        _wfopen_s( &fpDest, cstrNewFileFullPath.GetBuffer(), TEXT("wb") );
        if( !fpDest )
            return FILE_COPY_FAILED;

        pBuffer = new BYTE[szReadBuffer];
        if( pBuffer == NULL )
            return FILE_COPY_FAILED;

        sprintf_s( strNxfsPath, MAX_DIR_NAME_STR_SIZE + MAX_FILE_NAME_SIZE + 4, "/%s/%s", pDirInfo->m_strNxFSDirName, pFileInfo->m_strNxFSFileName );

        fdNew = CL_NxFS_File_Open( this->m_pNxFSHandle, (sint8 *)strNxfsPath, (sint8 *)"r", NULL, 0, pFileInfo->m_fdNxFS );
        if( fdNew < 1 )
        {
            if( pBuffer ) delete[] pBuffer;

            return FILE_COPY_FAILED;
        }

		 while( true )
        {
            szReadout = CL_NxFS_File_Read( this->m_pNxFSHandle, pBuffer, szReadBuffer, fdNew );
            if( szReadout <= 0 )
                break;

            fwrite( pBuffer, 1, szReadout, fpDest );
			  transferredBytes += szReadout;

			 if(lpProgressRoutine){
			 	LARGE_INTEGER TotalBytesTransferred;
				LARGE_INTEGER TotalFileSize;
				LARGE_INTEGER StreamSize = {0,};
				LARGE_INTEGER StreamBytesTransferred = {0,};

				TotalBytesTransferred.QuadPart = transferredBytes;

				TotalFileSize.QuadPart = pFileInfo->m_llFileSize * 1024 + 1023;
				 if( lpProgressRoutine(TotalFileSize,
								   TotalBytesTransferred,
								   StreamSize,
								   StreamBytesTransferred,
								   0,
								   0,
								   NULL,
								   NULL,
								   lpData) != PROGRESS_CONTINUE){
					bLastFileDelete = true;
					break;
				 }
			 }
        }

        if( pBuffer ) delete[] pBuffer;

        fflush( fpDest );
        fclose( fpDest );

		if(bLastFileDelete)
			CFile::Remove(cstrNewFileFullPath.GetBuffer());

        CL_NxFS_File_Close( this->m_pNxFSHandle, fdNew );
}
CWBCFileManager::EFILECOPY_RESULT CWBCFileManager::copy2Desktop( int nListupIndex , LPPROGRESS_ROUTINE lpProgressRoutine, LPVOID lpData)
{
    PSFILEINFO pFileInfo = this->getListupFileInfo( nListupIndex );
    if( pFileInfo == NULL )
        return FILE_COPY_FAILED;

    CString cstrNewFileName;
    CString cstrExtension;
    CString cstrFilter;
    CString cstrSourceFilePath;

    TCHAR tcsDesktopPath[MAX_PATH];
    SHGetSpecialFolderPath( NULL, tcsDesktopPath, CSIDL_DESKTOP, FALSE );
    cstrNewFileName = tcsDesktopPath;
    cstrNewFileName	+= TEXT("\\");

    cstrSourceFilePath = pFileInfo->m_cstrFullPath;

#if defined(__CLBB_NXFS_SUPPORT__)
    if( pFileInfo->m_fdNxFS > 0 )
    {
        int nPos = cstrSourceFilePath.ReverseFind( '.' );
        
        cstrNewFileName += cstrSourceFilePath.Left( nPos );
        cstrNewFileName += TEXT(".");
        cstrNewFileName += this->m_cstrNxFS2DesktopExt;

        cstrFilter.Format( TEXT("video(*.%s)|*.%s||"), this->m_cstrNxFS2DesktopExt, this->m_cstrNxFS2DesktopExt );
        CFileDialog saveDlg( FALSE, NULL, cstrNewFileName, OFN_OVERWRITEPROMPT|OFN_ENABLESIZING, cstrFilter, NULL );
		 if(m_pcstrMp4SavePath)
			 saveDlg.m_ofn.lpstrInitialDir = m_pcstrMp4SavePath->GetBuffer(); // 오픈할때 초기 경로 지정
			 
        if( saveDlg.DoModal() == IDOK )
        {
        	 m_pcstrMp4SavePath->Format(TEXT("%s"), saveDlg.GetPathName());
            return copy2Desktop(	saveDlg.GetPathName(), pFileInfo, lpProgressRoutine, lpData);
        }
        else
        {
            return FILE_COPY_CANCELD;
        }
    }
    else
#endif
    {
        int nPos = cstrSourceFilePath.ReverseFind( '\\' );
        cstrNewFileName += cstrSourceFilePath.Mid( nPos + 1, cstrSourceFilePath.GetLength() - nPos - 1 );

        nPos = cstrSourceFilePath.ReverseFind('.');
        if( nPos < 0 )	cstrExtension = TEXT("*");
        else			cstrExtension = cstrSourceFilePath.Mid( nPos + 1, cstrSourceFilePath.GetLength() - nPos - 1 );

        cstrFilter.Format( TEXT("video(*.%s)|*.%s||"), cstrExtension, cstrExtension );
        CFileDialog saveDlg( FALSE, NULL, cstrNewFileName, OFN_OVERWRITEPROMPT|OFN_ENABLESIZING, cstrFilter, NULL );
        //CFileDialog saveDlg( FALSE, NULL, cstrNewFileName, OFN_OVERWRITEPROMPT, cstrFilter, NULL );

		 saveDlg.m_ofn.lpstrInitialDir = m_pcstrMp4SavePath->GetBuffer(); // 오픈할때 초기 경로 지정
        if( saveDlg.DoModal() == IDOK )
        {
            CString cstrFileName	= saveDlg.GetFileName();
            CString cstrFileExt		= saveDlg.GetFileExt();
            CString cstrDstFile		= saveDlg.GetPathName();

            if( ! cstrSourceFilePath.Compare(cstrDstFile) )
                return FILE_COPY_FAILED;

			 m_pcstrMp4SavePath->Format(TEXT("%s"), cstrDstFile);
			 
            SHFILEOPSTRUCT stFileOP;

            cstrSourceFilePath += TEXT('\0');
            cstrDstFile += TEXT('\0');

            ZeroMemory( &stFileOP, sizeof(SHFILEOPSTRUCT) );
            stFileOP.hwnd	= this->m_hParentHwnd;
            stFileOP.wFunc	= FO_COPY;
            stFileOP.fFlags = FOF_NOCONFIRMATION;
            stFileOP.pFrom	= cstrSourceFilePath;
            stFileOP.pTo	= cstrDstFile;

            if( SHFileOperation(&stFileOP) )
                return FILE_COPY_FAILED;
            else
                return FILE_COPY_SUCCEEDED;
        }
        else
        {
            return FILE_COPY_CANCELD;
        }
    }

    return FILE_COPY_SUCCEEDED;
}


#if defined(__CLBB_NXFS_SUPPORT__)
bool CWBCFileManager::_selectNxFS( const PSDRIVEINFO pDriveInfo )
{
    bool bResult = false;
    ITER_DEDDIR iDDir;
    ITER_DEDDIR iEndDDir;

    this->m_dirList.clear();
    this->m_fileVec.clear();

    this->m_pNxFSHandle = new DEV_HANDLE;
    memset( this->m_pNxFSHandle, 0, sizeof(DEV_HANDLE) );
    sint32 ret = CL_NxFS_Device_Open( pDriveInfo->m_strPysical, ATTR_DEV_RDWR | ATTR_DEV_O_SYNC, this->m_pNxFSHandle );
    if( ret != ERRNO_SUCCESS )
    {
        goto FAILED_SELECT_NXFS;
    }

    iDDir = this->m_dedicatedDirList.begin();
    iEndDDir = this->m_dedicatedDirList.end();
    for( ; iDDir != iEndDDir; iDDir ++ )
    {
        SDIRINFO dirInfo;
        strcpy_s( dirInfo.m_strNxFSDirName, MAX_DIR_NAME_STR_SIZE, iDDir->m_strNxFSDirName );
        
        int nLen = MultiByteToWideChar( CP_UTF8, 0, dirInfo.m_strNxFSDirName, -1, 0, 0 );
        WCHAR* lpwzString = new WCHAR[nLen+1];
        MultiByteToWideChar( CP_UTF8, 0, dirInfo.m_strNxFSDirName, -1, lpwzString, nLen );

        dirInfo.m_cstrDirName.Format( TEXT("%s"), lpwzString );
        if( lpwzString )
            delete [] lpwzString;

        dirInfo.m_nFiles = 0;

        if( this->__openNxFSDirectory( &dirInfo, std::distance(this->m_dedicatedDirList.begin(), iDDir)) )
            this->m_dirList.push_back( dirInfo );
    }

    bResult = true;

FAILED_SELECT_NXFS:

    if( ! bResult )
    {
        if( this->m_pNxFSHandle )
        {
            delete this->m_pNxFSHandle;
            this->m_pNxFSHandle = NULL;
        }

        printf( "NXFS drive cannot be opened\n" );
    }

    return bResult;
}
#endif


bool CWBCFileManager::_selectRemovalDrive( const PSDRIVEINFO pDriveInfo )
{
    this->m_dirList.clear();
    this->m_fileVec.clear();

    ITER_DEDDIR iDDir = this->m_dedicatedDirList.begin();
    ITER_DEDDIR iEndDDir = this->m_dedicatedDirList.end();
    for( ; iDDir != iEndDDir; iDDir ++ )
    {
        SDIRINFO dirInfo;
        dirInfo.m_cstrDirName = pDriveInfo->m_cstrComboFace;
        dirInfo.m_cstrDirName += iDDir->m_cstrDirName;
        dirInfo.m_cstrDirName += TEXT("\\");
        dirInfo.m_nFiles = 0;

        this->__openDirectory( &dirInfo, std::distance(this->m_dedicatedDirList.begin(), iDDir), this->m_bRemovalRecvScan );
        this->m_dirList.push_back( dirInfo );
    }

    return true;
}


bool CWBCFileManager::_selectLocalPath( void )
{
    ITEMIDLIST* pidlBrowse;
    WCHAR achPath[MAX_PATH];
    BROWSEINFO brInfo;
    SDIRINFO dirInfo;

    if( this->m_hParentHwnd == NULL )
        return false;

    memset(&brInfo, 0x00, sizeof(BROWSEINFO));

    brInfo.hwndOwner    = this->m_hParentHwnd;
    brInfo.pidlRoot     = NULL;
    brInfo.lParam       = (LPARAM)NULL;
    brInfo.lpszTitle    = this->m_cstrSelectDir.GetBuffer();
    brInfo.ulFlags      = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN;
    brInfo.lpfn         = CWBCFileManager::__BrowseCallbackProc;

    pidlBrowse          = ::SHBrowseForFolder( &brInfo );

    if( pidlBrowse == NULL)
        return false;

    SHGetPathFromIDList( pidlBrowse, achPath );

    dirInfo.m_cstrDirName = achPath;
    dirInfo.m_cstrDirName += TEXT("\\");
    dirInfo.m_nFiles = 0;

    this->m_dirList.clear();
    this->m_fileVec.clear();

    this->__openDirectory( &dirInfo, 0, this->m_bDesktopRecvScan );

    if( ! dirInfo.m_nFiles )
        return false;

    this->m_dirList.push_back( dirInfo );

    return true;
}


bool CWBCFileManager::__openDirectory( PSDIRINFO pDirInfo, int nDirIndex, bool bRecursive )
{
    WIN32_FIND_DATA	findData;
    HANDLE			hDir = NULL;
    SYSTEMTIME      stUTC;
    SDIRINFO        dirInfo;

    hDir = FindFirstFileEx( pDirInfo->m_cstrDirName + TEXT("*.*"), FindExInfoStandard, &findData, FindExSearchNameMatch, NULL, 0 );
    if( hDir == INVALID_HANDLE_VALUE )
        return FALSE;

    do
    {
        if( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
        {
            if( !bRecursive || !_tcscmp(findData.cFileName, TEXT(".")) || !_tcscmp(findData.cFileName, TEXT("..")) )
                continue;

            dirInfo.m_cstrDirName = pDirInfo->m_cstrDirName + findData.cFileName + TEXT("\\");
            pDirInfo->m_nFiles += this->__openDirectory( &dirInfo, nDirIndex );
        }
        else
        {
            SFILEINFO fileInfo;
            fileInfo.m_cstrFileName = findData.cFileName;

#if defined(__CLBB_NXFS_SUPPORT__)
            fileInfo.m_fdNxFS = -1;
#endif
            if( ! this->__isValidExtension( fileInfo.m_cstrFileName) )
                continue;

            fileInfo.m_dirIndex = nDirIndex;
            fileInfo.m_ftFileTime = findData.ftCreationTime;

            FileTimeToSystemTime( &fileInfo.m_ftFileTime, &stUTC );
            SystemTimeToTzSpecificLocalTime( NULL, &stUTC, &fileInfo.m_stSystemTime );

            fileInfo.m_llFileSize = (((INT64)findData.nFileSizeHigh)<<32) + findData.nFileSizeLow;
            fileInfo.m_llFileSize /= 1024;
            fileInfo.m_cstrFullPath = pDirInfo->m_cstrDirName + findData.cFileName;

            pDirInfo->m_nFiles ++;

            this->m_fileVec.push_back( fileInfo );
        }

    } while( FindNextFile(hDir, &findData) );

    FindClose(hDir);

    return true;
}

#if defined(__CLBB_NXFS_SUPPORT__)
bool CWBCFileManager::__openNxFSDirectory( PSDIRINFO pDirInfo, int nDirIndex )
{
    SFILEINFO fileInfo;
    LONGLONG llTime;
    SYSTEMTIME stUTC;

    struct NxFS_FileList *pfileList = NULL;
    uint32 alloc_size = sizeof(struct NxFS_FileList) + (MAX_NUM_FILE_LIST_GET * sizeof(struct NxFS_FileInfo));
    pfileList = (NxFS_FileList*) malloc( alloc_size );

    int fileDescriptor_start = 1;
    memset( (uint8 *)pfileList, 0, alloc_size );
    pfileList->num_files = MAX_NUM_FILE_LIST_GET;

    do
    {
        int nFiles = CL_NxFS_Get_FileList( this->m_pNxFSHandle, (sint8 *)pDirInfo->m_strNxFSDirName, (uint32*)&fileDescriptor_start, pfileList, SORTING_TYPE_ASC );
        if( nFiles <= 0 )
            break;

        struct NxFS_FileInfo *pFileInfo = (struct NxFS_FileInfo *)pfileList->file_info;

        int nIndex;
        for( nIndex = 0; nIndex < nFiles; nIndex ++ )
        {
            fileInfo.m_dirIndex = nDirIndex;
            fileInfo.m_fdNxFS = pFileInfo[nIndex].fd;
            strcpy_s( fileInfo.m_strNxFSFileName, MAX_FILE_NAME_SIZE, (char*)pFileInfo[nIndex].file_name );

            llTime = Int32x32To64(pFileInfo[nIndex].file_created, 10000000) + 116444736000000000;
            fileInfo.m_ftFileTime.dwLowDateTime = (DWORD)llTime;
            fileInfo.m_ftFileTime.dwHighDateTime = llTime >> 32;

            FileTimeToSystemTime( &fileInfo.m_ftFileTime, &stUTC );
            SystemTimeToTzSpecificLocalTime( NULL, &stUTC, &fileInfo.m_stSystemTime );

            fileInfo.m_llFileSize = pFileInfo[nIndex].file_size / 1024;

            int nStrLen = MultiByteToWideChar( CP_ACP, 0, (char*)pFileInfo[nIndex].file_name, -1, 0, 0 );
            WCHAR* lpwzString = new WCHAR[nStrLen + 1];
            MultiByteToWideChar( CP_ACP, 0, (char*)pFileInfo[nIndex].file_name, -1, lpwzString, nStrLen );
            fileInfo.m_cstrFileName.SetString( lpwzString );
            fileInfo.m_cstrFullPath = fileInfo.m_cstrFileName;

            if( lpwzString != NULL )
                delete [] lpwzString;

            this->m_fileVec.push_back( fileInfo );
        }

        pDirInfo->m_nFiles += nFiles;

    } while( 1 );

    if( pfileList != NULL )
        free( pfileList );

    return true;
}
#endif


bool CWBCFileManager::__isValidExtension( const CString& cstrFilePath )
{
    if( !cstrFilePath.GetLength() )
        return false;

    CString cstrNewExtension;
    INT nPos = cstrFilePath.ReverseFind( '.' );
    cstrNewExtension = cstrFilePath.Mid( nPos + 1, cstrFilePath.GetLength() - nPos - 1 );
    cstrNewExtension.MakeLower();

    ITER_EXTFILTER iExt = this->m_extFilterList.begin();
    ITER_EXTFILTER iEndExt = this->m_extFilterList.end();
    for( ; iExt != iEndExt; iExt ++ )
    {
        if( ! iExt->Compare(cstrNewExtension) )
            break;
    }

    if( iExt == this->m_extFilterList.end() )
        return false;

    return true;
}

//================================================================================================= E. OPERATION
//================================================================================================= S. CONTROL


/*




// 순수 C++ 초고속 무손실 AVI Merge (3ch 비디오 + 오디오 + GPS/G-Sensor 100% 무손실 결합)
static bool NativeAviMerge(const std::vector<CString>& srcFiles, const CString& dstFile)
{
	if (srcFiles.size() < 2) return false;

	FILE* fpOut = NULL;
	if (_wfopen_s(&fpOut, dstFile, _T("wb")) != 0 || !fpOut)
		return false;

	const DWORD BUFFER_SIZE = 1024 * 1024;
	BYTE* pBuffer = new BYTE[BUFFER_SIZE];
	if (!pBuffer) { fclose(fpOut); return false; }

	// 1단계: 모든 원본 파일의 strd 센서 데이터 추출 및 통합
	std::vector<BYTE> mergedStrdData;
	for (size_t i = 0; i < srcFiles.size(); ++i) {
		ExtractStrdData(srcFiles[i], mergedStrdData);
	}

	std::vector<AVI_INDEX_ENTRY> mergedIndex;
	DWORD dwTotalMoviDataSize = 0;
	DWORD streamLengths[8] = { 0 }; // 0: 00dc, 1: 01dc, 2: 02dc, 3: 03wb, etc.
	DWORD dwMainVideoFrames = 0;

	// 2단계: 첫 번째 파일의 헤더 복사 및 새 strd 주입
	FILE* fpFirst = NULL;
	if (_wfopen_s(&fpFirst, srcFiles[0], _T("rb")) != 0 || !fpFirst) {
		delete[] pBuffer; fclose(fpOut); return false;
	}

	BYTE riffHeader[12];
	fread(riffHeader, 1, 12, fpFirst);
	fwrite(riffHeader, 1, 12, fpOut);

	DWORD fourCC = 0, listSize = 0, listType = 0;
	while (fread(&fourCC, 4, 1, fpFirst) == 1) {
		fread(&listSize, 4, 1, fpFirst);
		if (fourCC == 0x5453494C) { // 'LIST'
			fread(&listType, 4, 1, fpFirst);
			if (listType == 0x6C726468) { // 'hdrl'
				fwrite(&fourCC, 4, 1, fpOut);
				long hdrlSizePos = ftell(fpOut);
				fwrite(&listSize, 4, 1, fpOut);
				fwrite(&listType, 4, 1, fpOut);

				long hdrlStartPos = ftell(fpOut);
				long hdrlEnd = ftell(fpFirst) + (listSize - 4);

				while (ftell(fpFirst) < hdrlEnd) {
					DWORD subTag = 0, subSize = 0;
					if (fread(&subTag, 4, 1, fpFirst) != 1) break;
					if (fread(&subSize, 4, 1, fpFirst) != 1) break;

					if (subTag == 0x5453494C) { // sub-LIST ('strl')
						DWORD subListType = 0;
						fread(&subListType, 4, 1, fpFirst);
						fwrite(&subTag, 4, 1, fpOut);
						long strlSizePos = ftell(fpOut);
						fwrite(&subSize, 4, 1, fpOut);
						fwrite(&subListType, 4, 1, fpOut);

						long strlStartPos = ftell(fpOut);
						long strlEnd = ftell(fpFirst) + (subSize - 4);

						while (ftell(fpFirst) < strlEnd) {
							DWORD leafTag = 0, leafSize = 0;
							if (fread(&leafTag, 4, 1, fpFirst) != 1) break;
							if (fread(&leafSize, 4, 1, fpFirst) != 1) break;

							if (leafTag == 0x64727473 && !mergedStrdData.empty()) { // 'strd' 교체
								DWORD newStrdSize = (DWORD)(mergedStrdData.size() + 4);
								DWORD fcc = 0;
								fread(&fcc, 4, 1, fpFirst);
								fseek(fpFirst, leafSize - 4, SEEK_CUR);

								fwrite(&leafTag, 4, 1, fpOut);
								fwrite(&newStrdSize, 4, 1, fpOut);
								fwrite(&fcc, 4, 1, fpOut);
								fwrite(&mergedStrdData[0], 1, mergedStrdData.size(), fpOut);
							}
							else {
								fwrite(&leafTag, 4, 1, fpOut);
								fwrite(&leafSize, 4, 1, fpOut);
								DWORD rem = (leafSize + 1) & ~1;
								while (rem > 0) {
									DWORD rb = min(rem, BUFFER_SIZE);
									fread(pBuffer, 1, rb, fpFirst);
									fwrite(pBuffer, 1, rb, fpOut);
									rem -= rb;
								}
							}
						}

						// sub-LIST ('strl') 크기 보정
						long curStrlPos = ftell(fpOut);
						DWORD actualStrlSize = (DWORD)(curStrlPos - strlStartPos + 4);
						fseek(fpOut, strlSizePos, SEEK_SET);
						fwrite(&actualStrlSize, 4, 1, fpOut);
						fseek(fpOut, curStrlPos, SEEK_SET);
					}
					else if (subTag == 0x64727473 && !mergedStrdData.empty()) {
						DWORD newStrdSize = (DWORD)(mergedStrdData.size() + 4);
						DWORD fcc = 0;
						fread(&fcc, 4, 1, fpFirst);
						fseek(fpFirst, subSize - 4, SEEK_CUR);

						fwrite(&subTag, 4, 1, fpOut);
						fwrite(&newStrdSize, 4, 1, fpOut);
						fwrite(&fcc, 4, 1, fpOut);
						fwrite(&mergedStrdData[0], 1, mergedStrdData.size(), fpOut);
					}
					else {
						fwrite(&subTag, 4, 1, fpOut);
						fwrite(&subSize, 4, 1, fpOut);
						DWORD rem = (subSize + 1) & ~1;
						while (rem > 0) {
							DWORD rb = min(rem, BUFFER_SIZE);
							fread(pBuffer, 1, rb, fpFirst);
							fwrite(pBuffer, 1, rb, fpOut);
							rem -= rb;
						}
					}
				}

				// hdrl LIST 크기 보정
				long curHdrlPos = ftell(fpOut);
				DWORD actualHdrlSize = (DWORD)(curHdrlPos - hdrlStartPos + 4);
				fseek(fpOut, hdrlSizePos, SEEK_SET);
				fwrite(&actualHdrlSize, 4, 1, fpOut);
				fseek(fpOut, curHdrlPos, SEEK_SET);
				break;
			}
		}
		fseek(fpFirst, listSize, SEEK_CUR);
	}
	fclose(fpFirst);

	// 3단계: 'LIST' [moviSize] 'movi' 헤더 위치 기록
	long moviListHeaderPos = ftell(fpOut);
	DWORD dwMoviTag = 0x5453494C;
	DWORD dwMoviDummySize = 0;
	DWORD dwMoviType = 0x69766F6D;
	fwrite(&dwMoviTag, 4, 1, fpOut);
	fwrite(&dwMoviDummySize, 4, 1, fpOut);
	fwrite(&dwMoviType, 4, 1, fpOut);

	// 4단계: 각 원본 파일의 movi 데이터 복사 및 스트림별 프레임/오디오 독립 카운팅
	for (size_t i = 0; i < srcFiles.size(); ++i)
	{
		FILE* fpIn = NULL;
		if (_wfopen_s(&fpIn, srcFiles[i], _T("rb")) != 0 || !fpIn)
			continue;

		DWORD curMoviBaseOffset = dwTotalMoviDataSize + 4; // 'movi' FourCC 다음부터의 오프셋
		fseek(fpIn, 12, SEEK_SET);

		while (fread(&fourCC, 4, 1, fpIn) == 1)
		{
			if (fread(&listSize, 4, 1, fpIn) != 1) break;

			if (fourCC == 0x5453494C) { // 'LIST'
				fread(&listType, 4, 1, fpIn);
				if (listType == 0x69766F6D) { // 'movi'
					DWORD remain = listSize - 4;
					dwTotalMoviDataSize += remain;
					while (remain > 0) {
						DWORD readBytes = min(remain, BUFFER_SIZE);
						fread(pBuffer, 1, readBytes, fpIn);
						fwrite(pBuffer, 1, readBytes, fpOut);
						remain -= readBytes;
					}
				}
				else {
					fseek(fpIn, listSize - 4, SEEK_CUR);
				}
			}
			else if (fourCC == 0x31786469) { // 'idx1'
				DWORD entryCount = listSize / sizeof(AVI_INDEX_ENTRY);
				for (DWORD k = 0; k < entryCount; ++k) {
					AVI_INDEX_ENTRY entry;
					if (fread(&entry, sizeof(AVI_INDEX_ENTRY), 1, fpIn) == 1) {
						entry.dwOffset += (curMoviBaseOffset - 4); // movi 상대 오프셋 정밀 보정
						mergedIndex.push_back(entry);

						// 스트림 번호 파싱 ('00dc' -> 0, '01dc' -> 1, '02dc' -> 2, '03wb' -> 3)
						char ch0 = (char)(entry.dwFourCC & 0xFF);
						char ch1 = (char)((entry.dwFourCC >> 8) & 0xFF);
						char ch2 = (char)((entry.dwFourCC >> 16) & 0xFF);
						char ch3 = (char)((entry.dwFourCC >> 24) & 0xFF);

						if (ch0 >= '0' && ch0 <= '7' && ch1 >= '0' && ch1 <= '9') {
							int streamIdx = (ch0 - '0') * 10 + (ch1 - '0');
							if (streamIdx < 8) {
								if (ch2 == 'w' && ch3 == 'b') {
									// 오디오 스트림: 바이트(또는 샘플) 길이 누적
									streamLengths[streamIdx] += entry.dwLength;
								}
								else {
									// 비디오 스트림: 프레임 수 누적
									streamLengths[streamIdx]++;
									if (streamIdx == 0) {
										dwMainVideoFrames++;
									}
								}
							}
						}
					}
				}
			}
			else {
				fseek(fpIn, listSize, SEEK_CUR);
			}
		}
		fclose(fpIn);
	}

	// 5단계: 통합 idx1 인덱스 테이블 기록
	DWORD dwIdx1Tag = 0x31786469;
	DWORD dwIdx1Size = (DWORD)(mergedIndex.size() * sizeof(AVI_INDEX_ENTRY));
	fwrite(&dwIdx1Tag, 4, 1, fpOut);
	fwrite(&dwIdx1Size, 4, 1, fpOut);
	if (!mergedIndex.empty()) {
		fwrite(&mergedIndex[0], sizeof(AVI_INDEX_ENTRY), mergedIndex.size(), fpOut);
	}

	// 6단계: 파일 크기 및 헤더 메타데이터 정밀 갱신
	long lTotalFileSize = ftell(fpOut);

	// (1) RIFF 전체 크기
	fseek(fpOut, 4, SEEK_SET);
	DWORD dwRiffSize = (DWORD)(lTotalFileSize - 8);
	fwrite(&dwRiffSize, 4, 1, fpOut);

	// (2) avih.dwTotalFrames (메인 비디오 총 프레임 수)
	// jun 260826 dwMicroSecPerFrame(오프셋 32) 훼손 방지 및 dwTotalFrames(오프셋 48) 정상 기록
	fseek(fpOut, 48, SEEK_SET);
	fwrite(&dwMainVideoFrames, 4, 1, fpOut);

	// (3) movi LIST 실제 크기
	fseek(fpOut, moviListHeaderPos + 4, SEEK_SET);
	DWORD dwFinalMoviListSize = dwTotalMoviDataSize + 4;
	fwrite(&dwFinalMoviListSize, 4, 1, fpOut);

	// (4) 각 스트림별 strh.dwLength에 해당 스트림 프레임 길이로 1:1 패치
	// jun 260826 LIST 4바이트 건너뛰기 추가로 17억 바이트 점프 루프 탈출 버그 수정
	fseek(fpOut, 12, SEEK_SET);
	int currentStreamIdx = 0;

	while (ftell(fpOut) < moviListHeaderPos) {
		DWORD tag = 0, size = 0;
		if (fread(&tag, 4, 1, fpOut) != 1) break;
		if (fread(&size, 4, 1, fpOut) != 1) break;

		if (tag == 0x5453494C) { // 'LIST' 태그 내부 진입 (4바이트 listType 건너뜀)
			DWORD listType = 0;
			if (fread(&listType, 4, 1, fpOut) != 1) break;
			continue;
		}

		if (tag == 0x68727473) { // 'strh' 발견
			long strhDataStart = ftell(fpOut);
			if (currentStreamIdx < 8 && streamLengths[currentStreamIdx] > 0) {
				// strh.dwLength 위치는 strh 본체 시작 기준 32번째 바이트
				fseek(fpOut, strhDataStart + 32, SEEK_SET);
				fwrite(&streamLengths[currentStreamIdx], 4, 1, fpOut);
			}
			currentStreamIdx++;
			fseek(fpOut, strhDataStart + ((size + 1) & ~1), SEEK_SET);
		}
		else if (tag != 0x46464952) { // 'RIFF' 외 일반 청크는 패딩 정렬 건너뜀
			fseek(fpOut, (size + 1) & ~1, SEEK_CUR);
		}
	}

	delete[] pBuffer;
	fclose(fpOut);
	return true;
}

/*
bool CWBCFileManager::fileMerge_Legacy_FFmpeg(CString strDestPath, bool is_Nxfs)
{
	// Legacy FFmpeg merge code (disabled)
	return false;
}
*/

#pragma pack(push, 1)
typedef struct {
	DWORD dwFourCC;
	DWORD dwFlags;
	DWORD dwOffset;
	DWORD dwLength;
} AVI_INDEX_ENTRY;
#pragma pack(pop)

// jun 260826 AVI 파일의 FPS (초당 프레임 수) 추출 헬퍼 함수
static DWORD GetAviFps(const CString& filePath)
{
	FILE* fp = NULL;
	if (_wfopen_s(&fp, filePath, _T("rb")) != 0 || !fp)
		return 0;

	// AVI Main Header (avih)의 dwMicroSecPerFrame (오프셋 32)
	fseek(fp, 32, SEEK_SET);
	DWORD dwMicroSecPerFrame = 0;
	if (fread(&dwMicroSecPerFrame, 4, 1, fp) != 1 || dwMicroSecPerFrame == 0) {
		fclose(fp);
		return 0;
	}

	fclose(fp);
	DWORD dwFps = (DWORD)((1000000.0 / (double)dwMicroSecPerFrame) + 0.5);
	return dwFps;
}

// 각 원본 파일(또는 LIST 'strl' 내부)을 재귀 탐색하여 strd (센서 데이터)를 100% 추출하는 함수
static bool ExtractStrdData(const CString& filePath, std::vector<BYTE>& outStrd)
{
	FILE* fp = NULL;
	if (_wfopen_s(&fp, filePath, _T("rb")) != 0 || !fp)
		return false;

	fseek(fp, 12, SEEK_SET);
	DWORD tag = 0, size = 0;
	bool found = false;

	while (fread(&tag, 4, 1, fp) == 1) {
		if (fread(&size, 4, 1, fp) != 1) break;

		if (tag == 0x5453494C) { // 'LIST'
			DWORD listType = 0;
			fread(&listType, 4, 1, fp);
			if (listType == 0x69766F6D) { // 'movi' 진입 전 헤더 탐색
				break;
			}
		}
		else if (tag == 0x64727473) { // 'strd' 발견!
			if (size > 4) {
				DWORD fcc = 0;
				fread(&fcc, 4, 1, fp);
				DWORD actualDataSize = size - 4;
				size_t oldSize = outStrd.size();
				outStrd.resize(oldSize + actualDataSize);
				fread(&outStrd[oldSize], 1, actualDataSize, fp);
				found = true;
				break;
			}
		}
		else {
			fseek(fp, (size + 1) & ~1, SEEK_CUR);
		}
	}

	fclose(fp);
	return found;
}

// 순수 C++ 초고속 무손실 AVI Merge (3ch 비디오 + 오디오 + GPS/G-Sensor 100% 무손실 결합)
static bool NativeAviMerge(const std::vector<CString>& srcFiles, const CString& dstFile)
{
	if (srcFiles.size() < 2) return false;

	FILE* fpOut = NULL;
	if (_wfopen_s(&fpOut, dstFile, _T("wb")) != 0 || !fpOut)
		return false;

	const DWORD BUFFER_SIZE = 1024 * 1024;
	BYTE* pBuffer = new BYTE[BUFFER_SIZE];
	if (!pBuffer) { fclose(fpOut); return false; }

	// 1단계: 모든 원본 파일의 strd 센서 데이터 추출 및 통합
	std::vector<BYTE> mergedStrdData;
	for (size_t i = 0; i < srcFiles.size(); ++i) {
		ExtractStrdData(srcFiles[i], mergedStrdData);
	}

	std::vector<AVI_INDEX_ENTRY> mergedIndex;
	DWORD dwTotalMoviDataSize = 0;
	DWORD streamLengths[8] = { 0 }; // 0: 00dc, 1: 01dc, 2: 02dc, 3: 03wb, etc.
	DWORD dwMainVideoFrames = 0;

	// 2단계: 첫 번째 파일의 헤더 복사 및 새 strd 주입
	FILE* fpFirst = NULL;
	if (_wfopen_s(&fpFirst, srcFiles[0], _T("rb")) != 0 || !fpFirst) {
		delete[] pBuffer; fclose(fpOut); return false;
	}

	BYTE riffHeader[12];
	fread(riffHeader, 1, 12, fpFirst);
	fwrite(riffHeader, 1, 12, fpOut);

	DWORD fourCC = 0, listSize = 0, listType = 0;
	while (fread(&fourCC, 4, 1, fpFirst) == 1) {
		fread(&listSize, 4, 1, fpFirst);
		if (fourCC == 0x5453494C) { // 'LIST'
			fread(&listType, 4, 1, fpFirst);
			if (listType == 0x6C726468) { // 'hdrl'
				fwrite(&fourCC, 4, 1, fpOut);
				long hdrlSizePos = ftell(fpOut);
				fwrite(&listSize, 4, 1, fpOut);
				fwrite(&listType, 4, 1, fpOut);

				long hdrlStartPos = ftell(fpOut);
				long hdrlEnd = ftell(fpFirst) + (listSize - 4);

				while (ftell(fpFirst) < hdrlEnd) {
					DWORD subTag = 0, subSize = 0;
					if (fread(&subTag, 4, 1, fpFirst) != 1) break;
					if (fread(&subSize, 4, 1, fpFirst) != 1) break;

					if (subTag == 0x5453494C) { // sub-LIST ('strl')
						DWORD subListType = 0;
						fread(&subListType, 4, 1, fpFirst);
						fwrite(&subTag, 4, 1, fpOut);
						long strlSizePos = ftell(fpOut);
						fwrite(&subSize, 4, 1, fpOut);
						fwrite(&subListType, 4, 1, fpOut);

						long strlStartPos = ftell(fpOut);
						long strlEnd = ftell(fpFirst) + (subSize - 4);

						while (ftell(fpFirst) < strlEnd) {
							DWORD leafTag = 0, leafSize = 0;
							if (fread(&leafTag, 4, 1, fpFirst) != 1) break;
							if (fread(&leafSize, 4, 1, fpFirst) != 1) break;

							if (leafTag == 0x64727473 && !mergedStrdData.empty()) { // 'strd' 교체
								DWORD newStrdSize = (DWORD)(mergedStrdData.size() + 4);
								DWORD fcc = 0;
								fread(&fcc, 4, 1, fpFirst);
								fseek(fpFirst, leafSize - 4, SEEK_CUR);

								fwrite(&leafTag, 4, 1, fpOut);
								fwrite(&newStrdSize, 4, 1, fpOut);
								fwrite(&fcc, 4, 1, fpOut);
								fwrite(&mergedStrdData[0], 1, mergedStrdData.size(), fpOut);
							}
							else {
								fwrite(&leafTag, 4, 1, fpOut);
								fwrite(&leafSize, 4, 1, fpOut);
								DWORD rem = (leafSize + 1) & ~1;
								while (rem > 0) {
									DWORD rb = min(rem, BUFFER_SIZE);
									fread(pBuffer, 1, rb, fpFirst);
									fwrite(pBuffer, 1, rb, fpOut);
									rem -= rb;
								}
							}
						}

						// sub-LIST ('strl') 크기 보정
						long curStrlPos = ftell(fpOut);
						DWORD actualStrlSize = (DWORD)(curStrlPos - strlStartPos + 4);
						fseek(fpOut, strlSizePos, SEEK_SET);
						fwrite(&actualStrlSize, 4, 1, fpOut);
						fseek(fpOut, curStrlPos, SEEK_SET);
					}
					else if (subTag == 0x64727473 && !mergedStrdData.empty()) {
						DWORD newStrdSize = (DWORD)(mergedStrdData.size() + 4);
						DWORD fcc = 0;
						fread(&fcc, 4, 1, fpFirst);
						fseek(fpFirst, subSize - 4, SEEK_CUR);

						fwrite(&subTag, 4, 1, fpOut);
						fwrite(&newStrdSize, 4, 1, fpOut);
						fwrite(&fcc, 4, 1, fpOut);
						fwrite(&mergedStrdData[0], 1, mergedStrdData.size(), fpOut);
					}
					else {
						fwrite(&subTag, 4, 1, fpOut);
						fwrite(&subSize, 4, 1, fpOut);
						DWORD rem = (subSize + 1) & ~1;
						while (rem > 0) {
							DWORD rb = min(rem, BUFFER_SIZE);
							fread(pBuffer, 1, rb, fpFirst);
							fwrite(pBuffer, 1, rb, fpOut);
							rem -= rb;
						}
					}
				}

				// hdrl LIST 크기 보정
				long curHdrlPos = ftell(fpOut);
				DWORD actualHdrlSize = (DWORD)(curHdrlPos - hdrlStartPos + 4);
				fseek(fpOut, hdrlSizePos, SEEK_SET);
				fwrite(&actualHdrlSize, 4, 1, fpOut);
				fseek(fpOut, curHdrlPos, SEEK_SET);
				break;
			}
		}
		fseek(fpFirst, listSize, SEEK_CUR);
	}
	fclose(fpFirst);

	// 3단계: 'LIST' [moviSize] 'movi' 헤더 위치 기록
	long moviListHeaderPos = ftell(fpOut);
	DWORD dwMoviTag = 0x5453494C;
	DWORD dwMoviDummySize = 0;
	DWORD dwMoviType = 0x69766F6D;
	fwrite(&dwMoviTag, 4, 1, fpOut);
	fwrite(&dwMoviDummySize, 4, 1, fpOut);
	fwrite(&dwMoviType, 4, 1, fpOut);

	// 4단계: 각 원본 파일의 movi 데이터 복사 및 스트림별 프레임/오디오 독립 카운팅
	for (size_t i = 0; i < srcFiles.size(); ++i)
	{
		FILE* fpIn = NULL;
		if (_wfopen_s(&fpIn, srcFiles[i], _T("rb")) != 0 || !fpIn)
			continue;

		DWORD curMoviBaseOffset = dwTotalMoviDataSize + 4; // 'movi' FourCC 다음부터의 오프셋
		fseek(fpIn, 12, SEEK_SET);

		while (fread(&fourCC, 4, 1, fpIn) == 1)
		{
			if (fread(&listSize, 4, 1, fpIn) != 1) break;

			if (fourCC == 0x5453494C) { // 'LIST'
				fread(&listType, 4, 1, fpIn);
				if (listType == 0x69766F6D) { // 'movi'
					DWORD remain = listSize - 4;
					dwTotalMoviDataSize += remain;
					while (remain > 0) {
						DWORD readBytes = min(remain, BUFFER_SIZE);
						fread(pBuffer, 1, readBytes, fpIn);
						fwrite(pBuffer, 1, readBytes, fpOut);
						remain -= readBytes;
					}
				}
				else {
					fseek(fpIn, listSize - 4, SEEK_CUR);
				}
			}
			else if (fourCC == 0x31786469) { // 'idx1'
				DWORD entryCount = listSize / sizeof(AVI_INDEX_ENTRY);
				for (DWORD k = 0; k < entryCount; ++k) {
					AVI_INDEX_ENTRY entry;
					if (fread(&entry, sizeof(AVI_INDEX_ENTRY), 1, fpIn) == 1) {
						entry.dwOffset += (curMoviBaseOffset - 4); // movi 상대 오프셋 정밀 보정
						mergedIndex.push_back(entry);

						// 스트림 번호 파싱 ('00dc' -> 0, '01dc' -> 1, '02dc' -> 2, '03wb' -> 3)
						char ch0 = (char)(entry.dwFourCC & 0xFF);
						char ch1 = (char)((entry.dwFourCC >> 8) & 0xFF);
						char ch2 = (char)((entry.dwFourCC >> 16) & 0xFF);
						char ch3 = (char)((entry.dwFourCC >> 24) & 0xFF);

						if (ch0 >= '0' && ch0 <= '7' && ch1 >= '0' && ch1 <= '9') {
							int streamIdx = (ch0 - '0') * 10 + (ch1 - '0');
							if (streamIdx < 8) {
								if (ch2 == 'w' && ch3 == 'b') {
									// 오디오 스트림: 바이트(또는 샘플) 길이 누적
									streamLengths[streamIdx] += entry.dwLength;
								}
								else {
									// 비디오 스트림: 프레임 수 누적
									streamLengths[streamIdx]++;
									if (streamIdx == 0) {
										dwMainVideoFrames++;
									}
								}
							}
						}
					}
				}
			}
			else {
				fseek(fpIn, listSize, SEEK_CUR);
			}
		}
		fclose(fpIn);
	}

	// 5단계: 통합 idx1 인덱스 테이블 기록
	DWORD dwIdx1Tag = 0x31786469;
	DWORD dwIdx1Size = (DWORD)(mergedIndex.size() * sizeof(AVI_INDEX_ENTRY));
	fwrite(&dwIdx1Tag, 4, 1, fpOut);
	fwrite(&dwIdx1Size, 4, 1, fpOut);
	if (!mergedIndex.empty()) {
		fwrite(&mergedIndex[0], sizeof(AVI_INDEX_ENTRY), mergedIndex.size(), fpOut);
	}

	// 6단계: 파일 크기 및 헤더 메타데이터 정밀 갱신
	long lTotalFileSize = ftell(fpOut);

	// (1) RIFF 전체 크기
	fseek(fpOut, 4, SEEK_SET);
	DWORD dwRiffSize = (DWORD)(lTotalFileSize - 8);
	fwrite(&dwRiffSize, 4, 1, fpOut);

	// (2) avih.dwTotalFrames (메인 비디오 총 프레임 수)
	fseek(fpOut, 32, SEEK_SET);
	fwrite(&dwMainVideoFrames, 4, 1, fpOut);

	// (3) movi LIST 최종 크기
	fseek(fpOut, moviListHeaderPos + 4, SEEK_SET);
	DWORD dwFinalMoviListSize = dwTotalMoviDataSize + 4;
	fwrite(&dwFinalMoviListSize, 4, 1, fpOut);

	// (4) 각 스트림별 strh.dwLength를 해당 스트림의 실제 길이로 1:1 정밀 갱신
	fseek(fpOut, 12, SEEK_SET);
	int currentStreamIdx = 0;

	while (ftell(fpOut) < moviListHeaderPos) {
		DWORD tag = 0, size = 0;
		if (fread(&tag, 4, 1, fpOut) != 1) break;
		if (fread(&size, 4, 1, fpOut) != 1) break;

		if (tag == 0x68727473) { // 'strh'
			long strhDataStart = ftell(fpOut);
			if (currentStreamIdx < 8 && streamLengths[currentStreamIdx] > 0) {
				// strh.dwLength 위치는 strh 데이터 시작 후 32번째 바이트
				fseek(fpOut, strhDataStart + 32, SEEK_SET);
				fwrite(&streamLengths[currentStreamIdx], 4, 1, fpOut);
			}
			currentStreamIdx++;
			fseek(fpOut, strhDataStart + size, SEEK_SET);
		}
		else if (tag != 0x5453494C && tag != 0x46464952) {
			fseek(fpOut, size, SEEK_CUR);
		}
	}

	delete[] pBuffer;
	fclose(fpOut);
	return true;
}

bool CWBCFileManager::fileMerge(CString strDestPath, bool is_Nxfs)
{
	bool bSuccess = FALSE;
	int nFileCount = (int)this->m_listAviFilePath.size();

	if (nFileCount == 0)
		return FALSE;

	CString cstrPath(strDestPath);
	CString cstrMergeFileName(strDestPath);

	if (strDestPath.ReverseFind('\\') != strDestPath.GetLength() - 1) {
		cstrPath.Append(_T("\\"));
		cstrMergeFileName.Append(_T("\\"));
	}

	// 1. FVFS 암호화 해제 및 소스 파일 목록 수집
	std::vector<CString> srcFiles;
	ITER_AVI_FILE iFile = this->m_listAviFilePath.begin();
	ITER_AVI_FILE iFileEnd = this->m_listAviFilePath.end();

	for (; iFile != iFileEnd; iFile++)
	{
		if (iFile->Find(_T("fvfs")) >= 0) {
			CT2A ascii(iFile->GetBuffer());
			security_file_change(ascii.m_psz, false);
		}
		srcFiles.push_back(iFile->GetBuffer());
	}

	// jun 260826 다중 파일 머지 시 프레임 레이트(FPS) 불일치 검사 및 다국어 팝업 안내
	if (nFileCount > 1) {
		DWORD dwBaseFps = 0;
		for (size_t i = 0; i < srcFiles.size(); ++i) {
			DWORD dwCurFps = GetAviFps(srcFiles[i]);
			if (i == 0) {
				dwBaseFps = dwCurFps;
			}
			else if (dwBaseFps != 0 && dwCurFps != 0 && abs((int)dwBaseFps - (int)dwCurFps) >= 2) {
				// FPS 불일치 감지 (예: 15fps vs 30fps) -> 다국어 안내 팝업 출력 후 안전하게 중단
#if (BUILD_LANGUAGE == LANGUAGE_JAPANESE)
				AfxMessageBox(_T("選?されたファイルの中にフレ?ムレ?トが異なるファイルが含まれています。同じ??モ?ドのファイルを選?してください。"), MB_ICONWARNING);
#elif (BUILD_LANGUAGE == LANGUAGE_KOREAN)
				AfxMessageBox(_T("선택된 파일 중 프레임 레이트가 다른 파일이 포함되어 있습니다. 동일한 녹화 모드의 파일을 선택해 주세요."), MB_ICONWARNING);
#else
				AfxMessageBox(_T("Selected files have different frame rates. Please select files with the same recording mode."), MB_ICONWARNING);
#endif
				return FALSE;
			}
		}
	}

	// 2. 단일 파일 백업: 기존 분리 로직 수행
	if (nFileCount == 1) {
		if (is_Nxfs) {
			bSuccess = fileSeparate(this->m_listAviFilePath.begin()->GetBuffer(), TEXT("avi"), is_Nxfs);
		}
		else {
			CString csNewFilePath(cstrPath);
			csNewFilePath.Append(ClUtil::PATH::getFileNameSpecW(this->m_listAviFilePath.begin()->GetBuffer()).c_str());
			csNewFilePath.Append(TEXT(".avi"));

			if (::CopyFile(this->m_listAviFilePath.begin()->GetBuffer(), csNewFilePath.GetBuffer(), FALSE))
				bSuccess = fileSeparate(csNewFilePath.GetBuffer(), TEXT("avi"), is_Nxfs);
		}
	}
	// 3. 다중 파일 머지: 순수 C++ 무손실 머지 + 4개 파일 자동 생성
	else {
		// 최종 머지 파일명: [시작파일명]-[종료파일명].avi
		cstrMergeFileName.Append(ClUtil::PATH::getFileNameSpecW(this->m_listAviFilePath.front().GetBuffer()).c_str());
		cstrMergeFileName.Append(_T("-"));
		cstrMergeFileName.Append(ClUtil::PATH::getFileNameSpecW(this->m_listAviFilePath.back().GetBuffer()).c_str());
		cstrMergeFileName.Append(_T(".avi"));

		if (_taccess(cstrMergeFileName.GetBuffer(), 0) != -1)
			CFile::Remove(cstrMergeFileName.GetBuffer());

		// 1단계: 순수 C++ 초고속 무손실 머지 실행 (모든 채널 및 GPS/G-Sensor 100% 결합!)
		bSuccess = NativeAviMerge(srcFiles, cstrMergeFileName);

		if (bSuccess)
		{
			// 2단계: 결합된 파일로부터 채널별 분리 파일(_1, _2, _3) 생성 -> 총 4개 파일 완성!
			fileSeparate(cstrMergeFileName, TEXT("avi"), is_Nxfs);
		}
	}

	// 4. 후처리: NxFS 임시 파일 삭제 및 FVFS 재암호화
	iFile = this->m_listAviFilePath.begin();
	iFileEnd = this->m_listAviFilePath.end();

	for (; iFile != iFileEnd; iFile++)
	{
		if (is_Nxfs && nFileCount > 1) {
			CFile::Remove(iFile->GetBuffer());
		}
		if (iFile->Find(_T("fvfs")) >= 0) {
			CT2A ascii(iFile->GetBuffer());
			security_file_change(ascii.m_psz, true);
		}
	}

	return bSuccess;
}

bool CWBCFileManager::fileSeparate(CString cstrNewFileName, CString saveFileExt, bool is_Nxfs)
{
	TCHAR	szCommand[MAX_PATH];
	bool bFileSaveOk = FALSE;	
	bool	bSuccess;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	TCHAR *szP;
	int i;

	std::wstring wtsFileName = ClUtil::PATH::getFileNameSpecW( cstrNewFileName);
	std::wstring wtsFilePath = ClUtil::PATH::getPathSpecW( cstrNewFileName);
	CString cstrFileName = wtsFileName.c_str();
	cstrFileName += TEXT(".");
	cstrFileName += TEXT("avi");

	CString saveDstFile;
	saveDstFile.Format(TEXT("%s%s%s"), wtsFilePath.c_str(), TEXT("\\"), wtsFileName.c_str());

	CString saveCurrentFileName = wtsFilePath.c_str();
	saveCurrentFileName += TEXT("\\") + cstrFileName;

	std::wstring wtsPath = ClUtil::PATH::getPathSpecW( saveDstFile.GetBuffer());
	CString mp4SavePath;
	mp4SavePath.Format(TEXT("%s\\"), wtsPath.c_str());
		
	saveCurrentFileName += TEXT('\0');
	saveDstFile += TEXT('\0');
	
	std::wstring wtsPathName = ClUtil::PATH::getPathSpecW( saveDstFile.GetBuffer() );
	saveDstFile.Format(TEXT("%s\\%s"),  wtsPathName.c_str(), wtsFileName.c_str());


//// ++{ 20201026 FVFS Added *******************
	//char str_fvfs_header[5] = {0, };
	//if(CBBoxUtil::GetFileData(cstrNewFileName, str_fvfs_header, 4)){
		//if(strcmp(str_fvfs_header, "FVFS") == 0 ) {
		//	const char avi_riff_hd[16] = { 0x52, 0x49, 0x46, 0x46, 0xf8, 0xff, 0xbf, 0x00, 0x41, 0x56, 0x49, 0x20, 0x4c, 0x49, 0x53, 0x54 };
		//	if(!CBBoxUtil::ReplaceFileData(cstrNewFileName, avi_riff_hd, 16, 0)){
		//	}
		//}
	//}
	
	//if(cstrNewFileName.Find(_T("fvfs")) >= 0 )
	{
		CT2A ascii(cstrNewFileName);
		security_file_change(ascii.m_psz, false);
	}
	// 1ch은 분리하지 않음
	std::reverse_iterator<std::wstring::iterator> rit = wtsFileName.rbegin();
	
	if(rit[0] == L'1')
		return TRUE;
//// ++}***************************************




//전방만 동작하도록안책임 

#ifdef REAR_CAM_OFF
	int channel_count = 1;		
#else
	int channel_count = 2;
	if(rit[0] == L'3')
		channel_count = 3;
#endif	
	
	for(i = 0; i <  channel_count; i++)





//이전코드
/*
#ifdef REAR_CAM_OFF
	int channel_count = 1;	

	for(i = 0; i <  1; i++)   // jun 170407 : rear ch skip
		
#else

	int channel_count = 2;
	if(rit[0] == L'3')
		channel_count = 3;
	
	for(i = 0; i <  channel_count; i++)
		
#endif
*/




// 2채널 코드
/*
#ifdef REAR_CAM_OFF

	for(i = 0; i <  1; i++)   // jun 170407 : rear ch skip
		
#else

	for(i = 0; i <  2; i++)
		
#endif
*/




//	for(i=0, i=1; i < 1; i++, i < 2) // 20101125: front, rear ch skip
//	for(i = 1; i <  2; i++) //20160520 : front ch skip
	{
		int j;
		memset( &si, 0, sizeof(si));
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE; //SW_SHOW;

		for( j = 0; j < 2; j++)
		{
			CString cstrCmdOpt;
			CString cstrOutFileName;
			
			ZeroMemory(&pi, sizeof(pi));


			cstrOutFileName.Format(TEXT("%sPlease_wait__%d_%d_temp.%s"), mp4SavePath, i, j, saveFileExt);


#ifdef REAR_CAM_OFF2
			
			if( i == 1)
  			continue; // 실내 스킵
			

 #else

 

 #endif
			if(j== 0) { // av //20201026 j== 1 ==> 0 (2채널 백업: F,R파일에  Audio없음 문제 수정)


				//cstrCmdOpt.Format(TEXT("-vcodec copy -acodec copy -map 0:%d -map 0:0"), i+1);
				if(channel_count == 3)
					cstrCmdOpt.Format(TEXT("-vcodec copy -acodec copy -map 0:%d -map 0:3"), i);
				else
					cstrCmdOpt.Format(TEXT("-vcodec copy -acodec copy -map 0:%d -map 0:2"), i);
			}
			else // video only
				cstrCmdOpt.Format(TEXT("-vcodec copy -map 0:%d"), i);
			if(is_Nxfs)
				wsprintf( szCommand, _T("ffmpeg_nxfs.exe -y -i  \"%s\" %s \"%s\""),  saveCurrentFileName, cstrCmdOpt, cstrOutFileName);
           else
				wsprintf( szCommand, _T("ffmpeg.exe -y -i  \"%s\" %s \"%s\""),  saveCurrentFileName, cstrCmdOpt, cstrOutFileName);
				   
			bSuccess = CreateProcess(NULL,szCommand,NULL,NULL,NULL,CREATE_NEW_CONSOLE,NULL,NULL,&si,&pi);
#if 0 //debug				

			CFile file;
			CString cstrDebugFile;
			cstrDebugFile.Format(TEXT("%s_%d.txt"), cstrDstFile.GetBuffer() , i+j);
			if(file.Open(cstrDebugFile.GetBuffer(), CFile::modeCreate | CFile::modeReadWrite))
			{
				CString msg = szCommand;
				file.Write((void *)msg.GetBuffer(), msg.GetLength() * 2);
				file.Close();
			}
#endif

			if(bSuccess == TRUE)
			{
#if 1						
				DWORD dwResult = WaitForSingleObject(pi.hProcess, 300000); // wait 5min
				
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);

				if(dwResult != WAIT_OBJECT_0)
				{
					CLD_FL( DF_ERROR, "File copying failed." );
					//OPEN_NOTIFY_DIALOG( NOTIFY_ERR_COPY_FILE ,&this->m_pUIControl->m_Language);
				}
				else
				{
					HANDLE hFile = INVALID_HANDLE_VALUE;
					DWORD dwFileSize = 0;

					hFile = ::CreateFile( cstrOutFileName.GetBuffer(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					if(hFile != INVALID_HANDLE_VALUE)
					{
						dwFileSize = ::GetFileSize(hFile, NULL);
						::CloseHandle(hFile);									
					
						if(dwFileSize)
						{
							CString cstrNewFileName;

							bFileSaveOk = TRUE;

							if(channel_count == 2)
								cstrNewFileName.Format(TEXT("%s_%s.%s"), saveDstFile, (i == 0) ? TEXT("F") : TEXT("R"),  saveFileExt);
							else
								cstrNewFileName.Format(TEXT("%s_%s.%s"), saveDstFile, (i == 0) ? TEXT("1") : (i == 1) ? TEXT("2") : TEXT("3"),  saveFileExt);
								//cstrNewFileName.Format(TEXT("%s_%d.%s"), saveDstFile, i + 1,  saveFileExt);
	

							//same file name check and delete
							hFile = ::CreateFile( cstrNewFileName.GetBuffer(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
							if(hFile != INVALID_HANDLE_VALUE)
							{
								::CloseHandle(hFile);									
								CFile::Remove(cstrNewFileName.GetBuffer());
							}
							
							//rename
							CFile::Rename( cstrOutFileName.GetBuffer(), cstrNewFileName.GetBuffer());
							if( j == 0)
								break;
						}
						else //delete file
						{
							CFile::Remove(cstrOutFileName.GetBuffer());
						}
					}
				}
#else
				DWORD dwResult = WaitForSingleObject(pi.hProcess, 1000); // wait 1sec
				if(dwResult != WAIT_OBJECT_0)
				{
					bFileSaveOk = TRUE;
					if( j == 0)
						break;
				}
#endif
				
			}
			else
			{
				CLD_FL( DF_ERROR, "File copying failed." );
				//OPEN_NOTIFY_DIALOG( NOTIFY_ERR_COPY_FILE ,&this->m_pUIControl->m_Language);
			}
		}
	}

	return bFileSaveOk;
}



bool CWBCFileManager::addDedicatedDir( const CString& cstrDedDirName )
{
    ITER_DEDDIR iDDir = this->m_dedicatedDirList.begin();
    ITER_DEDDIR iEndDDir = this->m_dedicatedDirList.end();
    for( ; iDDir != iEndDDir; iDDir ++ )
    {
        if( ! iDDir->m_cstrDirName.Compare(cstrDedDirName) )
            break;
    }

    if( iDDir != this->m_dedicatedDirList.end() )
        return false;

    SDEDDIR dDir;
    dDir.m_cstrDirName = cstrDedDirName;

#if defined(__CLBB_NXFS_SUPPORT__)
    USES_CONVERSION;
    strcpy_s( dDir.m_strNxFSDirName, MAX_DIR_NAME_STR_SIZE, T2A(cstrDedDirName) );
#endif

    this->m_dedicatedDirList.push_back( dDir );

    return true;
}


bool CWBCFileManager::addExtFilter( const CString& cstrExtension )
{
    if( ! cstrExtension.GetLength() )
        return false;

    CString cstrNewExtenstion = cstrExtension;
    cstrNewExtenstion.MakeLower();

    ITER_EXTFILTER iExt = this->m_extFilterList.begin();
    ITER_EXTFILTER iEndExt = this->m_extFilterList.end();
    for( ; iExt != iEndExt; iExt++ )
    {
        if( !iExt->Compare(cstrNewExtenstion) )
            break;
    }

    if( iExt != iEndExt )
        return false;

    this->m_extFilterList.push_back( cstrNewExtenstion );

    return true;
}


bool CWBCFileManager::removeExtFilter( int nIndex )
{
    if( nIndex < 0 )
        return false;

    ITER_EXTFILTER iExt = this->m_extFilterList.begin();
    std::advance( iExt, nIndex );

    if( iExt == this->m_extFilterList.end() )
        return false;

    this->m_extFilterList.erase( iExt );

    return true;
}


bool CWBCFileManager::removeExtFilter( const CString& cstrExtension )
{
    if( !cstrExtension.GetLength() )
        return false;

    ITER_EXTFILTER iExt = this->m_extFilterList.begin();
    ITER_EXTFILTER iEndExt = this->m_extFilterList.end();
    for( ; iExt != iEndExt; iExt++ )
    {
        if( !iExt->Compare(cstrExtension) )
            break;
    }

    if( iExt != iEndExt )
        return false;

    this->m_extFilterList.erase( iExt );

    return true;
}


void CWBCFileManager::clearExtFilter( void )
{
    if( this->m_extFilterList.empty() )
        return;

    this->m_extFilterList.clear();

    return;
}


void CWBCFileManager::_clearDrives( void )
{
    if( ! this->m_driveList.empty() )
        this->m_driveList.clear();

    return;
}


void CWBCFileManager::_clearDirectories( void )
{
    if( ! this->m_dirList.empty() )
        this->m_dirList.clear();

    return;
}


void CWBCFileManager::_clearFiles( void )
{
    if( ! this->m_fileVec.empty() )
        this->m_fileVec.clear();

    return;
}


void CWBCFileManager::_sort( void )
{
    switch( this->m_eSortMethod )
    {
    case FM_SORT_NAME_TIME_SIZE_ASC:    std::sort( this->m_fileVec.begin(), this->m_fileVec.end(), CWBCFileManager::__ASC_Name_Time_Size ); break;
    case FM_SORT_TIME_NAME_SIZE_ASC:    std::sort( this->m_fileVec.begin(), this->m_fileVec.end(), CWBCFileManager::__ASC_Time_Name_Size ); break;
    case FM_SORT_NAME_TIME_SIZE_DEC:    std::sort( this->m_fileVec.begin(), this->m_fileVec.end(), CWBCFileManager::__DEC_Name_Time_Size ); break;
    case FM_SORT_TIME_NAME_SIZE_DEC:    std::sort( this->m_fileVec.begin(), this->m_fileVec.end(), CWBCFileManager::__DEC_Time_Name_Size ); break;
    }

    return;
}

//================================================================================================= E. CONTROL


//================================================================================================= S. GET/SET

bool CWBCFileManager::setParentWindow( HWND hParentWnd )
{
    if( hParentWnd == NULL )
        return false;

    this->m_hParentHwnd = hParentWnd;

    return true;
}


bool CWBCFileManager::setSelectDirString( const CString& cstrSelDir )
{
    if( ! cstrSelDir.GetLength() )
        return false;

    this->m_cstrSelectDir = cstrSelDir;

    return true;
}


bool CWBCFileManager::setFindDesktopFilesString( const CString& cstrFindDesktop )
{
    if( ! cstrFindDesktop.GetLength() )
        return false;

    this->m_cstrFindLocalFile = cstrFindDesktop;

    return true;
}


void CWBCFileManager::setRecursiveScan( bool bRemoval, bool bDesktop )
{
    this->m_bRemovalRecvScan = bRemoval;
    this->m_bDesktopRecvScan = bDesktop;

    return;
}


#if defined(__CLBB_NXFS_SUPPORT__)
bool CWBCFileManager::setNxFS2DesktopExt( const CString& cstrNxFS2DesktopExt )
{
    if( ! cstrNxFS2DesktopExt.GetLength() )
        return false;

    CString cstrNewExt = cstrNxFS2DesktopExt;
    cstrNewExt.MakeLower();

    if(     cstrNewExt.Compare(TEXT("avi"))
        &&  cstrNewExt.Compare(TEXT("mp4")) )
    {
        return false;
    }

    this->m_cstrNxFS2DesktopExt = cstrNewExt;

    return true;
}


DEV_HANDLE* CWBCFileManager::getNxFSHandle( void )
{
    return this->m_pNxFSHandle;
}
#endif


CWBCFileManager::EDRIVEMODE CWBCFileManager::getCurDriveMode( void )
{
    return this->m_eDriveMode;
}


const CWBCFileManager::PSDRIVEINFO CWBCFileManager::getCurDriveInfo( void )
{
    if( (this->m_nCurSelDrive < 0) || (this->m_nCurSelDrive >= (int)this->m_driveList.size()) )
        return NULL;

    ITER_DRIVEINFO iDriveInfo = this->m_driveList.begin();
    std::advance( iDriveInfo, this->m_nCurSelDrive );

    return &(*iDriveInfo);
}


int CWBCFileManager::getDrives( void )
{
    return this->m_driveList.size();
}


int CWBCFileManager::getDirectories( void )
{
    return this->m_dirList.size();
}


int CWBCFileManager::getFiles( void )
{
    return this->m_fileVec.size();
}


const CWBCFileManager::PSDRIVEINFO CWBCFileManager::getDriveInfo( int nIndex )
{
    if( nIndex >= 0 )
    {
        ITER_DRIVEINFO iDrive = this->m_driveList.begin();
        std::advance( iDrive, nIndex );

        if( iDrive != this->m_driveList.end() )
            return &(*iDrive);
    }

    return NULL;
}


const CWBCFileManager::PSDIRINFO CWBCFileManager::getDirectoryInfo( int nIndex )
{
    if( nIndex >= 0 )
    {
        ITER_DIRINFO iDir = this->m_dirList.begin();
        std::advance( iDir, nIndex );

        if( iDir != this->m_dirList.end() )
            return &(*iDir);
    }

    return NULL;
}


const CWBCFileManager::PSFILEINFO CWBCFileManager::getFileInfo( int nIndex )
{
    if( nIndex >= 0 )
    {
        ITER_FILEINFO iFile = this->m_fileVec.begin();
        std::advance( iFile, nIndex );

        if( iFile != this->m_fileVec.end() )
            return &(*iFile);
    }

    return NULL;
}


int CWBCFileManager::getListupFiles( void )
{
    return this->m_ListupFileList.size();
}


const CWBCFileManager::PSFILEINFO CWBCFileManager::getListupFileInfo( int nIndex )
{
    if( nIndex >= 0 )
    {
        ITER_LISTUP iListUp = this->m_ListupFileList.begin();
        std::advance( iListUp, nIndex );

        if( iListUp != this->m_ListupFileList.end() )
            return (*iListUp);
    }

    return NULL;
}


int CWBCFileManager::getExtFilterListsize( void )
{
    return this->m_extFilterList.size();
}


bool CWBCFileManager::getExtFilter( int nIndex, CString& cstrExtension )
{
    ITER_EXTFILTER iExt = this->m_extFilterList.begin();
    std::advance( iExt, nIndex );

    if( iExt == this->m_extFilterList.end() )
        return false;

    cstrExtension = (*iExt);

    return true;
}


void CWBCFileManager::setSortMethod( EFILEMANAGER_SORT eSortMethod )
{
    if( eSortMethod == FM_SORT_INVALID )
        return;

    this->m_eSortMethod = eSortMethod;

    return;
}


void CWBCFileManager::setUseDirSort( bool bUseDirSort )
{
    CWBCFileManager::m_sUseDirSort = bUseDirSort;

    return;
}

//================================================================================================= E. GET/SET
//================================================================================================= S. STATIC

bool CWBCFileManager::__ASC_Name_Time_Size ( const SFILEINFO& lhs, const SFILEINFO& rhs )
{
    if( CWBCFileManager::m_sUseDirSort )
    {
        if( lhs.m_dirIndex > rhs.m_dirIndex )   return true;
        if( lhs.m_dirIndex < rhs.m_dirIndex )   return false;
    }

    int nNameComapre = lhs.m_cstrFileName.Compare( rhs.m_cstrFileName );
    if( nNameComapre > 0 )                  return false;
    if( nNameComapre < 0 )                  return true;

    nNameComapre = ::CompareFileTime( &lhs.m_ftFileTime, &rhs.m_ftFileTime );
    if( nNameComapre > 0 )                  return false;
    if( nNameComapre < 0 )                  return true;

    return lhs.m_llFileSize > rhs.m_llFileSize ? true : false;
}


bool CWBCFileManager::__ASC_Time_Name_Size ( const SFILEINFO& lhs, const SFILEINFO& rhs )
{
    if( CWBCFileManager::m_sUseDirSort )
    {
        if( lhs.m_dirIndex > rhs.m_dirIndex )   return true;
        if( lhs.m_dirIndex < rhs.m_dirIndex )   return false;
    }

    int nNameComapre = ::CompareFileTime( &lhs.m_ftFileTime, &rhs.m_ftFileTime );
    if( nNameComapre > 0 )                  return false;
    if( nNameComapre < 0 )                  return true;

    nNameComapre = lhs.m_cstrFileName.Compare( rhs.m_cstrFileName );
    if( nNameComapre > 0 )                  return false;
    if( nNameComapre < 0 )                  return true;

    return lhs.m_llFileSize > rhs.m_llFileSize ? false : true;
}


bool CWBCFileManager::__DEC_Name_Time_Size ( const SFILEINFO& lhs, const SFILEINFO& rhs )
{
    if( CWBCFileManager::m_sUseDirSort )
    {
        if( lhs.m_dirIndex > rhs.m_dirIndex )   return true;
        if( lhs.m_dirIndex < rhs.m_dirIndex )   return false;
    }

    int nNameComapre = lhs.m_cstrFileName.Compare( rhs.m_cstrFileName );
    if( nNameComapre > 0 )                  return true;
    if( nNameComapre < 0 )                  return false;

    nNameComapre = ::CompareFileTime( &lhs.m_ftFileTime, &rhs.m_ftFileTime );
    if( nNameComapre > 0 )                  return true;
    if( nNameComapre < 0 )                  return false;

    return lhs.m_llFileSize > rhs.m_llFileSize ? true : false;
}


bool CWBCFileManager::__DEC_Time_Name_Size ( const SFILEINFO& lhs, const SFILEINFO& rhs )
{
    if( CWBCFileManager::m_sUseDirSort )
    {
        if( lhs.m_dirIndex > rhs.m_dirIndex )   return true;
        if( lhs.m_dirIndex < rhs.m_dirIndex )   return false;
    }

    int nNameComapre = ::CompareFileTime( &lhs.m_ftFileTime, &rhs.m_ftFileTime );
    if( nNameComapre > 0 )                  return true;
    if( nNameComapre < 0 )                  return false;

    nNameComapre = lhs.m_cstrFileName.Compare( rhs.m_cstrFileName );
    if( nNameComapre > 0 )                  return true;
    if( nNameComapre < 0 )                  return false;

    return lhs.m_llFileSize > rhs.m_llFileSize ? true : false;
}


int CALLBACK CWBCFileManager::__BrowseCallbackProc( HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData )
{
    UNREFERENCED_PARAMETER( lParam );

    switch( uMsg )
    {
    case BFFM_INITIALIZED :
        {
            HWND hShell = GetDlgItem(hwnd, 0);      // 0x00000000(Shell Class)
            HWND hTree  = GetDlgItem(hShell, 100);  // 0x00000064(Tree Control)
            HWND hNew   = GetDlgItem(hwnd, 14150);  // 0x00003746(New Folder Button)
            HWND hOK    = GetDlgItem(hwnd, 1);      // 0x00000001(OK Button)
            HWND hCancel= GetDlgItem(hwnd, 2);      // 0x00000002(Cancel Button)
            HWND hStatic= GetDlgItem(hwnd, 14146);  // 0x00003742(Static Control)

            if ( !hShell || !hTree || !hNew || !hOK || !hCancel )
                return 0;

            // Deleting upper status
            if( hStatic ) 
            {
                EnableWindow( hStatic, FALSE );
                ShowWindow( hStatic, SW_HIDE );
            }

            // initial path
            if( lpData != NULL )
            {
                ::SendMessage( hwnd, BFFM_SETSELECTION, TRUE, lpData );
            }

            CRect rectWnd;
            CRect rectNew;
            CRect rectOK;
            CRect rectCancel;
            GetClientRect( hwnd, &rectWnd );
            GetClientRect( hNew, &rectNew );
            GetClientRect( hOK, &rectOK );
            GetClientRect( hCancel, &rectCancel );

            // tree view size
            MoveWindow( hShell,
                        rectWnd.left + 10,
                        rectWnd.top  + 10,
                        rectWnd.Width()-22,
                        rectWnd.Height() - rectNew.Height() - 30,
                        TRUE );

            // new folder button
            MoveWindow( hNew,
                        rectWnd.left + 10,
                        rectWnd.bottom-rectNew.Height() - 10,
                        rectNew.Width(),
                        rectNew.Height(),
                        TRUE );

            // OK Button
            MoveWindow( hOK,
                        rectWnd.right-10-rectCancel.Width()-5-rectOK.Width(),
                        rectWnd.bottom-rectOK.Height()-10,
                        rectOK.Width(),
                        rectOK.Height(),
                        TRUE );

            // CANCEL Button
            MoveWindow( hCancel,
                        rectWnd.right-10-rectCancel.Width(),
                        rectWnd.bottom-rectCancel.Height()-10,
                        rectCancel.Width(),
                        rectCancel.Height(),
                        TRUE );
        } break;

    case BFFM_SELCHANGED :
    case BFFM_VALIDATEFAILED :
        break;
    }

    return 0;
}

//================================================================================================= E. STATIC
//================================================================================================= EOF
