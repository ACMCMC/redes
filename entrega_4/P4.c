#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <string.h>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

// Aldán Creo Mariño, Redes, 2020/21

int determinarInterfaz(char *file, char *ip);

int main(int argc, char **argv)
{

    int opt;
    // port: Puerto en el que escuchamos por datagramas entrantes
    char *file = NULL, *ip = NULL;

    // Comprueba que exista al menos un operando
    // En caso de error salimos de la función main con el codigo EXIT_FAILURE
    if (argc < 2)
    {
        printf("Falta un operando\n");
        printf("Usar: %s [-f tabla de reenvio] [-h dirección IP del host]\n", argv[0]);
        return (EXIT_FAILURE);
    }

    // La funcion getopt() permite de forma facil manejar operandos en linea de comandos
    // Las opciones n: s: i: p: indican que esos "flags" (nsip) deben de ir seguidos de un argumento
    // Ese parametro se guarda en la variable externa optarg
    while ((opt = getopt(argc, argv, "f:h:")) != -1)
    {
        switch (opt)
        {
        case 'f':
            file = optarg; // Argumento numero de puerto
            break;
        case 'h':
            ip = optarg; // Argumento numero de puerto
            break;
        case ':': // Se introdujo un flag sin argumento obligatorio
            fprintf(stderr, "La opción -%c requiere un argumento.\n", optopt);
            return (EXIT_FAILURE);
            break;
        case '?':
            if (isprint(optopt)) // Se introdujo un flag incorrecto
                fprintf(stderr, "Opción desconocida `-%c'.\n", optopt);
            else // Hay un caracter no legible en las opciones
                fprintf(stderr, "Caracter de opción desconocido `\\x%x'.\n", optopt);
            return (EXIT_FAILURE);
            break;
        default: // Se produjo un error indeterminado. Nunca se deberia llegar aqui.
            abort();
        }
    }

    if (file && ip)
    {
        if (determinarInterfaz(file, ip) != EXIT_SUCCESS)
        {
            fprintf(stderr, "La función determinarInterfaz ha tenido un error.\n");
        }
    }
    else
    {
        fprintf(stderr, "Falta algún parámetro requerido.\n");
    }
}

int determinarInterfaz(char *file, char *ip)
{
    FILE *fp;
    struct in_addr ipUsuario;
    struct in_addr redActual;
    struct in_addr mejorRed;
    char entradaActualTabla[50];
    char redIp[INET_ADDRSTRLEN];
    char red[INET_ADDRSTRLEN];
    int prefijo;
    int mejorPrefijo = 0;
    int interfazActual;
    int mejorInterfaz = 0;

    fp = fopen(file, "r");
    if (!fp)
    {
        fprintf(stderr, "Error abriendo el archivo.\n");
        return EXIT_FAILURE;
    }

    if (inet_pton(AF_INET, ip, &ipUsuario) != 1)
    {
        fprintf(stderr, "Error en inet_pton.\n");
        return EXIT_FAILURE;
    }

    while (!feof(fp))
    {
        fscanf(fp, " %[^,] , %d", entradaActualTabla, &interfazActual);

        prefijo = inet_net_pton(AF_INET, entradaActualTabla, &redActual, sizeof(struct in_addr)); // Convertimos la entrada de la tabla
        if (prefijo < 0)
        { // Hubo un error
            fprintf(stderr, "Error en inet_net_pton.\n");
            return EXIT_FAILURE;
        }

        if (!inet_net_ntop(AF_INET, &redActual, prefijo, red, INET_ADDRSTRLEN)) {
            fprintf(stderr, "Error en inet_net_ntop.\n");
            return EXIT_FAILURE;
        }
        if (!inet_net_ntop(AF_INET, &ipUsuario, prefijo, redIp, INET_ADDRSTRLEN)) {
            fprintf(stderr, "Error en inet_net_ntop.\n");
            return EXIT_FAILURE;
        }

        if ((strncmp( red, redIp, INET_ADDRSTRLEN)==0) && (prefijo > mejorPrefijo)) {
            mejorRed = redActual;
            mejorPrefijo = prefijo;
            mejorInterfaz = interfazActual;
        }
    }

        if (!inet_net_ntop(AF_INET, &mejorRed, mejorPrefijo, red, INET_ADDRSTRLEN)) {
            fprintf(stderr, "Error en inet_net_ntop.\n");
            return EXIT_FAILURE;
        }

    printf("Interfaz: " ANSI_COLOR_BLUE "%d" ANSI_COLOR_RESET "\nRed: " ANSI_COLOR_YELLOW "%s" ANSI_COLOR_RESET "\n", mejorInterfaz, red);

    return EXIT_SUCCESS;
}
