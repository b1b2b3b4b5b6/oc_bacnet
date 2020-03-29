/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE W` ARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mdf_json.h"
#include <stdbool.h>

static const char *TAG = "mdf_json.c";

int __json_parse_object(const char *json_str, const char *key, void *value, int value_type)
{
    cJSON *pJson = cJSON_Parse(json_str);
    MDF_ERROR_CHECK(!pJson, -1, "cJSON_Parse");
    cJSON *pSub = NULL;
    if(strlen(key) > 0)
        pSub = cJSON_GetObjectItem(pJson, key);
    else
        pSub = pJson;


    if (!pSub) {
        printf("cJSON_GetObjectItem, key: %s", key);
        goto ERR_EXIT;
    }

    char *pSub_raw     = NULL;
    int array_size     = 0;

    switch (value_type) {
        case 1:
            *((char *)value) = pSub->valueint;
            break;

        case 2:
            *((short *)value) = pSub->valueint;
            break;

        case 3:
            *((int *)value) = pSub->valueint;
            break;

        case 4:
            *((float *)value) = (float)(pSub->valuedouble);
            break;

        case 5:
            *((double *)value) = (double)pSub->valuedouble;
            break;

        /**< "case 6" means string, bur it should be handle in default channel,
             because it may just want to query string or subJson raw */
        default:


            switch (pSub->type) {
                case cJSON_False:
                    *((char *)value) = false;
                    break;

                case cJSON_True:
                    *((char *)value) = true;
                    break;

                case cJSON_Number:
                    *((char *)value) = pSub->valueint;
                    break;

                case cJSON_NULL:
                    if(value_type == 6)
                    (*(char **)value) = calloc(1,1);
                    break;

                case cJSON_String:
                    *((char **)value) = malloc(strlen(pSub->valuestring) + 1);
                    memcpy(*((char **)value), pSub->valuestring, strlen(pSub->valuestring) + 1);
                    break;

                case cJSON_Object:
                    pSub_raw = cJSON_PrintUnformatted(pSub);
                    MDF_ERROR_GOTO(!pSub_raw, ERR_EXIT, "cJSON_PrintUnformatted");
                    *((char **)value) = pSub_raw;
                    break;

                case cJSON_Array:
                    array_size = cJSON_GetArraySize(pSub);
                    char **index = malloc(array_size * sizeof(char *));
                    *((char ***)value) = index;
                    for (int i = 0; i < array_size; ++i)
                    {
                        cJSON *item = cJSON_GetArrayItem(pSub, i);
                        
                        if (item->type == cJSON_Number) {
                            *((int *)value + i) = item->valueint;
                            continue;
                        }

                        if (item->type == cJSON_String) {
                            int len = strlen(item->valuestring);
                            cJSON_Minify(item->valuestring);
                            index[i] = malloc(len + 1);
                            memcpy(index[i] , item->valuestring, len + 1);
                            continue;
                        }

                        if (item->type == cJSON_Object) {
                            char *temp = cJSON_PrintUnformatted(item);
                            MDF_ERROR_GOTO(!temp,ERR_EXIT, "cJSON_PrintUnformatted NULL");
                            *index = temp;
                            index ++;
                        }

                        /**< no sub cJSON_Array, just support one layer of cJSON_Array */
                    }

                    /**< cJSON_Array parsed successful, return array_size */
                    cJSON_Delete(pJson);
                    return array_size;
                    break;

                default:
                    BAC_LOGE("does not support this type(%d) of data parsing", pSub->type);
                    break;
            }
    }

    cJSON_Delete(pJson);
    return 0;

ERR_EXIT:
    cJSON_Delete(pJson);
    return -1;
}

int json_pack_object_raw(const char *json_str, const char *key, char **object_str)
{
    cJSON *pJson = cJSON_Parse(json_str);
    MDF_ERROR_CHECK(!pJson, -1, "cJSON_Parse");
    cJSON *pSub = NULL;
    pSub = cJSON_GetObjectItem(pJson, key);

    if (!pSub) {
        printf("cJSON_GetObjectItem, key: %s", key);
        goto ERR_EXIT;
    }

    char *pSub_raw     = NULL; 
    pSub_raw = cJSON_PrintUnformatted(pSub);
    MDF_ERROR_GOTO(!pSub_raw, ERR_EXIT, "cJSON_PrintUnformatted");
    *object_str = pSub_raw;

    cJSON_Delete(pJson);
    return 0;   
ERR_EXIT:
    cJSON_Delete(pJson);
    return -1;
}


ssize_t __json_pack_object(char *json_str, const char *key, void *value, int value_type)
{
    MDF_PARAM_CHECK(json_str);
    MDF_PARAM_CHECK(key);

    printf("value_type: %d", value_type);

    /**< start symbol of a json object */
    char identifier = '{';

    /**< pack data into array that has been existed */
    if (*key == '[') {
        identifier = '[';
        key = NULL;
    }

    int index = 0;

    if (*json_str == identifier) {
        index     = strlen(json_str) - 1;
        json_str += index;
        *json_str = ',';
    } else {
        *json_str = identifier;
    }

    /**< forward to key field by "++" */
    json_str++;
    index++;

    /**< pack key into json_str */
    int tmp_len = key ? sprintf(json_str, "\"%s\":", key) : 0;
    json_str += tmp_len;
    index    += tmp_len;
    char *string = *((char **)value);
    switch (value_type) {
        case 1: /**< integral number */
            tmp_len = sprintf(json_str, "%d", *((int *)value));
            break;

        case 2: /**< floating-point number, but just save the integral part */
            tmp_len = sprintf(json_str, "%lf", *((float *)value));
            break;

        case 3: /**< string */

            if (*string == '{' || *string == '[') {
                tmp_len = sprintf(json_str, "%s", (char *)string);
            } else {
                tmp_len = sprintf(json_str, "\"%s\"", (char *)string);
            }

            break;

        default:
            printf("invalid type: %d", value_type);
            return -1;
    }

    /**< update json_str and index after packed key */
    json_str += tmp_len;
    index    += tmp_len;

    /**< finish json_str with '}' or ']' */
    *json_str = identifier == '{' ? '}' : ']';
    json_str++;
    *json_str = '\0';

    index++;

    return index;
}
