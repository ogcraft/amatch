
#include <android/log.h>

#if 1

#define LOGV(TAG,...) __android_log_print(ANDROID_LOG_VERBOSE, TAG,__VA_ARGS__)
#define LOGD(TAG,...) __android_log_print(ANDROID_LOG_DEBUG  , TAG,__VA_ARGS__)
#define LOGI(TAG,...) __android_log_print(ANDROID_LOG_INFO   , TAG,__VA_ARGS__)
#define LOGW(TAG,...) __android_log_print(ANDROID_LOG_WARN   , TAG,__VA_ARGS__)
#define LOGE(TAG,...) __android_log_print(ANDROID_LOG_ERROR  , TAG,__VA_ARGS__)

#else
#include <stdio.h>
#define LOGV(TAG,...) printf(__VA_ARGS__)
#define LOGD(TAG,...) printf(__VA_ARGS__)
#define LOGI(TAG,...) printf(__VA_ARGS__)
#define LOGW(TAG,...) printf(__VA_ARGS__)
#define LOGE(TAG,...) printf(__VA_ARGS__)


#endif

extern const char* TAG;
