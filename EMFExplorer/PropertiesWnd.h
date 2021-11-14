
#pragma once
#include <memory>

struct PropertyNode;
struct PropertyNodeArray;

class CPropertiesToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

using CEMFRecPropertyGridCtrlBase = CMFCPropertyGridCtrl;

class CEMFRecPropertyGridCtrl : public CEMFRecPropertyGridCtrlBase
{
public:
	BOOL EndEditItem(BOOL bUpdateData = TRUE) override;
};

class CPropertiesWnd : public CDockablePane
{
// Construction
public:
	CPropertiesWnd() noexcept;

	void AdjustLayout();

// Attributes
public:
	void SetVSDotNetLook(BOOL bSet)
	{
		m_wndPropList.SetVSDotNetLook(bSet);
		m_wndPropList.SetGroupNameFullWidth(bSet);
	}

	void OnChangeVisualStyle();

	void Reset();

	void SetPropList(std::shared_ptr<PropertyNode> props);
private:
	CMFCPropertyGridProperty* AddPropList(const PropertyNode& node);

	CMFCPropertyGridProperty* AddPropListForArray(const PropertyNodeArray& node);

	CMFCPropertyGridProperty* AddPropListForArrayMember(const PropertyNodeArray& node, size_t index);
protected:
	CFont m_fntPropList;
	CPropertiesToolBar m_wndToolBar;
	CEMFRecPropertyGridCtrl m_wndPropList;

// Implementation
public:
	virtual ~CPropertiesWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExpandAllProperties();
	afx_msg void OnUpdateExpandAllProperties(CCmdUI* pCmdUI);
	afx_msg void OnSortProperties();
	afx_msg void OnUpdateSortProperties(CCmdUI* pCmdUI);
	afx_msg void OnProperties1();
	afx_msg void OnUpdateProperties1(CCmdUI* pCmdUI);
	afx_msg void OnProperties2();
	afx_msg void OnUpdateProperties2(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);

	DECLARE_MESSAGE_MAP()

	void InitPropList();
	void SetPropListFont();
};

