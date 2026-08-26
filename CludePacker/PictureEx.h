#pragma once


// CPictureEx

class CPictureEx : public CStatic
{
	DECLARE_DYNAMIC(CPictureEx)

public:
	CPictureEx();
	virtual ~CPictureEx();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
};

