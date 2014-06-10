LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := amatch
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/../../include
LOCAL_CFLAGS := -O3 
LOCAL_CPPFLAGS :=$(LOCAL_CFLAGS)

LOCAL_SRC_FILES := \
amatch_interface.cpp \
../../src/amatch.cpp \
../../src/asearch.cpp \
../../src/sigproc.cpp \
../../src/filters.cpp \
../../src/amatch_interface_imp.cpp \
../../src/utils.cpp 

#LOCAL_SHARED_LIBRARIES := -L$(LOCAL_PATH)/../ext_libs/armabi -lsndfile


LOCAL_CFLAGS += -I$(LOCAL_PATH)/boost/include/boost-1_54
#LOCAL_LDLIBS += -L$(LOCAL_PATH)/boost/lib/ -lboost_system-gcc-mt -lboost_thread-gcc-mt

#LOCAL_STATIC_LIBRARIES := $(LOCAL_PATH)/boost/lib/libboost_system-gcc-mt-1_54.a $(LOCAL_PATH)/boost/lib/libboost_thread-gcc-mt-1_54.a

LOCAL_CPPFLAGS += -fexceptions -fpermissive
LOCAL_CPPFLAGS += -frtti

#LOCAL_LDLIBS := -llog -lOpenSLES 
LOCAL_LDLIBS := -llog -Lext_libs/armeabi -lsndfile -lfftw3 $(LOCAL_PATH)/boost/lib/libboost_system-gcc-mt-1_54.a $(LOCAL_PATH)/boost/lib/libboost_thread-gcc-mt-1_54.a $(ANDROID_NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.6/libs/armeabi-v7a/libgnustl_static.a


include $(BUILD_SHARED_LIBRARY)

#include $(BUILD_EXECUTABLE)
