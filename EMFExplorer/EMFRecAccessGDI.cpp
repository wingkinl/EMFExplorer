#include "pch.h"
#include "framework.h"
#include "EMFRecAccessGDI.h"
#include "EMFAccess.h"
#include "EMFStruct2Props.h"

#undef min
#undef max

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

void EMFRecAccessGDIRecExcludeClipRect::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIClippingCat::CacheProperties(ctxt);
	auto pRec = (EMREXCLUDECLIPRECT*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
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
	auto pRec = (EMRSELECTOBJECT*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec && !(pRec->ihObject & ENHMETA_STOCK_OBJECT))
	{
		auto pLinkedRec = pEMF->GetObjectCreationRecord(pRec->ihObject, false);
		if (pLinkedRec)
		{
			ASSERT(pLinkedRec->GetRecordCategory() == RecCategoryObject);
			AddLinkRecord(pLinkedRec, LinkedObjTypeObjectUnspecified, LinkedObjTypeObjManipulation);
			// TODO, link drawing records?
		}
	}
}

void EMFRecAccessGDIRecSelectObject::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIObjManipulationCat::CacheProperties(ctxt);
	auto pRec = (EMRSELECTOBJECT*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (!pRec)
		return;
	if (pRec->ihObject & ENHMETA_STOCK_OBJECT)
	{
		auto hObj = pRec->ihObject & ~ENHMETA_STOCK_OBJECT;
		m_propsCached->AddValue(L"ihObject", pRec->ihObject, true);
	}
	else
	{
		m_propsCached->AddValue(L"ihObject", pRec->ihObject);
	}
}

