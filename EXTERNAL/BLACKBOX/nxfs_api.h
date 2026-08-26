/*====================================================================
**
** FILE: nxfs_api.h
** 
** Copyright (c) 2014 by ETXWORKS.	All Rights Reserved.		
**
=====================================================================*/
/*====================================================================

					  EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


mm/dd/yy	Auth.		Description
---------	---------	------------------------------------------------------------------
05/13/14	Kyoobom 	Created
=====================================================================*/

#ifndef __NXFS_API_H__
#define __NXFS_API_H__ 


/*=============================================================================
Header Files
=============================================================================*/

/*=============================================================================
Type Definition
=============================================================================*/

typedef int           		smallint;
typedef unsigned      		smalluint;
typedef unsigned int   		sector_t;
typedef unsigned long 		uoff_t;
typedef int           		errot_t;

typedef unsigned char		uint8;
typedef signed char 		sint8;
typedef unsigned short		uint16;
typedef signed short		sint16;
typedef unsigned int		uint32;
typedef signed int			sint32;
typedef unsigned long long  uint64;
typedef signed long long    sint64;
typedef unsigned long long	uint64_t;
typedef signed long long	sint64_t;



#if defined(__LINUX_PLATFORM__)

#define PACKED_STRUCT_HEADER
#define PACKED_STRUCT_TAIL __attribute__((packed))
//typedef unsigned char	BYTE;
typedef void*			LPVOID;
typedef uint32*			LPDWORD;	

#ifndef	HANDLE
#define	HANDLE			sint32 // for linux
#endif

#elif defined(__WIN32_PLATFORM__) // In WIN32, use project configuration > C/C++ > Code generation > struct member alignment > 1 byte setting
#include <winsock.h>
#include <Windows.h>	
#define PACKED_STRUCT_HEADER
#define PACKED_STRUCT_TAIL


#else // arm platform

#define PACKED_STRUCT_HEADER	__packed
#define PACKED_STRUCT_TAIL

#endif

#ifndef	null
#define	null	0
#endif

#ifndef	NULL
#define	NULL	0
#endif

#ifndef	bool
#define	bool	BOOL
#endif

#ifndef	BOOL
#define	BOOL	bool
#endif

#ifndef	true
#define	true	1
#endif

#ifndef	TRUE
#define	TRUE	1
#endif

#ifndef	false
#define	false	0
#endif

#ifndef	FALSE
#define	FALSE	0
#endif

//#ifndef	HANDLE
//#define	HANDLE	sint32 // for linux
//#endif

#ifndef	DWORD
#define	DWORD	unsigned long
#endif


#ifndef	u_long
#define	u_long	unsigned long
#endif

#if defined(__WIN32_PLATFORM__)
typedef long off_t;
#endif

//#if defined(__LINUX_PLATFORM__)
#define MUTEX			pthread_mutex_t
#define MUTEX_ATTRIB	pthread_mutexattr_t
//#elif defined(__WIN32_PLATFORM__)
//#define MUTEX HANDLE
//#endif 

/*=============================================================================
Constants Definition
=============================================================================*/

/*-----------------------------------------------------------------------------
max number of SD used
-----------------------------------------------------------------------------*/
#define  NXFS_DEVICE_MAX_NUM			2

/*-----------------------------------------------------------------------------
backup number for each table data
-----------------------------------------------------------------------------*/
#define  NXFS_TABLE_BACKUP_NUM			2

/*-----------------------------------------------------------------------------
max size of SD for secc
-----------------------------------------------------------------------------*/
#define  NXFS_DEVICE_MAX_SIZE			128		// unit : Gbytes

/*-----------------------------------------------------------------------------
definitions for SECTOR & CLUSTER SIZE
-----------------------------------------------------------------------------*/
#define SECTOR_PER_CLUSTER				0x80	// 128
#define SECTOR_PER_CLUSTER_SHIFT_BIT	7	// a<<7 OR a>>7
//#define SECTOR_PER_CLUSTER			0x100	 // 128K 
//#define SECTOR_PER_CLUSTER_SHIFT_BIT	8 
//#define SECTOR_PER_CLUSTER			0x200	 // 256K 
//#define SECTOR_PER_CLUSTER_SHIFT_BIT	9 
//#define SECTOR_PER_CLUSTER			0x400	 // 512K 
//#define SECTOR_PER_CLUSTER_SHIFT_BIT  10 
//#define SECTOR_PER_CLUSTER			0x800	 
//#define SECTOR_PER_CLUSTER_SHIFT_BIT  11 // 1M 
#define DEFAULT_SECTOR_SIZE 			512 	/* default sector size is 512 bytes 	*/
#define DEFAULT_SECTOR_SIZE_SHFIT_BIT	9	// 1>>9 or 1<<9

#define NXFS_CLUSTER_SIZE_SHIFT_BIT		(DEFAULT_SECTOR_SIZE_SHFIT_BIT+SECTOR_PER_CLUSTER_SHIFT_BIT)
#define NXFS_CLUSTER_SIZE				(1<<(NXFS_CLUSTER_SIZE_SHIFT_BIT))

/*-----------------------------------------------------------------------------
definitions for partitions
-----------------------------------------------------------------------------*/
#define MAX_NUM_PARTITION				4		/* NxFS supports up to 4 partitions 	*/
#define INVALID_PARTITION				0xFF
#define INVALID_DIR_DESC				0xFF
#define INVALID_FILE_DESC				0xFFFFFF


//#define MAX_LEN_DIR_NAME_STR			64

#define MAX_FILE_NAME_SIZE				64 // 36	
#define MAX_FILE_META_DATA_SIZE 		32
#define MAX_DIR_NAME_STR_SIZE			64

/*----------------------------------------------------------------------------------------------------
Partition Types

	- This is a list of known Master Boot Record partition types on IBM PC compatible computers
----------------------------------------------------------------------------------------------------*/
#define PARTITION_TYPE_EMPTY			0x00		
#define PARTITION_TYPE_FAT12			0x01
#define PARTITION_TYPE_NTFS_EXFAT		0x07
//#define PARTITION_TYPE_EXFAT			0x07
#define PARTITION_TYPE_FAT32_CHS		0x08		
#define PARTITION_TYPE_FAT32_WIN95		0x0B
#define PARTITION_TYPE_FAT32X_LBA		0x0C
#define PARTITION_TYPE_FAT16X_LBA		0x0E
//#define PARTITION_TYPE_NxFS 			0x00 /* this will be changed later */	
#define PARTITION_TYPE_NxFS 			0x52 /* this will be changed later */	

