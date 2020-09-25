#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main(int argc, char** argv) {
    int af = AF_INET6;
    char * iptext;
    struct in6_addr* ipbin = malloc(sizeof(struct in6_addr));
    int error_code = 0;

    if (argc < 2) {
        fprintf(stderr, "Usar %s IPv6\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    iptext = argv[1];

    error_code = inet_pton(af, iptext, (void*) ipbin);

    if (error_code == 0) {
        fprintf(stderr, "Dirección %s no válida.\n", iptext);
        exit(EXIT_FAILURE);
    } else if (error_code == -1) {
        fprintf(stderr, "La dirección no tiene el formato correcto.\n");
        exit(EXIT_FAILURE);
    } else if (error_code != 1) {
        fprintf(stderr, "Error desconocido\n");
        exit(EXIT_FAILURE);
    }

    printf("%s -> %X\n", iptext, ipbin);

    printf("Bytes:\n");
    for (int i = 0; i < 16; i++) {
        printf("Byte %d: %p\n", i, ipbin[i]);
    }


    return EXIT_SUCCESS;
}