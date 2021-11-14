#ifndef EMF_STRUCT_H
#define EMF_STRUCT_H

#include "DataAccess.h"

namespace emfgdi
{
	using namespace data_access;

	#define GdiArrayWrapper(_ty)	array_wrapper<const _ty>

	struct OEmfSetPaletteEntries
	{
		OEmfSetPaletteEntries(const EMRSETPALETTEENTRIES& rec);

		DWORD   ihPal;
		DWORD   iStart;
		GdiArrayWrapper(PALETTEENTRY)	PalEntries;
	};

	struct OEmfPolyline
	{
		OEmfPolyline(const EMRPOLYLINE& rec);

		RECTL	rclBounds;
		GdiArrayWrapper(POINTL)	aptl;
	};

	struct OEmfPolyline16
	{
		OEmfPolyline16(const EMRPOLYLINE16& rec);

		RECTL	rclBounds;
		GdiArrayWrapper(POINTS)	apts;
	};

	struct OEmfPolyPolyline
	{
		OEmfPolyPolyline(const EMRPOLYPOLYLINE& rec);

		RECTL	rclBounds;
		GdiArrayWrapper(DWORD)	aPolyCounts;
		GdiArrayWrapper(POINTL)	aptl;
	};

	struct OEmfPolyPolyline16
	{
		OEmfPolyPolyline16(const EMRPOLYPOLYLINE16& rec);

		RECTL	rclBounds;
		GdiArrayWrapper(DWORD)	aPolyCounts;
		GdiArrayWrapper(POINTS)	apts;
	};

	struct OEmfPolyDraw
	{
		OEmfPolyDraw(const EMRPOLYDRAW& rec);

		RECTL	rclBounds;
		GdiArrayWrapper(POINTL)	aptl;
		GdiArrayWrapper(BYTE)	abTypes;
	};

	struct OEmfPolyDraw16
	{
		OEmfPolyDraw16(const EMRPOLYDRAW16& rec);

		RECTL	rclBounds;
		GdiArrayWrapper(POINTS)	apts;
		GdiArrayWrapper(BYTE)	abTypes;
	};
}

#endif // EMF_STRUCT_H
