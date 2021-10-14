#ifndef EMF2SVG_GDIPLUSENUMS_H
#define EMF2SVG_GDIPLUSENUMS_H

#ifdef _ENABLE_GDIPLUS_STRUCT

namespace emfplus
{
	typedef	std::int8_t		i8t;
	typedef	std::uint8_t	u8t;
	typedef	std::int16_t	i16t;
	typedef	std::uint16_t	u16t;
	typedef	std::int32_t	i32t;
	typedef	std::uint32_t	u32t;
	typedef	std::int64_t	i64t;
	typedef	std::uint64_t	u64t;
	typedef float			Float;
	typedef u32t			Bool;

	enum OEmfPlusRecordType
	{
		EmfRecordTypeHeader                  = EMR_HEADER,
		EmfRecordTypePolyBezier              = EMR_POLYBEZIER,
		EmfRecordTypePolygon                 = EMR_POLYGON,
		EmfRecordTypePolyline                = EMR_POLYLINE,
		EmfRecordTypePolyBezierTo            = EMR_POLYBEZIERTO,
		EmfRecordTypePolyLineTo              = EMR_POLYLINETO,
		EmfRecordTypePolyPolyline            = EMR_POLYPOLYLINE,
		EmfRecordTypePolyPolygon             = EMR_POLYPOLYGON,
		EmfRecordTypeSetWindowExtEx          = EMR_SETWINDOWEXTEX,
		EmfRecordTypeSetWindowOrgEx          = EMR_SETWINDOWORGEX,
		EmfRecordTypeSetViewportExtEx        = EMR_SETVIEWPORTEXTEX,
		EmfRecordTypeSetViewportOrgEx        = EMR_SETVIEWPORTORGEX,
		EmfRecordTypeSetBrushOrgEx           = EMR_SETBRUSHORGEX,
		EmfRecordTypeEOF                     = EMR_EOF,
		EmfRecordTypeSetPixelV               = EMR_SETPIXELV,
		EmfRecordTypeSetMapperFlags          = EMR_SETMAPPERFLAGS,
		EmfRecordTypeSetMapMode              = EMR_SETMAPMODE,
		EmfRecordTypeSetBkMode               = EMR_SETBKMODE,
		EmfRecordTypeSetPolyFillMode         = EMR_SETPOLYFILLMODE,
		EmfRecordTypeSetROP2                 = EMR_SETROP2,
		EmfRecordTypeSetStretchBltMode       = EMR_SETSTRETCHBLTMODE,
		EmfRecordTypeSetTextAlign            = EMR_SETTEXTALIGN,
		EmfRecordTypeSetColorAdjustment      = EMR_SETCOLORADJUSTMENT,
		EmfRecordTypeSetTextColor            = EMR_SETTEXTCOLOR,
		EmfRecordTypeSetBkColor              = EMR_SETBKCOLOR,
		EmfRecordTypeOffsetClipRgn           = EMR_OFFSETCLIPRGN,
		EmfRecordTypeMoveToEx                = EMR_MOVETOEX,
		EmfRecordTypeSetMetaRgn              = EMR_SETMETARGN,
		EmfRecordTypeExcludeClipRect         = EMR_EXCLUDECLIPRECT,
		EmfRecordTypeIntersectClipRect       = EMR_INTERSECTCLIPRECT,
		EmfRecordTypeScaleViewportExtEx      = EMR_SCALEVIEWPORTEXTEX,
		EmfRecordTypeScaleWindowExtEx        = EMR_SCALEWINDOWEXTEX,
		EmfRecordTypeSaveDC                  = EMR_SAVEDC,
		EmfRecordTypeRestoreDC               = EMR_RESTOREDC,
		EmfRecordTypeSetWorldTransform       = EMR_SETWORLDTRANSFORM,
		EmfRecordTypeModifyWorldTransform    = EMR_MODIFYWORLDTRANSFORM,
		EmfRecordTypeSelectObject            = EMR_SELECTOBJECT,
		EmfRecordTypeCreatePen               = EMR_CREATEPEN,
		EmfRecordTypeCreateBrushIndirect     = EMR_CREATEBRUSHINDIRECT,
		EmfRecordTypeDeleteObject            = EMR_DELETEOBJECT,
		EmfRecordTypeAngleArc                = EMR_ANGLEARC,
		EmfRecordTypeEllipse                 = EMR_ELLIPSE,
		EmfRecordTypeRectangle               = EMR_RECTANGLE,
		EmfRecordTypeRoundRect               = EMR_ROUNDRECT,
		EmfRecordTypeArc                     = EMR_ARC,
		EmfRecordTypeChord                   = EMR_CHORD,
		EmfRecordTypePie                     = EMR_PIE,
		EmfRecordTypeSelectPalette           = EMR_SELECTPALETTE,
		EmfRecordTypeCreatePalette           = EMR_CREATEPALETTE,
		EmfRecordTypeSetPaletteEntries       = EMR_SETPALETTEENTRIES,
		EmfRecordTypeResizePalette           = EMR_RESIZEPALETTE,
		EmfRecordTypeRealizePalette          = EMR_REALIZEPALETTE,
		EmfRecordTypeExtFloodFill            = EMR_EXTFLOODFILL,
		EmfRecordTypeLineTo                  = EMR_LINETO,
		EmfRecordTypeArcTo                   = EMR_ARCTO,
		EmfRecordTypePolyDraw                = EMR_POLYDRAW,
		EmfRecordTypeSetArcDirection         = EMR_SETARCDIRECTION,
		EmfRecordTypeSetMiterLimit           = EMR_SETMITERLIMIT,
		EmfRecordTypeBeginPath               = EMR_BEGINPATH,
		EmfRecordTypeEndPath                 = EMR_ENDPATH,
		EmfRecordTypeCloseFigure             = EMR_CLOSEFIGURE,
		EmfRecordTypeFillPath                = EMR_FILLPATH,
		EmfRecordTypeStrokeAndFillPath       = EMR_STROKEANDFILLPATH,
		EmfRecordTypeStrokePath              = EMR_STROKEPATH,
		EmfRecordTypeFlattenPath             = EMR_FLATTENPATH,
		EmfRecordTypeWidenPath               = EMR_WIDENPATH,
		EmfRecordTypeSelectClipPath          = EMR_SELECTCLIPPATH,
		EmfRecordTypeAbortPath               = EMR_ABORTPATH,
		EmfRecordTypeReserved_069            = 69,  // Not Used
		EmfRecordTypeGdiComment              = EMR_GDICOMMENT,
		EmfRecordTypeFillRgn                 = EMR_FILLRGN,
		EmfRecordTypeFrameRgn                = EMR_FRAMERGN,
		EmfRecordTypeInvertRgn               = EMR_INVERTRGN,
		EmfRecordTypePaintRgn                = EMR_PAINTRGN,
		EmfRecordTypeExtSelectClipRgn        = EMR_EXTSELECTCLIPRGN,
		EmfRecordTypeBitBlt                  = EMR_BITBLT,
		EmfRecordTypeStretchBlt              = EMR_STRETCHBLT,
		EmfRecordTypeMaskBlt                 = EMR_MASKBLT,
		EmfRecordTypePlgBlt                  = EMR_PLGBLT,
		EmfRecordTypeSetDIBitsToDevice       = EMR_SETDIBITSTODEVICE,
		EmfRecordTypeStretchDIBits           = EMR_STRETCHDIBITS,
		EmfRecordTypeExtCreateFontIndirect   = EMR_EXTCREATEFONTINDIRECTW,
		EmfRecordTypeExtTextOutA             = EMR_EXTTEXTOUTA,
		EmfRecordTypeExtTextOutW             = EMR_EXTTEXTOUTW,
		EmfRecordTypePolyBezier16            = EMR_POLYBEZIER16,
		EmfRecordTypePolygon16               = EMR_POLYGON16,
		EmfRecordTypePolyline16              = EMR_POLYLINE16,
		EmfRecordTypePolyBezierTo16          = EMR_POLYBEZIERTO16,
		EmfRecordTypePolylineTo16            = EMR_POLYLINETO16,
		EmfRecordTypePolyPolyline16          = EMR_POLYPOLYLINE16,
		EmfRecordTypePolyPolygon16           = EMR_POLYPOLYGON16,
		EmfRecordTypePolyDraw16              = EMR_POLYDRAW16,
		EmfRecordTypeCreateMonoBrush         = EMR_CREATEMONOBRUSH,
		EmfRecordTypeCreateDIBPatternBrushPt = EMR_CREATEDIBPATTERNBRUSHPT,
		EmfRecordTypeExtCreatePen            = EMR_EXTCREATEPEN,
		EmfRecordTypePolyTextOutA            = EMR_POLYTEXTOUTA,
		EmfRecordTypePolyTextOutW            = EMR_POLYTEXTOUTW,
		EmfRecordTypeSetICMMode              = 98,  // EMR_SETICMMODE,
		EmfRecordTypeCreateColorSpace        = 99,  // EMR_CREATECOLORSPACE,
		EmfRecordTypeSetColorSpace           = 100, // EMR_SETCOLORSPACE,
		EmfRecordTypeDeleteColorSpace        = 101, // EMR_DELETECOLORSPACE,
		EmfRecordTypeGLSRecord               = 102, // EMR_GLSRECORD,
		EmfRecordTypeGLSBoundedRecord        = 103, // EMR_GLSBOUNDEDRECORD,
		EmfRecordTypePixelFormat             = 104, // EMR_PIXELFORMAT,
		EmfRecordTypeDrawEscape              = 105, // EMR_RESERVED_105,
		EmfRecordTypeExtEscape               = 106, // EMR_RESERVED_106,
		EmfRecordTypeStartDoc                = 107, // EMR_RESERVED_107,
		EmfRecordTypeSmallTextOut            = 108, // EMR_RESERVED_108,
		EmfRecordTypeForceUFIMapping         = 109, // EMR_RESERVED_109,
		EmfRecordTypeNamedEscape             = 110, // EMR_RESERVED_110,
		EmfRecordTypeColorCorrectPalette     = 111, // EMR_COLORCORRECTPALETTE,
		EmfRecordTypeSetICMProfileA          = 112, // EMR_SETICMPROFILEA,
		EmfRecordTypeSetICMProfileW          = 113, // EMR_SETICMPROFILEW,
		EmfRecordTypeAlphaBlend              = 114, // EMR_ALPHABLEND,
		EmfRecordTypeSetLayout               = 115, // EMR_SETLAYOUT,
		EmfRecordTypeTransparentBlt          = 116, // EMR_TRANSPARENTBLT,
		EmfRecordTypeReserved_117            = 117, // Not Used
		EmfRecordTypeGradientFill            = 118, // EMR_GRADIENTFILL,
		EmfRecordTypeSetLinkedUFIs           = 119, // EMR_RESERVED_119,
		EmfRecordTypeSetTextJustification    = 120, // EMR_RESERVED_120,
		EmfRecordTypeColorMatchToTargetW     = 121, // EMR_COLORMATCHTOTARGETW,
		EmfRecordTypeCreateColorSpaceW       = 122, // EMR_CREATECOLORSPACEW,
		EmfRecordTypeMax                     = 122,
		EmfRecordTypeMin                     = 1,

