LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	server.cpp

LOCAL_SHARED_LIBRARIES := libutils liblog libbinder libcomchatService

LOCAL_CFLAGS:= -DLOG_TAG=\"ccServer\"

LOCAL_MODULE:= comchatServer

include $(BUILD_EXECUTABLE)

