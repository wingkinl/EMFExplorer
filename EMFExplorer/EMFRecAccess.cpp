#include "pch.h"
#include "framework.h"
#include "EMFRecAccess.h"

#include <atlbase.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

using namespace emfplus;

CRect GetFitRect(const CRect& rcDest, const SIZE& szSrc, bool bCenter, float* pfScale)
{
	CRect rcFit = rcDest;
	CSize szDest = rcDest.Size();
	auto fScaleDst = (float)szDest.cy / szDest.cx;
	auto fScaleSrc = (float)szSrc.cy / szSrc.cx;
	float fScale = 1.f;
	if (fScaleSrc <= fScaleDst)
	{
		// source image is flatter than the target rectangle, so we fit the width
		fScale = (float)szDest.cx / szSrc.cx;
		szDest.cy	= (int)(szSrc.cy * fScale);
		rcFit.bottom = rcFit.top + szDest.cy;
		if (bCenter)
			rcFit.OffsetRect(0, (rcDest.Height() - rcFit.Height()) / 2);
	}
	else
	{
		fScale = (float)szDest.cy / szSrc.cy;
		szDest.cx	= (int)(szSrc.cx * fScale);
		rcFit.right = rcFit.left + szDest.cx;
		if (bCenter)
			rcFit.OffsetRect((rcDest.Width() - rcFit.Width()) / 2, 0);
	}
	if (pfScale)
		*pfScale = fScale;
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

static inline LPCWSTR EMFPlusMetafileTypeText(OMetafileDataType type)
{
	static const LPCWSTR aText[] = {
		L"Invalid", L"Wmf", L"WmfPlaceable", L"Emf", L"EmfPlusOnly", L"EmfPlusDual",
	};
	return aText[(int)type];
}

static void GetPropertiesFromGDIPlusHeader(PropertyNode* pNode, const Gdiplus::MetafileHeader& hdr)
{
	pNode->AddText(L"MetafileType", EMFPlusMetafileTypeText((OMetafileDataType)hdr.Type));
	pNode->AddValue(L"Version", hdr.Version, true);
	pNode->AddValue(L"EmfPlusFlags", hdr.EmfPlusFlags, true);
	pNode->AddValue(L"DpiX", hdr.DpiX);
	pNode->AddValue(L"DpiY", hdr.DpiY);
	pNode->AddValue(L"X", hdr.X);
	pNode->AddValue(L"Y", hdr.Y);
	pNode->AddValue(L"Width", hdr.Width);
	pNode->AddValue(L"Height", hdr.Height);
	pNode->AddValue(L"LogicalDpiX", hdr.LogicalDpiX);
	pNode->AddValue(L"LogicalDpiY", hdr.LogicalDpiY);
}

void EMFRecAccessGDIRecHeader::CachePropertiesFromGDI(EMFAccess* pEMF, const ENHMETARECORD* pEMFRec)
{
	auto pRec = (const ENHMETAHEADER*)pEMFRec;
	ASSERT(pRec->dSignature == ENHMETA_SIGNATURE);
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodeRectInt>(L"rclBounds", pRec->rclBounds));
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodeRectInt>(L"rclFrame", pRec->rclFrame));
	m_propsCached->AddValue(L"Signature", pRec->dSignature, true);
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

	auto pPlusNode = m_propsCached->AddBranch(L"GDI+ Header");
	auto& hdr = pEMF->GetMetafileHeader();
	GetPropertiesFromGDIPlusHeader(pPlusNode.get(), hdr);
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
	m_propsCached->AddValue(L"Flags", m_recInfo.Flags, true);
}

void EMFRecAccessGDIPlusRecHeader::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusControlCat::CacheProperties(pEMF);
	auto pHdrRec = (OEmfPlusHeader*)m_recInfo.Data;
	auto& hdr = pEMF->GetMetafileHeader();
	ASSERT(pHdrRec->Version == hdr.Version);
	ASSERT(pHdrRec->LogicalDpiX == hdr.LogicalDpiX);
	ASSERT(pHdrRec->EmfPlusFlags == hdr.EmfPlusFlags);
	ASSERT(pHdrRec->LogicalDpiY == hdr.LogicalDpiY);
	GetPropertiesFromGDIPlusHeader(m_propsCached.get(), hdr);
}

