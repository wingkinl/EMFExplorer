#ifndef EMF_STRUCT_VISIT_H
#define EMF_STRUCT_VISIT_H

#ifndef GS_VISITABLE_STRUCT
	#define GS_VISITABLE_STRUCT(emfplus::...)
#endif

GS_VISITABLE_STRUCT(POINT, x, y);
GS_VISITABLE_STRUCT(POINTL, x, y);
GS_VISITABLE_STRUCT(POINTS, x, y);
GS_VISITABLE_STRUCT(RECTL, left, top, right, bottom);
GS_VISITABLE_STRUCT(SIZEL, cx, cy);
GS_VISITABLE_STRUCT(XFORM, eM11, eM12, eM21, eM22, eDx, eDy);
GS_VISITABLE_STRUCT(PANOSE, bFamilyType,bSerifStyle,bWeight,bProportion,bContrast,
	bStrokeVariation,bArmStyle,bLetterform,bMidline,bXHeight);
GS_VISITABLE_STRUCT(COLORADJUSTMENT, caSize,caFlags,caIlluminantIndex,caRedGamma,caGreenGamma,
	caBlueGamma,caReferenceBlack,caReferenceWhite,caContrast,caBrightness,caColorfulness,caRedGreenTint);
GS_VISITABLE_STRUCT(CIEXYZ, ciexyzX, ciexyzY, ciexyzZ);
GS_VISITABLE_STRUCT(CIEXYZTRIPLE, ciexyzRed, ciexyzGreen, ciexyzBlue);

GS_VISITABLE_STRUCT(LOGBRUSH32, lbStyle, lbColor, lbHatch);
GS_VISITABLE_STRUCT(LOGPEN, lopnStyle, lopnWidth, lopnColor);
GS_VISITABLE_STRUCT(LOGFONTW, lfHeight,lfWidth,lfEscapement,lfOrientation,lfWeight,lfItalic,
	lfUnderline,lfStrikeOut,lfCharSet,lfOutPrecision,lfClipPrecision,lfQuality,lfPitchAndFamily,lfFaceName);
GS_VISITABLE_STRUCT(EXTLOGPEN32, elpPenStyle, elpWidth, elpBrushStyle, elpColor, elpHatch, elpNumEntries);
GS_VISITABLE_STRUCT(EXTLOGFONTW, elfLogFont,elfFullName,elfStyle,elfVersion,elfStyleSize,elfMatch,
	elfReserved,elfVendorId,elfCulture,elfPanose);
GS_VISITABLE_STRUCT(LOGPALETTE, palVersion, palNumEntries, palPalEntry);
GS_VISITABLE_STRUCT(LOGCOLORSPACEA, lcsSignature,lcsVersion,lcsSize,lcsCSType,lcsIntent,
	lcsEndpoints,lcsGammaRed,lcsGammaGreen,lcsGammaBlue,lcsFilename);
GS_VISITABLE_STRUCT(LOGCOLORSPACEW, lcsSignature,lcsVersion,lcsSize,lcsCSType,lcsIntent,
	lcsEndpoints,lcsGammaRed,lcsGammaGreen,lcsGammaBlue,lcsFilename);
GS_VISITABLE_STRUCT(EMRTEXT, ptlReference, nChars, offString, fOptions, rcl, offDx);
GS_VISITABLE_STRUCT(PIXELFORMATDESCRIPTOR, nSize,nVersion,dwFlags,iPixelType,cColorBits,cRedBits,cRedShift,
	cGreenBits,cGreenShift,cBlueBits,cBlueShift,cAlphaBits,cAlphaShift,cAccumBits,cAccumRedBits,cAccumGreenBits,
	cAccumBlueBits,cAccumAlphaBits,cDepthBits,cStencilBits,cAuxBuffers,iLayerType,bReserved,dwLayerMask,dwVisibleMask,
	dwDamageMask);
GS_VISITABLE_STRUCT(BITMAPINFOHEADER, biSize, biWidth, biHeight, biPlanes, biBitCount, biCompression, 
	biSizeImage, biXPelsPerMeter, biYPelsPerMeter, biClrUsed, biClrImportant);

GS_VISITABLE_STRUCT(EMREXCLUDECLIPRECT, rclClip);
GS_VISITABLE_STRUCT(EMREXTSELECTCLIPRGN, cbRgnData,iMode,RgnData);
GS_VISITABLE_STRUCT(EMROFFSETCLIPRGN, ptlOffset);
GS_VISITABLE_STRUCT(EMRSELECTCLIPPATH, iMode);

GS_VISITABLE_STRUCT(EMRRESTOREDC, iRelative);

