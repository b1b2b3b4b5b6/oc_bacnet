#ifndef REQUEST_H
#define REQUEST_H

#include "curl/curl.h"
#include "common.h"

typedef  enum{
    REQ_INFO = 1,
}request_type_t;

int request_init(char *ip);

int requeat_clean();

char *request_server(request_type_t type, char *body);


#endif