void EMFRecAccessGDIPlusRecObject::Preprocess(EMFAccess* pEMF)
{
	auto nObjectID = (u8t)(m_recInfo.Flags & OEmfPlusRecObjectReader::FlagObjectIDMask);
	pEMF->SetObjectToTable(nObjectID, this, true);
}

static inline LPCWSTR EMFPlusHatchStyleText(OHatchStyle val)
{
	static const LPCWSTR aText[] = {
		L"StyleHorizontal",
		L"StyleVertical",
		L"StyleForwardDiagonal",
		L"StyleBackwardDiagonal",
		L"StyleLargeGrid",
		L"StyleDiagonalCross",
		L"Style05Percent",
		L"Style10Percent",
		L"Style20Percent",
		L"Style25Percent",
		L"Style30Percent",
		L"Style40Percent",
		L"Style50Percent",
		L"Style60Percent",
		L"Style70Percent",
		L"Style75Percent",
		L"Style80Percent",
		L"Style90Percent",
		L"StyleLightDownwardDiagonal",
		L"StyleLightUpwardDiagonal",
		L"StyleDarkDownwardDiagonal",
		L"StyleDarkUpwardDiagonal",
		L"StyleWideDownwardDiagonal",
		L"StyleWideUpwardDiagonal",
		L"StyleLightVertical",
		L"StyleLightHorizontal",
		L"StyleNarrowVertical",
		L"StyleNarrowHorizontal",
		L"StyleDarkVertical",
		L"StyleDarkHorizontal",
		L"StyleDashedDownwardDiagonal",
		L"StyleDashedUpwardDiagonal",
		L"StyleDashedHorizontal",
		L"StyleDashedVertical",
		L"StyleSmallConfetti",
		L"StyleLargeConfetti",
		L"StyleZigZag",
		L"StyleWave",
		L"StyleDiagonalBrick",
		L"StyleHorizontalBrick",
		L"StyleWeave",
		L"StylePlaid",
		L"StyleDivot",
		L"StyleDottedGrid",
		L"StyleDottedDiamond",
		L"StyleShingle",
		L"StyleTrellis",
		L"StyleSphere",
		L"StyleSmallGrid",
		L"StyleSmallCheckerBoard",
		L"StyleLargeCheckerBoard",
		L"StyleOutlinedDiamond",
		L"StyleSolidDiamond"
	};
	return aText[(int)val];
}

static inline LPCWSTR EMFPlusWrapModeText(OWrapMode val)
{
	static const LPCWSTR aText[] = {
		L"Tile",
		L"TileFlipX",
		L"TileFlipY",
		L"TileFlipXY",
		L"Clamp"
	};
	return aText[(int)val];
}

static inline LPCWSTR EMFPlusBrushTypeText(OBrushType val)
{
	static const LPCWSTR aText[] = {
		L"SolidColor",
		L"HatchFill",
		L"TextureFill",
		L"PathGradient",
		L"LinearGradient"
	};
	return aText[(int)val];
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
		auto pObj = (OEmfPlusBrush*)m_obj.get();
		pNode->AddText(L"BrushType", EMFPlusBrushTypeText(pObj->Type));
		switch (pObj->Type)
		{
		case OBrushType::SolidColor:
			pNode->sub.emplace_back(std::make_shared<PropertyNodeColor>(L"SolidColor", pObj->BrushDataSolid.SolidColor));
			break;
		case OBrushType::HatchFill:
			pNode->AddText(L"HatchStyle", EMFPlusHatchStyleText(pObj->BrushDataHatch.HatchStyle));
			pNode->sub.emplace_back(std::make_shared<PropertyNodeColor>(L"ForeColor", pObj->BrushDataHatch.ForeColor));
			pNode->sub.emplace_back(std::make_shared<PropertyNodeColor>(L"BackColor", pObj->BrushDataHatch.BackColor));
			break;
		case OBrushType::TextureFill:
			pNode->AddValue(L"BrushDataFlags", (u32t)pObj->BrushDataTexture.BrushDataFlags, true);
			pNode->AddText(L"WrapMode", EMFPlusWrapModeText(pObj->BrushDataTexture.WrapMode));
			// TODO
			break;
		case OBrushType::PathGradient:
			break;
		case OBrushType::LinearGradient:
			break;
		}
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
			pNode->AddValue(L"PixelFormat", (u32t)pImg->ImageDataBmp.PixelFormat, true);
			pNode->AddText(L"BitmapType", EMFPlusBitmapTypeText(pImg->ImageDataBmp.Type));
			break;
		case OImageDataType::Metafile:
			pNode->AddText(L"MetafileType", EMFPlusMetafileTypeText(pImg->ImageDataMetafile.Type));
			pNode->AddValue(L"MetafileDataSize", pImg->ImageDataMetafile.MetafileDataSize);
			break;
		}
	}

	bool CacheGDIPlusObject(EMFAccess* pEMF) override
	{
		if (m_emf)
			return true;
		auto pImg = (OEmfPlusImage*)m_obj.get();
		switch (pImg->Type)
		{
		case OImageDataType::Bitmap:
			// TODO
			break;
		case OImageDataType::Metafile:
			m_emf = std::make_shared<EMFAccess>(pImg->ImageDataMetafile.MetafileData);
			m_emf->AddNestedPath(pEMF->GetNestedPath().c_str());
			m_emf->AddNestedPath(std::to_wstring(m_pObjRec->GetIndex()+1).c_str());
			break;
		}
		return true;
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
		pObjWrapper->CacheProperties(pEMF, m_propsCached.get());
	}
}

