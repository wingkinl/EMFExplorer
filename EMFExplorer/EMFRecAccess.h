#ifndef EMF_REC_ACCESS_H
#define EMF_REC_ACCESS_H

#include <memory>
#include <vector>
#include "GdiplusEnums.h"
#include "EmfPlusStruct.h"
#include "PropertyTree.h"

class EMFAccess;

class EMFRecAccess
{
public:
	EMFRecAccess() = default;
	virtual ~EMFRecAccess() = default;
public:
	virtual LPCWSTR GetRecordName() const = 0;

	virtual emfplus::OEmfPlusRecordType GetRecordType() const = 0;

	enum RecCategory
	{
		RecCategoryClipping,
		RecCategoryComment,
		RecCategoryControl,
		RecCategoryDrawing,
		RecCategoryObject,
		RecCategoryProperty,
		RecCategoryState,
		RecCategoryTerminalServer,
		RecCategoryTransform,
		RecCategoryBitmap,
		RecCategoryEscape,
		RecCategoryObjManipulation,
		RecCategoryOpenGL,
		RecCategoryPathBracket,
		RecCategoryReserved,
	};
	virtual RecCategory GetRecordCategory() const = 0;

	// Don't be confused with RecCategoryDrawing sicne there are other records that can draw
	// such as BitBlt
	bool IsDrawingRecord() const;

	virtual bool IsGDIRecord() const = 0;

	inline bool IsGDIPlusRecord() const { return !IsGDIRecord(); }

	const emfplus::OEmfPlusRecInfo& GetRecInfo() const { return m_recInfo; }

	std::shared_ptr<PropertyNode> GetProperties(EMFAccess* pEMF);

	inline size_t GetIndex() const { return m_nIndex; }

	bool IsLinked(const EMFRecAccess* pRec) const;

	inline size_t GetLinkedObjectCount() const { return m_linkRecs.size(); }

	inline EMFRecAccess* GetLinkedObject(size_t index)
	{
		// Caller ensure safety
		return m_linkRecs[index];
	}
protected:
	void SetRecInfo(const emfplus::OEmfPlusRecInfo& info) { m_recInfo = info; }

	void SetIndex(size_t nIndex) { m_nIndex = nIndex; }

	virtual void CacheProperties(EMFAccess* pEMF);

	virtual void Preprocess(EMFAccess* pEMF) {}

	void AddLinkRecord(EMFRecAccess* pRecAccess, bool bMutually = true);
protected:
	friend class EMFAccess;
	emfplus::OEmfPlusRecInfo		m_recInfo;
	size_t							m_nIndex = 0;
	std::shared_ptr<PropertyNode>	m_propsCached;
	std::vector<EMFRecAccess*>		m_linkRecs;
};

class EMFRecAccessGDIRec : public EMFRecAccess
{
public:
	bool IsGDIRecord() const override { return true; }

	void CacheProperties(EMFAccess* pEMF) override;

	virtual void CachePropertiesFromGDI(EMFAccess* pEMF, const ENHMETARECORD* pEMFRec) {}
};

class EMFRecAccessGDIBitmapRec : public EMFRecAccessGDIRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryBitmap; }
};

class EMFRecAccessGDIClippingRec : public EMFRecAccessGDIRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryClipping; }
};

class EMFRecAccessGDIControlRec : public EMFRecAccessGDIRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryControl; }
};

class EMFRecAccessGDIDrawingRec : public EMFRecAccessGDIRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryDrawing; }
};

class EMFRecAccessGDIEscapeRec : public EMFRecAccessGDIRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryEscape; }
};

class EMFRecAccessGDIObjectRec : public EMFRecAccessGDIRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryObject; }
};

class EMFRecAccessGDIObjManipulationRec : public EMFRecAccessGDIRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryObjManipulation; }
};

class EMFRecAccessGDIOpenGLRec : public EMFRecAccessGDIRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryOpenGL; }
};

class EMFRecAccessGDIPathBracketRec : public EMFRecAccessGDIRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryPathBracket; }
};

class EMFRecAccessGDIStateRec : public EMFRecAccessGDIRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryState; }
};

class EMFRecAccessGDITransformRec : public EMFRecAccessGDIRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryTransform; }
};

class EMFRecAccessGDIRecHeader : public EMFRecAccessGDIControlRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_HEADER"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeHeader; }

	void CachePropertiesFromGDI(EMFAccess* pEMF, const ENHMETARECORD* pEMFRec) override;
};

class EMFRecAccessGDIRecPolyBezier : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYBEZIER"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyBezier; }
};

class EMFRecAccessGDIRecPolygon : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYGON"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolygon; }
};

