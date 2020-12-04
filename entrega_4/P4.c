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

#define LONG_ADDR_CON_PREFIJO (INET_ADDRSTRLEN + 3)

// Aldán Creo Mariño, Redes, 2020/21

int determinarInterfaz(char *file, char *ip);

int main(int argc, char **argv)
{

    int opt;
    // file: el nombre del archivo de la tabla de reenvío
    // ip: la dirección IP que escribe el usuario
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

    if (file && ip) // Comprobamos que se hayan establecido los dos parámetros
    {
        if (determinarInterfaz(file, ip) != EXIT_SUCCESS)
        {
            fprintf(stderr, "La función determinarInterfaz ha tenido un error.\n");
        }
    }
    else
    {
        fprintf(stderr, "Falta algún parámetro requerido.\n");
        fprintf(stderr, "Usar: %s [-f tabla de reenvio] [-h dirección IP del host]\n", argv[0]);
    }
}

int determinarInterfaz(char *file, char *ip)
{
    FILE *fp;                          // Puntero al archivo de la tabla de reenvío
    struct in_addr ipUsuario;          // La IP que escribe el usuario, pero en formato binario
    struct in_addr redActual;          // La red que estamos leyendo de la tabla de reenvío, en formato binario
    struct in_addr mejorRed;           // La mejor red para el reenvío, será la solución global al final
    char entradaActualTabla[50];       // Una entrada de la tabla. He elegido un tamaño arbitrario, aunque se podría ajustar bastante más
    char redIp[LONG_ADDR_CON_PREFIJO]; // La parte de red de la IP que escribió el usuario, truncada según el sufijo de la red con que la que la estemos comparando
    char red[LONG_ADDR_CON_PREFIJO];   // La red que estamos leyendo de la tabla de reenvío, en formato presentación
    int prefijo;                       // El prefijo de la red que estamos leyendo de la tabla de reenvío
    int mejorPrefijo = 0;              // El mejor prefijo global, en principio será 0, y se modificará cuando encontremos una concidencia
    int interfazActual;                // La interfaz que estamos leyendo de la tabla de reenvío
    int mejorInterfaz = 0;             // La mejor interfaz, por defecto será 0

    fp = fopen(file, "r"); // Abrimos el archivo que especificó el usuario
    if (!fp)
    {
        fprintf(stderr, "Error abriendo el archivo.\n");
        return EXIT_FAILURE;
    }

    if (inet_pton(AF_INET, "0.0.0.0", &mejorRed) != 1) // Inicializamos mejorRed a 0
    {
        fprintf(stderr, "Error en inet_pton.\n");
        return EXIT_FAILURE;
    }

    if (inet_pton(AF_INET, ip, &ipUsuario) != 1) // Convertimos la red que pasó el usuario a formato binario
    {
        fprintf(stderr, "Error en inet_pton.\n");
        return EXIT_FAILURE;
    }

    while (!feof(fp)) // Vamos leyendo toda la tabla de reenvío
    {
        fscanf(fp, " %[^,] , %d", entradaActualTabla, &interfazActual); // Leemos una línea de la tabla

        prefijo = inet_net_pton(AF_INET, entradaActualTabla, &redActual, sizeof(struct in_addr)); // Convertimos la entrada de la tabla a formato binario; nos quedamos con el prefijo
        if (prefijo < 0)
        { // Hubo un error
            fprintf(stderr, "Error en inet_net_pton.\n");
            return EXIT_FAILURE;
        }

        if (prefijo > mejorPrefijo)
        { // Sólo comprobamos esta entrada de la tabla si la red tiene un prefijo mayor (si no lo es, la solución sería peor)
            if (!inet_net_ntop(AF_INET, &redActual, prefijo, red, LONG_ADDR_CON_PREFIJO))
            { // Convertimos la dirección de red a formato presentación, esto es para que por ejemplo si una entrada de la tabla es "0.0.0.0/0, entonces se convierta a 0/0". Así podremos compararla con la IP del usuario, ya que tendrán el mismo formato
                fprintf(stderr, "Error en inet_net_ntop.\n");
                return EXIT_FAILURE;
            }
            if (!inet_net_ntop(AF_INET, &ipUsuario, prefijo, redIp, LONG_ADDR_CON_PREFIJO))
            { // Convertimos la IP guardada en binario a formato presentación, pero aquí la truncamos al prefijo de la red con que la comparamos, por lo que si la IP está dentro de la red entonces serán la misma cadena de caracteres.
                fprintf(stderr, "Error en inet_net_ntop.\n");
                return EXIT_FAILURE;
            }

            if (strncmp(red, redIp, LONG_ADDR_CON_PREFIJO) == 0)
            { // Comparamos si la IP está dentro de la red; si lo está, nos quedamos con esta como solución (al menos por ahora).
                mejorRed = redActual;
                mejorPrefijo = prefijo;
                mejorInterfaz = interfazActual;
            }
        }
    }

    if (!inet_net_ntop(AF_INET, &mejorRed, mejorPrefijo, red, LONG_ADDR_CON_PREFIJO))
    { // Convertimos la mejor red a formato presentación
        fprintf(stderr, "Error en inet_net_ntop.\n");
        return EXIT_FAILURE;
    }

    printf("Interfaz: " ANSI_COLOR_BLUE "%d" ANSI_COLOR_RESET "\nRed: " ANSI_COLOR_YELLOW "%s" ANSI_COLOR_RESET "\n", mejorInterfaz, red); // Información para el usuario

    return EXIT_SUCCESS;
}
