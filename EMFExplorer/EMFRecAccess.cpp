#include "pch.h"
#include "framework.h"
#include "EMFRecAccess.h"
#include "EMFAccess.h"

using namespace emfplus;

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
	m_propsCached->AddText(L"RecordName", GetRecordName());
	m_propsCached->AddValue(L"RecordType", m_recInfo.Type);
	m_propsCached->AddValue(L"RecordDataSize", m_recInfo.DataSize);
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
	m_propsCached->AddValue(L"RecordFlags", m_recInfo.Flags, true);
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

static inline LPCWSTR EMFPlusImageDataTypeText(OImageDataType type)
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

static inline LPCWSTR EMFPlusUnitTypeText(OUnitType type)
{
	static const LPCWSTR aText[] = {
		L"World", L"Display", L"Pixel", L"Point", L"Inch", L"Document", L"Millimeter"
	};
	return aText[(int)type];
}

static inline LPCWSTR EMFPlusLineJoinTypeText(OLineJoinType type)
{
	static const LPCWSTR aText[] = {
		L"Miter", L"Bevel", L"Round", L"MiterClipped"
	};
	return aText[(int)type];
}

static inline LPCWSTR EMFPlusLineStyleText(OLineStyle type)
{
	static const LPCWSTR aText[] = {
		L"Solid", L"Dash", L"Dot", L"DashDot", L"DashDotDot", L"Custom"
	};
	return aText[(int)type];
}

static inline LPCWSTR EMFPlusDashedLineCapText(ODashedLineCap type)
{
	static const LPCWSTR aText[] = {
		L"Flat", L"Invalid", L"Round", L"Triangle"
	};
	return aText[(int)type];
}

static inline LPCWSTR EMFPlusPenAlignmentText(OPenAlignment type)
{
	static const LPCWSTR aText[] = {
		L"Center", L"Inset", L"Left", L"Outset", L"Right"
	};
	return aText[(int)type];
}

static inline LPCWSTR EMFPlusCustomLineCapDataTypeText(OCustomLineCapDataType type)
{
	static const LPCWSTR aText[] = {
		L"Default", L"AdjustableArrow"
	};
	return aText[(int)type];
}

static inline LPCWSTR EMFPlusStringAlignmentText(OStringAlignment type)
{
	static const LPCWSTR aText[] = {
		L"Near", L"Center", L"Far"
	};
	return aText[(int)type];
}

static inline LPCWSTR EMFPlusStringDigitSubstitutionText(OStringDigitSubstitution type)
{
	static const LPCWSTR aText[] = {
		L"User", L"None", L"National", L"Traditional"
	};
	return aText[(int)type];
}

static inline LPCWSTR EMFPlusHotkeyPrefixText(OHotkeyPrefix type)
{
	static const LPCWSTR aText[] = {
		L"None", L"Show", L"Hide"
	};
	return aText[(int)type];
}

static inline LPCWSTR EMFPlusStringTrimmingText(OStringTrimming type)
{
	static const LPCWSTR aText[] = {
		L"None", L"Character", L"Word", L"EllipsisCharacter", L"EllipsisWord", L"EllipsisPath"
	};
	return aText[(int)type];
}

static inline LPCWSTR EMFPlusImageAttributesClampTypeText(OEmfPlusImageAttributes::ClampType type)
{
	static const LPCWSTR aText[] = {
		L"Rect", L"Bitmap"
	};
	return aText[(int)type];
}


#define GS_VISITABLE_STRUCT		VISITABLE_STRUCT
#include "visit_struct.hpp"
#include "EmfStructVisit.h"

struct EmfStruct2Properties 
{
	template <typename ValT>
	static inline void Build(const ValT& obj, PropertyNode* pNode)
	{
		visit_struct::for_each(obj, [&](const char* name, auto& value)
			{
				BuildField(name, value, pNode);
			});
	}

#if __cpp_if_constexpr
	template <typename ValT>
	static inline void BuildField(const char* name, const ValT& value, PropertyNode* pNode)
	{
		if constexpr (data_access::is_optional_wrapper_v<ValT>)
		{
			if (value)
				BuildField(name, value.get(), pNode);
		}
		else if constexpr (std::is_class_v<ValT>)
		{
			if constexpr (data_access::is_vector_object_wrapper<ValT>::value)
			{
				if constexpr (std::is_arithmetic_v<typename ValT::value_type>)
				{
					if (!value.empty())
						pNode->AddValue(L"junk", value[0]);
				}
			}
			else if constexpr (data_access::is_object_wrapper<ValT>::value)
			{
				auto pBranchNode = pNode->AddBranch(CStringW(name));
				if (pBranchNode)
					Build(value.get(), pBranchNode.get());
			}
			else
			{
				auto pBranchNode = pNode->AddBranch(CStringW(name));
				if (pBranchNode)
					Build(value, pBranchNode.get());
			}
		}
		else if constexpr (std::is_arithmetic_v<ValT>)
		{
			CStringW str(name);
			pNode->AddValue(str, value);
		}
		else if constexpr (std::is_enum_v<ValT>)
		{
			CStringW str(name);
			pNode->AddText(str, GetEnumText(value));
		}
	}
#else
	template <typename ValT, typename std::enable_if_t<std::is_class<ValT>::value>* = nullptr>
	static inline void BuildField(const char* name, const ValT& value, PropertyNode* pNode)
	{
		//EmfStruct2Properties::Build(value, pNode);
	}

