#
# libtss (TPM 2.0 TSS)
#

include $(CLEAR_VARS)

common_cflags := \
        -Wall -W -Wmissing-declarations -Wmissing-prototypes -Wnested-externs \
        -ggdb -O0 -c \
        -DTPM_ENCRYPT_SESSIONS_DEFAULT="\"0\""

tss_cflags := -DTPM_TSS -DTPM_TPM20 -DTPM_POSIX -DTPM_SOCKET_UDS

tss_c_includes := \
  external/ibmtpm20tss/utils/. \
  external/ibmtpm20tss/utils/ibmtss \
  external/ibmtpm20tss/src/. \
  external/openssl_legacy/include \

tss_common_src_files := \
  utils/tss.c \
  utils/tssproperties.c \
  utils/tssmarshal.c \
  utils/tssauth.c \
  utils/tssutils.c \
  utils/tsssocket.c \
  utils/tssdev.c \
  utils/tsstransmit.c \
  utils/tssresponsecode.c \
  utils/tssccattributes.c \
  utils/tssprint.c \
  utils/Unmarshal.c \
  utils/CommandAttributeData.c

tss_common20_src_files := \
  utils/tss20.c	\
  utils/tssauth20.c \
  utils/Commands.c \
  utils/ntc2lib.c \
  utils/tssntc.c

tss_default_src_files := \
  utils/tssfile.c \
  utils/tsscryptoh.c \
  utils/tsscrypto.c \
  utils/tssprintcmd.c

LOCAL_CFLAGS += $(tss_cflags) $(common_cflags)
LOCAL_C_INCLUDES += $(tss_c_includes)

LOCAL_SRC_FILES := $(tss_common_src_files) $(tss_common20_src_files) $(tss_default_src_files)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= libibmtss

include $(BUILD_STATIC_LIBRARY)
