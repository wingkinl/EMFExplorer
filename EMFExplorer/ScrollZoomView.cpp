#include "pch.h"
#include "framework.h"
#include "ScrollZoomView.h"
#undef min
#undef max
#include <algorithm>

CScrollZoomView::CScrollZoomView()
{

}

CScrollZoomView::~CScrollZoomView()
{

}

BEGIN_MESSAGE_MAP(CScrollZoomView, CScrollZoomViewBase)
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CANCELMODE()
END_MESSAGE_MAP()

void CScrollZoomView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	UpdateViewSize();
}

CSize CScrollZoomView::GetViewSize() const
{
	return CSize(1, 1);
}

void CScrollZoomView::UpdateViewSize()
{
	CSize szScroll(0, 0);
	if (m_fitWndType == FitToNone)
	{
		CSize szImage = GetViewSize();
		szScroll.cx = (int)(szImage.cx * m_fZoomFactor);
		szScroll.cy = (int)(szImage.cy * m_fZoomFactor);
	}
	SetScrollSizes(MM_TEXT, szScroll);
}

void CScrollZoomView::OnDraw(CDC* pDC)
{
	float factor = GetRealZoomFactor();
	CSize szImage = GetViewSize();
	CRect rcDraw(0, 0, szImage.cx, szImage.cy);

	int nOldMapMode = pDC->SetMapMode(MM_ISOTROPIC);
	auto oldWndExt = pDC->SetWindowExt(1000, 1000);
	auto oldVwpExt = pDC->SetViewportExt((int)(1000 * factor), (int)(1000 * factor));

	OnDrawZoomedView(pDC, rcDraw);

	pDC->SetMapMode(nOldMapMode);
	pDC->SetWindowExt(oldWndExt);
	pDC->SetViewportExt(oldVwpExt);
}

void CScrollZoomView::OnDrawZoomedView(CDC* pDC, const CRect& rect)
{

}

BOOL CScrollZoomView::OnMouseWheel(UINT fFlags, short zDelta, CPoint point)
{
	if (fFlags & (MK_CONTROL))
	{
		CWaitCursor wait;
		SetZoom(zDelta > 0);
		return FALSE;
	}
	return CScrollZoomViewBase::OnMouseWheel(fFlags, zDelta, point);
}

void CScrollZoomView::OnMouseMove(UINT nFlags, CPoint point)
{
	if ((nFlags & MK_LBUTTON) && GetCapture() == this)
	{
		CSize szScroll = m_ptStartMovePt - point;
		OnScrollBy(szScroll);
		m_ptStartMovePt = point;
		return;
	}
	CScrollZoomViewBase::OnMouseMove(nFlags, point);
}

void CScrollZoomView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_ptStartMovePt = point;
	SetCapture();
	CScrollZoomViewBase::OnLButtonDown(nFlags, point);
}

void CScrollZoomView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (GetCapture() == this)
	{
		ReleaseCapture();
	}
	CScrollZoomViewBase::OnLButtonUp(nFlags, point);
}

void CScrollZoomView::OnCancelMode()
{
	if (GetCapture() == this)
	{
		ReleaseCapture();
	}
	CScrollZoomViewBase::OnCancelMode();
}

void CScrollZoomView::SetZoomFactorRange(float fMin, float fMax)
{
	ASSERT(fMin > 0 && fMax > 0);
	if (fMin > fMax)
		std::swap(fMin, fMax);
	m_fMinZoomFactor = fMin;
	m_fMaxZoomFactor = fMax;
	if (m_fZoomFactor < fMin)
		m_fZoomFactor = fMin;
	else if (m_fZoomFactor > fMax)
		m_fZoomFactor = fMax;
	UpdateViewSize();
}

void CScrollZoomView::SetFitToWindow(FitWinType val)
{
	m_fitWndType = val;
	m_fZoomFactor = 1.0f;
	UpdateViewSize();
	Invalidate();
}

void CScrollZoomView::SetCenter(bool val)
{
	m_bCenter = val;
	UpdateViewSize();
	Invalidate();
}

float CScrollZoomView::GetRealZoomFactor() const
{
	float factor = 1.0f;
	if (m_fitWndType)
	{
		CSize szImage = GetViewSize();

		CRect rect;
		GetClientRect(rect);
		CSize szClient = rect.Size();
		switch (m_fitWndType)
		{
		case FitToWidth:
			factor = (float)szClient.cx / szImage.cx;
			break;
		case FitToHeight:
			factor = (float)szClient.cy / szImage.cy;
			break;
		case FitToBoth:
			{
				auto fScaleSrc = (float)szImage.cy / szImage.cx;
				auto fScaleDst = (float)szClient.cy / szClient.cx;
				if (fScaleSrc <= fScaleDst)
					factor = (float)szClient.cx / szImage.cx;
				else
					factor = (float)szClient.cy / szImage.cy;
			}
			break;
		}
	}
	else
		factor = m_fZoomFactor;
	return factor;
}

void CScrollZoomView::SetZoom(bool bIn)
{
	float factor = GetRealZoomFactor();
	if (bIn)
		factor *= 1.1f;
	else
		factor *= 0.9f;
	m_fitWndType = FitToNone;
	SetZoomFactor(factor);
}

bool CScrollZoomView::CanDoZoom(bool bIn) const
{
	if (m_fitWndType)
		return true;
	if (bIn)
		return m_fZoomFactor < m_fMaxZoomFactor;
	return m_fZoomFactor > m_fMinZoomFactor;
}

void CScrollZoomView::SetZoomFactor(float factor)
{
	if (factor < m_fMinZoomFactor)
		factor = m_fMinZoomFactor;
	else if (factor > m_fMaxZoomFactor)
		factor = m_fMaxZoomFactor;
	m_fZoomFactor = factor;
	m_fitWndType = FitToNone;
	UpdateViewSize();
	Invalidate();
}

