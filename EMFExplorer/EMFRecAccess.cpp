#include "pch.h"
#include "framework.h"
#include "EMFRecAccess.h"

#include <atlbase.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

using namespace emfplus;

CRect GetFitRect(const CRect& rcDest, const SIZE& szSrc, bool bCenter)
{
	CRect rcFit = rcDest;
	CSize szDest = rcDest.Size();
	auto fScaleDst = (float)szDest.cy / szDest.cx;
	auto fScaleSrc = (float)szSrc.cy / szSrc.cx;
	if (fScaleSrc <= fScaleDst)
	{
		// source image is flatter than the target rectangle, so we fit the width
		szDest.cy	= (int)(szSrc.cy * (float)szDest.cx / szSrc.cx);
		rcFit.bottom = rcFit.top + szDest.cy;
		if (bCenter)
			rcFit.OffsetRect(0, (rcDest.Height() - rcFit.Height()) / 2);
	}
	else
	{
		szDest.cx	= (int)(szSrc.cx * (float)szDest.cy / szSrc.cy);
		rcFit.right = rcFit.left + szDest.cx;
		if (bCenter)
			rcFit.OffsetRect((rcDest.Width() - rcFit.Width()) / 2, 0);
	}
	return rcFit;
}

bool EMFRecAccess::IsDrawingRecord() const
{
	auto cate = GetRecordCategory();
	return cate == RecCategoryDrawing || cate == RecCategoryBitmap;
}

std::shared_ptr<PropertyNode> EMFRecAccess::GetProperties(EMFAccess* pEMF)
{
	if (!m_propsCached)
	{
		m_propsCached = std::make_shared<PropertyNode>();
		CacheProperties(pEMF);
	}
	return m_propsCached;
}

bool EMFRecAccess::IsLinked(const EMFRecAccess* pRec) const
{
	for (auto link : m_linkRecs)
	{
		if (link == pRec)
			return true;
	}
	return false;
}

void EMFRecAccess::CacheProperties(EMFAccess* pEMF)
{
	m_propsCached->AddText(L"Name", GetRecordName());
	m_propsCached->AddValue(L"RecordType", m_recInfo.Type);
	m_propsCached->AddValue(L"DataSize", m_recInfo.DataSize);
}

void EMFRecAccess::AddLinkRecord(EMFRecAccess* pRecAccess, bool bMutually)
{
	m_linkRecs.push_back(pRecAccess);
	if (bMutually)
		pRecAccess->AddLinkRecord(this, false);
}

void EMFRecAccessGDIRec::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccess::CacheProperties(pEMF);
	ENHMETARECORD recTemp{ 0 };
	const ENHMETARECORD* pRec = nullptr;
	if (m_recInfo.Data)
	{
		// I can't find any document for this, but it seems that this is the way to go (based on observation)
		pRec = (const ENHMETARECORD*)(m_recInfo.Data - sizeof(EMR));
	}
	else
	{
		recTemp.iType = m_recInfo.Type;
		recTemp.nSize = sizeof(EMR);
		pRec = &recTemp;
	}
	CachePropertiesFromGDI(pEMF, pRec);
}

void EMFRecAccessGDIRecHeader::CachePropertiesFromGDI(EMFAccess* pEMF, const ENHMETARECORD* pEMFRec)
{
	auto pRec = (const ENHMETAHEADER*)pEMFRec;
	ASSERT(pRec->dSignature == ENHMETA_SIGNATURE);
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodeRectInt>(L"rclBounds", pRec->rclBounds));
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodeRectInt>(L"rclFrame", pRec->rclFrame));
	CStringW str;
	str.Format(L"%08X", pRec->dSignature);
	m_propsCached->AddText(L"Signature", str);
	m_propsCached->AddValue(L"nVersion", pRec->nVersion);
	m_propsCached->AddValue(L"nBytes", pRec->nBytes);
	m_propsCached->AddValue(L"nRecords", pRec->nRecords);
	m_propsCached->AddValue(L"nHandles", pRec->nHandles);
	if (pRec->nDescription && pRec->offDescription)
	{
		m_propsCached->AddText(L"Description", (LPCWSTR)((const char*)pRec + pRec->offDescription));
	}
	m_propsCached->AddValue(L"nPalEntries", pRec->nPalEntries);
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodeSizeInt>(L"szlDevice", pRec->szlDevice));
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodeSizeInt>(L"szlMillimeters", pRec->szlMillimeters));
	if (pRec->cbPixelFormat && pRec->offPixelFormat)
	{
		// TODO, ENHMETAHEADER::offPixelFormat
	}
	m_propsCached->AddValue(L"bOpenGL", pRec->bOpenGL);
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodeSizeInt>(L"szlMicrometers", pRec->szlMicrometers));
}

void EMFRecAccessGDIRecGdiComment::CachePropertiesFromGDI(EMFAccess* pEMF, const ENHMETARECORD* pEMFRec)
{
	auto pRec = (const EMRCOMMENT_BASE*)pEMFRec;
	CStringW str;
	str.Format(L"%08X", pRec->CommentIdentifier);
	m_propsCached->AddText(L"Identifier", str);
	str = L"Unknown";
	CString strText;
	switch (pRec->CommentIdentifier)
	{
	case EMR_COMMENT_EMFPLUS:
		str = L"EMF+";
		break;
	case EMR_COMMENT_EMFSPOOL:
		str = L"EMF Spool";
		break;
	case EMR_COMMENT_PUBLIC:
		{
			auto pPublicRec = (EMRCOMMENT_PUBLIC*)pRec;
			switch (pPublicRec->PublicCommentIdentifier)
			{
			case EMR_COMMENT_BEGINGROUP:
				str = L"Public: Begin group";
				break;
			case EMR_COMMENT_ENDGROUP:
				str = L"Public: End group";
				break;
			case EMR_COMMENT_WINDOWS_METAFILE:
				str = L"Public: End Windows Metafile";
				break;
			case EMR_COMMENT_MULTIFORMATS:
				str = L"Public: Multiformats";
				break;
			case EMR_COMMENT_UNICODE_STRING:
				str = L"Public: Unicode String";
				break;
			case EMR_COMMENT_UNICODE_END:
				str = L"Public: Unicode End";
				break;
			}
		}
		break;
	default:
		if (pRec->DataSize)
		{
			char szBuffer[100]{0};
			strncpy_s(szBuffer, _countof(szBuffer), (char*)&pRec->CommentIdentifier, _countof(szBuffer));
			strText = szBuffer;
		}
		break;
	}
	m_propsCached->AddText(L"CommentType", str);
	if (!strText.IsEmpty())
		m_propsCached->AddText(L"Text", strText);
}

