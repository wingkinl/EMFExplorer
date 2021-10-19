#pragma once

using CScrollZoomViewBase = CScrollView;

class CScrollZoomView : public CScrollZoomViewBase
{
public:
	CScrollZoomView();
	~CScrollZoomView();
public:
	void SetZoomFactor(float factor);
	float GetZoomFactor() const { return m_fZoomFactor; }

	float GetMinZoomFactor() const { return m_fMinZoomFactor; }
	float GetMaxZoomFactor() const { return m_fMaxZoomFactor; }

	void SetZoomFactorRange(float fMin, float fMax);

	enum FitWinType
	{
		FitToNone,
		FitToWidth,
		FitToHeight,
		FitToBoth,
	};

	FitWinType GetFitToWindow() const { return m_fitWndType; }
	void SetFitToWindow(FitWinType val);

	bool GetCenter() const { return !!m_bCenter; }
	void SetCenter(bool val);

	float GetRealZoomFactor() const;

	void SetZoom(bool bIn);
	bool CanDoZoom(bool bIn) const;
public:
	void OnInitialUpdate() override;

	virtual CSize GetViewSize() const;

	CSize GetRealViewSize() const;

	virtual bool IsZoomAllowed() const { return true; }
protected:
	void UpdateViewSize();

	void OnDraw(CDC* pDC) override;

	virtual void OnDrawZoomedView(CDC* pDC, const CRect& rect);
protected:
	float	m_fZoomFactor = 1.0f;
	float	m_fMinZoomFactor = 0.01f;
	float	m_fMaxZoomFactor = 32.0f;
	FitWinType	m_fitWndType = FitToBoth;

	CPoint	m_ptStartMovePt;
protected:
	afx_msg BOOL OnMouseWheel(UINT fFlags, short zDelta, CPoint point);

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
protected:
	DECLARE_MESSAGE_MAP()
};
