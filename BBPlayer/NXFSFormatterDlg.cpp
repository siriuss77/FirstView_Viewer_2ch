
// NXFSFormatterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BBPlayer.h"
#include "NXFSFormatterDlg.h"
#include <WinIoCtl.h>
#include <CLUDE/version.h>
#include <CLENAW/ClImage.h>
#include <CLENAW/ClText.h>
#include <CLENAW/ClButton.h>
#include <CLENAW/ClSlider.h>
#include <HELPER/ClLog.H>
#include <HELPER/CLUtil.H>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNXFSFormatterDlg dialog
CNXFSFormatterDlg::CNXFSFormatterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNXFSFormatterDlg::IDD, pParent)
    , m_cstrStorageInfo(_T(""))
    , m_cstrWarning(_T(""))
    , m_cstrProgress(_T(""))
{
	this->m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    this->m_nCurDriveIndex = -1;
    this->m_bIsFormatting = FALSE;
}

void CNXFSFormatterDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);

    DDX_Control (pDX, IDC_FORMAT_CMB_SELDRIVE,                    m_comboDriveList                            );
    DDX_Text    (pDX, IDC_FORMAT_STC_STORAGESTATE,                m_cstrStorageInfo                           );
    DDX_Control (pDX, IDC_FORMAT_PRG_FORMAT,                      m_progressFormat                            );
    DDX_Text    (pDX, IDC_FORMAT_STC_PROGRESS,                    m_cstrProgress                              );
}

BEGIN_MESSAGE_MAP(CNXFSFormatterDlg, CDialog)
	ON_WM_PAINT         ()
	ON_WM_QUERYDRAGICON ()
    ON_WM_LBUTTONDOWN   ()
    ON_WM_TIMER         ()
	//}}AFX_MSG_MAP

    ON_CBN_SELCHANGE(IDC_FORMAT_CMB_SELDRIVE,                   &CNXFSFormatterDlg::OnCmbSelchangeSeldrive  )
    ON_BN_CLICKED   ( IDC_FORMAT_BTN_FORMAT,                    &CNXFSFormatterDlg::OnClickFormat           )
    ON_BN_CLICKED	(IDC_FORMAT_BTN_OK, 						&CNXFSFormatterDlg::OnBnClickedCancel)
    ON_MESSAGE      ( UID_FORMAT_PROGRESS,                      &CNXFSFormatterDlg::OnFormatProgress        )
    ON_MESSAGE      ( WM_FORMAT_USER_DRIVECHANGED,                     &CNXFSFormatterDlg::OnDriveChanged          )
END_MESSAGE_MAP()


// CNXFSFormatterDlg message handlers
//================================================================================================= S. VIRTUAL

BOOL CNXFSFormatterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon( this->m_hIcon, TRUE );
	SetIcon( this->m_hIcon, FALSE );

    HWND hWnd = GetSafeHwnd();
    LPITEMIDLIST ppidl;
    if( SHGetSpecialFolderLocation(hWnd, CSIDL_DESKTOP, &ppidl) == NOERROR )
    {
        SHChangeNotifyEntry shCNE;
        shCNE.pidl = ppidl;
        shCNE.fRecursive = TRUE;

        ULONG uiSHChangeNotifyRegister = SHChangeNotifyRegister( hWnd, SHCNE_DISKEVENTS,
                                                                 SHCNE_MEDIAINSERTED|SHCNE_MEDIAREMOVED|SHCNE_DRIVEADD|SHCNE_DRIVEREMOVED, 
                                                                 WM_USER_DRIVECHANGED, 1, &shCNE);

        if( !uiSHChangeNotifyRegister )
        {
            printf( "Shell notification failed\n" );
            return FALSE;
        }
    }
    else
    {
        printf( "Failed to get desktop location\n" );
        return FALSE;
    }

	WIN32_FIND_DATA	findData;
	HANDLE			hDir = NULL;
	CString csDrive = this->m_cstrSelDrive.Left(3);
	
	TCHAR   pszLabel[MAX_PATH];

	::GetVolumeInformation(csDrive.GetBuffer(), pszLabel, MAX_PATH, NULL, NULL, NULL, NULL, NULL);
	
	csDrive.Append(BLACKBOX_CONFIG_FILE);

	CString	csVolume = pszLabel;

