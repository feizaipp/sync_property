# Copyright 2006 The Android Open Source Project

LOCAL_PATH:= $(call my-dir)
###############################################
include $(CLEAR_VARS)

LOCAL_C_INCLUDES += \
    vendor/xdja/proprietary/include \

LOCAL_SRC_FILES:= \
	SyncPropService.cpp \
	ISyncPropService.cpp \

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libutils \
	libcutils \
	libbinder \
	libfission_services \
	libfission_cutils \

LOCAL_C_INCLUDES += \
    vendor/xdja/proprietary/include \

LOCAL_MODULE:= libsyncprop_services
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)


