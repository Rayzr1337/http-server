#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include "http.h"
#include "parser.c"
#include "handler.c"

#define METHOD currentRequest.method
#define PATH currentRequest.path
#define VERSION currentRequest.version
#define BODY currentRequest.body

#define METHOD_GET strcmp(METHOD, "GET") == 0
#define METHOD_POST strcmp(METHOD, "POST") == 0


int main(int argc, char* argv[]){
    if (argc != 3) { 
        fprintf(stderr, "usage: port rootdir\n");
        return EXIT_FAILURE;
    }

    char* PORT = argv[1];
    char* ROOT = argv[2];

    struct addrinfo hints, *results;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    int status, sockfd;

    if ((status = getaddrinfo(NULL, PORT, &hints, &results)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return EXIT_FAILURE;
    }

    for (struct addrinfo* p = results; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) != -1) {
            if (bind(sockfd, p->ai_addr, p->ai_addrlen) == 0){
                break;
            }
        }
    }
    freeaddrinfo(results);
    listen(sockfd, 20);
    printf("[+] Listening on port: %s\n", PORT);
    printf("-------------------------------\n\n");

    struct sockaddr_storage incoming;
    socklen_t in_size = sizeof incoming;
    char address[INET6_ADDRSTRLEN];

    while (1) {
        int incoming_fd = accept(sockfd, (struct sockaddr*) &incoming, &in_size);
        if (incoming_fd == -1) {
            continue;
        }
        inet_ntop(incoming.ss_family,
                  ((struct sockaddr*) &incoming)->sa_family == AF_INET ?
                  (void*) & ((struct sockaddr_in*)&incoming)->sin_addr :
                  (void*) & ((struct sockaddr_in6*)&incoming)->sin6_addr, 
                  address, sizeof address);

        printf("[+] Connection accepted from %s on port %s\n", address, PORT);

        char requestBuffer[8192];
        ssize_t rcvBytes = 0;

        while ((rcvBytes < sizeof requestBuffer) && !headersRecieved(requestBuffer, rcvBytes)){
            int currentRcv = recv(incoming_fd, requestBuffer + rcvBytes, sizeof requestBuffer - rcvBytes, 0);
            if (currentRcv == -1) {
                fprintf(stderr, "[+] (recv) error\n");
                continue;
            } else if (currentRcv == 0){
                fprintf(stderr, "[+] Remote side closed connection\n");
                break;
            }
            rcvBytes += currentRcv;
        }
        struct HTTP_REQUEST currentRequest;
        parseRequest(&currentRequest, requestBuffer, rcvBytes);

        char SERVE_PATH[4096];
        if (strcmp(PATH, "/") == 0) {
            snprintf(SERVE_PATH, sizeof SERVE_PATH, "%s/index.html", ROOT);
        } else {
            snprintf(SERVE_PATH, sizeof SERVE_PATH, "%s%s", ROOT, PATH);
        }
        printf("[+] HTTP Request recieved from %s: \n%s %s %s\n%s", address, METHOD, PATH, VERSION, BODY);

        struct HTTP_RESPONSE httpResponse = handleRequest(&currentRequest, SERVE_PATH);
        ssize_t bytesSent = 0;
        while (bytesSent < httpResponse.responseSize) {
            int currentSend = send(incoming_fd, httpResponse.responseMsg + bytesSent, httpResponse.responseSize - bytesSent, 0);
            if (currentSend == -1) {
                fprintf(stderr, "[+] (send) error on response\n");
                continue;
            }
            bytesSent += currentSend;
        }
        if (httpResponse.dynamicAllocate) free(httpResponse.responseMsg);

        printf("\r[+] HTTP Response sent to %s : %s\n", address, 
                (httpResponse.status == HTTP_OK ? "200 OK" : 
                 (httpResponse.status == HTTP_FORBIDDEN ? "403 FORBIDDEN" :
                  "404 NOT FOUND")));
        printf("-------------------------------\n\n");
        close(incoming_fd);
    }

    close (sockfd);
    return 0;
}