//	if(csVolume.Find(FORMAT_LABEL) >= 0)
	{
		hDir = FindFirstFileEx( csDrive, FindExInfoStandard, &findData, FindExSearchNameMatch, NULL, 0 );
		if( hDir != INVALID_HANDLE_VALUE )
		{
			m_bConfigOpen = TRUE;
			
			FindClose(hDir);
		}
	}

	if(!CConfigText::Load(csDrive, &m_stCfgDaview)){
		CString str("셋업을 확인할 수 없습니다");
		this->m_pCLanguage->GetLenguageText(&str , CLanguage::TEXT_SETUPNOTFIND);
		if (IDYES == ::AfxMessageBox(str, MB_ICONINFORMATION))
		{				
		}
		CDialog::OnCancel();
		return FALSE;
	}
	
    SetTimer( TID_INITIAL_START, TMDUR_INITIAL_START, NULL );

	return TRUE;
}

void CNXFSFormatterDlg::OnLanguageChange(void)
{
	CString strText;

	if(m_pCLanguage == NULL)
		return;

	SetItemText(IDC_FORMAT_STATIC_STORAGE_INFO, CLanguage::TEXT_STORAGE_INFO, L"");
	SetItemText(IDC_FORMAT_BTN_RELOAD, CLanguage::TEXT_RELOAD, L"");
	SetItemText(IDC_FORMAT_STATIC_CAP, CLanguage::TEXT_STORAGE_CAPACITY, L"");
//	SetItemText(IDC_FORMAT_STC_STORAGESTATE, CLanguage::TEXT_TIMESETTING, L"");
	
//	SetItemText(IDC_FORMAT_STC_PROGRESS, CLanguage::TEXT_TIMESETTING, L"");
	SetItemText(IDC_FORMAT_BTN_FORMAT, CLanguage::TEXT_SDFORMAT, L"");
	SetItemText(IDC_FORMAT_BTN_OK, CLanguage::TEXT_CLOSE, L"");


}

void  CNXFSFormatterDlg::SetItemText(int nIDDlgItem, UINT resource, CString index_str)
{
	CString strText;
	if(m_pCLanguage)
	{
		if(this->m_pCLanguage->GetLenguageText(&strText , resource))
			this->GetDlgItem(nIDDlgItem)->SetWindowText(index_str + strText);
	}
}

BOOL CNXFSFormatterDlg::PreTranslateMessage(MSG* pMsg)
{
    if( (pMsg->message == WM_SYSKEYDOWN) && (pMsg->wParam == VK_F4) )
        return TRUE;

    if( pMsg->message == WM_KEYDOWN )
    {
        if( (pMsg->wParam == VK_ESCAPE) || (pMsg->wParam == VK_RETURN)
            || (pMsg->wParam == VK_SPACE) || (pMsg->wParam == VK_CANCEL) )
        {
            return TRUE;        
        }
    }

    return CDialog::PreTranslateMessage( pMsg );
}

//================================================================================================= E. VIRTUAL
//================================================================================================= S. MESSAGE

void CNXFSFormatterDlg::OnPaint()
{
	OnLanguageChange();
	
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}


HCURSOR CNXFSFormatterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CNXFSFormatterDlg::OnTimer( UINT_PTR nIDEvent )
{
    if( nIDEvent == TID_INITIAL_START )
    {
        KillTimer( TID_INITIAL_START );

        this->_reloadDriveList();

        this->_selectDrive();

        this->m_progressFormat.SetRange( 0, 100 );

        UpdateData( FALSE );
    }

    return;
}


void CNXFSFormatterDlg::OnLButtonDown( UINT nFlags, CPoint point )
{
    UNREFERENCED_PARAMETER( nFlags );

    PostMessage( WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y) );

    CDialog::OnLButtonDown( nFlags, point );
}


