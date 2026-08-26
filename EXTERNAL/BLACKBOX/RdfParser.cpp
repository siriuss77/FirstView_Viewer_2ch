/**************************************************************************************************
 *
 *      File Name       :  RdfParser.cpp
 *      Description     :  주행정보 기록
 *
 *      Creator         :   tony ( icaro Co., Ltd )
 *      Create Date     :   2013/09/14
 *      Update History  :   
 *
 *************************************************************************************************/
#include "stdafx.h"
#include <CLUDE/version.h>
#include <HELPER/CLUtil.H>
#include "BBoxUtil.h"
#include "RdfParser.h"

#define ZONE_RDF_INIT	1
#define ZONE_RDF_FUNC	1
#define ZONE_RDF_ERROR	1

#define RDF_FILE_FMT	TEXT("%04d_%04d-%02d-%02d")		// index, year, month, day


CRdfData::CRdfData(struct tm *pDate /*= NULL*/)
{
	m_rdf_data_size = 0;

	m_pRdfData = NULL;
	::ZeroMemory((void *)&m_time, sizeof(m_time));
	::ZeroMemory((void *)&m_RdfHdr, sizeof(ST_RDF_DATA_HEADER));

	if(pDate)
	{
		m_time = *pDate;
	}
	else
	{
		struct tm *today = NULL ;
		time_t ltime;
				
		time(&ltime);
		today = localtime(&ltime);

		m_time = *today;
//		m_time.tm_year += 1900 ;
//		m_time.tm_mon += 1 ;	
	}
}

CRdfData::~CRdfData()
{
	if(m_pRdfData)
		delete [] m_pRdfData;
}

int CRdfData::GetTimezone(void)
{
	return (int)m_RdfHdr.time_zone;
}

u32 CRdfData::GetDriver_id(void)
{
	return m_RdfHdr.driver_id;
}

u32 CRdfData::GetVehicle_id(void)
{
	return m_RdfHdr.vehicle_id;
}

u32 CRdfData::GetLocalTime(u32 time /* = NULL */)
{
	if(time == NULL)
		time = m_RdfHdr.utd_time;
	
	return time + (3600 * m_RdfHdr.time_zone);
}

LPST_RDF_DATA CRdfData::GetRdfData(unsigned int pos)
{
	if(pos < m_rdf_data_size && m_pRdfData)
		return &m_pRdfData[pos] ;

	return FALSE;
}
unsigned int CRdfData::GetLength(void)
{
	return m_rdf_data_size;
}

void CRdfData::ClearData(void)
{
	m_rdf_data_size = 0;

	if(m_pRdfData != NULL)
	{
		delete [] m_pRdfData;
		m_pRdfData = NULL;
	}
}
	
unsigned int CRdfData::AddRdfData(LPST_RDF_DATA pRdf)
{	
	if(pRdf)
	{
		if(m_rdf_data_size >= RDFDATA_COUNT_OF_DAY)
		{
			DEBUGMSG( ZONE_RDF_ERROR, ("%s(): Buffer Full !! (%d) ERROR!\n", __func__, m_rdf_data_size));
		}
		else
		{
			if(m_pRdfData == NULL)
			{
				m_pRdfData = (LPST_RDF_DATA) new BYTE[sizeof(ST_RDF_DATA)  * RDFDATA_COUNT_OF_DAY];
				::ZeroMemory((void *)m_pRdfData,  sizeof(ST_RDF_DATA) * RDFDATA_COUNT_OF_DAY);
			}
			
			m_pRdfData[m_rdf_data_size] = *pRdf;

#if 0
			if(m_rdf_data_size)
			{
				if(m_pRdfData[m_rdf_data_size-1].bbox.utd_time == m_pRdfData[m_rdf_data_size].bbox.utd_time)
					m_pRdfData[m_rdf_data_size].bbox.utd_time++;
			}
#endif			
			m_rdf_data_size++;
		}
	}

	return m_rdf_data_size;
}
	
// return : rdfdata count
int CRdfData::Load(CString strDir, struct tm *pDate /*= NULL*/) // date
{
	CRdfParser rdfparser;
	
	if(pDate)
	{
		m_time = *pDate;
	}
	
	ClearData();
	return rdfparser.GetRdfDataLoad(strDir, &m_time, this);
}


///////////////////////////////////////////
CRdfParser::CRdfParser(void)
{
}

CRdfParser::~CRdfParser(void)
{
}


