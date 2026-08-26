#pragma once

#include <CLUDE/ClCludeApp.H>


class CPageParse : public CPropertyPageEx
{
	DECLARE_DYNAMIC(CPageParse)

public:
	CPageParse();
	virtual ~CPageParse();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PARSE_PAGE };

public:

			BOOL					SetCludeApp			( ClCludeApp* pCludeApp );


protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:

	afx_msg void					OnClickAppPath		();
	virtual BOOL					OnInitDialog		();
	virtual BOOL					OnSetActive			();


private:

			ClCludeApp*				m_pCludeApp;
			std::string				m_stsAppPath;
			std::wstring			m_stsXmlPath;

			BOOL					m_bIsNeedFind;
};