void CNXFSFormatterDlg::OnCmbSelchangeSeldrive( void )
{
    this->m_nCurDriveIndex = this->m_comboDriveList.GetCurSel();
    this->_selectDrive();
    UpdateData( FALSE );

    return;
}

void CNXFSFormatterDlg::OnClickFormat( void )
{
	CString str(TEXT("All data will be erased. Do you want to continue?"));
	this->m_pCLanguage->GetLenguageText(&str , CLanguage::TEXT_FORMATCONFIRM);
    if( AfxMessageBox( str, MB_OKCANCEL | MB_ICONQUESTION ) == IDOK )
    {
        this->_showInterrControls( FALSE );

        HANDLE hThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)CNXFSFormatterDlg::_ThreadFormatNXFS, this, 0, NULL );
        if( hThread )
            CloseHandle( hThread );
    }

    return;
}

void CNXFSFormatterDlg::OnBnClickedCancel()
{
	CDialog::OnCancel();
}



LRESULT CNXFSFormatterDlg::OnFormatProgress( WPARAM wParam, LPARAM lParam )
{
    switch( wParam )
    {
    case PROGRESS_FORMAT_START:
        {
            this->m_cstrProgress = TEXT("Format Started.");
            this->m_progressFormat.SetPos( (wParam * 20) + 5 );

            this->m_bIsFormatting = TRUE;

        } break;

    case PROGRESS_DELETE_PARTITION:
        {
            this->m_cstrProgress = TEXT("Delete Partition OK.");
            this->m_progressFormat.SetPos( (wParam * 20) + 5 );

        } break;

    case PROGRESS_CREATE_FATFS:
        {
            this->m_cstrProgress = TEXT("Create FAT Partition OK.");
            this->m_progressFormat.SetPos( (wParam * 20) + 5 );

        } break;

    case PROGRESS_CREATE_NXFS:
        {
            this->m_cstrProgress = TEXT("Create NXFS Partition OK.");
            this->m_progressFormat.SetPos( (wParam * 20) + 5 );

        } break;

    case PROGRESS_FORMAT_NXFS:
        {
            this->m_cstrProgress = TEXT("Format NXFS Partition OK.");
            this->m_progressFormat.SetPos( (wParam * 20) + 5 );

        } break;

    case PROGRESS_FORMAT_FAT_READY:
        {
            if( lParam )
            {
                PSDRIVEINFO pDriveInfo = this->getCurDriveInfo();
                /*DWORD dwResult =*/
                    SHFormatDrive( this->GetSafeHwnd(), pDriveInfo->m_nDriveIndex, SHFMT_ID_DEFAULT, SHFMT_OPT_FULL );
            }

            this->m_cstrProgress = TEXT("Format Completed. Please reload drive list.");
            this->m_progressFormat.SetPos( 100 );
            this->_showInterrControls();

        } break;

    case PROGRESS_FAILED:
        {
            this->m_bIsFormatting = FALSE;

            switch( lParam )
            {
            case FAILED_CHECK_INFO:     this->m_cstrProgress = TEXT("[ERROR] Format canceled while finding disk information."); break;
            case FAILED_DELETE_PART:    this->m_cstrProgress = TEXT("[ERROR] Format canceled while deleting partition.");       break;
            case FAILED_CREATE_FAT:     this->m_cstrProgress = TEXT("[ERROR] Format canceled while creating FAT partition.");   break;
            case FAILED_CREATE_NXFS:    this->m_cstrProgress = TEXT("[ERROR] Format canceled while creating NXFS partition.");  break;
            case FAILED_FORMAT_NXFS:    this->m_cstrProgress = TEXT("[ERROR] Format canceled while formatting NXFS partition.");break;
            default:                    this->m_cstrProgress = TEXT("[ERROR] Unknown reason.");                                 break;
            }

            GetDlgItem( IDOK )->EnableWindow( TRUE );

        } break;

    default:;
    }

    UpdateData( FALSE );

    return 0;
}


