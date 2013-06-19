/*
 * Copyright (C) 2009 Li Wenhao <liwenhao.g@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>

#include <jni.h>

#include <PDFDoc.h>
#include <SplashOutputDev.h>
#include <GlobalParams.h>
#include <splash/SplashBitmap.h>

#define LOG_TAG "PDFDocument"
#include <android/log.h>
#define LOGV(fmt, ...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, fmt, __VA_ARGS__)

#include <GLES/gl.h>

struct fields_t {
	jfieldID nativePDF;
	jfieldID useMediaBox;
	jfieldID crop;
	jfieldID hDPI;
	jfieldID vDPI;
	jfieldID rotate;
	jfieldID fd;
};
static fields_t fields;

struct NativePDF {
	PDFDoc *doc;
	SplashOutputDev *out;
};

//
// helper function to throw an exception
//
static void throwException(JNIEnv *env, const char* ex, const char* fmt,
		int data) {
	if (jclass cls = env->FindClass(ex)) {
		char msg[1000];
		sprintf(msg, fmt, data);
		env->ThrowNew(cls, msg);
		env->DeleteLocalRef(cls);
	}
}

static double gethDPI(JNIEnv* env, jobject clazz) {
	return env->GetDoubleField(clazz, fields.hDPI);
}

static double getvDPI(JNIEnv* env, jobject clazz) {
	return env->GetDoubleField(clazz, fields.vDPI);
}

static int getRotate(JNIEnv* env, jobject clazz) {
	return env->GetIntField(clazz, fields.rotate);
}

static jboolean getUseMediaBox(JNIEnv* env, jobject clazz) {
	return env->GetBooleanField(clazz, fields.useMediaBox);
}

static jboolean getCrop(JNIEnv* env, jobject clazz) {
	return env->GetBooleanField(clazz, fields.crop);
}

static NativePDF* getNativePDF(JNIEnv* env, jobject clazz) {
	return (NativePDF*) env->GetIntField(clazz, fields.nativePDF);
}

static PDFDoc* getDoc(JNIEnv* env, jobject clazz) {
	NativePDF *nativePDF = (NativePDF*) env->GetIntField(clazz,
			fields.nativePDF);
	return nativePDF->doc;
}

/*
 * Class:     com_android_pdfviewer_PDFDocument
 * Method:    getPageMediaWidth
 * Signature: (I)D
 */
static jdouble getPageMediaWidth(JNIEnv *env, jobject clazz, jint page) {
	PDFDoc *doc = getDoc(env, clazz);
	return doc->getPageMediaWidth(page);
}

/*
 * Class:     com_android_pdfviewer_PDFDocument
 * Method:    getPageMediaHeight
 * Signature: (I)D
 */
static jdouble getPageMediaHeight(JNIEnv *env, jobject clazz, jint page) {
	PDFDoc *doc = getDoc(env, clazz);
	return doc->getPageMediaHeight(page);
}

/*
 * Class:     com_android_pdfviewer_PDFDocument
 * Method:    getPageCropWidth
 * Signature: (I)D
 */
static jdouble getPageCropWidth(JNIEnv *env, jobject clazz, jint page) {
	PDFDoc *doc = getDoc(env, clazz);
	return doc->getPageCropWidth(page);
}

/*
 * Class:     com_android_pdfviewer_PDFDocument
 * Method:    getPageCropHeight
 * Signature: (I)D
 */
static jdouble getPageCropHeight(JNIEnv *env, jobject clazz, jint page) {
	PDFDoc *doc = getDoc(env, clazz);
	return doc->getPageCropHeight(page);
}

/*
 * Class:     com_android_pdfviewer_PDFDocument
 * Method:    getPageRotate
 * Signature: (I)I
 */
static jint getPageRotate(JNIEnv *env, jobject clazz, jint page) {
	PDFDoc *doc = getDoc(env, clazz);
	return doc->getPageRotate(page);
}

/*
 * Class:     com_android_pdfviewer_PDFDocument
 * Method:    isOk
 * Signature: ()Z
 */
static jboolean isOk(JNIEnv *env, jobject clazz) {
	PDFDoc *doc = getDoc(env, clazz);
	return doc->isOk();
}

/*
 * Class:     com_android_pdfviewer_PDFDocument
 * Method:    getNumPages
 * Signature: ()I
 */
