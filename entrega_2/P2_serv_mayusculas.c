#include "libs/lib_entrega2_2.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

/*
Este programa es el programa del lado del servidor para el envío de una serie de caracteres en mayúsculas.
*/
int main(int argc, char **argv)
{

    int opt;
    // port: Puerto
    // direccion: IP del servidor. Solo se usa si soy un cliente
    // mensaje_enviar: Si soy un servidor, qué mensaje enviarle a los que se me conectan
    char *port = NULL, *file = NULL;

    // Comprueba que exista al menos un operando
    // En caso de error salimos de la función main con el codigo EXIT_FAILURE
    if (argc < 2)
    {
        printf("Falta un operando\n");
        printf("Usar: %s [-f El nombre del archivo de texto con las mayúsculas a enviar] [-p Numero de puerto, tanto si es servidor como cliente] [-h dirección del servidor, en caso de ser un cliente]\n", argv[0]);
        return (EXIT_FAILURE);
    }

    // La funcion getopt() permite de forma facil manejar operandos en linea de comandos
    // Las opciones n: s: i: p: indican que esos "flags" (nsip) deben de ir seguidos de un argumento
    // Ese parametro se guarda en la variable externa optarg
    while ((opt = getopt(argc, argv, "f:p:")) != -1)
    {
        switch (opt)
        {
        case 'f':
            file = optarg; // El host es un cliente
            break;
        case 'p':
            port = optarg; // IP del servidor. Solo se usa si soy un cliente
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

    if (port && file) {
        
    } else {
        fprintf(stderr, "No se han especificado todos los parámetros necesarios.\n");
    }
}