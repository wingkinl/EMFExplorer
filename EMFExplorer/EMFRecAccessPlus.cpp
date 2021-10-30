#include "pch.h"
#include "framework.h"
#include "EMFRecAccessPlus.h"
#include "EMFAccess.h"
#include "EMFStruct2Props.h"

void EMFRecAccessGDIPlusRec::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccess::CacheProperties(ctxt);
	m_propsCached->AddValue(L"RecordFlags", m_recInfo.Flags, true);
}

void EMFRecAccessGDIPlusRecHeader::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusControlCat::CacheProperties(ctxt);
	auto pHdrRec = (OEmfPlusHeader*)m_recInfo.Data;
	auto& hdr = ctxt.pEMF->GetMetafileHeader();
	ASSERT(pHdrRec->Version == hdr.Version);
	ASSERT(pHdrRec->LogicalDpiX == hdr.LogicalDpiX);
	ASSERT(pHdrRec->EmfPlusFlags == hdr.EmfPlusFlags);
	ASSERT(pHdrRec->LogicalDpiY == hdr.LogicalDpiY);
	GetPropertiesFromGDIPlusHeader(m_propsCached.get(), hdr);
}

void EMFRecAccessGDIPlusRecObject::Preprocess(EMFAccess* pEMF)
{

}

void EMFRecAccessGDIPlusObjWrapper::CacheProperties(const CachePropertiesContext& ctxt, PropertyNode* pNode) const
{
	if (!m_obj)
		return;
	pNode->AddValue(L"Version", m_obj->Version, true);
}

class EMFRecAccessGDIPlusBrushWrapper : public EMFRecAccessGDIPlusObjWrapper
{
public:
	EMFRecAccessGDIPlusBrushWrapper()
	{
		m_obj.reset(new OEmfPlusBrush);
	}
protected:
	void CacheProperties(const CachePropertiesContext& ctxt, PropertyNode* pNode) const override
	{
		EMFRecAccessGDIPlusObjWrapper::CacheProperties(ctxt, pNode);
		auto pObj = (OEmfPlusBrush*)m_obj.get();
		auto pBranch = pNode->AddBranch(L"Brush");
		EmfStruct2Properties::Build(*pObj, pBranch.get());
	}
};

class EMFRecAccessGDIPlusPenWrapper : public EMFRecAccessGDIPlusObjWrapper
{
public:
	EMFRecAccessGDIPlusPenWrapper()
	{
		m_obj.reset(new OEmfPlusPen);
	}
protected:
	void CacheProperties(const CachePropertiesContext& ctxt, PropertyNode* pNode) const override
	{
		EMFRecAccessGDIPlusObjWrapper::CacheProperties(ctxt, pNode);
		auto pObj = (OEmfPlusPen*)m_obj.get();
		auto pBranch = pNode->AddBranch(L"Pen");
		EmfStruct2Properties::Build(*pObj, pBranch.get());
	}
};

class EMFRecAccessGDIPlusPathWrapper : public EMFRecAccessGDIPlusObjWrapper
{
public:
	EMFRecAccessGDIPlusPathWrapper()
	{
		m_obj.reset(new OEmfPlusPath);
	}
protected:
	void CacheProperties(const CachePropertiesContext& ctxt, PropertyNode* pNode) const override
	{
		EMFRecAccessGDIPlusObjWrapper::CacheProperties(ctxt, pNode);
		auto pObj = (OEmfPlusPath*)m_obj.get();
		auto pBranch = pNode->AddBranch(L"Path");
		EmfStruct2Properties::Build(*pObj, pBranch.get());
	}
};

class EMFRecAccessGDIPlusRegionWrapper : public EMFRecAccessGDIPlusObjWrapper
{
public:
	EMFRecAccessGDIPlusRegionWrapper()
	{
		m_obj.reset(new OEmfPlusRegion);
	}
protected:
	void CacheProperties(const CachePropertiesContext& ctxt, PropertyNode* pNode) const override
	{
		EMFRecAccessGDIPlusObjWrapper::CacheProperties(ctxt, pNode);
		auto pObj = (OEmfPlusRegion*)m_obj.get();
		auto pBranch = pNode->AddBranch(L"Region");
		EmfStruct2Properties::Build(*pObj, pBranch.get());
	}
};

class EMFRecAccessGDIPlusImageWrapper : public EMFRecAccessGDIPlusObjWrapper
{
public:
	EMFRecAccessGDIPlusImageWrapper()
	{
		m_obj.reset(new OEmfPlusImage);
	}
protected:
	void CacheProperties(const CachePropertiesContext& ctxt, PropertyNode* pNode) const override
	{
		EMFRecAccessGDIPlusObjWrapper::CacheProperties(ctxt, pNode);
		auto pImg = (OEmfPlusImage*)m_obj.get();
		auto pBranch = pNode->AddBranch(L"Image");
		EmfStruct2Properties::Build(*pImg, pBranch.get());
	}

	bool CacheGDIPlusObject(EMFAccess* pEMF) override
	{
		auto pImg = (OEmfPlusImage*)m_obj.get();
		switch (pImg->Type)
		{
		case OImageDataType::Bitmap:
			// TODO
			break;
		case OImageDataType::Metafile:
			if (!m_emf)
				m_emf = std::make_shared<EMFAccess>(pImg->ImageDataMetafile->MetafileData);
			if (pEMF && m_emf->GetNestedPath().empty())
			{
				m_emf->AddNestedPath(pEMF->GetNestedPath().c_str());
				m_emf->AddNestedPath(std::to_wstring(m_pObjRec->GetIndex() + 1).c_str());
			}
			break;
		}
		return true;
	}

	enum {
		ImgPreviewCX = 400,
		ImgPreviewCY = 400,
	};

	bool DrawPreview(EMFRecAccess::PreviewContext* info = nullptr) override
	{
		if (!CacheGDIPlusObject(nullptr))
			return false;
		auto pImg = (OEmfPlusImage*)m_obj.get();
		switch (pImg->Type)
		{
		case OImageDataType::Metafile:
			if (!m_emf)
				return false;
			if (!info)
				return true;
			auto& hdr = m_emf->GetMetafileHeader();
			CSize szEMF(hdr.Width, hdr.Height);
			CRect rect = info->rect;
			if (info->bCalcOnly)
			{
				CSize sz = info->GetDefaultImgPreviewSize();
				rect.right = rect.left + sz.cx;
				rect.bottom = rect.top + sz.cy;
			}
			CRect rcFit = GetFitRect(rect, szEMF, true);
			info->szPreferedSize = rcFit.Size();
			if (!info->bCalcOnly)
			{
				Gdiplus::Graphics gg(info->pDC->GetSafeHdc());
				m_emf->DrawMetafile(gg, rcFit);
			}
			return true;
		}
		return false;
	}

