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
		if (!m_linkRecs.empty())
		{
			auto pLinkBranch = m_propsCached->AddBranch(L"LinkedRecords");
			int count = 0;
			for (auto& link : m_linkRecs)
			{
				CStringW strName, strText;
				strName.Format(L"[%d]", count++);
				strText.Format(L"#%zu %s", link.pRec->GetIndex()+1, link.pRec->GetRecordName());
				pLinkBranch->AddText(strName, strText);
			}
		}
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

void EMFRecAccess::SetRecInfo(const emfplus::OEmfPlusRecInfo& info)
{
	m_recInfo = info;
	if (info.Data && info.DataSize)
	{
		m_recData.assign(info.Data, info.Data+info.DataSize);
		m_recInfo.Data = m_recData.data();
	}
}

void EMFRecAccess::CacheProperties(const CachePropertiesContext& ctxt)
{
	m_propsCached->AddText(L"RecordName", GetRecordName());
	m_propsCached->AddValue(L"RecordType", m_recInfo.Type);
	m_propsCached->AddValue(L"RecordDataSize", m_recInfo.DataSize);
}

void EMFRecAccess::AddLinkRecord(EMFRecAccess* pRec, LinkedObjType nType, LinkedObjType nTypeThis)
{
	LinkedObjInfo link{pRec, nType};
	m_linkRecs.emplace_back(link);
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