		// That is the END of the GDI EMF records.

		// Now we start the list of EMF+ records.  We leave quite
		// a bit of room here for the addition of any new GDI
		// records that may be added later.

		EmfPlusRecordBase	= 0x00004000,

		EmfPlusRecordTypeInvalid = EmfPlusRecordBase,
		EmfPlusRecordTypeHeader,
		EmfPlusRecordTypeEndOfFile,

		EmfPlusRecordTypeComment,

		EmfPlusRecordTypeGetDC,

		EmfPlusRecordTypeMultiFormatStart,
		EmfPlusRecordTypeMultiFormatSection,
		EmfPlusRecordTypeMultiFormatEnd,

		// For all persistent objects

		EmfPlusRecordTypeObject,

		// Drawing Records

		EmfPlusRecordTypeClear,
		EmfPlusRecordTypeFillRects,
		EmfPlusRecordTypeDrawRects,
		EmfPlusRecordTypeFillPolygon,
		EmfPlusRecordTypeDrawLines,
		EmfPlusRecordTypeFillEllipse,
		EmfPlusRecordTypeDrawEllipse,
		EmfPlusRecordTypeFillPie,
		EmfPlusRecordTypeDrawPie,
		EmfPlusRecordTypeDrawArc,
		EmfPlusRecordTypeFillRegion,
		EmfPlusRecordTypeFillPath,
		EmfPlusRecordTypeDrawPath,
		EmfPlusRecordTypeFillClosedCurve,
		EmfPlusRecordTypeDrawClosedCurve,
		EmfPlusRecordTypeDrawCurve,
		EmfPlusRecordTypeDrawBeziers,
		EmfPlusRecordTypeDrawImage,
		EmfPlusRecordTypeDrawImagePoints,
		EmfPlusRecordTypeDrawString,