GS_VISITABLE_STRUCT(EMRCOLORCORRECTPALETTE, ihPalette, nFirstEntry, nPalEntries, nReserved);
GS_VISITABLE_STRUCT(EMRDELETECOLORSPACE, ihCS);
GS_VISITABLE_STRUCT(EMRDELETEOBJECT, ihObject);
GS_VISITABLE_STRUCT(EMRRESIZEPALETTE, ihPal, cEntries);
GS_VISITABLE_STRUCT(EMRSELECTPALETTE, ihPal);
//GS_VISITABLE_STRUCT(EMRSETPALETTEENTRIES, ihPal, iStart, cEntries, aPalEntries);

GS_VISITABLE_STRUCT(EMRCOLORMATCHTOTARGET, dwAction, dwFlags, cbName, cbData, Data);
GS_VISITABLE_STRUCT(EMRINVERTRGN, rclBounds, cbRgnData, RgnData);
GS_VISITABLE_STRUCT(EMRMOVETOEX, ptl);
GS_VISITABLE_STRUCT(EMRPIXELFORMAT, pfd);
GS_VISITABLE_STRUCT(EMRSCALEVIEWPORTEXTEX, xNum,xDenom,yNum,yDenom);
GS_VISITABLE_STRUCT(EMRSETARCDIRECTION, iArcDirection);
GS_VISITABLE_STRUCT(EMRSETBKCOLOR, crColor);
GS_VISITABLE_STRUCT(EMRSETBRUSHORGEX, ptlOrigin);
GS_VISITABLE_STRUCT(EMRSETCOLORADJUSTMENT, ColorAdjustment);
GS_VISITABLE_STRUCT(EMRSETICMPROFILEA, dwFlags, cbName, cbData, Data);
GS_VISITABLE_STRUCT(EMRSETMAPPERFLAGS, dwFlags);
GS_VISITABLE_STRUCT(EMRSETMITERLIMIT, eMiterLimit);
GS_VISITABLE_STRUCT(EMRSETVIEWPORTEXTEX, szlExtent);

GS_VISITABLE_STRUCT(EMRMODIFYWORLDTRANSFORM, xform, iMode);
GS_VISITABLE_STRUCT(EMRSETWORLDTRANSFORM, xform);

GS_VISITABLE_STRUCT(EMRALPHABLEND, rclBounds,xDest,yDest,cxDest,cyDest,dwRop,
	xSrc,ySrc,xformSrc,crBkColorSrc,iUsageSrc,offBmiSrc,cbBmiSrc,offBitsSrc,cbBitsSrc,cxSrc,cySrc);
GS_VISITABLE_STRUCT(EMRBITBLT, rclBounds, xDest, yDest, cxDest, cyDest, dwRop,
	xSrc,ySrc,xformSrc,crBkColorSrc,iUsageSrc,offBmiSrc,cbBmiSrc,offBitsSrc,cbBitsSrc);
GS_VISITABLE_STRUCT(EMRMASKBLT, rclBounds, xDest, yDest, cxDest, cyDest, dwRop,
	xSrc,ySrc,xformSrc,crBkColorSrc,iUsageSrc,offBmiSrc,cbBmiSrc,offBitsSrc,cbBitsSrc,
	xMask,yMask,iUsageMask,offBmiMask,cbBmiMask,offBitsMask,cbBitsMask);
GS_VISITABLE_STRUCT(EMRPLGBLT, rclBounds, aptlDest, xSrc, ySrc, cxSrc, cySrc, 
	xformSrc,crBkColorSrc,iUsageSrc,offBmiSrc,cbBmiSrc,offBitsSrc,cbBitsSrc,
	xMask,yMask,iUsageMask,offBmiMask,cbBmiMask,offBitsMask,cbBitsMask);
GS_VISITABLE_STRUCT(EMRSTRETCHBLT, rclBounds, xDest, yDest, cxDest,cyDest,
	dwRop,xSrc,ySrc,xformSrc,crBkColorSrc,offBmiSrc,cbBmiSrc,offBitsSrc,cbBitsSrc,cxSrc,cySrc);
GS_VISITABLE_STRUCT(EMRSTRETCHDIBITS, rclBounds, xDest, yDest, 
	xSrc,ySrc,cxSrc,cySrc,offBmiSrc,cbBmiSrc,offBitsSrc,cbBitsSrc,iUsageSrc,dwRop,cxDest,cyDest);
GS_VISITABLE_STRUCT(EMRSETDIBITSTODEVICE, rclBounds, xDest, yDest, 
	xSrc,ySrc,cxSrc,cySrc,offBmiSrc,cbBmiSrc,offBitsSrc,cbBitsSrc,iUsageSrc,iStartScan,cScans);