static jint getNumPages(JNIEnv *env, jobject clazz) {
	PDFDoc *doc = getDoc(env, clazz);
	return doc->getNumPages();
}

/*
 * Class:     com_android_pdfviewer_PDFDocument
 * Method:    native_getImageWidth
 * Signature: ()I
 */
static jint native_getImageWidth(JNIEnv *env, jobject clazz) {
	NativePDF *nativePDF = getNativePDF(env, clazz);

	return nativePDF->out->getBitmapWidth();
}

/*
 * Class:     com_android_pdfviewer_PDFDocument
 * Method:    native_getImageHeight
 * Signature: ()I
 */
static jint native_getImageHeight(JNIEnv *env, jobject clazz) {
	NativePDF *nativePDF = getNativePDF(env, clazz);
	return nativePDF->out->getBitmapHeight();
}

static GLuint nextPot(GLuint x) {
	GLuint ret = 1;
	for (int i = 1; i < 32; i++) {
		ret <<= 1;
		if (ret > x)
			break;
	}
	return ret;
}

/*
 * Class:     com_android_pdfviewer_PDFDocument
 * Method:    native_getImageData
 * Signature: ()I
 */
static jint native_loadTexture(JNIEnv *env, jobject clazz) {
	NativePDF *nativePDF = getNativePDF(env, clazz);
	SplashBitmap *bitmap = nativePDF->out->getBitmap();

	GLuint textureName = -1;
	glGenTextures(1, &textureName);

	glBindTexture(GL_TEXTURE_2D, textureName);

	glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	GLuint width, height, potWidth, potHeight;
	width = bitmap->getWidth();
	height = bitmap->getHeight();
	GLvoid* data = 0;

	LOGV("w/h: %d %d", width, height);

	if ((width & width - 1) || (height & height - 1)) {
		width = nextPot(width);
		height = nextPot(height);
	} else {
		data = bitmap->getDataPtr();
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
			GL_UNSIGNED_BYTE, data);

	if (data == 0) {
		data = bitmap->getDataPtr();
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, bitmap->getWidth(),
				bitmap->getHeight(), GL_RGB, GL_UNSIGNED_BYTE, data);
	}

	return textureName;
}

/*
 * Class:     com_android_pdfviewer_PDFDocument
 * Method:    drawPage
 * Signature: (I)V
 */
static void drawPage(JNIEnv *env, jobject clazz, jint page) {
	NativePDF *nativePDF = getNativePDF(env, clazz);

	if (!nativePDF->doc || !nativePDF->doc->isOk())
		return;

	double hDPI = gethDPI(env, clazz);
	double vDPI = getvDPI(env, clazz);
	int rotate = getRotate(env, clazz);
	int useMediaBox = getUseMediaBox(env, clazz);
	int crop = getCrop(env, clazz);

	nativePDF->doc->displayPage(nativePDF->out, (int) page, hDPI, vDPI, 0*90,
			gFalse, gTrue, gFalse, NULL, NULL, NULL, NULL);
}

/*
 * Class:     com_android_pdfviewer_PDFDocument
 * Method:    drawPageSlice
 * Signature: (IIIII)V
 */
static void drawPageSlice(JNIEnv *env, jobject clazz, jint page, jint sliceX,
		jint sliceY, jint sliceW, jint sliceH) {
	NativePDF *nativePDF = getNativePDF(env, clazz);

	if (!nativePDF->doc || !nativePDF->doc->isOk())
		return;

	double hDPI = gethDPI(env, clazz);
	double vDPI = getvDPI(env, clazz);
	int rotate = getRotate(env, clazz);
	int useMediaBox = getUseMediaBox(env, clazz);
	int crop = getCrop(env, clazz);

	nativePDF->doc->displayPageSlice(nativePDF->out, (int) page, hDPI, vDPI,
			rotate, useMediaBox, crop, 0, sliceX, sliceY, sliceW, sliceH, NULL,
			NULL, NULL, NULL);
}

/*
 * Class:     com_android_pdfviewer_PDFDocument
 * Method:    init_native
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I
 */
