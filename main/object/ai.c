/**************************************************************************
*
* Copyright (C) 2005 Steve Karg <skarg@users.sourceforge.net>
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*********************************************************************/

/* Analog Output Objects - customize for your use */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "bacdef.h"
#include "bacdcode.h"
#include "bacenum.h"
#include "bacapp.h"
#include "config.h"     /* the custom stuff */
#include "wp.h"
#include "ai.h"
#include "handlers.h"
#include "get_data.h"

#define TYPE  OBJECT_ANALOG_INPUT
static device_key_t g_local_list[10000] = {0};
static int g_device_list_count;
static const char *TAG = "ai.c";
/* These three arrays are used by the ReadPropertyMultiple handler */
static const int Properties_Required[] = {
    PROP_OBJECT_IDENTIFIER,
    PROP_OBJECT_NAME,
    PROP_OBJECT_TYPE,
    PROP_PRESENT_VALUE,
    PROP_STATUS_FLAGS,
    //PROP_STATE_TEXT,
    PROP_DESCRIPTION,
    //PROP_UNITS,
    //PROP_CHANGE_OF_STATE_TIME,
    -1
};

static const int Properties_Optional[] = {
    -1
};

static const int Properties_Proprietary[] = {
    -1
};

void Analog_Input_Property_Lists(
    const int **pRequired,
    const int **pOptional,
    const int **pProprietary)
{
    if (pRequired)
        *pRequired = Properties_Required;
    if (pOptional)
        *pOptional = Properties_Optional;
    if (pProprietary)
        *pProprietary = Properties_Proprietary;

    return;
}

void Analog_Input_Init(
    void)
{
    OBJECT_INIT;
    return;
}

/* we simply have 0-n object instances.  Yours might be */
/* more complex, and then you need validate that the */
/* given instance exists */
bool Analog_Input_Valid_Instance(
    uint32_t object_instance)
{
    int res;
    res = Analog_Input_Instance_To_Index(object_instance);
    if(res < 0)
    {
        //BAC_LOGE("undefined object : %d", object_instance);
        return false;
    }
    else
        return true;
}

/* we simply have 0-n object instances.  Yours might be */
/* more complex, and then count how many you have */
unsigned Analog_Input_Count(
    void)
{
    return g_device_list_count;
}

/* we simply have 0-n object instances.  Yours might be */
/* more complex, and then you need to return the instance */
/* that correlates to the correct index */
uint32_t Analog_Input_Index_To_Instance(
    unsigned index)
{
    return g_local_list[index].object_instance;
}

/* we simply have 0-n object instances.  Yours might be */
/* more complex, and then you need to return the index */
/* that correlates to the correct instance number */
unsigned Analog_Input_Instance_To_Index(
    uint32_t object_instance)
{
    for(int n = 0; n < g_device_list_count; n++)
    {
        if(g_local_list[n].object_instance == object_instance)
            return n;
    }
    return -1;
}

float Analog_Input_Present_Value(
    uint32_t object_instance)
{
    float value;
    unsigned index = Analog_Input_Instance_To_Index(object_instance);
    device_key_t *key;
    key = &g_local_list[index];
    value = key->object->get_device_value(key->device);
    return value;
}


/* note: the object name must be unique within this device */
bool Analog_Input_Object_Name(
    uint32_t object_instance,
    BACNET_CHARACTER_STRING * object_name)
{
    static char text_string[100] = "";   /* okay for single thread */
    bool status = false;
    unsigned index = 0;

    index = Analog_Input_Instance_To_Index(object_instance);
    device_key_t *key;
    key = &g_local_list[index];
    key->object->get_device_name(key->device, text_string);
    status = characterstring_init_ansi(object_name, text_string);

    return status;
}

bool Analog_Input_Object_Description(
    uint32_t object_instance,
    BACNET_CHARACTER_STRING * description_name)
{
    unsigned index = Analog_Input_Instance_To_Index(object_instance);
    static char text_string[100] = "";   /* okay for single thread */
    bool status = false;

    index = Analog_Input_Instance_To_Index(object_instance);
    device_key_t *key;
    key = &g_local_list[index];
    key->object->get_device_description(key->device, text_string);
    status = characterstring_init_ansi(description_name, text_string);

    return status; 
}

