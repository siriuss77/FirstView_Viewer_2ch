#pragma once

#include <atlimage.h>

#define MAX_PRT_STR		64
#define MAX_PRT_MEMO	256

enum _TAG_CAMERA
{
	FrontCamera = 0,
	RearCamera,

	MAX_CHANNEL
};

typedef struct _PRINTINFO_
{
	TCHAR	tzTitle[MAX_PRT_STR];
	TCHAR	tzLine1[MAX_PRT_STR];
	TCHAR	tzLine1C[MAX_PRT_STR];
	TCHAR	tzLine2[MAX_PRT_STR];
	TCHAR	tzLine2C1[MAX_PRT_STR];
	TCHAR	tzLine2C2[MAX_PRT_STR];
	TCHAR	tzLine2C3[MAX_PRT_STR];
	TCHAR	tzLine3[MAX_PRT_STR];
	TCHAR	tzLine3C[MAX_PRT_STR];
	TCHAR	tzLine4[MAX_PRT_STR];
	TCHAR	tzMemo1[MAX_PRT_MEMO];
	TCHAR	tzMemo2[MAX_PRT_MEMO];
	TCHAR	tzMemo3[MAX_PRT_MEMO];
	TCHAR	tzMemo4[MAX_PRT_MEMO];
	TCHAR	tzMemo5[MAX_PRT_MEMO];
	TCHAR	tzMemo6[MAX_PRT_MEMO];
	TCHAR	tzBottom[MAX_PRT_STR];
}PRINTINFO;

typedef struct _STREAM
{
	BYTE*	pStream;
	DWORD	dwWidth;
	DWORD   dwHeight;
} STREAM, *PSTREAM;

typedef struct _REPORT_ITEM
{
	TCHAR	strDateTime[MAX_PATH];
	CImage	*cpStreamImage[MAX_CHANNEL];
	CImage	*cpMapImage;
	double	fNGps;
	double	fEGps;
	double	pDigit;
	double	fXSensor;
	double	fYSensor;
	double	fZSensor;
	double	fSpeed;

	int		nSpeedMode;
	int 		streamCount;
	STREAM	vidStream[4];
} REPORT_ITEM, *PREPORT_ITEM;

class CReportPreview
{
public:
	void GetPrintOutDC(CDC* pDC, RECT* pRc,	PREPORT_ITEM pReportItem, PRINTINFO* pPrintInfo);
	void PrintOut(PREPORT_ITEM pReportItem, PRINTINFO*	pPrintInfo);
};
