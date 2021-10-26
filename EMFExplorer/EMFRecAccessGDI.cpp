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

void EMFRecAccessGDIRecSelectObject::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMRSELECTOBJECT*)EMFRecAccessGDIObjectCat::GetGDIRecord(m_recInfo);
	auto pLinkedRec = pEMF->GetObjectCreationRecord(pRec->ihObject, false);
	if (pLinkedRec)
	{
		ASSERT(pLinkedRec->GetRecordCategory() == RecCategoryObject);
		AddLinkRecord(pLinkedRec, LinkedObjTypeObjectUnspecified, LinkedObjTypeObjManipulation);
		// TODO, link drawing records?
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

void EMFRecAccessGDIRecCreatePen::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMRCREATEPEN*)EMFRecAccessGDIObjectCat::GetGDIRecord(m_recInfo);
	pEMF->SetObjectToTable(pRec->ihPen, this, false);
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

void EMFRecAccessGDIRecCreateBrushIndirect::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMRCREATEBRUSHINDIRECT*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	pEMF->SetObjectToTable(pRec->ihBrush, this, false);
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

void EMFRecAccessGDIRecDeleteObject::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMRDELETEOBJECT*)EMFRecAccessGDIObjectCat::GetGDIRecord(m_recInfo);
	auto pLinkedRec = pEMF->GetObjectCreationRecord(pRec->ihObject, false);
	if (pLinkedRec)
	{
		ASSERT(pLinkedRec->GetRecordCategory() == RecCategoryObject);
		AddLinkRecord(pLinkedRec, LinkedObjTypeObjectUnspecified, LinkedObjTypeObjManipulation);
		// TODO, link drawing records?
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

void EMFRecAccessGDIRecSelectPalette::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMRSELECTPALETTE*)EMFRecAccessGDIObjectCat::GetGDIRecord(m_recInfo);
	auto pLinkedRec = pEMF->GetObjectCreationRecord(pRec->ihPal, false);
	if (pLinkedRec)
	{
		ASSERT(pLinkedRec->GetRecordCategory() == RecCategoryObject);
		AddLinkRecord(pLinkedRec, LinkedObjTypePalette, LinkedObjTypeObjManipulation);
		// TODO, link drawing records?
	}
}

void EMFRecAccessGDIRecCreatePalette::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMRCREATEPALETTE*)EMFRecAccessGDIObjectCat::GetGDIRecord(m_recInfo);
	pEMF->SetObjectToTable(pRec->ihPal, this, false);
}

void EMFRecAccessGDIRecSetPaletteEntries::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMRSETPALETTEENTRIES*)EMFRecAccessGDIObjectCat::GetGDIRecord(m_recInfo);
	auto pLinkedRec = pEMF->GetObjectCreationRecord(pRec->ihPal, false);
	if (pLinkedRec)
	{
		AddLinkRecord(pLinkedRec, LinkedObjTypePalette, LinkedObjTypeObjManipulation);
		// TODO, link all drawing records too?
	}
}

void EMFRecAccessGDIRecResizePalette::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMRRESIZEPALETTE*)EMFRecAccessGDIObjectCat::GetGDIRecord(m_recInfo);
	auto pLinkedRec = pEMF->GetObjectCreationRecord(pRec->ihPal, false);
	if (pLinkedRec)
	{
		ASSERT(pLinkedRec->GetRecordCategory() == RecCategoryObject);
		AddLinkRecord(pLinkedRec, LinkedObjTypePalette, LinkedObjTypeObjManipulation);
		// TODO, link drawing records?
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

void EMFRecAccessGDIRecExtCreateFontIndirect::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMREXTCREATEFONTINDIRECTW*)EMFRecAccessGDIObjectCat::GetGDIRecord(m_recInfo);
	pEMF->SetObjectToTable(pRec->ihFont, this, false);
}

void EMFRecAccessGDIRecExtTextOutA::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMREXTTEXTOUTA*)EMFRecAccessGDIObjectCat::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecExtTextOutW::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMREXTTEXTOUTW*)EMFRecAccessGDIObjectCat::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecCreateMonoBrush::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMRCREATEMONOBRUSH*)EMFRecAccessGDIObjectCat::GetGDIRecord(m_recInfo);
	pEMF->SetObjectToTable(pRec->ihBrush, this, false);
}

void EMFRecAccessGDIRecCreateDIBPatternBrushPt::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMRCREATEDIBPATTERNBRUSHPT*)EMFRecAccessGDIObjectCat::GetGDIRecord(m_recInfo);
	pEMF->SetObjectToTable(pRec->ihBrush, this, false);
}

void EMFRecAccessGDIRecExtCreatePen::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMREXTCREATEPEN*)EMFRecAccessGDIObjectCat::GetGDIRecord(m_recInfo);
	pEMF->SetObjectToTable(pRec->ihPen, this, false);
}

void EMFRecAccessGDIRecExtCreatePen::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIObjectCat::CacheProperties(ctxt);
	auto pRec = (EMREXTCREATEPEN*)EMFRecAccessGDIObjectCat::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecCreateColorSpace::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMRCREATECOLORSPACE*)EMFRecAccessGDIObjectCat::GetGDIRecord(m_recInfo);
	pEMF->SetObjectToTable(pRec->ihCS, this, false);
}

void EMFRecAccessGDIRecSetColorSpace::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMRSETCOLORSPACE*)EMFRecAccessGDIObjectCat::GetGDIRecord(m_recInfo);
	auto pLinkedRec = pEMF->GetObjectCreationRecord(pRec->ihCS, false);
	if (pLinkedRec)
	{
		AddLinkRecord(pLinkedRec, LinkedObjTypeColorspace, LinkedObjTypeObjManipulation);
		// TODO, link all drawing records too?
	}
}

void EMFRecAccessGDIRecDeleteColorSpace::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMRDELETECOLORSPACE*)EMFRecAccessGDIObjectCat::GetGDIRecord(m_recInfo);
	auto pLinkedRec = pEMF->GetObjectCreationRecord(pRec->ihCS, false);
	if (pLinkedRec)
	{
		AddLinkRecord(pLinkedRec, LinkedObjTypeColorspace, LinkedObjTypeObjManipulation);
		// TODO, link all drawing records too?
	}
}

void EMFRecAccessGDIRecColorCorrectPalette::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMRCOLORCORRECTPALETTE*)EMFRecAccessGDIObjectCat::GetGDIRecord(m_recInfo);
	auto pLinkedRec = pEMF->GetObjectCreationRecord(pRec->ihPalette, false);
	if (pLinkedRec)
		AddLinkRecord(pLinkedRec, LinkedObjTypePalette, LinkedObjTypeObjManipulation);
}

void EMFRecAccessGDIRecCreateColorSpaceW::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMRCREATECOLORSPACEW*)EMFRecAccessGDIObjectCat::GetGDIRecord(m_recInfo);
	pEMF->SetObjectToTable(pRec->ihCS, this, false);
}