class EMFRecAccessGDIRecPolyline : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYLINE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyline; }
};

class EMFRecAccessGDIRecPolyBezierTo : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYBEZIERTO"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyBezierTo; }
};

class EMFRecAccessGDIRecPolyLineTo : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYLINETO"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyLineTo; }
};

class EMFRecAccessGDIRecPolyPolyline : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYPOLYLINE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyPolyline; }
};

class EMFRecAccessGDIRecPolyPolygon : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYPOLYGON"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyPolygon; }
};

class EMFRecAccessGDIRecSetWindowExtEx : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETWINDOWEXTEX"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetWindowExtEx; }
};

class EMFRecAccessGDIRecSetWindowOrgEx : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETWINDOWORGEX"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetWindowOrgEx; }
};

class EMFRecAccessGDIRecSetViewportExtEx : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETVIEWPORTEXTEX"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetViewportExtEx; }
};

class EMFRecAccessGDIRecSetViewportOrgEx : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETVIEWPORTORGEX"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetViewportOrgEx; }
};

class EMFRecAccessGDIRecSetBrushOrgEx : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETBRUSHORGEX"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetBrushOrgEx; }
};

class EMFRecAccessGDIRecEOF : public EMFRecAccessGDIControlRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_EOF"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeEOF; }
};

class EMFRecAccessGDIRecSetPixelV : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETPIXELV"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetPixelV; }
};

class EMFRecAccessGDIRecSetMapperFlags : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETMAPPERFLAGS"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetMapperFlags; }
};

class EMFRecAccessGDIRecSetMapMode : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETMAPMODE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetMapMode; }
};

class EMFRecAccessGDIRecSetBkMode : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETBKMODE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetBkMode; }
};

class EMFRecAccessGDIRecSetPolyFillMode : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETPOLYFILLMODE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetPolyFillMode; }
};

class EMFRecAccessGDIRecSetROP2 : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETROP2"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetROP2; }
};

class EMFRecAccessGDIRecSetStretchBltMode : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETSTRETCHBLTMODE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetStretchBltMode; }
};

class EMFRecAccessGDIRecSetTextAlign : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETTEXTALIGN"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetTextAlign; }
};

class EMFRecAccessGDIRecSetColorAdjustment : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETCOLORADJUSTMENT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetColorAdjustment; }
};

class EMFRecAccessGDIRecSetTextColor : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETTEXTCOLOR"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetTextColor; }
};

class EMFRecAccessGDIRecSetBkColor : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETBKCOLOR"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetBkColor; }
};

class EMFRecAccessGDIRecOffsetClipRgn : public EMFRecAccessGDIClippingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_OFFSETCLIPRGN"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeOffsetClipRgn; }
};

class EMFRecAccessGDIRecMoveToEx : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_MOVETOEX"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeMoveToEx; }
};

class EMFRecAccessGDIRecSetMetaRgn : public EMFRecAccessGDIClippingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETMETARGN"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetMetaRgn; }
};

class EMFRecAccessGDIRecExcludeClipRect : public EMFRecAccessGDIClippingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_EXCLUDECLIPRECT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeExcludeClipRect; }
};

class EMFRecAccessGDIRecIntersectClipRect : public EMFRecAccessGDIClippingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_INTERSECTCLIPRECT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeIntersectClipRect; }
};

class EMFRecAccessGDIRecScaleViewportExtEx : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SCALEVIEWPORTEXTEX"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeScaleViewportExtEx; }
};

class EMFRecAccessGDIRecScaleWindowExtEx : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SCALEWINDOWEXTEX"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeScaleWindowExtEx; }
};

class EMFRecAccessGDIRecSaveDC : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SAVEDC"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSaveDC; }
};

class EMFRecAccessGDIRecRestoreDC : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_RESTOREDC"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeRestoreDC; }
};

class EMFRecAccessGDIRecSetWorldTransform : public EMFRecAccessGDITransformRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETWORLDTRANSFORM"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetWorldTransform; }
};

class EMFRecAccessGDIRecModifyWorldTransform : public EMFRecAccessGDITransformRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_MODIFYWORLDTRANSFORM"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeModifyWorldTransform; }
};

class EMFRecAccessGDIRecSelectObject : public EMFRecAccessGDIObjManipulationRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SELECTOBJECT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSelectObject; }
};

class EMFRecAccessGDIRecCreatePen : public EMFRecAccessGDIObjectRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_CREATEPEN"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeCreatePen; }
};

class EMFRecAccessGDIRecCreateBrushIndirect : public EMFRecAccessGDIObjectRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_CREATEBRUSHINDIRECT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeCreateBrushIndirect; }
};

