#include PCH_FNAME
#ifdef _ENABLE_GDIPLUS_STRUCT

#include "EmfPlusStruct.h"
#include <sstream>
#include <iomanip>

namespace emfplus
{

bool OEmfPlusGraphObject::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	u32t Version = 0;
	reader.ReadBytes(&Version, sizeof(Version));
	enum
	{
		VersionMetafileSignature		= 0xDBC01,
		VersionMetafileSignatureMask	= 0xFFFFF000,
		VersionGraphicsVersionMask		= 0x00000FFF,
	};
	ASSERT(VersionMetafileSignature == ((Version & VersionMetafileSignatureMask) >> 12)
		&& GraphicsVersion1_1 == (Version & VersionGraphicsVersionMask));
	return true;
}

static inline i16t	_ReadEmfPlusPointRInteger(DataReader& reader)
{
	i16t val = 0;
	u8t n1{ 0 };
	reader.ReadBytes(&n1, 1);
	if (n1 & 0x80)
	{
		// must be a EmfPlusInteger15 object
		val = (i16t)((n1 & ~0x80) << 8);
		reader.ReadBytes(&n1, 1);
		val |= n1;
	}
	else
	{
		// must be a EmfPlusInteger7 object
		val = (i16t)(n1 & ~0x80);
	}
	return val;
}

void OEmfPlusPointDataArray::Read(DataReader& reader, u32t Count, bool bRelative, bool asInt)
{
	if (Count == 0)
		return;
	if (bRelative)
	{
		// the following code is not tested because I don't know how to make
		// such case, please verify and remove the assert
		ASSERT(0);
		// EmfPlusPointR
		// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-emfplus/c861a0d4-39f0-4f6c-bad9-e3f7bf63205e
		i.resize((size_t)Count);
		for (u32t ii = 0; ii < Count; ++ii)
		{
			i[ii].x = _ReadEmfPlusPointRInteger(reader);
			i[ii].y = _ReadEmfPlusPointRInteger(reader);
		}
	}
	else
	{
		if (asInt)
			reader.ReadArray(i, (size_t)Count);
		else
			reader.ReadArray(f, (size_t)Count);
	}
}

void OEmfPlusPointDataArray::Reset()
{
	i.clear();
	f.clear();
}

bool OEmfPlusPath::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	OEmfPlusGraphObject::Read(reader);
	reader.ReadBytes(&PathPointCount, sizeof(PathPointCount));
	reader.ReadBytes(&PathPointFlags, sizeof(PathPointFlags));
	PathPoints.Read(reader, PathPointCount, PointsAreRelative(), !PointsAreFloats());

	if (PathPointTypesAreRLE())
	{
		// Not tested implementation, please check
		ASSERT(0);
		reader.ReadArray(PathPointTypes, (size_t)PathPointCount);
	}
	else
		reader.ReadArray(PathPointTypes, (size_t)PathPointCount, sizeof(OPathPointType));
	readerCheck.SkipAlignmentPadding();
	return true;
}

void OEmfPlusPath::Reset()
{
	PathPointCount = 0;
	PathPointFlags = 0;
	PathPoints.Reset();
}

bool OEmfPlusRegionNodePath::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	u32t RegionNodePathLength = 0;
	reader.ReadBytes(&RegionNodePathLength, sizeof(RegionNodePathLength));
	ASSERT(readerCheck.CheckLeftoverSize((size_t)RegionNodePathLength));
	RegionNodePath.Read(reader, (size_t)RegionNodePathLength);
	return true;
}

OEmfPlusRegionNode::OEmfPlusRegionNode(const OEmfPlusRegionNode& other)
{
	*this = other;
}

OEmfPlusRegionNode::OEmfPlusRegionNode()
{
	MakeInfinite();
}

OEmfPlusRegionNode& OEmfPlusRegionNode::operator=(const OEmfPlusRegionNode& other)
{
	Type = other.Type;
	memcpy(transform, other.transform, sizeof(transform));
	path.Reset();
	childNodes.reset();
	switch (Type)
	{
	case ORegionNodeDataTypeEmpty:
	case ORegionNodeDataTypeInfinite:
		// nothing
		break;
	case ORegionNodeDataTypeRect:
		rect = other.rect;
		break;
	case ORegionNodeDataTypePath:
		path = other.path;
		break;
	case ORegionNodeDataTypeAnd:
	case ORegionNodeDataTypeOr:
	case ORegionNodeDataTypeXor:
	case ORegionNodeDataTypeExclude:
	case ORegionNodeDataTypeComplement:
		if (other.childNodes)
			childNodes = std::make_unique<OEmfPlusRegionNodeChildNodes>(*other.childNodes);
		break;
	default:
		ASSERT(0);
		break;
	}
	return *this;
}

