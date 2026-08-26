#pragma once


// CPagePackComplete 대화 상자입니다.

class CPageComplete : public CPropertyPageEx
{
	DECLARE_DYNAMIC(CPageComplete)

public:
	CPageComplete();
	virtual ~CPageComplete();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_COMPLETE_PAGE };


public:

	virtual	BOOL					OnSetActive			();


protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