static jint native_init(JNIEnv *env, jobject clazz, jobject descriptor,
		jstring ownerPassword, jstring userPassword) {
	int fd = env->GetIntField(descriptor, fields.fd);
	FILE *file = fdopen(fd, "r");
	if (!file) {
		//LOGV("Open fd failed: %d", fd);
	}

	Object obj;
	obj.initNull();
	FileStream *stream = new FileStream(file, 0, gFalse, 0, &obj);
	if (!stream) {
		//LOGV("Create stream failed: %d", fd);
	}

	NativePDF *nativePDF = new NativePDF();

	nativePDF->doc = new PDFDoc(stream);
	if (!nativePDF->doc->isOk()) {
		//LOGV("Open failed: %d", doc->getErrorCode());
	}

	SplashColor sc;
	sc[0] = 255;
	sc[1] = 255;
	sc[2] = 255;

	nativePDF->out = new SplashOutputDev(splashModeRGB8, 1, gFalse, sc);
	nativePDF->out->startDoc(nativePDF->doc->getXRef());

	return (jint) nativePDF;
}

/*
 * Class:     com_android_pdfviewer_PDFDocument
 * Method:    native_close
 * Signature: ()V
 */
static void native_close(JNIEnv *env, jobject clazz) {
	NativePDF *nativePDF = getNativePDF(env, clazz);

	if (nativePDF->doc) {
		delete nativePDF->doc;
	}
	if (nativePDF->out) {
		delete nativePDF->out;
	}

	//TODO: set native pointer to null
}

static void native_class_init(JNIEnv* env, jclass clazz) {
	fields.nativePDF = env->GetFieldID(clazz, "mNativePDF", "I");
	fields.useMediaBox = env->GetFieldID(clazz, "mUseMediaBox", "Z");
	fields.crop = env->GetFieldID(clazz, "mCrop", "Z");
	fields.hDPI = env->GetFieldID(clazz, "mH_DPI", "D");
	fields.vDPI = env->GetFieldID(clazz, "mV_DPI", "D");
	fields.rotate = env->GetFieldID(clazz, "mRotate", "I");

	jclass fd = env->FindClass("java/io/FileDescriptor");
	fields.fd = env->GetFieldID(fd, "descriptor", "I");

	globalParams = new GlobalParams();

}

// ----------------------------------------------------------------------------

static JNINativeMethod gMethods[] = {
		{"native_class_init", "()V", (void*) native_class_init},
		{"native_init", "(Ljava/io/FileDescriptor;Ljava/lang/String;Ljava/lang/String;)I", (void*) native_init},
		{"native_close", "()V", (void*) native_close},
		{"getPageMediaWidth", "(I)D", (void*) getPageMediaWidth},
		{"getPageMediaHeight", "(I)D", (void*) getPageMediaHeight},
		{"getPageCropWidth", "(I)D", (void*) getPageCropWidth},
		{"getPageCropHeight", "(I)D", (void*) getPageCropHeight},
		{"getPageRotate", "(I)I", (void*) getPageRotate},
		{"isOk", "()Z", (void*) isOk},
		{"getNumPages", "()I", (void*) getNumPages},
		{"native_draw_page", "(I)V", (void*) drawPage},
		{"native_draw_page_slice", "(IIIII)V", (void*) drawPageSlice},
		{"native_getImageWidth", "()I", (void*) native_getImageWidth},
		{"native_getImageHeight", "()I", (void*) native_getImageHeight},
		{"native_loadTexture", "()I", (void*) native_loadTexture }
};

static int registerNativeMethods(JNIEnv* env, const char* className,
		JNINativeMethod* gMethods, int numMethods) {
	jclass clazz;

	clazz = env->FindClass(className);
	if (clazz == NULL) {
		fprintf(stderr, "Native registration unable to find class '%s'\n",
				className);
		return JNI_FALSE;
	}
	if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
		fprintf(stderr, "RegisterNatives failed for '%s'\n", className);
		return JNI_FALSE;
	}

	return JNI_TRUE;
}

static int registerNatives(JNIEnv *env) {
	const char* const kClassPathName = "com/apdfviewer/PDFDocument";

	return registerNativeMethods(env, kClassPathName, gMethods,
			sizeof(gMethods) / sizeof(gMethods[0]));
}

/*
 * Returns the JNI version on success, -1 on failure.
 */
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
	JNIEnv* env = NULL;
	jint result = -1;

	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		fprintf(stderr, "ERROR: GetEnv failed\n");
		goto fail;
	}
	assert(env != NULL);

	if (!registerNatives(env)) {
		fprintf(stderr, "ERROR: PDFDocument native registration failed\n");
		goto fail;
	}

	/* success -- return valid version number */
	result = JNI_VERSION_1_4;

	fail: return result;
}
