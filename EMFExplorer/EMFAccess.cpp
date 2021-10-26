#include "pch.h"
#include "framework.h"
#include "EMFAccess.h"

#include <atlbase.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

EMFAccessBase::EMFAccessBase(const void* pData, size_t nSize)
{
	ATL::CComPtr<IStream> stream;
	stream.Attach(SHCreateMemStream((const BYTE *)pData, (UINT)nSize));
	m_pMetafile = std::make_unique<Gdiplus::Metafile>(stream);
	m_pMetafile->GetMetafileHeader(&m_hdr);
}

EMFAccessBase::EMFAccessBase(const std::vector<BYTE>& data)
	: EMFAccessBase(data.data(), data.size())
{
}

EMFAccessBase::~EMFAccessBase()
{
	
}

void EMFAccessBase::DrawMetafile(Gdiplus::Graphics& gg, const CRect& rcDraw) const
{
	ASSERT(m_pMetafile.get());
	Gdiplus::Rect rcDrawP(rcDraw.left, rcDraw.top, rcDraw.Width(), rcDraw.Height());
	gg.DrawImage(m_pMetafile.get(), rcDrawP);
}

struct EnumDrawEmfPlusContext
{
	Gdiplus::Metafile*	pMetafile;
	Gdiplus::Graphics*	pGraphics;
	size_t				nRecordStop;
	size_t				nRecordIndex;
};

extern "C"
BOOL CALLBACK EnumDrawMetafilePlusProc(Gdiplus::EmfPlusRecordType type, UINT flags, UINT dataSize, const BYTE* data, VOID* pCallbackData)
{
	auto& ctxt = *(EnumDrawEmfPlusContext*)pCallbackData;
	auto ret = ctxt.nRecordIndex <= ctxt.nRecordStop;
	if (ret)
		ctxt.pMetafile->PlayRecord(type, flags, dataSize, data);
	++ctxt.nRecordIndex;
	return ret;
}

void EMFAccessBase::DrawMetafileUntilRecord(Gdiplus::Graphics& gg, const CRect& rcDraw, size_t nRecord) const
{
	ASSERT(m_pMetafile.get());
	Gdiplus::Rect rcDrawP(rcDraw.left, rcDraw.top, rcDraw.Width(), rcDraw.Height());
	EnumDrawEmfPlusContext ctxt{ m_pMetafile.get(), &gg, nRecord };
	auto sts = gg.EnumerateMetafile(m_pMetafile.get(), rcDrawP, EnumDrawMetafilePlusProc, (void*)&ctxt);
}

Gdiplus::Image* EMFAccessBase::CloneMetafile() const
{
	ASSERT(m_pMetafile.get());
	return m_pMetafile->Clone();
}

#ifndef SHARED_HANDLERS
using namespace emfplus;

#include "EMFRecAccessGDI.h"
#include "EMFRecAccessPlus.h"

EMFAccess::EMFAccess(const data_access::memory_wrapper& data)
	: EMFAccess(data.data(), data.size())
{
}

EMFAccess::~EMFAccess()
{
	FreeRecords();
}

struct EnumEmfPlusContext
{
	Gdiplus::Metafile*	pMetafile;
	Gdiplus::Graphics*	pGraphics;
	EMFAccess*			pAccess;
};

extern "C"
BOOL CALLBACK EnumMetafilePlusProc(Gdiplus::EmfPlusRecordType type, UINT flags, UINT dataSize, const BYTE* data, VOID* pCallbackData)
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
		{
			EMFGDIState state;
			state.pSavedRec = pRecAccess;
			m_vGDIState.emplace_back(state);
		}
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
		{
			auto nObjectID = (u8t)(rec.Flags & OEmfPlusRecObjectReader::FlagObjectIDMask);
			SetObjectToTable(nObjectID, pRecAccess, true);
		}
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
		{
			auto pRec = (OEmfPlusRecSave*)rec.Data;
			ASSERT((u32t)m_vPlusState.size() == pRec->StackIndex);
			EMFPlusState state;
			state.bContainer = false;
			state.pSavedRec = pRecAccess;
			m_vPlusState.emplace_back(state);
		}
		break;
	case EmfPlusRecordTypeRestore:
		pRecAccess = new EMFRecAccessGDIPlusRecRestore;
		break;
	case EmfPlusRecordTypeBeginContainer:
		pRecAccess = new EMFRecAccessGDIPlusRecBeginContainer;
		{
			auto pRec = (const OEmfPlusRecBeginContainer*)rec.Data;
			ASSERT((u32t)m_vPlusState.size() == pRec->StackIndex);
			EMFPlusState state;
			state.bContainer = true;
			state.pSavedRec = pRecAccess;
			m_vPlusState.emplace_back(state);
		}
		break;
	case EmfPlusRecordTypeBeginContainerNoParams:
		pRecAccess = new EMFRecAccessGDIPlusRecBeginContainerNoParams;
		{
			auto pRec = (const OEmfPlusRecBeginContainerNoParams*)rec.Data;
			ASSERT((u32t)m_vPlusState.size() == pRec->StackIndex);
			EMFPlusState state;
			state.bContainer = true;
			state.pSavedRec = pRecAccess;
			m_vPlusState.emplace_back(state);
		}
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

	switch (type)
	{
	case EmfRecordTypeRestoreDC:
		{
			auto pRec = EMFRecAccessGDIRec::GetGDIRecord(rec);
			auto nSavedDC = ((EMRRESTOREDC*)pRec)->iRelative;
			if (nSavedDC < 0)
				nSavedDC = (int)m_vGDIState.size() + nSavedDC;
			if (nSavedDC < 0 || nSavedDC >= (int)m_vGDIState.size())
				break;
			m_vGDIState.resize(nSavedDC);
		}
		break;
	case EmfPlusRecordTypeRestore:
		PopPlusState(((OEmfPlusRecRestore*)rec.Data)->StackIndex, false);
		break;
	case EmfPlusRecordTypeEndContainer:
		PopPlusState(((OEmfPlusRecEndContainer*)rec.Data)->StackIndex, true);
		break;
	}

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
		if (index < m_vGDIObjTable.size())
			return m_vGDIObjTable[index].pRec;
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
		if (index >= m_vGDIObjTable.size())
			m_vGDIObjTable.resize(index + 1);
		m_vGDIObjTable[index].pRec = pRec;
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

EMFRecAccess* EMFAccess::GetGDISaveRecord(LONG nSavedDC) const
{
	if (nSavedDC < 0)
		nSavedDC = (LONG)m_vGDIState.size() + nSavedDC;
	if (nSavedDC < 0 || nSavedDC >= (LONG)m_vGDIState.size())
		return nullptr;
	return m_vGDIState[nSavedDC].pSavedRec;
}

EMFRecAccess* EMFAccess::GetPlusSaveRecord(u32t nStackIndex) const
{
	if (nStackIndex >= (u32t)m_vPlusState.size())
	{
		ASSERT(0);
		return nullptr;
	}
	return m_vPlusState[nStackIndex].pSavedRec;
}

bool EMFAccess::PopPlusState(uint32_t nStackIndex, bool bContainer)
{
	if (nStackIndex >= (u32t)m_vPlusState.size())
	{
		ASSERT(0);
		return false;
	}
	auto it = m_vPlusState.begin() + nStackIndex;
	ASSERT(it->bContainer == bContainer);
	m_vPlusState.erase(it, m_vPlusState.end());
	return true;
}

#endif // SHARED_HANDLERS