	std::shared_ptr<EMFAccess> GetEMFAccess() const override
	{
		return m_emf;
	}
private:
	std::shared_ptr<EMFAccess>	m_emf;
};

class EMFRecAccessGDIPlusFontWrapper : public EMFRecAccessGDIPlusObjWrapper
{
public:
	EMFRecAccessGDIPlusFontWrapper()
	{
		m_obj.reset(new OEmfPlusFont);
	}
protected:
	void CacheProperties(const CachePropertiesContext& ctxt, PropertyNode* pNode) const override
	{
		EMFRecAccessGDIPlusObjWrapper::CacheProperties(ctxt, pNode);
		auto pObj = (OEmfPlusFont*)m_obj.get();
		auto pBranch = pNode->AddBranch(L"Font");
		EmfStruct2Properties::Build(*pObj, pBranch.get());
	}
};

class EMFRecAccessGDIPlusStringFormatWrapper : public EMFRecAccessGDIPlusObjWrapper
{
public:
	EMFRecAccessGDIPlusStringFormatWrapper()
	{
		m_obj.reset(new OEmfPlusStringFormat);
	}
protected:
	void CacheProperties(const CachePropertiesContext& ctxt, PropertyNode* pNode) const override
	{
		EMFRecAccessGDIPlusObjWrapper::CacheProperties(ctxt, pNode);
		auto pObj = (OEmfPlusStringFormat*)m_obj.get();
		auto pBranch = pNode->AddBranch(L"StringFormat");
		EmfStruct2Properties::Build(*pObj, pBranch.get());
	}
};

class EMFRecAccessGDIPlusImageAttributesWrapper : public EMFRecAccessGDIPlusObjWrapper
{
public:
	EMFRecAccessGDIPlusImageAttributesWrapper()
	{
		m_obj.reset(new OEmfPlusImageAttributes);
	}
protected:
	void CacheProperties(const CachePropertiesContext& ctxt, PropertyNode* pNode) const override
	{
		EMFRecAccessGDIPlusObjWrapper::CacheProperties(ctxt, pNode);
		auto pObj = (OEmfPlusImageAttributes*)m_obj.get();
		auto pBranch = pNode->AddBranch(L"ImageAttributes");
		EmfStruct2Properties::Build(*pObj, pBranch.get());
	}
};

class EMFRecAccessGDIPlusCustomLineCapWrapper : public EMFRecAccessGDIPlusObjWrapper
{
public:
	EMFRecAccessGDIPlusCustomLineCapWrapper()
	{
		m_obj.reset(new OEmfPlusCustomLineCap);
	}
protected:
	void CacheProperties(const CachePropertiesContext& ctxt, PropertyNode* pNode) const override
	{
		EMFRecAccessGDIPlusObjWrapper::CacheProperties(ctxt, pNode);
		auto pObj = (OEmfPlusCustomLineCap*)m_obj.get();
		auto pBranch = pNode->AddBranch(L"CustomLineCap");
		EmfStruct2Properties::Build(*pObj, pBranch.get());
	}
};

static EMFRecAccessGDIPlusObjWrapper* CreatePlusObjectAccessWrapper(OObjType type)
{
	EMFRecAccessGDIPlusObjWrapper* pObj = nullptr;
	switch (type)
	{
	case OObjType::Brush:
		pObj = new EMFRecAccessGDIPlusBrushWrapper;
		break;
	case OObjType::Pen:
		pObj = new EMFRecAccessGDIPlusPenWrapper;
		break;
	case OObjType::Path:
		pObj = new EMFRecAccessGDIPlusPathWrapper;
		break;
	case OObjType::Region:
		pObj = new EMFRecAccessGDIPlusRegionWrapper;
		break;
	case OObjType::Image:
		pObj = new EMFRecAccessGDIPlusImageWrapper;
		break;
	case OObjType::Font:
		pObj = new EMFRecAccessGDIPlusFontWrapper;
		break;
	case OObjType::StringFormat:
		pObj = new EMFRecAccessGDIPlusStringFormatWrapper;
		break;
	case OObjType::ImageAttributes:
		pObj = new EMFRecAccessGDIPlusImageAttributesWrapper;
		break;
	case OObjType::CustomLineCap:
		pObj = new EMFRecAccessGDIPlusCustomLineCapWrapper;
		break;
	default:
		return nullptr;
	}
	return pObj;
}

static inline LPCWSTR EMFPlusObjectTypeText(OObjType type)
{
	static const LPCWSTR aText[] = {
		L"Invalid", L"Brush", L"Pen", L"Path", L"Region", L"Image",
		L"Font", L"StringFormat", L"ImageAttributes", L"CustomLineCap"
	};
	return aText[(int)type];
}

EMFRecAccessGDIPlusObjWrapper* EMFRecAccessGDIPlusRecObject::GetObjectWrapper()
{
	if (!m_recDataCached)
	{
		auto nObjType = OEmfPlusRecObjectReader::GetObjectType(m_recInfo);
		auto pObjWrapper = CreatePlusObjectAccessWrapper(nObjType);
		if (!pObjWrapper)
		{
			ASSERT(0);
			return nullptr;
		}
		pObjWrapper->m_pObjRec = this;
		m_recDataCached.reset(pObjWrapper);
		DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
		VERIFY(pObjWrapper->GetObject()->Read(reader, m_recInfo.DataSize));
	}
	return m_recDataCached.get();
}

void EMFRecAccessGDIPlusRecObject::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusObjectCat::CacheProperties(ctxt);
	auto nObjType = OEmfPlusRecObjectReader::GetObjectType(m_recInfo);
	m_propsCached->AddText(L"ObjectType", EMFPlusObjectTypeText(nObjType));
	auto nObjectID = (u8t)(m_recInfo.Flags & OEmfPlusRecObjectReader::FlagObjectIDMask);
	m_propsCached->AddValue(L"Index", nObjectID);
	auto pObjWrapper = GetObjectWrapper();
	if (pObjWrapper)
	{
		pObjWrapper->CacheProperties(ctxt, m_propsCached.get());
	}
}

bool EMFRecAccessGDIPlusRecObject::DrawPreview(PreviewContext* info)
{
	auto pObjWrapper = GetObjectWrapper();
	if (pObjWrapper)
		return pObjWrapper->DrawPreview(info);
	return false;
}

void EMFRecAccessGDIPlusRecClear::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(ctxt);
	auto pRec = (OEmfPlusRecClear*)m_recInfo.Data;
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodeColor>(L"Color", pRec->Color));
}

