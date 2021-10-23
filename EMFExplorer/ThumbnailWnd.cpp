#include "pch.h"
#include "framework.h"

#include "ThumbnailWnd.h"
#include "Resource.h"
#include "EMFExplorer.h"
#include "EMFRecAccess.h"
#include "MainFrm.h"
#include "EMFExplorerView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CThumbnailWnd

CThumbnailWnd::CThumbnailWnd() noexcept
{
}

CThumbnailWnd::~CThumbnailWnd()
{
}

BEGIN_MESSAGE_MAP(CThumbnailWnd, CThumbnailWndBase)
#ifdef HANDLE_THUMBNAIL_WND_PAINT_WITH_DOUBLE_BUFFER
	ON_WM_ERASEBKGND()
#endif // HANDLE_THUMBNAIL_WND_PAINT_WITH_DOUBLE_BUFFER
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CANCELMODE()
END_MESSAGE_MAP()

#ifdef HANDLE_THUMBNAIL_WND_PAINT_WITH_DOUBLE_BUFFER
BOOL CThumbnailWnd::OnEraseBkgnd(CDC*)
{
	return TRUE;
}
#else
BOOL CThumbnailWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	// double buffer
	cs.dwExStyle |= WS_EX_COMPOSITED;
	return CThumbnailWndBase::PreCreateWindow(cs);
}
#endif // HANDLE_THUMBNAIL_WND_PAINT_WITH_DOUBLE_BUFFER

void CThumbnailWnd::OnSize(UINT nType, int cx, int cy)
{
	Invalidate();
	CThumbnailWndBase::OnSize(nType, cx, cy);
}

void CThumbnailWnd::OnPaint()
{
	CPaintDC dc(this);
	CMemDC dcMem(dc, this);
	CDC* pDCDraw = &dcMem.GetDC();

	CRect rect;
	GetClientRect(&rect);
	
	COLORREF crfBk = GetSysColor(COLOR_WINDOW);
	if (theApp.IsDarkTheme())
		crfBk = theApp.m_crfDarkThemeBkColor;
	pDCDraw->FillSolidRect(rect, crfBk);

	DrawImage(pDCDraw, rect);
	DrawZoomArea(pDCDraw, rect);
}

void CThumbnailWnd::DrawImage(CDC* pDCDraw, CRect rect)
{
	if (!m_emf)
		return;
	auto hdr = m_emf->GetMetafileHeader();
	CSize szImg(hdr.Width, hdr.Height);
	m_rcImg = GetFitRect(rect, szImg, true);

	Gdiplus::Graphics gg(pDCDraw->GetSafeHdc());
	gg.SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
	gg.SetInterpolationMode(Gdiplus::InterpolationModeLowQuality);
	gg.SetSmoothingMode(Gdiplus::SmoothingModeHighSpeed);
	Gdiplus::SolidBrush brush(Gdiplus::Color::White);
	gg.FillRectangle(&brush, m_rcImg.left, m_rcImg.top, m_rcImg.Width(), m_rcImg.Height());
	m_emf->DrawMetafile(gg, m_rcImg);
}

void CThumbnailWnd::DrawZoomArea(CDC* pDCDraw, CRect rect)
{
	m_rcZoomRect.SetRectEmpty();

	auto pFrameWnd = DYNAMIC_DOWNCAST(CMainFrame, GetTopLevelFrame());
	auto pView = pFrameWnd ? DYNAMIC_DOWNCAST(CEMFExplorerView, pFrameWnd->GetActiveView()) : nullptr;
	if (!pView)
		return;
	CSize szView = pView->GetRealViewSize();
	CPoint ptView = pView->GetDeviceScrollPosition();
	CRect rcView(0, 0, szView.cx, szView.cy);
	m_factor = (float)m_rcImg.Width() / szView.cx;
	rcView.left = ptView.x;
	rcView.top = ptView.y;
	CRect rcClient;
	pView->GetClientRect(&rcClient);
	rcView.right = rcView.left + rcClient.Width();
	rcView.bottom = rcView.top + rcClient.Height();
	if (rcView.Width() >= szView.cx && rcView.Height() >= szView.cy)
		return;
	m_rcZoomRect.left = (LONG)(rcView.left * m_factor);
	m_rcZoomRect.top = (LONG)(rcView.top * m_factor);
	m_rcZoomRect.right = (LONG)(rcView.right * m_factor);
	m_rcZoomRect.bottom = (LONG)(rcView.bottom * m_factor);
	m_rcZoomRect.OffsetRect(m_rcImg.TopLeft());
	if (m_rcZoomRect.left < m_rcImg.left)
		m_rcZoomRect.left = m_rcImg.left;
	if (m_rcZoomRect.top < m_rcImg.top)
		m_rcZoomRect.top = m_rcImg.top;
	if (m_rcZoomRect.right > m_rcImg.right)
		m_rcZoomRect.right = m_rcImg.right;
	if (m_rcZoomRect.bottom > m_rcImg.bottom)
		m_rcZoomRect.bottom = m_rcImg.bottom;
	int oldRop2 = pDCDraw->SetROP2(R2_NOTXORPEN);
	pDCDraw->Rectangle(m_rcZoomRect);
	pDCDraw->SetROP2(oldRop2);
}

void CThumbnailWnd::ScrollViewByOffset(CSize szOffset)
{
	auto pFrameWnd = DYNAMIC_DOWNCAST(CMainFrame, GetTopLevelFrame());
	auto pView = pFrameWnd ? DYNAMIC_DOWNCAST(CEMFExplorerView, pFrameWnd->GetActiveView()) : nullptr;
	if (pView)
	{
		szOffset.cx = (LONG)(szOffset.cx / m_factor);
		szOffset.cy = (LONG)(szOffset.cy / m_factor);
		pView->OnScrollBy(szOffset);
	}
}

void CThumbnailWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if ((nFlags & MK_LBUTTON) && m_bStartedScrolling)
	{
		ASSERT(GetCapture() == this);
		CSize szOffset = point - m_ptStartMovePt;
		ScrollViewByOffset(szOffset);
		m_ptStartMovePt = point;
		return;
	}
	CThumbnailWndBase::OnMouseMove(nFlags, point);
}

void CThumbnailWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_rcImg.PtInRect(point))
	{
		m_bStartedScrolling = TRUE;
		m_ptStartMovePt = point;
		if (!m_rcZoomRect.PtInRect(point))
		{
			CSize szOffset = point - m_rcZoomRect.CenterPoint();
			ScrollViewByOffset(szOffset);
		}
		SetCapture();
		return;
	}
	CThumbnailWndBase::OnLButtonDown(nFlags, point);
}

void CThumbnailWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bStartedScrolling)
	{
		ASSERT(GetCapture() == this);
		ReleaseCapture();
		m_bStartedScrolling = FALSE;
	}
	CThumbnailWndBase::OnLButtonUp(nFlags, point);
}

void CThumbnailWnd::OnCancelMode()
{
	if (GetCapture() == this)
	{
		ReleaseCapture();
	}
	CThumbnailWndBase::OnCancelMode();
}

void CThumbnailWnd::OnChangeVisualStyle()
{
	
}

void CThumbnailWnd::SetEMFAccess(std::shared_ptr<EMFAccess> emf)
{
	m_emf = emf;
}

void CThumbnailWnd::LoadEMFDataEvent(bool bBefore)
{
	if (bBefore)
	{
		SetRedraw(FALSE);
		m_emf = nullptr;
	}
	else
	{
		SetRedraw(TRUE);
		Invalidate();
	}
}

void CThumbnailWnd::OnViewUpdateSizeScroll()
{
	Invalidate();
}

