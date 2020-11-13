#include "libs/lib_entrega3_1.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

int main(int argc, char **argv)
{

    int opt;
    // puerto_propio: Puerto donde montar el socket de envío
    // ip_receptor: IP del receptor
    // mensaje_enviar: Es opcional, si se especifica, se enviará este mensaje
    char *puerto_propio = NULL, *mensaje_enviar = NULL, *puerto_receptor = NULL, *ip_receptor = NULL;

    // Comprueba que exista al menos un operando
    // En caso de error salimos de la función main con el codigo EXIT_FAILURE
    if (argc < 2)
    {
        printf("Falta un operando\n");
        printf("Usar: %s [-p numero de puerto_propio propio] [-r numero de puerto_propio del receptor] [-h IP del receptor] [-m el mensaje que enviar]\n", argv[0]);
        return (EXIT_FAILURE);
    }

    // La funcion getopt() permite de forma facil manejar operandos en linea de comandos
    // Las opciones n: s: i: p: indican que esos "flags" (nsip) deben de ir seguidos de un argumento
    // Ese parametro se guarda en la variable externa optarg
    while ((opt = getopt(argc, argv, "p:m:r:h:")) != -1)
    {
        switch (opt)
        {
        case 'm':
            mensaje_enviar = optarg; // Si soy un servidor, qué mensaje enviarle a los que se me conectan
            break;
        case 'p':
            puerto_propio = optarg; // Argumento numero de puerto_propio
            break;
        case 'r':
            puerto_receptor = optarg; // Argumento numero de puerto_propio
            break;
        case 'h':
            ip_receptor = optarg; // Argumento numero de puerto_propio
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

    if (puerto_propio && puerto_receptor && ip_receptor)
    {
        // Ejecutamos la función de envío, que se encuentra definida en su propia librería.
        if (sender(puerto_propio, mensaje_enviar, ip_receptor, puerto_receptor) != EXIT_SUCCESS)
        {
            fprintf(stderr, "Error ejecutando la función de envío\n");
        }
    }
    else
    {
        fprintf(stderr, "Es necesario especificar los parámetros requeridos.\n");
    }
}
