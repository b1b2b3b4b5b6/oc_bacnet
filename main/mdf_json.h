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
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __MDF_JOSN_H__
#define __MDF_JOSN_H__

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif /**< _cplusplus */

int json_pack_object_raw(const char *json_str, const char *key, char **object_str);

ssize_t __json_pack_object(char *json_str, const char *key, void *value, int value_type);

#define json_pack_object(json_str, key, value) \
    __json_pack_object((char *)(json_str), ( const char *)(key), (void *)(value), \
                    __builtin_types_compatible_p(typeof(value), char) * 1\
                    + __builtin_types_compatible_p(typeof(value), bool *) * 1\
                    + __builtin_types_compatible_p(typeof(value), int8_t *) * 1\
                    + __builtin_types_compatible_p(typeof(value), uint8_t *) * 1\
                    + __builtin_types_compatible_p(typeof(value), short) * 1\
                    + __builtin_types_compatible_p(typeof(value), uint16_t *) * 1\
                    + __builtin_types_compatible_p(typeof(value), int *) * 1\
                    + __builtin_types_compatible_p(typeof(value), uint32_t *) * 1\
                    + __builtin_types_compatible_p(typeof(value), long *) * 1\
                    + __builtin_types_compatible_p(typeof(value), unsigned long *) * 1\
                    + __builtin_types_compatible_p(typeof(value), float *) * 2\
                    + __builtin_types_compatible_p(typeof(value), double *) * 2\
                    + __builtin_types_compatible_p(typeof(value), char **) * 3\
                    + __builtin_types_compatible_p(typeof(value), const char *[]) * 3\
                    + __builtin_types_compatible_p(typeof(value), const char **) * 3\
                    + __builtin_types_compatible_p(typeof(value), unsigned char **) * 3\
                    + __builtin_types_compatible_p(typeof(value), const unsigned char **) * 3)

esp_err_t __json_parse_object(const char *json_str, const char *key, void *value, int value_type);
#define json_parse_object(json_str, key, value) \
    __json_parse_object(( const char *)(json_str), ( const char *)(key), value, \
                     __builtin_types_compatible_p(typeof(value), int8_t *) * 1\
                     + __builtin_types_compatible_p(typeof(value), uint8_t *) * 1\
                     + __builtin_types_compatible_p(typeof(value), short *) * 2\
                     + __builtin_types_compatible_p(typeof(value), uint16_t *) * 2\
                     + __builtin_types_compatible_p(typeof(value), int *) * 3\
                     + __builtin_types_compatible_p(typeof(value), uint32_t *) * 3\
                     + __builtin_types_compatible_p(typeof(value), long *) * 3\
                     + __builtin_types_compatible_p(typeof(value), unsigned long *) * 3\
                     + __builtin_types_compatible_p(typeof(value), float *) * 4\
                     + __builtin_types_compatible_p(typeof(value), double *) * 5\
                     + __builtin_types_compatible_p(typeof(value), char **) * 6\
                     + __builtin_types_compatible_p(typeof(value), char ***) * 7)\

#ifdef __cplusplus
}
#endif /**< _cplusplus */

#endif /**< __MDF_JOSN_H__ */
