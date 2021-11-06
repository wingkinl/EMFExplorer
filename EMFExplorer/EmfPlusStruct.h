#ifndef EMFPLUSSTRUCT_H
#define EMFPLUSSTRUCT_H

#ifdef _ENABLE_GDIPLUS_STRUCT

#include "GdiplusEnums.h"
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include "DataAccess.h"

#define _USE_MATH_DEFINES
#include <math.h>

#ifndef F_PI
	#define F_PI	M_PI
#endif
#ifndef F_PI_2
	#define F_PI_2	M_PI_2
#endif
#ifndef F_2PI
	#define F_2PI	(M_PI*2.0)
#endif

#pragma push_macro("min")
#pragma push_macro("max")
#undef max
#undef min

namespace emfplus
{
	using namespace data_access;

	using memory_vector = std::vector<u8t>;

#ifndef GSArray
	#ifdef DISABLE_GS_WRAP_TYPE
		template <typename _Ty>
		struct GSOptionalType
		{
			_Ty	val;
			inline _Ty* operator->() { return &val; }
			inline const _Ty* operator->() const { return &val; }

			inline _Ty& operator*() { return val; }
			inline const _Ty& operator*() const { return val; }

			inline operator _Ty& () { return val; }
			inline operator const _Ty& () { return val; }
		};
		#define GSOptional(_ty)			GSOptionalType<_ty>
		#define GSOptionalArray(_ty)	std::vector<_ty>
	#else
		#define GSOptional(_ty)			optional_wrapper<_ty>
		#define GSOptionalArray(_ty)	GSOptional(GSArray(_ty))
	#endif // DISABLE_GS_WRAP_TYPE
	#define GSArray(_ty)			std::vector<_ty>
#endif // GSArray


struct OEmfPlusRec
{
	u16t	Type;
	u16t	Flags;
	u32t	Size;
	u32t	DataSize;
};

struct OEmfPlusRecInfo
{
	u16t	Type;
	u16t	Flags;
	u32t	Size;
	u32t	DataSize;
	u8t*	Data;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emf/929b78e1-b848-44a5-9fac-327cae5c2ae5
enum {
	EMR_COMMENT_EMFPLUS				= 0x2B464D45,	// "+FME"
	EMR_COMMENT_EMFSPOOL			= 0x00000000,
	EMR_COMMENT_EMFSPOOL_RECORD_ID	= 0x544F4E46,	// "TONE"
	EMR_COMMENT_PUBLIC				= 0x43494447,	// "CIDG"
	EMR_COMMENT_WINDOWS_METAFILE	= 0x80000001,
	EMR_COMMENT_BEGINGROUP			= 0x00000002,
	EMR_COMMENT_ENDGROUP			= 0x00000003,
	EMR_COMMENT_MULTIFORMATS		= 0x40000004,
	EMR_COMMENT_UNICODE_STRING		= 0x00000040,
	EMR_COMMENT_UNICODE_END			= 0x00000080,
};

struct EMRCOMMENT_BASE
{
	u32t		Type;
	u32t		Size;
	u32t		DataSize;
	u32t		CommentIdentifier;
};

struct EMRCOMMENT_EMFPLUS : public EMRCOMMENT_BASE
{
	OEmfPlusRec		rec;
};

static_assert(sizeof(EMRCOMMENT_EMFPLUS) == sizeof(EMRCOMMENT_BASE)+sizeof(OEmfPlusRec), "EMRCOMMENT_EMFPLUS has incorrect size");

struct EMRCOMMENT_PUBLIC : public EMRCOMMENT_BASE
{
	u32t	PublicCommentIdentifier;
};

struct EMRCOMMENT_BEGINGROUP : public EMRCOMMENT_PUBLIC
{
	RECTL	Rectangle;
	u32t	DescriptionLen;
	wchar_t	Description[1];
};

using EMRCOMMENT_ENDGROUP = EMRCOMMENT_PUBLIC;

struct EMRCOMMENT_MULTIFORMATS : public  EMRCOMMENT_PUBLIC
{
	RECTL		OutputRect;
	u32t		CountFormats;
	EMRFORMAT	Formats[1];
};

struct EMRCOMMENT_WINDOWS_METAFILE : public EMRCOMMENT_PUBLIC
{
	u16t	Version;
	u16t	Reserved;
	u32t	Checksum;
	u32t	Flags;
	u32t	WinMetafileSize;
	u8t		WinMetafile[1];
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/fa7c00e7-ef14-4070-b12a-cb047d964ebe
struct OEmfPlusHeader 
{
	enum Flag
	{
		FlagD	= 0x0001,
	};
	enum EmfPlusFlag
	{
		EmfPlusFlagV	= 0x00000001,
	};

	static bool IsDual(u16t flags)
	{
		return flags & FlagD;
	}

	u32t	Version;
	u32t	EmfPlusFlags;
	u32t	LogicalDpiX;
	u32t	LogicalDpiY;
};

struct OEmfPlusHeaderEx 
{
	const ENHMETAHEADER*	pEMF = nullptr;
	const OEmfPlusHeader*	pPlus = nullptr;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/a0558721-f6df-4325-b455-a0e6edf63cf4
struct OEmfPlusPoint
{
	i16t x;
	i16t y;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/65ddf0d3-ae37-4da6-9a89-251ded97f1ad
struct OEmfPlusPointF 
{
	Float x;
	Float y;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/c2b45b77-c9ce-4b2e-8ede-ff5e90ec7f1b
typedef i8t		OEmfPlusInteger7;

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/c5478039-f448-422e-ba0b-ff5eddcada8e
typedef i16t	OEmfPlusInteger15;

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/2b930b6c-b55b-4912-a1ae-bd6f2a4239b9
struct OEmfPlusGraphObject 
{
	OEmfPlusGraphObject() = default;
	virtual ~OEmfPlusGraphObject() = default;

	enum
	{
		VersionMetafileSignature		= 0xDBC01,
		VersionMetafileSignatureMask	= 0xFFFFF000,
		VersionGraphicsVersionMask		= 0x00000FFF,
	};

	u32t Version;

	virtual bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE);

	virtual OObjType GetObjType() const = 0 { return OObjType::Invalid; }
};

struct OEmfPlusPointDataArray 
{
	GSOptionalArray(OEmfPlusPoint)	ivals;
	GSOptionalArray(OEmfPlusPointF)	fvals;

	inline size_t size() const
	{
		return std::max(ivals.size(), fvals.size());
	}