void EMFRecAccessGDIPlusRecFillRects::Preprocess(EMFAccess* pEMF)
{
	if (!(m_recInfo.Flags & OEmfPlusRecFillRects::FlagS))
	{
		auto nID = ((OEmfPlusRecFillRects*)m_recInfo.Data)->BrushId;
		auto pRec = pEMF->GetObjectCreationRecord(nID, true);
		if (pRec)
			AddLinkRecord(pRec, LinkedObjTypeBrush);
	}
}

void EMFRecAccessGDIPlusRecFillRects::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(ctxt);
	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusRectDataArray>(L"RectData", m_recDataCached.RectData));
	if (m_recInfo.Flags & OEmfPlusRecFillRects::FlagS)
	{
		m_propsCached->sub.emplace_back(std::make_shared<PropertyNodeColor>(L"Brush", (emfplus::OEmfPlusARGB&)m_recDataCached.BrushId));
	}
	else
	{
		m_propsCached->AddValue(L"BrushID", m_recDataCached.BrushId);
		auto pBrushRec = GetLinkedRecord(LinkedObjTypeBrush);
		if (pBrushRec)
		{
			auto pBrushProp = pBrushRec->GetProperties(ctxt);
			auto pBrushNode = m_propsCached->AddBranch(L"Brush");
			pBrushNode->sub = pBrushProp->sub;
		}
	}
}

void EMFRecAccessGDIPlusRecDrawRects::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawRects::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
	{
		AddLinkRecord(pRec, LinkedObjTypePen);
	}
}

void EMFRecAccessGDIPlusRecDrawRects::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(ctxt);
	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusRectDataArray>(L"RectData", m_recDataCached.RectData));
	auto pPenRec = GetLinkedRecord(LinkedObjTypePen);
	if (pPenRec)
	{
		auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawRects::FlagObjectIDMask);
		m_propsCached->AddValue(L"PenID", nID);
		auto pPenProp = pPenRec->GetProperties(ctxt);
		auto pPenNode = m_propsCached->AddBranch(L"Pen");
		pPenNode->sub = pPenProp->sub;
	}
}

void EMFRecAccessGDIPlusRecFillPolygon::Preprocess(EMFAccess* pEMF)
{
	if (!(m_recInfo.Flags & OEmfPlusRecFillPolygon::FlagS))
	{
		auto nID = ((OEmfPlusRecFillPolygon*)m_recInfo.Data)->BrushId;
		auto pRec = pEMF->GetObjectCreationRecord(nID, true);
		if (pRec)
			AddLinkRecord(pRec, LinkedObjTypeBrush);
	}
}

void EMFRecAccessGDIPlusRecFillPolygon::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(ctxt);
	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));
	if (m_recInfo.Flags & OEmfPlusRecFillPolygon::FlagS)
	{
		m_propsCached->sub.emplace_back(std::make_shared<PropertyNodeColor>(L"Brush", (emfplus::OEmfPlusARGB&)m_recDataCached.BrushId));
	}
	else
	{
		m_propsCached->AddValue(L"BrushID", m_recDataCached.BrushId);
		auto pBrushRec = GetLinkedRecord(LinkedObjTypeBrush);
		if (pBrushRec)
		{
			auto pBrushProp = pBrushRec->GetProperties(ctxt);
			auto pBrushNode = m_propsCached->AddBranch(L"Brush");
			pBrushNode->sub = pBrushProp->sub;
		}
	}
	bool bRelative = (m_recInfo.Flags & OEmfPlusRecFillPolygon::FlagP);
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusPointDataArray>(L"PointData", m_recDataCached.PointData, bRelative));
}

void EMFRecAccessGDIPlusRecDrawLines::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawLines::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
	{
		AddLinkRecord(pRec, LinkedObjTypePen);
	}
}

void EMFRecAccessGDIPlusRecDrawLines::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(ctxt);
	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawLines::FlagObjectIDMask);
	m_propsCached->AddValue(L"PenID", nID);
	m_propsCached->AddValue(L"Closed", m_recInfo.Flags & OEmfPlusRecDrawLines::FlagL);
	bool bRelative = (m_recInfo.Flags & OEmfPlusRecDrawLines::FlagP);
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusPointDataArray>(L"PointData", m_recDataCached.PointData, bRelative));
	auto pPenRec = GetLinkedRecord(LinkedObjTypePen);
	if (pPenRec)
	{
		auto pPenProp = pPenRec->GetProperties(ctxt);
		auto pPenNode = m_propsCached->AddBranch(L"Pen");
		pPenNode->sub = pPenProp->sub;
	}
}

void EMFRecAccessGDIPlusRecFillEllipse::Preprocess(EMFAccess* pEMF)
{
	if (!(m_recInfo.Flags & OEmfPlusRecFillEllipse::FlagS))
	{
		auto nID = ((OEmfPlusRecFillEllipse*)m_recInfo.Data)->BrushId;
		auto pRec = pEMF->GetObjectCreationRecord(nID, true);
		if (pRec)
			AddLinkRecord(pRec, LinkedObjTypeBrush);
	}
}

void EMFRecAccessGDIPlusRecFillEllipse::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(ctxt);
	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));
	if (m_recInfo.Flags & OEmfPlusRecFillEllipse::FlagS)
	{
		m_propsCached->sub.emplace_back(std::make_shared<PropertyNodeColor>(L"Brush", (emfplus::OEmfPlusARGB&)m_recDataCached.BrushId));
	}
	else
	{
		m_propsCached->AddValue(L"BrushID", m_recDataCached.BrushId);
		auto pBrushRec = GetLinkedRecord(LinkedObjTypeBrush);
		if (pBrushRec)
		{
			auto pBrushProp = pBrushRec->GetProperties(ctxt);
			auto pBrushNode = m_propsCached->AddBranch(L"Brush");
			pBrushNode->sub = pBrushProp->sub;
		}
	}
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusRectData>(L"RectData", m_recDataCached.RectData));
}

void EMFRecAccessGDIPlusRecDrawEllipse::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawEllipse::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
	{
		AddLinkRecord(pRec, LinkedObjTypePen);
	}
}

void EMFRecAccessGDIPlusRecDrawEllipse::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(ctxt);
	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawEllipse::FlagObjectIDMask);
	m_propsCached->AddValue(L"PenID", nID);
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusRectData>(L"RectData", m_recDataCached.RectData));
	auto pPenRec = GetLinkedRecord(LinkedObjTypePen);
	if (pPenRec)
	{
		auto pPenProp = pPenRec->GetProperties(ctxt);
		auto pPenNode = m_propsCached->AddBranch(L"Pen");
		pPenNode->sub = pPenProp->sub;
	}
}

void EMFRecAccessGDIPlusRecFillPie::Preprocess(EMFAccess* pEMF)
{
	if (!(m_recInfo.Flags & OEmfPlusRecFillPie::FlagS))
	{
		auto nID = ((OEmfPlusRecFillPie*)m_recInfo.Data)->BrushId;
		auto pRec = pEMF->GetObjectCreationRecord(nID, true);
		if (pRec)
			AddLinkRecord(pRec, LinkedObjTypeBrush);
	}
}

