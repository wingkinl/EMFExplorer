#ifndef EMF_REC_ACCESS_H
#define EMF_REC_ACCESS_H

#include <memory>
#include <vector>
#include "GdiplusEnums.h"

class EMFRecAccess
{
public:
	EMFRecAccess() = default;
	virtual ~EMFRecAccess() = default;
public:

};

class EMFAccess final
{
public:
	EMFAccess(const std::vector<emfplus::u8t>* pData = nullptr);
	~EMFAccess();
public:
	const Gdiplus::MetafileHeader GetMetafileHeader() const { return m_hdr; }

	CRect GetFittingDrawRect(const CRect& rect) const;

	void DrawMetafile(Gdiplus::Graphics& gg, const CRect& rcDraw) const;
protected:
	using EmfRecArray	= std::vector<std::unique_ptr<EMFRecAccess>>;
	using EMFPtr		= std::unique_ptr<Gdiplus::Metafile>;

	EMFPtr					m_pMetafile;
	EmfRecArray				m_EMFRecords;
	Gdiplus::MetafileHeader	m_hdr;
};

#endif // EMF_REC_ACCESS_H
