LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	service.cpp

LOCAL_SHARED_LIBRARIES := libutils liblog libbinder libMyService

LOCAL_CFLAGS:= -DLOG_TAG=\"myservice\"

LOCAL_MODULE:= myservice

include $(BUILD_EXECUTABLE)