	template <typename ValT, typename std::enable_if_t<std::is_arithmetic<ValT>::value>* = nullptr>
	static inline void BuildField(const char* name, const ValT& value, PropertyNode* pNode)
	{
		CStringW str(name);
		pNode->AddValue(str, value);
	}

	template <typename ValT, typename std::enable_if_t<std::is_enum<ValT>::value>* = nullptr>
	static inline void BuildField(const char* name, const ValT& value, PropertyNode* pNode)
	{
		CStringW str(name);
		pNode->AddText(str, GetEnumText(value));
	}

	template <typename ValT>
	static inline void BuildField(const char* name, const optional_wrapper<ValT>& value, PropertyNode* pNode)
	{
		if (value)
			BuildField(name, value.get(), pNode);
	}
#endif
	template <typename ValT>
	static inline void BuildField(const char* name, const std::unique_ptr<ValT>& value, PropertyNode* pNode)
	{
		if (value)
			BuildField(name, *value, pNode);
	}

	static inline void BuildField(const char* name, const OEmfPlusARGB& value, PropertyNode* pNode)
	{
		pNode->sub.emplace_back(std::make_shared<PropertyNodeColor>(CStringW(name), value));
	}

	static inline void BuildField(const char* name, const OEmfPlusPointDataArray& value, PropertyNode* pNode)
	{
	}

	static inline void BuildField(const char* name, const std::wstring& value, PropertyNode* pNode)
	{
		pNode->AddText(CStringW(name), value.c_str());
	}