bool OEmfPlusRegionNode::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&Type, sizeof(Type));
	switch (Type)
	{
	default:
		ASSERT(0);
	case ORegionNodeDataTypeEmpty:
	case ORegionNodeDataTypeInfinite:
		// nothing
		break;
	case ORegionNodeDataTypeRect:
		reader.ReadBytes(&rect, sizeof(rect));
		break;
	case ORegionNodeDataTypePath:
		path.Read(reader);
		break;
	case ORegionNodeDataTypeAnd:
	case ORegionNodeDataTypeOr:
	case ORegionNodeDataTypeXor:
	case ORegionNodeDataTypeExclude:
	case ORegionNodeDataTypeComplement:
		if (!childNodes.get())
			childNodes = std::make_unique<OEmfPlusRegionNodeChildNodes>();
		childNodes->Read(reader, readerCheck.GetLeftoverSize());
		break;
	}
	return true;
}

void OEmfPlusRegionNode::MakeEmpty()
{
	Type = ORegionNodeDataTypeEmpty;
	childNodes.reset();
	SetTransformAsIdentity();
}

void OEmfPlusRegionNode::MakeInfinite()
{
	Type = ORegionNodeDataTypeInfinite;
	childNodes.reset();
	SetTransformAsIdentity();
}

bool OEmfPlusRegionNodeChildNodes::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	Left.Read(reader);
	Right.Read(reader, readerCheck.GetLeftoverSize());
	return true;
}

bool OEmfPlusDashedLineData::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&DashedLineDataSize, sizeof(DashedLineDataSize));
	reader.ReadArray(DashedLineData, (size_t)DashedLineDataSize);
	return true;
}

bool OEmfPlusCompoundLineData::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&CompoundLineDataSize, sizeof(CompoundLineDataSize));
	reader.ReadArray(CompoundLineData, (size_t)CompoundLineDataSize);
	return true;
}

bool OEmfPlusFillPath::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&FillPathLength, sizeof(FillPathLength));
	ASSERT(readerCheck.CheckLeftoverSize((size_t)FillPathLength));
	FillPath.Read(reader, (size_t)FillPathLength);
	return true;
}

bool OEmfPlusLinePath::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&LinePathLength, sizeof(LinePathLength));
	ASSERT(readerCheck.CheckLeftoverSize((size_t)LinePathLength));
	LinePath.Read(reader, (size_t)LinePathLength);
	return true;
}

bool OEmfPlusCustomLineCapData::OEmfPlusCustomLineCapOptionalData::Read(DataReader& reader, u32t CustomLineCapDataFlags, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	if (CustomLineCapDataFlags & (u32t)OCustomLineCapData::FillPath)
		FillData.Read(reader);
	if (CustomLineCapDataFlags & (u32t)OCustomLineCapData::LinePath)
		OutlineData.Read(reader, readerCheck.GetLeftoverSize());
	return true;
}

bool OEmfPlusCustomLineCapData::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&CustomLineCapDataFlags, sizeof(CustomLineCapDataFlags));
	reader.ReadBytes(&BaseCap, sizeof(BaseCap));
	reader.ReadBytes(&BaseInset, sizeof(BaseInset));
	reader.ReadBytes(&StrokeStartCap, sizeof(StrokeStartCap));
	reader.ReadBytes(&StrokeEndCap, sizeof(StrokeEndCap));
	reader.ReadBytes(&StrokeJoin, sizeof(StrokeJoin));
	reader.ReadBytes(&StrokeMiterLimit, sizeof(StrokeMiterLimit));
	reader.ReadBytes(&WidthScale, sizeof(WidthScale));
	reader.ReadBytes(&FillHotSpot, sizeof(FillHotSpot));
	reader.ReadBytes(&StrokeHotSpot, sizeof(StrokeHotSpot));
	OptionalData.Read(reader, CustomLineCapDataFlags, readerCheck.GetLeftoverSize());
	return true;
}

bool OEmfPlusCustomLineCap::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	OEmfPlusGraphObject::Read(reader);
	reader.ReadBytes(&Type, sizeof(Type));
	if (Type == OCustomLineCapDataType::Default)
	{
		CustomLineCapData.Read(reader, readerCheck.GetLeftoverSize());
	}
	else
	{
		ASSERT(Type == OCustomLineCapDataType::AdjustableArrow);
		reader.ReadBytes(&CustomLineCapDataArrow, sizeof(CustomLineCapDataArrow));
	}
	return true;
}

