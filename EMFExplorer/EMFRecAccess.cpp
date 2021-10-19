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
			rcFit.OffsetRect(0, (rcFit.Height() - rcDest.Height()) / 2);
	}
	else
	{
		szDest.cx	= (int)(szSrc.cx * (float)szDest.cy / szSrc.cy);
		rcFit.right = rcFit.left + szDest.cx;
		if (bCenter)
			rcFit.OffsetRect((rcFit.Width() - rcDest.Width()) / 2, 0);
	}
	return rcFit;
}

const PropertyNode& EMFRecAccess::GetProperties(EMFAccess* pEMF)
{
	if (!m_bRecDataCached)
	{
		CacheProperties(pEMF);
		m_bRecDataCached = true;
	}
	return m_propsCached;
}

void EMFRecAccess::CacheProperties(EMFAccess* pEMF)
{
	m_propsCached.Add(L"Type", std::to_wstring(m_recInfo.Type).c_str());
	m_propsCached.Add(L"DataSize", std::to_wstring(m_recInfo.DataSize).c_str());
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

void EMFRecAccessGDIRecHeader::CachePropertiesFromGDI(EMFAccess* pEMF, const ENHMETARECORD* pRec)
{
	auto pHeader = (const ENHMETAHEADER*)pRec;
	CStringW str;
	str.Format(L"%08X", pHeader->dSignature);
	m_propsCached.Add(L"Signature", str);
}

void EMFRecAccessGDIPlusRec::CacheProperties(EMFAccess* pEMF)
{
	EMFRecAccess::CacheProperties(pEMF);
	m_propsCached.Add(L"Flags", std::to_wstring(m_recInfo.Flags).c_str());
}

EMFAccess::EMFAccess(const std::vector<emfplus::u8t>& vData)
{
	ATL::CComPtr<IStream> stream;
	stream.Attach(SHCreateMemStream(vData.data(), (UINT)vData.size()));
	m_pMetafile = std::make_unique<Gdiplus::Metafile>(stream);
	m_pMetafile->GetMetafileHeader(&m_hdr);
}

EMFAccess::~EMFAccess()
{
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

bool EMFAccess::HandleEMFRecord(OEmfPlusRecordType type, UINT flags, UINT dataSize, const BYTE* data)
{
	OEmfPlusRecInfo rec;
	rec.Type = (u16t)type;
	rec.Flags = (u16t)flags;
	rec.Size = sizeof(OEmfPlusRec) + dataSize;
	rec.DataSize = dataSize;
	rec.Data = (u8t*)data;
	std::unique_ptr<EMFRecAccess> pRecAccess;
	switch (type)
	{
	case EmfRecordTypeHeader:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecHeader>();
		break;
	case EmfRecordTypePolyBezier:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecPolyBezier>();
		break;
	case EmfRecordTypePolygon:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecPolygon>();
		break;
	case EmfRecordTypePolyline:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecPolyline>();
		break;
	case EmfRecordTypePolyBezierTo:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecPolyBezierTo>();
		break;
	case EmfRecordTypePolyLineTo:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecPolyLineTo>();
		break;
	case EmfRecordTypePolyPolyline:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecPolyPolyline>();
		break;
	case EmfRecordTypePolyPolygon:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecPolyPolygon>();
		break;
	case EmfRecordTypeSetWindowExtEx:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetWindowExtEx>();
		break;
	case EmfRecordTypeSetWindowOrgEx:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetWindowOrgEx>();
		break;
	case EmfRecordTypeSetViewportExtEx:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetViewportExtEx>();
		break;
	case EmfRecordTypeSetViewportOrgEx:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetViewportOrgEx>();
		break;
	case EmfRecordTypeSetBrushOrgEx:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetBrushOrgEx>();
		break;
	case EmfRecordTypeEOF:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecEOF>();
		break;
	case EmfRecordTypeSetPixelV:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetPixelV>();
		break;
	case EmfRecordTypeSetMapperFlags:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetMapperFlags>();
		break;
	case EmfRecordTypeSetMapMode:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetMapMode>();
		break;
	case EmfRecordTypeSetBkMode:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetBkMode>();
		break;
	case EmfRecordTypeSetPolyFillMode:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetPolyFillMode>();
		break;
	case EmfRecordTypeSetROP2:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetROP2>();
		break;
	case EmfRecordTypeSetStretchBltMode:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetStretchBltMode>();
		break;
	case EmfRecordTypeSetTextAlign:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetTextAlign>();
		break;
	case EmfRecordTypeSetColorAdjustment:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetColorAdjustment>();
		break;
	case EmfRecordTypeSetTextColor:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetTextColor>();
		break;
	case EmfRecordTypeSetBkColor:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetBkColor>();
		break;
	case EmfRecordTypeOffsetClipRgn:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecOffsetClipRgn>();
		break;
	case EmfRecordTypeMoveToEx:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecMoveToEx>();
		break;
	case EmfRecordTypeSetMetaRgn:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetMetaRgn>();
		break;
	case EmfRecordTypeExcludeClipRect:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecExcludeClipRect>();
		break;
	case EmfRecordTypeIntersectClipRect:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecIntersectClipRect>();
		break;
	case EmfRecordTypeScaleViewportExtEx:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecScaleViewportExtEx>();
		break;
	case EmfRecordTypeScaleWindowExtEx:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecScaleWindowExtEx>();
		break;
	case EmfRecordTypeSaveDC:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSaveDC>();
		break;
	case EmfRecordTypeRestoreDC:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecRestoreDC>();
		break;
	case EmfRecordTypeSetWorldTransform:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetWorldTransform>();
		break;
	case EmfRecordTypeModifyWorldTransform:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecModifyWorldTransform>();
		break;
	case EmfRecordTypeSelectObject:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSelectObject>();
		break;
	case EmfRecordTypeCreatePen:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecCreatePen>();
		break;
	case EmfRecordTypeCreateBrushIndirect:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecCreateBrushIndirect>();
		break;
	case EmfRecordTypeDeleteObject:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecDeleteObject>();
		break;
	case EmfRecordTypeAngleArc:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecAngleArc>();
		break;
	case EmfRecordTypeEllipse:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecEllipse>();
		break;
	case EmfRecordTypeRectangle:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecRectangle>();
		break;
	case EmfRecordTypeRoundRect:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecRoundRect>();
		break;
	case EmfRecordTypeArc:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecArc>();
		break;
	case EmfRecordTypeChord:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecChord>();
		break;
	case EmfRecordTypePie:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecPie>();
		break;
	case EmfRecordTypeSelectPalette:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSelectPalette>();
		break;
	case EmfRecordTypeCreatePalette:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecCreatePalette>();
		break;
	case EmfRecordTypeSetPaletteEntries:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetPaletteEntries>();
		break;
	case EmfRecordTypeResizePalette:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecResizePalette>();
		break;
	case EmfRecordTypeRealizePalette:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecRealizePalette>();
		break;
	case EmfRecordTypeExtFloodFill:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecExtFloodFill>();
		break;
	case EmfRecordTypeLineTo:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecLineTo>();
		break;
	case EmfRecordTypeArcTo:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecArcTo>();
		break;
	case EmfRecordTypePolyDraw:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecPolyDraw>();
		break;
	case EmfRecordTypeSetArcDirection:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetArcDirection>();
		break;
	case EmfRecordTypeSetMiterLimit:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetMiterLimit>();
		break;
	case EmfRecordTypeBeginPath:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecBeginPath>();
		break;
	case EmfRecordTypeEndPath:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecEndPath>();
		break;
	case EmfRecordTypeCloseFigure:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecCloseFigure>();
		break;
	case EmfRecordTypeFillPath:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecFillPath>();
		break;
	case EmfRecordTypeStrokeAndFillPath:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecStrokeAndFillPath>();
		break;
	case EmfRecordTypeStrokePath:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecStrokePath>();
		break;
	case EmfRecordTypeFlattenPath:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecFlattenPath>();
		break;
	case EmfRecordTypeWidenPath:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecWidenPath>();
		break;
	case EmfRecordTypeSelectClipPath:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSelectClipPath>();
		break;
	case EmfRecordTypeAbortPath:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecAbortPath>();
		break;
	case EmfRecordTypeReserved_069:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecReserved_069>();
		break;
	case EmfRecordTypeGdiComment:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecGdiComment>();
		break;
	case EmfRecordTypeFillRgn:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecFillRgn>();
		break;
	case EmfRecordTypeFrameRgn:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecFrameRgn>();
		break;
	case EmfRecordTypeInvertRgn:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecInvertRgn>();
		break;
	case EmfRecordTypePaintRgn:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecPaintRgn>();
		break;
	case EmfRecordTypeExtSelectClipRgn:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecExtSelectClipRgn>();
		break;
	case EmfRecordTypeBitBlt:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecBitBlt>();
		break;
	case EmfRecordTypeStretchBlt:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecStretchBlt>();
		break;
	case EmfRecordTypeMaskBlt:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecMaskBlt>();
		break;
	case EmfRecordTypePlgBlt:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecPlgBlt>();
		break;
	case EmfRecordTypeSetDIBitsToDevice:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetDIBitsToDevice>();
		break;
	case EmfRecordTypeStretchDIBits:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecStretchDIBits>();
		break;
	case EmfRecordTypeExtCreateFontIndirect:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecExtCreateFontIndirect>();
		break;
	case EmfRecordTypeExtTextOutA:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecExtTextOutA>();
		break;
	case EmfRecordTypeExtTextOutW:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecExtTextOutW>();
		break;
	case EmfRecordTypePolyBezier16:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecPolyBezier16>();
		break;
	case EmfRecordTypePolygon16:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecPolygon16>();
		break;
	case EmfRecordTypePolyline16:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecPolyline16>();
		break;
	case EmfRecordTypePolyBezierTo16:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecPolyBezierTo16>();
		break;
	case EmfRecordTypePolylineTo16:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecPolylineTo16>();
		break;
	case EmfRecordTypePolyPolyline16:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecPolyPolyline16>();
		break;
	case EmfRecordTypePolyPolygon16:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecPolyPolygon16>();
		break;
	case EmfRecordTypePolyDraw16:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecPolyDraw16>();
		break;
	case EmfRecordTypeCreateMonoBrush:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecCreateMonoBrush>();
		break;
	case EmfRecordTypeCreateDIBPatternBrushPt:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecCreateDIBPatternBrushPt>();
		break;
	case EmfRecordTypeExtCreatePen:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecExtCreatePen>();
		break;
	case EmfRecordTypePolyTextOutA:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecPolyTextOutA>();
		break;
	case EmfRecordTypePolyTextOutW:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecPolyTextOutW>();
		break;
	case EmfRecordTypeSetICMMode:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetICMMode>();
		break;
	case EmfRecordTypeCreateColorSpace:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecCreateColorSpace>();
		break;
	case EmfRecordTypeSetColorSpace:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetColorSpace>();
		break;
	case EmfRecordTypeDeleteColorSpace:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecDeleteColorSpace>();
		break;
	case EmfRecordTypeGLSRecord:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecGLSRecord>();
		break;
	case EmfRecordTypeGLSBoundedRecord:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecGLSBoundedRecord>();
		break;
	case EmfRecordTypePixelFormat:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecPixelFormat>();
		break;
	case EmfRecordTypeDrawEscape:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecDrawEscape>();
		break;
	case EmfRecordTypeExtEscape:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecExtEscape>();
		break;
	case EmfRecordTypeStartDoc:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecStartDoc>();
		break;
	case EmfRecordTypeSmallTextOut:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSmallTextOut>();
		break;
	case EmfRecordTypeForceUFIMapping:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecForceUFIMapping>();
		break;
	case EmfRecordTypeNamedEscape:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecNamedEscape>();
		break;
	case EmfRecordTypeColorCorrectPalette:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecColorCorrectPalette>();
		break;
	case EmfRecordTypeSetICMProfileA:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetICMProfileA>();
		break;
	case EmfRecordTypeSetICMProfileW:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetICMProfileW>();
		break;
	case EmfRecordTypeAlphaBlend:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecAlphaBlend>();
		break;
	case EmfRecordTypeSetLayout:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetLayout>();
		break;
	case EmfRecordTypeTransparentBlt:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecTransparentBlt>();
		break;
	case EmfRecordTypeReserved_117:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecReserved_117>();
		break;
	case EmfRecordTypeGradientFill:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecGradientFill>();
		break;
	case EmfRecordTypeSetLinkedUFIs:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetLinkedUFIs>();
		break;
	case EmfRecordTypeSetTextJustification:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecSetTextJustification>();
		break;
	case EmfRecordTypeColorMatchToTargetW:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecColorMatchToTargetW>();
		break;
	case EmfRecordTypeCreateColorSpaceW:
		pRecAccess = std::make_unique<EMFRecAccessGDIRecCreateColorSpaceW>();
		break;
	case EmfPlusRecordTypeHeader:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecHeader>();
		break;
	case EmfPlusRecordTypeEndOfFile:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecEndOfFile>();
		break;
	case EmfPlusRecordTypeComment:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecComment>();
		break;
	case EmfPlusRecordTypeGetDC:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecGetDC>();
		break;
	case EmfPlusRecordTypeMultiFormatStart:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecMultiFormatStart>();
		break;
	case EmfPlusRecordTypeMultiFormatSection:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecMultiFormatSection>();
		break;
	case EmfPlusRecordTypeMultiFormatEnd:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecMultiFormatEnd>();
		break;
	case EmfPlusRecordTypeObject:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecObject>();
		break;
	case EmfPlusRecordTypeClear:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecClear>();
		break;
	case EmfPlusRecordTypeFillRects:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecFillRects>();
		break;
	case EmfPlusRecordTypeDrawRects:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecDrawRects>();
		break;
	case EmfPlusRecordTypeFillPolygon:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecFillPolygon>();
		break;
	case EmfPlusRecordTypeDrawLines:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecDrawLines>();
		break;
	case EmfPlusRecordTypeFillEllipse:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecFillEllipse>();
		break;
	case EmfPlusRecordTypeDrawEllipse:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecDrawEllipse>();
		break;
	case EmfPlusRecordTypeFillPie:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecFillPie>();
		break;
	case EmfPlusRecordTypeDrawPie:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecDrawPie>();
		break;
	case EmfPlusRecordTypeDrawArc:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecDrawArc>();
		break;
	case EmfPlusRecordTypeFillRegion:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecFillRegion>();
		break;
	case EmfPlusRecordTypeFillPath:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecFillPath>();
		break;
	case EmfPlusRecordTypeDrawPath:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecDrawPath>();
		break;
	case EmfPlusRecordTypeFillClosedCurve:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecFillClosedCurve>();
		break;
	case EmfPlusRecordTypeDrawClosedCurve:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecDrawClosedCurve>();
		break;
	case EmfPlusRecordTypeDrawCurve:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecDrawCurve>();
		break;
	case EmfPlusRecordTypeDrawBeziers:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecDrawBeziers>();
		break;
	case EmfPlusRecordTypeDrawImage:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecDrawImage>();
		break;
	case EmfPlusRecordTypeDrawImagePoints:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecDrawImagePoints>();
		break;
	case EmfPlusRecordTypeDrawString:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecDrawString>();
		break;
	case EmfPlusRecordTypeSetRenderingOrigin:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecSetRenderingOrigin>();
		break;
	case EmfPlusRecordTypeSetAntiAliasMode:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecSetAntiAliasMode>();
		break;
	case EmfPlusRecordTypeSetTextRenderingHint:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecSetTextRenderingHint>();
		break;
	case EmfPlusRecordTypeSetTextContrast:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecSetTextContrast>();
		break;
	case EmfPlusRecordTypeSetInterpolationMode:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecSetInterpolationMode>();
		break;
	case EmfPlusRecordTypeSetPixelOffsetMode:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecSetPixelOffsetMode>();
		break;
	case EmfPlusRecordTypeSetCompositingMode:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecSetCompositingMode>();
		break;
	case EmfPlusRecordTypeSetCompositingQuality:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecSetCompositingQuality>();
		break;
	case EmfPlusRecordTypeSave:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecSave>();
		break;
	case EmfPlusRecordTypeRestore:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecRestore>();
		break;
	case EmfPlusRecordTypeBeginContainer:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecBeginContainer>();
		break;
	case EmfPlusRecordTypeBeginContainerNoParams:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecBeginContainerNoParams>();
		break;
	case EmfPlusRecordTypeEndContainer:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecEndContainer>();
		break;
	case EmfPlusRecordTypeSetWorldTransform:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecSetWorldTransform>();
		break;
	case EmfPlusRecordTypeResetWorldTransform:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecResetWorldTransform>();
		break;
	case EmfPlusRecordTypeMultiplyWorldTransform:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecMultiplyWorldTransform>();
		break;
	case EmfPlusRecordTypeTranslateWorldTransform:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecTranslateWorldTransform>();
		break;
	case EmfPlusRecordTypeScaleWorldTransform:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecScaleWorldTransform>();
		break;
	case EmfPlusRecordTypeRotateWorldTransform:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecRotateWorldTransform>();
		break;
	case EmfPlusRecordTypeSetPageTransform:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecSetPageTransform>();
		break;
	case EmfPlusRecordTypeResetClip:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecResetClip>();
		break;
	case EmfPlusRecordTypeSetClipRect:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecSetClipRect>();
		break;
	case EmfPlusRecordTypeSetClipPath:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecSetClipPath>();
		break;
	case EmfPlusRecordTypeSetClipRegion:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecSetClipRegion>();
		break;
	case EmfPlusRecordTypeOffsetClip:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecOffsetClip>();
		break;
	case EmfPlusRecordTypeDrawDriverString:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecDrawDriverString>();
		break;
	case EmfPlusRecordTypeStrokeFillPath:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecStrokeFillPath>();
		break;
	case EmfPlusRecordTypeSerializableObject:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecSerializableObject>();
		break;
	case EmfPlusRecordTypeSetTSGraphics:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecSetTSGraphics>();
		break;
	case EmfPlusRecordTypeSetTSClip:
		pRecAccess = std::make_unique<EMFRecAccessGDIPlusRecSetTSClip>();
		break;
	}
	if (!pRecAccess)
	{
		ASSERT(0);
		return false;
	}
	pRecAccess->SetRecInfo(rec);
	m_EMFRecords.push_back(std::move(pRecAccess));
	return true;
}

EMFRecAccess* EMFAccess::GetRecord(size_t index) const
{
	ASSERT(index < GetRecordCount());
	return m_EMFRecords[index].get();
}