class EMFRecAccessGDIRecDeleteObject : public EMFRecAccessGDIObjManipulationRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_DELETEOBJECT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeDeleteObject; }
};

class EMFRecAccessGDIRecAngleArc : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_ANGLEARC"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeAngleArc; }
};

class EMFRecAccessGDIRecEllipse : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_ELLIPSE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeEllipse; }
};

class EMFRecAccessGDIRecRectangle : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_RECTANGLE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeRectangle; }
};

class EMFRecAccessGDIRecRoundRect : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_ROUNDRECT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeRoundRect; }
};

class EMFRecAccessGDIRecArc : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_ARC"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeArc; }
};

class EMFRecAccessGDIRecChord : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_CHORD"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeChord; }
};

class EMFRecAccessGDIRecPie : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_PIE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePie; }
};

class EMFRecAccessGDIRecSelectPalette : public EMFRecAccessGDIObjManipulationRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SELECTPALETTE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSelectPalette; }
};

class EMFRecAccessGDIRecCreatePalette : public EMFRecAccessGDIObjectRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_CREATEPALETTE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeCreatePalette; }
};

class EMFRecAccessGDIRecSetPaletteEntries : public EMFRecAccessGDIObjManipulationRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETPALETTEENTRIES"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetPaletteEntries; }
};

class EMFRecAccessGDIRecResizePalette : public EMFRecAccessGDIObjManipulationRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_RESIZEPALETTE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeResizePalette; }
};

class EMFRecAccessGDIRecRealizePalette : public EMFRecAccessGDIObjManipulationRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_REALIZEPALETTE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeRealizePalette; }
};

class EMFRecAccessGDIRecExtFloodFill : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_EXTFLOODFILL"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeExtFloodFill; }
};

class EMFRecAccessGDIRecLineTo : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_LINETO"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeLineTo; }
};

class EMFRecAccessGDIRecArcTo : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_ARCTO"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeArcTo; }
};

class EMFRecAccessGDIRecPolyDraw : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYDRAW"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyDraw; }
};

class EMFRecAccessGDIRecSetArcDirection : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETARCDIRECTION"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetArcDirection; }
};

class EMFRecAccessGDIRecSetMiterLimit : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETMITERLIMIT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetMiterLimit; }
};

class EMFRecAccessGDIRecBeginPath : public EMFRecAccessGDIPathBracketRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_BEGINPATH"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeBeginPath; }
};

class EMFRecAccessGDIRecEndPath : public EMFRecAccessGDIPathBracketRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_ENDPATH"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeEndPath; }
};

class EMFRecAccessGDIRecCloseFigure : public EMFRecAccessGDIPathBracketRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_CLOSEFIGURE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeCloseFigure; }
};

class EMFRecAccessGDIRecFillPath : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_FILLPATH"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeFillPath; }
};

class EMFRecAccessGDIRecStrokeAndFillPath : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_STROKEANDFILLPATH"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeStrokeAndFillPath; }
};

class EMFRecAccessGDIRecStrokePath : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_STROKEPATH"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeStrokePath; }
};

class EMFRecAccessGDIRecFlattenPath : public EMFRecAccessGDIPathBracketRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_FLATTENPATH"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeFlattenPath; }
};

class EMFRecAccessGDIRecWidenPath : public EMFRecAccessGDIPathBracketRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_WIDENPATH"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeWidenPath; }
};

class EMFRecAccessGDIRecSelectClipPath : public EMFRecAccessGDIClippingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SELECTCLIPPATH"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSelectClipPath; }
};

class EMFRecAccessGDIRecAbortPath : public EMFRecAccessGDIPathBracketRec
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

	void CachePropertiesFromGDI(EMFAccess* pEMF, const ENHMETARECORD* pEMFRec) override;
};

class EMFRecAccessGDIRecFillRgn : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_FILLRGN"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeFillRgn; }
};

class EMFRecAccessGDIRecFrameRgn : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_FRAMERGN"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeFrameRgn; }
};

class EMFRecAccessGDIRecInvertRgn : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_INVERTRGN"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeInvertRgn; }
};

class EMFRecAccessGDIRecPaintRgn : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_PAINTRGN"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePaintRgn; }
};

class EMFRecAccessGDIRecExtSelectClipRgn : public EMFRecAccessGDIClippingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_EXTSELECTCLIPRGN"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeExtSelectClipRgn; }
};

class EMFRecAccessGDIRecBitBlt : public EMFRecAccessGDIBitmapRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_BITBLT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeBitBlt; }
};

class EMFRecAccessGDIRecStretchBlt : public EMFRecAccessGDIBitmapRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_STRETCHBLT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeStretchBlt; }
};