	static inline LPCWSTR GetEnumText(OBrushType value)
	{
		return EMFPlusBrushTypeText(value);
	}
	static inline LPCWSTR GetEnumText(OHatchStyle value)
	{
		return EMFPlusHatchStyleText(value);
	}
	static inline LPCWSTR GetEnumText(OWrapMode value)
	{
		return EMFPlusWrapModeText(value);
	}
	static inline LPCWSTR GetEnumText(OImageDataType value)
	{
		return EMFPlusImageDataTypeText(value);
	}
	static inline LPCWSTR GetEnumText(OBitmapDataType value)
	{
		return EMFPlusBitmapTypeText(value);
	}
	static inline LPCWSTR GetEnumText(OMetafileDataType value)
	{
		return EMFPlusMetafileTypeText(value);
	}
	static inline LPCWSTR GetEnumText(OUnitType value)
	{
		return EMFPlusUnitTypeText(value);
	}
	static inline LPCWSTR GetEnumText(OLineJoinType value)
	{
		return EMFPlusLineJoinTypeText(value);
	}
	static inline LPCWSTR GetEnumText(OLineStyle value)
	{
		return EMFPlusLineStyleText(value);
	}
	static inline LPCWSTR GetEnumText(ODashedLineCap value)
	{
		return EMFPlusDashedLineCapText(value);
	}
	static inline LPCWSTR GetEnumText(OPenAlignment value)
	{
		return EMFPlusPenAlignmentText(value);
	}
	static inline LPCWSTR GetEnumText(OCustomLineCapDataType value)
	{
		return EMFPlusCustomLineCapDataTypeText(value);
	}
	static inline LPCWSTR GetEnumText(OStringAlignment value)
	{
		return EMFPlusStringAlignmentText(value);
	}
	static inline LPCWSTR GetEnumText(OStringDigitSubstitution value)
	{
		return EMFPlusStringDigitSubstitutionText(value);
	}
	static inline LPCWSTR GetEnumText(OHotkeyPrefix value)
	{
		return EMFPlusHotkeyPrefixText(value);
	}
	static inline LPCWSTR GetEnumText(OStringTrimming value)
	{
		return EMFPlusStringTrimmingText(value);
	}
	static inline LPCWSTR GetEnumText(OEmfPlusImageAttributes::ClampType value)
	{
		return EMFPlusImageAttributesClampTypeText(value);
	}
	static inline CStringW GetEnumText(OLineCapType value)
	{
		return GetEnumTextAsHex((u32t)value);
	}
	static inline CStringW GetEnumText(OPixelFormat value)
	{
		return GetEnumTextAsHex((u32t)value);
	}
	static inline CStringW GetEnumText(OBrushData value)
	{
		return GetEnumTextAsHex((u32t)value);
	}
	static inline CStringW GetEnumTextAsHex(u32t value)
	{
		CStringW str;
		str.Format(L"%08X", value);
		return str;
	}
};

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
	void CacheProperties(EMFAccess* pEMF, PropertyNode* pNode) const override
	{
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
	void CacheProperties(EMFAccess* pEMF, PropertyNode* pNode) const override
	{
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
	void CacheProperties(EMFAccess* pEMF, PropertyNode* pNode) const override
	{
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
	void CacheProperties(EMFAccess* pEMF, PropertyNode* pNode) const override
	{
		auto pImg = (OEmfPlusImage*)m_obj.get();
		//pNode->AddText(L"ImageType", EMFPlusImageDataTypeText(pImg->Type));
		//switch (pImg->Type)
		//{
		//case OImageDataType::Bitmap:
		//	pNode->AddValue(L"Width", pImg->ImageDataBmp->Width);
		//	pNode->AddValue(L"Height", pImg->ImageDataBmp->Height);
		//	pNode->AddValue(L"Stride", pImg->ImageDataBmp->Stride);
		//	pNode->AddValue(L"PixelFormat", (u32t)pImg->ImageDataBmp->PixelFormat, true);
		//	pNode->AddText(L"BitmapType", EMFPlusBitmapTypeText(pImg->ImageDataBmp->Type));
		//	break;
		//case OImageDataType::Metafile:
		//	pNode->AddText(L"MetafileType", EMFPlusMetafileTypeText(pImg->ImageDataMetafile->Type));
		//	pNode->AddValue(L"MetafileDataSize", pImg->ImageDataMetafile->MetafileDataSize);
		//	break;
		//}
		auto pBranch = pNode->AddBranch(L"Image");
		EmfStruct2Properties::Build(*pImg, pBranch.get());
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
			m_emf = std::make_shared<EMFAccess>(pImg->ImageDataMetafile->MetafileData);
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
	void CacheProperties(EMFAccess* pEMF, PropertyNode* pNode) const override
	{
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
	void CacheProperties(EMFAccess* pEMF, PropertyNode* pNode) const override
	{
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
	void CacheProperties(EMFAccess* pEMF, PropertyNode* pNode) const override
	{
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

void EMFRecAccessGDIPlusRecSave::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusStateCat::CacheProperties(pEMF);
	auto pRec = (OEmfPlusRecSave*)m_recInfo.Data;
	m_propsCached->AddValue(L"StackIndex", pRec->StackIndex);
}

void EMFRecAccessGDIPlusRecRestore::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (OEmfPlusRecRestore*)m_recInfo.Data;
	auto pSaveRec = pEMF->GetPlusSaveRecord(pRec->StackIndex);
	if (pSaveRec)
	{
		AddLinkRecord(pSaveRec);
	}
}

void EMFRecAccessGDIPlusRecRestore::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusStateCat::CacheProperties(pEMF);
	auto pRec = (OEmfPlusRecRestore*)m_recInfo.Data;
	m_propsCached->AddValue(L"StackIndex", pRec->StackIndex);
}

void EMFRecAccessGDIPlusRecBeginContainer::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusStateCat::CacheProperties(pEMF);
	auto pRec = (OEmfPlusRecBeginContainer*)m_recInfo.Data;
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusRectF>(L"DestRect", pRec->DestRect));
	m_propsCached->sub.emplace_back(std::make_shared<PropertyNodePlusRectF>(L"SrcRect", pRec->SrcRect));
	m_propsCached->AddValue(L"StackIndex", pRec->StackIndex);
}

void EMFRecAccessGDIPlusRecBeginContainerNoParams::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccessGDIPlusStateCat::CacheProperties(pEMF);
	auto pRec = (OEmfPlusRecBeginContainerNoParams*)m_recInfo.Data;
	m_propsCached->AddValue(L"StackIndex", pRec->StackIndex);
}

void EMFRecAccessGDIPlusRecEndContainer::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (OEmfPlusRecEndContainer*)m_recInfo.Data;
	auto pSaveRec = pEMF->GetPlusSaveRecord(pRec->StackIndex);
	if (pSaveRec)
	{
		AddLinkRecord(pSaveRec);
	}
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

