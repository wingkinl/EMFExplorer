
#include "pch.h"
#include "framework.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "EMFExplorer.h"
#include "PropertyTree.h"

#undef min
#undef max

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd() noexcept
{
}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	ON_COMMAND(ID_PROPERTIES2, OnProperties2)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar message handlers

void CPropertiesWnd::AdjustLayout()
{
	CWnd* pWndFrame = nullptr;
	if (GetSafeHwnd () == nullptr 
		|| ((pWndFrame = GetTopLevelFrame()) && pWndFrame->IsIconic()))
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(nullptr, rectClient.left, rectClient.top + cyTlb, rectClient.Width(), rectClient.Height() -cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CPropertiesWnd::OnChangeVisualStyle()
{
	COLORREF clrBackground = (COLORREF)-1;
	COLORREF clrText = (COLORREF)-1;
	COLORREF clrGroupBackground = (COLORREF)-1;
	COLORREF clrGroupText = (COLORREF)-1;
	COLORREF clrDescriptionBackground = (COLORREF)-1;
	COLORREF clrDescriptionText = (COLORREF)-1;
	COLORREF clrLine = (COLORREF)-1;

	if (theApp.IsDarkTheme())
	{
		clrBackground = theApp.m_crfDarkThemeBkColor;
		clrText = theApp.m_crfDarkThemeTxtColor;
		clrGroupBackground = clrBackground;
		clrGroupText = clrText;
		clrDescriptionBackground = clrBackground;
		clrDescriptionText = clrText;
		clrLine = RGB(0x8d,0x8d,0x8d);
	}
	m_wndPropList.SetCustomColors(clrBackground, clrText, clrGroupBackground, clrGroupText, clrDescriptionBackground, clrDescriptionText, clrLine);
}

void CPropertiesWnd::Reset()
{
	m_wndPropList.RemoveAll();
	m_wndPropList.Invalidate();
}

void CPropertiesWnd::SetPropList(std::shared_ptr<PropertyNode> props)
{
	CWaitCursor wait;
	Reset();
	m_wndPropList.SetRedraw(FALSE);
	for (auto& sub : props->sub)
	{
		auto pGridProp = AddPropList(*sub);
		if (pGridProp)
		{
			pGridProp->AllowEdit(FALSE);
			m_wndPropList.AddProperty(pGridProp);
		}
	}
	m_wndPropList.SetRedraw(TRUE);
	m_wndPropList.Invalidate();
}

#include <tuple>

CMFCPropertyGridProperty* CPropertiesWnd::AddPropList(const PropertyNode& node)
{
	CMFCPropertyGridProperty* pGridProp = nullptr;
	CMFCPropertyGridProperty* pSubProp = nullptr;
	switch (node.GetNodeType())
	{
	case PropertyNode::NodeTypeBranch:
		pGridProp = new CMFCPropertyGridProperty(node.name);
		for (auto& sub : node.sub)
		{
			pSubProp = AddPropList(*sub);
			if (pSubProp)
			{
				pSubProp->AllowEdit(FALSE);
				pGridProp->AddSubItem(pSubProp);
			}
		}
		break;
	case PropertyNode::NodeTypeText:
		pGridProp = new CMFCPropertyGridProperty(node.name, CString(node.text));
		break;
	case PropertyNode::NodeTypeRectInt:
		pGridProp = new CMFCPropertyGridProperty(node.name, 0, TRUE);
		{
			auto& data = (const PropertyNodeRectInt&)node;
			std::vector<std::tuple<LPCWSTR, LONG>> vals;
			vals.emplace_back(L"left", data.rect.left);
			vals.emplace_back(L"top", data.rect.top);
			vals.emplace_back(L"right", data.rect.right);
			vals.emplace_back(L"bottom", data.rect.bottom);
			vals.emplace_back(L"width", ((CRect&)data.rect).Width());
			vals.emplace_back(L"height", ((CRect&)data.rect).Height());
			for (auto& v : vals)
			{
				pSubProp = new CMFCPropertyGridProperty(std::get<0>(v), (_variant_t)std::get<1>(v), nullptr);
				pSubProp->AllowEdit(FALSE);
				pGridProp->AddSubItem(pSubProp);
			}
		}
		break;
	case PropertyNode::NodeTypeRectFloat:
		pGridProp = new CMFCPropertyGridProperty(node.name, 0, TRUE);
		{
			auto& prop = (const PropertyNodePlusRectF&)node;
			std::vector<std::tuple<LPCWSTR, emfplus::Float>> vals;
			vals.emplace_back(L"X", prop.data.X);
			vals.emplace_back(L"Y", prop.data.Y);
			vals.emplace_back(L"Width", prop.data.Width);
			vals.emplace_back(L"Height", prop.data.Height);
			for (auto& v : vals)
			{
				pSubProp = new CMFCPropertyGridProperty(std::get<0>(v), (_variant_t)std::get<1>(v), nullptr);
				pSubProp->AllowEdit(FALSE);
				pGridProp->AddSubItem(pSubProp);
			}
			pGridProp->Expand();
		}
		break;
	case PropertyNode::NodeTypeRectData:
		pGridProp = new CMFCPropertyGridProperty(node.name, 0, TRUE);
		{
			auto& prop = (const PropertyNodePlusRectData&)node;
			std::vector<std::tuple<LPCWSTR, emfplus::Float>> vals;
			bool bFloat = !prop.data.AsInt;
			vals.emplace_back(L"X", bFloat ? prop.data.fval->X : prop.data.ival->X);
			vals.emplace_back(L"Y", bFloat ? prop.data.fval->Y : prop.data.ival->Y);
			vals.emplace_back(L"Width", bFloat ? prop.data.fval->Width : prop.data.ival->Width);
			vals.emplace_back(L"Height", bFloat ? prop.data.fval->Height : prop.data.ival->Height);
			for (auto& v : vals)
			{
				pSubProp = new CMFCPropertyGridProperty(std::get<0>(v), (_variant_t)std::get<1>(v), nullptr);
				pSubProp->AllowEdit(FALSE);
				pGridProp->AddSubItem(pSubProp);
			}
			pGridProp->Expand();
		}
		break;
	case PropertyNode::NodeTypeSizeInt:
		pGridProp = new CMFCPropertyGridProperty(node.name, 0, TRUE);
		{
			auto& data = (const PropertyNodeSizeInt&)node;
			pSubProp = new CMFCPropertyGridProperty(L"width", (_variant_t)data.size.cx, nullptr);
			pSubProp->AllowEdit(FALSE);
			pGridProp->AddSubItem(pSubProp);
			pSubProp = new CMFCPropertyGridProperty(L"height", (_variant_t)data.size.cy, nullptr);
			pSubProp->AllowEdit(FALSE);
			pGridProp->AddSubItem(pSubProp);
		}
		break;
	case PropertyNode::NodeTypePointDataArray:
		pGridProp = new CMFCPropertyGridProperty(node.name, 0);
		{
			auto& prop = (const PropertyNodePlusPointDataArray&)node;
			const size_t maxCount = 10;
			auto nActualSize = prop.data.GetSize();
			size_t nCount = std::min(maxCount, nActualSize);
			auto& data = prop.data;
			bool bFloat = !data.fvals.empty();
			for (size_t ii = 0; ii < nCount; ++ii)
			{
				CStringW strName, strVal;
				strName.Format(L"[%llu]", ii);
				if (bFloat)
					strVal.Format(L"%g, %g", data.fvals[ii].x, data.fvals[ii].y);
				else
					strVal.Format(L"%d, %d", data.ivals[ii].x, data.ivals[ii].y);
				pSubProp = new CMFCPropertyGridProperty(strName, strVal, nullptr);
				pSubProp->AllowEdit(FALSE);
				pGridProp->AddSubItem(pSubProp);
			}
			if (nActualSize > nCount)
			{
				pSubProp = new CMFCPropertyGridProperty(L"Size", std::to_wstring(nActualSize).c_str(), nullptr);
				pSubProp->AllowEdit(FALSE);
				pGridProp->AddSubItem(pSubProp);
			}
			// TODO, add way to view more data
			pGridProp->Expand();
		}
		break;
	case PropertyNode::NodeTypeMatrix:
		pGridProp = new CMFCPropertyGridProperty(node.name, 0, TRUE);
		{
			auto& prop = (const PropertyNodePlusTransform&)node;
			for (size_t ii = 0; ii < _countof(prop.data); ++ii)
			{
				CStringW strName, strVal;
				strName.Format(L"[%llu]", ii);
				strVal.Format(L"%lg", prop.data[ii]);
				pSubProp = new CMFCPropertyGridProperty(strName, strVal, nullptr);
				pSubProp->AllowEdit(FALSE);
				pGridProp->AddSubItem(pSubProp);
			}
			auto& mat = prop.data;
			if (mat[emfplus::OTM12] == 0.0f && mat[emfplus::OTM21] == 0.0f)
			{
				if (mat[emfplus::OTM11] == 1.0f && mat[emfplus::OTM22] == 1.0f)
				{
					if (mat[emfplus::OTMDX] || mat[emfplus::OTMDY])
					{
						CStringW str;
						str.Format(L"(%lf, %lf)", mat[emfplus::OTMDX], mat[emfplus::OTMDY]);
						pSubProp = new CMFCPropertyGridProperty(L"translate", str, nullptr);
						pSubProp->AllowEdit(FALSE);
						pGridProp->AddSubItem(pSubProp);
					}
				}
				else if (mat[emfplus::OTMDX] == 0.0f && mat[emfplus::OTMDY] == 0.0f)
				{
					// scaling
					CStringW str;
					str.Format(L"(%lf, %lf)", mat[emfplus::OTM11], mat[emfplus::OTM22]);
					pSubProp = new CMFCPropertyGridProperty(L"scale", str, nullptr);
					pSubProp->AllowEdit(FALSE);
					pGridProp->AddSubItem(pSubProp);
				}
			}
			pGridProp->Expand();
		}
		break;
	case PropertyNode::NodeTypeColor:
		{
			auto& prop = (const PropertyNodeColor&)node;
			pGridProp = new CMFCPropertyGridColorProperty(node.name, prop.data.ToCOLORREF(), nullptr);
			pGridProp->AllowEdit(FALSE);
		}
		break;
	case PropertyNode::NodeTypeFont:
		break;
	default:
		pGridProp = new CMFCPropertyGridProperty(node.name, (_variant_t)node.text, nullptr);
		break;
	}
	return pGridProp;
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("Failed to create Properties Grid \n");
		return -1;      // fail to create
	}

	InitPropList();

	OnChangeVisualStyle();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* Is locked */);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* Locked */);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
{
}

void CPropertiesWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnProperties1()
{
	// TODO: Add your command handler code here
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: Add your command update UI handler code here
}

void CPropertiesWnd::OnProperties2()
{
	// TODO: Add your command handler code here
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO: Add your command update UI handler code here
}

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	//m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	//m_wndPropList.MarkModifiedProperties();
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
}