void EMFRecAccessGDIPlusRecFillPie::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(ctxt);
	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));
	if (m_recInfo.Flags & OEmfPlusRecFillPie::FlagS)
	{
		m_propsCached->sub.emplace_back(std::make_shared<PropertyNodeColor>(L"Brush", (emfplus::OEmfPlusARGB&)m_recDataCached.BrushId));
	}
	else
	{
		m_propsCached->AddValue(L"BrushID", m_recDataCached.BrushId);
		auto pBrushRec = GetLinkedRecord(LinkedObjTypeBrush);
		if (pBrushRec)
		{
			auto pBrushProp = pBrushRec->GetProperties(ctxt);
			auto pBrushNode = m_propsCached->AddBranch(L"Brush");
			pBrushNode->sub = pBrushProp->sub;
		}
	}
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusArcData>(L"ArcData", m_recDataCached.ArcData));
}

void EMFRecAccessGDIPlusRecDrawPie::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawPie::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
	{
		AddLinkRecord(pRec, LinkedObjTypePen);
	}
}

void EMFRecAccessGDIPlusRecDrawPie::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(ctxt);
	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawPie::FlagObjectIDMask);
	m_propsCached->AddValue(L"PenID", nID);
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusArcData>(L"ArcData", m_recDataCached.ArcData));
	auto pPenRec = GetLinkedRecord(LinkedObjTypePen);
	if (pPenRec)
	{
		auto pPenProp = pPenRec->GetProperties(ctxt);
		auto pPenNode = m_propsCached->AddBranch(L"Pen");
		pPenNode->sub = pPenProp->sub;
	}
}

void EMFRecAccessGDIPlusRecDrawArc::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawArc::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
	{
		AddLinkRecord(pRec, LinkedObjTypePen);
	}
}

void EMFRecAccessGDIPlusRecDrawArc::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(ctxt);
	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawArc::FlagObjectIDMask);
	m_propsCached->AddValue(L"PenID", nID);
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusArcData>(L"ArcData", m_recDataCached.ArcData));
	auto pPenRec = GetLinkedRecord(LinkedObjTypePen);
	if (pPenRec)
	{
		auto pPenProp = pPenRec->GetProperties(ctxt);
		auto pPenNode = m_propsCached->AddBranch(L"Pen");
		pPenNode->sub = pPenProp->sub;
	}
}

static inline LPCWSTR EMFPlusCombineModeText(OCombineMode type)
{
	static const LPCWSTR aText[] = {
		L"Replace", L"Intersect", L"Union", L"XOR", L"Exclude", L"Complement"
	};
	return aText[(int)type];
}

void EMFRecAccessGDIPlusRecSetClipRect::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusClippingCat::CacheProperties(ctxt);
	auto pRec = (OEmfPlusRecSetClipRect*)m_recInfo.Data;
	auto mode = OEmfPlusRecSetClipRect::GetCombineMode(m_recInfo.Flags);
	m_propsCached->AddText(L"CombineMode", EMFPlusCombineModeText(mode));
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusRectF>(L"ClipRect", pRec->ClipRect));
}

void EMFRecAccessGDIPlusRecSetClipPath::Preprocess(EMFAccess* pEMF)
{
	auto nID = OEmfPlusRecSetClipPath::GetObjectID(m_recInfo.Flags);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
		AddLinkRecord(pRec, LinkedObjTypePath);
}

void EMFRecAccessGDIPlusRecSetClipPath::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusClippingCat::CacheProperties(ctxt);
	auto pRec = (OEmfPlusRecSetClipPath*)m_recInfo.Data;
	auto mode = OEmfPlusRecSetClipPath::GetCombineMode(m_recInfo.Flags);
	m_propsCached->AddText(L"CombineMode", EMFPlusCombineModeText(mode));
	auto pPath = GetLinkedRecord(LinkedObjTypePath);
	if (pPath)
	{
		auto pProp = pPath->GetProperties(ctxt);
		auto pNode = m_propsCached->AddBranch(L"Path");
		pNode->sub = pProp->sub;
	}
}

void EMFRecAccessGDIPlusRecSetClipRegion::Preprocess(EMFAccess* pEMF)
{
	auto nID = OEmfPlusRecSetClipRegion::GetObjectID(m_recInfo.Flags);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
		AddLinkRecord(pRec, LinkedObjTypeRegion);
}

void EMFRecAccessGDIPlusRecSetClipRegion::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusClippingCat::CacheProperties(ctxt);
	auto pRec = (OEmfPlusRecSetClipRegion*)m_recInfo.Data;
	auto mode = OEmfPlusRecSetClipRegion::GetCombineMode(m_recInfo.Flags);
	m_propsCached->AddText(L"CombineMode", EMFPlusCombineModeText(mode));
	auto pPath = GetLinkedRecord(LinkedObjTypeRegion);
	if (pPath)
	{
		auto pProp = pPath->GetProperties(ctxt);
		auto pNode = m_propsCached->AddBranch(L"Region");
		pNode->sub = pProp->sub;
	}
}

void EMFRecAccessGDIPlusRecOffsetClip::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusClippingCat::CacheProperties(ctxt);
	auto* pRec = (OEmfPlusRecOffsetClip*)m_recInfo.Data;
	m_propsCached->AddValue(L"dx", pRec->dx);
	m_propsCached->AddValue(L"dy", pRec->dy);
}

void EMFRecAccessGDIPlusRecFillRegion::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecFillRegion::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
	{
		AddLinkRecord(pRec, LinkedObjTypeRegion);
		if (!(m_recInfo.Flags & OEmfPlusRecFillRegion::FlagS))
		{
			auto pFillRec = (const OEmfPlusRecFillRegion*)m_recInfo.Data;
			auto pRecBrush = pEMF->GetObjectCreationRecord(pFillRec->BrushId, true);
			if (pRecBrush)
				AddLinkRecord(pRecBrush, LinkedObjTypeBrush);
		}
	}
}

