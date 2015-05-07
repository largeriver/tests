LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	client.cpp

LOCAL_CFLAGS:= -DLOG_TAG=\"ccCli\"
LOCAL_SHARED_LIBRARIES := libutils libbinder libcomchatPS

LOCAL_MODULE:= comchatClient

include $(BUILD_EXECUTABLE)
