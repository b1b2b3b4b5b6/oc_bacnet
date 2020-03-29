
#ifndef AV_H
#define AV_H

#include <stdbool.h>
#include <stdint.h>
#include "bacdef.h"
#include "bacerror.h"
#include "wp.h"
#include "rp.h"


void Analog_Value_Property_Lists(
    const int **pRequired,
    const int **pOptional,
    const int **pProprietary);
bool Analog_Value_Valid_Instance(
    uint32_t object_instance);
unsigned Analog_Value_Count(
    void);
uint32_t Analog_Value_Index_To_Instance(
    unsigned index);
unsigned Analog_Value_Instance_To_Index(
    uint32_t object_instance);

bool Analog_Value_Object_Name(
    uint32_t object_instance,
    BACNET_CHARACTER_STRING * object_name);

int Analog_Value_Read_Property(
    BACNET_READ_PROPERTY_DATA * rpdata);

bool Analog_Value_Write_Property(
    BACNET_WRITE_PROPERTY_DATA * wp_data);

int Analog_Value_Present_Value_Set(
    uint32_t object_instance,
    float value,
    unsigned priority);
float Analog_Value_Present_Value(
    uint32_t object_instance);
bool Analog_Value_Change_Of_Value(
    uint32_t instance);
void Analog_Value_Change_Of_Value_Clear(
    uint32_t instance);
bool Analog_Value_Encode_Value_List(
    uint32_t object_instance,
    BACNET_PROPERTY_VALUE * value_list);
float Analog_Value_COV_Increment(
    uint32_t instance);
void Analog_Value_COV_Increment_Set(
    uint32_t instance,
    float value);

char *Analog_Value_Description(
    uint32_t instance);
bool Analog_Value_Description_Set(
    uint32_t instance,
    char *new_name);

uint16_t Analog_Value_Units(
    uint32_t instance);
bool Analog_Value_Units_Set(
    uint32_t instance, uint16_t unit);

bool Analog_Value_Out_Of_Service(
    uint32_t instance);
void Analog_Value_Out_Of_Service_Set(
    uint32_t instance,
    bool oos_flag);

/* note: header of Intrinsic_Reporting function is required
    even when INTRINSIC_REPORTING is not defined */
void Analog_Value_Intrinsic_Reporting(
    uint32_t object_instance);

bool Analog_Value_Create(
    uint32_t object_instance);
bool Analog_Value_Delete(
    uint32_t object_instance);
void Analog_Value_Cleanup(
    void);
void Analog_Value_Init(
    void);

#endif