void EMFRecAccessGDIPlusRecFillRegion::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(ctxt);
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecFillRegion::FlagObjectIDMask);
	m_propsCached->AddValue(L"RegionID", nID);
	auto pFillRec = (const OEmfPlusRecFillRegion*)m_recInfo.Data;
	if (m_recInfo.Flags & OEmfPlusRecFillPie::FlagS)
	{
		m_propsCached->sub.emplace_back(std::make_shared<PropertyNodeColor>(L"Brush", (emfplus::OEmfPlusARGB&)pFillRec->BrushId));
	}
	else
	{
		m_propsCached->AddValue(L"BrushID", pFillRec->BrushId);
		auto pBrushRec = GetLinkedRecord(LinkedObjTypeBrush);
		if (pBrushRec)
		{
			auto pBrushProp = pBrushRec->GetProperties(ctxt);
			auto pBrushNode = m_propsCached->AddBranch(L"Brush");
			pBrushNode->sub = pBrushProp->sub;
		}
	}
	auto pRgnRec = GetLinkedRecord(LinkedObjTypeRegion);
	if (pRgnRec)
	{
		auto pRgnProp = pRgnRec->GetProperties(ctxt);
		auto pRgnNode = m_propsCached->AddBranch(L"Region");
		pRgnNode->sub = pRgnProp->sub;
	}
}

void EMFRecAccessGDIPlusRecFillPath::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecFillPath::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
	{
		AddLinkRecord(pRec, LinkedObjTypePath);
		if (!(m_recInfo.Flags & OEmfPlusRecFillPath::FlagS))
		{
			auto pFillRec = (const OEmfPlusRecFillPath*)m_recInfo.Data;
			auto pRecBrush = pEMF->GetObjectCreationRecord(pFillRec->BrushId, true);
			if (pRecBrush)
				AddLinkRecord(pRecBrush, LinkedObjTypeBrush);
		}
	}
}

void EMFRecAccessGDIPlusRecFillPath::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(ctxt);
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecFillPath::FlagObjectIDMask);
	m_propsCached->AddValue(L"PathID", nID);
	auto pFillRec = (const OEmfPlusRecFillPath*)m_recInfo.Data;
	if (m_recInfo.Flags & OEmfPlusRecFillPath::FlagS)
	{
		m_propsCached->sub.emplace_back(std::make_shared<PropertyNodeColor>(L"Brush", (emfplus::OEmfPlusARGB&)pFillRec->BrushId));
	}
	else
	{
		m_propsCached->AddValue(L"BrushID", pFillRec->BrushId);
		auto pBrushRec = GetLinkedRecord(LinkedObjTypeBrush);
		if (pBrushRec)
		{
			auto pBrushProp = pBrushRec->GetProperties(ctxt);
			auto pBrushNode = m_propsCached->AddBranch(L"Brush");
			pBrushNode->sub = pBrushProp->sub;
		}
	}
	auto pRgnRec = GetLinkedRecord(LinkedObjTypePath);
	if (pRgnRec)
	{
		auto pRgnProp = pRgnRec->GetProperties(ctxt);
		auto pRgnNode = m_propsCached->AddBranch(L"Path");
		pRgnNode->sub = pRgnProp->sub;
	}
}

void EMFRecAccessGDIPlusRecDrawPath::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawPath::FlagObjectIDMask);
	auto pPathRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pPathRec)
	{
		AddLinkRecord(pPathRec, LinkedObjTypePath);
		auto pRec = (const OEmfPlusRecDrawPath*)m_recInfo.Data;
		auto pPen = pEMF->GetObjectCreationRecord(pRec->PenId, true);
		if (pPen)
			AddLinkRecord(pPen, LinkedObjTypePen);
	}
}

void EMFRecAccessGDIPlusRecDrawPath::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(ctxt);
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawPath::FlagObjectIDMask);
	m_propsCached->AddValue(L"PathID", nID);
	auto pPathRec = GetLinkedRecord(LinkedObjTypePath);
	if (pPathRec)
	{
		auto pProp = pPathRec->GetProperties(ctxt);
		auto pNode = m_propsCached->AddBranch(L"Path");
		pNode->sub = pProp->sub;
	}
	auto pRec = (const OEmfPlusRecDrawPath*)m_recInfo.Data;
	m_propsCached->AddValue(L"PenID", pRec->PenId);
	auto pPenRec = GetLinkedRecord(LinkedObjTypePen);
	if (pPenRec)
	{
		auto pPenProp = pPenRec->GetProperties(ctxt);
		auto pPenNode = m_propsCached->AddBranch(L"Pen");
		pPenNode->sub = pPenProp->sub;
	}
}

void EMFRecAccessGDIPlusRecFillClosedCurve::Preprocess(EMFAccess* pEMF)
{
	if (!(m_recInfo.Flags & OEmfPlusRecFillClosedCurve::FlagS))
	{
		auto nID = ((OEmfPlusRecFillClosedCurve*)m_recInfo.Data)->BrushId;
		auto pRec = pEMF->GetObjectCreationRecord(nID, true);
		if (pRec)
			AddLinkRecord(pRec, LinkedObjTypeBrush);
	}
}

static inline LPCWSTR EMFPlusWindingModeText(bool bWinding)
{
	return bWinding ? L"Winding" : L"Alternate";
}

void EMFRecAccessGDIPlusRecFillClosedCurve::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(ctxt);
	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));
	m_propsCached->AddText(L"FillMode", EMFPlusWindingModeText(m_recInfo.Flags & OEmfPlusRecFillClosedCurve::FlagW));
	auto nID = ((OEmfPlusRecFillClosedCurve*)m_recInfo.Data)->BrushId;
	m_propsCached->AddValue(L"BrushID", nID);
	if (m_recInfo.Flags & OEmfPlusRecFillPath::FlagS)
	{
		m_propsCached->sub.emplace_back(std::make_shared<PropertyNodeColor>(L"Brush", (emfplus::OEmfPlusARGB&)m_recDataCached.BrushId));
	}
	else
	{
		auto pBrushRec = GetLinkedRecord(LinkedObjTypeBrush);
		if (pBrushRec)
		{
			auto pBrushProp = pBrushRec->GetProperties(ctxt);
			auto pBrushNode = m_propsCached->AddBranch(L"Brush");
			pBrushNode->sub = pBrushProp->sub;
		}
	}
	m_propsCached->AddValue(L"Tension", m_recDataCached.Tension);
	bool bRelative = (m_recInfo.Flags & OEmfPlusRecFillClosedCurve::FlagP);
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusPointDataArray>(L"PointData", m_recDataCached.PointData, bRelative));
}

void EMFRecAccessGDIPlusRecDrawClosedCurve::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawClosedCurve::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
		AddLinkRecord(pRec, LinkedObjTypePen);
}

void EMFRecAccessGDIPlusRecDrawClosedCurve::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(ctxt);
	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawClosedCurve::FlagObjectIDMask);
	m_propsCached->AddValue(L"PenID", nID);
	auto pRec = GetLinkedRecord(LinkedObjTypePen);
	if (pRec)
	{
		auto pProp = pRec->GetProperties(ctxt);
		auto pNode = m_propsCached->AddBranch(L"Pen");
		pNode->sub = pProp->sub;
	}
	m_propsCached->AddValue(L"Tension", m_recDataCached.Tension);
	bool bRelative = (m_recInfo.Flags & OEmfPlusRecDrawClosedCurve::FlagP);
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusPointDataArray>(L"PointData", m_recDataCached.PointData, bRelative));
}

