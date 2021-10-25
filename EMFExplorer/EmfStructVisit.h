#ifndef EMF_STRUCT_VISIT_H
#define EMF_STRUCT_VISIT_H

#ifndef GS_VISITABLE_STRUCT
	#define GS_VISITABLE_STRUCT(...)
#endif

GS_VISITABLE_STRUCT(OEmfPlusPoint, x, y);
GS_VISITABLE_STRUCT(OEmfPlusPointF, x, y);
GS_VISITABLE_STRUCT(OEmfPlusPath, PathPointCount, PathPointFlags, PathPoints, 
	PathPointTypes, PathPointTypesRLE);
GS_VISITABLE_STRUCT(OEmfPlusRegionNodePath, RegionNodePath);
GS_VISITABLE_STRUCT(OEmfPlusRect, X, Y, Width, Height);
GS_VISITABLE_STRUCT(OEmfPlusRectF, X, Y, Width, Height);
GS_VISITABLE_STRUCT(ORectL, Left, Top, Right, Bottom);
GS_VISITABLE_STRUCT(OEmfPlusRegionNode, rect, path, childNodes);
GS_VISITABLE_STRUCT(OEmfPlusRegionNodeChildNodes, Left, Right);
GS_VISITABLE_STRUCT(OEmfPlusDashedLineData, DashedLineDataSize, DashedLineData);
GS_VISITABLE_STRUCT(OEmfPlusCompoundLineData, CompoundLineDataSize, CompoundLineData);
GS_VISITABLE_STRUCT(OEmfPlusFillPath, FillPathLength, FillPath);
GS_VISITABLE_STRUCT(OEmfPlusLinePath, LinePathLength, LinePath);
GS_VISITABLE_STRUCT(OEmfPlusCustomLineCapData::OEmfPlusCustomLineCapOptionalData, 
	FillData, OutlineData);
GS_VISITABLE_STRUCT(OEmfPlusCustomLineCapData, CustomLineCapDataFlags, BaseCap, 
	BaseInset, StrokeStartCap, StrokeEndCap, StrokeJoin, StrokeMiterLimit, WidthScale, FillHotSpot, StrokeHotSpot);
GS_VISITABLE_STRUCT(OEmfPlusCustomLineCapArrowData, Width, Height, MiddleInset, FillState, 
	LineStartCap, LineEndCap, LineJoin, LineMiterLimit, WidthScale, FillHotSpot, LineHotSpot);
GS_VISITABLE_STRUCT(OEmfPlusCustomLineCap, Type, CustomLineCapData, CustomLineCapDataArrow);
GS_VISITABLE_STRUCT(OEmfPlusCustomStartCapData, CustomStartCapSize, CustomStartCap);
GS_VISITABLE_STRUCT(OEmfPlusCustomEndCapData, CustomEndCapSize, CustomEndCap);
GS_VISITABLE_STRUCT(OEmfPlusPenData::OEmfPlusPenOptionalData, TransformMatrix, StartCap, 
	EndCap, Join, MiterLimit, LineStyle, DashedLineCapType, DashOffset, DashedLineData, 
	PenAlignment, CompoundLineData, CustomStartCapData, CustomEndCapData);
GS_VISITABLE_STRUCT(OEmfPlusPenData, PenDataFlags, PenUnit, PenWidth, OptionalData);
GS_VISITABLE_STRUCT(OEmfPlusMetafile, Type, MetafileDataSize, MetafileData);
GS_VISITABLE_STRUCT(OEmfPlusPalette, PaletteStyleFlags, PaletteCount, PaletteEntries);
GS_VISITABLE_STRUCT(OEmfPlusBitmapData, Colors, PixelData);
GS_VISITABLE_STRUCT(OEmfPlusCompressedImage, CompressedImageData);
GS_VISITABLE_STRUCT(OEmfPlusBitmap, Width, Height, Stride, PixelFormat, Type, BitmapData, BitmapDataCompressed);
GS_VISITABLE_STRUCT(OEmfPlusImage, Type, ImageDataBmp, ImageDataMetafile);
GS_VISITABLE_STRUCT(OEmfPlusImageAttributes, Reserved1, WrapMode, ClampColor, ObjectClamp, Reserved2);
GS_VISITABLE_STRUCT(OEmfPlusHatchBrushData, HatchStyle, ForeColor, BackColor);
GS_VISITABLE_STRUCT(OEmfPlusBlendColors, PositionCount, BlendPositions, BlendColors);
GS_VISITABLE_STRUCT(OEmfPlusBlendFactors, PositionCount, BlendPositions, BlendFactors);
GS_VISITABLE_STRUCT(OEmfPlusLinearGradientBrushOptionalData::BlendPatternData, colors, factorsH, factorsV);
GS_VISITABLE_STRUCT(OEmfPlusLinearGradientBrushOptionalData, TransformMatrix, BlendPattern);
GS_VISITABLE_STRUCT(OEmfPlusLinearGradientBrushData, BrushDataFlags, WrapMode, RectF, StartColor, EndColor, Reserved1, Reserved2);
GS_VISITABLE_STRUCT(OEmfPlusFocusScaleData, FocusScaleCount, FocusScaleX, FocusScaleY);
GS_VISITABLE_STRUCT(OEmfPlusBoundaryPathData, BoundaryPathSize, BoundaryPathData);
GS_VISITABLE_STRUCT(OEmfPlusBoundaryPointData, BoundaryPointCount, BoundaryPointData);
GS_VISITABLE_STRUCT(OEmfPlusPathGradientBrushOptionalData::BlendPatternData, colors, factors);
GS_VISITABLE_STRUCT(OEmfPlusPathGradientBrushOptionalData, TransformMatrix, BlendPattern, FocusScaleData);
GS_VISITABLE_STRUCT(OEmfPlusPathGradientBrushData, BrushDataFlags, WrapMode, CenterColor, CenterPointF,
	SurroundingColorCount, SurroundingColor, BoundaryDataPath, BoundaryDataPoint, OptionalData);
GS_VISITABLE_STRUCT(OEmfPlusSolidBrushData, SolidColor);
GS_VISITABLE_STRUCT(OEmfPlusTextureBrushData::OEmfPlusTextureBrushOptionalData, TransformMatrix, ImageObject);
GS_VISITABLE_STRUCT(OEmfPlusTextureBrushData, BrushDataFlags, WrapMode, OptionalData);
GS_VISITABLE_STRUCT(OEmfPlusBrush, Type, BrushDataHatch, BrushDataLinearGrad, BrushDataPathGrad, BrushDataSolid, BrushDataTexture);
GS_VISITABLE_STRUCT(OEmfPlusPen, PenData, BrushObject);
GS_VISITABLE_STRUCT(OEmfPlusRegion, RegionNode);
GS_VISITABLE_STRUCT(OEmfPlusFont, EmSize, SizeUnit, FontStyleFlags, Reserved, Length, FamilyName);
GS_VISITABLE_STRUCT(OEmfPlusStringFormat::OEmfPlusStringFormatData, TabStops, CharRange);
GS_VISITABLE_STRUCT(OEmfPlusStringFormat, StringFormatFlags, Language, StringAlignment, LineAlign,
	DigitSubstitution, DigitLanguage, FirstTabOffset, HotkeyPrefix, LeadingMargin, TrailingMargin,
	Tracking, Trimming, TabStopCount, RangeCount, StringFormatData);
GS_VISITABLE_STRUCT(OEmfPlusArcData, StartAngle, SweepAngle, RectData);

#endif // EMF_STRUCT_VISIT_H