void EMFRecAccessGDIPlusRecClear::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(pEMF);
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
			AddLinkRecord(pRec);
	}
}

void EMFRecAccessGDIPlusRecFillRects::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(pEMF);
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
		auto pBrushRec = GetLinkedObject(0);
		if (pBrushRec)
		{
			auto pBrushProp = pBrushRec->GetProperties(pEMF);
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
		AddLinkRecord(pRec);
	}
}

void EMFRecAccessGDIPlusRecDrawRects::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(pEMF);
	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusRectDataArray>(L"RectData", m_recDataCached.RectData));
	auto pPenRec = GetLinkedObject(0);
	if (pPenRec)
	{
		auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawRects::FlagObjectIDMask);
		m_propsCached->AddValue(L"PenID", nID);
		auto pPenProp = pPenRec->GetProperties(pEMF);
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
			AddLinkRecord(pRec);
	}
}

void EMFRecAccessGDIPlusRecFillPolygon::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(pEMF);
	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));
	if (m_recInfo.Flags & OEmfPlusRecFillPolygon::FlagS)
	{
		m_propsCached->sub.emplace_back(std::make_shared<PropertyNodeColor>(L"Brush", (emfplus::OEmfPlusARGB&)m_recDataCached.BrushId));
	}
	else
	{
		m_propsCached->AddValue(L"BrushID", m_recDataCached.BrushId);
		auto pBrushRec = GetLinkedObject(0);
		if (pBrushRec)
		{
			auto pBrushProp = pBrushRec->GetProperties(pEMF);
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
		AddLinkRecord(pRec);
	}
}

void EMFRecAccessGDIPlusRecDrawLines::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(pEMF);
	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawLines::FlagObjectIDMask);
	m_propsCached->AddValue(L"PenID", nID);
	m_propsCached->AddValue(L"Closed", m_recInfo.Flags & OEmfPlusRecDrawLines::FlagL);
	bool bRelative = (m_recInfo.Flags & OEmfPlusRecDrawLines::FlagP);
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusPointDataArray>(L"PointData", m_recDataCached.PointData, bRelative));
	auto pPenRec = GetLinkedObject(0);
	if (pPenRec)
	{
		auto pPenProp = pPenRec->GetProperties(pEMF);
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
			AddLinkRecord(pRec);
	}
}

void EMFRecAccessGDIPlusRecFillEllipse::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(pEMF);
	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));
	if (m_recInfo.Flags & OEmfPlusRecFillEllipse::FlagS)
	{
		m_propsCached->sub.emplace_back(std::make_shared<PropertyNodeColor>(L"Brush", (emfplus::OEmfPlusARGB&)m_recDataCached.BrushId));
	}
	else
	{
		m_propsCached->AddValue(L"BrushID", m_recDataCached.BrushId);
		auto pBrushRec = GetLinkedObject(0);
		if (pBrushRec)
		{
			auto pBrushProp = pBrushRec->GetProperties(pEMF);
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
		AddLinkRecord(pRec);
	}
}