/*----------------------------------------------------------------------------------------------------
Constants Definition for File Seek seek_origin in READ MODE
----------------------------------------------------------------------------------------------------*/

#define 	NXFS_SEEK_ORIGIN_START		0
#define 	NXFS_SEEK_ORIGIN_CUR		1	
#define 	NXFS_SEEK_ORIGIN_END		2

/*----------------------------------------------------------------------------------------------------
Constants Definition for maximum number of directories 
----------------------------------------------------------------------------------------------------*/
#define MAX_NUM_DIR 					15 // this value must be larger than 13

/*----------------------------------------------------------------------------------------------------
Constants Definition for maximum number of files per directory
----------------------------------------------------------------------------------------------------*/
#define MAX_NUM_FILES_PER_DIR			0xFFFF //12 is for test <-- 65535
#define MAX_NUM_FILE_LIST_GET			30



#define __NXFS_SS_DEV_OC_SCHEME__



/*====================================================================================================
Enumeration Constants Definition
====================================================================================================*/

/*----------------------------------------------------------------------------------------------------
E_NxFS_FILESYSTEM_TYPE

	- The followings are file system type enumeration constants
----------------------------------------------------------------------------------------------------*/
typedef enum
{
	FILE_SYSTEM_UNASSIGNED =	0x00,
	FILE_SYSTEM_FAT32 =			0x01,
	FILE_SYSTEM_NXFS  =			0x02,
	FILE_SYSTEM_FAT32_WIN95 =	0x03,
	FILE_SYSTEM_ALL   =			0x07

}E_NxFS_FILESYSTEM_TYPE;

/*----------------------------------------------------------------------------------------------------
E_NxFS_FORMAT_TYPE


	- The following enumeration constants are the format types to proceed
----------------------------------------------------------------------------------------------------*/
typedef enum
{
	LOW_FORMAT =				0x01,			/* remove all partitions */
	PARTITION_FORMAT =			0x02,		/* remove a selected partition */
	
	FORMAT_TYPE_MAX = PARTITION_FORMAT + 1
	
}E_NxFS_FORMAT_TYPE;


/*----------------------------------------------------------------------------------------------------
E_NxFS_RESULT_TYPE

	- The following enumeration constants are error number returned from NxFS APIs.
----------------------------------------------------------------------------------------------------*/
typedef enum
{
	ERRNO_SUCCESS = 0x00,

	ERRNO_DEV_FAIL,

	ERRNO_NXFS_START_FAIL,
	ERRNO_NXFS_STOP_FAIL,

	ERRNO_MEM_ALLOC_FAIL,
	ERRNO_INVALID_PARAMETER, 
	ERRNO_FORMAT_FAIL,
	
	ERRNO_NUM_PARTITION_EXCEEDED,
	ERRNO_PARTITION_CREATION_FAIL,
	ERRNO_INVALID_PARTION_TYPE,
	ERRNO_INVALID_PARTION_SIZE,
	ERRNO_PARTITION_REMOVE,
	
	ERRNO_NUM_DIR_EXCEEDED,
	ERRNO_INVALID_DIR_NAME,
	ERRNO_INVALID_DIR_SIZE,
	ERRNO_INVALID_PARTITION,
	ERRNO_ASSIGN_DIR_DESC,
	ERRNO_FAILED_DIR_TABLE,
	
	ERRNO_NUM_FILE_EXCEEDED,
	ERRNO_INVALID_FILE_NAME,
	ERRNO_INVALID_FILE_SIZE,
	ERRNO_INVALID_FILE_TYPE,
	ERRNO_INVALID_FILE_DESC,
	ERRNO_GET_FILE_LIST,
	ERRNO_GET_FILE_INFO,
	ERRNO_INSUFFICIENT_STORAGE_SPACE,
	
	ERRNO_FILE_OPEN_FAIL,
	ERRNO_FILE_ALREADY_OPENED,
	ERRNO_FILE_READ_FAIL,
	ERRNO_FILE_WRITE_FAIL,
	ERRNO_FILE_HEADER_WRITE_FAIL,

	ERRNO_BOOT_RECORD_READ_FAIL,
	ERRNO_LOAD_BOOT_RECORD_FAIL,
	ERRNO_NOT_ENOUGH_SPACE,
	ERRNO_NOT_SUB_DEVICE
	
	/* ... more error numbers shall be added ... */
	
}E_NxFS_RESULT_TYPE;

/*----------------------------------------------------------------------------------------------------
E_DEV_ATTRIBUTE_TYPE

	- The following enumeration constants are attributes of a device
----------------------------------------------------------------------------------------------------*/
typedef enum
{
#if defined(__LINUX_PLATFORM__)
	ATTR_DEV_RDONLY    = O_RDONLY,
	ATTR_DEV_WRONLY    = O_WRONLY,
	ATTR_DEV_RDWR      = O_RDWR,
	ATTR_DEV_ACCMODE   = O_ACCMODE,
	ATTR_DEV_CREATE    = O_CREAT,
	ATTR_DEV_EXCL      = O_EXCL,
	ATTR_DEV_NOCTTY    = O_NOCTTY,
	ATTR_DEV_TRUNC     = O_TRUNC,
	ATTR_DEV_APPEND    = O_APPEND,
	ATTR_DEV_NONBLOCK  = O_NONBLOCK,
	//ATTR_DEV_D_SYNC    = O_DSYNC,
	ATTR_DEV_O_SYNC    = O_SYNC,
	//ATTR_DEV_O_DIRECT  = O_DIRECT,
	//ATTR_DEV_O_LARGEFILE = O_LARGEFILE
	
#elif defined(__WIN32_PLATFORM__)
#ifndef O_SYNC
#define O_SYNC 0x0080
#endif

	ATTR_DEV_RDONLY    = O_RDONLY,
	ATTR_DEV_WRONLY    = O_WRONLY,
	ATTR_DEV_RDWR      = O_RDWR,
	ATTR_DEV_ACCMODE   = (O_RDONLY|O_WRONLY|O_RDWR),
	ATTR_DEV_CREATE    = O_CREAT,
	ATTR_DEV_EXCL      = O_EXCL,
//	ATTR_DEV_NOCTTY    = O_NOCTTY,
	ATTR_DEV_TRUNC     = O_TRUNC,
	ATTR_DEV_APPEND    = O_APPEND,
//	ATTR_DEV_NONBLOCK  = O_NONBLOCK,
//	ATTR_DEV_D_SYNC    = O_DSYNC,
	ATTR_DEV_O_SYNC    = O_SYNC,
//	ATTR_DEV_O_DIRECT  = O_DIRECT,
//	ATTR_DEV_O_LARGEFILE = O_LARGEFILE
#else

#endif
}E_NxFS_DEV_ATTRIBUTE_TYPE;


