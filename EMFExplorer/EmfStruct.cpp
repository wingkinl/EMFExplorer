#include PCH_FNAME

#include "EmfStruct.h"

namespace emfgdi
{

OEmfSetPaletteEntries::OEmfSetPaletteEntries(const EMRSETPALETTEENTRIES& rec)
	: ihPal(rec.ihPal), iStart(rec.iStart)
	, PalEntries(rec.aPalEntries, rec.cEntries)
{

}

OEmfPolyline::OEmfPolyline(const EMRPOLYLINE& rec)
	: rclBounds(rec.rclBounds)
	, aptl(rec.aptl, rec.cptl)
{

}

OEmfPolyline16::OEmfPolyline16(const EMRPOLYLINE16& rec)
	: rclBounds(rec.rclBounds)
	, apts(rec.apts, rec.cpts)
{

}

OEmfPolyPolyline::OEmfPolyPolyline(const EMRPOLYPOLYLINE& rec)
	: rclBounds(rec.rclBounds)
	, aPolyCounts(rec.aPolyCounts, rec.nPolys)
	, aptl((POINTL*)(rec.aPolyCounts+rec.nPolys), rec.cptl)
{

}

OEmfPolyPolyline16::OEmfPolyPolyline16(const EMRPOLYPOLYLINE16& rec)
	: rclBounds(rec.rclBounds)
	, aPolyCounts(rec.aPolyCounts, rec.nPolys)
	, apts((POINTS*)(rec.aPolyCounts+rec.nPolys), rec.cpts)
{

}

OEmfPolyDraw::OEmfPolyDraw(const EMRPOLYDRAW& rec)
	: rclBounds(rec.rclBounds)
	, aptl(rec.aptl, rec.cptl)
	, abTypes((BYTE*)(rec.aptl+rec.cptl), rec.cptl)
{

}

OEmfPolyDraw16::OEmfPolyDraw16(const EMRPOLYDRAW16& rec)
	: rclBounds(rec.rclBounds)
	, apts(rec.apts, rec.cpts)
	, abTypes((BYTE*)(rec.apts + rec.cpts), rec.cpts)
{

}

}