void EMFRecAccessGDIPlusRecDrawEllipse::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(pEMF);
	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawEllipse::FlagObjectIDMask);
	m_propsCached->AddValue(L"PenID", nID);
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusRectData>(L"RectData", m_recDataCached.RectData));
	auto pPenRec = GetLinkedObject(0);
	if (pPenRec)
	{
		auto pPenProp = pPenRec->GetProperties(pEMF);
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
			AddLinkRecord(pRec);
	}
}

void EMFRecAccessGDIPlusRecFillPie::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(pEMF);
	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));
	if (m_recInfo.Flags & OEmfPlusRecFillPie::FlagS)
	{
		m_propsCached->sub.emplace_back(std::make_shared<PropertyNodeColor>(L"Brush", (emfplus::OEmfPlusARGB&)m_recDataCached.BrushId));
	}
	else
	{
		m_propsCached->AddValue(L"BrushID", m_recDataCached.BrushId);
		auto pBrushRec = GetLinkedObject(0);
		if (pBrushRec)
		{
			auto pBrushProp = pBrushRec->GetProperties(pEMF);
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
		AddLinkRecord(pRec);
	}
}

void EMFRecAccessGDIPlusRecDrawPie::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(pEMF);
	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawPie::FlagObjectIDMask);
	m_propsCached->AddValue(L"PenID", nID);
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusArcData>(L"ArcData", m_recDataCached.ArcData));
	auto pPenRec = GetLinkedObject(0);
	if (pPenRec)
	{
		auto pPenProp = pPenRec->GetProperties(pEMF);
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
		AddLinkRecord(pRec);
	}
}

void EMFRecAccessGDIPlusRecDrawArc::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(pEMF);
	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawArc::FlagObjectIDMask);
	m_propsCached->AddValue(L"PenID", nID);
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusArcData>(L"ArcData", m_recDataCached.ArcData));
	auto pPenRec = GetLinkedObject(0);
	if (pPenRec)
	{
		auto pPenProp = pPenRec->GetProperties(pEMF);
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

void EMFRecAccessGDIPlusRecSetClipRect::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusClippingCat::CacheProperties(pEMF);
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
		AddLinkRecord(pRec);
}

void EMFRecAccessGDIPlusRecSetClipPath::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusClippingCat::CacheProperties(pEMF);
	auto pRec = (OEmfPlusRecSetClipPath*)m_recInfo.Data;
	auto mode = OEmfPlusRecSetClipPath::GetCombineMode(m_recInfo.Flags);
	m_propsCached->AddText(L"CombineMode", EMFPlusCombineModeText(mode));
	auto pPath = GetLinkedObject(0);
	if (pPath)
	{
		auto pProp = pPath->GetProperties(pEMF);
		auto pNode = m_propsCached->AddBranch(L"Path");
		pNode->sub = pProp->sub;
	}
}

void EMFRecAccessGDIPlusRecSetClipRegion::Preprocess(EMFAccess* pEMF)
{
	auto nID = OEmfPlusRecSetClipRegion::GetObjectID(m_recInfo.Flags);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
		AddLinkRecord(pRec);
}

void EMFRecAccessGDIPlusRecSetClipRegion::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusClippingCat::CacheProperties(pEMF);
	auto pRec = (OEmfPlusRecSetClipRegion*)m_recInfo.Data;
	auto mode = OEmfPlusRecSetClipRegion::GetCombineMode(m_recInfo.Flags);
	m_propsCached->AddText(L"CombineMode", EMFPlusCombineModeText(mode));
	auto pPath = GetLinkedObject(0);
	if (pPath)
	{
		auto pProp = pPath->GetProperties(pEMF);
		auto pNode = m_propsCached->AddBranch(L"Region");
		pNode->sub = pProp->sub;
	}
}

void EMFRecAccessGDIPlusRecOffsetClip::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusClippingCat::CacheProperties(pEMF);
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

