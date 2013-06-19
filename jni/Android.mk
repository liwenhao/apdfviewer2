MY_LOCAL_PATH := $(call my-dir)

EXTERNAL_PATH := $(MY_LOCAL_PATH)/external

include $(call all-subdir-makefiles)

LOCAL_PATH := $(MY_LOCAL_PATH)

include $(CLEAR_VARS)

LOCAL_MODULE    := PDFDocument_jni
LOCAL_SRC_FILES := PDFDocument.cpp

LOCAL_C_INCLUDES :=         		\
	$(EXTERNAL_PATH)/customize/poppler \
	$(EXTERNAL_PATH)/customize/poppler/poppler \
	$(EXTERNAL_PATH)/poppler	\
	$(EXTERNAL_PATH)/poppler/poppler

LOCAL_CFLAGS += -DPLATFORM_ANDROID
LOCAL_LDLIBS := -llog -lz -lGLESv1_CM

LOCAL_STATIC_LIBRARIES := libpoppler libft2

include $(BUILD_SHARED_LIBRARY)
