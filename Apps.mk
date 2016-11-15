tss_app_common_cflags := \
        -Wall -W -Wmissing-declarations -Wmissing-prototypes -Wnested-externs \
        -ggdb -O0 -c \
        -DTPM_ENCRYPT_SESSIONS_DEFAULT="\"0\""

tss_app_cflags := -DTPM_POSIX -DTPM_SOCKET_UDS

tss_app_c_includes := \
  external/ibmtpm20tss/utils


include $(CLEAR_VARS)

LOCAL_MODULE := tpm2_powerup
LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += $(tss_app_cflags) $(tss_app_common_cflags)
LOCAL_C_INCLUDES += $(tss_app_c_includes)

LOCAL_SRC_FILES := utils/powerup.c

LOCAL_STATIC_LIBRARIES := \
        libc \
        libcutils \
	libtss \
	libcrypto_static

LOCAL_WHOLE_STATIC_LIBRARIES := libtss

LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT_SBIN)
LOCAL_UNSTRIPPED_PATH := $(TARGET_ROOT_OUT_SBIN_UNSTRIPPED)

LOCAL_MODULE_CLASS := EXECUTABLES

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_MODULE := tpm2_startup
LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += $(tss_app_cflags) $(tss_app_common_cflags)
LOCAL_C_INCLUDES += $(tss_app_c_includes)

LOCAL_SRC_FILES := utils/startup.c

LOCAL_STATIC_LIBRARIES := \
        libc \
        libcutils \
	libtss \
	libcrypto_static

LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT_SBIN)
LOCAL_UNSTRIPPED_PATH := $(TARGET_ROOT_OUT_SBIN_UNSTRIPPED)

LOCAL_MODULE_CLASS := EXECUTABLES

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_MODULE := tpm2_create
LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += $(tss_app_cflags) $(tss_app_common_cflags)
LOCAL_C_INCLUDES += $(tss_app_c_includes)

LOCAL_SRC_FILES := \
	utils/objecttemplates.c \
	utils/create.c

LOCAL_STATIC_LIBRARIES := \
        libc \
        libcutils \
	libtss \
	libcrypto_static

LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT_SBIN)
LOCAL_UNSTRIPPED_PATH := $(TARGET_ROOT_OUT_SBIN_UNSTRIPPED)

LOCAL_MODULE_CLASS := EXECUTABLES

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_MODULE := tpm2_createprimary
LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += $(tss_app_cflags) $(tss_app_common_cflags)
LOCAL_C_INCLUDES += $(tss_app_c_includes)

LOCAL_SRC_FILES := \
	utils/objecttemplates.c \
	utils/createprimary.c

LOCAL_STATIC_LIBRARIES := \
        libc \
        libcutils \
	libtss \
	libcrypto_static

LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT_SBIN)
LOCAL_UNSTRIPPED_PATH := $(TARGET_ROOT_OUT_SBIN_UNSTRIPPED)

LOCAL_MODULE_CLASS := EXECUTABLES

include $(BUILD_EXECUTABLE)
