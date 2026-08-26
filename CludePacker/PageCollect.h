#pragma once


#include "ImageCollector.h"
#include <CLUDE/ClCludeApp.h>

#define	WM_USER_LOADIMAGE							( WM_USER + 100 )

class CPageCollect : public CPropertyPageEx
{
	DECLARE_DYNAMIC(CPageCollect)

public:
	CPageCollect();
	virtual ~CPageCollect();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_COLLECT_PAGE };

public:

			BOOL					SetCludeApp			( ClCludeApp* pCludeApp );
			ClCludeApp*				GetCludeApp			( VOID );
			BOOL					SetImageCollector	( CImageCollector* pImageCollector );
			CImageCollector*		GetImageCollector	( VOID );


protected:

	virtual void					DoDataExchange		( CDataExchange* pDX );    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:

	afx_msg void					OnClickCollectImage	();
	afx_msg	LRESULT					OnLoadImage			( WPARAM wParam, LPARAM lParam );
	virtual	BOOL					OnSetActive			();

private:

	static	DWORD WINAPI			CollectThread		( LPVOID lpArg );

	static	void					parseDialog			( TiXmlElement* pEleDialog, CImageCollector* pImageCollector );
	static	void					parseButton			( TiXmlElement* pEleButton, CImageCollector* pImageCollector );
	static	void					parseDigit			( TiXmlElement* pEleDigit, CImageCollector* pImageCollector );
	static	void					parseImage			( TiXmlElement* pEleImage, CImageCollector* pImageCollector );
	static	void					parseSlider			( TiXmlElement* pEleSlider, CImageCollector* pImageCollector );
	static	void					parseText			( TiXmlElement* pEleText, CImageCollector* pImageCollector );
	static	void					parseListCtrl		( TiXmlElement* pEleListCtrl, CImageCollector* pImageCollector );
	static	void					parseGSensor		( TiXmlElement* pEleGSensor, CImageCollector* pImageCollector );

private:

			ClCludeApp*				m_pCludeApp;
			CImageCollector*		m_pImageCollector;
};