LRESULT CNXFSFormatterDlg::OnDriveChanged( WPARAM wParam, LPARAM lParam )
{
    if( this->m_bIsFormatting )
        return 0;

    struct SHNOTIFYSTRUCT{ DWORD dwItem1; DWORD dwItem2; };
    SHNOTIFYSTRUCT *shns = (SHNOTIFYSTRUCT *)wParam;

    BOOL bValid = FALSE;
    BOOL bReload = FALSE;

    switch( lParam )
    {
    case SHCNE_DRIVEREMOVED:
    case SHCNE_MEDIAREMOVED:
        {
            if( !this->m_driveList.empty() && this->m_nCurDriveIndex >= 0 )
            {
                CString cstrDropDrive;
                CString cstrCurDrive;

                ITER_DRIVEINFO iDriveInfo = this->m_driveList.begin();
                std::advance( iDriveInfo, this->m_nCurDriveIndex );
                if( iDriveInfo != this->m_driveList.end() )
                {
                    cstrCurDrive.Format( TEXT("%c:\\"), 'A' + iDriveInfo->m_nDriveIndex );

                    WCHAR wcsPath[MAX_PATH];
                    if( SHGetPathFromIDList((struct _ITEMIDLIST *)shns->dwItem1, wcsPath) )
                    {
                        cstrDropDrive = wcsPath;

                        if( ! cstrCurDrive.Compare(cstrDropDrive) )
                        {
                            AfxMessageBox( TEXT("Selected drive has been removed. Reload drive list"), MB_ICONINFORMATION );
                            bReload = TRUE;
                        }
                    }
                }
            }

            bValid = TRUE;

        } break;

    case SHCNE_MEDIAINSERTED:
    case SHCNE_DRIVEADD:
        {
            if( ! this->m_driveList.empty() )
            {
                if( AfxMessageBox(TEXT("NEW MEDIA or DRIVE detected.\nReload ?"), MB_YESNO | MB_ICONQUESTION) == IDYES )
                    bReload = TRUE;
            }
            else
            {
                bReload = TRUE;
            }

            bValid = TRUE;

        } break;

    default:;
    }


    if( bValid && bReload )
    {
        this->_reloadDriveList();
        this->_selectDrive();
        UpdateData( FALSE );
    }

    return 0;
}

//================================================================================================= E. MESSAGE
//================================================================================================= S. OPERATION
//
//================================================================================================= E. OPERATION
//================================================================================================= S. PROTECTED_OPERATION

