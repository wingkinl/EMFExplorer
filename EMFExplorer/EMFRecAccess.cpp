#include "pch.h"
#include "framework.h"
#include "EMFRecAccess.h"
#include "EMFAccess.h"
#include "EMFStruct2Props.h"

using namespace emfplus;

bool EMFRecAccess::IsDrawingRecord() const
{
	auto cate = GetRecordCategory();
	return cate == RecCategoryDrawing || cate == RecCategoryBitmap;
}

std::shared_ptr<PropertyNode> EMFRecAccess::GetProperties(const CachePropertiesContext& ctxt)
{
	if (!m_propsCached)
	{
		m_propsCached = std::make_shared<PropertyNode>();
		CacheProperties(ctxt);
	}
	return m_propsCached;
}

auto EMFRecAccess::IsLinked(const EMFRecAccess* pRec) const -> LinkedObjType
{
	for (auto& link : m_linkRecs)
	{
		if (link.pRec == pRec)
			return link.nType;
	}
	return LinkedObjTypeInvalid;
}

void EMFRecAccess::CacheProperties(const CachePropertiesContext& ctxt)
{
	m_propsCached->AddText(L"RecordName", GetRecordName());
	m_propsCached->AddValue(L"RecordType", m_recInfo.Type);
	m_propsCached->AddValue(L"RecordDataSize", m_recInfo.DataSize);
}

void EMFRecAccess::AddLinkRecord(EMFRecAccess* pRec, LinkedObjType nType, LinkedObjType nTypeThis)
{
	m_linkRecs.emplace_back(pRec, nType);
	if (nTypeThis != LinkedObjTypeInvalid)
		pRec->AddLinkRecord(this, nTypeThis, LinkedObjTypeInvalid);
}

void GetPropertiesFromGDIPlusHeader(PropertyNode* pNode, const Gdiplus::MetafileHeader& hdr)
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