void EMFRecAccessGDIPlusRec::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccess::CacheProperties(pEMF);
	m_propsCached->AddValue(L"Flags", m_recInfo.Flags, PropertyNode::NodeTypeText, true);
}

void EMFRecAccessGDIPlusRecObject::Preprocess(EMFAccess* pEMF)
{
	auto nObjectID = (u8t)(m_recInfo.Flags & OEmfPlusRecObjectReader::FlagObjectIDMask);
	pEMF->SetObjectToTable(nObjectID, this, true);
}

class EMFRecAccessGDIPlusBrushWrapper : public EMFRecAccessGDIPlusObjWrapper
{
public:
	EMFRecAccessGDIPlusBrushWrapper()
	{
		m_obj.reset(new OEmfPlusBrush);
	}
protected:
	void CacheProperties(EMFAccess* pEMF, PropertyNode* pNode) const override
	{
		// TODO
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
	void CacheProperties(EMFAccess* pEMF, PropertyNode* pNode) const override
	{
		// TODO
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
	void CacheProperties(EMFAccess* pEMF, PropertyNode* pNode) const override
	{
		// TODO
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
	void CacheProperties(EMFAccess* pEMF, PropertyNode* pNode) const override
	{
		// TODO
	}
};

static inline LPCWSTR EMFPlusImageTypeText(OImageDataType type)
{
	static const LPCWSTR aText[] = {
		L"Unknown", L"Bitmap", L"Metafile"
	};
	return aText[(int)type];
}

static inline LPCWSTR EMFPlusBitmapTypeText(OBitmapDataType type)
{
	static const LPCWSTR aText[] = {
		L"Pixel", L"Compressed"
	};
	return aText[(int)type];
}

class EMFRecAccessGDIPlusImageWrapper : public EMFRecAccessGDIPlusObjWrapper
{
public:
	EMFRecAccessGDIPlusImageWrapper()
	{
		m_obj.reset(new OEmfPlusImage);
	}
protected:
	void CacheProperties(EMFAccess* pEMF, PropertyNode* pNode) const override
	{
		auto pImg = (OEmfPlusImage*)m_obj.get();
		pNode->AddText(L"ImageType", EMFPlusImageTypeText(pImg->Type));
		switch (pImg->Type)
		{
		case OImageDataType::Bitmap:
			pNode->AddValue(L"Width", pImg->ImageDataBmp.Width);
			pNode->AddValue(L"Height", pImg->ImageDataBmp.Height);
			pNode->AddValue(L"Stride", pImg->ImageDataBmp.Stride);
			pNode->AddValue(L"PixelFormat", (u32t)pImg->ImageDataBmp.PixelFormat, PropertyNode::NodeTypeText, true);
			pNode->AddText(L"BitmapType", EMFPlusBitmapTypeText(pImg->ImageDataBmp.Type));
			break;
		case OImageDataType::Metafile:
			// TODO
			break;
		}
	}
};

class EMFRecAccessGDIPlusFontWrapper : public EMFRecAccessGDIPlusObjWrapper
{
public:
	EMFRecAccessGDIPlusFontWrapper()
	{
		m_obj.reset(new OEmfPlusFont);
	}
protected:
	void CacheProperties(EMFAccess* pEMF, PropertyNode* pNode) const override
	{
		// TODO
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
	void CacheProperties(EMFAccess* pEMF, PropertyNode* pNode) const override
	{
		// TODO
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
	void CacheProperties(EMFAccess* pEMF, PropertyNode* pNode) const override
	{
		// TODO
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
	void CacheProperties(EMFAccess* pEMF, PropertyNode* pNode) const override
	{
		// TODO
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
		auto pObj = CreatePlusObjectAccessWrapper(nObjType);
		if (!pObj)
		{
			ASSERT(0);
			return nullptr;
		}
		m_recDataCached.reset(pObj);
	}
	return m_recDataCached.get();
}

void EMFRecAccessGDIPlusRecObject::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusObjectCat::CacheProperties(pEMF);
	auto nObjType = OEmfPlusRecObjectReader::GetObjectType(m_recInfo);
	m_propsCached->AddText(L"ObjectType", EMFPlusObjectTypeText(nObjType));
	auto nObjectID = (u8t)(m_recInfo.Flags & OEmfPlusRecObjectReader::FlagObjectIDMask);
	m_propsCached->AddValue(L"Index", nObjectID);
	auto pObjWrapper = GetObjectWrapper();
	if (pObjWrapper)
	{
		DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
		VERIFY(pObjWrapper->GetObject()->Read(reader, m_recInfo.DataSize));

		pObjWrapper->CacheProperties(pEMF, m_propsCached.get());
	}
}

void EMFRecAccessGDIPlusRecFillRects::Preprocess(EMFAccess* pEMF)
{
	if (!(m_recInfo.Flags & OEmfPlusRecFillRects::FlagS))
	{
		auto nID = ((OEmfPlusRecFillRects*)m_recInfo.Data)->BrushId;
		auto pRec = pEMF->GetObjectCreationRecord(nID, true);
		if (pRec)
			AddLinkRecord(pRec);
	}
}

void EMFRecAccessGDIPlusRecDrawRects::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawRects::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
	{
		AddLinkRecord(pRec);
	}
}

void EMFRecAccessGDIPlusRecFillPolygon::Preprocess(EMFAccess* pEMF)
{
	if (!(m_recInfo.Flags & OEmfPlusRecFillPolygon::FlagS))
	{
		DataReader reader(m_recInfo.Data, (size_t)m_recInfo.DataSize);
		auto nID = ((OEmfPlusRecFillPolygon*)m_recInfo.Data)->BrushId;
		auto pRec = pEMF->GetObjectCreationRecord(nID, true);
		if (pRec)
			AddLinkRecord(pRec);
	}
}

void EMFRecAccessGDIPlusRecDrawLines::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawLines::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
	{
		AddLinkRecord(pRec);
	}
}

void EMFRecAccessGDIPlusRecFillEllipse::Preprocess(EMFAccess* pEMF)
{
	if (!(m_recInfo.Flags & OEmfPlusRecFillEllipse::FlagS))
	{
		DataReader reader(m_recInfo.Data, (size_t)m_recInfo.DataSize);
		auto nID = ((OEmfPlusRecFillEllipse*)m_recInfo.Data)->BrushId;
		auto pRec = pEMF->GetObjectCreationRecord(nID, true);
		if (pRec)
			AddLinkRecord(pRec);
	}
}

void EMFRecAccessGDIPlusRecDrawEllipse::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawEllipse::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
	{
		AddLinkRecord(pRec);
	}
}

void EMFRecAccessGDIPlusRecFillPie::Preprocess(EMFAccess* pEMF)
{
	if (!(m_recInfo.Flags & OEmfPlusRecFillPie::FlagS))
	{
		DataReader reader(m_recInfo.Data, (size_t)m_recInfo.DataSize);
		auto nID = ((OEmfPlusRecFillPie*)m_recInfo.Data)->BrushId;
		auto pRec = pEMF->GetObjectCreationRecord(nID, true);
		if (pRec)
			AddLinkRecord(pRec);
	}
}

void EMFRecAccessGDIPlusRecDrawPie::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawPie::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
	{
		AddLinkRecord(pRec);
	}
}

void EMFRecAccessGDIPlusRecDrawArc::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawArc::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
	{
		AddLinkRecord(pRec);
	}
}

