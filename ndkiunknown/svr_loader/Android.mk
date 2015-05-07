LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	service.cpp
	
LOCAL_CFLAGS:=-I../include -DLOG_TAG=\"nuSvrLoader\"
LOCAL_SHARED_LIBRARIES := libutils libbinder

LOCAL_MODULE:= nuSvrLoader

include $(BUILD_EXECUTABLE)