bool OEmfPlusCustomStartCapData::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&CustomStartCapSize, sizeof(CustomStartCapSize));
	CustomStartCap.Read(reader, readerCheck.GetLeftoverSize());
	return true;
}

bool OEmfPlusCustomEndCapData::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&CustomEndCapSize, sizeof(CustomEndCapSize));
	CustomEndCap.Read(reader, readerCheck.GetLeftoverSize());
	return true;
}

bool OEmfPlusPenData::OEmfPlusPenOptionalData::Read(DataReader& reader, u32t PenDataFlags, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	if (PenDataFlags & (u32t)OPenData::Transform)
		reader.ReadBytes(&TransformMatrix, sizeof(TransformMatrix));
	if (PenDataFlags & (u32t)OPenData::StartCap)
		reader.ReadBytes(&StartCap, sizeof(StartCap));
	if (PenDataFlags & (u32t)OPenData::EndCap)
		reader.ReadBytes(&EndCap, sizeof(EndCap));
	if (PenDataFlags & (u32t)OPenData::Join)
		reader.ReadBytes(&Join, sizeof(Join));
	if (PenDataFlags & (u32t)OPenData::MiterLimit)
		reader.ReadBytes(&MiterLimit, sizeof(MiterLimit));
	if (PenDataFlags & (u32t)OPenData::LineStyle)
		reader.ReadBytes(&LineStyle, sizeof(LineStyle));
	if (PenDataFlags & (u32t)OPenData::DashedLineCap)
		reader.ReadBytes(&DashedLineCapType, sizeof(DashedLineCapType));
	if (PenDataFlags & (u32t)OPenData::DashedLineOffset)
		reader.ReadBytes(&DashOffset, sizeof(DashOffset));
	if (PenDataFlags & (u32t)OPenData::DashedLine)
		DashedLineData.Read(reader);
	if (PenDataFlags & (u32t)OPenData::NonCenter)
		reader.ReadBytes(&PenAlignment, sizeof(PenAlignment));
	if (PenDataFlags & (u32t)OPenData::CompoundLine)
		CompoundLineData.Read(reader);
	if (PenDataFlags & (u32t)OPenData::CustomStartCap)
		CustomStartCapData.Read(reader);
	if (PenDataFlags & (u32t)OPenData::CustomEndCap)
		CustomEndCapData.Read(reader);
	return true;
}

bool OEmfPlusPenData::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&PenDataFlags, sizeof(PenDataFlags));
	reader.ReadBytes(&PenUnit, sizeof(PenUnit));
	reader.ReadBytes(&PenWidth, sizeof(PenWidth));
	OptionalData.Read(reader, PenDataFlags, readerCheck.GetLeftoverSize());
	return true;
}

std::string OEmfPlusARGB::GetColorText() const
{
	std::ostringstream ss;
#ifdef USE_RGBA_FOR_COLOR
	if (Alpha == 255)
#endif // USE_RGBA_FOR_COLOR
	{
		switch (argb & 0x00ffffff)
		{
		case 0:
			return "black";
		case 0xffffff:
			return "white";
		case 0xff0000:
			return "red";
		case 0x00ff00:
			return "lime";
		case 0x008000:
			return "green";
		case 0x0000ff:
			return "blue";
		}
		ss << "#" << std::uppercase << std::setfill('0') << std::setw(6) 
			<< std::hex << (argb & 0x00FFFFFF);
	}
#ifdef USE_RGBA_FOR_COLOR
	else
	{
		ss << "rgba(" << (short)Red << "," << (short)Green << "," << (short)Blue;
		ss << "," << (Alpha/255.f) << ")";
	}
#endif // USE_RGBA_FOR_COLOR
	return ss.str();
}

bool ORedEyeCorrectionEffect::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&NumberOfAreas, sizeof(NumberOfAreas));
	reader.ReadArray(Areas, (size_t)NumberOfAreas);
	return true;
}

bool OEmfPlusBlendColors::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&PositionCount, sizeof(PositionCount));
	reader.ReadArray(BlendPositions, (size_t)PositionCount);
	reader.ReadArray(BlendColors, (size_t)PositionCount);
	return true;
}

