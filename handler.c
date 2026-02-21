#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "http.h"


char* fileRead(const char* name, ssize_t* outSize) {
    FILE* filePtr = fopen(name, "rb");
    char* buffer;
    ssize_t size;

    if (filePtr == NULL) {
        fprintf(stderr, "[+] Error reading %s\n", name);
        return NULL;
    }

    fseek(filePtr, 0, SEEK_END);
    size = ftell(filePtr);
    fseek(filePtr, 0, SEEK_SET);

    buffer = (char*)malloc(size);

    if (buffer == NULL) {
        fprintf(stderr, "[+] Error allocating memory for %s\n", name);
        fclose(filePtr);
        return NULL;
    }

    if (fread(buffer, 1, size, filePtr) != size){
        free(buffer);
        return NULL;
    }

    fclose(filePtr);

    *outSize = size;
    return buffer;
}

const char* getFileType(const char* filename) {
    const char* dotPos = strrchr(filename, '.');
    const char* extension = (dotPos + 1);
    for (int i = 0; i < strlen(extension); i++) tolower(extension[i]);

    if (strcmp(extension, "html") == 0 || strcmp(extension, "htm") == 0) {
    return "text/html";
    } else if (strcmp(extension, "css") == 0) {
        return "text/css";
    } else if (strcmp(extension, "js") == 0) {
        return "application/js";
    } else if (strcmp(extension, "png") == 0) {
        return "image/png";
    } else if (strcmp(extension, "jpg") == 0 || strcmp(extension, "jpeg") == 0) {
        return "image/jpg";
    } else if (strcmp(extension, "svg") == 0) {
        return "image/svg";
    } else if (strcmp(extension, "webp") == 0) {
        return "image/webp";
    } else if (strcmp(extension, "txt") == 0) {
        return "text/plain";
    } else if (strcmp(extension, "json") == 0) {
        return "application/json";
    }

    return "application/octet-stream";
}


struct HTTP_RESPONSE handleRequest (struct HTTP_REQUEST* request, const char* SERVEPATH){
    struct HTTP_RESPONSE thisResponse = {NULL, 0, HTTP_NOT_FOUND};

    if (strstr(SERVEPATH, "..")) {
        thisResponse.status = HTTP_FORBIDDEN;
        thisResponse.responseMsg = "HTTP/1.1 403 Forbidden\r\n"
                                   "content-length: 17\r\n"
                                   "content-type: text/plain\r\n\r\n"
                                   "403 - Forbidden\n";
        thisResponse.responseSize = strlen(thisResponse.responseMsg);
        thisResponse.dynamicAllocate = 0;
        return thisResponse;
    }

    ssize_t readSize;
    char* readPath = fileRead(SERVEPATH, &readSize);
    if (readPath == NULL) {
        thisResponse.status = HTTP_NOT_FOUND;
        thisResponse.responseMsg = "HTTP/1.1 404 Not Found\r\n"
                                   "content-length: 21\r\n"
                                   "content-type: text/plain\r\n\r\n"
                                   "404 - File Not Found\n";
        thisResponse.responseSize = strlen(thisResponse.responseMsg);
        thisResponse.dynamicAllocate = 0;
    } else {
        thisResponse.status = HTTP_OK;
        ssize_t totalSize = 128 + readSize;
        thisResponse.responseMsg = (char*)malloc(totalSize);
        snprintf(thisResponse.responseMsg, totalSize, "HTTP/1.1 200 OK\r\n" 
                                                      "content-length: %d\r\n"
                                                      "content-type: %s\r\n\r\n"
                                                      "%s", readSize, getFileType(SERVEPATH), readPath);
        thisResponse.dynamicAllocate = 1;
        thisResponse.responseSize = strlen(thisResponse.responseMsg);
    }

    return thisResponse;
}