		// Graphics State Records

		EmfPlusRecordTypeSetRenderingOrigin,
		EmfPlusRecordTypeSetAntiAliasMode,
		EmfPlusRecordTypeSetTextRenderingHint,
		EmfPlusRecordTypeSetTextContrast,
		EmfPlusRecordTypeSetInterpolationMode,
		EmfPlusRecordTypeSetPixelOffsetMode,
		EmfPlusRecordTypeSetCompositingMode,
		EmfPlusRecordTypeSetCompositingQuality,
		EmfPlusRecordTypeSave,
		EmfPlusRecordTypeRestore,
		EmfPlusRecordTypeBeginContainer,
		EmfPlusRecordTypeBeginContainerNoParams,
		EmfPlusRecordTypeEndContainer,
		EmfPlusRecordTypeSetWorldTransform,
		EmfPlusRecordTypeResetWorldTransform,
		EmfPlusRecordTypeMultiplyWorldTransform,
		EmfPlusRecordTypeTranslateWorldTransform,
		EmfPlusRecordTypeScaleWorldTransform,
		EmfPlusRecordTypeRotateWorldTransform,
		EmfPlusRecordTypeSetPageTransform,
		EmfPlusRecordTypeResetClip,
		EmfPlusRecordTypeSetClipRect,
		EmfPlusRecordTypeSetClipPath,
		EmfPlusRecordTypeSetClipRegion,
		EmfPlusRecordTypeOffsetClip,

