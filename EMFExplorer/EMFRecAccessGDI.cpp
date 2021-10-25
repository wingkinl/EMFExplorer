#include "pch.h"
#include "framework.h"
#include "EMFRecAccessGDI.h"
#include "EMFAccess.h"
#include "EMFStruct2Props.h"

const ENHMETARECORD* EMFRecAccessGDIRec::GetGDIRecord(const emfplus::OEmfPlusRecInfo& rec)
{
	if (rec.Data)
	{
		// I can't find any document for this, but it seems that this is the way to go (based on observation)
		auto pRec = (const ENHMETARECORD*)(rec.Data - sizeof(EMR));
		return pRec;
	}
	return nullptr;
}

void EMFRecAccessGDIRecHeader::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIControlCat::CacheProperties(ctxt);
	auto pRec = (const ENHMETAHEADER*)GetGDIRecord(m_recInfo);
	if (!pRec)
	{
		ASSERT(0);
		return;
	}
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
	auto& hdr = ctxt.pEMF->GetMetafileHeader();
	GetPropertiesFromGDIPlusHeader(pPlusNode.get(), hdr);
}

void EMFRecAccessGDIRecRestoreDC::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMRRESTOREDC*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	auto pSaveRec = pEMF->GetGDISaveRecord(pRec->iRelative);
	if (pSaveRec)
	{
		AddLinkRecord(pSaveRec, LinkedObjTypeGraphicState, LinkedObjTypeGraphicState);
	}
}

void EMFRecAccessGDIRecRestoreDC::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIStateCat::CacheProperties(ctxt);
	auto pRec = (EMRRESTOREDC*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		m_propsCached->AddValue(L"iRelative", pRec->iRelative);
	}
}

void EMFRecAccessGDIRecSelectObject::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIObjManipulationCat::CacheProperties(ctxt);
	auto pRec = (EMRSELECTOBJECT*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		m_propsCached->AddValue(L"ihObject", pRec->ihObject);
	}
}

void EMFRecAccessGDIRecCreatePen::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIObjectCat::CacheProperties(ctxt);
	auto pRec = (EMRCREATEPEN*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecCreateBrushIndirect::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIObjectCat::CacheProperties(ctxt);
	auto pRec = (EMRCREATEBRUSHINDIRECT*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecDeleteObject::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIObjManipulationCat::CacheProperties(ctxt);
	auto pRec = (EMRDELETEOBJECT*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		m_propsCached->AddValue(L"ihObject", pRec->ihObject);
	}
}

void EMFRecAccessGDIRecGdiComment::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIRec::CacheProperties(ctxt);
	auto pRec = (const EMRCOMMENT_BASE*)GetGDIRecord(m_recInfo);
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
			char szBuffer[100]{ 0 };
			strncpy_s(szBuffer, _countof(szBuffer), (char*)&pRec->CommentIdentifier, _countof(szBuffer));
			strText = szBuffer;
		}
		break;
	}
	m_propsCached->AddText(L"CommentType", str);
	if (!strText.IsEmpty())
		m_propsCached->AddText(L"Text", strText);
}

void EMFRecAccessGDIRecExtCreatePen::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIObjectCat::CacheProperties(ctxt);
	auto pRec = (EMREXTCREATEPEN*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}
