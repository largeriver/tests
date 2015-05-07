LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SRC_FILES:= \
	ChatSession.cpp

#LOCAL_C_INCLUDES := 	$(JNI_H_INCLUDE)

LOCAL_SHARED_LIBRARIES := \
    libutils liblog libbinder libcutils libcomchatPS

LOCAL_CFLAGS:= -DLOG_TAG=\"ccSer\"
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := libcomchatService



include $(BUILD_SHARED_LIBRARY)