void EMFRecAccessGDIPlusRecFillRegion::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(pEMF);
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
		auto pBrushRec = GetLinkedObject(LinkedObjTypeBrush);
		if (pBrushRec)
		{
			auto pBrushProp = pBrushRec->GetProperties(pEMF);
			auto pBrushNode = m_propsCached->AddBranch(L"Brush");
			pBrushNode->sub = pBrushProp->sub;
		}
	}
	auto pRgnRec = GetLinkedObject(LinkedObjTypeRegion);
	if (pRgnRec)
	{
		auto pRgnProp = pRgnRec->GetProperties(pEMF);
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

void EMFRecAccessGDIPlusRecFillPath::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(pEMF);
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
		auto pBrushRec = GetLinkedObject(LinkedObjTypeBrush);
		if (pBrushRec)
		{
			auto pBrushProp = pBrushRec->GetProperties(pEMF);
			auto pBrushNode = m_propsCached->AddBranch(L"Brush");
			pBrushNode->sub = pBrushProp->sub;
		}
	}
	auto pRgnRec = GetLinkedObject(LinkedObjTypePath);
	if (pRgnRec)
	{
		auto pRgnProp = pRgnRec->GetProperties(pEMF);
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
		AddLinkRecord(pPathRec);
		auto pRec = (const OEmfPlusRecDrawPath*)m_recInfo.Data;
		auto pPen = pEMF->GetObjectCreationRecord(pRec->PenId, true);
		if (pPen)
			AddLinkRecord(pPen);
	}
}

void EMFRecAccessGDIPlusRecDrawPath::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(pEMF);
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawPath::FlagObjectIDMask);
	m_propsCached->AddValue(L"PathID", nID);
	auto pPathRec = GetLinkedObject(LinkedObjTypePath);
	if (pPathRec)
	{
		auto pProp = pPathRec->GetProperties(pEMF);
		auto pNode = m_propsCached->AddBranch(L"Path");
		pNode->sub = pProp->sub;
	}
	auto pRec = (const OEmfPlusRecDrawPath*)m_recInfo.Data;
	m_propsCached->AddValue(L"PenID", pRec->PenId);
	auto pPenRec = GetLinkedObject(LinkedObjTypePen);
	if (pPenRec)
	{
		auto pPenProp = pPenRec->GetProperties(pEMF);
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
			AddLinkRecord(pRec);
	}
}

static inline LPCWSTR EMFPlusWindingModeText(bool bWinding)
{
	return bWinding ? L"Winding" : L"Alternate";
}

void EMFRecAccessGDIPlusRecFillClosedCurve::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(pEMF);
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
		auto pBrushRec = GetLinkedObject(0);
		if (pBrushRec)
		{
			auto pBrushProp = pBrushRec->GetProperties(pEMF);
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
		AddLinkRecord(pRec);
}

void EMFRecAccessGDIPlusRecDrawClosedCurve::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(pEMF);
	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawClosedCurve::FlagObjectIDMask);
	m_propsCached->AddValue(L"PenID", nID);
	auto pRec = GetLinkedObject(0);
	if (pRec)
	{
		auto pProp = pRec->GetProperties(pEMF);
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
		AddLinkRecord(pRec);
}

void EMFRecAccessGDIPlusRecDrawCurve::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(pEMF);
	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawCurve::FlagObjectIDMask);
	m_propsCached->AddValue(L"PenID", nID);
	auto pRec = GetLinkedObject(0);
	if (pRec)
	{
		auto pProp = pRec->GetProperties(pEMF);
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
		AddLinkRecord(pRec);
}

void EMFRecAccessGDIPlusRecDrawBeziers::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(pEMF);
	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawBeziers::FlagObjectIDMask);
	m_propsCached->AddValue(L"PenID", nID);
	auto pRec = GetLinkedObject(0);
	if (pRec)
	{
		auto pProp = pRec->GetProperties(pEMF);
		auto pNode = m_propsCached->AddBranch(L"Pen");
		pNode->sub = pProp->sub;
	}
	bool bRelative = (m_recInfo.Flags & OEmfPlusRecDrawBeziers::FlagP);
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusPointDataArray>(L"PointData", m_recDataCached.PointData, bRelative));
}

void EMFRecAccessGDIPlusRecDrawImage::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawImage::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
	{
		AddLinkRecord(pRec);
		auto pImgRec = (const OEmfPlusRecDrawImage*)m_recInfo.Data;
		if (pImgRec->ImageAttributesID)
		{
			// TODO
		}
	}
}

