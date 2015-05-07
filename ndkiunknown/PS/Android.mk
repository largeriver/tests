LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SRC_FILES:= \
	MyService.cpp

#LOCAL_C_INCLUDES := base/cmd/ndkiunknown/include

LOCAL_SHARED_LIBRARIES := \
    libutils liblog libbinder libcutils

LOCAL_CFLAGS:=  -DLOG_TAG=\"libnuMyServicePS\"
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := libnuMyServicePS

include $(BUILD_SHARED_LIBRARY)


