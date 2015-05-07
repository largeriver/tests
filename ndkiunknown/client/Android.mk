LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	client.cpp
	
LOCAL_CFLAGS:=-I../include -DLOG_TAG=\"nuClient\"
LOCAL_SHARED_LIBRARIES := libutils liblog

LOCAL_MODULE:= nuClient

include $(BUILD_EXECUTABLE)