static inline LPCWSTR EMFPlusUnitTypeText(OUnitType type)
{
	static const LPCWSTR aText[] = {
		L"World", L"Display", L"Pixel", L"Point", L"Inch", L"Document", L"Millimeter"
	};
	return aText[(int)type];
}

void EMFRecAccessGDIPlusRecDrawImage::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(pEMF);

	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));

	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawImage::FlagObjectIDMask);
	m_propsCached->AddValue(L"ImageID", nID);
	m_propsCached->AddValue(L"ImageAttributesID", m_recDataCached.ImageAttributesID);
	m_propsCached->AddText(L"SrcUnit", EMFPlusUnitTypeText(m_recDataCached.SrcUnit));
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusRectF>(L"SrcRect", m_recDataCached.SrcRect));
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusRectData>(L"RectData", m_recDataCached.RectData));

	auto pImg = GetLinkedObject(LinkedObjTypeImage);
	if (pImg)
	{
		auto pImgProp = pImg->GetProperties(pEMF);
		auto pImgNode = m_propsCached->AddBranch(L"Image");
		pImgNode->sub = pImgProp->sub;

		auto pAttrRec = GetLinkedObject(LinkedObjTypeImageAttribute);
		if (pAttrRec)
		{
			auto pAttrProp = pAttrRec->GetProperties(pEMF);
			auto pAttrNode = m_propsCached->AddBranch(L"ImageAttribute");
			pAttrNode->sub = pAttrProp->sub;
		}
	}
}

void EMFRecAccessGDIPlusRecDrawImagePoints::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawImagePoints::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
	{
		AddLinkRecord(pRec);
		auto pImgRec = (const OEmfPlusRecDrawImage*)m_recInfo.Data;
		if (pImgRec->ImageAttributesID)
		{
			// TODO
		}
	}
}

void EMFRecAccessGDIPlusRecDrawImagePoints::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(pEMF);

	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));

	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawImagePoints::FlagObjectIDMask);
	m_propsCached->AddValue(L"ImageID", nID);
	m_propsCached->AddValue(L"ImageAttributesID", m_recDataCached.ImageAttributesID);
	m_propsCached->AddText(L"SrcUnit", EMFPlusUnitTypeText(m_recDataCached.SrcUnit));
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusRectF>(L"SrcRect", m_recDataCached.SrcRect));
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusPointDataArray>(L"PointData", m_recDataCached.PointData));

	auto pImg = GetLinkedObject(LinkedObjTypeImage);
	if (pImg)
	{
		auto pImgProp = pImg->GetProperties(pEMF);
		auto pImgNode = m_propsCached->AddBranch(L"Image");
		pImgNode->sub = pImgProp->sub;

		auto pAttrRec = GetLinkedObject(LinkedObjTypeImageAttribute);
		if (pAttrRec)
		{
			auto pAttrProp = pAttrRec->GetProperties(pEMF);
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
		AddLinkRecord(pRec);
		if (!(m_recInfo.Flags & OEmfPlusRecDrawString::FlagS))
		{
			auto nID = ((OEmfPlusRecDrawString*)m_recInfo.Data)->BrushId;
			pRec = pEMF->GetObjectCreationRecord(nID, true);
			if (pRec)
				AddLinkRecord(pRec);
		}
	}
}

void EMFRecAccessGDIPlusRecDrawString::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(pEMF);

	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));

	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawString::FlagObjectIDMask);
	m_propsCached->AddValue(L"FontID", nID);
	if (m_recInfo.Flags & OEmfPlusRecDrawString::FlagS)
	{
		m_propsCached->sub.emplace_back(std::make_shared<PropertyNodeColor>(L"Brush", (emfplus::OEmfPlusARGB&)m_recDataCached.BrushId));
	}
	else
	{
		auto pBrush = GetLinkedObject(LinkedObjTypeBrush);
		if (pBrush)
		{
			auto pProp = pBrush->GetProperties(pEMF);
			auto pNode = m_propsCached->AddBranch(L"Brush");
			pNode->sub = pProp->sub;
		}
	}
	// TODO, format id
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusRectF>(L"LayoutRect", m_recDataCached.LayoutRect));
	m_propsCached->AddText(L"StringData", m_recDataCached.StringData.data());
}

