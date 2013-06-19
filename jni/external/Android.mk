MY_DIR := $(call my-dir)

## freetype2
CUSTOMZIE_PATH := $(MY_DIR)/customize/freetype2
LOCAL_PATH := $(MY_DIR)/freetype2

include $(CLEAR_VARS)

LOCAL_SRC_FILES := 		\
	src/base/ftbbox.c 	\
	src/base/ftbitmap.c 	\
	src/base/ftglyph.c 	\
	src/base/ftstroke.c 	\
	src/base/ftxf86.c 	\
	src/base/ftbase.c 	\
	src/base/ftsystem.c 	\
	src/base/ftinit.c 	\
	src/base/ftgasp.c 	\
	src/raster/raster.c 	\
	src/sfnt/sfnt.c 	\
	src/smooth/smooth.c 	\
	src/autofit/autofit.c 	\
	src/truetype/truetype.c \
	src/cff/cff.c 		\
	src/psnames/psnames.c 	\
	src/pshinter/pshinter.c \
	src/type1/type1.c 	\
	src/cid/type1cid.c 	\
	src/type42/type42.c 	\
	src/winfonts/winfnt.c 	\
	src/pcf/pcf.c 		\
	src/bdf/bdf.c 		\
	src/lzw/ftlzw.c 	\
	src/gzip/ftgzip.c 	\
	src/psaux/psaux.c 	\
	src/pfr/pfr.c

LOCAL_C_INCLUDES += 		\
	$(CUSTOMIZE_PATH)	\
	$(LOCAL_PATH)/builds 	\
	$(LOCAL_PATH)/include

LOCAL_CFLAGS += -W -Wall
LOCAL_CFLAGS += -fPIC -DPIC
LOCAL_CFLAGS += "-DDARWIN_NO_CARBON"
LOCAL_CFLAGS += "-DFT2_BUILD_LIBRARY"

LOCAL_CFLAGS += -O2

LOCAL_MODULE := libft2

include $(BUILD_STATIC_LIBRARY)

## poppler
LOCAL_PATH := $(MY_DIR)/poppler

CUSTOMZIE_PATH := $(MY_DIR)/customize/poppler

include $(CLEAR_VARS)

LOCAL_MODULE := libpoppler

LOCAL_CPP_EXTENSION := .cc

LOCAL_SRC_FILES :=      \
	fofi/FoFiBase.cc		\
	fofi/FoFiEncodings.cc		\
	fofi/FoFiTrueType.cc		\
	fofi/FoFiType1.cc		\
	fofi/FoFiType1C.cc		\
	goo/gfile.cc			\
	goo/gmempp.cc			\
	goo/GooHash.cc			\
	goo/GooList.cc			\
	goo/GooTimer.cc			\
	goo/GooString.cc		\
	goo/gmem.cc			\
	goo/FixedPoint.cc		\
	goo/gstrtod.cc			\
	poppler/CachedFile.cc		\
	poppler/GlobalParams.cc		\
	poppler/FlateStream.cc		\
	poppler/Annot.cc		\
	poppler/Array.cc 		\
	poppler/BuiltinFont.cc		\
	poppler/BuiltinFontTables.cc	\
	poppler/Catalog.cc 		\
	poppler/CharCodeToUnicode.cc	\
	poppler/CMap.cc			\
	poppler/DateInfo.cc		\
	poppler/Decrypt.cc		\
	poppler/Dict.cc 		\
	poppler/FileSpec.cc		\
	poppler/FontEncodingTables.cc	\
	poppler/Form.cc 		\
	poppler/FontInfo.cc		\
	poppler/Function.cc		\
	poppler/Gfx.cc 			\
	poppler/Hints.cc		\
	poppler/Linearization.cc	\
	poppler/ViewerPreferences.cc	\
	poppler/GfxFont.cc 		\
	poppler/GfxState.cc		\
	poppler/JArithmeticDecoder.cc	\
	poppler/JBIG2Stream.cc		\
	poppler/JPXStream.cc		\
	poppler/Lexer.cc 		\
	poppler/Link.cc 		\
	poppler/Movie.cc                \
	poppler/NameToCharCode.cc	\
	poppler/Rendition.cc		\
	poppler/Object.cc 		\
	poppler/OptionalContent.cc	\
	poppler/Outline.cc		\
	poppler/OutputDev.cc 		\
	poppler/Page.cc 		\
	poppler/PageTransition.cc	\
	poppler/Parser.cc 		\
	poppler/PDFDoc.cc 		\
	poppler/PDFDocEncoding.cc	\
	poppler/PopplerCache.cc		\
	poppler/ProfileData.cc		\
	poppler/PSTokenizer.cc		\
	poppler/Stream.cc 		\
	poppler/UnicodeMap.cc		\
	poppler/UnicodeTypeTable.cc	\
	poppler/XRef.cc			\
	poppler/PageLabelInfo.cc	\
	poppler/SecurityHandler.cc	\
	poppler/Sound.cc		\
	poppler/Error.cc		\
	poppler/SplashOutputDev.cc	\
	splash/Splash.cc		\
	splash/SplashBitmap.cc		\
	splash/SplashClip.cc		\
	splash/SplashFTFont.cc		\
	splash/SplashFTFontEngine.cc	\
	splash/SplashFTFontFile.cc	\
	splash/SplashFont.cc		\
	splash/SplashFontEngine.cc	\
	splash/SplashFontFile.cc	\
	splash/SplashFontFileID.cc	\
	splash/SplashPath.cc		\
	splash/SplashPattern.cc		\
	splash/SplashScreen.cc		\
	splash/SplashState.cc		\
	splash/SplashXPath.cc		\
	splash/SplashXPathScanner.cc	\
	../customize/poppler/GlobalParamsAndroid.cc	\
	../customize/poppler/customize.cc

LOCAL_C_INCLUDES :=         	\
	$(CUSTOMZIE_PATH)	\
	$(CUSTOMZIE_PATH)/poppler	\
	$(LOCAL_PATH)/		\
	$(LOCAL_PATH)/poppler	\
	$(LOCAL_PATH)/goo	\
	$(EXTERNAL_PATH)/freetype2/include

LOCAL_CFLAGS += -w -DPLATFORM_ANDROID

include $(BUILD_STATIC_LIBRARY)

