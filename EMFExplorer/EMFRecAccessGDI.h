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

class EMFGDIRecPolygonPreviewHelper
{
public:
	bool DrawPreview(EMFRecAccess::PreviewContext* info, const EMRPOLYGON* pRec, emfplus::OEmfPlusRecordType nType);
private:
	CRect m_rcBounds;
};

class EMFRecAccessGDIRecPolyBezier : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYBEZIER"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyBezier; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;

	bool DrawPreview(PreviewContext* info = nullptr) override;
private:
	EMFGDIRecPolygonPreviewHelper	m_previewHelper;
};

class EMFRecAccessGDIRecPolygon : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYGON"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolygon; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;

	bool DrawPreview(PreviewContext* info = nullptr) override;
private:
	EMFGDIRecPolygonPreviewHelper	m_previewHelper;
};

class EMFRecAccessGDIRecPolyline : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYLINE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyline; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;

	bool DrawPreview(PreviewContext* info = nullptr) override;
private:
	EMFGDIRecPolygonPreviewHelper	m_previewHelper;
};

class EMFRecAccessGDIRecPolyBezierTo : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYBEZIERTO"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyBezierTo; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;

	bool DrawPreview(PreviewContext* info = nullptr) override;
private:
	EMFGDIRecPolygonPreviewHelper	m_previewHelper;
};

class EMFRecAccessGDIRecPolyLineTo : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYLINETO"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyLineTo; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;

	bool DrawPreview(PreviewContext* info = nullptr) override;
private:
	EMFGDIRecPolygonPreviewHelper	m_previewHelper;
};