bool OEmfPlusBlendFactors::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&PositionCount, sizeof(PositionCount));
	reader.ReadArray(BlendPositions, (size_t)PositionCount);
	reader.ReadArray(BlendFactors, (size_t)PositionCount);
	return true;
}

bool OEmfPlusLinearGradientBrushData::OEmfPlusLinearGradientBrushOptionalData::Read(DataReader& reader, u32t BrushDataFlags, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	if (BrushDataFlags & (u32t)OBrushData::Transform)
		reader.ReadBytes(TransformMatrix, sizeof(TransformMatrix));
	if (BrushDataFlags & (u32t)OBrushData::PresetColors)
		BlendPattern.colors.Read(reader);
	if (BrushDataFlags & (u32t)OBrushData::BlendFactorsH)
		BlendPattern.factorsH.Read(reader);
	if (BrushDataFlags & (u32t)OBrushData::BlendFactorsV)
		BlendPattern.factorsV.Read(reader);
	return true;
}

bool OEmfPlusLinearGradientBrushData::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&BrushDataFlags, sizeof(BrushDataFlags));
	reader.ReadBytes(&WrapMode, sizeof(WrapMode));
	reader.ReadBytes(&RectF, sizeof(RectF));
	reader.ReadBytes(&StartColor, sizeof(StartColor));
	reader.ReadBytes(&EndColor, sizeof(EndColor));
	reader.ReadBytes(&Reserved1, sizeof(Reserved1));
	reader.ReadBytes(&Reserved2, sizeof(Reserved2));
	OptionalData.Read(reader, BrushDataFlags, readerCheck.GetLeftoverSize());
	return true;
}

bool OEmfPlusBoundaryPathData::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&BoundaryPathSize, sizeof(BoundaryPathSize));
	ASSERT(readerCheck.CheckLeftoverSize((size_t)BoundaryPathSize));
	BoundaryPathData.Read(reader, (size_t)BoundaryPathSize);
	return true;
}

bool OEmfPlusBoundaryPointData::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&BoundaryPointCount, sizeof(BoundaryPointCount));
	reader.ReadArray(BoundaryPointData, (size_t)BoundaryPointCount);
	return true;
}

bool OEmfPlusPathGradientBrushData::OEmfPlusPathGradientBrushOptionalData::Read(DataReader& reader, u32t BrushDataFlags, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	if (BrushDataFlags & (u32t)OBrushData::Transform)
		reader.ReadBytes(TransformMatrix, sizeof(TransformMatrix));
	if (BrushDataFlags & (u32t)OBrushData::PresetColors)
		BlendPattern.colors.Read(reader);
	if (BrushDataFlags & (u32t)OBrushData::BlendFactorsH)
		BlendPattern.factors.Read(reader);
	if (BrushDataFlags & (u32t)OBrushData::FocusScales)
		reader.ReadBytes(&FocusScaleData, sizeof(FocusScaleData));
	return true;
}

bool OEmfPlusPathGradientBrushData::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&BrushDataFlags, sizeof(BrushDataFlags));
	reader.ReadBytes(&WrapMode, sizeof(WrapMode));
	reader.ReadBytes(&CenterColor, sizeof(CenterColor));
	reader.ReadBytes(&CenterPointF, sizeof(CenterPointF));
	reader.ReadBytes(&SurroundingColorCount, sizeof(SurroundingColorCount));
	reader.ReadArray(SurroundingColor, (size_t)SurroundingColorCount);
	if (BrushDataFlags & (u32t)OBrushData::Path)
		BoundaryDataPath.Read(reader);
	else
		BoundaryDataPoint.Read(reader);
	OptionalData.Read(reader, BrushDataFlags, readerCheck.GetLeftoverSize());
	return true;
}

bool OEmfPlusTextureBrushData::OEmfPlusTextureBrushOptionalData::Read(DataReader& reader, OBrushData BrushDataFlags, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	if ((u32t)BrushDataFlags & (u32t)OBrushData::Transform)
		reader.ReadBytes(&TransformMatrix, sizeof(TransformMatrix));
	auto nImgSize = readerCheck.GetLeftoverSize();
	if (nImgSize > 0)
		ImageObject.Read(reader, nImgSize);
	return true;
}

bool OEmfPlusTextureBrushData::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&BrushDataFlags, sizeof(BrushDataFlags));
	reader.ReadBytes(&WrapMode, sizeof(WrapMode));
	Optionaldata.Read(reader, BrushDataFlags, readerCheck.GetLeftoverSize());
	return true;
}