bool Analog_Input_Timeout(uint32_t object_instance)
{
    unsigned index = Analog_Input_Instance_To_Index(object_instance);
    device_key_t *key;
    key = &g_local_list[index];
    return key->object->get_device_timeout(key->device) > 0?true:false;
}

bool Analog_Input_Status_Text(
    uint32_t object_instance,
    BACNET_CHARACTER_STRING * Text)
{
    char text_string[32] = "";   /* okay for single thread */
    bool status = false;

    if(Analog_Input_Timeout(object_instance) == true)
        strcat(text_string, "Time Out");
    else
        strcat(text_string, "Online");
    status = characterstring_init_ansi(Text, text_string);

    return status;
}

bool Analog_Input_ChangeTime(
    uint32_t object_instance,
    BACNET_CHARACTER_STRING * Text)
{
    char text_string[100] = "";   /* okay for single thread */
    bool status = false;
    unsigned index = 0;

    index = Analog_Input_Instance_To_Index(object_instance);
    device_key_t *key;
    key = &g_local_list[index];
    key->object->get_device_upload_time(key->device, text_string);
    status = characterstring_init_ansi(Text, text_string);
    return status;
}




/* return apdu len, or BACNET_STATUS_ERROR on error */
int Analog_Input_Read_Property(
    BACNET_READ_PROPERTY_DATA * rpdata)
{
    int apdu_len = 0;   /* return value */
    BACNET_BIT_STRING bit_string;
    BACNET_CHARACTER_STRING char_string;
    float real_value = (float) 1.414;
    uint8_t *apdu = NULL;

    if ((rpdata == NULL) || (rpdata->application_data == NULL) ||
        (rpdata->application_data_len == 0)) {
        return 0;
    }
    apdu = rpdata->application_data;
    
    //BAC_LOGI("property: %d, bject_instance: %d", rpdata->object_property, rpdata->object_instance);
    switch (rpdata->object_property) {
        case PROP_OBJECT_IDENTIFIER:
            apdu_len =
                encode_application_object_id(&apdu[0], TYPE,
                rpdata->object_instance);
            break;
        case PROP_OBJECT_NAME:
            Analog_Input_Object_Name(rpdata->object_instance, &char_string);
            apdu_len =
                encode_application_character_string(&apdu[0], &char_string);
            break;
        case PROP_DESCRIPTION:
            Analog_Input_Object_Description(rpdata->object_instance, &char_string);
            apdu_len =
                encode_application_character_string(&apdu[0], &char_string);
            break;
        case PROP_OBJECT_TYPE:
            apdu_len =
                encode_application_enumerated(&apdu[0], TYPE);
            break;
        case PROP_PRESENT_VALUE:
            real_value = Analog_Input_Present_Value(rpdata->object_instance);
            apdu_len = encode_application_real(&apdu[0], real_value);
            break;
        case PROP_STATUS_FLAGS:
            bitstring_init(&bit_string);
            bitstring_set_bit(&bit_string, STATUS_FLAG_IN_ALARM, false);
            bitstring_set_bit(&bit_string, STATUS_FLAG_FAULT, Analog_Input_Timeout(rpdata->object_instance));
            bitstring_set_bit(&bit_string, STATUS_FLAG_OVERRIDDEN, false);
            bitstring_set_bit(&bit_string, STATUS_FLAG_OUT_OF_SERVICE, false);
            apdu_len = encode_application_bitstring(&apdu[0], &bit_string);
            break;

        case PROP_UNITS:
            apdu_len = encode_application_enumerated(&apdu[0], UNITS_NO_UNITS);
            break;

        case PROP_STATE_TEXT:
            /* note: object name must be unique in our device */
            Analog_Input_Status_Text(rpdata->object_instance, &char_string);
            apdu_len =
                encode_application_character_string(&apdu[0], &char_string);
            break;

        case PROP_CHANGE_OF_STATE_TIME:
            Analog_Input_ChangeTime(rpdata->object_instance, &char_string);
            apdu_len =
                encode_application_character_string(&apdu[0], &char_string);
            break;
   
        default:
            rpdata->error_class = ERROR_CLASS_PROPERTY;
            rpdata->error_code = ERROR_CODE_UNKNOWN_PROPERTY;
            apdu_len = BACNET_STATUS_ERROR;
            break;
    }
    /*  only array properties can have array options */
    if ((apdu_len >= 0) && (rpdata->array_index != BACNET_ARRAY_ALL)) {
        rpdata->error_class = ERROR_CLASS_PROPERTY;
        rpdata->error_code = ERROR_CODE_PROPERTY_IS_NOT_AN_ARRAY;
        apdu_len = BACNET_STATUS_ERROR;
    }

    return apdu_len;
}