/*----------------------------------------------------------------------------------------------------
E_DIR_CONTENT_TYPE

	- The following enumeration constants are attributes of a directory
----------------------------------------------------------------------------------------------------*/
typedef enum
{
	DIR_TYPE_UNSPECIFIED =		0x00,
	DIR_TYPE_AUDIO  =			0x01,
	DIR_TYPE_VIDEO  =			0x02,
	DIR_TYPE_AUDIO_VIDEO  =		0x03,
	DIR_TYPE_IMAGE  =			0x04,
	DIR_TYPE_ARCHIVE =			0x05,
	DIR_TYPE_TXT =				0x06,
	DIR_TYPE_RSVD1 =			0x07,
	DIR_TYPE_RSVD2 =			0x08,

	DIR_TYPE_MAX = DIR_TYPE_RSVD2
	
}E_NxFS_DIR_CONTENT_TYPE;


/*----------------------------------------------------------------------------------------------------
E_FILE_ATTRIBUTE_TYPE

	-	The following enumeration constants are options to handle file.
		To be determined
----------------------------------------------------------------------------------------------------*/
typedef enum
{
	ATTR_FILE_NOT_DEFINED = 0x00,
	ATTR_FILE_READ  =		0x01,	/* r  */
	ATTR_FILE_WRITE =		0x02,	/* w  */

	ATTR_FILE_MAX = ATTR_FILE_WRITE
	
}E_FILE_ATTRIBUTE_TYPE;



/*----------------------------------------------------------------------------------------------------
E_DEINIT_MODE_TYPE

	-	The following enumeration constants are options to handle deinit mode.
		To be determined
----------------------------------------------------------------------------------------------------*/
typedef enum
{
	DEINIT_MODE_NON_BLOCKING =	0x00,
	DEINIT_MODE_BLOCKING =		0x01
	
}E_DEINIT_MODE_TYPE;


/*----------------------------------------------------------------------------------------------------
E_SORTING_TYPE

	-	The following enumeration constants are options to handle sorting type.
----------------------------------------------------------------------------------------------------*/
typedef enum
{
	SORTING_TYPE_ASC =	0x00,
	SORTING_TYPE_DESC = 0x01
	
}E_SORTING_TYPE;

/*----------------------------------------------------------------------------------------------------
E_FSEEK_TYPE

	-	The following enumeration constants are options to handle deinit mode.
		To be determined
----------------------------------------------------------------------------------------------------*/
typedef enum
{
	FSEEK_SET = 0x00,
	FSEEK_CUR = 0x01,
	FSEEK_END = 0x02
	
}E_FSEEK_TYPE;

/*=============================================================================
Structure Definition
=============================================================================*/

/*----------------------------------------------------------------------------------------------------
structNxFS_BootSector;

	- The following structure is used to read MBR information of storage device
----------------------------------------------------------------------------------------------------*/
PACKED_STRUCT_HEADER struct NxFS_BootSector
{										
	uint8		BS_jmpBoot[3];		/* Jump Boot code */
	sint8		BS_OEMName[8];		/* OEM Name */
	uint16		BPB_BytesPerSec;	/* Bytes per Sector : always 512 */
	uint8		BPB_SecPerClus; 	/* sector per cluster : 1, 2, 4, 8, 16, 32, 64, 128 */
	uint16		BPB_RsvdSecCnt; 	/* reserved sector count */
	uint8		BPB_NumFATs;		/* number of FATs */
	uint16		BPB_RootEntCnt; 	/* Root entry count */
	uint16		BPB_TotSec16;		/* Total Sector FAT16 */
	uint8		BPB_Media;			/* media type */
	uint16		BPB_FATSz16;		/* FAT16 size */
	uint16		BPB_SecPerTrk;		/* Sector per track */
	uint16		BPB_NumHeads;		/* number of heads */
	uint32		BPB_HiddSec;		/* hidden sectors */
	uint32		BPB_TotSec32;		/* total sectors FAT32 */
	
} PACKED_STRUCT_TAIL;

/*----------------------------------------------------------------------------------------------------
struct NxFS_PartitionEntry

	- The following structure is used to read Partition information of storage device
----------------------------------------------------------------------------------------------------*/
PACKED_STRUCT_HEADER struct NxFS_PartitionEntry
{									
	uint8		Bootable;		/* bootable or not */
	uint8		StartHead;		/* start number of head */
	uint16		StartCylSec;	/* start number of cylinder & sector */
	uint8		Type;			/* FAT type */
	uint8		EndHead;		/* start number of head */
	uint16		EndCylSec;		/* end number of cylinder & sector */
	sint32		StartLBA;		/* start sector of partition */
	sint32		Size;			/* total sectors of partition */
	
}PACKED_STRUCT_TAIL;
/*----------------------------------------------------------------------------------------------------
struct NxFS_BPB

	- The following structure is used to read BIOS Parameter Block of storage device
----------------------------------------------------------------------------------------------------*/
PACKED_STRUCT_HEADER struct NxFS_BPB
{									
	uint32		BPB_FATSz32;			/* FAT32 size */
	uint16		BPB_ExtFlags;			/* FAT32 extension Flags */
	uint16		BPB_FSVer;				/* file system version */
	uint32		BPB_RootClus;			/* root cluster number */
	uint16		BPB_FSInfo; 			/* file system information */
	uint16		BPB_BkBootSec;			/* backup boot sector */
	uint8		BPB_Reserved[12];		/* reserved 12 bytes */
	uint8		BS_DrvNum;				/* drive number */
	uint8		BS_Reserved1;			/* reserved */
	uint8		BS_BootSig; 			/* boot signal */
	uint32		BS_VolID;				/* volume Identifier */
	sint8		BS_VolLabel[11];		/* volume label string */
	sint8		BS_FilSysType[8];		/* file system type string */
	
}PACKED_STRUCT_TAIL;