	void Read(DataReader& reader, u32t Count, bool bRelative, bool asInt);

	void Reset();
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/b539cf16-6232-4705-9f6e-6f914705145f
struct OEmfPlusPath : public OEmfPlusGraphObject
{
	enum OPathPointFlag
	{
		PathPointFlagR = 0x00000800,
		PathPointFlagC = 0x00004000,
		// Winding mode. Note: MS does not document this (maybe they forgot)
		PathPointFlagW = 0x00002000,
	};

	u32t	PathPointCount;
	u32t	PathPointFlags;	// see OPathPointFlag
	OEmfPlusPointDataArray	PathPoints;

	enum class OPathPointType : u8t
	{
		Start			= 0,
		Line			= 1,
		Bezier			= 3,
		PathTypeMask	= 0x0F, // type mask (lowest 3 bits).
		DashMode		= 0x10, // currently in dash mode.
		PathMarker		= 0x20, // a marker for the path.
		CloseSubpath	= 0x80, // closed flag
		FlagMask		= 0xF0,
	};
	enum class OEmfPlusPathPointTypeRLEFlag : u16t
	{
		Bezier			= 0x8000,
		RunCountMask	= 0x3F00,
		PointTypeMask	= 0x00FF,
	};
	GSOptionalArray(u8t)		PathPointTypes;
	GSOptionalArray(u16t)		PathPointTypesRLE;

	typedef u16t	OEmfPlusPathPointTypeRLE;

	static inline OPathPointType GetPathPointType(u16t val)
	{
		return (OPathPointType)((u8t)val & (u8t)OPathPointType::PathTypeMask);
	}
	static inline u8t GetPathPointTypeFlags(u16t val)
	{
		return ((u8t)val & (u8t)OPathPointType::FlagMask);
	}
	static inline u8t GetRunCount(OEmfPlusPathPointTypeRLE val)
	{
		return (val & (u16t)OEmfPlusPathPointTypeRLEFlag::RunCountMask) >> 8;
	}
	static inline OPathPointType GetPathPointTypeFromRLE(OEmfPlusPathPointTypeRLE val)
	{
		return (OPathPointType)(val & (u16t)OEmfPlusPathPointTypeRLEFlag::PointTypeMask);
	}
	inline bool PointsAreRelative() const
	{
		return (PathPointFlags & PathPointFlagR);
	}
	inline bool PointsAreFloats() const
	{
		return !(PathPointFlags & PathPointFlagR) && !(PathPointFlags & PathPointFlagC);
	}
	inline bool PathPointTypesAreRLE() const
	{
		return (PathPointFlags & PathPointFlagR);
	}
	inline bool IsWindingFillMode() const
	{
		return (PathPointFlags & PathPointFlagW);
	}

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE) override;

	void Reset();

	OObjType GetObjType() const override { return OObjType::Path; }
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/01c7de4f-d37e-49d1-9f64-5627a9ca2eea
struct OEmfPlusRegionNodePath 
{
	OEmfPlusPath	RegionNodePath;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE);

	inline void Reset()
	{
		RegionNodePath.Reset();
	}
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/2addbcb6-0baf-4794-a733-048cb7a3e1e4
struct OEmfPlusRect
{
	i16t	X;
	i16t	Y;
	i16t	Width;
	i16t	Height;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/f02202c0-9ef1-4e0d-b81d-0dbb92757b7c
struct OEmfPlusRectF
{
	Float X;
	Float Y;
	Float Width;
	Float Height;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-wmf/fe9329f4-7a87-4025-9a8a-541ee21e6530
struct ORectL 
{
	i32t	Left;
	i32t	Top;
	i32t	Right;
	i32t	Bottom;
};

// FLT_EPSILON = 1.192092896e-07F
static inline Float GetSmallFloat() { return 0.000001f; }

static inline bool EqualZero(Float val)
{
	return (fabs(val) <= GetSmallFloat());
}

static inline bool FloatsEqual(Float val1, Float val2)
{
	return (fabs(val1 - val2) <= GetSmallFloat());
}

static inline bool EqualOne(Float val)
{
	return FloatsEqual(val, 1.0f);
}

static inline int32_t fround( double val )
{
	if (val >= std::numeric_limits<int32_t>::max() - .5)
		return std::numeric_limits<int32_t>::max();
	else if (val <= std::numeric_limits<int32_t>::min() + .5)
		return std::numeric_limits<int32_t>::min();
	return val > 0.0 ? static_cast<int32_t>( val + .5 ) : static_cast<int32_t>( val - .5 );
}

static inline double Deg2Rad(double v)
{
	return v / 90.0 * M_PI_2;
}

static inline double Rad2Deg(double v)
{
	return v / M_PI_2 * 90.0;
}

enum OTransformElem
{
	OTM11,
	OTM12,
	OTM21,
	OTM22,
	OTMDX,
	OTMDY,
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/d65ccfa9-3674-42c8-93aa-df51fec6a37a
typedef Float OEmfPlusTransformMatrix[6];

struct OEmfPlusRegionNodeChildNodes;

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/118a1cab-68ee-4658-b7a8-7b03aa1fafe3
struct OEmfPlusRegionNode
{
	ORegionNodeDataType		Type;

	GSOptional(OEmfPlusRectF)			rect;
	GSOptional(OEmfPlusRegionNodePath)	path;
	std::unique_ptr<OEmfPlusRegionNodeChildNodes>	childNodes;

	OEmfPlusRegionNode();
	OEmfPlusRegionNode(const OEmfPlusRegionNode& other);
	OEmfPlusRegionNode& operator=(const OEmfPlusRegionNode& other);

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE);

	inline void SetTransform(const OEmfPlusTransformMatrix& mat)
	{
		memcpy(transform, mat, sizeof(mat));
	}

	inline void SetTransformAsIdentity()
	{
		SetTransform({1,0,0,1,0,0});
	}

	void MakeEmpty();

	void MakeInfinite();

