###############################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	funcs/sync_prop.cpp \

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libutils \
	libcutils \
	libbinder \
    libfission_cutils \
    libfission_utils \
	libsyncprop_services \

LOCAL_C_INCLUDES += \
    vendor/xdja/proprietary/include \

LOCAL_MODULE:= sync_prop
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

###############################################