/*----------------------------------------------------------------------------------------------------
struct NxFS_DirEntryBuffer
	- get directory entry position with cluster & entry 
----------------------------------------------------------------------------------------------------*/
/* number of directory entry 
#define ENTRY_BUF_SIZE			15

PACKED_STRUCT_HEADER struct NxFS_DirEntryBuffer	
{
	uint32 cluster[ENTRY_BUF_SIZE];
	uint8 entry[ENTRY_BUF_SIZE];
	uint8 index;
	
}PACKED_STRUCT_TAIL;
*/

PACKED_STRUCT_HEADER struct NxFs_BootRecordInfo
{
	uint8    SecPerClus;		// Sectors per Cluster
	uint32   FirstDataSector;	// first data sector
	uint32   StartFATSector;	// FAT Start sector
	uint32   RootCluster;		// Root Directory cluster number : generally 2
	uint32   BPB_TotSec32;
	uint32   BkupPbrSector;
	uint16   RsvdSectorCnt;
	
}PACKED_STRUCT_TAIL;

/*----------------------------------------------------------------------------------------------------
struct NxFS_PartitionTable
	- get partition table entry 
----------------------------------------------------------------------------------------------------*/

PACKED_STRUCT_HEADER struct NxFS_PartitionTable
{
	uint8 num_partition;
	
	struct NxFS_PartitionEntry partition_entry[MAX_NUM_PARTITION];
	struct NxFs_BootRecordInfo boot_record_info[MAX_NUM_PARTITION];
	
}PACKED_STRUCT_TAIL;

/*----------------------------------------------------------------------------------------------------
struct NxFS_DevInfo
	- get device geometry
----------------------------------------------------------------------------------------------------*/

PACKED_STRUCT_HEADER struct NxFS_DevInfo
{
	uint32 heads;
	uint32 sectors;
	uint32 cylinders;
	uint32 total_sectors;
	
}PACKED_STRUCT_TAIL;

/*----------------------------------------------------------------------------------------------------
struct NxFS_DirTblEntry
	- The following structure holds directory entry info

struct NxFS_DirTbl
	- The following structure holds directory entries info
----------------------------------------------------------------------------------------------------*/
PACKED_STRUCT_HEADER struct NxFS_DirEntry
{
	sint8   dir_name[MAX_DIR_NAME_STR_SIZE];

	uint64  size; 
	sint32	max_num_files;
	uint8   dir_type;
	
}PACKED_STRUCT_TAIL;


PACKED_STRUCT_HEADER struct NxFS_DirTbl
{
	uint8        num_dir;
	struct NxFS_DirEntry dir_entry[MAX_NUM_DIR];
}PACKED_STRUCT_TAIL;

/*----------------------------------------------------------------------------------------------------
struct NxFS_DEV_HANDLE

	- The following structure holds FAT information of storage device 
----------------------------------------------------------------------------------------------------*/
PACKED_STRUCT_HEADER struct NxFS_DEV_HANDLE
{
	HANDLE dev_desc;	
	struct NxFS_PartitionTable sPartitionTbl;
}PACKED_STRUCT_TAIL;

typedef struct NxFS_DEV_HANDLE DEV_HANDLE;
/*----------------------------------------------------------------------------------------------------
struct NxFS_FileInfo

	- The following structure is used to read file information
----------------------------------------------------------------------------------------------------*/
PACKED_STRUCT_HEADER struct NxFS_FileInfo
{
	uint8	file_name[MAX_FILE_NAME_SIZE];
	uint8   file_meta_data_size;
	uint8	file_meta_data[MAX_FILE_META_DATA_SIZE];
	uint8	file_attr;

	sint32  fd;
	uint32  file_size;
	uint32  file_created;

	uint8	reserved[18];

}PACKED_STRUCT_TAIL;

/*----------------------------------------------------------------------------------------------------
struct NxFS_FileList

	- The following structure is used to read file information
----------------------------------------------------------------------------------------------------*/
PACKED_STRUCT_HEADER struct NxFS_FileList
{
	/* The number of files to get. Maximum number of files is MAX_NUM_FILES_PER_DIR */
	uint32 num_files;	
	uint8 file_info[1];
		
}PACKED_STRUCT_TAIL;

#if 0
/*----------------------------------------------------------------------------------------------------
struct NxFS_DirTblEntryFat32

	- Directory Information of FAT32
----------------------------------------------------------------------------------------------------*/

PACKED_STRUCT_HEADER struct NxFS_DirTblEntryFat32
{									
	uint8	DIR_Name[8];			/* directory name string */
	uint8	DIR_Ext[3]; 			/* directory extension string */
	uint8	DIR_Attr;				/* directory attribute */
	uint8	DIR_NTRes;				/* reserved */
	uint8	DIR_CreationTimeTenth;	/* creation time (x10) */
	uint16	DIR_CreationTime;		/* creation time */
	uint16	DIR_CreationDate;		/* create date */
	uint16	DIR_LastAccessDate; 	/* last access date */
	uint16	DIR_FirstClusterHigh;	/* first cluster number higher 2 bytes */
	uint16	DIR_UpdateTime; 		/* update time */
	uint16	DIR_UpdateDate; 		/* update date */
	uint16	DIR_FirstClusterLow;	/* first cluster number lower 2 bytes */
	uint32	DIR_Size;				/* directory size */
	
}PACKED_STRUCT_TAIL;

