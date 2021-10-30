#ifndef EMF_REC_ACCESS_PLUS_H
#define EMF_REC_ACCESS_PLUS_H

#include "EMFRecAccess.h"

class EMFRecAccessGDIPlusRec : public EMFRecAccess
{
public:
	bool IsGDIRecord() const override { return false; }
protected:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
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
protected:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
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

class EMFRecAccessGDIPlusRecObject;

class EMFRecAccessGDIPlusObjWrapper
{
public:
	EMFRecAccessGDIPlusObjWrapper() = default;
	virtual ~EMFRecAccessGDIPlusObjWrapper() = default;

	emfplus::OEmfPlusGraphObject* GetObject() const { return m_obj.get(); }

	virtual bool CacheGDIPlusObject(EMFAccess* pEMF) { return false; }

	virtual std::shared_ptr<EMFAccess> GetEMFAccess() const { return nullptr; }

	virtual bool DrawPreview(EMFRecAccess::PreviewContext* info = nullptr) { return false; }
protected:
	friend class EMFRecAccessGDIPlusRecObject;

	virtual void CacheProperties(const CachePropertiesContext& ctxt, PropertyNode* pNode) const;
protected:
	EMFRecAccessGDIPlusRecObject* m_pObjRec = nullptr;
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

	void CacheProperties(const CachePropertiesContext& ctxt) override;

	bool DrawPreview(PreviewContext* info = nullptr) override;
private:
	std::unique_ptr<EMFRecAccessGDIPlusObjWrapper>	m_recDataCached;
};

class EMFRecAccessGDIPlusRecClear : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusClear"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeClear; }
protected:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
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

	void CacheProperties(const CachePropertiesContext& ctxt) override;
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

	void CacheProperties(const CachePropertiesContext& ctxt) override;
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

	void CacheProperties(const CachePropertiesContext& ctxt) override;
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

	void CacheProperties(const CachePropertiesContext& ctxt) override;
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

	void CacheProperties(const CachePropertiesContext& ctxt) override;
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

	void CacheProperties(const CachePropertiesContext& ctxt) override;
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

	void CacheProperties(const CachePropertiesContext& ctxt) override;
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

	void CacheProperties(const CachePropertiesContext& ctxt) override;
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

	void CacheProperties(const CachePropertiesContext& ctxt) override;
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

	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIPlusRecFillPath : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusFillPath"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeFillPath; }
private:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIPlusRecDrawPath : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusDrawPath"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeDrawPath; }
private:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
private:
	emfplus::OEmfPlusRecDrawPath	m_recDataCached;
};

class EMFRecAccessGDIPlusRecFillClosedCurve : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusFillClosedCurve"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeFillClosedCurve; }
protected:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
private:
	emfplus::OEmfPlusRecFillClosedCurve	m_recDataCached;
};

class EMFRecAccessGDIPlusRecDrawClosedCurve : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusDrawClosedCurve"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeDrawClosedCurve; }
protected:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
private:
	emfplus::OEmfPlusRecDrawClosedCurve	m_recDataCached;
};

class EMFRecAccessGDIPlusRecDrawCurve : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusDrawCurve"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeDrawCurve; }
protected:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
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

	void CacheProperties(const CachePropertiesContext& ctxt) override;
private:
	emfplus::OEmfPlusRecDrawBeziers	m_recDataCached;
};

class EMFRecAccessGDIPlusRecDrawImage : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusDrawImage"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeDrawImage; }

	bool DrawPreview(PreviewContext* info = nullptr) override;
private:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
private:
	emfplus::OEmfPlusRecDrawImage	m_recDataCached;
};

class EMFRecAccessGDIPlusRecDrawImagePoints : public EMFRecAccessGDIPlusDrawingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusDrawImagePoints"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeDrawImagePoints; }

	bool DrawPreview(PreviewContext* info = nullptr) override;
private:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
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

	void CacheProperties(const CachePropertiesContext& ctxt) override;
private:
	emfplus::OEmfPlusRecDrawString	m_recDataCached;
};

class EMFRecAccessGDIPlusRecSetRenderingOrigin : public EMFRecAccessGDIPlusPropertyCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSetRenderingOrigin"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSetRenderingOrigin; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
private:
	emfplus::OEmfPlusRecSetRenderingOrigin	m_recDataCached;
};