		EmfPlusRecordTypeDrawDriverString,
#if (GDIPVER >= 0x0110)
		EmfPlusRecordTypeStrokeFillPath,
		EmfPlusRecordTypeSerializableObject,

		EmfPlusRecordTypeSetTSGraphics,
		EmfPlusRecordTypeSetTSClip,
#endif
		// NOTE: New records *must* be added immediately before this line.

		EmfPlusRecordTotal,

		EmfPlusRecordTypeMax = EmfPlusRecordTotal-1,
		EmfPlusRecordTypeMin = EmfPlusRecordTypeHeader,
	};

enum {
	OEmfPlusInteger7Mask	= 0x7F,
	OEmfPlusInteger15Mask	= 0x7FFF,
	OEmfPlusInteger15Flag	= 0x8000,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/e71e18d6-9e78-4df2-8da8-ae4a305b237b
enum OGraphicsVersion
{
	GraphicsVersion1		= 0x0001,
	GraphicsVersion1_1		= 0x0002,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/85484477-90eb-4fb8-bf45-4923e2f49daf
enum class OObjType
{
	Invalid,
	Brush,
	Pen,
	Path,
	Region,
	Image,
	Font,
	StringFormat,
	ImageAttributes,
	CustomLineCap,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/b2c1433b-1c80-4992-a861-bcc0e491c8da
enum ORegionNodeDataType : u32t
{
	ORegionNodeDataTypeAnd			= 0x00000001,
	ORegionNodeDataTypeOr			= 0x00000002,
	ORegionNodeDataTypeXor			= 0x00000003,
	ORegionNodeDataTypeExclude		= 0x00000004,
	ORegionNodeDataTypeComplement	= 0x00000005,
	ORegionNodeDataTypeRect			= 0x10000000,
	ORegionNodeDataTypePath			= 0x10000001,
	ORegionNodeDataTypeEmpty		= 0x10000002,
	ORegionNodeDataTypeInfinite		= 0x10000003
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/57ccc13f-2baa-46f2-a498-64e70be36953
enum class OCombineMode : u32t
{
	Replace		= 0x00000000,
	Intersect	= 0x00000001,
	Union		= 0x00000002,
	XOR			= 0x00000003,
	Exclude		= 0x00000004,
	Complement	= 0x00000005,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/cbfc8c78-778d-466e-a52a-a82f8ce6cfee
enum class OCompositingMode : u8t
{
	SourceOver = 0x00,
	SourceCopy = 0x01,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/a6a6bdf6-ef08-4b45-8cbf-04cd48c3751f
enum class OCompositingQuality : u8t
{
	Default			= 0x01,
	HighSpeed		= 0x02,
	HighQuality		= 0x03,
	GammaCorrected	= 0x04,
	AssumeLinear	= 0x05,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/e44e8618-9147-41ce-8fd5-eb2fd9f99608
enum class OCurveAdjustments : u32t
{
	Exposure		= 0x00000000,
	Density			= 0x00000001,
	Contrast		= 0x00000002,
	Highlight		= 0x00000003,
	Shadow			= 0x00000004,
	Midtone			= 0x00000005,
	WhiteSaturation = 0x00000006,
	BlackSaturation = 0x00000007,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/10e9a5e7-e864-4bb6-8f92-69ed7a0e31a5
enum class OCurveChannel : u32t
{
	CurveChannelAll = 0x00000000,
	CurveChannelRed = 0x00000001,
	CurveChannelGreen = 0x00000002,
	CurveChannelBlue = 0x00000003,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/5d44b6ca-ad59-464b-b9af-37d79c578f19
enum class OFilterType : u8t
{
	None			= 0x00,
	Point			= 0x01,
	Linear			= 0x02,
	Triangle		= 0x03,
	Box				= 0x04,
	PyramidalQuad	= 0x06,
	GaussianQuad	= 0x07,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/51312252-755a-4c8b-84bd-6ae73da1b5ea
enum class OInterpolationMode : u8t
{
	Default				= 0x00,
	LowQuality			= 0x01,
	HighQuality			= 0x02,
	Bilinear			= 0x03,
	Bicubic				= 0x04,
	NearestNeighbor		= 0x05,
	HighQualityBilinear	= 0x06,
	HighQualityBicubic	= 0x07,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/8ded25a8-bfa4-4bbd-b2c3-f627aae96545
enum class OCustomLineCapDataType : i32t
{
	Default			= 0x00000000,
	AdjustableArrow	= 0x00000001
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/a350fc30-8896-4750-91c8-7ff7e067ed04
enum class OCustomLineCapData : u32t
{
	FillPath		= 0x00000001,
	LinePath		= 0x00000002,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/1b87da0b-bfd6-458a-805e-d99d6eb3b16b
enum class OLineCapType : i32t
{
	Flat			= 0x00000000,
	Square			= 0x00000001,
	Round			= 0x00000002,
	Triangle		= 0x00000003,
	NoAnchor		= 0x00000010,
	SquareAnchor	= 0x00000011,
	RoundAnchor		= 0x00000012,
	DiamondAnchor	= 0x00000013,
	ArrowAnchor		= 0x00000014,
	AnchorMask		= 0x000000F0,
	Custom			= 0x000000FF,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/2ac2f9f4-56e4-4cfb-96dd-169229bc4517
enum class OLineJoinType : i32t
{
	Miter			= 0x00000000,
	Bevel			= 0x00000001,
	Round			= 0x00000002,
	MiterClipped	= 0x00000003
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/ffbb5332-4e04-4bca-9161-30217b0ec164
enum class OPenData : u32t
{
	Transform		= 0x00000001,
	StartCap		= 0x00000002,
	EndCap			= 0x00000004,
	Join			= 0x00000008,
	MiterLimit		= 0x00000010,
	LineStyle		= 0x00000020,
	DashedLineCap	= 0x00000040,
	DashedLineOffset= 0x00000080,
	DashedLine		= 0x00000100,
	NonCenter		= 0x00000200,
	CompoundLine	= 0x00000400,
	CustomStartCap	= 0x00000800,
	CustomEndCap	= 0x00001000,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/67986cd8-3814-4ab7-bd59-471416d5fe26
enum class OUnitType : u32t
{
	World		= 0x00,
	Display		= 0x01,
	Pixel		= 0x02,
	Point		= 0x03,
	Inch		= 0x04,
	Document	= 0x05,
	Millimeter	= 0x06
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/3435a9d6-b0ba-4c73-bfd2-e7dbe6e7e9de
enum class OLineStyle : i32t
{
	Solid		= 0x00000000,
	Dash		= 0x00000001,
	Dot			= 0x00000002,
	DashDot		= 0x00000003,
	DashDotDot	= 0x00000004,
	Custom		= 0x00000005,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/411e7bbe-5513-40d1-b167-c8e08ab4d946
enum class ODashedLineCap : i32t
{
	Flat		= 0x00000000,
	Round		= 0x00000002,
	Triangle	= 0x00000003,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/22cd03cb-3602-4f0c-b5ef-777348a7ddf6
enum class OPenAlignment : i32t
{
	Center	= 0x00000000,
	Inset	= 0x00000001,
	Left	= 0x00000002,
	Outset	= 0x00000003,
	Right	= 0x00000004,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/ac72d44f-1ec9-401b-82d6-f16c8e84134d
enum class OHatchStyle : u32t
{
	StyleHorizontal				= 0x00000000,
	StyleVertical				= 0x00000001,
	StyleForwardDiagonal		= 0x00000002,
	StyleBackwardDiagonal		= 0x00000003,
	StyleLargeGrid				= 0x00000004,
	StyleDiagonalCross			= 0x00000005,
	Style05Percent				= 0x00000006,
	Style10Percent				= 0x00000007,
	Style20Percent				= 0x00000008,
	Style25Percent				= 0x00000009,
	Style30Percent				= 0x0000000A,
	Style40Percent				= 0x0000000B,
	Style50Percent				= 0x0000000C,
	Style60Percent				= 0x0000000D,
	Style70Percent				= 0x0000000E,
	Style75Percent				= 0x0000000F,
	Style80Percent				= 0x00000010,
	Style90Percent				= 0x00000011,
	StyleLightDownwardDiagonal	= 0x00000012,
	StyleLightUpwardDiagonal	= 0x00000013,
	StyleDarkDownwardDiagonal	= 0x00000014,
	StyleDarkUpwardDiagonal		= 0x00000015,
	StyleWideDownwardDiagonal	= 0x00000016,
	StyleWideUpwardDiagonal		= 0x00000017,
	StyleLightVertical			= 0x00000018,
	StyleLightHorizontal		= 0x00000019,
	StyleNarrowVertical			= 0x0000001A,
	StyleNarrowHorizontal		= 0x0000001B,
	StyleDarkVertical			= 0x0000001C,
	StyleDarkHorizontal			= 0x0000001D,
	StyleDashedDownwardDiagonal = 0x0000001E,
	StyleDashedUpwardDiagonal	= 0x0000001F,
	StyleDashedHorizontal		= 0x00000020,
	StyleDashedVertical			= 0x00000021,
	StyleSmallConfetti			= 0x00000022,
	StyleLargeConfetti			= 0x00000023,
	StyleZigZag					= 0x00000024,
	StyleWave					= 0x00000025,
	StyleDiagonalBrick			= 0x00000026,
	StyleHorizontalBrick		= 0x00000027,
	StyleWeave					= 0x00000028,
	StylePlaid					= 0x00000029,
	StyleDivot					= 0x0000002A,
	StyleDottedGrid				= 0x0000002B,
	StyleDottedDiamond			= 0x0000002C,
	StyleShingle				= 0x0000002D,
	StyleTrellis				= 0x0000002E,
	StyleSphere					= 0x0000002F,
	StyleSmallGrid				= 0x00000030,
	StyleSmallCheckerBoard		= 0x00000031,
	StyleLargeCheckerBoard		= 0x00000032,
	StyleOutlinedDiamond		= 0x00000033,
	StyleSolidDiamond			= 0x00000034,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/47cbe48e-d13c-450b-8a23-6aa95488428e
enum class OPixelFormat : u32t
{
	FormatUndefined			= 0x00000000,
	Format1bppIndexed		= 0x00030101,
	Format4bppIndexed		= 0x00030402,
	Format8bppIndexed		= 0x00030803,
	Format16bppGrayScale	= 0x00101004,
	Format16bppRGB555		= 0x00021005,
	Format16bppRGB565		= 0x00021006,
	Format16bppARGB1555		= 0x00061007,
	Format24bppRGB			= 0x00021808,
	Format32bppRGB			= 0x00022009,
	Format32bppARGB			= 0x0026200A,
	Format32bppPARGB		= 0x000E200B,
	Format48bppRGB			= 0x0010300C,
	Format64bppARGB			= 0x0034400D,
	Format64bppPARGB		= 0x001A400E,