/*----------------------------------------------------------------------------------------------------
struct NxFS_FileInfoFat32

	- The following structure is used to read file information
----------------------------------------------------------------------------------------------------*/
PACKED_STRUCT_HEADER struct NxFS_FileInfoFat32
{
	uint8	File_Name[8];				/* directory name string */
	uint8	File_Ext[3];				/* directory extension string */
	uint8	File_Attr;					/* file attribute */
	uint8	File_Rsvd;					/* reserved */
	uint8	File_CreationTimeTenth; 	/* creation time (x10) */
	uint16	File_CreationTime;			/* creation time */
	uint16	File_CreationDate;			/* create date */
	uint16	File_LastAccessTime;		/* last access date */
	uint16	File_FirstClusterHigh;		/* first cluster number higher 2 bytes */
	uint16	File_UpdateTime;			/* update time */
	uint16	File_UpdateDate;			/* update date */
	uint16	File_FirstClusterLow;		/* first cluster number lower 2 bytes */
	uint32	File_Size;					/* file size */
	
	//uint8	File_Type;					/* file type */
	
	/* more members will be added */
	
}PACKED_STRUCT_TAIL;

/*----------------------------------------------------------------------------------------------------
struct NxFS_LongFileInfoFat32
	
	- The following structure is used to read file information with long file name.
----------------------------------------------------------------------------------------------------*/
PACKED_STRUCT_HEADER struct NxFS_LongFileInfoFat32
{
	uint8		File_Sequence;		/* Sequence Number and allocation Status */
	uint8		File_Name1[10]; 	/* File Name Characters (Unicode) */
	uint8		File_Attr;				/* File Attrubutes */
	uint8		File_Rsvd;			/* reserved */
	uint8		File_Checksum;		/* checksum */
	uint8		File_Name2[12]; 	/* File Name Characters (Unicode) */
	uint16		File_ClusterNumber; /* File cluster number */
	uint8		File_Name3[4];		/* File Name Characters (Unicode) */
	
}PACKED_STRUCT_TAIL;
#endif

/*=============================================================================
Function Prototype
=============================================================================*/


#ifdef __WIN32_PLATFORM__ // Visual Studio specific macro

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NXFS_DLL_EXPORTS
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __declspec(dllimport)
#endif

	//	#define DLLLOCAL 
#else 
//	#define DLLLOCAL 
#define DLLEXPORT //__attribute__ ((visibility("default")))
#define DLLLOCAL  // __attribute__ ((visibility("hidden")))
#endif 

#if defined(__NXFS_SS_DEV_OC_SCHEME__)

/*----------------------------------------------------------------------------------------------
Function Name	

	CL_NxFS_Start

Description 	

	- This function starts NxFS file system.
	
Arguments		

	- none 
	
Return Value		

	- returns E_NxFS_RESULT_TYPE error values if fails, otherwise returns E_ERROR_SUCCESS

Dependencies

	- This function should be called before any other functions are called.
	
Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT E_NxFS_RESULT_TYPE CL_NxFS_Start
(

);

/*----------------------------------------------------------------------------------------------
Function Name	

	CL_NxFS_Stop

Description 	

	- This function stops NxFS file system.
	
Arguments		

	- none
	
Return Value		

	- returns E_NxFS_RESULT_TYPE error values if fails, otherwise returns E_ERROR_SUCCESS

Dependencies

	- This function should be called after each CL_NxFS_Device_Close() of opened devices is called
	
Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT E_NxFS_RESULT_TYPE CL_NxFS_Stop
(
	
);


#endif



/*-----------------------------------------------------------------------------
Function Name	

	CL_NxFS_Device_Open

Description 	

	-	Given a pathname for a device, this function opens device and 
	    returns a device handle
	
Arguments		

	- sub_dev_name	: sub-device name of partitioned device name. e.g) /dev/sdb1, /dev/sdb2...
	- flags 		: E_DEV_ATTRIBUTE_TYPE
	- dev_handle	: pointer to the DEV_HANDLE type data structure
	
Return Value		

	- returns ERRNO_SUCCESS if success, 
	  otherwise returns a non-negative integer 

Side Effects		

-----------------------------------------------------------------------------*/