class EMFRecAccessGDIPlusRecSetAntiAliasMode : public EMFRecAccessGDIPlusPropertyCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSetAntiAliasMode"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSetAntiAliasMode; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIPlusRecSetTextRenderingHint : public EMFRecAccessGDIPlusPropertyCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSetTextRenderingHint"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSetTextRenderingHint; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIPlusRecSetTextContrast : public EMFRecAccessGDIPlusPropertyCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSetTextContrast"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSetTextContrast; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIPlusRecSetInterpolationMode : public EMFRecAccessGDIPlusPropertyCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSetInterpolationMode"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSetInterpolationMode; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIPlusRecSetPixelOffsetMode : public EMFRecAccessGDIPlusPropertyCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSetPixelOffsetMode"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSetPixelOffsetMode; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIPlusRecSetCompositingMode : public EMFRecAccessGDIPlusPropertyCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSetCompositingMode"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSetCompositingMode; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIPlusRecSetCompositingQuality : public EMFRecAccessGDIPlusPropertyCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSetCompositingQuality"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSetCompositingQuality; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIPlusRecSave : public EMFRecAccessGDIPlusStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSave"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSave; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
private:
	emfplus::OEmfPlusRecSave	m_recDataCached;
};

class EMFRecAccessGDIPlusRecRestore : public EMFRecAccessGDIPlusStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusRestore"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeRestore; }
private:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
private:
	emfplus::OEmfPlusRecRestore	m_recDataCached;
};

class EMFRecAccessGDIPlusRecBeginContainer : public EMFRecAccessGDIPlusStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusBeginContainer"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeBeginContainer; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
private:
	emfplus::OEmfPlusRecBeginContainer	m_recDataCached;
};

class EMFRecAccessGDIPlusRecBeginContainerNoParams : public EMFRecAccessGDIPlusStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusBeginContainerNoParams"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeBeginContainerNoParams; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
private:
	emfplus::OEmfPlusRecBeginContainerNoParams	m_recDataCached;
};

class EMFRecAccessGDIPlusRecEndContainer : public EMFRecAccessGDIPlusStateCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusEndContainer"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeEndContainer; }
private:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
private:
	emfplus::OEmfPlusRecEndContainer	m_recDataCached;
};

class EMFRecAccessGDIPlusRecSetWorldTransform : public EMFRecAccessGDIPlusTransformCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSetWorldTransform"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSetWorldTransform; }
protected:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
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
protected:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
private:
	emfplus::OEmfPlusRecMultiplyWorldTransform	m_recDataCached;
};

class EMFRecAccessGDIPlusRecTranslateWorldTransform : public EMFRecAccessGDIPlusTransformCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusTranslateWorldTransform"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeTranslateWorldTransform; }
protected:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
private:
	emfplus::OEmfPlusRecTranslateWorldTransform	m_recDataCached;
};

class EMFRecAccessGDIPlusRecScaleWorldTransform : public EMFRecAccessGDIPlusTransformCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusScaleWorldTransform"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeScaleWorldTransform; }
protected:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
private:
	emfplus::OEmfPlusRecScaleWorldTransform	m_recDataCached;
};

class EMFRecAccessGDIPlusRecRotateWorldTransform : public EMFRecAccessGDIPlusTransformCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusRotateWorldTransform"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeRotateWorldTransform; }
protected:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
private:
	emfplus::OEmfPlusRecRotateWorldTransform	m_recDataCached;
};

class EMFRecAccessGDIPlusRecSetPageTransform : public EMFRecAccessGDIPlusTransformCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSetPageTransform"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSetPageTransform; }
protected:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
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
	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIPlusRecSetClipPath : public EMFRecAccessGDIPlusClippingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSetClipPath"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSetClipPath; }
private:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIPlusRecSetClipRegion : public EMFRecAccessGDIPlusClippingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusSetClipRegion"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeSetClipRegion; }
private:
	void Preprocess(EMFAccess* pEMF) override;

	void CacheProperties(const CachePropertiesContext& ctxt) override;
};

class EMFRecAccessGDIPlusRecOffsetClip : public EMFRecAccessGDIPlusClippingCat
{
public:
	LPCWSTR GetRecordName() const override { return L"EmfPlusOffsetClip"; }

	emfplus::OEmfPlusRecordType GetRecordType() const override { return emfplus::EmfPlusRecordTypeOffsetClip; }
private:
	void CacheProperties(const CachePropertiesContext& ctxt) override;
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

	void CacheProperties(const CachePropertiesContext& ctxt) override;
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

#endif // EMF_REC_ACCESS_PLUS_H