# Copyright 2006 The Android Open Source Project

LOCAL_PATH:= $(call my-dir)

###############################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	syncproptest.cpp \

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libutils \
	libcutils \
	libbinder \
	libsyncprop_services \

LOCAL_C_INCLUDES += \
    vendor/xdja/proprietary/include \

LOCAL_MODULE:= syncproptest
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

###############################################