void CNXFSFormatterDlg::_selectDrive( void )
{
    ITER_DRIVEINFO iDriveInfo = this->m_driveList.begin();
    std::advance( iDriveInfo, this->m_nCurDriveIndex );

    if( iDriveInfo == this->m_driveList.end() )
        return;

    this->m_nTotalDiskSpaceMB = iDriveInfo->m_CapacityKB/1024;
    this->m_cstrStorageInfo.Format( TEXT("%C:\\ (%d MB, %.2f GB) storage selected"),
        'A' + iDriveInfo->m_nDriveIndex,
        this->m_nTotalDiskSpaceMB,
        (double)this->m_nTotalDiskSpaceMB / 1024 );
#if 0
    this->m_Partitions[PART_FAT].m_chkUseThis.SetCheck( BST_CHECKED );
    this->m_Partitions[PART_FAT].m_chkUseThis.EnableWindow( TRUE );
    this->m_Partitions[PART_FAT].m_spnCapacity.SetRange( 0, (short)this->m_nTotalDiskSpaceMB / 2 ); // 50%
    
    this->m_Partitions[PART_FAT].m_spnCapacity.SetPos( this->m_Partitions[PART_FAT].m_nCapMB );
    this->m_Partitions[PART_FAT].m_spnCapacity.EnableWindow( TRUE );
#endif
	this->m_Partitions[PART_FAT].m_nCapMB = CAP_FAT_DEFAULT_MB;
    this->m_Partitions[PART_FAT].m_fCapPercent = (float)this->m_Partitions[PART_FAT].m_nCapMB / this->m_nTotalDiskSpaceMB * 100;

#if 0
    this->m_Partitions[PART_NXFS].m_chkUseThis.SetCheck( BST_CHECKED );
    this->m_Partitions[PART_NXFS].m_chkUseThis.EnableWindow( FALSE );
    this->m_Partitions[PART_NXFS].m_spnCapacity.SetRange( (short)this->m_nTotalDiskSpaceMB / 2, (short)this->m_nTotalDiskSpaceMB );
    this->m_Partitions[PART_NXFS].m_spnCapacity.SetPos( this->m_Partitions[PART_NXFS].m_nCapMB );
    this->m_Partitions[PART_NXFS].m_spnCapacity.EnableWindow( TRUE );
#endif
	this->m_Partitions[PART_NXFS].m_nCapMB = this->m_nTotalDiskSpaceMB - this->m_Partitions[PART_FAT].m_nCapMB - 100;
    this->m_Partitions[PART_NXFS].m_fCapPercent = (float)this->m_Partitions[PART_NXFS].m_nCapMB / this->m_nTotalDiskSpaceMB * 100;

    this->m_NXFSDirInfo[0].m_cstrDirName = NXFS_DIR_NAME_0;
    this->m_NXFSDirInfo[1].m_cstrDirName = NXFS_DIR_NAME_1;
    this->m_NXFSDirInfo[2].m_cstrDirName = NXFS_DIR_NAME_2;
    this->m_NXFSDirInfo[3].m_cstrDirName = NXFS_DIR_NAME_3;
    this->m_NXFSDirInfo[4].m_cstrDirName = NXFS_DIR_NAME_4;
    this->m_NXFSDirInfo[5].m_cstrDirName = NXFS_DIR_NAME_5;

    this->m_NXFSDirInfo[0].m_nCapPercent = NXFS_DIR_CAP_PERCENT_0;
    this->m_NXFSDirInfo[1].m_nCapPercent = NXFS_DIR_CAP_PERCENT_1;
    this->m_NXFSDirInfo[2].m_nCapPercent = NXFS_DIR_CAP_PERCENT_2;
    this->m_NXFSDirInfo[3].m_nCapPercent = NXFS_DIR_CAP_PERCENT_3;
    this->m_NXFSDirInfo[4].m_nCapPercent = NXFS_DIR_CAP_PERCENT_4;
    this->m_NXFSDirInfo[5].m_nCapPercent = NXFS_DIR_CAP_PERCENT_5;

	if(m_bConfigOpen){
		switch(m_stCfgDaview.iEventMode)
		{
			case 0://A
			 this->m_NXFSDirInfo[0].m_nCapPercent = 100;
			 this->m_NXFSDirInfo[2].m_nCapPercent = 0;
			 break;
			case 1://B
			 this->m_NXFSDirInfo[0].m_nCapPercent = 90;
			 this->m_NXFSDirInfo[2].m_nCapPercent = 10;
			 break;
			case 2://C
			 this->m_NXFSDirInfo[0].m_nCapPercent = 80;
			 this->m_NXFSDirInfo[2].m_nCapPercent = 20;
			 break;
		}
	}
	
    int nIndex;
    for( nIndex = 0; nIndex < NXFS_MAX_DIRS; nIndex ++ )
    {
        this->m_NXFSDirInfo[nIndex].m_nFiles        = NXFS_DIR_DEF_FILES;
        this->m_NXFSDirInfo[nIndex].m_nDirAttribute = NXFSDIR_ARCHIVE;
        this->m_NXFSDirInfo[nIndex].m_nDirCapMB     = 0;
        this->m_NXFSDirInfo[nIndex].m_cstrDirCapMB  = TEXT("  0 MB");

//        this->m_NXFSDirInfo[nIndex].m_spnDirCap.SetRange( 0, 100 );
//        this->m_NXFSDirInfo[nIndex].m_spnDirCap.SetPos( this->m_NXFSDirInfo[nIndex].m_nCapPercent );

//        this->m_NXFSDirInfo[nIndex].m_cstrCapPercent.Format( TEXT("%d %%"), this->m_NXFSDirInfo[nIndex].m_nCapPercent );
    }

    return;
}