void EMFRecAccessGDIPlusRecSetClipPath::Preprocess(EMFAccess* pEMF)
{
	auto nID = OEmfPlusRecSetClipPath::GetObjectID(m_recInfo.Flags);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
		AddLinkRecord(pRec);
}

void EMFRecAccessGDIPlusRecSetClipRegion::Preprocess(EMFAccess* pEMF)
{
	auto nID = OEmfPlusRecSetClipRegion::GetObjectID(m_recInfo.Flags);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
		AddLinkRecord(pRec);
}

void EMFRecAccessGDIPlusRecFillRegion::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecFillRegion::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
	{
		AddLinkRecord(pRec);
		if (!(m_recInfo.Flags & OEmfPlusRecFillRegion::FlagS))
		{
			auto pFillRec = (const OEmfPlusRecFillRegion*)m_recInfo.Data;
			auto pRecBrush = pEMF->GetObjectCreationRecord(pFillRec->BrushId, true);
			if (pRecBrush)
				AddLinkRecord(pRecBrush);
		}
	}
}

void EMFRecAccessGDIPlusRecFillPath::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecFillPath::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
	{
		AddLinkRecord(pRec);
		if (!(m_recInfo.Flags & OEmfPlusRecFillPath::FlagS))
		{
			auto pFillRec = (const OEmfPlusRecFillPath*)m_recInfo.Data;
			auto pRecBrush = pEMF->GetObjectCreationRecord(pFillRec->BrushId, true);
			if (pRecBrush)
				AddLinkRecord(pRecBrush);
		}
	}
}

void EMFRecAccessGDIPlusRecDrawPath::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawPath::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
		AddLinkRecord(pRec);
}

void EMFRecAccessGDIPlusRecDrawBeziers::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawBeziers::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
		AddLinkRecord(pRec);
}

void EMFRecAccessGDIPlusRecDrawImage::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawImage::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
		AddLinkRecord(pRec);
}

void EMFRecAccessGDIPlusRecDrawImagePoints::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawImagePoints::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
		AddLinkRecord(pRec);
}

static inline LPCWSTR EMFPlusUnitTypeText(OUnitType type)
{
	static const LPCWSTR aText[] = {
		L"World", L"Display", L"Pixel", L"Point", L"Inch", L"Document", L"Millimeter"
	};
	return aText[(int)type];
}

void EMFRecAccessGDIPlusRecDrawImagePoints::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(pEMF);

	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));

	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawImagePoints::FlagObjectIDMask);
	m_propsCached->AddValue(L"ImageObjectID", nID);
	m_propsCached->AddValue(L"ImageAttributesID", m_recDataCached.ImageAttributesID);
	m_propsCached->AddText(L"SrcUnit", EMFPlusUnitTypeText(m_recDataCached.SrcUnit));
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodeRectFloat>(L"SrcRect", m_recDataCached.SrcRect));
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePointDataArray>(L"PointData", m_recDataCached.PointData));

	if (!m_linkRecs.empty())
	{
		auto pImg = m_linkRecs.front();
		auto pImgProp = pImg->GetProperties(pEMF);
		auto pImgNode = m_propsCached->AddText(L"Image", nullptr, PropertyNode::NodeTypeBranch);
		pImgNode->sub = pImgProp->sub;
	}
}

void EMFRecAccessGDIPlusRecDrawString::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawString::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
		AddLinkRecord(pRec);
	if (!(m_recInfo.Flags & OEmfPlusRecDrawString::FlagS))
	{
		DataReader reader(m_recInfo.Data, (size_t)m_recInfo.DataSize);
		auto nID = ((OEmfPlusRecDrawString*)m_recInfo.Data)->BrushId;
		pRec = pEMF->GetObjectCreationRecord(nID, true);
		if (pRec)
			AddLinkRecord(pRec);
	}
}

