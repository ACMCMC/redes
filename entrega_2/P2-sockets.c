#include "libs/lib_entrega2.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

int main(int argc, char **argv)
{

    int opt;
    // port: Puerto
    char *port = NULL, *direccion = NULL, *mensaje_enviar = NULL;
    int tipo_host = 0; // Por defecto es un cliente

    // Comprueba que exista al menos un operando
    // En caso de error salimos de la función main con el codigo EXIT_FAILURE
    if (argc < 2)
    {
        printf("Falta un operando\n");
        printf("Usar: %s [-p Numero de puerto]\n", argv[0]);
        return (EXIT_FAILURE);
    }

    // La funcion getopt() permite de forma facil manejar operandos en linea de comandos
    // Las opciones n: s: i: p: indican que esos "flags" (nsip) deben de ir seguidos de un argumento
    // Ese parametro se guarda en la variable externa optarg
    while ((opt = getopt(argc, argv, "scp:h:m:")) != -1)
    {
        switch (opt)
        {
        case 'm':
            mensaje_enviar = optarg; // El host es un servidor
            break;
        case 's':
            tipo_host = 1; // El host es un servidor
            break;
        case 'c':
            tipo_host = 0; // El host es un cliente
            break;
        case 'p':
            port = optarg; // Argumento numero de puerto
            break;
        case 'h':
            direccion = optarg; // Argumento numero de puerto
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

    if (port) {
        if (tipo_host) {
            printf("Este host es un servidor. Vamos a aceptar conexiones entrantes.\n");
            if (crear_servidor(port, mensaje_enviar) != EXIT_SUCCESS)
                fprintf(stderr, "Error ejecutando crear_servidor\n");
        } else {
            printf("Este host es un cliente. Vamos a conectarnos al servidor.\n");
            if (direccion) {
            if (enviar_paquete(port, direccion) != EXIT_SUCCESS)
                fprintf(stderr, "Error ejecutando enviar_paquete\n");
            } else {
                fprintf(stderr, "Es necesario especificar una IP para realizar el envío.\n");
            }
        }
    } else {
        fprintf(stderr, "Es necesario especificar el puerto.\n");
    }
}