void EMFRecAccessGDIPlusRecDrawCurve::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawCurve::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
		AddLinkRecord(pRec, LinkedObjTypePen);
}

void EMFRecAccessGDIPlusRecDrawCurve::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(ctxt);
	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawCurve::FlagObjectIDMask);
	m_propsCached->AddValue(L"PenID", nID);
	auto pRec = GetLinkedRecord(LinkedObjTypePen);
	if (pRec)
	{
		auto pProp = pRec->GetProperties(ctxt);
		auto pNode = m_propsCached->AddBranch(L"Pen");
		pNode->sub = pProp->sub;
	}
	m_propsCached->AddValue(L"Tension", m_recDataCached.Tension);
	m_propsCached->AddValue(L"Offset", m_recDataCached.Offset);
	m_propsCached->AddValue(L"NumSegments", m_recDataCached.NumSegments);
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusPointDataArray>(L"PointData", m_recDataCached.PointData));
}

void EMFRecAccessGDIPlusRecDrawBeziers::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawBeziers::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
		AddLinkRecord(pRec, LinkedObjTypePen);
}

void EMFRecAccessGDIPlusRecDrawBeziers::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(ctxt);
	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawBeziers::FlagObjectIDMask);
	m_propsCached->AddValue(L"PenID", nID);
	auto pRec = GetLinkedRecord(LinkedObjTypePen);
	if (pRec)
	{
		auto pProp = pRec->GetProperties(ctxt);
		auto pNode = m_propsCached->AddBranch(L"Pen");
		pNode->sub = pProp->sub;
	}
	bool bRelative = (m_recInfo.Flags & OEmfPlusRecDrawBeziers::FlagP);
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusPointDataArray>(L"PointData", m_recDataCached.PointData, bRelative));
}

bool EMFRecAccessGDIPlusRecDrawImage::DrawPreview(PreviewContext* info)
{
	auto pRec = GetLinkedRecord(LinkedObjTypeImage);
	if (!pRec)
		return false;
	return pRec->DrawPreview(info);
}

void EMFRecAccessGDIPlusRecDrawImage::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawImage::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
	{
		AddLinkRecord(pRec, LinkedObjTypeImage);
		auto pImgRec = (const OEmfPlusRecDrawImage*)m_recInfo.Data;
		if (pImgRec->ImageAttributesID != (u32t)InvalidObjectID)
		{
			pRec = pEMF->GetObjectCreationRecord(pImgRec->ImageAttributesID, true);
			if (pRec)
				AddLinkRecord(pRec, LinkedObjTypeImageAttributes);
		}
	}
}

void EMFRecAccessGDIPlusRecDrawImage::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(ctxt);

	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));

	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawImage::FlagObjectIDMask);
	m_propsCached->AddValue(L"ImageID", nID);
	m_propsCached->AddValue(L"ImageAttributesID", m_recDataCached.ImageAttributesID);
	m_propsCached->AddText(L"SrcUnit", EMFPlusUnitTypeText(m_recDataCached.SrcUnit));
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusRectF>(L"SrcRect", m_recDataCached.SrcRect));
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusRectData>(L"RectData", m_recDataCached.RectData));

	auto pImg = GetLinkedRecord(LinkedObjTypeImage);
	if (pImg)
	{
		auto pImgProp = pImg->GetProperties(ctxt);
		auto pImgNode = m_propsCached->AddBranch(L"Image");
		pImgNode->sub = pImgProp->sub;

		auto pAttrRec = GetLinkedRecord(LinkedObjTypeImageAttributes);
		if (pAttrRec)
		{
			auto pAttrProp = pAttrRec->GetProperties(ctxt);
			auto pAttrNode = m_propsCached->AddBranch(L"ImageAttribute");
			pAttrNode->sub = pAttrProp->sub;
		}
	}
}

bool EMFRecAccessGDIPlusRecDrawImagePoints::DrawPreview(PreviewContext* info)
{
	auto pRec = GetLinkedRecord(LinkedObjTypeImage);
	if (!pRec)
		return false;
	return pRec->DrawPreview(info);
}

void EMFRecAccessGDIPlusRecDrawImagePoints::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawImagePoints::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
	{
		AddLinkRecord(pRec, LinkedObjTypeImage);
		auto pImgRec = (const OEmfPlusRecDrawImage*)m_recInfo.Data;
		if (pImgRec->ImageAttributesID != (u32t)InvalidObjectID)
		{
			pRec = pEMF->GetObjectCreationRecord(pImgRec->ImageAttributesID, true);
			if (pRec)
				AddLinkRecord(pRec, LinkedObjTypeImageAttributes);
		}
	}
}

void EMFRecAccessGDIPlusRecDrawImagePoints::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(ctxt);

	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));

	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawImagePoints::FlagObjectIDMask);
	m_propsCached->AddValue(L"ImageID", nID);
	m_propsCached->AddValue(L"ImageAttributesID", m_recDataCached.ImageAttributesID);
	m_propsCached->AddText(L"SrcUnit", EMFPlusUnitTypeText(m_recDataCached.SrcUnit));
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusRectF>(L"SrcRect", m_recDataCached.SrcRect));
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusPointDataArray>(L"PointData", m_recDataCached.PointData));

	auto pImg = GetLinkedRecord(LinkedObjTypeImage);
	if (pImg)
	{
		auto pImgProp = pImg->GetProperties(ctxt);
		auto pImgNode = m_propsCached->AddBranch(L"Image");
		pImgNode->sub = pImgProp->sub;

		auto pAttrRec = GetLinkedRecord(LinkedObjTypeImageAttributes);
		if (pAttrRec)
		{
			auto pAttrProp = pAttrRec->GetProperties(ctxt);
			auto pAttrNode = m_propsCached->AddBranch(L"ImageAttribute");
			pAttrNode->sub = pAttrProp->sub;
		}
	}
}

void EMFRecAccessGDIPlusRecDrawString::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawString::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
	{
		AddLinkRecord(pRec, LinkedObjTypeFont);
		auto pDrawRec = (OEmfPlusRecDrawString*)m_recInfo.Data;
		if (!(m_recInfo.Flags & OEmfPlusRecDrawString::FlagS))
		{
			auto nID = pDrawRec->BrushId;
			pRec = pEMF->GetObjectCreationRecord(nID, true);
			if (pRec)
				AddLinkRecord(pRec, LinkedObjTypeBrush);
		}
		if (pDrawRec->FormatID != (u32t)InvalidObjectID)
		{
			pRec = pEMF->GetObjectCreationRecord(pDrawRec->FormatID, true);
			if (pRec)
				AddLinkRecord(pRec, LinkedObjTypeStringFormat);
		}
	}
}