class EMFRecAccessGDIRecMaskBlt : public EMFRecAccessGDIBitmapRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_MASKBLT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeMaskBlt; }
};

class EMFRecAccessGDIRecPlgBlt : public EMFRecAccessGDIBitmapRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_PLGBLT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePlgBlt; }
};

class EMFRecAccessGDIRecSetDIBitsToDevice : public EMFRecAccessGDIBitmapRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETDIBITSTODEVICE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetDIBitsToDevice; }
};

class EMFRecAccessGDIRecStretchDIBits : public EMFRecAccessGDIBitmapRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_STRETCHDIBITS"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeStretchDIBits; }
};

class EMFRecAccessGDIRecExtCreateFontIndirect : public EMFRecAccessGDIObjectRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_EXTCREATEFONTINDIRECTW"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeExtCreateFontIndirect; }
};

class EMFRecAccessGDIRecExtTextOutA : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_EXTTEXTOUTA"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeExtTextOutA; }
};

class EMFRecAccessGDIRecExtTextOutW : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_EXTTEXTOUTW"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeExtTextOutW; }
};

class EMFRecAccessGDIRecPolyBezier16 : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYBEZIER16"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyBezier16; }
};

class EMFRecAccessGDIRecPolygon16 : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYGON16"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolygon16; }
};

class EMFRecAccessGDIRecPolyline16 : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYLINE16"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyline16; }
};

class EMFRecAccessGDIRecPolyBezierTo16 : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYBEZIERTO16"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyBezierTo16; }
};

class EMFRecAccessGDIRecPolylineTo16 : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYLINETO16"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolylineTo16; }
};

class EMFRecAccessGDIRecPolyPolyline16 : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYPOLYLINE16"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyPolyline16; }
};

class EMFRecAccessGDIRecPolyPolygon16 : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYPOLYGON16"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyPolygon16; }
};

class EMFRecAccessGDIRecPolyDraw16 : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYDRAW16"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyDraw16; }
};

class EMFRecAccessGDIRecCreateMonoBrush : public EMFRecAccessGDIObjectRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_CREATEMONOBRUSH"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeCreateMonoBrush; }
};

class EMFRecAccessGDIRecCreateDIBPatternBrushPt : public EMFRecAccessGDIObjectRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_CREATEDIBPATTERNBRUSHPT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeCreateDIBPatternBrushPt; }
};

class EMFRecAccessGDIRecExtCreatePen : public EMFRecAccessGDIObjectRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_EXTCREATEPEN"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeExtCreatePen; }
};

class EMFRecAccessGDIRecPolyTextOutA : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYTEXTOUTA"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyTextOutA; }
};

class EMFRecAccessGDIRecPolyTextOutW : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_POLYTEXTOUTW"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePolyTextOutW; }
};

class EMFRecAccessGDIRecSetICMMode : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETICMMODE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetICMMode; }
};

class EMFRecAccessGDIRecCreateColorSpace : public EMFRecAccessGDIObjectRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_CREATECOLORSPACE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeCreateColorSpace; }
};

class EMFRecAccessGDIRecSetColorSpace : public EMFRecAccessGDIObjManipulationRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETCOLORSPACE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetColorSpace; }
};

class EMFRecAccessGDIRecDeleteColorSpace : public EMFRecAccessGDIObjManipulationRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_DELETECOLORSPACE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeDeleteColorSpace; }
};

class EMFRecAccessGDIRecGLSRecord : public EMFRecAccessGDIOpenGLRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_GLSRECORD"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeGLSRecord; }
};

class EMFRecAccessGDIRecGLSBoundedRecord : public EMFRecAccessGDIOpenGLRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_GLSBOUNDEDRECORD"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeGLSBoundedRecord; }
};

class EMFRecAccessGDIRecPixelFormat : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_PIXELFORMAT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypePixelFormat; }
};

class EMFRecAccessGDIRecDrawEscape : public EMFRecAccessGDIEscapeRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_RESERVED_105"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeDrawEscape; }
};

class EMFRecAccessGDIRecExtEscape : public EMFRecAccessGDIEscapeRec
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

class EMFRecAccessGDIRecSmallTextOut : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_RESERVED_108/EMR_SMALLTEXTOUT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSmallTextOut; }
};

class EMFRecAccessGDIRecForceUFIMapping : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_RESERVED_109/EMR_FORCEUFIMAPPING"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeForceUFIMapping; }
};

class EMFRecAccessGDIRecNamedEscape : public EMFRecAccessGDIEscapeRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_RESERVED_110/EMR_NAMEDESCAPE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeNamedEscape; }
};

class EMFRecAccessGDIRecColorCorrectPalette : public EMFRecAccessGDIObjManipulationRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_COLORCORRECTPALETTE"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeColorCorrectPalette; }
};

