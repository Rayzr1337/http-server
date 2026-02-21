#ifndef HTTP
#define HTTP
#include <sys/types.h>

enum HTTP_STATUS {
    HTTP_OK,
    HTTP_NOT_FOUND,
    HTTP_FORBIDDEN
};


struct HTTP_REQUEST {
    char* method;
    char* path;
    char* version;
    char* body;
};

struct HTTP_RESPONSE {
    char* responseMsg;
    ssize_t responseSize;
    enum HTTP_STATUS status;
    int dynamicAllocate;
};

#endif