void EMFRecAccessGDIPlusRecDrawString::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(ctxt);

	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));

	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawString::FlagObjectIDMask);
	m_propsCached->AddValue(L"FontID", nID);
	auto pFont = GetLinkedRecord(LinkedObjTypeFont);
	if (pFont)
	{
		auto pProp = pFont->GetProperties(ctxt);
		auto pNode = m_propsCached->AddBranch(EMFPlusLinkedObjText(LinkedObjTypeFont));
		pNode->sub = pProp->sub;
	}
	if (m_recInfo.Flags & OEmfPlusRecDrawString::FlagS)
	{
		m_propsCached->sub.emplace_back(std::make_shared<PropertyNodeColor>(L"Brush", (emfplus::OEmfPlusARGB&)m_recDataCached.BrushId));
	}
	else
	{
		auto pBrush = GetLinkedRecord(LinkedObjTypeBrush);
		if (pBrush)
		{
			auto pProp = pBrush->GetProperties(ctxt);
			auto pNode = m_propsCached->AddBranch(EMFPlusLinkedObjText(LinkedObjTypeBrush));
			pNode->sub = pProp->sub;
		}
	}
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusRectF>(L"LayoutRect", m_recDataCached.LayoutRect));
	m_propsCached->AddText(L"StringData", m_recDataCached.StringData.data());

	if (m_recDataCached.FormatID != (u32t)InvalidObjectID)
	{
		auto pFormat = GetLinkedRecord(LinkedObjTypeStringFormat);
		if (pFormat)
		{
			auto pProp = pFormat->GetProperties(ctxt);
			auto pNode = m_propsCached->AddBranch(EMFPlusLinkedObjText(LinkedObjTypeStringFormat));
			pNode->sub = pProp->sub;
		}
	}
}

void EMFRecAccessGDIPlusRecDrawDriverString::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawDriverString::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
	{
		AddLinkRecord(pRec, LinkedObjTypeFont);
		if (!(m_recInfo.Flags & OEmfPlusRecDrawDriverString::FlagS))
		{
			auto nID = ((OEmfPlusRecDrawDriverString*)m_recInfo.Data)->BrushId;
			pRec = pEMF->GetObjectCreationRecord(nID, true);
			if (pRec)
				AddLinkRecord(pRec, LinkedObjTypeBrush);
		}
	}
}

void EMFRecAccessGDIPlusRecDrawDriverString::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(ctxt);

	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));

	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawDriverString::FlagObjectIDMask);
	m_propsCached->AddValue(L"FontID", nID);
	auto pFont = GetLinkedRecord(LinkedObjTypeFont);
	if (pFont)
	{
		auto pProp = pFont->GetProperties(ctxt);
		auto pNode = m_propsCached->AddBranch(EMFPlusLinkedObjText(LinkedObjTypeFont));
		pNode->sub = pProp->sub;
	}
	if (m_recInfo.Flags & OEmfPlusRecDrawString::FlagS)
	{
		m_propsCached->sub.emplace_back(std::make_shared<PropertyNodeColor>(L"Brush", (emfplus::OEmfPlusARGB&)m_recDataCached.BrushId));
	}
	else
	{
		auto pBrush = GetLinkedRecord(LinkedObjTypeBrush);
		if (pBrush)
		{
			auto pProp = pBrush->GetProperties(ctxt);
			auto pNode = m_propsCached->AddBranch(EMFPlusLinkedObjText(LinkedObjTypeBrush));
			pNode->sub = pProp->sub;
		}
	}
	m_propsCached->AddValue(L"DriverStringOptionsFlags", m_recDataCached.DriverStringOptionsFlags, true);
	m_propsCached->AddValue(L"MatrixPresent", m_recDataCached.MatrixPresent);
	// TODO, Glyphs, GlyphPos
	if (!m_recDataCached.Glyphs.empty())
	{
		if ((u32t)ODriverStringOptions::CmapLookup & m_recDataCached.DriverStringOptionsFlags)
		{
			m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusUnicodeArrayWrapper>(L"Glyphs", m_recDataCached.Glyphs));
		}
		else
		{

		}
	}
	if (!m_recDataCached.GlyphPos.empty())
	{
		m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusPointFArrayWrapper>(L"GlyphPos", m_recDataCached.GlyphPos));
	}
	if (m_recDataCached.MatrixPresent)
	{
		m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusTransform>(L"TransformMatrix", m_recDataCached.TransformMatrix));
	}
}

void EMFRecAccessGDIPlusRecSetRenderingOrigin::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusPropertyCat::CacheProperties(ctxt);
	auto pRec = (OEmfPlusRecSetRenderingOrigin*)m_recInfo.Data;
	m_propsCached->AddValue(L"x", pRec->x);
	m_propsCached->AddValue(L"y", pRec->y);
}

static inline LPCWSTR EMFPlusSmoothingModeText(OSmoothingMode val)
{
	static const LPCWSTR aText[] = {
		L"Default", L"HighSpeed", L"HighQuality", L"None", L"AntiAlias8x4", L"AntiAlias8x8"
	};
	return aText[(int)val];
}

void EMFRecAccessGDIPlusRecSetAntiAliasMode::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusPropertyCat::CacheProperties(ctxt);
	auto val = OEmfPlusRecSetAntiAliasMode::GetSmoothingMode(m_recInfo.Flags);
	m_propsCached->AddText(L"SmoothingMode", EMFPlusSmoothingModeText(val));
	m_propsCached->AddValue(L"Anti-aliasing", m_recInfo.Flags & OEmfPlusRecSetAntiAliasMode::FlagA);
}

static inline LPCWSTR EMFPlusTextRenderingHintText(OTextRenderingHint val)
{
	static const LPCWSTR aText[] = {
		L"SystemDefault", L"SingleBitPerPixelGridFit", L"SingleBitPerPixel",
		L"AntialiasGridFit", L"Antialias", L"ClearTypeGridFit"
	};
	return aText[(int)val];
}

void EMFRecAccessGDIPlusRecSetTextRenderingHint::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusPropertyCat::CacheProperties(ctxt);
	auto val = OEmfPlusRecSetTextRenderingHint::GetTextRenderingHint(m_recInfo.Flags);
	m_propsCached->AddText(L"TextRenderingHint", EMFPlusTextRenderingHintText(val));
}

void EMFRecAccessGDIPlusRecSetTextContrast::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusPropertyCat::CacheProperties(ctxt);
	auto val = OEmfPlusRecSetTextContrast::GetTextContrast(m_recInfo.Flags);
	m_propsCached->AddValue(L"TextContrast", val);
}

