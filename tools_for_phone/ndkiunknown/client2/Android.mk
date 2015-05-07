LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	client.cpp

LOCAL_SHARED_LIBRARIES := libutils liblog libbinder

LOCAL_CFLAGS:= -DLOG_TAG=\"nuClient2\"

LOCAL_MODULE:= nuClient2

include $(BUILD_EXECUTABLE)

