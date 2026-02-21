#include <stdio.h>
#include <string.h>
#include "http.h"

int headersRecieved(char* buffer, ssize_t len) {
    for (ssize_t i = 0; i <= (len - 4); i++) {
        if (buffer[i] == '\r' && buffer[i+1] == '\n' &&
            buffer[i+2] == '\r' && buffer[i+3] == '\n') {
            return 1;
        }
    }
    return 0;
}

void parseRequest(struct HTTP_REQUEST* request, char* buffer, ssize_t len) {
    int idx = 0;
    for (int i = 0; !(buffer[i] == '\r' && buffer[i+1] == '\n') 
            && ((i+1) < len); i++) {
        if ((buffer[i] == ' ' )|| (buffer[i] == '\r')) buffer[i] = '\0';
        idx++;
    }
    buffer[idx + 1] = '\0';

    request->method = buffer;
    request->path = request->method + strlen(request->method) + 1;
    request->version = request->path + strlen(request->path) + 1;
    request->body = request->version + strlen(request->version) + 1;

    char* query = strchr(request->path, '?');
    if (query) {
        (*query) = '\0';
    }
}

