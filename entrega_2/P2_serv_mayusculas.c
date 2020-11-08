#include "libs/lib_entrega2_2.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

int main(int argc, char **argv)
{

    int opt;
    // port: Puerto
    char *port = NULL;

    // Comprueba que exista al menos un operando
    // En caso de error salimos de la función main con el codigo EXIT_FAILURE
    if (argc < 2)
    {
        printf("Falta un operando\n");
        printf("Usar: %s [-p, puerto del servidor]\n", argv[0]);
        return (EXIT_FAILURE);
    }

    // La funcion getopt() permite de forma facil manejar operandos en linea de comandos
    // Las opciones n: s: i: p: indican que esos "flags" (nsip) deben de ir seguidos de un argumento
    // Ese parametro se guarda en la variable externa optarg
    while ((opt = getopt(argc, argv, "p:")) != -1)
    {
        switch (opt)
        {
        case 'p':
            port = optarg; // Argumento numero de puerto
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

    printf("\n");

    // Llamamos a la función adecuada
    if (port)
    {
        if (serv_mayusculas(port) == EXIT_SUCCESS)
        {
            exit(EXIT_SUCCESS);
        }
        else
        {
            fprintf(stderr, "Error en la función del servidor de mayúsculas. No ha devuelto EXIT_SUCCESS.\n");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        fprintf(stderr, "Debe especificarse un puerto\n");
        exit(EXIT_FAILURE);
    }
}
