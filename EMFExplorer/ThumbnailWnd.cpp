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

	if (!m_emf)
		return;

	auto hdr = m_emf->GetMetafileHeader();
	CSize szImg(hdr.Width, hdr.Height);
	m_rcThumbnail = GetFitRect(rect, szImg, true);

	Gdiplus::Graphics gg(pDCDraw->GetSafeHdc());
	gg.SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
	gg.SetInterpolationMode(Gdiplus::InterpolationModeLowQuality);
	gg.SetSmoothingMode(Gdiplus::SmoothingModeHighSpeed);
	Gdiplus::SolidBrush brush(Gdiplus::Color::White);
	gg.FillRectangle(&brush, m_rcThumbnail.left, m_rcThumbnail.top, m_rcThumbnail.Width(), m_rcThumbnail.Height());
	m_emf->DrawMetafile(gg, m_rcThumbnail);

	auto pFrameWnd = DYNAMIC_DOWNCAST(CMainFrame, GetTopLevelFrame());
	auto pView = pFrameWnd ? DYNAMIC_DOWNCAST(CEMFExplorerView, pFrameWnd->GetActiveView()) : nullptr;
	if (pView)
	{
		CSize szView = pView->GetRealViewSize();
		CPoint ptView = pView->GetDeviceScrollPosition();
		CRect rcView(0, 0, szView.cx, szView.cy);
		m_factor = (float)m_rcThumbnail.Width() / szView.cx;
		rcView.left = ptView.x;
		rcView.top = ptView.y;
		CRect rcClient;
		pView->GetClientRect(&rcClient);
		rcView.right = rcView.left + rcClient.Width();
		rcView.bottom = rcView.top + rcClient.Height();
		m_rcZoomRect.left = (LONG)(rcView.left * m_factor);
		m_rcZoomRect.top = (LONG)(rcView.top * m_factor);
		m_rcZoomRect.right = (LONG)(rcView.right * m_factor);
		m_rcZoomRect.bottom = (LONG)(rcView.bottom * m_factor);
		m_rcZoomRect.OffsetRect(m_rcThumbnail.TopLeft());
		Gdiplus::Pen pen(Gdiplus::Color::Black);
		gg.DrawRectangle(&pen, m_rcZoomRect.left, m_rcZoomRect.top, m_rcZoomRect.Width(), m_rcZoomRect.Height());
	}
}

void CThumbnailWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if ((nFlags & MK_LBUTTON) && GetCapture() == this)
	{
		CSize szOffset = m_ptStartMovePt - point;
		
		m_ptStartMovePt = point;
		return;
	}
	CThumbnailWndBase::OnMouseMove(nFlags, point);
}

void CThumbnailWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_ptStartMovePt = point;
	SetCapture();
	CThumbnailWndBase::OnLButtonDown(nFlags, point);
}

void CThumbnailWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (GetCapture() == this)
	{
		ReleaseCapture();
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

