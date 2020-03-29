#ifndef OBJECT_H
#define OBJECT_H

#include "ao.h"
#include "av.h"
#include "ai.h"
#include "bi.h"

#define SITE_INFO_LEN 100

#define NAME_LEN 100

typedef struct {
    int people;
}status_change_t;

typedef struct {
    int object_id;
    int people_num;
    status_change_t status_change;
    char mac[30];
    char upload_time[100];
    char location[100];
}device_t;

typedef enum {
    INFO = 1,
}sensor_type_t;

typedef struct {
    int device_type;
    int object_type;
    long object_offset;
    device_t *device_list;
    long object_count;
    int (*check_sensor_change)(device_t *);
    int (*clear_sensor_change)(device_t *);
    int (*get_device_name)(device_t *, char *name);
    int (*get_device_description)(device_t *, char *);
    int (*get_device_value)(device_t *);
    int (*get_device_timeout)(device_t *);
    int (*get_device_upload_time)(device_t *, char *);
    int (*set_device_value)(device_t *, int);
}object_t;

typedef struct {
    object_t *object;
    device_t *device;
    uint32_t object_instance;
}device_key_t;

extern object_t g_object_list[];

void object_init();

#define OBJECT_INIT do{\
    int object_conut = 0;\
    g_device_list_count = 0;\
    object_t *object;\
    for(;;)\
    {   \
        object = &g_object_list[object_conut];\
        if(object->device_type == -1)\
            break;\
        if(object->object_type == TYPE)\
        {\
            for(int n = 0; n < (object->object_count); n++)\
            {\
                g_local_list[g_device_list_count].object = object;\
                g_local_list[g_device_list_count].device = &object->device_list[n];\
                g_local_list[g_device_list_count].object_instance = object->object_offset + object->device_list[n].object_id;\
                g_device_list_count++;\
            }\
        }\
        object_conut++;\
    }\
    BAC_LOGD("init success, count :%d", g_device_list_count);\
}while(0)



#endif