bool OEmfPlusBrush::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	OEmfPlusGraphObject::Read(reader);
	reader.ReadBytes(&Type, sizeof(Type));
	switch (Type)
	{
	case OBrushType::SolidColor:
		reader.ReadBytes(&BrushDataSolid, sizeof(BrushDataSolid));
		break;
	case OBrushType::HatchFill:
		reader.ReadBytes(&BrushDataHatch, sizeof(BrushDataHatch));
		break;
	case OBrushType::TextureFill:
		BrushDataTexture.Read(reader, readerCheck.GetLeftoverSize());
		break;
	case OBrushType::PathGradient:
		BrushDataPathGrad.Read(reader, readerCheck.GetLeftoverSize());
		break;
	case OBrushType::LinearGradient:
		BrushDataLinearGrad.Read(reader, readerCheck.GetLeftoverSize());
		break;
	}
	return true;
}

bool OEmfPlusPen::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	OEmfPlusGraphObject::Read(reader);
	u32t Type = 0;	// must be zero
	reader.ReadBytes(&Type, sizeof(Type));
	ASSERT(Type == 0);
	PenData.Read(reader);
	BrushObject.Read(reader, readerCheck.GetLeftoverSize());
	return true;
}

bool OEmfPlusRegion::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	OEmfPlusGraphObject::Read(reader);
	u32t RegionNodeCount = 0;
	reader.ReadBytes(&RegionNodeCount, sizeof(RegionNodeCount));
	RegionNode.Read(reader, readerCheck.GetLeftoverSize());
	return true;
}

bool OEmfPlusMetafile::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&Type, sizeof(Type));
	reader.ReadBytes(&MetafileDataSize, sizeof(MetafileDataSize));
	reader.ReadArray(MetafileData, (size_t)MetafileDataSize);
	return true;
}

bool OEmfPlusPalette::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&PaletteStyleFlags, sizeof(PaletteStyleFlags));
	reader.ReadBytes(&PaletteCount, sizeof(PaletteCount));
	reader.ReadArray(PaletteEntries, (size_t)PaletteCount);
	return true;
}

bool OEmfPlusBitmapData::Read(DataReader& reader, OPixelFormat PixelFormat, size_t nExpectedSize)
{
	// TODO, OEmfPlusBitmapData::Read, not tested yet
	ASSERT(0);
	ASSERT(nExpectedSize != SIZE_MAX);
	ReaderChecker readerCheck(reader, nExpectedSize);
	if ((u32t)PixelFormat & (u32t)OPixelFormat::FormatIFlag)
		Colors.Read(reader);
	reader.ReadArray(PixelData, (size_t)readerCheck.GetLeftoverSize());
	return true;
}

bool OEmfPlusCompressedImage::Read(DataReader& reader, size_t nExpectedSize)
{
	ASSERT(nExpectedSize != SIZE_MAX);
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadArray(CompressedImageData, (size_t)nExpectedSize);
	return true;
}

bool OEmfPlusBitmap::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&Width, sizeof(Width));
	reader.ReadBytes(&Height, sizeof(Height));
	reader.ReadBytes(&Stride, sizeof(Stride));
	reader.ReadBytes(&PixelFormat, sizeof(PixelFormat));
	reader.ReadBytes(&Type, sizeof(Type));
	if (Type == OBitmapDataType::Pixel)
		BitmapData.Read(reader, PixelFormat, readerCheck.GetLeftoverSize());
	else
		BitmapDataCompressed.Read(reader, readerCheck.GetLeftoverSize());
	return true;
}

bool OEmfPlusImage::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	OEmfPlusGraphObject::Read(reader);
	reader.ReadBytes(&Type, sizeof(Type));
	switch (Type)
	{
	case OImageDataType::Unknown:
		ASSERT(0);
		break;
	case OImageDataType::Bitmap:
		ImageDataBmp.Read(reader, readerCheck.GetLeftoverSize());
		break;
	case OImageDataType::Metafile:
		ImageDataMetafile.Read(reader, readerCheck.GetLeftoverSize());
		break;
	}
	return true;
}

bool OEmfPlusImageAttributes::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	OEmfPlusGraphObject::Read(reader);
	reader.ReadBytes(&Reserved1, sizeof(Reserved1));
	reader.ReadBytes(&WrapMode, sizeof(WrapMode));
	reader.ReadBytes(&ClampColor, sizeof(ClampColor));
	reader.ReadBytes(&ObjectClamp, sizeof(ObjectClamp));
	reader.ReadBytes(&Reserved2, sizeof(Reserved2));
	return true;
}

