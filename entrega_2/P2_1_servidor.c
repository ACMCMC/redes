#include "libs/lib_entrega2_1.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

int main(int argc, char **argv)
{

    int opt;
    // port: Puerto
    // direccion: IP del servidor. Solo se usa si soy un cliente
    // mensaje_enviar: Si soy un servidor, qué mensaje enviarle a los que se me conectan
    char *port = NULL, *mensaje_enviar = NULL;

    // Comprueba que exista al menos un operando
    // En caso de error salimos de la función main con el codigo EXIT_FAILURE
    if (argc < 2)
    {
        printf("Falta un operando\n");
        printf("Usar: %s [-p, numero de puerto] [-m, el mensaje con el que responder]\n", argv[0]);
        return (EXIT_FAILURE);
    }

    // La funcion getopt() permite de forma facil manejar operandos en linea de comandos
    // Las opciones n: s: i: p: indican que esos "flags" (nsip) deben de ir seguidos de un argumento
    // Ese parametro se guarda en la variable externa optarg
    while ((opt = getopt(argc, argv, "p:m:")) != -1)
    {
        switch (opt)
        {
        case 'm':
            mensaje_enviar = optarg; // Si soy un servidor, qué mensaje enviarle a los que se me conectan
            break;
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

    if (port)
    {
        printf("Este host es un servidor. Vamos a aceptar conexiones entrantes.\n");
        if (crear_servidor(port, mensaje_enviar) != EXIT_SUCCESS)
        {
            fprintf(stderr, "Error ejecutando crear_servidor\n");
        }
    }
    else
    {
        fprintf(stderr, "Es necesario especificar el puerto.\n");
    }
}
