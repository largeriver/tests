LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	client.cpp

LOCAL_SHARED_LIBRARIES := libutils libbinder

LOCAL_MODULE:= myclient

include $(BUILD_EXECUTABLE)
