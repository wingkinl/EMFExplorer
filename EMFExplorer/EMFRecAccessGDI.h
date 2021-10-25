#ifndef EMF_REC_ACCESS_GDI_H
#define EMF_REC_ACCESS_GDI_H

#include "EMFRecAccess.h"

class EMFRecAccessGDIRec : public EMFRecAccess
{
public:
	bool IsGDIRecord() const override { return true; }

	static const ENHMETARECORD* GetGDIRecord(const emfplus::OEmfPlusRecInfo& rec);
};

class EMFRecAccessGDIBitmapCat : public EMFRecAccessGDIRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryBitmap; }
};

class EMFRecAccessGDIClippingCat : public EMFRecAccessGDIRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryClipping; }
};

class EMFRecAccessGDIControlCat : public EMFRecAccessGDIRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryControl; }
};

class EMFRecAccessGDIDrawingCat : public EMFRecAccessGDIRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryDrawing; }
};

class EMFRecAccessGDIEscapeCat : public EMFRecAccessGDIRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryEscape; }
};

class EMFRecAccessGDIObjectCat : public EMFRecAccessGDIRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryObject; }
};

class EMFRecAccessGDIObjManipulationCat : public EMFRecAccessGDIRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryObjManipulation; }
};

class EMFRecAccessGDIOpenGLCat : public EMFRecAccessGDIRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryOpenGL; }
};

class EMFRecAccessGDIPathBracketCat : public EMFRecAccessGDIRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryPathBracket; }
};

class EMFRecAccessGDIStateCat : public EMFRecAccessGDIRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryState; }
};

class EMFRecAccessGDITransformCat : public EMFRecAccessGDIRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryTransform; }
};

class EMFRecAccessGDIRecHeader : public EMFRecAccessGDIControlCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_HEADER"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeHeader; }
protected:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecPolyBezier : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYBEZIER"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyBezier; }
};

class EMFRecAccessGDIRecPolygon : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYGON"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolygon; }
};

class EMFRecAccessGDIRecPolyline : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYLINE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyline; }
};

class EMFRecAccessGDIRecPolyBezierTo : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYBEZIERTO"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyBezierTo; }
};

class EMFRecAccessGDIRecPolyLineTo : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYLINETO"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyLineTo; }
};

class EMFRecAccessGDIRecPolyPolyline : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYPOLYLINE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyPolyline; }
};

class EMFRecAccessGDIRecPolyPolygon : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYPOLYGON"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyPolygon; }
};

class EMFRecAccessGDIRecSetWindowExtEx : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETWINDOWEXTEX"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetWindowExtEx; }
};

class EMFRecAccessGDIRecSetWindowOrgEx : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETWINDOWORGEX"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetWindowOrgEx; }
};

class EMFRecAccessGDIRecSetViewportExtEx : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETVIEWPORTEXTEX"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetViewportExtEx; }
};

class EMFRecAccessGDIRecSetViewportOrgEx : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETVIEWPORTORGEX"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetViewportOrgEx; }
};

class EMFRecAccessGDIRecSetBrushOrgEx : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETBRUSHORGEX"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetBrushOrgEx; }
};

class EMFRecAccessGDIRecEOF : public EMFRecAccessGDIControlCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_EOF"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeEOF; }
};

class EMFRecAccessGDIRecSetPixelV : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETPIXELV"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetPixelV; }
};

class EMFRecAccessGDIRecSetMapperFlags : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETMAPPERFLAGS"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetMapperFlags; }
};

class EMFRecAccessGDIRecSetMapMode : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETMAPMODE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetMapMode; }
};

class EMFRecAccessGDIRecSetBkMode : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETBKMODE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetBkMode; }
};

class EMFRecAccessGDIRecSetPolyFillMode : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETPOLYFILLMODE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetPolyFillMode; }
};

class EMFRecAccessGDIRecSetROP2 : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETROP2"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetROP2; }
};

class EMFRecAccessGDIRecSetStretchBltMode : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETSTRETCHBLTMODE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetStretchBltMode; }
};

class EMFRecAccessGDIRecSetTextAlign : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETTEXTALIGN"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetTextAlign; }
};

class EMFRecAccessGDIRecSetColorAdjustment : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETCOLORADJUSTMENT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetColorAdjustment; }
};

class EMFRecAccessGDIRecSetTextColor : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETTEXTCOLOR"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetTextColor; }
};

class EMFRecAccessGDIRecSetBkColor : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETBKCOLOR"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetBkColor; }
};