void EMFRecAccessGDIRecCreatePen::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMRCREATEPEN*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
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
	auto pRec = (EMRDELETEOBJECT*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
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
	auto pRec = (EMRSELECTPALETTE*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	auto pLinkedRec = pEMF->GetObjectCreationRecord(pRec->ihPal, false);
	if (pLinkedRec)
	{
		ASSERT(pLinkedRec->GetRecordCategory() == RecCategoryObject);
		AddLinkRecord(pLinkedRec, LinkedObjTypePalette, LinkedObjTypeObjManipulation);
		// TODO, link drawing records?
	}
}

void EMFRecAccessGDIRecSelectPalette::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIObjManipulationCat::CacheProperties(ctxt);
	auto pRec = (EMRSELECTPALETTE*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecCreatePalette::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMRCREATEPALETTE*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	pEMF->SetObjectToTable(pRec->ihPal, this, false);
}

void EMFRecAccessGDIRecCreatePalette::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIObjectCat::CacheProperties(ctxt);
	auto pRec = (EMRCREATEPALETTE*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecSetPaletteEntries::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMRSETPALETTEENTRIES*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	auto pLinkedRec = pEMF->GetObjectCreationRecord(pRec->ihPal, false);
	if (pLinkedRec)
	{
		AddLinkRecord(pLinkedRec, LinkedObjTypePalette, LinkedObjTypeObjManipulation);
		// TODO, link all drawing records too?
	}
}

void EMFRecAccessGDIRecSetPaletteEntries::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIObjManipulationCat::CacheProperties(ctxt);
	auto pRec = (EMRSETPALETTEENTRIES*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecResizePalette::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMRRESIZEPALETTE*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	auto pLinkedRec = pEMF->GetObjectCreationRecord(pRec->ihPal, false);
	if (pLinkedRec)
	{
		ASSERT(pLinkedRec->GetRecordCategory() == RecCategoryObject);
		AddLinkRecord(pLinkedRec, LinkedObjTypePalette, LinkedObjTypeObjManipulation);
		// TODO, link drawing records?
	}
}

void EMFRecAccessGDIRecResizePalette::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIObjManipulationCat::CacheProperties(ctxt);
	auto pRec = (EMRRESIZEPALETTE*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
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

void EMFRecAccessGDIRecBitBlt::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIBitmapCat::CacheProperties(ctxt);
	auto pRec = (EMRBITBLT*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecStretchBlt::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIBitmapCat::CacheProperties(ctxt);
	auto pRec = (EMRSTRETCHBLT*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecMaskBlt::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIBitmapCat::CacheProperties(ctxt);
	auto pRec = (EMRMASKBLT*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecPlgBlt::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIBitmapCat::CacheProperties(ctxt);
	auto pRec = (EMRPLGBLT*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecSetDIBitsToDevice::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIBitmapCat::CacheProperties(ctxt);
	auto pRec = (EMRSETDIBITSTODEVICE*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecStretchDIBits::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIBitmapCat::CacheProperties(ctxt);
	auto pRec = (EMRSTRETCHDIBITS*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecExtCreateFontIndirect::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMREXTCREATEFONTINDIRECTW*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	pEMF->SetObjectToTable(pRec->ihFont, this, false);
}

void EMFRecAccessGDIRecExtCreateFontIndirect::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIObjectCat::CacheProperties(ctxt);
	auto pRec = (EMREXTCREATEFONTINDIRECTW*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecExtTextOutA::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMREXTTEXTOUTA*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecExtTextOutW::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMREXTTEXTOUTW*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecCreateMonoBrush::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMRCREATEMONOBRUSH*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	pEMF->SetObjectToTable(pRec->ihBrush, this, false);
}

void EMFRecAccessGDIRecCreateMonoBrush::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIObjectCat::CacheProperties(ctxt);
	auto pRec = (EMRCREATEMONOBRUSH*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecCreateDIBPatternBrushPt::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMRCREATEDIBPATTERNBRUSHPT*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	pEMF->SetObjectToTable(pRec->ihBrush, this, false);
}

void EMFRecAccessGDIRecCreateDIBPatternBrushPt::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIObjectCat::CacheProperties(ctxt);
	auto pRec = (EMRCREATEDIBPATTERNBRUSHPT*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecExtCreatePen::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMREXTCREATEPEN*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	pEMF->SetObjectToTable(pRec->ihPen, this, false);
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

void EMFRecAccessGDIRecCreateColorSpace::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMRCREATECOLORSPACE*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	pEMF->SetObjectToTable(pRec->ihCS, this, false);
}

void EMFRecAccessGDIRecCreateColorSpace::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIObjectCat::CacheProperties(ctxt);
	auto pRec = (EMRCREATECOLORSPACE*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecSetColorSpace::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMRSETCOLORSPACE*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	auto pLinkedRec = pEMF->GetObjectCreationRecord(pRec->ihCS, false);
	if (pLinkedRec)
	{
		AddLinkRecord(pLinkedRec, LinkedObjTypeColorspace, LinkedObjTypeObjManipulation);
		// TODO, link all drawing records too?
	}
}

void EMFRecAccessGDIRecSetColorSpace::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIObjManipulationCat::CacheProperties(ctxt);
	auto pRec = (EMRSETCOLORSPACE*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecDeleteColorSpace::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMRDELETECOLORSPACE*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	auto pLinkedRec = pEMF->GetObjectCreationRecord(pRec->ihCS, false);
	if (pLinkedRec)
	{
		AddLinkRecord(pLinkedRec, LinkedObjTypeColorspace, LinkedObjTypeObjManipulation);
		// TODO, link all drawing records too?
	}
}

void EMFRecAccessGDIRecDeleteColorSpace::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIObjManipulationCat::CacheProperties(ctxt);
	auto pRec = (EMRDELETECOLORSPACE*)EMFRecAccessGDIObjectCat::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecColorCorrectPalette::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMRCOLORCORRECTPALETTE*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	auto pLinkedRec = pEMF->GetObjectCreationRecord(pRec->ihPalette, false);
	if (pLinkedRec)
		AddLinkRecord(pLinkedRec, LinkedObjTypePalette, LinkedObjTypeObjManipulation);
}

void EMFRecAccessGDIRecColorCorrectPalette::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIObjManipulationCat::CacheProperties(ctxt);
	auto pRec = (EMRCOLORCORRECTPALETTE*)EMFRecAccessGDIObjectCat::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecAlphaBlend::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIBitmapCat::CacheProperties(ctxt);
	auto pRec = (EMRALPHABLEND*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecTransparentBlt::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIBitmapCat::CacheProperties(ctxt);
	auto pRec = (EMRTRANSPARENTBLT*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecCreateColorSpaceW::Preprocess(EMFAccess* pEMF)
{
	auto pRec = (EMRCREATECOLORSPACEW*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	pEMF->SetObjectToTable(pRec->ihCS, this, false);
}

void EMFRecAccessGDIRecCreateColorSpaceW::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIObjectCat::CacheProperties(ctxt);
	auto pRec = (EMRCREATECOLORSPACEW*)EMFRecAccessGDIObjectCat::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecPolyBezier::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRPOLYBEZIER*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

bool EMFRecAccessGDIRecPolyBezier::DrawPreview(PreviewContext* info)
{
	auto pRec = (EMRPOLYGON*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	return m_previewHelper.DrawPreview(info, pRec, GetRecordType());
}

void EMFRecAccessGDIRecPolygon::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRPOLYGON*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

bool EMFGDIRecPolygonPreviewHelper::DrawPreview(EMFRecAccess::PreviewContext* info, const EMRPOLYGON* pRec, emfplus::OEmfPlusRecordType nType)
{
	if (!pRec || pRec->cptl == 0)
		return false;
	if (!info)
		return true;
	CRect rect = info->rect;
	if (info->bCalcOnly)
	{
		CSize sz = info->GetDefaultImgPreviewSize();
		rect.right = rect.left + sz.cx;
		rect.bottom = rect.top + sz.cy;
	}
	if (m_rcBounds.IsRectNull())
	{
		auto& rclBounds = pRec->rclBounds;
		if (rclBounds.left == 0 && rclBounds.top == 0
			&& rclBounds.top == -1 && rclBounds.bottom == -1)
		{
			auto ptMin = pRec->aptl[0];
			auto ptMax = ptMin;
			for (DWORD ii = 1; ii < pRec->cptl; ++ii)
			{
				auto& pt = pRec->aptl[ii];
				ptMin.x = std::min(ptMin.x, pt.x);
				ptMin.y = std::min(ptMin.y, pt.y);
				ptMax.x = std::max(ptMax.x, pt.x);
				ptMax.y = std::max(ptMax.y, pt.y);
			}
			m_rcBounds.SetRect(ptMin.x, ptMin.y, ptMax.x, ptMax.y);
		}
		else
		{
			m_rcBounds = (RECT&)rclBounds;
		}
	}
	CSize szBound = m_rcBounds.Size();
	szBound.cx = std::abs(szBound.cx);
	szBound.cy = std::abs(szBound.cy);
	CRect rcFit = GetFitRect(rect, szBound, true);
	info->szPreferedSize = rcFit.Size();
	if (!info->bCalcOnly)
	{
		auto pDC = info->pDC;
		int nOldMapMode = pDC->SetMapMode(MM_ISOTROPIC);
		auto oldWndExt = pDC->SetWindowExt(szBound.cx, szBound.cy);
		auto oldVwpExt = pDC->SetViewportExt(info->szPreferedSize.cx, info->szPreferedSize.cy);
		auto oldVwpOrg = pDC->SetViewportOrg(rcFit.TopLeft());
		auto oldWndOrg = pDC->SetWindowOrg(m_rcBounds.TopLeft());

		CPen pen(PS_SOLID, 1, RGB(0, 0, 255));
		auto oldPen = pDC->SelectObject(&pen);

		switch (nType)
		{
		case EmfRecordTypePolygon:
		case EmfRecordTypePolyline:
		case EmfRecordTypePolyLineTo:
			pDC->Polygon((POINT*)pRec->aptl, (int)pRec->cptl);
			break;
		case EmfRecordTypePolyBezier:
		case EmfRecordTypePolyBezierTo:
			pDC->PolyBezier((POINT*)pRec->aptl, (int)pRec->cptl);
			break;
		}

		pDC->SelectObject(oldPen);

		pDC->SetViewportOrg(oldVwpOrg);
		pDC->SetWindowOrg(oldWndOrg);
		pDC->SetViewportExt(oldVwpExt);
		pDC->SetWindowExt(oldWndExt);
		pDC->SetMapMode(nOldMapMode);
	}
	return true;
}

bool EMFRecAccessGDIRecPolygon::DrawPreview(PreviewContext* info)
{
	auto pRec = (EMRPOLYGON*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	return m_previewHelper.DrawPreview(info, pRec, GetRecordType());
}

void EMFRecAccessGDIRecPolyline::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRPOLYLINE*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

bool EMFRecAccessGDIRecPolyline::DrawPreview(PreviewContext* info)
{
	auto pRec = (EMRPOLYGON*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	return m_previewHelper.DrawPreview(info, pRec, GetRecordType());
}

void EMFRecAccessGDIRecPolyBezierTo::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRPOLYBEZIERTO*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

bool EMFRecAccessGDIRecPolyBezierTo::DrawPreview(PreviewContext* info)
{
	auto pRec = (EMRPOLYGON*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	return m_previewHelper.DrawPreview(info, pRec, GetRecordType());
}

void EMFRecAccessGDIRecPolyLineTo::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRPOLYLINETO*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

bool EMFRecAccessGDIRecPolyLineTo::DrawPreview(PreviewContext* info)
{
	auto pRec = (EMRPOLYGON*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	return m_previewHelper.DrawPreview(info, pRec, GetRecordType());
}

void EMFRecAccessGDIRecPolyPolyline::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRPOLYPOLYLINE*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecPolyPolygon::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRPOLYPOLYGON*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecSetWindowExtEx::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIStateCat::CacheProperties(ctxt);
	auto pRec = (EMRSETWINDOWEXTEX*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecSetWindowOrgEx::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIStateCat::CacheProperties(ctxt);
	auto pRec = (EMRSETWINDOWORGEX*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecSetViewportExtEx::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIStateCat::CacheProperties(ctxt);
	auto pRec = (EMRSETVIEWPORTEXTEX*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecSetViewportOrgEx::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIStateCat::CacheProperties(ctxt);
	auto pRec = (EMRSETVIEWPORTORGEX*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecSetBrushOrgEx::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIStateCat::CacheProperties(ctxt);
	auto pRec = (EMRSETBRUSHORGEX*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecSetPixelV::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRSETPIXELV*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecSetMapperFlags::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIStateCat::CacheProperties(ctxt);
	auto pRec = (EMRSETMAPPERFLAGS*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecSetMapMode::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIStateCat::CacheProperties(ctxt);
	auto pRec = (EMRSETMAPMODE*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecSetBkMode::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIStateCat::CacheProperties(ctxt);
	auto pRec = (EMRSETBKMODE*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecSetPolyFillMode::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIStateCat::CacheProperties(ctxt);
	auto pRec = (EMRSETPOLYFILLMODE*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecSetROP2::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIStateCat::CacheProperties(ctxt);
	auto pRec = (EMRSETROP2*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecSetStretchBltMode::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIStateCat::CacheProperties(ctxt);
	auto pRec = (EMRSETSTRETCHBLTMODE*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecSetTextAlign::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIStateCat::CacheProperties(ctxt);
	auto pRec = (EMRSETTEXTALIGN*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecSetColorAdjustment::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIStateCat::CacheProperties(ctxt);
	auto pRec = (EMRSETCOLORADJUSTMENT*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecSetTextColor::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIStateCat::CacheProperties(ctxt);
	auto pRec = (EMRSETTEXTCOLOR*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecSetBkColor::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIStateCat::CacheProperties(ctxt);
	auto pRec = (EMRSETBKCOLOR*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecOffsetClipRgn::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIClippingCat::CacheProperties(ctxt);
	auto pRec = (EMROFFSETCLIPRGN*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecMoveToEx::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIStateCat::CacheProperties(ctxt);
	auto pRec = (EMRMOVETOEX*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecIntersectClipRect::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIClippingCat::CacheProperties(ctxt);
	auto pRec = (EMRINTERSECTCLIPRECT*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecScaleViewportExtEx::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIStateCat::CacheProperties(ctxt);
	auto pRec = (EMRSCALEVIEWPORTEXTEX*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecScaleWindowExtEx::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIStateCat::CacheProperties(ctxt);
	auto pRec = (EMRSCALEWINDOWEXTEX*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecSetWorldTransform::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDITransformCat::CacheProperties(ctxt);
	auto pRec = (EMRSETWORLDTRANSFORM*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecModifyWorldTransform::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDITransformCat::CacheProperties(ctxt);
	auto pRec = (EMRMODIFYWORLDTRANSFORM*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecAngleArc::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRANGLEARC*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecEllipse::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRELLIPSE*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecRectangle::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRRECTANGLE*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecRoundRect::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRROUNDRECT*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecArc::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRARC*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecChord::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRCHORD*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecPie::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRPIE*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecExtFloodFill::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMREXTFLOODFILL*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecLineTo::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRLINETO*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecArcTo::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRARCTO*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecPolyDraw::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRPOLYDRAW*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecSetArcDirection::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIStateCat::CacheProperties(ctxt);
	auto pRec = (EMRSETARCDIRECTION*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecSetMiterLimit::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIStateCat::CacheProperties(ctxt);
	auto pRec = (EMRSETMITERLIMIT*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecFillPath::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRFILLPATH*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecStrokeAndFillPath::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRSTROKEANDFILLPATH*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecStrokePath::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRSTROKEPATH*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecSelectClipPath::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIClippingCat::CacheProperties(ctxt);
	auto pRec = (EMRSELECTCLIPPATH*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecFillRgn::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRFILLRGN*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecFrameRgn::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRFRAMERGN*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecInvertRgn::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIStateCat::CacheProperties(ctxt);
	auto pRec = (EMRINVERTRGN*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecPaintRgn::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRPAINTRGN*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecExtSelectClipRgn::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIClippingCat::CacheProperties(ctxt);
	auto pRec = (EMREXTSELECTCLIPRGN*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecPolyBezier16::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRPOLYBEZIER16*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecPolygon16::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRPOLYGON16*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecPolyline16::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRPOLYLINE16*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecPolyBezierTo16::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRPOLYBEZIERTO16*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecPolylineTo16::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRPOLYLINETO16*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecPolyPolyline16::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRPOLYPOLYLINE16*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecPolyPolygon16::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRPOLYPOLYGON16*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecPolyDraw16::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRPOLYDRAW16*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecPolyTextOutA::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRPOLYTEXTOUTA*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecPolyTextOutW::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRPOLYTEXTOUTW*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecSetICMMode::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIStateCat::CacheProperties(ctxt);
	auto pRec = (EMRSETICMMODE*)EMFRecAccessGDIRec::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecPixelFormat::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIStateCat::CacheProperties(ctxt);
	auto pRec = (EMRPIXELFORMAT*)EMFRecAccessGDIObjectCat::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecSetICMProfileA::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIStateCat::CacheProperties(ctxt);
	auto pRec = (EMRSETICMPROFILEA*)EMFRecAccessGDIObjectCat::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecSetICMProfileW::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIStateCat::CacheProperties(ctxt);
	auto pRec = (EMRSETICMPROFILEW*)EMFRecAccessGDIObjectCat::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecSetLayout::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIStateCat::CacheProperties(ctxt);
	auto pRec = (EMRSETLAYOUT*)EMFRecAccessGDIObjectCat::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecGradientFill::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIDrawingCat::CacheProperties(ctxt);
	auto pRec = (EMRGRADIENTFILL*)EMFRecAccessGDIObjectCat::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}

void EMFRecAccessGDIRecColorMatchToTargetW::CacheProperties(const CachePropertiesContext& ctxt)
{
	EMFRecAccessGDIStateCat::CacheProperties(ctxt);
	auto pRec = (EMRCOLORMATCHTOTARGET*)EMFRecAccessGDIObjectCat::GetGDIRecord(m_recInfo);
	if (pRec)
	{
		EmfStruct2Properties::Build(*pRec, m_propsCached.get());
	}
}