	FormatIndexMask			= 0x000000FF,
	FormatBitsPerPixelMask	= 0x0000FF00,
	FormatIFlag				= 0x00010000,
	FormatGFlag				= 0x00020000,
	FormatAFlag				= 0x00040000,
	FormatPFlag				= 0x00080000,
	FormatEFlag				= 0x00100000,
	FormatNFlag				= 0x00200000,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/e15c92d2-6054-48a0-b6dc-3155aab3203c
enum class OPixelOffsetMode : u8t
{
	Default		= 0x00,
	HighSpeed	= 0x01,
	HighQuality	= 0x02,
	None		= 0x03,
	Half		= 0x04,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/08ea5d19-dbd9-4056-9db3-424d8f47c3c1
enum class OSmoothingMode : u8t
{
	Default			= 0x00,
	HighSpeed		= 0x01,
	HighQuality		= 0x02,
	None			= 0x03,
	AntiAlias8x4	= 0x04,
	AntiAlias8x8	= 0x05,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/75788808-426f-48b6-8a9d-3fc41fec3963
enum class OBitmapDataType : u32t
{
	Pixel		= 0x00000000,
	Compressed	= 0x00000001,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/fbdf8835-4deb-481e-b246-15cebbb18710
enum class OPaletteStyle : u32t
{
	HasAlpha	= 0x00000001,
	GrayScale	= 0x00000002,
	Halftone	= 0x00000004,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/0eefa442-7d98-449f-b1a6-ad40575e3f25
enum class OImageDataType : u32t
{
	Unknown		= 0x00000000,
	Bitmap		= 0x00000001,
	Metafile	= 0x00000002,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/f9878a20-51f1-4ac7-ab68-8fd119521dcc
enum class OMetafileDataType : u32t
{
	Wmf				= 0x00000001,
	WmfPlaceable	= 0x00000002,
	Emf				= 0x00000003,
	EmfPlusOnly		= 0x00000004,
	EmfPlusDual		= 0x00000005,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/1ab04f60-1c58-410f-a29e-d725f5eb9b0f
enum class OBrushData : u32t
{
	Path				= 0x00000001,
	Transform			= 0x00000002,
	PresetColors		= 0x00000004,
	BlendFactorsH		= 0x00000008,
	BlendFactorsV		= 0x00000010,
	FocusScales			= 0x00000040,
	IsGammaCorrected	= 0x00000080,
	DoNotTransform		= 0x00000100,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/79d01e4a-6a59-4464-bd8c-2d3fe26df5bc
enum class OWrapMode : i32t
{
	Tile		= 0x00000000,
	TileFlipX	= 0x00000001,
	TileFlipY	= 0x00000002,
	TileFlipXY	= 0x00000003,
	Clamp		= 0x00000004,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/6a62c568-0916-4032-ab49-7c9e377a3d70
enum class OBrushType : u32t
{
	SolidColor		= 0x00000000,
	HatchFill		= 0x00000001,
	TextureFill		= 0x00000002,
	PathGradient	= 0x00000003,
	LinearGradient	= 0x00000004
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/d1838763-2f34-4747-a36d-0842fc253ab1
enum class OFontStyle : i32t
{
	Bold		= 0x00000001,
	Italic		= 0x00000002,
	Underline	= 0x00000004,
	Strikeout	= 0x00000008,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/03efbd65-8249-4bb6-b2d6-1ce05dcfaf51
enum class OStringFormat : u32t
{
	DirectionRightToLeft	= 0x00000001,
	DirectionVertical		= 0x00000002,
	NoFitBlackBox			= 0x00000004,
	DisplayFormatControl	= 0x00000020,
	NoFontFallback			= 0x00000400,
	MeasureTrailingSpaces	= 0x00000800,
	NoWrap					= 0x00001000,
	LineLimit				= 0x00002000,
	NoClip					= 0x00004000,
	BypassGDI				= 0x80000000,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/6485b089-6da5-4efd-9c55-155534363e49
enum class OStringAlignment : u32t
{
	Near	= 0x00000000,
	Center	= 0x00000001,
	Far		= 0x00000002
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/dcc3e25a-6d9c-4e3e-ac04-dd34203d1cc1
enum class OStringDigitSubstitution : u32t
{
	User		= 0x00000000,
	None		= 0x00000001,
	National	= 0x00000002,
	Traditional	= 0x00000003
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/5bb52f91-5625-4700-8123-f08609137208
enum class OHotkeyPrefix : i32t
{
	None = 0x00000000,
	Show = 0x00000001,
	Hide = 0x00000002
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/1f73aa0d-38c7-4b85-b177-5e8e90f516cf
enum class OStringTrimming : u32t
{
	None				= 0x00000000,
	Character			= 0x00000001,
	Word				= 0x00000002,
	EllipsisCharacter	= 0x00000003,
	EllipsisWord		= 0x00000004,
	EllipsisPath		= 0x00000005
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/2fb31108-f374-4e39-b01a-d4c130359970
enum class OTextRenderingHint : u8t
{
	SystemDefault				= 0x00,
	SingleBitPerPixelGridFit	= 0x01,
	SingleBitPerPixel			= 0x02,
	AntialiasGridFit			= 0x03,
	Antialias					= 0x04,
	ClearTypeGridFit			= 0x05,
};

enum class ODriverStringOptions : u32t
{
	CmapLookup		= 0x00000001,
	Vertical		= 0x00000002,
	RealizedAdvance	= 0x00000004,
	LimitSubpixel	= 0x00000008,
};

// {633C80A4-1843-482b-9EF2-BE2834C5FDD4}
static const GUID BlurEffectGuid = 
{ 0x633c80a4, 0x1843, 0x482b, { 0x9e, 0xf2, 0xbe, 0x28, 0x34, 0xc5, 0xfd, 0xd4 } };

// {63CBF3EE-C526-402c-8F71-62C540BF5142}
static const GUID SharpenEffectGuid = 
{ 0x63cbf3ee, 0xc526, 0x402c, { 0x8f, 0x71, 0x62, 0xc5, 0x40, 0xbf, 0x51, 0x42 } };

// {718F2615-7933-40e3-A511-5F68FE14DD74}
static const GUID ColorMatrixEffectGuid = 
{ 0x718f2615, 0x7933, 0x40e3, { 0xa5, 0x11, 0x5f, 0x68, 0xfe, 0x14, 0xdd, 0x74 } };

// {A7CE72A9-0F7F-40d7-B3CC-D0C02D5C3212}
static const GUID ColorLUTEffectGuid = 
{ 0xa7ce72a9, 0xf7f, 0x40d7, { 0xb3, 0xcc, 0xd0, 0xc0, 0x2d, 0x5c, 0x32, 0x12 } };

// {D3A1DBE1-8EC4-4c17-9F4C-EA97AD1C343D}
static const GUID BrightnessContrastEffectGuid = 
{ 0xd3a1dbe1, 0x8ec4, 0x4c17, { 0x9f, 0x4c, 0xea, 0x97, 0xad, 0x1c, 0x34, 0x3d } };

// {8B2DD6C3-EB07-4d87-A5F0-7108E26A9C5F}
static const GUID HueSaturationLightnessEffectGuid = 
{ 0x8b2dd6c3, 0xeb07, 0x4d87, { 0xa5, 0xf0, 0x71, 0x8, 0xe2, 0x6a, 0x9c, 0x5f } };

// {99C354EC-2A31-4f3a-8C34-17A803B33A25}
static const GUID LevelsEffectGuid = 
{ 0x99c354ec, 0x2a31, 0x4f3a, { 0x8c, 0x34, 0x17, 0xa8, 0x3, 0xb3, 0x3a, 0x25 } };

// {1077AF00-2848-4441-9489-44AD4C2D7A2C}
static const GUID TintEffectGuid = 
{ 0x1077af00, 0x2848, 0x4441, { 0x94, 0x89, 0x44, 0xad, 0x4c, 0x2d, 0x7a, 0x2c } };

// {537E597D-251E-48da-9664-29CA496B70F8}
static const GUID ColorBalanceEffectGuid = 
{ 0x537e597d, 0x251e, 0x48da, { 0x96, 0x64, 0x29, 0xca, 0x49, 0x6b, 0x70, 0xf8 } };

// {74D29D05-69A4-4266-9549-3CC52836B632}
static const GUID RedEyeCorrectionEffectGuid = 
{ 0x74d29d05, 0x69a4, 0x4266, { 0x95, 0x49, 0x3c, 0xc5, 0x28, 0x36, 0xb6, 0x32 } };

// {DD6A0022-58E4-4a67-9D9B-D48EB881A53D}
static const GUID ColorCurveEffectGuid =
{ 0xdd6a0022, 0x58e4, 0x4a67, { 0x9d, 0x9b, 0xd4, 0x8e, 0xb8, 0x81, 0xa5, 0x3d }
};

}

#endif // _ENABLE_GDIPLUS_STRUCT

#endif // EMF2SVG_GDIPLUSENUMS_H
