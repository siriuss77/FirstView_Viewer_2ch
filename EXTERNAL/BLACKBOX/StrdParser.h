/**************************************************************************************************
 *
 *      File Name       : StrdParser.h
 *      Description     :  I3 UserData
 *
 *      Creator         :   tony ( datech Co., Ltd )
 *      Create Date     :   2019/07/02
 *      Update History  :   
 *
 *************************************************************************************************/

#ifndef __STRD_PARSER_H__
#define __STRD_PARSER_H__

#include <BLACKBOX/TextData.h>
///////////////////// DEFINE ////////////////////////////////////////////

///////////////////// TYPE ///////////////////////////////////////////

////////////////////////////////////////////////////////////////

bool security_file_change (const char* filepath, bool avi_to_security);

class CStrdParser  
{
public:
	CStrdParser(void);
	virtual ~CStrdParser(void);

	HRESULT GetTextData(char *filepath, BYTE *chunk_data, int *pSize, unsigned long *pCts);
	int CStrdParserOpen(const char* filepath);
	void CStrdParserClose(void);
	
	double fps;
       double duration;
       double VideoFramesPerSec;
       double AudioBlockPerSec ;

       int HeaderSize;
       int VideoSize ;
       int AudioSize ;

       double VideoBitRate;
       double AudioBitRate;
       double FileBitRate;
		
	CTextData *mp_textData;
	
protected:
	FILE* m_fp;
	BYTE * mp_UserData;
	int m_UserDataSize;

	int m_parse_pos;
	UINT64 m_start_time;
	int m_cts;

};

#endif
