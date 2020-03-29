#include "object.h"
#include "get_data.h"

static const char *TAG = "object.c";

//被bacnet-stack调用
void object_init()
{
    Analog_Input_Init();
    Analog_Output_Init();
    Analog_Value_Init();
    Binary_Input_Init();
    Binary_Output_Init();
}



int check_sensor_change_people(device_t * device)
{
    if(device->status_change.people == 1)
        return 1;
    else
        return 0;
}

int clear_sensor_change_people(device_t * device)
{
    device->status_change.people = 0;
    return 0;
}


int get_device_name_people(device_t * device, char *name)
{
    sprintf(name, "%s", device->location);  
    return 0;
}

int get_device_description(device_t * device, char *str)
{
    sprintf(str, "%s", device->mac);
    return 0;    
}

int get_device_value_people(device_t *device)
{
	return device->people_num;
}

int get_device_timeout(device_t * device)
{
        return 0;
}

int get_device_upload_time(device_t * device, char *str)
{
    strcpy(str, device->upload_time);
    return 0; 
}


object_t g_object_list[] = {
    {
        INFO,
        OBJECT_ANALOG_INPUT,
        0,
        g_device_list,
        0,
        check_sensor_change_people,
        clear_sensor_change_people,
        get_device_name_people,
        get_device_description,
        get_device_value_people,
        get_device_timeout,
        get_device_upload_time,
        NULL,
    },

    {
        -1,
        0,
        0,
        NULL,
        0,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    },
};