	// This transform matrix is for clipping only
	OEmfPlusTransformMatrix	transform;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/07891e54-946e-45e4-ad68-213a21be5c3d
struct OEmfPlusRegionNodeChildNodes 
{
	OEmfPlusRegionNode	Left;
	OEmfPlusRegionNode	Right;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/43d5fb61-3ee5-46ba-be9b-ff830c91ee05
struct OEmfPlusDashedLineData 
{
	u32t			DashedLineDataSize;
	GSArray(Float)	DashedLineData;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/9b1387ba-6008-435b-9d13-878323083130
struct OEmfPlusCompoundLineData
{
	u32t			CompoundLineDataSize;
	GSArray(Float)	CompoundLineData;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/8958a5a3-2de1-469c-ad2d-f4b64a9132e7
struct OEmfPlusFillPath
{
	i32t			FillPathLength;
	OEmfPlusPath	FillPath;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/4b798085-3651-4287-a02a-12e692c8a546
struct OEmfPlusLinePath
{
	i32t			LinePathLength;
	OEmfPlusPath	LinePath;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/6fbd47ae-6c34-424c-b4cc-0e86e78d9bd4
struct OEmfPlusCustomLineCapData
{
	u32t			CustomLineCapDataFlags;	// see OCustomLineCapDataFlag
	OLineCapType	BaseCap;
	Float			BaseInset;
	OLineCapType	StrokeStartCap;
	OLineCapType	StrokeEndCap;
	OLineJoinType	StrokeJoin;
	Float			StrokeMiterLimit;
	Float			WidthScale;
	OEmfPlusPointF	FillHotSpot;
	OEmfPlusPointF	StrokeHotSpot;

	// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/967095e9-c2cf-4b25-936a-57136754baec
	struct OEmfPlusCustomLineCapOptionalData
	{
		GSOptional(OEmfPlusFillPath) FillData;
		GSOptional(OEmfPlusLinePath) OutlineData;

		bool Read(DataReader& reader, u32t CustomLineCapDataFlags, size_t nExpectedSize = UNKNOWN_SIZE);
	};

	OEmfPlusCustomLineCapOptionalData	OptionalData;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/f4c454d8-775b-4379-a0c7-bf7e64ca3480
struct OEmfPlusCustomLineCapArrowData
{
	Float			Width;
	Float			Height;
	Float			MiddleInset;
	u32t			FillState;
	OLineCapType	LineStartCap;
	OLineCapType	LineEndCap;
	OLineJoinType	LineJoin;
	Float			LineMiterLimit;
	Float			WidthScale;
	OEmfPlusPointF	FillHotSpot;
	OEmfPlusPointF	LineHotSpot;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/67d95a39-7b04-47e8-bc12-52d60596457a
struct OEmfPlusCustomLineCap : public OEmfPlusGraphObject
{
	OCustomLineCapDataType						Type;
	GSOptional(OEmfPlusCustomLineCapData)		CustomLineCapData;
	GSOptional(OEmfPlusCustomLineCapArrowData)	CustomLineCapDataArrow;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE) override;

	OObjType GetObjType() const override { return OObjType::CustomLineCap; }
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/b4db5048-1aba-4eee-bcc6-98f5f2aa9ac4
struct OEmfPlusCustomStartCapData
{
	u32t					CustomStartCapSize;
	OEmfPlusCustomLineCap	CustomStartCap;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/c9ddf636-f172-4f1f-84e2-4e07ad536702
struct OEmfPlusCustomEndCapData
{
	u32t					CustomEndCapSize;
	OEmfPlusCustomLineCap	CustomEndCap;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/33d8ced5-7768-47aa-a082-a14e5dfabc96
struct OEmfPlusPenData
{
	u32t		PenDataFlags;	// see OPenData
	OUnitType	PenUnit;
	Float		PenWidth;

	// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/5ef071f3-f503-4f16-b027-7c4bcf2d1d81
	struct OEmfPlusPenOptionalData
	{
		GSOptional(OEmfPlusTransformMatrix)	TransformMatrix;
		GSOptional(OLineCapType)				StartCap;
		GSOptional(OLineCapType)				EndCap;
		GSOptional(OLineJoinType)				Join;
		GSOptional(Float)						MiterLimit;
		GSOptional(OLineStyle)					LineStyle;
		GSOptional(ODashedLineCap)				DashedLineCapType;
		GSOptional(Float)						DashOffset;
		GSOptional(OEmfPlusDashedLineData)		DashedLineData;
		GSOptional(OPenAlignment)				PenAlignment;
		GSOptional(OEmfPlusCompoundLineData)	CompoundLineData;
		GSOptional(OEmfPlusCustomStartCapData)	CustomStartCapData;
		GSOptional(OEmfPlusCustomEndCapData)	CustomEndCapData;

		bool Read(DataReader& reader, u32t PenDataFlags, size_t nExpectedSize = UNKNOWN_SIZE);
	};

	OEmfPlusPenOptionalData	OptionalData;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/10284df9-0c5e-48d0-9196-c91c09de069f
union OEmfPlusARGB
{
	struct 
	{
		u8t	Blue;
		u8t	Green;
		u8t	Red;
		u8t	Alpha;
	};
	u32t	argb;

	// rgba is not supported well in some tools such as inkscape
	//#define USE_RGBA_FOR_COLOR
	std::string GetColorText() const;

	static inline OEmfPlusARGB FromCOLORREF(COLORREF clr)
	{
		OEmfPlusARGB argb = {0};
		argb.Red	= GetRValue(clr);
		argb.Green	= GetGValue(clr);
		argb.Blue	= GetBValue(clr);
		argb.Alpha	= 255;
		return argb;
	}

	inline COLORREF ToCOLORREF() const
	{
		return RGB(Red, Green, Blue);
	}

	static inline std::string GetColorTextFromBGR(COLORREF crf)
	{
		return FromCOLORREF(crf).GetColorText();
	}
};

static_assert(sizeof(OEmfPlusARGB) == 4, "OEmfPlusARGB must be a 32 bit value");

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/503573f8-791a-469c-a2af-d4b570585944
struct OEmfPlusMetafile 
{
	OMetafileDataType	Type;
	u32t				MetafileDataSize;
	memory_vector		MetafileData;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/dfb51d9c-7d65-45ec-ac00-9666e0578783
struct OEmfPlusPalette
{
	u32t	PaletteStyleFlags;	// see OPaletteStyle
	u32t	PaletteCount;
	GSArray(OEmfPlusARGB)	PaletteEntries;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/c236db12-fc5d-4e5b-8266-0468881cd940
struct OEmfPlusBitmapData
{
	GSOptional(OEmfPlusPalette)		Colors;
	memory_vector					PixelData;

	bool Read(DataReader& reader, OPixelFormat PixelFormat, size_t nExpectedSize);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/9c00912b-adfa-469e-8baa-82d9d3d8d6ae
struct OEmfPlusCompressedImage
{
	memory_vector	CompressedImageData;