#define FROM_D_DRIVES   3
void CNXFSFormatterDlg::_reloadDriveList( void )
{
    DWORD dwDrives = ::GetLogicalDrives();
    CString cstrDrivePath;

    this->m_driveList.clear();

    dwDrives >>= FROM_D_DRIVES;

    int nDriveIndex;
    for( nDriveIndex = FROM_D_DRIVES; dwDrives; nDriveIndex++ ) // From D Drive
    {
        if(dwDrives & 1)
        {
            cstrDrivePath.Format( TEXT("%c:\\"),'A' + nDriveIndex );
            SDRIVEINFO driveInfo;

            switch( ::GetDriveType(cstrDrivePath) )
            {
            case DRIVE_REMOVABLE:
                {
                    driveInfo.m_nDriveIndex     = nDriveIndex;
					driveInfo.m_cstrDrivePath	= cstrDrivePath;
                    driveInfo.m_cstrComboFace   = cstrDrivePath;
                    driveInfo.m_eDriveMode      = DRIVE_MODE_REMOVAL;

                    if( this->__TestNXFSDrive(nDriveIndex, &driveInfo) )
                        this->m_driveList.push_back( driveInfo );

                } break;

            default:;
            }
        }

        dwDrives >>=1;
    }

    this->m_nCurDriveIndex = 0;
    this->m_comboDriveList.ResetContent();
    if( this->m_driveList.empty() )
    {
        this->m_comboDriveList.AddString( TEXT("Insert SD-CARD then reload...") );
        this->m_cstrStorageInfo = TEXT("No Selected storage");
    }
    else
    {
        ITER_DRIVEINFO iDriveInfo = this->m_driveList.begin();
        ITER_DRIVEINFO iEndDriveInfo = this->m_driveList.end();
        for( ; iDriveInfo != iEndDriveInfo; iDriveInfo ++ )
            this->m_comboDriveList.AddString( iDriveInfo->m_cstrComboFace );
    }

    this->m_comboDriveList.SetCurSel( this->m_nCurDriveIndex );

    return;
}


bool CNXFSFormatterDlg::__TestNXFSDrive( int nDiskIndex, PSDRIVEINFO pDriveInfo )
{
    HANDLE hDevice = NULL;

    nDiskIndex = this->__getPhysicalDriveIndex( char('A' + nDiskIndex) );

    if( nDiskIndex <= 0 )
        return false;

    sprintf_s( pDriveInfo->m_strPysical, "\\\\.\\PhysicalDrive%d", nDiskIndex );

    hDevice = CreateFileA( pDriveInfo->m_strPysical, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL );
    if( hDevice == INVALID_HANDLE_VALUE )
        return false;

	CloseHandle( hDevice );

    if( CL_NxFS_Check_NxFS_partition((sint8*)pDriveInfo->m_strPysical) )
    {
        pDriveInfo->m_eDriveMode = DRIVE_MODE_REMOVAL_NXFS;
        pDriveInfo->m_cstrComboFace += TEXT(" (NXFS)");
    }
    else
    {
        DWORD dwSectorsPerCluster;
        DWORD dwBytesPerSector;
        DWORD dwNumberOfFreeClusters;
        DWORD dwTotalNumberOfClusters;

        pDriveInfo->m_eDriveMode = DRIVE_MODE_REMOVAL;
        GetDiskFreeSpace( (LPCTSTR)pDriveInfo->m_cstrComboFace, &dwSectorsPerCluster, &dwBytesPerSector, &dwNumberOfFreeClusters, &dwTotalNumberOfClusters );

        if( !dwBytesPerSector )
            return false;
    }

    struct NxFS_DevInfo sDevInfo;
    CL_NxFS_Get_Master_Device_Info( (sint8*)pDriveInfo->m_strPysical, &sDevInfo );
    uint64_t ret = ((uint64_t)sDevInfo.total_sectors  * BYTES_PER_SEC) >> 10;
    if( ret > 0 )   pDriveInfo->m_CapacityKB = (int)ret;
    else            pDriveInfo->m_CapacityKB = 0;

    return true;
}


