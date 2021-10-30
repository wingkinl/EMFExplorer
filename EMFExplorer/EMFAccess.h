#ifndef EMF_ACCESS_H
#define EMF_ACCESS_H

#include <memory>
#include <vector>

class EMFAccessBase
{
public:
	EMFAccessBase(const void* pData, size_t nSize);
	EMFAccessBase(const std::vector<BYTE>& data);
	virtual ~EMFAccessBase();
public:
	void DrawMetafile(Gdiplus::Graphics& gg, const CRect& rcDraw) const;

	void DrawMetafileUntilRecord(Gdiplus::Graphics& gg, const CRect& rcDraw, size_t nRecord) const;

	const Gdiplus::MetafileHeader& GetMetafileHeader() const { return m_hdr; }

	Gdiplus::Image* CloneMetafile() const;
protected:
	using EMFPtr = std::unique_ptr<Gdiplus::Metafile>;
	EMFPtr	m_pMetafile;
	Gdiplus::MetafileHeader	m_hdr;
};

#ifndef SHARED_HANDLERS
#include "DataAccess.h"
#include "EMFRecAccess.h"
#include <string>

class EMFAccess : public EMFAccessBase
{
public:
	using EMFAccessBase::EMFAccessBase;

	EMFAccess(const emfplus::memory_vector& data);
	~EMFAccess();
public:
	inline size_t GetRecordCount() const { return m_EMFRecords.size(); }

	inline EMFRecAccess* GetRecord(size_t index) const
	{
		if (index >= m_EMFRecords.size())
			return nullptr;
		return m_EMFRecords[index];
	}

	bool GetRecords();

	void FreeRecords();

	bool HandleEMFRecord(emfplus::OEmfPlusRecordType type, UINT flags, UINT dataSize, const BYTE* data);

	EMFRecAccess* GetObjectCreationRecord(size_t index, bool bPlus) const;

	bool SetObjectToTable(size_t index, EMFRecAccess* pRec, bool bPlus);

	bool SaveToFile(LPCWSTR szPath) const;

	const std::wstring GetNestedPath() const
	{
		return m_strNestedPath;
	}
	inline void AddNestedPath(LPCWSTR szPath)
	{
		if (!m_strNestedPath.empty())
			m_strNestedPath += L"/";
		m_strNestedPath += szPath;
	}

	EMFRecAccess* GetGDISaveRecord(LONG nSavedDC) const;

	EMFRecAccess* GetPlusSaveRecord(emfplus::u32t nStackIndex) const;

	EMFRecAccess* HitTest(const POINT& pos, unsigned tolerance = 3) const;
private:
	bool PopPlusState(uint32_t nStackIndex, bool bContainer);
protected:
	using EmfRecArray	= std::vector<EMFRecAccess*>;
	
	EmfRecArray			m_EMFRecords;
	size_t				m_nDrawRecCount = 0;
	std::wstring		m_strNestedPath;

	//////////////////////////////
	// GDI
	//////////////////////////////
	struct EMFGDIState 
	{
		EMFRecAccess* pSavedRec;
	};
	std::vector<EMFGDIState> m_vGDIState;

	struct EMFGDIObjInfo
	{
		EMFRecAccess* pRec;
	};
	std::vector<EMFGDIObjInfo>	m_vGDIObjTable;

	//////////////////////////////
	// GDI+
	//////////////////////////////
	struct EMFPlusState
	{
		bool			bContainer;
		EMFRecAccess*	pSavedRec;
	};
	std::vector<EMFPlusState> m_vPlusState;

	struct EMFPlusObjInfo 
	{
		EMFRecAccess* pRec;
	};
	std::vector<EMFPlusObjInfo>			m_vPlusObjTable;
	emfplus::OEmfPlusRecObjectReader	m_PlusRecObjReader;
};

#endif // SHARED_HANDLERS

#endif // EMF_ACCESS_H