	bool Read(DataReader& reader, size_t nExpectedSize);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/112a5e2c-6bb3-4daf-8ee3-0f3d3984410f
struct OEmfPlusBitmap 
{
	i32t			Width;
	i32t			Height;
	i32t			Stride;
	OPixelFormat	PixelFormat;
	OBitmapDataType	Type;
	GSOptional(OEmfPlusBitmapData)		BitmapData;
	GSOptional(OEmfPlusCompressedImage)	BitmapDataCompressed;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/02c80141-208e-4335-ad51-190b40a1802c
struct OEmfPlusImage : public OEmfPlusGraphObject
{
	OImageDataType	Type;

	GSOptional(OEmfPlusBitmap)		ImageDataBmp;
	GSOptional(OEmfPlusMetafile)	ImageDataMetafile;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE) override;

	OObjType GetObjType() const override { return OObjType::Image; }
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/9f73a2af-8c9c-4d05-98ab-19a485064364
struct OEmfPlusImageAttributes : public OEmfPlusGraphObject
{
	u32t			Reserved1;
	OWrapMode		WrapMode;
	OEmfPlusARGB	ClampColor;

	enum class ClampType : i32t
	{
		Rect	= 0x00000000,
		Bitmap	= 0x00000001,
	};
	ClampType		ObjectClamp;
	u32t			Reserved2;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE) override;

	OObjType GetObjType() const override { return OObjType::ImageAttributes; }
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/5b45e8f4-f1ff-4701-a058-c85d15d8f0c2
struct OBlurEffect
{
	Float	BlurRadius;
	Bool	ExpandEdge;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/3991e19c-f74a-488c-9507-65a7a92e23d2
struct OBrightnessContrastEffect
{
	i32t	BrightnessLevel;
	i32t	ContrastLevel;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/026ce55f-15d4-48ff-a701-797989d73565
struct OColorBalanceEffect
{
	i32t	CyanRed;
	i32t	MagentaGreen;
	i32t	YellowBlue;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/9d18533b-502e-4226-bb19-5a9a1865c026
struct OColorCurveEffect
{
	OCurveAdjustments	CurveAdjustment;
	OCurveChannel		CurveChannel;
	i32t				AdjustmentIntensity;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/e4196341-3eb1-4ba3-8918-172bd77d4691
struct OColorLookupTableEffect
{
	u8t		BlueLookupTable[256];
	u8t		GreenLookupTable[256];
	u8t		RedLookupTable[256];
	u8t		AlphaLookupTable[256];
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/cc0984b7-d119-4a19-b972-2479e0f8d4e7
struct OColorMatrixEffect
{
	Float	Matrix_N_0[5];
	Float	Matrix_N_1[5];
	Float	Matrix_N_2[5];
	Float	Matrix_N_3[5];
	Float	Matrix_N_4[5];
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/6ceba4cb-2bba-45b1-9068-3b0524a44d20
struct OHueSaturationLightnessEffect
{
	i32t	HueLevel;
	i32t	SaturationLevel;
	i32t	LightnessLevel;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/682fbb52-42fa-472e-8b4a-8bb8143e3f38
struct OLevelsEffect
{
	i32t	Highlight;
	i32t	MidTone;
	i32t	Shadow;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/5e61e7fb-fe4a-44bf-82d4-a21352b01919
struct ORedEyeCorrectionEffect
{
	i32t	NumberOfAreas;
	GSArray(ORectL)	Areas;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/1a41a97f-4c8e-4500-80bd-e4ea201ed99b
struct OSharpenEffect
{
	Float	Radius;
	Float	Amount;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/f48f4d48-0d46-4433-aae3-e40d1498fff3
struct OTintEffect 
{
	i32t	Hue;
	i32t	Amount;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/ffb875ee-9713-4356-a869-5e4dc4ea4afa
struct OEmfPlusHatchBrushData 
{
	OHatchStyle		HatchStyle;
	OEmfPlusARGB	ForeColor;
	OEmfPlusARGB	BackColor;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/bab3d8e7-cc3c-44a4-a6be-2b4b88ab389c
struct OEmfPlusBlendColors 
{
	u32t					PositionCount;
	GSArray(Float)			BlendPositions;
	GSArray(OEmfPlusARGB)	BlendColors;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/c8a8d3db-09ed-4b79-b7b4-f2c502df1869
struct OEmfPlusBlendFactors
{
	u32t				PositionCount;
	GSArray(Float)		BlendPositions;
	GSArray(Float)		BlendFactors;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/af50f5e3-e5c2-43ce-b819-d9476e93378e
struct OEmfPlusLinearGradientBrushOptionalData 
{
	GSOptional(OEmfPlusTransformMatrix)		TransformMatrix;

	struct BlendPatternData
	{
		GSOptional(OEmfPlusBlendColors)		colors;
		GSOptional(OEmfPlusBlendFactors)	factorsH;
		GSOptional(OEmfPlusBlendFactors)	factorsV;
	};
	BlendPatternData			BlendPattern;

	bool Read(DataReader& reader, u32t BrushDataFlags, size_t nExpectedSize = UNKNOWN_SIZE);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/55bba6b0-bb57-4545-93f1-4c64f02b0a80
struct OEmfPlusLinearGradientBrushData
{
	u32t			BrushDataFlags;	// BrushDataFlag
	OWrapMode		WrapMode;
	OEmfPlusRectF	RectF;
	OEmfPlusARGB	StartColor;
	OEmfPlusARGB	EndColor;
	u32t			Reserved1;
	u32t			Reserved2;

	OEmfPlusLinearGradientBrushOptionalData OptionalData;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/d90f9243-3d44-48e5-9baa-1db4ae5394b4
struct OEmfPlusFocusScaleData
{
	u32t	FocusScaleCount;
	Float	FocusScaleX;
	Float	FocusScaleY;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/2065ba3d-b70f-4bc0-b025-6978b1aa7311
struct OEmfPlusBoundaryPathData 
{
	i32t			BoundaryPathSize;
	OEmfPlusPath	BoundaryPathData;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/6a55add5-6d97-4744-ac13-e5e9ef7b050a
struct OEmfPlusBoundaryPointData
{
	i32t					BoundaryPointCount;
	GSArray(OEmfPlusPointF)	BoundaryPointData;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/0008fc6a-b678-41a6-ab99-9be99fc14092
struct OEmfPlusPathGradientBrushOptionalData 
{
	GSOptional(OEmfPlusTransformMatrix)		TransformMatrix;

