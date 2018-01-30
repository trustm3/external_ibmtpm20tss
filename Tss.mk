#
# libtss (TPM 2.0 TSS)
#

include $(CLEAR_VARS)

common_cflags := \
        -Wall -W -Wmissing-declarations -Wmissing-prototypes -Wnested-externs \
        -ggdb -O0 -c \
        -DTPM_ENCRYPT_SESSIONS_DEFAULT="\"0\""

tss_cflags := -DTPM_TSS -DTPM_POSIX -DTPM_SOCKET_UDS

tss_c_includes := \
  external/ibmtpm20tss/utils/. \
  external/ibmtpm20tss/utils/tss2 \
  external/ibmtpm20tss/src/. \
  external/openssl_legacy/include \


tss_src_files := \
  utils/tss.c \
  utils/tssproperties.c \
  utils/tssauth.c \
  utils/tssmarshal.c \
  utils/tsscrypto.c \
  utils/tssutils.c \
  utils/tsssocket.c \
  utils/tssdev.c \
  utils/tsstransmit.c \
  utils/tssresponsecode.c \
  utils/tssccattributes.c \
  utils/fail.c \
  utils/tssprint.c

tpm_shared_src_files := \
  src/Unmarshal.c \
  src/Commands.c \
  src/CommandAttributeData.c \
  src/CpriHash.c \
  src/CpriSym.c

LOCAL_CFLAGS += $(tss_cflags) $(common_cflags)
LOCAL_C_INCLUDES += $(tss_c_includes)

LOCAL_LDFLAGS := -Wl

LOCAL_SRC_FILES := $(tpm_shared_src_files) $(tss_src_files)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= libtss

include $(BUILD_STATIC_LIBRARY)