#define VOLUMEDISKSIZE (sizeof(VOLUME_DISK_EXTENTS))
int CNXFSFormatterDlg::__getPhysicalDriveIndex( char cDriveName )
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


void CNXFSFormatterDlg::_showInterrControls( BOOL bShow )
{
    this->m_comboDriveList.EnableWindow( bShow );
    GetDlgItem(IDC_FORMAT_BTN_RELOAD)->EnableWindow( bShow );


    if( this->m_bIsFormatting )
        GetDlgItem(IDC_FORMAT_BTN_FORMAT)->EnableWindow( FALSE );
    else
        GetDlgItem(IDC_FORMAT_BTN_FORMAT)->EnableWindow( bShow );

    GetDlgItem(IDC_FORMAT_BTN_OK)->EnableWindow( bShow );

    return;
}

//================================================================================================= E. PROTECTED_OPERATION
//================================================================================================= S. GET/SET

CNXFSFormatterDlg::PSDRIVEINFO CNXFSFormatterDlg::getCurDriveInfo( void )
{
    if( this->m_nCurDriveIndex < 0 )
        return NULL;

    ITER_DRIVEINFO iDriveInfo = this->m_driveList.begin();
    std::advance( iDriveInfo, this->m_nCurDriveIndex );

    if( iDriveInfo == this->m_driveList.end() )
        return NULL;

    return &(*iDriveInfo);
}


CNXFSFormatterDlg::PSPARTION_INFO CNXFSFormatterDlg::getPartitionInfo( EPARTION_INFO ePartition )
{
    if( ePartition == PART_INVALID )
        return NULL;

    return &this->m_Partitions[ePartition];
}


CNXFSFormatterDlg::PSNXFS_DIR CNXFSFormatterDlg::getNXFSDirInfo( int nIndex )
{
    if( (nIndex < 0) || (nIndex > 5) )
        return NULL;

    return &this->m_NXFSDirInfo[nIndex];
}

//================================================================================================= E. GET/SET
//================================================================================================= S. STATIC_THREAD

DWORD CNXFSFormatterDlg::_ThreadFormatNXFS( LPVOID lpArg )
{
    CNXFSFormatterDlg* pDlg = (CNXFSFormatterDlg*)lpArg;

    PSDRIVEINFO pDriveInfo = pDlg->getCurDriveInfo();
    CNXFSFormatterDlg::PSPARTION_INFO pPartInfo = pDlg->getPartitionInfo( CNXFSFormatterDlg::PART_FAT );
    if( (pDriveInfo != NULL) && (pPartInfo != NULL) )
    {
        pDlg->PostMessage( UID_FORMAT_PROGRESS, PROGRESS_FORMAT_START, 0 );

        uint64_t szFAT = pPartInfo->m_nCapMB;
        if( ! CNXFSFormatterDlg::__createPartition( pDlg->GetSafeHwnd(), (sint8*)pDriveInfo->m_strPysical, szFAT) )
            return 0;

        struct NxFS_DirTbl dir_list;
        int nDirIndex = 0;
        int nIndex;
        for( nIndex = 0; nIndex < NXFS_MAX_DIRS; nIndex ++ )
        {
            CNXFSFormatterDlg::PSNXFS_DIR pDir = pDlg->getNXFSDirInfo( nIndex );
            if( (pDir == NULL) || (! pDir->m_cstrDirName.GetLength()) || (! pDir->m_nCapPercent) )
                continue;

            USES_CONVERSION;
            strcpy_s( (char *)dir_list.dir_entry[nDirIndex].dir_name, MAX_DIR_NAME_STR_SIZE, T2A(pDir->m_cstrDirName) );
            dir_list.dir_entry[nDirIndex].size          = pDir->m_nCapPercent;
            dir_list.dir_entry[nDirIndex].max_num_files = pDir->m_nFiles;
            dir_list.dir_entry[nDirIndex].dir_type      = (uint8)pDir->m_nDirAttribute;

            nDirIndex ++;
        }

        dir_list.num_dir = (uint8)nDirIndex;

        if( ! CNXFSFormatterDlg::__formatPartition(pDlg->GetSafeHwnd(), (sint8*)pDriveInfo->m_strPysical, &dir_list) )
        {
            pDlg->PostMessage( UID_FORMAT_PROGRESS, PROGRESS_FORMAT_FAT_READY, szFAT ? 1 : 0 );
        }
    }

    return 0;
}