void EMFRecAccessGDIPlusRecDrawDriverString::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawDriverString::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
		AddLinkRecord(pRec);
	if (!(m_recInfo.Flags & OEmfPlusRecDrawDriverString::FlagS))
	{
		DataReader reader(m_recInfo.Data, (size_t)m_recInfo.DataSize);
		auto nID = ((OEmfPlusRecDrawDriverString*)m_recInfo.Data)->BrushId;
		pRec = pEMF->GetObjectCreationRecord(nID, true);
		if (pRec)
			AddLinkRecord(pRec);
	}
}

EMFAccess::EMFAccess(const std::vector<u8t>& vData)
{
	ATL::CComPtr<IStream> stream;
	stream.Attach(SHCreateMemStream(vData.data(), (UINT)vData.size()));
	m_pMetafile = std::make_unique<Gdiplus::Metafile>(stream);
	m_pMetafile->GetMetafileHeader(&m_hdr);
}

EMFAccess::~EMFAccess()
{
	FreeRecords();
}

void EMFAccess::DrawMetafile(Gdiplus::Graphics& gg, const CRect& rcDraw) const
{
	ASSERT(m_pMetafile.get());
	Gdiplus::Rect rcDrawP(rcDraw.left, rcDraw.top, rcDraw.Width(), rcDraw.Height());
	gg.DrawImage(m_pMetafile.get(), rcDrawP);
}

Gdiplus::Image* EMFAccess::CloneMetafile() const
{
	ASSERT(m_pMetafile.get());
	return m_pMetafile->Clone();
}

struct EnumEmfPlusContext
{
	Gdiplus::Metafile*	pMetafile;
	Gdiplus::Graphics*	pGraphics;
	EMFAccess*			pAccess;
};

BOOL EnumMetafilePlusProc(Gdiplus::EmfPlusRecordType type, UINT flags, UINT dataSize, const BYTE* data, VOID* pCallbackData)
{
	auto& ctxt = *(EnumEmfPlusContext*)pCallbackData;
	auto ret = ctxt.pAccess->HandleEMFRecord((OEmfPlusRecordType)type, flags, dataSize, data);
	if (ret)
		ctxt.pMetafile->PlayRecord(type, flags, dataSize, data);
	return ret;
}

bool EMFAccess::GetRecords()
{
	CDC dcMem;
	dcMem.CreateCompatibleDC(nullptr);
	Gdiplus::Graphics gg(dcMem.GetSafeHdc());
	Gdiplus::Point pt(m_hdr.X, m_hdr.Y);
	EnumEmfPlusContext ctxt{ m_pMetafile.get(), &gg, this };
	auto sts = gg.EnumerateMetafile(m_pMetafile.get(), pt, EnumMetafilePlusProc, (void*)&ctxt);
	return sts == Gdiplus::Ok;
}

void EMFAccess::FreeRecords()
{
	for (auto pRec : m_EMFRecords)
	{
		delete pRec;
	}
	m_EMFRecords.clear();
	m_vPlusObjTable.clear();
}

