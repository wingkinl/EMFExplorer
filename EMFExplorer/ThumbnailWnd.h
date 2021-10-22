#pragma once
#include <memory>
class EMFAccess;

//#define HANDLE_THUMBNAIL_WND_PAINT_WITH_DOUBLE_BUFFER

using CThumbnailWndBase = CDockablePane;

class CThumbnailWnd : public CThumbnailWndBase
{
	// Construction
public:
	CThumbnailWnd() noexcept;

	// Attributes
public:
	void OnChangeVisualStyle();

	void SetEMFAccess(std::shared_ptr<EMFAccess> emf);

	void LoadEMFDataEvent(bool bBefore);

	void OnViewUpdateSizeScroll();
protected:
#ifndef HANDLE_THUMBNAIL_WND_PAINT_WITH_DOUBLE_BUFFER
	BOOL PreCreateWindow(CREATESTRUCT& cs) override;
#endif // HANDLE_THUMBNAIL_WND_PAINT_WITH_DOUBLE_BUFFER
protected:
	std::shared_ptr<EMFAccess>	m_emf;
	CRect	m_rcThumbnail;
	CRect	m_rcZoomRect;
	CPoint	m_ptStartMovePt;
	float	m_factor = 1.0f;
	// Implementation
public:
	virtual ~CThumbnailWnd();

protected:
#ifdef HANDLE_THUMBNAIL_WND_PAINT_WITH_DOUBLE_BUFFER
	afx_msg BOOL OnEraseBkgnd(CDC*);
#endif // HANDLE_THUMBNAIL_WND_PAINT_WITH_DOUBLE_BUFFER
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();

	DECLARE_MESSAGE_MAP()
};

