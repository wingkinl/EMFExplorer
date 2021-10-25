#ifndef EMF_STRUCT_VISIT_H
#define EMF_STRUCT_VISIT_H

#ifndef GS_VISITABLE_STRUCT
	#define GS_VISITABLE_STRUCT(emfplus::...)
#endif

GS_VISITABLE_STRUCT(POINT, x, y);
GS_VISITABLE_STRUCT(LOGBRUSH32, lbStyle, lbColor, lbHatch);
GS_VISITABLE_STRUCT(LOGPEN, lopnStyle, lopnWidth, lopnColor);
GS_VISITABLE_STRUCT(EXTLOGPEN32, elpPenStyle, elpWidth, elpBrushStyle, elpColor, elpHatch, elpNumEntries);


GS_VISITABLE_STRUCT(EMRCREATEBRUSHINDIRECT, ihBrush, lb);
GS_VISITABLE_STRUCT(EMRCREATEPEN, ihPen, lopn);
GS_VISITABLE_STRUCT(EMREXTCREATEPEN, ihPen, offBmi, cbBmi, offBits, cbBits, elp);

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

GS_VISITABLE_STRUCT(emfplus::OEmfPlusPoint, x, y);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusPointF, x, y);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusPath, PathPointCount, PathPointFlags, PathPoints, 
	PathPointTypes, PathPointTypesRLE);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusRegionNodePath, RegionNodePath);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusRect, X, Y, Width, Height);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusRectF, X, Y, Width, Height);
GS_VISITABLE_STRUCT(emfplus::ORectL, Left, Top, Right, Bottom);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusRegionNode, Type, rect, path, childNodes);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusRegionNodeChildNodes, Left, Right);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusDashedLineData, DashedLineDataSize, DashedLineData);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusCompoundLineData, CompoundLineDataSize, CompoundLineData);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusFillPath, FillPathLength, FillPath);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusLinePath, LinePathLength, LinePath);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusCustomLineCapData::OEmfPlusCustomLineCapOptionalData, 
	FillData, OutlineData);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusCustomLineCapData, CustomLineCapDataFlags, BaseCap, 
	BaseInset, StrokeStartCap, StrokeEndCap, StrokeJoin, StrokeMiterLimit, WidthScale, FillHotSpot, StrokeHotSpot);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusCustomLineCapArrowData, Width, Height, MiddleInset, FillState, 
	LineStartCap, LineEndCap, LineJoin, LineMiterLimit, WidthScale, FillHotSpot, LineHotSpot);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusCustomLineCap, Type, CustomLineCapData, CustomLineCapDataArrow);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusCustomStartCapData, CustomStartCapSize, CustomStartCap);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusCustomEndCapData, CustomEndCapSize, CustomEndCap);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusPenData::OEmfPlusPenOptionalData, TransformMatrix, StartCap, 
	EndCap, Join, MiterLimit, LineStyle, DashedLineCapType, DashOffset, DashedLineData, 
	PenAlignment, CompoundLineData, CustomStartCapData, CustomEndCapData);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusPenData, PenDataFlags, PenUnit, PenWidth, OptionalData);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusMetafile, Type, MetafileDataSize, MetafileData);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusPalette, PaletteStyleFlags, PaletteCount, PaletteEntries);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusBitmapData, Colors, PixelData);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusCompressedImage, CompressedImageData);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusBitmap, Width, Height, Stride, PixelFormat, Type, BitmapData, BitmapDataCompressed);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusImage, Type, ImageDataBmp, ImageDataMetafile);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusImageAttributes, Reserved1, WrapMode, ClampColor, ObjectClamp, Reserved2);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusHatchBrushData, HatchStyle, ForeColor, BackColor);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusBlendColors, PositionCount, BlendPositions, BlendColors);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusBlendFactors, PositionCount, BlendPositions, BlendFactors);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusLinearGradientBrushOptionalData::BlendPatternData, colors, factorsH, factorsV);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusLinearGradientBrushOptionalData, TransformMatrix, BlendPattern);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusLinearGradientBrushData, BrushDataFlags, WrapMode, RectF, StartColor, EndColor, Reserved1, Reserved2);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusFocusScaleData, FocusScaleCount, FocusScaleX, FocusScaleY);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusBoundaryPathData, BoundaryPathSize, BoundaryPathData);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusBoundaryPointData, BoundaryPointCount, BoundaryPointData);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusPathGradientBrushOptionalData::BlendPatternData, colors, factors);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusPathGradientBrushOptionalData, TransformMatrix, BlendPattern, FocusScaleData);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusPathGradientBrushData, BrushDataFlags, WrapMode, CenterColor, CenterPointF,
	SurroundingColorCount, SurroundingColor, BoundaryDataPath, BoundaryDataPoint, OptionalData);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusSolidBrushData, SolidColor);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusTextureBrushData::OEmfPlusTextureBrushOptionalData, TransformMatrix, ImageObject);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusTextureBrushData, BrushDataFlags, WrapMode, OptionalData);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusBrush, Type, BrushDataHatch, BrushDataLinearGrad, BrushDataPathGrad, BrushDataSolid, BrushDataTexture);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusPen, PenData, BrushObject);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusRegion, RegionNode);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusFont, EmSize, SizeUnit, FontStyleFlags, Reserved, Length, FamilyName);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusStringFormat::OEmfPlusStringFormatData, TabStops, CharRange);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusStringFormat, StringFormatFlags, Language, StringAlignment, LineAlign,
	DigitSubstitution, DigitLanguage, FirstTabOffset, HotkeyPrefix, LeadingMargin, TrailingMargin,
	Tracking, Trimming, TabStopCount, RangeCount, StringFormatData);
GS_VISITABLE_STRUCT(emfplus::OEmfPlusArcData, StartAngle, SweepAngle, RectData);

#endif // EMF_STRUCT_VISIT_H