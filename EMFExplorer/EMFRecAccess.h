#ifndef EMF_REC_ACCESS_H
#define EMF_REC_ACCESS_H

#include <memory>
#include <vector>
#include "GdiplusEnums.h"
#include "EmfPlusStruct.h"
#include "PropertyTree.h"

class EMFAccess;

struct CachePropertiesContext
{
	EMFAccess* pEMF;
};

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

	std::shared_ptr<PropertyNode> GetProperties(const CachePropertiesContext& ctxt);

	inline size_t GetIndex() const { return m_nIndex; }

	enum LinkedObjType
	{
		LinkedObjTypeInvalid,
		LinkedObjTypeBrush,
		LinkedObjTypePen,
		LinkedObjTypePath,
		LinkedObjTypeRegion,
		LinkedObjTypeImage,
		LinkedObjTypeFont,
		LinkedObjTypeStringFormat,
		LinkedObjTypeImageAttributes,
		LinkedObjTypeCustomLineCap,
		LinkedObjTypeDrawingRecord	= 100,
		LinkedObjTypeGraphicState,
	};

	LinkedObjType IsLinked(const EMFRecAccess* pRec) const;

	inline size_t GetLinkedRecordCount() const { return m_linkRecs.size(); }

	// There could be multiple LinkedObjTypeDrawingRecord type but for now we don't care
	inline EMFRecAccess* GetLinkedRecord(LinkedObjType nType)
	{
		for (auto& link : m_linkRecs)
		{
			if (link.nType = nType)
				return link.pRec;
		}
		return nullptr;
	}
protected:
	void SetRecInfo(const emfplus::OEmfPlusRecInfo& info) { m_recInfo = info; }

	void SetIndex(size_t nIndex) { m_nIndex = nIndex; }

	virtual void CacheProperties(const CachePropertiesContext& ctxt);

	virtual void Preprocess(EMFAccess* pEMF) {}

	struct LinkedObjInfo
	{
		EMFRecAccess* pRec;
		LinkedObjType nType;
	};
	void AddLinkRecord(EMFRecAccess* pRec, LinkedObjType nType, LinkedObjType nTypeThis = LinkedObjTypeDrawingRecord);
protected:
	friend class EMFAccess;
	emfplus::OEmfPlusRecInfo		m_recInfo;
	size_t							m_nIndex = 0;
	std::shared_ptr<PropertyNode>	m_propsCached;
	std::vector<LinkedObjInfo>		m_linkRecs;
};

void GetPropertiesFromGDIPlusHeader(PropertyNode* pNode, const Gdiplus::MetafileHeader& hdr);

#endif // EMF_REC_ACCESS_H
