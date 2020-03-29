#ifndef COMMON_H
#define COMMON_H

#include "errno.h"
#include "time.h"
#include "semaphore.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "dirent.h"
#include "sys/stat.h"

#if 0
    #define TIME_STR ({ \
        time_t s_now;\
        time(&s_now);\
        asctime(localtime(&s_now));\
    })
#else
    #define TIME_STR ""
#endif

#if 0
    #define BAC_LOGV(format, ...) printf( "%s""[%s | line %d ] : "format"\r\n", TIME_STR, TAG, __LINE__,  ##__VA_ARGS__ )
#else
    #define BAC_LOGV(format, ...) 
#endif

#if 1
    #define BAC_LOGD(format, ...) printf( "%s""[%s | line %d ] : "format"\r\n", TIME_STR, TAG, __LINE__,  ##__VA_ARGS__ )
#else
    #define BAC_LOGD(format, ...) 
#endif



#define BAC_LOGI(format, ...) printf( "\033[32m%s""[%s | line %d ] : "format"\r\n\033[0m", TIME_STR, TAG, __LINE__,  ##__VA_ARGS__ )
#define BAC_LOGE(format, ...) printf( "\033[31m%s""[%s | line %d ] : "format"\r\n\033[0m", TIME_STR , TAG, __LINE__,  ##__VA_ARGS__)



#define MDF_PARAM_CHECK(arg) if(!arg) printf("error\r\n")
#define ESP_FAIL -1
#define ESP_OK 0
#define MDF_ERROR_GOTO(con, lable, format, ...) do { \
        if (con) { \
            printf(format , ##__VA_ARGS__); \
            goto lable; \
        } \
    }while (0)

#define MDF_ERROR_CHECK(con, err, format, ...) do { \
        if (con) { \
            printf(format , ##__VA_ARGS__); \
            return err; \
        } \
    }while (0)
typedef int esp_err_t;


#endif