class EMFRecAccessGDIRecOffsetClipRgn : public EMFRecAccessGDIClippingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_OFFSETCLIPRGN"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeOffsetClipRgn; }
};

class EMFRecAccessGDIRecMoveToEx : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_MOVETOEX"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeMoveToEx; }
};

class EMFRecAccessGDIRecSetMetaRgn : public EMFRecAccessGDIClippingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETMETARGN"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetMetaRgn; }
};

class EMFRecAccessGDIRecExcludeClipRect : public EMFRecAccessGDIClippingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_EXCLUDECLIPRECT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeExcludeClipRect; }
};

class EMFRecAccessGDIRecIntersectClipRect : public EMFRecAccessGDIClippingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_INTERSECTCLIPRECT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeIntersectClipRect; }
};

class EMFRecAccessGDIRecScaleViewportExtEx : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SCALEVIEWPORTEXTEX"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeScaleViewportExtEx; }
};

class EMFRecAccessGDIRecScaleWindowExtEx : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SCALEWINDOWEXTEX"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeScaleWindowExtEx; }
};

class EMFRecAccessGDIRecSaveDC : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SAVEDC"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSaveDC; }
};

class EMFRecAccessGDIRecRestoreDC : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_RESTOREDC"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeRestoreDC; }
private:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecSetWorldTransform : public EMFRecAccessGDITransformCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETWORLDTRANSFORM"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetWorldTransform; }
};

class EMFRecAccessGDIRecModifyWorldTransform : public EMFRecAccessGDITransformCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_MODIFYWORLDTRANSFORM"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeModifyWorldTransform; }
};

class EMFRecAccessGDIRecSelectObject : public EMFRecAccessGDIObjManipulationCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SELECTOBJECT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSelectObject; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecCreatePen : public EMFRecAccessGDIObjectCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_CREATEPEN"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeCreatePen; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecCreateBrushIndirect : public EMFRecAccessGDIObjectCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_CREATEBRUSHINDIRECT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeCreateBrushIndirect; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecDeleteObject : public EMFRecAccessGDIObjManipulationCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_DELETEOBJECT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeDeleteObject; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecAngleArc : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_ANGLEARC"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeAngleArc; }
};

class EMFRecAccessGDIRecEllipse : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_ELLIPSE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeEllipse; }
};

class EMFRecAccessGDIRecRectangle : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_RECTANGLE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeRectangle; }
};

class EMFRecAccessGDIRecRoundRect : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_ROUNDRECT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeRoundRect; }
};

class EMFRecAccessGDIRecArc : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_ARC"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeArc; }
};

class EMFRecAccessGDIRecChord : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_CHORD"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeChord; }
};

class EMFRecAccessGDIRecPie : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_PIE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePie; }
};

class EMFRecAccessGDIRecSelectPalette : public EMFRecAccessGDIObjManipulationCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SELECTPALETTE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSelectPalette; }
};

class EMFRecAccessGDIRecCreatePalette : public EMFRecAccessGDIObjectCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_CREATEPALETTE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeCreatePalette; }
};

class EMFRecAccessGDIRecSetPaletteEntries : public EMFRecAccessGDIObjManipulationCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETPALETTEENTRIES"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetPaletteEntries; }
};

class EMFRecAccessGDIRecResizePalette : public EMFRecAccessGDIObjManipulationCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_RESIZEPALETTE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeResizePalette; }
};

class EMFRecAccessGDIRecRealizePalette : public EMFRecAccessGDIObjManipulationCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_REALIZEPALETTE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeRealizePalette; }
};

class EMFRecAccessGDIRecExtFloodFill : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_EXTFLOODFILL"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeExtFloodFill; }
};

class EMFRecAccessGDIRecLineTo : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_LINETO"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeLineTo; }
};

class EMFRecAccessGDIRecArcTo : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_ARCTO"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeArcTo; }
};

class EMFRecAccessGDIRecPolyDraw : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYDRAW"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyDraw; }
};

class EMFRecAccessGDIRecSetArcDirection : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETARCDIRECTION"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetArcDirection; }
};

class EMFRecAccessGDIRecSetMiterLimit : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETMITERLIMIT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetMiterLimit; }
};

class EMFRecAccessGDIRecBeginPath : public EMFRecAccessGDIPathBracketCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_BEGINPATH"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeBeginPath; }
};

class EMFRecAccessGDIRecEndPath : public EMFRecAccessGDIPathBracketCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_ENDPATH"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeEndPath; }
};

