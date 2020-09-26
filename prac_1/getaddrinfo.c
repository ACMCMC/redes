#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char** argv) {
    struct addrinfo** res;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));

    getaddrinfo("www.google.es", NULL, hints, res);


    return EXIT_SUCCESS;
}