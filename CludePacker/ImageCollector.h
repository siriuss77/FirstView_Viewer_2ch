
#pragma once

#include <string>
#include <map>
#include <HELPER/ClHashedString.H>

class CImageCollector
{

public:
	CImageCollector(void);
	~CImageCollector(void);

	enum IMAGE_FORMAT
	{
		FORMAT_RGB888	= 0	,
		FORMAT_RGB8888		,
		FORMAT_INVALID
	};

	typedef struct _tagIMAGEINFO
	{
		std::wstring	m_wtsFilePath;
		INT				m_nSize;
		BOOL			m_bIsLoad;
		void*			m_pBuffer;

	} IMAGEINFO, *PIMAGEINFO;

	typedef std::map<HashedString,IMAGEINFO>			COLLECTIMAGE_POOL;
	typedef COLLECTIMAGE_POOL::iterator					ITER_COLLECTIMAGE;
	typedef std::pair<HashedString,IMAGEINFO>			PAIR_COLLECTIMAGE;

public:

		BOOL						isEmptyImageList	( VOID );
		INT							getImageListSize	( VOID );
		BOOL						addImagePath		( const HashedString& hashedPath );
		PIMAGEINFO					getImageInfo		( INT nIndex );
		BOOL						getImageHash		( INT nIndex, HashedString& imageHash );
		BOOL						removeImageInfo		( const HashedString& hashedPath );
		VOID						clearImageInfo		( VOID );

		BOOL						setImageRoot		( const CString& cstrImageRoot );

private:

		COLLECTIMAGE_POOL			m_ImageList;

		CString						m_cstrImageRoot;
		BOOL						m_bUseCompress;
};