bool OEmfPlusFont::Read(DataReader& reader, size_t nExpectedSize)
{
	ASSERT(nExpectedSize != SIZE_MAX);
	ReaderChecker readerCheck(reader, nExpectedSize);
	OEmfPlusGraphObject::Read(reader);
	reader.ReadBytes(&EmSize, sizeof(EmSize));
	reader.ReadBytes(&SizeUnit, sizeof(SizeUnit));
	reader.ReadBytes(&FontStyleFlags, sizeof(FontStyleFlags));
	reader.ReadBytes(&Reserved, sizeof(Reserved));
	reader.ReadBytes(&Length, sizeof(Length));
	std::vector<wchar_t> vsTemp;
	reader.ReadArray(vsTemp, (size_t)Length);
	vsTemp.push_back(L'\0');
	FamilyName.assign(vsTemp.data());
	auto leftOver = readerCheck.GetLeftoverSize();
	if (leftOver)
	{
		// I have tested a few cases, it seems that sometimes there could be 2 extra bytes
		// left, so I am putting this here just to verify that.
		ASSERT(leftOver == 2);
		reader.Skip(leftOver);
	}
	return true;
}

bool OEmfPlusStringFormat::Read(DataReader& reader, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	OEmfPlusGraphObject::Read(reader);
	reader.ReadBytes(&StringFormatFlags, sizeof(StringFormatFlags));
	reader.ReadBytes(&Language, sizeof(Language));
	reader.ReadBytes(&StringAlignment, sizeof(StringAlignment));
	reader.ReadBytes(&LineAlign, sizeof(LineAlign));
	reader.ReadBytes(&DigitSubstitution, sizeof(DigitSubstitution));
	reader.ReadBytes(&DigitLanguage, sizeof(DigitLanguage));
	reader.ReadBytes(&FirstTabOffset, sizeof(FirstTabOffset));
	reader.ReadBytes(&HotkeyPrefix, sizeof(HotkeyPrefix));
	reader.ReadBytes(&LeadingMargin, sizeof(LeadingMargin));
	reader.ReadBytes(&TrailingMargin, sizeof(TrailingMargin));
	reader.ReadBytes(&Tracking, sizeof(Tracking));
	reader.ReadBytes(&Trimming, sizeof(Trimming));
	reader.ReadBytes(&TabStopCount, sizeof(TabStopCount));
	reader.ReadBytes(&RangeCount, sizeof(RangeCount));
	reader.ReadArray(StringFormatData.TabStops, (size_t)TabStopCount);
	reader.ReadArray(StringFormatData.CharRange, (size_t)RangeCount);
	return true;
}

bool OEmfPlusRecComment::Read(DataReader& reader, u16t /*nFlags*/, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadArray(PrivateData, nExpectedSize);
	return true;
}

void OEmfPlusRectData::Read(DataReader& reader, bool asInt)
{
	AsInt = asInt;
	if (asInt)
		reader.ReadBytes(&i, sizeof(i));
	else
		reader.ReadBytes(&f, sizeof(f));
}

void OEmfPlusRectDataArray::Read(DataReader& reader, bool asInt)
{
	reader.ReadBytes(&Count, sizeof(Count));
	if (Count)
	{
		if (asInt)
			reader.ReadArray(i, (size_t)Count);
		else
			reader.ReadArray(f, (size_t)Count);
	}
}

bool OEmfPlusArcData::Read(DataReader& reader, bool bIntRectData, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&StartAngle, sizeof(StartAngle));
	reader.ReadBytes(&SweepAngle, sizeof(SweepAngle));
	RectData.Read(reader, bIntRectData);
	return true;
}

bool OEmfPlusRecDrawArc::Read(DataReader& reader, u16t nFlags, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	ArcData.Read(reader, nFlags & FlagC, nExpectedSize);
	return true;
}

bool OEmfPlusRecDrawBeziers::Read(DataReader& reader, u16t nFlags, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&Count, sizeof(Count));
	PointData.Read(reader, Count, FlagP & nFlags, FlagC & nFlags);
	return true;
}

bool OEmfPlusRecDrawClosedCurve::Read(DataReader& reader, u16t nFlags, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&Tension, sizeof(Tension));
	reader.ReadBytes(&Count, sizeof(Count));
	PointData.Read(reader, Count, FlagP & nFlags, FlagC & nFlags);
	return true;
}

