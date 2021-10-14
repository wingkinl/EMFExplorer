#include "pch.h"
#include "framework.h"
#include "EMFRecAccess.h"

#include <atlbase.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

EMFAccess::EMFAccess(const std::vector<emfplus::u8t>* pData)
{
	if (pData)
	{
		ATL::CComPtr<IStream> stream;
		stream.Attach(SHCreateMemStream(pData->data(), (UINT)pData->size()));
		m_pMetafile = std::make_unique<Gdiplus::Metafile>(stream);
		m_pMetafile->GetMetafileHeader(&m_hdr);
	}
}

EMFAccess::~EMFAccess()
{

}

CRect EMFAccess::GetFittingDrawRect(const CRect& rect) const
{
	CSize szSrc{ m_hdr.Width, m_hdr.Height };
	CRect rcDraw = rect;
	CSize szDest = rect.Size();
	auto fScaleDst = (float)szDest.cy / szDest.cx;
	auto fScaleDIB = (float)szSrc.cy / szSrc.cx;
	if (fScaleDIB <= fScaleDst)
	{
		// source image is flatter than the target rectangle, so we fit the width
		szDest.cy	= (int)(szSrc.cy * (float)szDest.cx / szSrc.cx);
		rcDraw.bottom = rcDraw.top + szDest.cy;
	}
	else
	{
		szDest.cx	= (int)(szSrc.cx * (float)szDest.cy / szSrc.cy);
		rcDraw.right = rcDraw.left + szDest.cx;
	}
	return rcDraw;
}

void EMFAccess::DrawMetafile(Gdiplus::Graphics& gg, const CRect& rcDraw) const
{
	ASSERT(m_pMetafile.get());
	Gdiplus::Rect rcDrawP(rcDraw.left, rcDraw.top, rcDraw.Width(), rcDraw.Height());
	gg.DrawImage(m_pMetafile.get(), rcDrawP);
}
