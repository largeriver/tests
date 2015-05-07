LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	svr_loader.cpp

LOCAL_SHARED_LIBRARIES := libutils liblog libbinder

LOCAL_CFLAGS:= -DLOG_TAG=\"ccSL\"

LOCAL_MODULE:= comchatSvrLoader

include $(BUILD_EXECUTABLE)