bool EMFAccess::HandleEMFRecord(OEmfPlusRecordType type, UINT flags, UINT dataSize, const BYTE* data)
{
	OEmfPlusRecInfo rec;
	rec.Type = (u16t)type;
	rec.Flags = (u16t)flags;
	rec.Size = sizeof(OEmfPlusRec) + dataSize;
	rec.DataSize = dataSize;
	rec.Data = (u8t*)data;
	EMFRecAccess* pRecAccess = nullptr;
	switch (type)
	{
	case EmfRecordTypeHeader:
		pRecAccess = new EMFRecAccessGDIRecHeader;
		break;
	case EmfRecordTypePolyBezier:
		pRecAccess = new EMFRecAccessGDIRecPolyBezier;
		break;
	case EmfRecordTypePolygon:
		pRecAccess = new EMFRecAccessGDIRecPolygon;
		break;
	case EmfRecordTypePolyline:
		pRecAccess = new EMFRecAccessGDIRecPolyline;
		break;
	case EmfRecordTypePolyBezierTo:
		pRecAccess = new EMFRecAccessGDIRecPolyBezierTo;
		break;
	case EmfRecordTypePolyLineTo:
		pRecAccess = new EMFRecAccessGDIRecPolyLineTo;
		break;
	case EmfRecordTypePolyPolyline:
		pRecAccess = new EMFRecAccessGDIRecPolyPolyline;
		break;
	case EmfRecordTypePolyPolygon:
		pRecAccess = new EMFRecAccessGDIRecPolyPolygon;
		break;
	case EmfRecordTypeSetWindowExtEx:
		pRecAccess = new EMFRecAccessGDIRecSetWindowExtEx;
		break;
	case EmfRecordTypeSetWindowOrgEx:
		pRecAccess = new EMFRecAccessGDIRecSetWindowOrgEx;
		break;
	case EmfRecordTypeSetViewportExtEx:
		pRecAccess = new EMFRecAccessGDIRecSetViewportExtEx;
		break;
	case EmfRecordTypeSetViewportOrgEx:
		pRecAccess = new EMFRecAccessGDIRecSetViewportOrgEx;
		break;
	case EmfRecordTypeSetBrushOrgEx:
		pRecAccess = new EMFRecAccessGDIRecSetBrushOrgEx;
		break;
	case EmfRecordTypeEOF:
		pRecAccess = new EMFRecAccessGDIRecEOF;
		break;
	case EmfRecordTypeSetPixelV:
		pRecAccess = new EMFRecAccessGDIRecSetPixelV;
		break;
	case EmfRecordTypeSetMapperFlags:
		pRecAccess = new EMFRecAccessGDIRecSetMapperFlags;
		break;
	case EmfRecordTypeSetMapMode:
		pRecAccess = new EMFRecAccessGDIRecSetMapMode;
		break;
	case EmfRecordTypeSetBkMode:
		pRecAccess = new EMFRecAccessGDIRecSetBkMode;
		break;
	case EmfRecordTypeSetPolyFillMode:
		pRecAccess = new EMFRecAccessGDIRecSetPolyFillMode;
		break;
	case EmfRecordTypeSetROP2:
		pRecAccess = new EMFRecAccessGDIRecSetROP2;
		break;
	case EmfRecordTypeSetStretchBltMode:
		pRecAccess = new EMFRecAccessGDIRecSetStretchBltMode;
		break;
	case EmfRecordTypeSetTextAlign:
		pRecAccess = new EMFRecAccessGDIRecSetTextAlign;
		break;
	case EmfRecordTypeSetColorAdjustment:
		pRecAccess = new EMFRecAccessGDIRecSetColorAdjustment;
		break;
	case EmfRecordTypeSetTextColor:
		pRecAccess = new EMFRecAccessGDIRecSetTextColor;
		break;
	case EmfRecordTypeSetBkColor:
		pRecAccess = new EMFRecAccessGDIRecSetBkColor;
		break;
	case EmfRecordTypeOffsetClipRgn:
		pRecAccess = new EMFRecAccessGDIRecOffsetClipRgn;
		break;
	case EmfRecordTypeMoveToEx:
		pRecAccess = new EMFRecAccessGDIRecMoveToEx;
		break;
	case EmfRecordTypeSetMetaRgn:
		pRecAccess = new EMFRecAccessGDIRecSetMetaRgn;
		break;
	case EmfRecordTypeExcludeClipRect:
		pRecAccess = new EMFRecAccessGDIRecExcludeClipRect;
		break;
	case EmfRecordTypeIntersectClipRect:
		pRecAccess = new EMFRecAccessGDIRecIntersectClipRect;
		break;
	case EmfRecordTypeScaleViewportExtEx:
		pRecAccess = new EMFRecAccessGDIRecScaleViewportExtEx;
		break;
	case EmfRecordTypeScaleWindowExtEx:
		pRecAccess = new EMFRecAccessGDIRecScaleWindowExtEx;
		break;
	case EmfRecordTypeSaveDC:
		pRecAccess = new EMFRecAccessGDIRecSaveDC;
		break;
	case EmfRecordTypeRestoreDC:
		pRecAccess = new EMFRecAccessGDIRecRestoreDC;
		break;
	case EmfRecordTypeSetWorldTransform:
		pRecAccess = new EMFRecAccessGDIRecSetWorldTransform;
		break;
	case EmfRecordTypeModifyWorldTransform:
		pRecAccess = new EMFRecAccessGDIRecModifyWorldTransform;
		break;
	case EmfRecordTypeSelectObject:
		pRecAccess = new EMFRecAccessGDIRecSelectObject;
		break;
	case EmfRecordTypeCreatePen:
		pRecAccess = new EMFRecAccessGDIRecCreatePen;
		break;
	case EmfRecordTypeCreateBrushIndirect:
		pRecAccess = new EMFRecAccessGDIRecCreateBrushIndirect;
		break;
	case EmfRecordTypeDeleteObject:
		pRecAccess = new EMFRecAccessGDIRecDeleteObject;
		break;
	case EmfRecordTypeAngleArc:
		pRecAccess = new EMFRecAccessGDIRecAngleArc;
		break;
	case EmfRecordTypeEllipse:
		pRecAccess = new EMFRecAccessGDIRecEllipse;
		break;
	case EmfRecordTypeRectangle:
		pRecAccess = new EMFRecAccessGDIRecRectangle;
		break;
	case EmfRecordTypeRoundRect:
		pRecAccess = new EMFRecAccessGDIRecRoundRect;
		break;
	case EmfRecordTypeArc:
		pRecAccess = new EMFRecAccessGDIRecArc;
		break;
	case EmfRecordTypeChord:
		pRecAccess = new EMFRecAccessGDIRecChord;
		break;
	case EmfRecordTypePie:
		pRecAccess = new EMFRecAccessGDIRecPie;
		break;
	case EmfRecordTypeSelectPalette:
		pRecAccess = new EMFRecAccessGDIRecSelectPalette;
		break;
	case EmfRecordTypeCreatePalette:
		pRecAccess = new EMFRecAccessGDIRecCreatePalette;
		break;
	case EmfRecordTypeSetPaletteEntries:
		pRecAccess = new EMFRecAccessGDIRecSetPaletteEntries;
		break;
	case EmfRecordTypeResizePalette:
		pRecAccess = new EMFRecAccessGDIRecResizePalette;
		break;
	case EmfRecordTypeRealizePalette:
		pRecAccess = new EMFRecAccessGDIRecRealizePalette;
		break;
	case EmfRecordTypeExtFloodFill:
		pRecAccess = new EMFRecAccessGDIRecExtFloodFill;
		break;
	case EmfRecordTypeLineTo:
		pRecAccess = new EMFRecAccessGDIRecLineTo;
		break;
	case EmfRecordTypeArcTo:
		pRecAccess = new EMFRecAccessGDIRecArcTo;
		break;
	case EmfRecordTypePolyDraw:
		pRecAccess = new EMFRecAccessGDIRecPolyDraw;
		break;
	case EmfRecordTypeSetArcDirection:
		pRecAccess = new EMFRecAccessGDIRecSetArcDirection;
		break;
	case EmfRecordTypeSetMiterLimit:
		pRecAccess = new EMFRecAccessGDIRecSetMiterLimit;
		break;
	case EmfRecordTypeBeginPath:
		pRecAccess = new EMFRecAccessGDIRecBeginPath;
		break;
	case EmfRecordTypeEndPath:
		pRecAccess = new EMFRecAccessGDIRecEndPath;
		break;
	case EmfRecordTypeCloseFigure:
		pRecAccess = new EMFRecAccessGDIRecCloseFigure;
		break;
	case EmfRecordTypeFillPath:
		pRecAccess = new EMFRecAccessGDIRecFillPath;
		break;
	case EmfRecordTypeStrokeAndFillPath:
		pRecAccess = new EMFRecAccessGDIRecStrokeAndFillPath;
		break;
	case EmfRecordTypeStrokePath:
		pRecAccess = new EMFRecAccessGDIRecStrokePath;
		break;
	case EmfRecordTypeFlattenPath:
		pRecAccess = new EMFRecAccessGDIRecFlattenPath;
		break;
	case EmfRecordTypeWidenPath:
		pRecAccess = new EMFRecAccessGDIRecWidenPath;
		break;
	case EmfRecordTypeSelectClipPath:
		pRecAccess = new EMFRecAccessGDIRecSelectClipPath;
		break;
	case EmfRecordTypeAbortPath:
		pRecAccess = new EMFRecAccessGDIRecAbortPath;
		break;
	case EmfRecordTypeReserved_069:
		pRecAccess = new EMFRecAccessGDIRecReserved_069;
		break;
	case EmfRecordTypeGdiComment:
		pRecAccess = new EMFRecAccessGDIRecGdiComment;
		break;
	case EmfRecordTypeFillRgn:
		pRecAccess = new EMFRecAccessGDIRecFillRgn;
		break;
	case EmfRecordTypeFrameRgn:
		pRecAccess = new EMFRecAccessGDIRecFrameRgn;
		break;
	case EmfRecordTypeInvertRgn:
		pRecAccess = new EMFRecAccessGDIRecInvertRgn;
		break;
	case EmfRecordTypePaintRgn:
		pRecAccess = new EMFRecAccessGDIRecPaintRgn;
		break;
	case EmfRecordTypeExtSelectClipRgn:
		pRecAccess = new EMFRecAccessGDIRecExtSelectClipRgn;
		break;
	case EmfRecordTypeBitBlt:
		pRecAccess = new EMFRecAccessGDIRecBitBlt;
		break;
	case EmfRecordTypeStretchBlt:
		pRecAccess = new EMFRecAccessGDIRecStretchBlt;
		break;
	case EmfRecordTypeMaskBlt:
		pRecAccess = new EMFRecAccessGDIRecMaskBlt;
		break;
	case EmfRecordTypePlgBlt:
		pRecAccess = new EMFRecAccessGDIRecPlgBlt;
		break;
	case EmfRecordTypeSetDIBitsToDevice:
		pRecAccess = new EMFRecAccessGDIRecSetDIBitsToDevice;
		break;
	case EmfRecordTypeStretchDIBits:
		pRecAccess = new EMFRecAccessGDIRecStretchDIBits;
		break;
	case EmfRecordTypeExtCreateFontIndirect:
		pRecAccess = new EMFRecAccessGDIRecExtCreateFontIndirect;
		break;
	case EmfRecordTypeExtTextOutA:
		pRecAccess = new EMFRecAccessGDIRecExtTextOutA;
		break;
	case EmfRecordTypeExtTextOutW:
		pRecAccess = new EMFRecAccessGDIRecExtTextOutW;
		break;
	case EmfRecordTypePolyBezier16:
		pRecAccess = new EMFRecAccessGDIRecPolyBezier16;
		break;
	case EmfRecordTypePolygon16:
		pRecAccess = new EMFRecAccessGDIRecPolygon16;
		break;
	case EmfRecordTypePolyline16:
		pRecAccess = new EMFRecAccessGDIRecPolyline16;
		break;
	case EmfRecordTypePolyBezierTo16:
		pRecAccess = new EMFRecAccessGDIRecPolyBezierTo16;
		break;
	case EmfRecordTypePolylineTo16:
		pRecAccess = new EMFRecAccessGDIRecPolylineTo16;
		break;
	case EmfRecordTypePolyPolyline16:
		pRecAccess = new EMFRecAccessGDIRecPolyPolyline16;
		break;
	case EmfRecordTypePolyPolygon16:
		pRecAccess = new EMFRecAccessGDIRecPolyPolygon16;
		break;
	case EmfRecordTypePolyDraw16:
		pRecAccess = new EMFRecAccessGDIRecPolyDraw16;
		break;
	case EmfRecordTypeCreateMonoBrush:
		pRecAccess = new EMFRecAccessGDIRecCreateMonoBrush;
		break;
	case EmfRecordTypeCreateDIBPatternBrushPt:
		pRecAccess = new EMFRecAccessGDIRecCreateDIBPatternBrushPt;
		break;
	case EmfRecordTypeExtCreatePen:
		pRecAccess = new EMFRecAccessGDIRecExtCreatePen;
		break;
	case EmfRecordTypePolyTextOutA:
		pRecAccess = new EMFRecAccessGDIRecPolyTextOutA;
		break;
	case EmfRecordTypePolyTextOutW:
		pRecAccess = new EMFRecAccessGDIRecPolyTextOutW;
		break;
	case EmfRecordTypeSetICMMode:
		pRecAccess = new EMFRecAccessGDIRecSetICMMode;
		break;
	case EmfRecordTypeCreateColorSpace:
		pRecAccess = new EMFRecAccessGDIRecCreateColorSpace;
		break;
	case EmfRecordTypeSetColorSpace:
		pRecAccess = new EMFRecAccessGDIRecSetColorSpace;
		break;
	case EmfRecordTypeDeleteColorSpace:
		pRecAccess = new EMFRecAccessGDIRecDeleteColorSpace;
		break;
	case EmfRecordTypeGLSRecord:
		pRecAccess = new EMFRecAccessGDIRecGLSRecord;
		break;
	case EmfRecordTypeGLSBoundedRecord:
		pRecAccess = new EMFRecAccessGDIRecGLSBoundedRecord;
		break;
	case EmfRecordTypePixelFormat:
		pRecAccess = new EMFRecAccessGDIRecPixelFormat;
		break;
	case EmfRecordTypeDrawEscape:
		pRecAccess = new EMFRecAccessGDIRecDrawEscape;
		break;
	case EmfRecordTypeExtEscape:
		pRecAccess = new EMFRecAccessGDIRecExtEscape;
		break;
	case EmfRecordTypeStartDoc:
		pRecAccess = new EMFRecAccessGDIRecStartDoc;
		break;
	case EmfRecordTypeSmallTextOut:
		pRecAccess = new EMFRecAccessGDIRecSmallTextOut;
		break;
	case EmfRecordTypeForceUFIMapping:
		pRecAccess = new EMFRecAccessGDIRecForceUFIMapping;
		break;
	case EmfRecordTypeNamedEscape:
		pRecAccess = new EMFRecAccessGDIRecNamedEscape;
		break;
	case EmfRecordTypeColorCorrectPalette:
		pRecAccess = new EMFRecAccessGDIRecColorCorrectPalette;
		break;
	case EmfRecordTypeSetICMProfileA:
		pRecAccess = new EMFRecAccessGDIRecSetICMProfileA;
		break;
	case EmfRecordTypeSetICMProfileW:
		pRecAccess = new EMFRecAccessGDIRecSetICMProfileW;
		break;
	case EmfRecordTypeAlphaBlend:
		pRecAccess = new EMFRecAccessGDIRecAlphaBlend;
		break;
	case EmfRecordTypeSetLayout:
		pRecAccess = new EMFRecAccessGDIRecSetLayout;
		break;
	case EmfRecordTypeTransparentBlt:
		pRecAccess = new EMFRecAccessGDIRecTransparentBlt;
		break;
	case EmfRecordTypeReserved_117:
		pRecAccess = new EMFRecAccessGDIRecReserved_117;
		break;
	case EmfRecordTypeGradientFill:
		pRecAccess = new EMFRecAccessGDIRecGradientFill;
		break;
	case EmfRecordTypeSetLinkedUFIs:
		pRecAccess = new EMFRecAccessGDIRecSetLinkedUFIs;
		break;
	case EmfRecordTypeSetTextJustification:
		pRecAccess = new EMFRecAccessGDIRecSetTextJustification;
		break;
	case EmfRecordTypeColorMatchToTargetW:
		pRecAccess = new EMFRecAccessGDIRecColorMatchToTargetW;
		break;
	case EmfRecordTypeCreateColorSpaceW:
		pRecAccess = new EMFRecAccessGDIRecCreateColorSpaceW;
		break;
	case EmfPlusRecordTypeHeader:
		pRecAccess = new EMFRecAccessGDIPlusRecHeader;
		break;
	case EmfPlusRecordTypeEndOfFile:
		pRecAccess = new EMFRecAccessGDIPlusRecEndOfFile;
		break;
	case EmfPlusRecordTypeComment:
		pRecAccess = new EMFRecAccessGDIPlusRecComment;
		break;
	case EmfPlusRecordTypeGetDC:
		pRecAccess = new EMFRecAccessGDIPlusRecGetDC;
		break;
	case EmfPlusRecordTypeMultiFormatStart:
		pRecAccess = new EMFRecAccessGDIPlusRecMultiFormatStart;
		break;
	case EmfPlusRecordTypeMultiFormatSection:
		pRecAccess = new EMFRecAccessGDIPlusRecMultiFormatSection;
		break;
	case EmfPlusRecordTypeMultiFormatEnd:
		pRecAccess = new EMFRecAccessGDIPlusRecMultiFormatEnd;
		break;
	case EmfPlusRecordTypeObject:
		pRecAccess = new EMFRecAccessGDIPlusRecObject;
		break;
	case EmfPlusRecordTypeClear:
		pRecAccess = new EMFRecAccessGDIPlusRecClear;
		break;
	case EmfPlusRecordTypeFillRects:
		pRecAccess = new EMFRecAccessGDIPlusRecFillRects;
		break;
	case EmfPlusRecordTypeDrawRects:
		pRecAccess = new EMFRecAccessGDIPlusRecDrawRects;
		break;
	case EmfPlusRecordTypeFillPolygon:
		pRecAccess = new EMFRecAccessGDIPlusRecFillPolygon;
		break;
	case EmfPlusRecordTypeDrawLines:
		pRecAccess = new EMFRecAccessGDIPlusRecDrawLines;
		break;
	case EmfPlusRecordTypeFillEllipse:
		pRecAccess = new EMFRecAccessGDIPlusRecFillEllipse;
		break;
	case EmfPlusRecordTypeDrawEllipse:
		pRecAccess = new EMFRecAccessGDIPlusRecDrawEllipse;
		break;
	case EmfPlusRecordTypeFillPie:
		pRecAccess = new EMFRecAccessGDIPlusRecFillPie;
		break;
	case EmfPlusRecordTypeDrawPie:
		pRecAccess = new EMFRecAccessGDIPlusRecDrawPie;
		break;
	case EmfPlusRecordTypeDrawArc:
		pRecAccess = new EMFRecAccessGDIPlusRecDrawArc;
		break;
	case EmfPlusRecordTypeFillRegion:
		pRecAccess = new EMFRecAccessGDIPlusRecFillRegion;
		break;
	case EmfPlusRecordTypeFillPath:
		pRecAccess = new EMFRecAccessGDIPlusRecFillPath;
		break;
	case EmfPlusRecordTypeDrawPath:
		pRecAccess = new EMFRecAccessGDIPlusRecDrawPath;
		break;
	case EmfPlusRecordTypeFillClosedCurve:
		pRecAccess = new EMFRecAccessGDIPlusRecFillClosedCurve;
		break;
	case EmfPlusRecordTypeDrawClosedCurve:
		pRecAccess = new EMFRecAccessGDIPlusRecDrawClosedCurve;
		break;
	case EmfPlusRecordTypeDrawCurve:
		pRecAccess = new EMFRecAccessGDIPlusRecDrawCurve;
		break;
	case EmfPlusRecordTypeDrawBeziers:
		pRecAccess = new EMFRecAccessGDIPlusRecDrawBeziers;
		break;
	case EmfPlusRecordTypeDrawImage:
		pRecAccess = new EMFRecAccessGDIPlusRecDrawImage;
		break;
	case EmfPlusRecordTypeDrawImagePoints:
		pRecAccess = new EMFRecAccessGDIPlusRecDrawImagePoints;
		break;
	case EmfPlusRecordTypeDrawString:
		pRecAccess = new EMFRecAccessGDIPlusRecDrawString;
		break;
	case EmfPlusRecordTypeSetRenderingOrigin:
		pRecAccess = new EMFRecAccessGDIPlusRecSetRenderingOrigin;
		break;
	case EmfPlusRecordTypeSetAntiAliasMode:
		pRecAccess = new EMFRecAccessGDIPlusRecSetAntiAliasMode;
		break;
	case EmfPlusRecordTypeSetTextRenderingHint:
		pRecAccess = new EMFRecAccessGDIPlusRecSetTextRenderingHint;
		break;
	case EmfPlusRecordTypeSetTextContrast:
		pRecAccess = new EMFRecAccessGDIPlusRecSetTextContrast;
		break;
	case EmfPlusRecordTypeSetInterpolationMode:
		pRecAccess = new EMFRecAccessGDIPlusRecSetInterpolationMode;
		break;
	case EmfPlusRecordTypeSetPixelOffsetMode:
		pRecAccess = new EMFRecAccessGDIPlusRecSetPixelOffsetMode;
		break;
	case EmfPlusRecordTypeSetCompositingMode:
		pRecAccess = new EMFRecAccessGDIPlusRecSetCompositingMode;
		break;
	case EmfPlusRecordTypeSetCompositingQuality:
		pRecAccess = new EMFRecAccessGDIPlusRecSetCompositingQuality;
		break;
	case EmfPlusRecordTypeSave:
		pRecAccess = new EMFRecAccessGDIPlusRecSave;
		break;
	case EmfPlusRecordTypeRestore:
		pRecAccess = new EMFRecAccessGDIPlusRecRestore;
		break;
	case EmfPlusRecordTypeBeginContainer:
		pRecAccess = new EMFRecAccessGDIPlusRecBeginContainer;
		break;
	case EmfPlusRecordTypeBeginContainerNoParams:
		pRecAccess = new EMFRecAccessGDIPlusRecBeginContainerNoParams;
		break;
	case EmfPlusRecordTypeEndContainer:
		pRecAccess = new EMFRecAccessGDIPlusRecEndContainer;
		break;
	case EmfPlusRecordTypeSetWorldTransform:
		pRecAccess = new EMFRecAccessGDIPlusRecSetWorldTransform;
		break;
	case EmfPlusRecordTypeResetWorldTransform:
		pRecAccess = new EMFRecAccessGDIPlusRecResetWorldTransform;
		break;
	case EmfPlusRecordTypeMultiplyWorldTransform:
		pRecAccess = new EMFRecAccessGDIPlusRecMultiplyWorldTransform;
		break;
	case EmfPlusRecordTypeTranslateWorldTransform:
		pRecAccess = new EMFRecAccessGDIPlusRecTranslateWorldTransform;
		break;
	case EmfPlusRecordTypeScaleWorldTransform:
		pRecAccess = new EMFRecAccessGDIPlusRecScaleWorldTransform;
		break;
	case EmfPlusRecordTypeRotateWorldTransform:
		pRecAccess = new EMFRecAccessGDIPlusRecRotateWorldTransform;
		break;
	case EmfPlusRecordTypeSetPageTransform:
		pRecAccess = new EMFRecAccessGDIPlusRecSetPageTransform;
		break;
	case EmfPlusRecordTypeResetClip:
		pRecAccess = new EMFRecAccessGDIPlusRecResetClip;
		break;
	case EmfPlusRecordTypeSetClipRect:
		pRecAccess = new EMFRecAccessGDIPlusRecSetClipRect;
		break;
	case EmfPlusRecordTypeSetClipPath:
		pRecAccess = new EMFRecAccessGDIPlusRecSetClipPath;
		break;
	case EmfPlusRecordTypeSetClipRegion:
		pRecAccess = new EMFRecAccessGDIPlusRecSetClipRegion;
		break;
	case EmfPlusRecordTypeOffsetClip:
		pRecAccess = new EMFRecAccessGDIPlusRecOffsetClip;
		break;
	case EmfPlusRecordTypeDrawDriverString:
		pRecAccess = new EMFRecAccessGDIPlusRecDrawDriverString;
		break;
	case EmfPlusRecordTypeStrokeFillPath:
		pRecAccess = new EMFRecAccessGDIPlusRecStrokeFillPath;
		break;
	case EmfPlusRecordTypeSerializableObject:
		pRecAccess = new EMFRecAccessGDIPlusRecSerializableObject;
		break;
	case EmfPlusRecordTypeSetTSGraphics:
		pRecAccess = new EMFRecAccessGDIPlusRecSetTSGraphics;
		break;
	case EmfPlusRecordTypeSetTSClip:
		pRecAccess = new EMFRecAccessGDIPlusRecSetTSClip;
		break;
	}
	if (!pRecAccess)
	{
		ASSERT(0);
		return false;
	}
	pRecAccess->SetRecInfo(rec);
	pRecAccess->SetIndex(m_EMFRecords.size());
	pRecAccess->Preprocess(this);
	m_EMFRecords.push_back(pRecAccess);
	return true;
}

EMFRecAccess* EMFAccess::GetObjectCreationRecord(size_t index, bool bPlus) const
{
	if (bPlus)
	{
		if (index < m_vPlusObjTable.size())
			return m_vPlusObjTable[index].pRec;
	}
	else
	{
		// TODO
	}
	return nullptr;
}

bool EMFAccess::SetObjectToTable(size_t index, EMFRecAccess* pRec, bool bPlus)
{
	if (bPlus)
	{
		if (index >= m_vPlusObjTable.size())
			m_vPlusObjTable.resize(index + 1);
		m_vPlusObjTable[index].pRec = pRec;
	}
	else
	{
		// TODO
	}
	return true;
}