DLLEXPORT E_NxFS_RESULT_TYPE CL_NxFS_Device_Open
(	
	const char 			*sub_dev_name,
	uint32				flags,
	DEV_HANDLE			*dev_handle
);
/*----------------------------------------------------------------------------------------------
Function Name	

	CL_NxFS_Device_Close

Description 	

	-	Given a device handle, this function closes a device 
	
Arguments		

	- dev_handle		: pointer to the device handle of a device 
	
Return Value		

	- none

Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT E_NxFS_RESULT_TYPE CL_NxFS_Device_Close
(	
	DEV_HANDLE  *dev_handle

);

/*----------------------------------------------------------------------------------------------
Function Name	

	CL_NxFS_Get_Master_Device_Info

Description 	

	- This function reads device information.
	
Arguments		

	- master_dev_name	: master device name string. e.g) /dev/sdb , /dev/sdc ...
	- dev_info			: pointer to the data structure on device geometry
	
Return Value		

	- returns E_NxFS_RESULT_TYPE error values if fails, otherwise returns E_ERROR_SUCCESS

Dependencies

	
Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT E_NxFS_RESULT_TYPE CL_NxFS_Get_Master_Device_Info
(
	sint8 				*master_dev_name, 
	struct NxFS_DevInfo *dev_info
);

/*----------------------------------------------------------------------------------------------
Function Name	

	CL_NxFS_Get_Device_Info

Description 	

	- This function reads device information.
	
Arguments		

	- dev_handle	: device handle of storage device 
	- dev_info		: pointer to the data structure on device geometry
	
Return Value		

	- returns E_NxFS_RESULT_TYPE error values if fails, otherwise returns E_ERROR_SUCCESS

Dependencies

	- CL_NxFS_Device_Open () should be called before invoking this function.
	
Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT E_NxFS_RESULT_TYPE	CL_NxFS_Get_Device_Info 	
(	
	DEV_HANDLE           *dev_handle,
	struct NxFS_DevInfo  *dev_info
);


#if !defined(__NXFS_SS_DEV_OC_SCHEME__)
/*----------------------------------------------------------------------------------------------
Function Name	

	CL_NxFS_Device_Init

Description 	

	- This function initializes NxFS file system.
	
Arguments		

	- dev_handle	: device handle of storage device 
	
Return Value		

	- returns E_NxFS_RESULT_TYPE error values if fails, otherwise returns E_ERROR_SUCCESS

Dependencies

	- CL_NxFS_Device_Open () should be called before invoking this function.
	
Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT E_NxFS_RESULT_TYPE CL_NxFS_Device_Init
(
	DEV_HANDLE *dev_handle
);

/*----------------------------------------------------------------------------------------------
Function Name	

	CL_NxFS_Device_Deinit

Description 	

	- This function deinitializes NxFS file system.
	
Arguments		

	- dev_handle	: device handle of storage device 
	- mode 			: deinit mode indicating blocking or non-blocking mode
					  0 - non blocking mode, 1 - blocking mode
	
Return Value		

	- returns E_NxFS_RESULT_TYPE error values if fails, otherwise returns E_ERROR_SUCCESS

Dependencies

	- CL_NxFS_Device_Init () should be called before invoking this function.
	
Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT E_NxFS_RESULT_TYPE CL_NxFS_Device_Deinit
(
	DEV_HANDLE *dev_handle,
	E_DEINIT_MODE_TYPE	mode
);
#endif

/*----------------------------------------------------------------------------------------------
Function Name	

	CL_NxFS_Get_Partition_Info

Description 	

	- This function reads partition information & boot record.
	
Arguments		

	- dev_handle	: device handle of a storage device 
	
Return Value		

	- returns E_NxFS_RESULT_TYPE error values if fails, otherwise returns E_ERROR_SUCCESS

Dependencies

	- CL_NxFS_Device_Open () should be called before invoking this function.
	
Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT E_NxFS_RESULT_TYPE	CL_NxFS_Get_Partition_Info 	
(	
	DEV_HANDLE                  *dev_handle
);

/*----------------------------------------------------------------------------------------------
Function Name	

	CL_NxFS_Create_Partition

Description 	

	- This function is used to create a partition.
	
Arguments		

	- master_device_path_name	: master device name string of the master physical device
	- file_system_type 			: file system type to create
	- size						: partition size to create in 1 MBytes (1024*1024 bytes)
	- partition_no				: pointer to the partition descriptor. 
								  On successful partition creation, partition descriptor, 
								  a non-negative integer value, is passed through this parameter.
								  Otherwise, 0 is passed.
	- alloc_sectors    			: pointer to the allocated number of sectors
	
Return Value		

	- returns E_NxFS_RESULT_TYPE error values if fails, otherwise returns E_ERROR_SUCCESS

Dependencies

	- CL_NxFS_Device_Open () should be called before invoking this function.

Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT E_NxFS_RESULT_TYPE		CL_NxFS_Create_Partition
(	
	sint8		*master_device_path_name,
	uint32		file_system_type,
	uint64		size,
	uint32		*partition_no,
	uint32		*alloc_sectors
);

/*----------------------------------------------------------------------------------------------
Function Name	

	CL_NxFS_Delete_Partition

Description 	

	- This function is used to delete a partition.
	
Arguments		

	- dev_name		   : device name string of the master physical device
	- file_system_type : file system type to create
	
Return Value		

	- returns E_NxFS_RESULT_TYPE error values if fails, otherwise returns E_ERROR_SUCCESS

Dependencies

	- CL_NxFS_Device_Open () should be called before invoking this function.

Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT E_NxFS_RESULT_TYPE CL_NxFS_Delete_Partition
(
	sint8		*dev_name,
	uint32		file_system_type
);

/*----------------------------------------------------------------------------------------------
Function Name	

	CL_NxFS_Format

Description 	

	- This function formats FAT32 partition or NxFS partition.
	
Arguments		

	- master_dev_name	   : master device (physical drive) name string
	- sub_dev_name		   : sub-device(partition) name string
	- file_system_type : file system type to format
	- format_type	   : E_FORMAT_TYPE format type
	- dir_list		   : pointer to the directory list to create. 
						 This is valid if file_system_type is NxFS.
	- fptr_format_doen : function pointer to the callback after performing format
	
Return Value		

	- returns E_NxFS_RESULT_TYPE error values if fails, otherwise returns E_ERROR_SUCCESS

Dependencies

	- CL_NxFS_Device_Open () should be called before invoking this function.

Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT E_NxFS_RESULT_TYPE CL_NxFS_Format
(	
	sint8			   *mnaster_dev_name, 
	sint8			   *sub_dev_name, 
	E_NxFS_FILESYSTEM_TYPE file_system_type,
	E_NxFS_FORMAT_TYPE	   format_type,
	struct NxFS_DirTbl	   *dir_list,
	void (*fptr_format_done_cb)(
								E_NxFS_FILESYSTEM_TYPE file_system_type,
								E_NxFS_FORMAT_TYPE	   format_type,
								struct NxFS_DirTbl	   *dir_list,
								E_NxFS_RESULT_TYPE	   *format_errno)
);


/*----------------------------------------------------------------------------------------------
Function Name	

	CL_NxFS_Get_Dir_Desciptor

Description 	

	- This function returns a directory descriptor.
	
Arguments		

	- dev_handle	   : pointer to the device handle of a storage device 
	- dir_name		   : directory name string
	- dir_desc		   : pointer to the directory descriptor. 
	
Return Value		

	- returns E_NxFS_RESULT_TYPE error values if fails, otherwise returns E_ERROR_SUCCESS

Dependencies


Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT E_NxFS_RESULT_TYPE CL_NxFS_Get_Dir_Desciptor
(	
	DEV_HANDLE *dev_handle, 
	sint8      *dir_name, 
	sint32 	   *dir_desc
);

/*----------------------------------------------------------------------------------------------
Function Name	

	CL_NxFS_Get_Dir_Info

Description 	

	- This function returns a directory information.
	
Arguments		

	- dev_handle	   : pointer to the device handle of a storage device 
	- dir_name		   : directory name string
	- dir_info		   : pointer to the struct NxFS_DirTableEntry. 
	- total_space      : pointer to the total space of the specified dir_name directory
	- free_space       : pointer to the free space of the specified dir_name directory
	
Return Value		

	- returns E_NxFS_RESULT_TYPE error values if fails, otherwise returns E_ERROR_SUCCESS

Dependencies

	

Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT E_NxFS_RESULT_TYPE CL_NxFS_Get_Dir_Info
(
	DEV_HANDLE           *dev_handle, 
	sint8                *dir_name,
	struct NxFS_DirEntry *dir_info,
	uint64               *total_space,
	uint64               *free_space
);

/*----------------------------------------------------------------------------------------------
Function Name	

	CL_NxFS_File_Open

Description 	

	- This function returns a file descriptor.
	
Arguments		

	- dev_handle	   : pointer to the device handle of a storage device 
	- file_path		   : file path string
	- mode		       : pointer to the mode string "r", or "w"
	- meta_data        : pointer to the meta data of file
	- meta_data_size   : length of meta data 
	- assigned_fd      : This is an assigned fd value of the specified file. 
	                     If mode is "w", this argument should be set to 0.
	                     If mode is "r", this argument should be set to the assigned . 
	
Return Value		

	- returns file descriptor on success. If fails, returns -1

Dependencies

	
Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT sint32 CL_NxFS_File_Open
(	
	DEV_HANDLE *dev_handle, 
	sint8      *file_path,
	sint8      *mode,
	void       *meta_data,
	uint32      meta_data_size,
	sint32      assigned_fd
);

/*----------------------------------------------------------------------------------------------
Function Name	

	CL_NxFS_Get_FileList

Description 	

	- This function retrieves file lists of a directory.
	
Arguments		

	- dev_handle	   : pointer to the device handle of a storage device 
	- dir_name 	       : directory name string
	- file_number_from : pointer to the (file number)-th file from the start of file info table
	- file_list	       : pointer to the struct NxFS_FileList 
	                     The num_files member variable specified the number of files to get.
	                     e.g) If file_number_from = 10 and file_list->num_files = 10,
	                          this function returns information on the files from 11-th to 20-th.
	- sort_type        : defining sorting type of file list on buffer

Return Value		

	- return the number of files read on success, otherwise returns -1 on failure.
	- return zero if end of file information list is reached

Dependencies

	
Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT sint32 CL_NxFS_Get_FileList
(	
	DEV_HANDLE	         *dev_handle, 
	sint8 	             *dir_name,
	uint32               *file_number_from,
	struct NxFS_FileList *file_list,
	E_SORTING_TYPE       sort_type
);

/*----------------------------------------------------------------------------------------------
Function Name	

	CL_NxFS_Get_FileInfo

Description 	

	- This function retrieves file information.
	
Arguments		

	- dev_handle	   : pointer to the device handle of a storage device 
	- file_desc		   : file descriptor
	- file_info 	   : pointer to the struct NxFS_FileInfo 
	
Return Value		

	- returns E_NxFS_RESULT_TYPE valus on failure, returns ERRNO_SUCCESS on success

Dependencies

	
Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT E_NxFS_RESULT_TYPE CL_NxFS_Get_FileInfo
(	
	DEV_HANDLE	  *dev_handle, 
	sint32         file_desc,
	struct NxFS_FileInfo *file_info
);

/*----------------------------------------------------------------------------------------------
Function Name	

	fptr_file_delete_cb

Description 	

	- This function is file deletion callback function prototype.
	
Arguments		

	- fd  	        : pointer to the device handle of a storage device 
	- file_name_len	: deleted file name length
	- file_name     : deleted file name 
	
Return Value		

	- none

Dependencies

	
Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT void (*fptr_file_delete_cb)
(
	sint32   fd, 
	sint32   file_name_len, 
	sint8   *file_name
);

/*----------------------------------------------------------------------------------------------
Function Name	

	CL_NxFS_File_Delete_Callback_Register

Description 	

	- This function is to register callback function to be invoked after file deletion.
	
Arguments		

	- fd			: pointer to the device handle of a storage device 
	- file_name_len : deleted file name length
	- file_name 	: deleted file name 
	
Return Value		

	- none

Dependencies

	
Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT void CL_NxFS_File_Delete_Callback_Register
( 
	void (*fptr_file_delete_cb)(sint32 fd, sint32 file_name_len, sint8 *file_name)
);

/*----------------------------------------------------------------------------------------------
Function Name	

	CL_NxFS_File_Write

Description 	

	- This function write data to storage device.
	
Arguments		

	- dev_handle	      : pointer to the device handle of a storage device 
	- data 	              : pointer to the data to write
	- size 	              : length of data 
	- file_desc 	      : file descriptor
	
Return Value		

	- returns the number of bytes written

Dependencies

	
Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT sint32 CL_NxFS_File_Write
( 
	DEV_HANDLE	  *dev_handle, 
	void		  *data,
	uint32         size,
	sint32 		   file_desc
);

/*----------------------------------------------------------------------------------------------
Function Name	

	CL_Nxfs_File_Read

Description 	

	- This function reads data from storage device.
	
Arguments		

	- dev_handle	   : pointer to the device handle of a storage device 
	- data			   : pointer to the buffer to store of read data
	- size			   : length of buffer 
	- file_desc 	   : file descriptor
	
Return Value		

	- returns the number of bytes written

Dependencies

	
Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT sint32 CL_NxFS_File_Read
(	
	DEV_HANDLE	  *dev_handle, 
	void		  *data,
	uint32         size,
	sint32 		   file_desc
);

/*----------------------------------------------------------------------------------------------
Function Name	

	CL_NxFS_File_Seek

Description 	

	- This function sets the file position indicator for the file descriptor.  The new position,
	  measured in bytes, is obtained by adding offset bytes to the position specified by seek_origin.
	  If whence  is  set to  NXFS_SEEK_ORIGIN_START, NXFS_SEEK_ORIGIN_CUR, or NXFS_SEEK_ORIGIN_END,  
	  the offset is relative to the start of the file, the current position indicator, or end-of-file, 
	  respectively.
	  
Arguments		

	- dev_handle	   : pointer to the device handle of a storage device 
	- file_desc		   : file descriptor
	- seek_origin	   : NXFS_SEEK_ORIGIN_START, NXFS_SEEK_ORIGIN_CUR, or NXFS_SEEK_ORIGIN_END
	- offset	 	   : offset bytes to the position specified by seek_origin
	
Return Value		

	- returns 1 on success, otherwise returns 0 on failure

Dependencies

	
Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT sint8 CL_NxFS_File_Seek
(
	DEV_HANDLE *dev_handle, 
	sint32      file_desc, 
	uint8       seek_origin, 
	sint32      offset
);

/*----------------------------------------------------------------------------------------------
Function Name	

	CL_NxFS_File_Tell

Description 	

	- This function  obtains  the current value of the file position indicator for the stream 
	  pointed to by file descriptor.
	  
Arguments		

	- dev_handle	   : pointer to the device handle of a storage device 
	- file_desc 	   : file descriptor
	
Return Value		

	- returns the current value of the file position indicator on success, \
	  otherwize returns -1 on failure
	 

Dependencies

	
Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT sint32 CL_NxFS_File_Tell
(
	DEV_HANDLE *dev_handle, 
	sint32		file_desc
);

/*----------------------------------------------------------------------------------------------
Function Name	

	CL_NxFS_File_Close

Description 	

	- This function closes a file.
	
Arguments		

	- dev_handle	   : pointer to the device handle of a storage device 
	- file_desc 	   : file descriptor
	
Return Value		

	- returns 0 on success. return -1 on failure.

Dependencies

	
Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT sint32 CL_NxFS_File_Close
( 
	DEV_HANDLE	  *dev_handle, 
	sint32 		   file_desc
);


/*----------------------------------------------------------------------------------------------
Function Name	

	CL_NxFS_Get_FAT32_Partition

Description 	

	- This function returns the FAT32 partition number in MBR Partition Table.
	
Arguments		

	- dev_handle	   : pointer to the device handle of a storage device 
	- partition_no 	   : pointer to the FAT32 partition number and this value 
						 starts from 1 (1 is the first partition table entry of MBR)
	
Return Value		

	- returns 0 on success. return -1 on failure.

Dependencies

	
Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT sint32 CL_NxFS_Get_FAT32_Partition
( 
	DEV_HANDLE	  *dev_handle, 
	sint32 		  *partition_no
);

/*----------------------------------------------------------------------------------------------
Function Name	

	CL_NxFS_Get_NxFS_Partition

Description 	

	- This function returns the NxFS partition number in MBR Partition Table.
	
Arguments		

	- dev_handle	   : pointer to the device handle of a storage device 
	- partition_no	   : pointer to the NxFS partition number and this value 
						 starts from 1 (1 is the first partition table entry of MBR)
	
Return Value		

	- returns 0 on success. return -1 on failure.

Dependencies

	
Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT sint32 CL_NxFS_Get_NxFS_Partition
( 
	DEV_HANDLE	  *dev_handle, 
	sint32 		  *partition_no
);


/*----------------------------------------------------------------------------------------------
Function Name	

	CL_NxFS_Get_Available_Partition 

Description 	

	- This function returns a free partition number and its available size in MBytes.
	
Arguments		

	- dev_handle     : pointer to the device handle of a storage device 
	- partition_no   : pointer to the available paritition
	- available_size : pointer to the available paritition size in MBytes
	
Return Value		

	- returns 0 on success. return -1 on failure.

Dependencies

	- The maximum partition number to allocate is 2, and one for FAT32 and the other for NxFS
      If a partition is allocated in the rear part of a device, then, this function returns the
      available space of the front part and vice versa.
	
Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT sint32 CL_NxFS_Get_Available_Partition
( 
	DEV_HANDLE	  *dev_handle, 
	uint32  *partition_no, 
	uint32  *available_size
);
/*----------------------------------------------------------------------------------------------
Function Name	

	CL_NxFS_Fwrite_Header_Update 

Description 	

	- This function writes file header data of a file specified by fd.
	
Arguments		

	- dev_handle : pointer to the device handle of a storage device 
	- fd	     : file descriptor
	- offset     : offset value of header to write
	- buf        : pointer to buffer of header data
	- size       : buffer size
	
Return Value		

	- returns ERRNO_SUCCESS on success. returns other values on failure.

Dependencies
	
Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT E_NxFS_RESULT_TYPE CL_NxFS_Fwrite_Header_Update
(
	DEV_HANDLE *dev_handle, 
	sint32      fd, 
	uint32      offset, 
	uint8      *buf, 
	uint32      size
);

/*----------------------------------------------------------------------------------------------
Function Name	

	CL_NxFS_Check_NxFS_partition

Description 	

	- This function write header data of a file specified by fd.
	
Arguments		

	- dev_name   : device name string
	
Return Value		

	- returns TRUE if NxFS partition exists, otherwise returns FALSE

Dependencies
	
Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT sint32 CL_NxFS_Check_NxFS_partition
(
	sint8     *dev_name
);


/*----------------------------------------------------------------------------------------------
Function Name	

	CL_NxFS_Get_Finfo_with_Fname

Description 	

	- This function retrieves file information with a give path (including file) name.
	
Arguments		

	- dev_handle	   : pointer to the device handle of a storage device 
	- file_name 	   : file name string (e.g : /NORMAL/test.dat)
	- file_info 	   : pointer to the struct NxFS_FileInfo 
	
Return Value		

	- returns E_NxFS_RESULT_TYPE valus on failure, returns ERRNO_SUCCESS on success

Dependencies

	
Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT E_NxFS_RESULT_TYPE CL_NxFS_Get_Finfo_with_Fname
(	
	DEV_HANDLE	  *dev_handle, 
	sint8		  *file_name,
	struct NxFS_FileInfo *file_info
);
/*----------------------------------------------------------------------------------------------
Function Name	

	CL_NxFS_Get_Dir_List

Description 	

	- This function retrieves directory lists
	
Arguments		

	- dev_handle	   : pointer to the device handle of a storage device 
	- dir_list	 	   : pointer to the struct NxFS_DirTbl 
	
Return Value		

	- returns E_NxFS_RESULT_TYPE valus on failure, returns ERRNO_SUCCESS on success

Dependencies

	
Side Effects		

	- none
----------------------------------------------------------------------------------------------*/
DLLEXPORT E_NxFS_RESULT_TYPE CL_NxFS_Get_Dir_List
(
	DEV_HANDLE	  		*dev_handle, 
	struct NxFS_DirTbl 	*dir_list
);

#ifdef __WIN32_PLATFORM__ // Visual Studio specific macro
	#ifdef __cplusplus
	}
	#endif
 
#else 

#endif 

#endif /* __NXFS_API_H__ */

/*=============================< End of File >===============================*/