class EMFRecAccessGDIRecCloseFigure : public EMFRecAccessGDIPathBracketCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_CLOSEFIGURE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeCloseFigure; }
};

class EMFRecAccessGDIRecFillPath : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_FILLPATH"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeFillPath; }
};

class EMFRecAccessGDIRecStrokeAndFillPath : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_STROKEANDFILLPATH"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeStrokeAndFillPath; }
};

class EMFRecAccessGDIRecStrokePath : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_STROKEPATH"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeStrokePath; }
};

class EMFRecAccessGDIRecFlattenPath : public EMFRecAccessGDIPathBracketCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_FLATTENPATH"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeFlattenPath; }
};

class EMFRecAccessGDIRecWidenPath : public EMFRecAccessGDIPathBracketCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_WIDENPATH"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeWidenPath; }
};

class EMFRecAccessGDIRecSelectClipPath : public EMFRecAccessGDIClippingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SELECTCLIPPATH"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSelectClipPath; }
};

class EMFRecAccessGDIRecAbortPath : public EMFRecAccessGDIPathBracketCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_ABORTPATH"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeAbortPath; }
};

class EMFRecAccessGDIRecReserved_069 : public EMFRecAccessGDIRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_Reserved_069"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeReserved_069; }

	RecCategory GetRecordCategory() const override { return RecCategoryReserved; }
};

class EMFRecAccessGDIRecGdiComment : public EMFRecAccessGDIRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_GDICOMMENT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeGdiComment; }

	RecCategory GetRecordCategory() const override { return RecCategoryComment; }
protected:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecFillRgn : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_FILLRGN"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeFillRgn; }
};

class EMFRecAccessGDIRecFrameRgn : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_FRAMERGN"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeFrameRgn; }
};

class EMFRecAccessGDIRecInvertRgn : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_INVERTRGN"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeInvertRgn; }
};

class EMFRecAccessGDIRecPaintRgn : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_PAINTRGN"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePaintRgn; }
};

class EMFRecAccessGDIRecExtSelectClipRgn : public EMFRecAccessGDIClippingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_EXTSELECTCLIPRGN"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeExtSelectClipRgn; }
};

class EMFRecAccessGDIRecBitBlt : public EMFRecAccessGDIBitmapCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_BITBLT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeBitBlt; }
};

class EMFRecAccessGDIRecStretchBlt : public EMFRecAccessGDIBitmapCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_STRETCHBLT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeStretchBlt; }
};

class EMFRecAccessGDIRecMaskBlt : public EMFRecAccessGDIBitmapCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_MASKBLT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeMaskBlt; }
};

class EMFRecAccessGDIRecPlgBlt : public EMFRecAccessGDIBitmapCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_PLGBLT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePlgBlt; }
};

class EMFRecAccessGDIRecSetDIBitsToDevice : public EMFRecAccessGDIBitmapCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETDIBITSTODEVICE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetDIBitsToDevice; }
};

class EMFRecAccessGDIRecStretchDIBits : public EMFRecAccessGDIBitmapCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_STRETCHDIBITS"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeStretchDIBits; }
};

class EMFRecAccessGDIRecExtCreateFontIndirect : public EMFRecAccessGDIObjectCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_EXTCREATEFONTINDIRECTW"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeExtCreateFontIndirect; }
};

class EMFRecAccessGDIRecExtTextOutA : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_EXTTEXTOUTA"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeExtTextOutA; }
};

class EMFRecAccessGDIRecExtTextOutW : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_EXTTEXTOUTW"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeExtTextOutW; }
};

class EMFRecAccessGDIRecPolyBezier16 : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYBEZIER16"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyBezier16; }
};

class EMFRecAccessGDIRecPolygon16 : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYGON16"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolygon16; }
};

class EMFRecAccessGDIRecPolyline16 : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYLINE16"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyline16; }
};

class EMFRecAccessGDIRecPolyBezierTo16 : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYBEZIERTO16"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyBezierTo16; }
};

class EMFRecAccessGDIRecPolylineTo16 : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYLINETO16"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolylineTo16; }
};

class EMFRecAccessGDIRecPolyPolyline16 : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYPOLYLINE16"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyPolyline16; }
};

class EMFRecAccessGDIRecPolyPolygon16 : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYPOLYGON16"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyPolygon16; }
};

class EMFRecAccessGDIRecPolyDraw16 : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYDRAW16"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyDraw16; }
};

class EMFRecAccessGDIRecCreateMonoBrush : public EMFRecAccessGDIObjectCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_CREATEMONOBRUSH"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeCreateMonoBrush; }
};