class EMFRecAccessGDIRecSetICMProfileA : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETICMPROFILEA"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetICMProfileA; }
};

class EMFRecAccessGDIRecSetICMProfileW : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETICMPROFILEW"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetICMProfileW; }
};

class EMFRecAccessGDIRecAlphaBlend : public EMFRecAccessGDIBitmapRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_ALPHABLEND"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeAlphaBlend; }
};

class EMFRecAccessGDIRecSetLayout : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_SETLAYOUT"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetLayout; }
};

class EMFRecAccessGDIRecTransparentBlt : public EMFRecAccessGDIBitmapRec
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

class EMFRecAccessGDIRecGradientFill : public EMFRecAccessGDIDrawingRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_GRADIENTFILL"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeGradientFill; }
};

class EMFRecAccessGDIRecSetLinkedUFIs : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_RESERVED_119/EMR_SetLinkedUFIs"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetLinkedUFIs; }
};

class EMFRecAccessGDIRecSetTextJustification : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_RESERVED_120/EMR_SETTEXTJUSTIFICATION"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeSetTextJustification; }
};

class EMFRecAccessGDIRecColorMatchToTargetW : public EMFRecAccessGDIStateRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_COLORMATCHTOTARGETW"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeColorMatchToTargetW; }
};

class EMFRecAccessGDIRecCreateColorSpaceW : public EMFRecAccessGDIObjectRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EMR_CREATECOLORSPACEW"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfRecordTypeCreateColorSpaceW; }
};


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

class EMFRecAccessGDIPlusRec : public EMFRecAccess
{
public:
	bool IsGDIRecord() const override { return false; }

	void CacheProperties(EMFAccess* pEMF) override;
};

class EMFRecAccessGDIPlusClippingCat : public EMFRecAccessGDIPlusRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryClipping; }
};

class EMFRecAccessGDIPlusControlCat : public EMFRecAccessGDIPlusRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryControl; }
};

class EMFRecAccessGDIPlusDrawingCat : public EMFRecAccessGDIPlusRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryDrawing; }
};

class EMFRecAccessGDIPlusObjectCat : public EMFRecAccessGDIPlusRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryObject; }
};

class EMFRecAccessGDIPlusPropertyCat : public EMFRecAccessGDIPlusRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryProperty; }
};

class EMFRecAccessGDIPlusStateCat : public EMFRecAccessGDIPlusRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryState; }
};

class EMFRecAccessGDIPlusTerminalServerCat : public EMFRecAccessGDIPlusRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryTerminalServer; }
};

class EMFRecAccessGDIPlusTransformCat : public EMFRecAccessGDIPlusRec
{
public:
	RecCategory GetRecordCategory() const override { return RecCategoryTransform; }
};

class EMFRecAccessGDIPlusRecHeader : public EMFRecAccessGDIPlusControlCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusHeader"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeHeader; }
private:
	emfplus::OEmfPlusHeader	m_recDataCached;
};

class EMFRecAccessGDIPlusRecEndOfFile : public EMFRecAccessGDIPlusControlCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusEndOfFile"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeEndOfFile; }
};

class EMFRecAccessGDIPlusRecComment : public EMFRecAccessGDIPlusRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusComment"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeComment; }

	RecCategory GetRecordCategory() const override { return RecCategoryComment; }
private:
	emfplus::OEmfPlusRecComment	m_recDataCached;
};

class EMFRecAccessGDIPlusRecGetDC : public EMFRecAccessGDIPlusControlCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusGetDC"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeGetDC; }
};

class EMFRecAccessGDIPlusRecMultiFormatStart : public EMFRecAccessGDIPlusRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusMultiFormatStart"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeMultiFormatStart; }

	RecCategory GetRecordCategory() const override { return RecCategoryReserved; }
};

class EMFRecAccessGDIPlusRecMultiFormatSection : public EMFRecAccessGDIPlusRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusMultiFormatSection"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeMultiFormatSection; }

	RecCategory GetRecordCategory() const override { return RecCategoryReserved; }
};

class EMFRecAccessGDIPlusRecMultiFormatEnd : public EMFRecAccessGDIPlusRec
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusMultiFormatEnd"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeMultiFormatEnd; }

	RecCategory GetRecordCategory() const override { return RecCategoryReserved; }
};

class EMFRecAccessGDIPlusObjWrapper
{
public:
	EMFRecAccessGDIPlusObjWrapper() = default;
	virtual ~EMFRecAccessGDIPlusObjWrapper() = default;

	emfplus::OEmfPlusGraphObject* GetObject() const { return m_obj.get(); }

	virtual bool CacheGDIPlusObject() { return false; }