	struct BlendPatternData
	{
		GSOptional(OEmfPlusBlendColors)		colors;
		GSOptional(OEmfPlusBlendFactors)	factors;
	};
	BlendPatternData					BlendPattern;
	GSOptional(OEmfPlusFocusScaleData)	FocusScaleData;

	bool Read(DataReader& reader, u32t BrushDataFlags, size_t nExpectedSize);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/cfe9439a-f2b6-4663-9032-67ce8feea5aa
struct OEmfPlusPathGradientBrushData
{
	u32t			BrushDataFlags;	// BrushDataFlag
	OWrapMode		WrapMode;
	OEmfPlusARGB	CenterColor;
	OEmfPlusPointF	CenterPointF;
	u32t			SurroundingColorCount;
	GSArray(OEmfPlusARGB)	SurroundingColor;
	GSOptional(OEmfPlusBoundaryPathData) BoundaryDataPath;
	GSOptional(OEmfPlusBoundaryPointData) BoundaryDataPoint;
	OEmfPlusPathGradientBrushOptionalData OptionalData;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/ac826aa5-7060-4161-af70-6b1a76a27e0f
struct OEmfPlusSolidBrushData
{
	OEmfPlusARGB	SolidColor;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/d59b926e-f79c-4a88-af18-77c2e58d53d7
struct OEmfPlusTextureBrushData
{
	OBrushData	BrushDataFlags;
	OWrapMode	WrapMode;

	// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/65bf0dda-baf3-4ee0-a6c4-f37a2f73c026
	struct OEmfPlusTextureBrushOptionalData
	{
		GSOptional(OEmfPlusTransformMatrix)		TransformMatrix;
		GSOptional(OEmfPlusImage)				ImageObject;

		bool Read(DataReader& reader, OBrushData BrushDataFlags, size_t nExpectedSize);
	};

	OEmfPlusTextureBrushOptionalData	OptionalData;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/79c653fb-bf01-4f87-8bd2-eac1de71e140
struct OEmfPlusBrush : public OEmfPlusGraphObject
{
	OBrushType		Type;

	GSOptional(OEmfPlusHatchBrushData)				BrushDataHatch;
	GSOptional(OEmfPlusLinearGradientBrushData)		BrushDataLinearGrad;
	GSOptional(OEmfPlusPathGradientBrushData)		BrushDataPathGrad;
	GSOptional(OEmfPlusSolidBrushData)				BrushDataSolid;
	GSOptional(OEmfPlusTextureBrushData)			BrushDataTexture;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE) override;

	OObjType GetObjType() const override { return OObjType::Brush; }
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/7af4bee7-b3e0-47c5-9678-ffcbb234e378
struct OEmfPlusPen : public OEmfPlusGraphObject 
{
	OEmfPlusPenData	PenData;
	OEmfPlusBrush	BrushObject;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE) override;

	OObjType GetObjType() const override { return OObjType::Pen; }
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/3f8c4a6a-a0af-4ba2-8bb2-0d2f4569c493
struct OEmfPlusRegion : public OEmfPlusGraphObject
{
	OEmfPlusRegionNode	RegionNode;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE) override;

	OObjType GetObjType() const override { return OObjType::Region; }
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/b7d36375-b4e2-491d-aa73-bc7809d4af4f
struct OEmfPlusFont : public OEmfPlusGraphObject
{
	Float			EmSize;
	OUnitType		SizeUnit;
	i32t			FontStyleFlags;	// see OFontStyle
	u32t			Reserved;
	u32t			Length;
	std::wstring	FamilyName;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE) override;

	OObjType GetObjType() const override { return OObjType::Font; }
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/2e9018c2-4274-47bb-9639-8baf9ec573e2
typedef u16t	OEmfPlusLanguageIdentifier;

enum OLangMask
{
	LangMaskPrimaryLangId	= 0x03FF,
	LangMaskSubLangId		= 0xFC00,
};

inline u8t	GetPrimaryLanguageId(OEmfPlusLanguageIdentifier langID)
{
	return (u8t)(langID & LangMaskPrimaryLangId);
}

inline u8t	GetSubLanguageId(OEmfPlusLanguageIdentifier langID)
{
	return (u8t)((langID & LangMaskSubLangId) >> 10);
}

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/14b1a0a8-d9c5-4cb0-be32-c8125fabb981
struct OEmfPlusCharacterRange
{
	i32t	First;
	i32t	Length;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/9df612d2-fb87-44f7-93fb-0fa0b7b2f276
struct OEmfPlusStringFormat : public OEmfPlusGraphObject
{
	u32t						StringFormatFlags;	// see OStringFormat
	OEmfPlusLanguageIdentifier	Language;
	OStringAlignment			StringAlignment;
	OStringAlignment			LineAlign;
	OStringDigitSubstitution	DigitSubstitution;
	OEmfPlusLanguageIdentifier	DigitLanguage;
	Float						FirstTabOffset;
	OHotkeyPrefix				HotkeyPrefix;
	Float						LeadingMargin;
	Float						TrailingMargin;
	Float						Tracking;
	OStringTrimming				Trimming;
	i32t						TabStopCount;
	i32t						RangeCount;

	// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/9567fbcd-d999-4c70-873f-12d320f02dc3
	struct OEmfPlusStringFormatData 
	{
		GSArray(Float)					TabStops;
		GSArray(OEmfPlusCharacterRange)	CharRange;
	};

	OEmfPlusStringFormatData	StringFormatData;

	bool Read(DataReader& reader, size_t nExpectedSize = UNKNOWN_SIZE) override;

	OObjType GetObjType() const override { return OObjType::StringFormat; }
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/a3cf7ce4-f038-443c-b071-c8ae2a358aee
struct OEmfPlusRecOffsetClip
{
	Float	dx;
	Float	dy;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/624a4998-3242-42f3-bc4a-726af63fa074
struct OEmfPlusRecSetClipPath
{
	enum Flag
	{
		FlagCM_Mask			= 0x0F00,
		FlagObjectID_Mask	= 0x00FF,
	};
	static OCombineMode GetCombineMode(u32t flags)
	{
		return (OCombineMode)((flags & FlagCM_Mask) >> 8);
	}
	static u32t GetObjectID(u32t flags)
	{
		return flags & FlagObjectID_Mask;
	}
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/835e495a-83c9-4a6f-a2df-b5b5ea3fd15e
struct OEmfPlusRecSetClipRect
{
	enum Flag
	{
		FlagCM_Mask	= 0x0F00,
	};
	static OCombineMode GetCombineMode(u32t flags)
	{
		return (OCombineMode)((flags & FlagCM_Mask) >> 8);
	}

