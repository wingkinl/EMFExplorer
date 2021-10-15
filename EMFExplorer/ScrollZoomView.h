#pragma once

using CScrollZoomViewBase = CScrollView;

class CScrollZoomView : public CScrollZoomViewBase
{
public:
	CScrollZoomView();
	~CScrollZoomView();
public:
	void OnInitialUpdate() override;

	virtual CSize GetDocSize() const;
protected:
protected:
	DECLARE_MESSAGE_MAP()
};