	virtual std::shared_ptr<EMFAccess> GetEMFAccess() const { return nullptr; }
protected:
	friend class EMFRecAccessGDIPlusRecObject;

	virtual void CacheProperties(EMFAccess* pEMF, PropertyNode* pNode) const {}
protected:
	std::unique_ptr<emfplus::OEmfPlusGraphObject>	m_obj;
};

class EMFRecAccessGDIPlusRecObject : public EMFRecAccessGDIPlusObjectCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusObject"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeObject; }

	EMFRecAccessGDIPlusObjWrapper* GetObjectWrapper();
private:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(EMFAccess* pEMF) override;
private:
	std::unique_ptr<EMFRecAccessGDIPlusObjWrapper>	m_recDataCached;
};

class EMFRecAccessGDIPlusRecClear : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusClear"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeClear; }
private:
	emfplus::OEmfPlusRecClear	m_recDataCached;
};

class EMFRecAccessGDIPlusRecFillRects : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusFillRects"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeFillRects; }
private:
	void Preprocess(EMFAccess* pEMF) override;
private:
	emfplus::OEmfPlusRecFillRects	m_recDataCached;
};

class EMFRecAccessGDIPlusRecDrawRects : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusDrawRects"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeDrawRects; }
private:
	void Preprocess(EMFAccess* pEMF) override;
private:
	emfplus::OEmfPlusRecDrawRects	m_recDataCached;
};

class EMFRecAccessGDIPlusRecFillPolygon : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusFillPolygon"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeFillPolygon; }
private:
	void Preprocess(EMFAccess* pEMF) override;
private:
	emfplus::OEmfPlusRecFillPolygon	m_recDataCached;
};

class EMFRecAccessGDIPlusRecDrawLines : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusDrawLines"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeDrawLines; }
private:
	void Preprocess(EMFAccess* pEMF) override;
private:
	emfplus::OEmfPlusRecDrawLines	m_recDataCached;
};

class EMFRecAccessGDIPlusRecFillEllipse : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusFillEllipse"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeFillEllipse; }
private:
	void Preprocess(EMFAccess* pEMF) override;
private:
	emfplus::OEmfPlusRecFillEllipse	m_recDataCached;
};

class EMFRecAccessGDIPlusRecDrawEllipse : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusDrawEllipse"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeDrawEllipse; }
private:
	void Preprocess(EMFAccess* pEMF) override;
private:
	emfplus::OEmfPlusRecDrawEllipse	m_recDataCached;
};

class EMFRecAccessGDIPlusRecFillPie : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusFillPie"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeFillPie; }
private:
	void Preprocess(EMFAccess* pEMF) override;
private:
	emfplus::OEmfPlusRecFillPie	m_recDataCached;
};

class EMFRecAccessGDIPlusRecDrawPie : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusDrawPie"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeDrawPie; }
private:
	void Preprocess(EMFAccess* pEMF) override;
private:
	emfplus::OEmfPlusRecDrawPie	m_recDataCached;
};

class EMFRecAccessGDIPlusRecDrawArc : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusDrawArc"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeDrawArc; }
private:
	void Preprocess(EMFAccess* pEMF) override;
private:
	emfplus::OEmfPlusRecDrawArc	m_recDataCached;
};

class EMFRecAccessGDIPlusRecFillRegion : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusFillRegion"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeFillRegion; }
private:
	void Preprocess(EMFAccess* pEMF) override;
private:
	emfplus::OEmfPlusRecFillRegion	m_recDataCached;
};

class EMFRecAccessGDIPlusRecFillPath : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusFillPath"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeFillPath; }
private:
	void Preprocess(EMFAccess* pEMF) override;
private:
	emfplus::OEmfPlusRecFillPath	m_recDataCached;
};

class EMFRecAccessGDIPlusRecDrawPath : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusDrawPath"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeDrawPath; }
private:
	void Preprocess(EMFAccess* pEMF) override;
private:
	emfplus::OEmfPlusRecDrawPath	m_recDataCached;
};

class EMFRecAccessGDIPlusRecFillClosedCurve : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusFillClosedCurve"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeFillClosedCurve; }
private:
	emfplus::OEmfPlusRecFillClosedCurve	m_recDataCached;
};

class EMFRecAccessGDIPlusRecDrawClosedCurve : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusDrawClosedCurve"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeDrawClosedCurve; }
private:
	emfplus::OEmfPlusRecDrawClosedCurve	m_recDataCached;
};

class EMFRecAccessGDIPlusRecDrawCurve : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusDrawCurve"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeDrawCurve; }
private:
	emfplus::OEmfPlusRecDrawCurve	m_recDataCached;
};