	OEmfPlusRectF	ClipRect;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/a9d596a3-08cb-4232-8ea6-4b58717c4c04
struct OEmfPlusRecSetClipRegion
{
	enum Flag
	{
		FlagCM_Mask			= 0x0F00,
		FlagObjectID_Mask	= 0x00FF,
	};
	static OCombineMode GetCombineMode(u32t flags)
	{
		return (OCombineMode)((flags & FlagCM_Mask) >> 8);
	}
	static u32t GetObjectID(u32t flags)
	{
		return flags & FlagObjectID_Mask;
	}
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/beb96556-0251-4b33-964f-b30aca0870ea
struct OEmfPlusRecComment
{
	memory_vector	PrivateData;

	bool Read(DataReader& reader, u16t nFlags, size_t nExpectedSize);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/38dc4973-cae8-406e-89f3-7536e2139def
struct OEmfPlusRecClear 
{
	OEmfPlusARGB	Color;
};

struct OEmfPlusRectData 
{
	GSOptional(OEmfPlusRect)	ival;
	GSOptional(OEmfPlusRectF)	fval;

	bool AsInt;

	void Read(DataReader& reader, bool asInt);
};

struct OEmfPlusRectDataArray 
{
	u32t	Count;
	GSOptionalArray(OEmfPlusRect)	ivals;
	GSOptionalArray(OEmfPlusRectF)	fvals;

	inline size_t size() const
	{
		return std::max(ivals.size(), fvals.size());
	}

	void Read(DataReader& reader, bool asInt);
};

struct OEmfPlusArcData 
{
	Float				StartAngle;
	Float				SweepAngle;
	OEmfPlusRectData	RectData;

	bool Read(DataReader& reader, bool bIntRectData, size_t nExpectedSize);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/71cc1d60-c2e9-4554-b26e-4e982004dddb
struct OEmfPlusRecDrawArc 
{
	enum Flag
	{
		FlagC				= 0x4000,
		FlagObjectIDMask	= 0x00FF,
	};
	OEmfPlusArcData	ArcData;

	bool Read(DataReader& reader, u16t nFlags, size_t nExpectedSize);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/3be0d4c9-a28b-4206-9532-5044cc07b8cc
struct OEmfPlusRecDrawBeziers 
{
	enum Flag
	{
		FlagC				= 0x4000,
		FlagP				= 0x0800,
		FlagObjectIDMask	= 0x00FF,
	};
	u32t	Count;
	OEmfPlusPointDataArray	PointData;

	bool Read(DataReader& reader, u16t nFlags, size_t nExpectedSize);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/ad7439d3-4294-47ad-aa09-1e6f10937910
struct OEmfPlusRecDrawClosedCurve
{
	enum Flag
	{
		FlagC				= 0x4000,
		FlagP				= 0x0800,
		FlagObjectIDMask	= 0x00FF,
	};

	Float					Tension;
	u32t					Count;
	OEmfPlusPointDataArray	PointData;

	bool Read(DataReader& reader, u16t nFlags, size_t nExpectedSize);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/e2002379-cb4c-47c0-a08a-239454aaf364
struct OEmfPlusRecDrawCurve
{
	enum Flag
	{
		FlagC				= 0x4000,
		FlagObjectIDMask	= 0x00FF,
	};

	Float					Tension;
	u32t					Offset;
	u32t					NumSegments;
	u32t					Count;
	OEmfPlusPointDataArray	PointData;

	bool Read(DataReader& reader, u16t nFlags, size_t nExpectedSize);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/b794b780-e9a8-4682-af65-9b614aecfbe6
struct OEmfPlusRecDrawDriverString
{
	enum Flag
	{
		FlagS				= 0x8000,
		FlagObjectIDMask	= 0x00FF,
	};

	u32t	BrushId;
	u32t	DriverStringOptionsFlags;	// see ODriverStringOptions
	Bool	MatrixPresent;
	u32t	GlyphCount;
	GSArray(u16t) Glyphs;
	GSArray(OEmfPlusPointF) GlyphPos;
	GSOptional(OEmfPlusTransformMatrix) TransformMatrix;

	bool Read(DataReader& reader, u16t nFlags, size_t nExpectedSize);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/450984d0-e056-4733-82de-749034fff56b
struct OEmfPlusRecDrawEllipse
{
	enum Flag
	{
		FlagC				= 0x4000,
		FlagObjectIDMask	= 0x00FF,
	};

	OEmfPlusRectData		RectData;

	bool Read(DataReader& reader, u16t nFlags, size_t nExpectedSize);
};

enum : u32t {InvalidObjectID = (u32t)-1,};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/778bfc49-e058-4fb2-b19d-9f801f8e458a
struct OEmfPlusRecDrawImage
{
	enum Flag
	{
		FlagC				= 0x4000,
		FlagObjectIDMask	= 0x00FF,
	};

	u32t				ImageAttributesID;	// could be InvalidObjectID
	OUnitType			SrcUnit;
	OEmfPlusRectF		SrcRect;
	OEmfPlusRectData	RectData;

	bool Read(DataReader& reader, u16t nFlags, size_t nExpectedSize);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/9bad3867-71b0-46fb-9b90-f7d8fb37ff76
struct OEmfPlusRecDrawImagePoints
{
	enum Flag
	{
		FlagC				= 0x4000,
		FlagE				= 0x2000,
		FlagP				= 0x0800,
		FlagObjectIDMask	= 0x00FF,
	};

	u32t					ImageAttributesID;	// could be InvalidObjectID
	OUnitType				SrcUnit;
	OEmfPlusRectF			SrcRect;
	u32t					Count;
	OEmfPlusPointDataArray	PointData;

	bool Read(DataReader& reader, u16t nFlags, size_t nExpectedSize);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/a5c0bc88-ab0e-4126-b68f-47b04bfc5cad
struct OEmfPlusRecDrawLines
{
	enum Flag
	{
		// Compressed data
		FlagC				= 0x4000,
		// Draw an extra line between the last point and the first point
		FlagL				= 0x2000,
		// This bit indicates whether the PointData field specifies relative or absolute locations.
		FlagP				= 0x0800,
		FlagObjectIDMask	= 0x00FF,
	};

	u32t	Count;
	OEmfPlusPointDataArray	PointData;