GS_VISITABLE_STRUCT(EMRTRANSPARENTBLT,rclBounds, xDest, yDest, cxDest,cyDest,dwRop,
	xSrc,ySrc,xformSrc,crBkColorSrc,iUsageSrc,offBmiSrc,cbBmiSrc,offBitsSrc,cbBitsSrc,cxSrc,cySrc);

GS_VISITABLE_STRUCT(EMRANGLEARC, ptlCenter,nRadius,eStartAngle,eSweepAngle);
GS_VISITABLE_STRUCT(EMRARC, rclBox,ptlStart,ptlEnd);
GS_VISITABLE_STRUCT(EMRELLIPSE, rclBox);
GS_VISITABLE_STRUCT(EMREXTFLOODFILL, ptlStart, crColor, iMode);
GS_VISITABLE_STRUCT(EMREXTTEXTOUTA, rclBounds, iGraphicsMode, exScale, eyScale, emrtext);
GS_VISITABLE_STRUCT(EMRFILLPATH, rclBounds);
GS_VISITABLE_STRUCT(EMRFILLRGN, rclBounds,cbRgnData, ihBrush, RgnData);
GS_VISITABLE_STRUCT(EMRFRAMERGN, rclBounds,cbRgnData, ihBrush, szlStroke, RgnData);
GS_VISITABLE_STRUCT(EMRGRADIENTFILL, rclBounds,nVer,nTri,ulMode,Ver);
//GS_VISITABLE_STRUCT(EMRPAINTRGN, rclBounds,cbRgnData, RgnData);
//GS_VISITABLE_STRUCT(EMRPOLYBEZIER, rclBounds,cptl, aptl);
//GS_VISITABLE_STRUCT(EMRPOLYBEZIER16, rclBounds,cpts, apts);
//GS_VISITABLE_STRUCT(EMRPOLYDRAW, rclBounds,cptl, aptl, abTypes);
//GS_VISITABLE_STRUCT(EMRPOLYDRAW16, rclBounds,cpts, apts, abTypes);
//GS_VISITABLE_STRUCT(EMRPOLYPOLYGON, rclBounds,nPolys, cptl,aPolyCounts, aptl);
//GS_VISITABLE_STRUCT(EMRPOLYPOLYGON16, rclBounds,nPolys, cpts,aPolyCounts, apts);
GS_VISITABLE_STRUCT(EMRPOLYTEXTOUTA, rclBounds,iGraphicsMode, exScale,eyScale, cStrings, aemrtext);
GS_VISITABLE_STRUCT(EMRROUNDRECT, rclBox,szlCorner);
GS_VISITABLE_STRUCT(EMRSETPIXELV, ptlPixel,crColor);


GS_VISITABLE_STRUCT(EMRCREATEBRUSHINDIRECT, ihBrush, lb);
GS_VISITABLE_STRUCT(EMRCREATECOLORSPACE, ihCS, lcs);
GS_VISITABLE_STRUCT(EMRCREATECOLORSPACEW, ihCS, lcs, dwFlags, cbData, Data);
GS_VISITABLE_STRUCT(EMRCREATEDIBPATTERNBRUSHPT, ihBrush, iUsage, offBmi, cbBmi, offBits, cbBits);
GS_VISITABLE_STRUCT(EMRCREATEMONOBRUSH, ihBrush, iUsage, offBmi, cbBmi, offBits, cbBits);
GS_VISITABLE_STRUCT(EMRCREATEPALETTE, ihPal, lgpl);
GS_VISITABLE_STRUCT(EMRCREATEPEN, ihPen, lopn);
GS_VISITABLE_STRUCT(EMREXTCREATEFONTINDIRECTW, ihFont, elfw);
GS_VISITABLE_STRUCT(EMREXTCREATEPEN, ihPen, offBmi, cbBmi, offBits, cbBits, elp);

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

GS_VISITABLE_STRUCT(emfgdi::OEmfSetPaletteEntries, ihPal, iStart, PalEntries);
GS_VISITABLE_STRUCT(emfgdi::OEmfPolyline, rclBounds, aptl);
GS_VISITABLE_STRUCT(emfgdi::OEmfPolyline16, rclBounds, apts);
GS_VISITABLE_STRUCT(emfgdi::OEmfPolyPolyline, rclBounds, aPolyCounts, aptl);
GS_VISITABLE_STRUCT(emfgdi::OEmfPolyPolyline16, rclBounds, aPolyCounts, apts);
GS_VISITABLE_STRUCT(emfgdi::OEmfPolyDraw, rclBounds, aptl, abTypes);
GS_VISITABLE_STRUCT(emfgdi::OEmfPolyDraw16, rclBounds, apts, abTypes);

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