class EMFRecAccessGDIPlusRecDrawBeziers : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusDrawBeziers"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeDrawBeziers; }
private:
	void Preprocess(EMFAccess* pEMF) override;
private:
	emfplus::OEmfPlusRecDrawBeziers	m_recDataCached;
};

class EMFRecAccessGDIPlusRecDrawImage : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusDrawImage"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeDrawImage; }
private:
	void Preprocess(EMFAccess* pEMF) override;
private:
	emfplus::OEmfPlusRecDrawImage	m_recDataCached;
};

class EMFRecAccessGDIPlusRecDrawImagePoints : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusDrawImagePoints"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeDrawImagePoints; }
private:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(EMFAccess* pEMF) override;
private:
	emfplus::OEmfPlusRecDrawImagePoints	m_recDataCached;
};

class EMFRecAccessGDIPlusRecDrawString : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusDrawString"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeDrawString; }
private:
	void Preprocess(EMFAccess* pEMF) override;
private:
	emfplus::OEmfPlusRecDrawString	m_recDataCached;
};

class EMFRecAccessGDIPlusRecSetRenderingOrigin : public EMFRecAccessGDIPlusPropertyCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSetRenderingOrigin"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSetRenderingOrigin; }
private:
	emfplus::OEmfPlusRecSetRenderingOrigin	m_recDataCached;
};

class EMFRecAccessGDIPlusRecSetAntiAliasMode : public EMFRecAccessGDIPlusPropertyCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSetAntiAliasMode"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSetAntiAliasMode; }
};

class EMFRecAccessGDIPlusRecSetTextRenderingHint : public EMFRecAccessGDIPlusPropertyCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSetTextRenderingHint"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSetTextRenderingHint; }
};

class EMFRecAccessGDIPlusRecSetTextContrast : public EMFRecAccessGDIPlusPropertyCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSetTextContrast"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSetTextContrast; }
};

class EMFRecAccessGDIPlusRecSetInterpolationMode : public EMFRecAccessGDIPlusPropertyCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSetInterpolationMode"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSetInterpolationMode; }
};

class EMFRecAccessGDIPlusRecSetPixelOffsetMode : public EMFRecAccessGDIPlusPropertyCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSetPixelOffsetMode"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSetPixelOffsetMode; }
};

class EMFRecAccessGDIPlusRecSetCompositingMode : public EMFRecAccessGDIPlusPropertyCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSetCompositingMode"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSetCompositingMode; }
};

class EMFRecAccessGDIPlusRecSetCompositingQuality : public EMFRecAccessGDIPlusPropertyCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSetCompositingQuality"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSetCompositingQuality; }
};

class EMFRecAccessGDIPlusRecSave : public EMFRecAccessGDIPlusStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSave"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSave; }
private:
	emfplus::OEmfPlusRecSave	m_recDataCached;
};

class EMFRecAccessGDIPlusRecRestore : public EMFRecAccessGDIPlusStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusRestore"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeRestore; }
private:
	emfplus::OEmfPlusRecRestore	m_recDataCached;
};

class EMFRecAccessGDIPlusRecBeginContainer : public EMFRecAccessGDIPlusStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusBeginContainer"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeBeginContainer; }
private:
	emfplus::OEmfPlusRecBeginContainer	m_recDataCached;
};

class EMFRecAccessGDIPlusRecBeginContainerNoParams : public EMFRecAccessGDIPlusStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusBeginContainerNoParams"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeBeginContainerNoParams; }
private:
	emfplus::OEmfPlusRecBeginContainerNoParams	m_recDataCached;
};

class EMFRecAccessGDIPlusRecEndContainer : public EMFRecAccessGDIPlusStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusEndContainer"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeEndContainer; }
private:
	emfplus::OEmfPlusRecEndContainer	m_recDataCached;
};

class EMFRecAccessGDIPlusRecSetWorldTransform : public EMFRecAccessGDIPlusTransformCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSetWorldTransform"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSetWorldTransform; }
private:
	emfplus::OEmfPlusRecSetWorldTransform	m_recDataCached;
};

class EMFRecAccessGDIPlusRecResetWorldTransform : public EMFRecAccessGDIPlusTransformCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusResetWorldTransform"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeResetWorldTransform; }
};

class EMFRecAccessGDIPlusRecMultiplyWorldTransform : public EMFRecAccessGDIPlusTransformCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusMultiplyWorldTransform"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeMultiplyWorldTransform; }
private:
	emfplus::OEmfPlusRecMultiplyWorldTransform	m_recDataCached;
};

class EMFRecAccessGDIPlusRecTranslateWorldTransform : public EMFRecAccessGDIPlusTransformCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusTranslateWorldTransform"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeTranslateWorldTransform; }
private:
	emfplus::OEmfPlusRecTranslateWorldTransform	m_recDataCached;
};