bool OEmfPlusRecDrawCurve::Read(DataReader& reader, u16t nFlags, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&Tension, sizeof(Tension));
	reader.ReadBytes(&Offset, sizeof(Offset));
	reader.ReadBytes(&NumSegments, sizeof(NumSegments));
	reader.ReadBytes(&Count, sizeof(Count));
	PointData.Read(reader, Count, false, FlagC & nFlags);
	return true;
}

bool OEmfPlusRecDrawDriverString::Read(DataReader& reader, u16t /*nFlags*/, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&BrushId, sizeof(BrushId));
	reader.ReadBytes(&DriverStringOptionsFlags, sizeof(DriverStringOptionsFlags));
	reader.ReadBytes(&MatrixPresent, sizeof(MatrixPresent));
	reader.ReadBytes(&GlyphCount, sizeof(GlyphCount));
	reader.ReadArray(Glyphs, (size_t)GlyphCount);
	reader.ReadArray(GlyphPos, (size_t)GlyphCount);
	if (MatrixPresent)
		reader.ReadBytes(&TransformMatrix, sizeof(TransformMatrix));
	// Note: this is not documented by MS but I keep seeing assert in readerCheck so I added this
	readerCheck.SkipAlignmentPadding();
	return true;
}

bool OEmfPlusRecDrawEllipse::Read(DataReader& reader, u16t nFlags, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	RectData.Read(reader, nFlags & FlagC);
	return true;
}

bool OEmfPlusRecDrawImage::Read(DataReader& reader, u16t nFlags, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&ImageAttributesID, sizeof(ImageAttributesID));
	reader.ReadBytes(&SrcUnit, sizeof(SrcUnit));
	reader.ReadBytes(&SrcRect, sizeof(SrcRect));
	RectData.Read(reader, nFlags & FlagC);
	return true;
}

bool OEmfPlusRecDrawImagePoints::Read(DataReader& reader, u16t nFlags, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&ImageAttributesID, sizeof(ImageAttributesID));
	reader.ReadBytes(&SrcUnit, sizeof(SrcUnit));
	reader.ReadBytes(&SrcRect, sizeof(SrcRect));
	reader.ReadBytes(&Count, sizeof(Count));
	ASSERT(Count == 3);
	PointData.Read(reader, Count, FlagP & nFlags, FlagC & nFlags);
	return true;
}

bool OEmfPlusRecDrawLines::Read(DataReader& reader, u16t nFlags, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&Count, sizeof(Count));
	PointData.Read(reader, Count, FlagP & nFlags, FlagC & nFlags);
	return true;
}

bool OEmfPlusRecDrawPie::Read(DataReader& reader, u16t nFlags, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	ArcData.Read(reader, nFlags & FlagC, nExpectedSize);
	return true;
}

bool OEmfPlusRecDrawRects::Read(DataReader& reader, u16t nFlags, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	RectData.Read(reader, nFlags & FlagC);
	return true;
}

bool OEmfPlusRecDrawString::Read(DataReader& reader, u16t /*nFlags*/, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&BrushId, sizeof(BrushId));
	reader.ReadBytes(&FormatID, sizeof(FormatID));
	reader.ReadBytes(&Length, sizeof(Length));
	reader.ReadBytes(&LayoutRect, sizeof(LayoutRect));
	reader.ReadArray(StringData, (size_t)Length);
	StringData.push_back(L'\0');
	readerCheck.SkipAlignmentPadding();
	return true;
}

bool OEmfPlusRecFillClosedCurve::Read(DataReader& reader, u16t nFlags, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&BrushId, sizeof(BrushId));
	reader.ReadBytes(&Tension, sizeof(Tension));
	reader.ReadBytes(&Count, sizeof(Count));
	PointData.Read(reader, Count, FlagP & nFlags, FlagC & nFlags);
	return true;
}

bool OEmfPlusRecFillEllipse::Read(DataReader& reader, u16t nFlags, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&BrushId, sizeof(BrushId));
	RectData.Read(reader, nFlags & FlagC);
	return true;
}

bool OEmfPlusRecFillPie::Read(DataReader& reader, u16t nFlags, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&BrushId, sizeof(BrushId));
	ArcData.Read(reader, nFlags & FlagC, readerCheck.GetLeftoverSize());
	return true;
}

bool OEmfPlusRecFillPolygon::Read(DataReader& reader, u16t nFlags, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&BrushId, sizeof(BrushId));
	reader.ReadBytes(&Count, sizeof(Count));
	PointData.Read(reader, Count, FlagP & nFlags, FlagC & nFlags);
	return true;
}

