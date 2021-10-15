#include "pch.h"
#include "framework.h"
#include "ScrollZoomView.h"

CScrollZoomView::CScrollZoomView()
{

}

CScrollZoomView::~CScrollZoomView()
{

}

BEGIN_MESSAGE_MAP(CScrollZoomView, CScrollZoomViewBase)
END_MESSAGE_MAP()

void CScrollZoomView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sz = GetDocSize();
	SetScrollSizes(MM_TEXT, sz);
}

CSize CScrollZoomView::GetDocSize() const
{
	return CSize(1, 1);
}