class EMFRecAccessGDIPlusRecScaleWorldTransform : public EMFRecAccessGDIPlusTransformCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusScaleWorldTransform"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeScaleWorldTransform; }
private:
	emfplus::OEmfPlusRecScaleWorldTransform	m_recDataCached;
};

class EMFRecAccessGDIPlusRecRotateWorldTransform : public EMFRecAccessGDIPlusTransformCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusRotateWorldTransform"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeRotateWorldTransform; }
private:
	emfplus::OEmfPlusRecRotateWorldTransform	m_recDataCached;
};

class EMFRecAccessGDIPlusRecSetPageTransform : public EMFRecAccessGDIPlusTransformCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSetPageTransform"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSetPageTransform; }
private:
	emfplus::OEmfPlusRecSetPageTransform	m_recDataCached;
};

class EMFRecAccessGDIPlusRecResetClip : public EMFRecAccessGDIPlusClippingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusResetClip"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeResetClip; }
};

class EMFRecAccessGDIPlusRecSetClipRect : public EMFRecAccessGDIPlusClippingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSetClipRect"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSetClipRect; }
private:
	emfplus::OEmfPlusRecSetClipRect	m_recDataCached;
};

class EMFRecAccessGDIPlusRecSetClipPath : public EMFRecAccessGDIPlusClippingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSetClipPath"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSetClipPath; }
private:
	void Preprocess(EMFAccess* pEMF) override;
};

class EMFRecAccessGDIPlusRecSetClipRegion : public EMFRecAccessGDIPlusClippingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSetClipRegion"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSetClipRegion; }
private:
	void Preprocess(EMFAccess* pEMF) override;
};

class EMFRecAccessGDIPlusRecOffsetClip : public EMFRecAccessGDIPlusClippingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusOffsetClip"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeOffsetClip; }
private:
	emfplus::OEmfPlusRecOffsetClip	m_recDataCached;
};

class EMFRecAccessGDIPlusRecDrawDriverString : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusDrawDriverString"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeDrawDriverString; }
private:
	void Preprocess(EMFAccess* pEMF) override;
private:
	emfplus::OEmfPlusRecDrawDriverString	m_recDataCached;
};

class EMFRecAccessGDIPlusRecStrokeFillPath : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusStrokeFillPath"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeStrokeFillPath; }
};

class EMFRecAccessGDIPlusRecSerializableObject : public EMFRecAccessGDIPlusObjectCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSerializableObject"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSerializableObject; }
private:
	emfplus::OEmfPlusRecSerializableObject	m_recDataCached;
};

class EMFRecAccessGDIPlusRecSetTSGraphics : public EMFRecAccessGDIPlusTerminalServerCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSetTSGraphics"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSetTSGraphics; }
};

class EMFRecAccessGDIPlusRecSetTSClip : public EMFRecAccessGDIPlusTerminalServerCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSetTSClip"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSetTSClip; }
};

CRect GetFitRect(const CRect& rcDest, const SIZE& szSrc, bool bCenter = false);


class EMFAccess final
{
public:
	EMFAccess(const std::vector<emfplus::u8t>& vData);
	~EMFAccess();
public:
	const Gdiplus::MetafileHeader GetMetafileHeader() const { return m_hdr; }

	void DrawMetafile(Gdiplus::Graphics& gg, const CRect& rcDraw) const;

	Gdiplus::Image* CloneMetafile() const;

	inline size_t GetRecordCount() const { return m_EMFRecords.size(); }

	inline EMFRecAccess* GetRecord(size_t index) const
	{
		return m_EMFRecords[index];
	}

	bool GetRecords();

	void FreeRecords();

	bool HandleEMFRecord(emfplus::OEmfPlusRecordType type, UINT flags, UINT dataSize, const BYTE* data);

	EMFRecAccess* GetObjectCreationRecord(size_t index, bool bPlus) const;

	bool SetObjectToTable(size_t index, EMFRecAccess* pRec, bool bPlus);
protected:
	using EmfRecArray	= std::vector<EMFRecAccess*>;
	using EMFPtr		= std::unique_ptr<Gdiplus::Metafile>;

	EMFPtr					m_pMetafile;
	EmfRecArray				m_EMFRecords;
	Gdiplus::MetafileHeader	m_hdr;

	struct EMFPlusObjInfo 
	{
		EMFRecAccess* pRec;
	};
	std::vector<EMFPlusObjInfo>			m_vPlusObjTable;
	emfplus::OEmfPlusRecObjectReader	m_PlusRecObjReader;
};

#endif // EMF_REC_ACCESS_H