static inline LPCWSTR EMFPlusInterpolationModeText(OInterpolationMode val)
{
	static const LPCWSTR aText[] = {
		L"Default", L"LowQuality", L"HighQuality", L"Bilinear",
		L"Bicubic", L"NearestNeighbor", L"HighQualityBilinear", L"HighQualityBicubic",
	};
	return aText[(int)val];
}

void EMFRecAccessGDIPlusRecSetInterpolationMode::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusPropertyCat::CacheProperties(ctxt);
	auto val = OEmfPlusRecSetInterpolationMode::GetInterpolationMode(m_recInfo.Flags);
	m_propsCached->AddText(L"InterpolationMode", EMFPlusInterpolationModeText(val));
}

static inline LPCWSTR EMFPlusPixelOffsetModeText(OPixelOffsetMode val)
{
	static const LPCWSTR aText[] = {
		L"Default", L"HighSpeed", L"HighQuality", L"None", L"Half"
	};
	return aText[(int)val];
}

void EMFRecAccessGDIPlusRecSetPixelOffsetMode::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusPropertyCat::CacheProperties(ctxt);
	auto val = OEmfPlusRecSetPixelOffsetMode::GetPixelOffsetMode(m_recInfo.Flags);
	m_propsCached->AddText(L"PixelOffsetMode", EMFPlusPixelOffsetModeText(val));
}

static inline LPCWSTR EMFPlusCompositingModeText(OCompositingMode val)
{
	static const LPCWSTR aText[] = {
		L"SourceOver", L"SourceCopy"
	};
	return aText[(int)val];
}

void EMFRecAccessGDIPlusRecSetCompositingMode::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusPropertyCat::CacheProperties(ctxt);
	auto val = OEmfPlusRecSetCompositingMode::GetCompositingMode(m_recInfo.Flags);
	m_propsCached->AddText(L"CompositingMode", EMFPlusCompositingModeText(val));
}

static inline LPCWSTR EMFPlusCompositingQualityText(OCompositingQuality val)
{
	static const LPCWSTR aText[] = {
		L"Invalid", L"Default", L"HighSpeed", L"HighQuality", L"GammaCorrected", L"AssumeLinear"
	};
	return aText[(int)val];
}

void EMFRecAccessGDIPlusRecSetCompositingQuality::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusPropertyCat::CacheProperties(ctxt);
	auto val = OEmfPlusRecSetCompositingQuality::GetCompositingQuality(m_recInfo.Flags);
	m_propsCached->AddText(L"CompositingQuality", EMFPlusCompositingQualityText(val));
}

void EMFRecAccessGDIPlusRecSave::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusStateCat::CacheProperties(ctxt);
	auto pRec = (OEmfPlusRecSave*)m_recInfo.Data;
	m_propsCached->AddValue(L"StackIndex", pRec->StackIndex);
}

void EMFRecAccessGDIPlusRecRestore::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (OEmfPlusRecRestore*)m_recInfo.Data;
	auto pSaveRec = pEMF->GetPlusSaveRecord(pRec->StackIndex);
	if (pSaveRec)
	{
		AddLinkRecord(pSaveRec, LinkedObjTypeGraphicState, LinkedObjTypeGraphicState);
	}
}

void EMFRecAccessGDIPlusRecRestore::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusStateCat::CacheProperties(ctxt);
	auto pRec = (OEmfPlusRecRestore*)m_recInfo.Data;
	m_propsCached->AddValue(L"StackIndex", pRec->StackIndex);
}

void EMFRecAccessGDIPlusRecBeginContainer::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusStateCat::CacheProperties(ctxt);
	auto pRec = (OEmfPlusRecBeginContainer*)m_recInfo.Data;
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusRectF>(L"DestRect", pRec->DestRect));
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusRectF>(L"SrcRect", pRec->SrcRect));
	m_propsCached->AddValue(L"StackIndex", pRec->StackIndex);
}

void EMFRecAccessGDIPlusRecBeginContainerNoParams::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusStateCat::CacheProperties(ctxt);
	auto pRec = (OEmfPlusRecBeginContainerNoParams*)m_recInfo.Data;
	m_propsCached->AddValue(L"StackIndex", pRec->StackIndex);
}

void EMFRecAccessGDIPlusRecEndContainer::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (OEmfPlusRecEndContainer*)m_recInfo.Data;
	auto pSaveRec = pEMF->GetPlusSaveRecord(pRec->StackIndex);
	if (pSaveRec)
	{
		AddLinkRecord(pSaveRec, LinkedObjTypeGraphicState, LinkedObjTypeGraphicState);
	}
}

void EMFRecAccessGDIPlusRecEndContainer::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusStateCat::CacheProperties(ctxt);
	auto pRec = (OEmfPlusRecEndContainer*)m_recInfo.Data;
	m_propsCached->AddValue(L"StackIndex", pRec->StackIndex);
}


void EMFRecAccessGDIPlusRecSetWorldTransform::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusTransformCat::CacheProperties(ctxt);
	auto* pRec = (OEmfPlusRecSetWorldTransform*)m_recInfo.Data;
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusTransform>(L"MatrixData", pRec->MatrixData));
}

void EMFRecAccessGDIPlusRecMultiplyWorldTransform::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusTransformCat::CacheProperties(ctxt);
	auto* pRec = (OEmfPlusRecMultiplyWorldTransform*)m_recInfo.Data;
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusTransform>(L"MatrixData", pRec->MatrixData));
}

void EMFRecAccessGDIPlusRecTranslateWorldTransform::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusTransformCat::CacheProperties(ctxt);
	auto* pRec = (OEmfPlusRecTranslateWorldTransform*)m_recInfo.Data;
	m_propsCached->AddValue(L"dx", pRec->dx);
	m_propsCached->AddValue(L"dy", pRec->dy);
}

void EMFRecAccessGDIPlusRecScaleWorldTransform::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusTransformCat::CacheProperties(ctxt);
	auto* pRec = (OEmfPlusRecScaleWorldTransform*)m_recInfo.Data;
	m_propsCached->AddValue(L"Sx", pRec->Sx);
	m_propsCached->AddValue(L"Sy", pRec->Sy);
}

void EMFRecAccessGDIPlusRecRotateWorldTransform::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusTransformCat::CacheProperties(ctxt);
	auto* pRec = (OEmfPlusRecRotateWorldTransform*)m_recInfo.Data;
	m_propsCached->AddValue(L"Angle", pRec->Angle);
}

void EMFRecAccessGDIPlusRecSetPageTransform::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIPlusTransformCat::CacheProperties(ctxt);
	auto* pRec = (OEmfPlusRecSetPageTransform*)m_recInfo.Data;
	m_propsCached->AddValue(L"PageScale", pRec->PageScale);
}