void EMFRecAccessGDIPlusRecDrawDriverString::Preprocess(EMFAccess* pEMF)
{
	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawDriverString::FlagObjectIDMask);
	auto pRec = pEMF->GetObjectCreationRecord(nID, true);
	if (pRec)
	{
		AddLinkRecord(pRec);
		if (!(m_recInfo.Flags & OEmfPlusRecDrawDriverString::FlagS))
		{
			auto nID = ((OEmfPlusRecDrawDriverString*)m_recInfo.Data)->BrushId;
			pRec = pEMF->GetObjectCreationRecord(nID, true);
			if (pRec)
				AddLinkRecord(pRec);
		}
	}
}

void EMFRecAccessGDIPlusRecDrawDriverString::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusDrawingCat::CacheProperties(pEMF);

	DataReader reader(m_recInfo.Data, m_recInfo.DataSize);
	VERIFY(m_recDataCached.Read(reader, m_recInfo.Flags, m_recInfo.DataSize));

	auto nID = (u8t)(m_recInfo.Flags & OEmfPlusRecDrawDriverString::FlagObjectIDMask);
	m_propsCached->AddValue(L"FontID", nID);
	if (m_recInfo.Flags & OEmfPlusRecDrawString::FlagS)
	{
		m_propsCached->sub.emplace_back(std::make_shared<PropertyNodeColor>(L"Brush", (emfplus::OEmfPlusARGB&)m_recDataCached.BrushId));
	}
	else
	{
		auto pBrush = GetLinkedObject(LinkedObjTypeBrush);
		if (pBrush)
		{
			auto pProp = pBrush->GetProperties(pEMF);
			auto pNode = m_propsCached->AddBranch(L"Brush");
			pNode->sub = pProp->sub;
		}
	}
	m_propsCached->AddValue(L"DriverStringOptionsFlags", m_recDataCached.DriverStringOptionsFlags, true);
	m_propsCached->AddValue(L"MatrixPresent", m_recDataCached.MatrixPresent);
	// TODO, Glyphs, GlyphPos
	if (m_recDataCached.MatrixPresent)
	{
		m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusTransform>(L"TransformMatrix", m_recDataCached.TransformMatrix));
	}
}

void EMFRecAccessGDIPlusRecSetRenderingOrigin::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusPropertyCat::CacheProperties(pEMF);
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

void EMFRecAccessGDIPlusRecSetAntiAliasMode::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusPropertyCat::CacheProperties(pEMF);
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

void EMFRecAccessGDIPlusRecSetTextRenderingHint::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusPropertyCat::CacheProperties(pEMF);
	auto val = OEmfPlusRecSetTextRenderingHint::GetTextRenderingHint(m_recInfo.Flags);
	m_propsCached->AddText(L"TextRenderingHint", EMFPlusTextRenderingHintText(val));
}

void EMFRecAccessGDIPlusRecSetTextContrast::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusPropertyCat::CacheProperties(pEMF);
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

void EMFRecAccessGDIPlusRecSetInterpolationMode::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusPropertyCat::CacheProperties(pEMF);
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

void EMFRecAccessGDIPlusRecSetPixelOffsetMode::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusPropertyCat::CacheProperties(pEMF);
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

void EMFRecAccessGDIPlusRecSetCompositingMode::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusPropertyCat::CacheProperties(pEMF);
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

void EMFRecAccessGDIPlusRecSetCompositingQuality::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusPropertyCat::CacheProperties(pEMF);
	auto val = OEmfPlusRecSetCompositingQuality::GetCompositingQuality(m_recInfo.Flags);
	m_propsCached->AddText(L"CompositingQuality", EMFPlusCompositingQualityText(val));
}

void EMFRecAccessGDIPlusRecSave::Preprocess(EMFAccess* pEMF)
{
	//auto pRec = (OEmfPlusRecSave*)m_recInfo.Data;
	// TODO
}

void EMFRecAccessGDIPlusRecSave::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusStateCat::CacheProperties(pEMF);
	auto pRec = (OEmfPlusRecSave*)m_recInfo.Data;
	m_propsCached->AddValue(L"StackIndex", pRec->StackIndex);
}

void EMFRecAccessGDIPlusRecRestore::Preprocess(EMFAccess* pEMF)
{
	//auto pRec = (OEmfPlusRecRestore*)m_recInfo.Data;
	// TODO
}

