LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := amatch
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/../../include
LOCAL_CFLAGS := -O3 
LOCAL_CPPFLAGS :=$(LOCAL_CFLAGS)

LOCAL_SRC_FILES := amatch_interface_imp.cpp \
opensl_io.c \
amatch_main.cpp \
amatch_interface.cpp \
../../src/amatch.cpp \
../../src/asearch.cpp \
../../src/sigproc.cpp \
../../src/filters.cpp \
../../src/utils.cpp 
#../../src/amatch.cpp \

#LOCAL_SHARED_LIBRARIES := -L$(LOCAL_PATH)/../ext_libs/armabi -lsndfile

#LOCAL_SHARED_LIBRARIES := -lsndfile
LOCAL_LDLIBS := -llog -lOpenSLES 

include $(BUILD_SHARED_LIBRARY)
#include $(BUILD_EXECUTABLE)