	bool Read(DataReader& reader, u16t nFlags, size_t nExpectedSize);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/3bfd9228-3b89-409c-bef2-ad3841b212a0
struct OEmfPlusRecDrawPath 
{
	enum Flag
	{
		FlagObjectIDMask	= 0x00FF,
	};
	u32t	PenId;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/bd93aa68-e96f-42f1-8aea-390920f327fd
struct OEmfPlusRecDrawPie 
{
	enum Flag
	{
		FlagC				= 0x4000,
		FlagObjectIDMask	= 0x00FF,
	};
	OEmfPlusArcData	ArcData;

	bool Read(DataReader& reader, u16t nFlags, size_t nExpectedSize);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/0f98925c-fecf-450c-ade0-dc837044ee4f
struct OEmfPlusRecDrawRects 
{
	enum Flag
	{
		FlagC				= 0x4000,
		FlagObjectIDMask	= 0x00FF,
	};
	OEmfPlusRectDataArray	RectData;

	bool Read(DataReader& reader, u16t nFlags, size_t nExpectedSize);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/ae7927c3-e416-4069-a9b8-3200113d6c41
struct OEmfPlusRecDrawString
{
	enum Flag
	{
		FlagS				= 0x8000,
		FlagObjectIDMask	= 0x00FF,
	};
	u32t					BrushId;
	u32t					FormatID;
	u32t					Length;
	OEmfPlusRectF			LayoutRect;
	std::vector<wchar_t>	StringData;

	bool Read(DataReader& reader, u16t nFlags, size_t nExpectedSize);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/d7b561b0-3dc7-4444-b7ac-55492b5af0f4
struct OEmfPlusRecFillClosedCurve 
{
	enum Flag
	{
		FlagS	= 0x8000,
		FlagC	= 0x4000,
		FlagW	= 0x2000,
		FlagP	= 0x0800,
	};
	u32t					BrushId;
	Float					Tension;
	u32t					Count;
	OEmfPlusPointDataArray	PointData;

	bool Read(DataReader& reader, u16t nFlags, size_t nExpectedSize);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/62d7bb9e-707c-4d9b-a69a-660350e858cb
struct OEmfPlusRecFillEllipse 
{
	enum Flag
	{
		FlagS	= 0x8000,
		FlagC	= 0x4000,
	};

	u32t					BrushId;
	OEmfPlusRectData		RectData;

	bool Read(DataReader& reader, u16t nFlags, size_t nExpectedSize);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/4a7b2c37-ba3d-42d8-a3cc-113af57958e9
struct OEmfPlusRecFillPath
{
	enum Flag
	{
		FlagS				= 0x8000,
		FlagObjectIDMask	= 0x00FF,
	};
	u32t	BrushId;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/59c3a6cc-1ab5-436c-b39a-b341779af88f
struct OEmfPlusRecFillPie
{
	enum Flag
	{
		FlagS	= 0x8000,
		FlagC	= 0x4000,
	};
	u32t			BrushId;
	OEmfPlusArcData	ArcData;

	bool Read(DataReader& reader, u16t nFlags, size_t nExpectedSize);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/813d7a7b-835d-4159-a8a8-bc3547a878e2
struct OEmfPlusRecFillPolygon 
{
	enum Flag
	{
		// If set, BrushId specifies a color as an OEmfPlusARGB object
		FlagS	= 0x8000,
		// This bit indicates whether the PointData field specifies compressed data.
		FlagC	= 0x4000,
		// Winding mode. Note: MS does not document this (maybe they forgot)
		FlagW	= 0x2000,
		// This bit indicates whether the PointData field specifies relative or absolute locations.
		FlagP	= 0x0800,
	};
	u32t					BrushId;
	u32t					Count;
	OEmfPlusPointDataArray	PointData;

	bool Read(DataReader& reader, u16t nFlags, size_t nExpectedSize);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/8d510051-eeb2-482f-9964-e9cd1dad6fca
struct OEmfPlusRecFillRects 
{
	enum Flag
	{
		FlagS	= 0x8000,	// If set, BrushId specifies a color
		FlagC	= 0x4000,	// If set, use RectData, otherwise use RectDataF
	};
	u32t					BrushId;
	OEmfPlusRectDataArray	RectData;

	bool Read(DataReader& reader, u16t nFlags, size_t nExpectedSize);
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/25183a76-a71f-46a1-9670-77dae28e0027
struct OEmfPlusRecFillRegion 
{
	enum Flag
	{
		FlagS				= 0x8000,
		FlagObjectIDMask	= 0x00FF,
	};
	u32t			BrushId;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/978404e2-c994-4e60-bade-cb6ab78e7ffc
struct OEmfPlusContinuedObjectRecord
{
	OEmfPlusRec	rec;
	u32t		TotalObjectSize;

	inline u8t* RecData() const
	{
		return (u8t*)this + sizeof(OEmfPlusContinuedObjectRecord);
	}
};

struct OEmfPlusContinuedObjectRecordData
{
	u32t	TotalObjectSize;

	inline u8t* RecData() const
	{
		return (u8t*)this + sizeof(OEmfPlusContinuedObjectRecordData);
	}
};

struct OEmfPlusRecObjectReader
{
	enum
	{
		FlagContinueObj		= 0x8000,
		FlagObjectTypeMask	= 0x7F00,
		FlagObjectIDMask	= 0x00FF,
	};

	// Optional
	u32t	TotalObjectSize;

	enum Status
	{
		StatusComplete,
		StatusContinue,
		StatusError,
	};
	Status Read(const OEmfPlusRecInfo& rec);

	OEmfPlusGraphObject* CreateObject();

	static OEmfPlusGraphObject* CreateObjectByType(OObjType objType);

	bool IsContinueObj() const;

	OObjType GetObjectType() const;

	static OObjType GetObjectType(const OEmfPlusRecInfo& rec);

