// http protocol parsing and stuff
#ifndef _HTTP_H
#define _HTTP_H

#include "strio.h"

#include <stdbool.h>

#define OK 200
#define BAD_REQUEST 400
#define NOT_FOUND 404
#define LENGTH_REQUIRED 411
#define PAYLOAD_TOO_LARGE 413
#define URI_TOO_LONG 414
#define IM_A_TEAPOT 418
#define INTERNAL_SERVER_ERROR 500
#define NOT_IMPLEMENTED 501

enum method
{
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    CONNECT,
    OPTIONS,
    TRACE,
    PATCH,
};

enum version
{
    // _0_9, // no one will get mad if we don't support 0.9
    _1_0,
    _1_1,
};

struct status
{
    char desc[24]; // longest message is currently ~18b
    int code;
};

enum version verfromstr(const char verstr[8]);

bool stopafterstring(struct buffer* buf, void* _str);
//stop after finding \r\n
bool stopafterline(struct buffer* buf, void*_);
void serve(fd sock);
#endif