class EMFRecAccessGDIRecCreateDIBPatternBrushPt : public EMFRecAccessGDIObjectCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_CREATEDIBPATTERNBRUSHPT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeCreateDIBPatternBrushPt; }
};

class EMFRecAccessGDIRecExtCreatePen : public EMFRecAccessGDIObjectCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_EXTCREATEPEN"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeExtCreatePen; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecPolyTextOutA : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYTEXTOUTA"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyTextOutA; }
};

class EMFRecAccessGDIRecPolyTextOutW : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYTEXTOUTW"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyTextOutW; }
};

class EMFRecAccessGDIRecSetICMMode : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETICMMODE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetICMMode; }
};

class EMFRecAccessGDIRecCreateColorSpace : public EMFRecAccessGDIObjectCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_CREATECOLORSPACE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeCreateColorSpace; }
};

class EMFRecAccessGDIRecSetColorSpace : public EMFRecAccessGDIObjManipulationCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETCOLORSPACE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetColorSpace; }
};

class EMFRecAccessGDIRecDeleteColorSpace : public EMFRecAccessGDIObjManipulationCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_DELETECOLORSPACE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeDeleteColorSpace; }
};

class EMFRecAccessGDIRecGLSRecord : public EMFRecAccessGDIOpenGLCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_GLSRECORD"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeGLSRecord; }
};

class EMFRecAccessGDIRecGLSBoundedRecord : public EMFRecAccessGDIOpenGLCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_GLSBOUNDEDRECORD"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeGLSBoundedRecord; }
};

class EMFRecAccessGDIRecPixelFormat : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_PIXELFORMAT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePixelFormat; }
};

class EMFRecAccessGDIRecDrawEscape : public EMFRecAccessGDIEscapeCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_RESERVED_105"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeDrawEscape; }
};

class EMFRecAccessGDIRecExtEscape : public EMFRecAccessGDIEscapeCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_RESERVED_106/EMR_EXTESCAPE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeExtEscape; }
};

class EMFRecAccessGDIRecStartDoc : public EMFRecAccessGDIRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_RESERVED_107/EMR_STARTDOC"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeStartDoc; }

	RecCategory GetRecordCategory() const override { return RecCategoryReserved; }
};

class EMFRecAccessGDIRecSmallTextOut : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_RESERVED_108/EMR_SMALLTEXTOUT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSmallTextOut; }
};

class EMFRecAccessGDIRecForceUFIMapping : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_RESERVED_109/EMR_FORCEUFIMAPPING"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeForceUFIMapping; }
};

class EMFRecAccessGDIRecNamedEscape : public EMFRecAccessGDIEscapeCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_RESERVED_110/EMR_NAMEDESCAPE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeNamedEscape; }
};

class EMFRecAccessGDIRecColorCorrectPalette : public EMFRecAccessGDIObjManipulationCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_COLORCORRECTPALETTE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeColorCorrectPalette; }
};

class EMFRecAccessGDIRecSetICMProfileA : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETICMPROFILEA"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetICMProfileA; }
};

class EMFRecAccessGDIRecSetICMProfileW : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETICMPROFILEW"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetICMProfileW; }
};

class EMFRecAccessGDIRecAlphaBlend : public EMFRecAccessGDIBitmapCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_ALPHABLEND"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeAlphaBlend; }
};

class EMFRecAccessGDIRecSetLayout : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETLAYOUT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetLayout; }
};

class EMFRecAccessGDIRecTransparentBlt : public EMFRecAccessGDIBitmapCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_TRANSPARENTBLT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeTransparentBlt; }
};

class EMFRecAccessGDIRecReserved_117 : public EMFRecAccessGDIRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_Reserved_117"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeReserved_117; }

	RecCategory GetRecordCategory() const override { return RecCategoryReserved; }
};

class EMFRecAccessGDIRecGradientFill : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_GRADIENTFILL"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeGradientFill; }
};

class EMFRecAccessGDIRecSetLinkedUFIs : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_RESERVED_119/EMR_SetLinkedUFIs"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetLinkedUFIs; }
};

class EMFRecAccessGDIRecSetTextJustification : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_RESERVED_120/EMR_SETTEXTJUSTIFICATION"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetTextJustification; }
};

class EMFRecAccessGDIRecColorMatchToTargetW : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_COLORMATCHTOTARGETW"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeColorMatchToTargetW; }
};

class EMFRecAccessGDIRecCreateColorSpaceW : public EMFRecAccessGDIObjectCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_CREATECOLORSPACEW"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeCreateColorSpaceW; }
};

#endif // EMF_REC_ACCESS_GDI_H