class EMFRecAccessGDIRecPolyPolyline : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYPOLYLINE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyPolyline; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecPolyPolygon : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYPOLYGON"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyPolygon; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecSetWindowExtEx : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETWINDOWEXTEX"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetWindowExtEx; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecSetWindowOrgEx : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETWINDOWORGEX"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetWindowOrgEx; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecSetViewportExtEx : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETVIEWPORTEXTEX"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetViewportExtEx; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecSetViewportOrgEx : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETVIEWPORTORGEX"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetViewportOrgEx; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecSetBrushOrgEx : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETBRUSHORGEX"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetBrushOrgEx; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
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
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecSetMapperFlags : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETMAPPERFLAGS"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetMapperFlags; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecSetMapMode : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETMAPMODE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetMapMode; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecSetBkMode : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETBKMODE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetBkMode; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecSetPolyFillMode : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETPOLYFILLMODE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetPolyFillMode; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecSetROP2 : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETROP2"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetROP2; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecSetStretchBltMode : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETSTRETCHBLTMODE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetStretchBltMode; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecSetTextAlign : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETTEXTALIGN"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetTextAlign; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecSetColorAdjustment : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETCOLORADJUSTMENT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetColorAdjustment; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecSetTextColor : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETTEXTCOLOR"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetTextColor; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecSetBkColor : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETBKCOLOR"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetBkColor; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecOffsetClipRgn : public EMFRecAccessGDIClippingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_OFFSETCLIPRGN"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeOffsetClipRgn; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecMoveToEx : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_MOVETOEX"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeMoveToEx; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
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
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecIntersectClipRect : public EMFRecAccessGDIClippingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_INTERSECTCLIPRECT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeIntersectClipRect; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecScaleViewportExtEx : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SCALEVIEWPORTEXTEX"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeScaleViewportExtEx; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecScaleWindowExtEx : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SCALEWINDOWEXTEX"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeScaleWindowExtEx; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
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
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecModifyWorldTransform : public EMFRecAccessGDITransformCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_MODIFYWORLDTRANSFORM"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeModifyWorldTransform; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecSelectObject : public EMFRecAccessGDIObjManipulationCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SELECTOBJECT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSelectObject; }
private:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecCreatePen : public EMFRecAccessGDIObjectCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_CREATEPEN"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeCreatePen; }
private:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecCreateBrushIndirect : public EMFRecAccessGDIObjectCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_CREATEBRUSHINDIRECT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeCreateBrushIndirect; }
private:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecDeleteObject : public EMFRecAccessGDIObjManipulationCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_DELETEOBJECT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeDeleteObject; }
private:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecAngleArc : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_ANGLEARC"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeAngleArc; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecEllipse : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_ELLIPSE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeEllipse; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecRectangle : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_RECTANGLE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeRectangle; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecRoundRect : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_ROUNDRECT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeRoundRect; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecArc : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_ARC"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeArc; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecChord : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_CHORD"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeChord; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecPie : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_PIE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePie; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecSelectPalette : public EMFRecAccessGDIObjManipulationCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SELECTPALETTE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSelectPalette; }
private:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecCreatePalette : public EMFRecAccessGDIObjectCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_CREATEPALETTE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeCreatePalette; }
private:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecSetPaletteEntries : public EMFRecAccessGDIObjManipulationCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETPALETTEENTRIES"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetPaletteEntries; }
private:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecResizePalette : public EMFRecAccessGDIObjManipulationCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_RESIZEPALETTE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeResizePalette; }
private:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
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
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecLineTo : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_LINETO"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeLineTo; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecArcTo : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_ARCTO"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeArcTo; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecPolyDraw : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYDRAW"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyDraw; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecSetArcDirection : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETARCDIRECTION"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetArcDirection; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecSetMiterLimit : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETMITERLIMIT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetMiterLimit; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
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
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecStrokeAndFillPath : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_STROKEANDFILLPATH"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeStrokeAndFillPath; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecStrokePath : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_STROKEPATH"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeStrokePath; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
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
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
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
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecFrameRgn : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_FRAMERGN"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeFrameRgn; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecInvertRgn : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_INVERTRGN"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeInvertRgn; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecPaintRgn : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_PAINTRGN"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePaintRgn; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecExtSelectClipRgn : public EMFRecAccessGDIClippingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_EXTSELECTCLIPRGN"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeExtSelectClipRgn; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecBitBlt : public EMFRecAccessGDIBitmapCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_BITBLT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeBitBlt; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecStretchBlt : public EMFRecAccessGDIBitmapCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_STRETCHBLT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeStretchBlt; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecMaskBlt : public EMFRecAccessGDIBitmapCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_MASKBLT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeMaskBlt; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecPlgBlt : public EMFRecAccessGDIBitmapCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_PLGBLT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePlgBlt; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecSetDIBitsToDevice : public EMFRecAccessGDIBitmapCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETDIBITSTODEVICE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetDIBitsToDevice; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecStretchDIBits : public EMFRecAccessGDIBitmapCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_STRETCHDIBITS"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeStretchDIBits; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecExtCreateFontIndirect : public EMFRecAccessGDIObjectCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_EXTCREATEFONTINDIRECTW"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeExtCreateFontIndirect; }
private:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecExtTextOutA : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_EXTTEXTOUTA"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeExtTextOutA; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecExtTextOutW : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_EXTTEXTOUTW"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeExtTextOutW; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecPolyBezier16 : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYBEZIER16"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyBezier16; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecPolygon16 : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYGON16"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolygon16; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecPolyline16 : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYLINE16"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyline16; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecPolyBezierTo16 : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYBEZIERTO16"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyBezierTo16; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecPolylineTo16 : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYLINETO16"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolylineTo16; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecPolyPolyline16 : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYPOLYLINE16"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyPolyline16; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecPolyPolygon16 : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYPOLYGON16"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyPolygon16; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecPolyDraw16 : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYDRAW16"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyDraw16; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecCreateMonoBrush : public EMFRecAccessGDIObjectCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_CREATEMONOBRUSH"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeCreateMonoBrush; }
private:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecCreateDIBPatternBrushPt : public EMFRecAccessGDIObjectCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_CREATEDIBPATTERNBRUSHPT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeCreateDIBPatternBrushPt; }
private:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecExtCreatePen : public EMFRecAccessGDIObjectCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_EXTCREATEPEN"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeExtCreatePen; }
private:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecPolyTextOutA : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYTEXTOUTA"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyTextOutA; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecPolyTextOutW : public EMFRecAccessGDIDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYTEXTOUTW"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyTextOutW; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecSetICMMode : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETICMMODE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetICMMode; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecCreateColorSpace : public EMFRecAccessGDIObjectCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_CREATECOLORSPACE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeCreateColorSpace; }
private:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecSetColorSpace : public EMFRecAccessGDIObjManipulationCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETCOLORSPACE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetColorSpace; }
private:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecDeleteColorSpace : public EMFRecAccessGDIObjManipulationCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_DELETECOLORSPACE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeDeleteColorSpace; }
private:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
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
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
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
private:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecSetICMProfileA : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETICMPROFILEA"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetICMProfileA; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecSetICMProfileW : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETICMPROFILEW"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetICMProfileW; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecAlphaBlend : public EMFRecAccessGDIBitmapCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_ALPHABLEND"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeAlphaBlend; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecSetLayout : public EMFRecAccessGDIStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETLAYOUT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetLayout; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecTransparentBlt : public EMFRecAccessGDIBitmapCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_TRANSPARENTBLT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeTransparentBlt; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
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
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
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
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIRecCreateColorSpaceW : public EMFRecAccessGDIObjectCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_CREATECOLORSPACEW"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeCreateColorSpaceW; }
private:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

#endif // EMF_REC_ACCESS_GDI_H