bool Analog_Input_Encode_Value_List(
    uint32_t object_instance,
    BACNET_PROPERTY_VALUE * value_list)
{
    bool status = false;
    if (value_list) {
        value_list->propertyIdentifier = PROP_PRESENT_VALUE;
        value_list->propertyArrayIndex = BACNET_ARRAY_ALL;
        value_list->value.context_specific = false;
        value_list->value.tag = BACNET_APPLICATION_TAG_ENUMERATED;
        value_list->value.next = NULL;
        value_list->value.type.Enumerated =
            Analog_Input_Present_Value(object_instance);
        value_list->priority = BACNET_NO_PRIORITY;
        value_list = value_list->next;
    }
    if (value_list) {
        value_list->propertyIdentifier = PROP_STATUS_FLAGS;
        value_list->propertyArrayIndex = BACNET_ARRAY_ALL;
        value_list->value.context_specific = false;
        value_list->value.tag = BACNET_APPLICATION_TAG_BIT_STRING;
        value_list->value.next = NULL;
        bitstring_init(&value_list->value.type.Bit_String);
        bitstring_set_bit(&value_list->value.type.Bit_String,
            STATUS_FLAG_IN_ALARM, false);
        bitstring_set_bit(&value_list->value.type.Bit_String,
            STATUS_FLAG_FAULT,Analog_Input_Timeout(object_instance));
        bitstring_set_bit(&value_list->value.type.Bit_String,
            STATUS_FLAG_OVERRIDDEN, false);
        bitstring_set_bit(&value_list->value.type.Bit_String,
                STATUS_FLAG_OUT_OF_SERVICE, false);
        value_list->priority = BACNET_NO_PRIORITY;
        value_list->next = NULL;
        status = true;
    }

    return status;
}


/* returns true if successful */
bool Analog_Input_Write_Property(
    BACNET_WRITE_PROPERTY_DATA * wp_data)
{
    bool status = false;        /* return value */
    int len = 0;
    BACNET_APPLICATION_DATA_VALUE value;
    int code;

    /* decode the some of the request */
    len =
        bacapp_decode_application_data(wp_data->application_data,
        wp_data->application_data_len, &value);
    /* FIXME: len < application_data_len: more data? */
    if (len < 0) {
        /* error while decoding - a value larger than we can handle */
        wp_data->error_class = ERROR_CLASS_PROPERTY;
        wp_data->error_code = ERROR_CODE_VALUE_OUT_OF_RANGE;
        return false;
    }
    /*  only array properties can have array options */
    if ((wp_data->object_property != PROP_PRIORITY_ARRAY) &&
        (wp_data->array_index != BACNET_ARRAY_ALL)) {
        wp_data->error_class = ERROR_CLASS_PROPERTY;
        wp_data->error_code = ERROR_CODE_PROPERTY_IS_NOT_AN_ARRAY;
        return false;
    }
    switch (wp_data->object_property) {
        default:
            wp_data->error_class = ERROR_CLASS_PROPERTY;
            wp_data->error_code = ERROR_CODE_WRITE_ACCESS_DENIED;
            break;
    }

    return status;
}


bool Analog_Input_Change_Of_Value(
    uint32_t object_instance)
{
    bool status = false;
    unsigned index;

    index = Analog_Input_Instance_To_Index(object_instance);
    device_key_t *key;
    key = &g_local_list[index];
    status = key->object->check_sensor_change(key->device);
    return status;
}

void Analog_Input_Change_Of_Value_Clear(
    uint32_t object_instance)
{
    unsigned index;

    index = Analog_Input_Instance_To_Index(object_instance);
    device_key_t *key;
    key = &g_local_list[index];
    key->object->clear_sensor_change(key->device);
    return;
}