	u8t GetObjectID() const;
private:
	const OEmfPlusRecInfo*	StartRec;
#ifdef _DEBUG
	UINT					ContinuableObjFlags = 0;
#endif // _DEBUG
	std::vector<u8t>		ContinuableObjData;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/bbd49011-1527-46be-8fe6-ccceecfd005f
struct OEmfPlusRecSerializableObject 
{
	//u8t		ObjectGUID[16];
	GUID	ObjectGUID;
	u32t	BufferSize;
	u8t*	Buffer;
};
static_assert(sizeof(GUID) == 16, "GUID must be of size 16!");

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/83b32511-b093-4e26-adf7-1538df304265
struct OEmfPlusRecSetAntiAliasMode 
{
	enum Flag
	{
		FlagSmoothingModeMask	= 0x00FE,
		FlagA					= 0x0001,
	};
	static OSmoothingMode GetSmoothingMode(u32t flags)
	{
		return (OSmoothingMode)((flags & FlagSmoothingModeMask) >> 1);
	}
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/d7ace3ef-2092-4e94-89da-103ac3f9ac5f
struct OEmfPlusRecSetCompositingMode 
{
	enum Flag
	{
		FlagCompositingModeMask	= 0x00FF,
	};
	static OCompositingMode GetCompositingMode(u32t flags)
	{
		return (OCompositingMode)(flags & FlagCompositingModeMask);
	}
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/99e4d432-526e-4fcf-872c-d0be6a6ffbfd
struct OEmfPlusRecSetCompositingQuality
{
	enum Flag
	{
		FlagCompositingQualityMask	= 0x00FF,
	};
	static OCompositingQuality GetCompositingQuality(u32t flags)
	{
		return (OCompositingQuality)(flags & FlagCompositingQualityMask);
	}
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/2fe03573-1e0c-4cc5-8ab3-74421ef17c18
struct OEmfPlusRecSetInterpolationMode
{
	enum Flag
	{
		FlagInterpolationModeyMask	= 0x00FF,
	};
	static OInterpolationMode GetInterpolationMode(u32t flags)
	{
		return (OInterpolationMode)(flags & FlagInterpolationModeyMask);
	}
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/679d2b1d-e618-4730-9400-fea5f4fd3b92
struct OEmfPlusRecSetPixelOffsetMode
{
	enum Flag
	{
		FlagPixelOffsetModeMask	= 0x00FF,
	};
	static OPixelOffsetMode GetPixelOffsetMode(u32t flags)
	{
		return (OPixelOffsetMode)(flags & FlagPixelOffsetModeMask);
	}
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/4f88414d-a117-4aea-9817-c05338585777
struct OEmfPlusRecSetRenderingOrigin
{
	i32t	x;
	i32t	y;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/3b0259a8-727d-4d25-a6d7-976a738f8aab
struct OEmfPlusRecSetTextContrast
{
	enum Flag
	{
		FlagTextContrastMask	= 0x00FF,
	};
	static u16t GetTextContrast(u32t flags)
	{
		return (u16t)(flags & FlagTextContrastMask);
	}
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/a05c335d-0777-4897-862a-aa35c0b684d8
struct OEmfPlusRecSetTextRenderingHint
{
	enum Flag
	{
		FlagTextRenderingHintMask	= 0x00FF,
	};
	static OTextRenderingHint GetTextRenderingHint(u32t flags)
	{
		return (OTextRenderingHint)(flags & FlagTextRenderingHintMask);
	}
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/9d8d1b89-349e-42da-aa74-8a68a3401601
struct OEmfPlusRecBeginContainer 
{
	enum Flag
	{
		FlagPageUnitMask	= 0xFF00,
	};

	static inline OUnitType	GetUnitType(u32t flags)
	{
		return (OUnitType)((flags & FlagPageUnitMask) >> 8);
	}

	OEmfPlusRectF	DestRect;
	OEmfPlusRectF	SrcRect;
	u32t			StackIndex;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/a7d12a9f-c4a8-4668-af29-fce0aa53b39c
struct OEmfPlusRecBeginContainerNoParams 
{
	u32t			StackIndex;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/6480c3e6-ed84-46fd-a908-8d101476c124
struct OEmfPlusRecEndContainer
{
	u32t			StackIndex;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/524b7c3c-9b25-4a33-8598-b5cea7173d0d
struct OEmfPlusRecRestore 
{
	u32t			StackIndex;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/6ceb9470-c3d3-43f1-b91e-0788ea329114
struct OEmfPlusRecSave
{
	u32t			StackIndex;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/0dfb6f4f-e53c-413b-80cf-57a3cadd5d38
struct OEmfPlusRecSetTSClip 
{
	enum Flag
	{
		FlagC				= 0x8000,
	};
	// NOT IMPLEMENTED
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/20124d49-50a9-4605-89c3-edaa68ac7230
struct OEmfPlusSetTSGraphics
{
	enum Flag
	{
		FlagV	= 0x0002,
		FlagT	= 0x0001,
	};

	OSmoothingMode			AntiAliasMode;
	OTextRenderingHint		TextRenderHint;
	OCompositingMode		CompositingMode;
	OCompositingQuality		CompositingQuality;
	i16t					RenderOriginX;
	i16t					RenderOriginY;
	u16t					TextContrast;
	OFilterType				FilterType;
	OPixelOffsetMode		PixelOffset;
	OEmfPlusTransformMatrix	WorldToDevice;

	// NOT IMPLEMENTED
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/673bb239-1826-4a9a-bbcd-40c741b0e482
struct OEmfPlusRecMultiplyWorldTransform 
{
	enum Flag
	{
		FlagA	= 0x2000,	// post-multiplied
	};

	OEmfPlusTransformMatrix	MatrixData;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/bf0811ba-bab6-4e01-ae19-e750be630391
struct OEmfPlusRecRotateWorldTransform 
{
	enum Flag
	{
		FlagA	= 0x2000,	// post-multiplied
	};

	Float		Angle;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/fc77b4c7-def1-43b1-8eb3-59f5e0410df7
struct OEmfPlusRecScaleWorldTransform 
{
	enum Flag
	{
		FlagA	= 0x2000,	// post-multiplied
	};

	Float		Sx;
	Float		Sy;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/9bf3fad2-4551-4e7b-9aaf-1bf017e7a473
struct OEmfPlusRecSetPageTransform 
{
	enum Flag
	{
		FlagPageUnitMask	= 0x00FF,
	};

	static inline OUnitType	GetUnitType(u32t flags)
	{
		return (OUnitType)(flags & FlagPageUnitMask);
	}

	Float	PageScale;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/b033cee7-7b03-40c1-8fb9-4de96c1d38e3
struct OEmfPlusRecSetWorldTransform 
{
	OEmfPlusTransformMatrix	MatrixData;
};

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/5b650f3d-59e4-4408-b768-7e516fe139bb
struct OEmfPlusRecTranslateWorldTransform 
{
	enum Flag
	{
		FlagA	= 0x2000,	// post-multiplied
	};
	Float	dx;
	Float	dy;
};

}

#pragma pop_macro("min")
#pragma pop_macro("max")

#endif // _ENABLE_GDIPLUS_STRUCT

#endif // EMFPLUSSTRUCT_H