void EMFRecAccessGDIPlusRecRestore::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusStateCat::CacheProperties(pEMF);
	auto pRec = (OEmfPlusRecRestore*)m_recInfo.Data;
	m_propsCached->AddValue(L"StackIndex", pRec->StackIndex);
}

void EMFRecAccessGDIPlusRecBeginContainer::Preprocess(EMFAccess* pEMF)
{
	//auto pRec = (OEmfPlusRecBeginContainer*)m_recInfo.Data;
	// TODO
}

void EMFRecAccessGDIPlusRecBeginContainer::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusStateCat::CacheProperties(pEMF);
	auto pRec = (OEmfPlusRecBeginContainer*)m_recInfo.Data;
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusRectF>(L"DestRect", pRec->DestRect));
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusRectF>(L"SrcRect", pRec->SrcRect));
	m_propsCached->AddValue(L"StackIndex", pRec->StackIndex);
}

void EMFRecAccessGDIPlusRecBeginContainerNoParams::Preprocess(EMFAccess* pEMF)
{
	//auto pRec = (OEmfPlusRecBeginContainerNoParams*)m_recInfo.Data;
	// TODO
}

void EMFRecAccessGDIPlusRecBeginContainerNoParams::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusStateCat::CacheProperties(pEMF);
	auto pRec = (OEmfPlusRecBeginContainerNoParams*)m_recInfo.Data;
	m_propsCached->AddValue(L"StackIndex", pRec->StackIndex);
}

void EMFRecAccessGDIPlusRecEndContainer::Preprocess(EMFAccess* pEMF)
{
	//auto pRec = (OEmfPlusRecEndContainer*)m_recInfo.Data;
	// TODO
}

void EMFRecAccessGDIPlusRecEndContainer::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusStateCat::CacheProperties(pEMF);
	auto pRec = (OEmfPlusRecEndContainer*)m_recInfo.Data;
	m_propsCached->AddValue(L"StackIndex", pRec->StackIndex);
}


void EMFRecAccessGDIPlusRecSetWorldTransform::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusTransformCat::CacheProperties(pEMF);
	auto* pRec = (OEmfPlusRecSetWorldTransform*)m_recInfo.Data;
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusTransform>(L"MatrixData", pRec->MatrixData));
}

void EMFRecAccessGDIPlusRecMultiplyWorldTransform::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusTransformCat::CacheProperties(pEMF);
	auto* pRec = (OEmfPlusRecMultiplyWorldTransform*)m_recInfo.Data;
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusTransform>(L"MatrixData", pRec->MatrixData));
}

void EMFRecAccessGDIPlusRecTranslateWorldTransform::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusTransformCat::CacheProperties(pEMF);
	auto* pRec = (OEmfPlusRecTranslateWorldTransform*)m_recInfo.Data;
	m_propsCached->AddValue(L"dx", pRec->dx);
	m_propsCached->AddValue(L"dy", pRec->dy);
}

void EMFRecAccessGDIPlusRecScaleWorldTransform::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusTransformCat::CacheProperties(pEMF);
	auto* pRec = (OEmfPlusRecScaleWorldTransform*)m_recInfo.Data;
	m_propsCached->AddValue(L"Sx", pRec->Sx);
	m_propsCached->AddValue(L"Sy", pRec->Sy);
}

void EMFRecAccessGDIPlusRecRotateWorldTransform::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusTransformCat::CacheProperties(pEMF);
	auto* pRec = (OEmfPlusRecRotateWorldTransform*)m_recInfo.Data;
	m_propsCached->AddValue(L"Angle", pRec->Angle);
}

void EMFRecAccessGDIPlusRecSetPageTransform::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusTransformCat::CacheProperties(pEMF);
	auto* pRec = (OEmfPlusRecSetPageTransform*)m_recInfo.Data;
	m_propsCached->AddValue(L"PageScale", pRec->PageScale);
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
	if (!m_EMFRecords.empty())
		return true;
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

bool EMFAccess::SaveToFile(LPCWSTR szPath) const
{
	if (!m_pMetafile || !szPath)
		return false;
	{
		CClientDC dc(nullptr);
		Gdiplus::Metafile mf(szPath, dc.GetSafeHdc());
		Gdiplus::Graphics gg(&mf);
		gg.DrawImage(m_pMetafile.get(), 0, 0);
	}
	return true;
}