// return : rdf data size
int CRdfParser::GetRdfDataLoad(CString strDir, struct tm *pDate, CRdfData *p_crdf_data)
{
	int rdf_count = 0;
//	CString strDir = CBBoxUtil::GetModuleDir();

	if(p_crdf_data == NULL)
		return 0;
	
	CString csDrive = strDir.Left(3);
	
	csDrive.Append(RDF_FOLDER);
	csDrive.Append(_T("\\"));
	
	if(OpenRdfFile(csDrive, pDate))
	{
		char *pstr = NULL;
		unsigned int ulFileSize = 0, ulReadSize = 0;
		
		ulFileSize = m_rdf_file.file.GetLength();

		if(ulFileSize > sizeof(ST_RDF_DATA_HEADER))
		{
			ulReadSize += m_rdf_file.file.Read((void *)&p_crdf_data->m_RdfHdr, sizeof(ST_RDF_DATA_HEADER));
		}
		else
		{
			DEBUGMSG( ZONE_RDF_ERROR, ("%s(): size(%d) ERROR!\n", __func__, ulFileSize));
			m_rdf_file.file.Close();
			return 0;
		}
		

		pstr = p_crdf_data->m_RdfHdr.header;
		DEBUGMSG( ZONE_RDF_FUNC, ("%s(): size(%d) Header %c%c%c%c \n", __func__, ulFileSize, pstr[0], pstr[1], pstr[2], pstr[3]));

		while(ulReadSize < ulFileSize)
		{
			ST_RDF_DATA rdf = { 0,};
			unsigned int ulSize = m_rdf_file.file.Read((void *)&rdf, sizeof(ST_RDF_DATA));

			if(ulSize !=  sizeof(ST_RDF_DATA))
				break;
			
			if(rdf.bbox.header[0] != 'R')
			{
				DEBUGMSG(ZONE_RDF_FUNC, ("%s(): Rdf Data End Position(%d)\n", __func__, ulReadSize - sizeof(ST_RDF_DATA)));
				break;		
			}

			p_crdf_data->AddRdfData(&rdf);
				
			ulReadSize += ulSize;
			rdf_count ++;
		}

		
		m_rdf_file.file.Close();
	}

	return rdf_count;
}

//return : 
BOOL CRdfParser::OpenRdfFile( CString cstrDir , struct tm *pDate )
{	
	BOOL bRet = FALSE;
	if( !cstrDir.GetLength() || pDate == NULL )
		return FALSE;
		
	WIN32_FIND_DATA	findData;
	HANDLE			hDir = NULL;

	hDir = FindFirstFileEx( cstrDir + TEXT("*.RDF"), FindExInfoStandard, &findData, FindExSearchNameMatch, NULL, 0 );
	if( hDir == INVALID_HANDLE_VALUE )
		return FALSE;

	
	do
	{
		if( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			if( !_tcscmp(findData.cFileName, TEXT(".")) || !_tcscmp(findData.cFileName, TEXT("..")) )
				continue;

			CString cstrSubDir = cstrDir + findData.cFileName + TEXT("\\");
			this->OpenRdfFile( cstrSubDir, pDate);
		}
		else
		{
			LPST_RDF_FILE p_rdf = &m_rdf_file;
			
			swscanf(findData.cFileName, RDF_FILE_FMT, &p_rdf->file_index, &p_rdf->tm.tm_year, &p_rdf->tm.tm_mon, &p_rdf->tm.tm_mday);

			DEBUGMSG( ZONE_RDF_FUNC, ("%s(): search...  index:%d, %d-%d-%d\n", __func__, p_rdf->file_index, p_rdf->tm.tm_year, p_rdf->tm.tm_mon, p_rdf->tm.tm_mday));

			if(pDate->tm_year + 1900 == p_rdf->tm.tm_year && pDate->tm_mon+1 == p_rdf->tm.tm_mon && pDate->tm_mday == p_rdf->tm.tm_mday)
			{
				p_rdf->cstrFileName = cstrDir + findData.cFileName;
				DEBUGMSG( ZONE_RDF_FUNC, ("%s(): find index:%d, %d-%d-%d\n", __func__, p_rdf->file_index, p_rdf->tm.tm_year, p_rdf->tm.tm_mon, p_rdf->tm.tm_mday));

				if( !p_rdf->file.Open(p_rdf->cstrFileName, CFile::modeRead | CFile::typeBinary, NULL) )
				{
					DEBUGMSG( ZONE_RDF_ERROR, ("Can not open rdf file." ));
				}
				else
					bRet = TRUE;

				break;
			}
		}

	} while( FindNextFile(hDir, &findData) );

	FindClose(hDir);
	
	return bRet;
}

