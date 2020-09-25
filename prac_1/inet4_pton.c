#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main(int argc, char** argv) {
    int af = AF_INET;
    char * iptext;
    struct in_addr* ipbin = malloc(sizeof(struct in_addr));
    int error_code = 0;

    if (argc < 2) {
        fprintf(stderr, "Usar %s IPv4\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    iptext = argv[1];

    error_code = inet_pton(af, iptext, (void*) ipbin);

    if (error_code == 0) {
        fprintf(stderr, "Dirección %s no válida.\n", iptext);
        exit(EXIT_FAILURE);
    } else if (error_code == -1) {

    } else if (error_code != 1) {
        fprintf(stderr, "Error desconocido\n");
    }

    printf("%s -> %X\n", iptext, ipbin);




    return EXIT_SUCCESS;
}