bool CNXFSFormatterDlg::__createPartition( HWND hWnd, sint8* pPhsicalDevice, uint64_t u64szFATCapMB )
{
    uint64_t ret;
    uint32 alloc_sectors, partition_no;
    uint64_t nTotalDiskSize = 0;
    uint64_t size;
    struct NxFS_DevInfo sDevInfo;

    CL_NxFS_Get_Master_Device_Info( pPhsicalDevice, &sDevInfo );
    ret = ((uint64_t)sDevInfo.total_sectors * BYTES_PER_SEC) >> 10;
    if( ret <= 0 )
    {
        ::PostMessage( hWnd, UID_FORMAT_PROGRESS, PROGRESS_FAILED, FAILED_CHECK_INFO );
        return false;
    }

    nTotalDiskSize = ret;

    ret = CL_NxFS_Delete_Partition( pPhsicalDevice, FILE_SYSTEM_ALL );
    if( ret != ERRNO_SUCCESS )
    {
        ::PostMessage( hWnd, UID_FORMAT_PROGRESS, PROGRESS_FAILED, FAILED_DELETE_PART );
        return false;
    }

    ::PostMessage( hWnd, UID_FORMAT_PROGRESS, PROGRESS_DELETE_PARTITION, 0 );

    partition_no = DEFAULT_FAT_PARTITION_NUM;

    if( u64szFATCapMB )
    {
        ret = CL_NxFS_Create_Partition( pPhsicalDevice, FILE_SYSTEM_FAT32, u64szFATCapMB, &partition_no, &alloc_sectors );
        if( ret != ERRNO_SUCCESS )
        {
            ::PostMessage( hWnd, UID_FORMAT_PROGRESS, PROGRESS_FAILED, FAILED_CREATE_FAT );
            return false;
        }

        ::PostMessage( hWnd, UID_FORMAT_PROGRESS, PROGRESS_CREATE_FATFS, 0 );
    }

    size = nTotalDiskSize - (u64szFATCapMB * 1024);
    size = size >> 10;

    ret = CL_NxFS_Create_Partition( pPhsicalDevice, FILE_SYSTEM_NXFS, size, &partition_no, &alloc_sectors );
    if( ret != ERRNO_SUCCESS )
    {
        ::PostMessage( hWnd, UID_FORMAT_PROGRESS, PROGRESS_FAILED, FAILED_CREATE_NXFS );
        return false;
    }

    ::PostMessage( hWnd, UID_FORMAT_PROGRESS, PROGRESS_CREATE_NXFS, 0 );

    return true;
}


int CNXFSFormatterDlg::__formatPartition( HWND hWnd, sint8* pPhsicalDevice, NxFS_DirTbl* pDirList )
{
    uint64_t ret;

    ret = CL_NxFS_Format( pPhsicalDevice, pPhsicalDevice, FILE_SYSTEM_NXFS, PARTITION_FORMAT, pDirList, NULL );
    if( ret != ERRNO_SUCCESS )
    {
        ::PostMessage( hWnd, UID_FORMAT_PROGRESS, PROGRESS_FAILED, FAILED_FORMAT_NXFS );
        return 1;
    }

    ::PostMessage( hWnd, UID_FORMAT_PROGRESS, PROGRESS_FORMAT_NXFS, 0 );

    return 0;
}

//================================================================================================= E. STATIC_THREAD
//================================================================================================= EOF