bool OEmfPlusRecFillRects::Read(DataReader& reader, u16t nFlags, size_t nExpectedSize)
{
	ReaderChecker readerCheck(reader, nExpectedSize);
	reader.ReadBytes(&BrushId, sizeof(BrushId));
	RectData.Read(reader, nFlags & FlagC);
	return true;
}

auto OEmfPlusRecObjectReader::Read(const OEmfPlusRecInfo& rec) -> Status
{
	if (!StartRec)
		StartRec = &rec;
	if ( (rec.Flags & FlagContinueObj) || !ContinuableObjData.empty())
	{
		// According to MS-EMFPLUS:
		// This (FlagContinueObj) flag is never set in the final record that defines the object.
		// However, this is not true judging from the test case, so do not make assumption on it.
		auto pContinueObj = (const OEmfPlusContinuedObjectRecordData*)rec.Data;
		bool bIsContinueObj = rec.Flags & FlagContinueObj;
		const u8t* pData = bIsContinueObj ? pContinueObj->RecData() : rec.Data;
		size_t nDataSize = rec.DataSize;
		if (ContinuableObjData.empty())
		{
		#ifdef _DEBUG
			ContinuableObjFlags = rec.Flags & ~FlagContinueObj;
		#endif // _DEBUG
			TotalObjectSize = pContinueObj->TotalObjectSize;
		}
		else
		{
			ASSERT(!bIsContinueObj || pContinueObj->TotalObjectSize == TotalObjectSize);
		}
		ContinuableObjData.resize(ContinuableObjData.size() + nDataSize);
		std::memcpy(ContinuableObjData.data() + ContinuableObjData.size() - nDataSize, pData, nDataSize);
		if (ContinuableObjData.size() < (size_t)TotalObjectSize)
			return StatusContinue;

	}
	return StatusComplete;
}

OEmfPlusGraphObject* OEmfPlusRecObjectReader::CreateObject()
{
	if (!StartRec)
	{
		// Call Read() first!
		ASSERT(0);
		return nullptr;
	}
	auto nObjType = GetObjectType();
	OEmfPlusGraphObject* pObj = nullptr;
	switch (nObjType)
	{
	case OObjType::Brush:
		pObj = new OEmfPlusBrush;
		break;
	case OObjType::Pen:
		pObj = new OEmfPlusPen;
		break;
	case OObjType::Path:
		pObj = new OEmfPlusPath;
		break;
	case OObjType::Region:
		pObj = new OEmfPlusRegion;
		break;
	case OObjType::Image:
		pObj = new OEmfPlusImage;
		break;
	case OObjType::Font:
		pObj = new OEmfPlusFont;
		break;
	case OObjType::StringFormat:
		pObj = new OEmfPlusStringFormat;
		break;
	case OObjType::ImageAttributes:
		pObj = new OEmfPlusImageAttributes;
		break;
	case OObjType::CustomLineCap:
		pObj = new OEmfPlusCustomLineCap;
		break;
	default:
		return nullptr;
	}
	if (pObj)
	{
		bool bContinue = IsContinueObj();
		const u8t* pData = bContinue ? ContinuableObjData.data() : StartRec->Data;
		size_t nDataSize = bContinue ? TotalObjectSize : StartRec->DataSize;
		DataReader reader(pData, nDataSize);
		VERIFY(pObj->Read(reader, nDataSize));

		ContinuableObjData.clear();
	#ifdef _DEBUG
		ContinuableObjFlags = 0;
	#endif // _DEBUG
		StartRec = nullptr;
	}
	return pObj;
}

bool OEmfPlusRecObjectReader::IsContinueObj() const
{
	if (!StartRec)
	{
		// Call Read() first!
		ASSERT(0);
		return false;
	}
	return StartRec->Flags & FlagContinueObj;
}

OObjType OEmfPlusRecObjectReader::GetObjectType() const
{
	if (!StartRec)
	{
		// Call Read() first!
		ASSERT(0);
		return OObjType::Invalid;
	}
	auto nObjType = (OObjType)((StartRec->Flags & FlagObjectTypeMask) >> 8);
	return nObjType;
}

u8t OEmfPlusRecObjectReader::GetObjectID() const
{
	if (!StartRec)
	{
		// Call Read() first!
		ASSERT(0);
		return (u8t)-1;
	}
	auto nObjectID = (u8t)(StartRec->Flags & FlagObjectIDMask);
	return nObjectID;
}

}

#endif // _ENABLE_GDIPLUS_STRUCT
