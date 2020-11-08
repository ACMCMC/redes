#include "libs/lib_entrega2_2.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

int main(int argc, char **argv)
{

    int opt;
    // port: Puerto
    // direccion: IP del servidor
    // arch_enviar: el archivo de líneas de texto a enviarle al servidor
    char *port = NULL, *direccion = NULL, *arch_enviar = NULL;

    // Comprueba que exista al menos un operando
    // En caso de error salimos de la función main con el codigo EXIT_FAILURE
    if (argc < 4)
    {
        printf("Falta un operando\n");
        printf("Usar: %s [-f, nombre del archivo de texto a enviar] [-h, dirección del servidor] [-p, puerto del servidor]\n", argv[0]);
        return (EXIT_FAILURE);
    }

    // La funcion getopt() permite de forma facil manejar operandos en linea de comandos
    // Las opciones n: s: i: p: indican que esos "flags" (nsip) deben de ir seguidos de un argumento
    // Ese parametro se guarda en la variable externa optarg
    while ((opt = getopt(argc, argv, "p:h:f:")) != -1)
    {
        switch (opt)
        {
        case 'f':
            arch_enviar = optarg; // El archivo de texto
            break;
        case 'p':
            port = optarg; // Argumento numero de puerto
            break;
        case 'h':
            direccion = optarg; // IP del servidor
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
    if (direccion && port && arch_enviar)
    {
        if (cliente_mayusculas(arch_enviar, direccion, port) == EXIT_SUCCESS)
        {
            exit(EXIT_SUCCESS);
        }
        else
        {
            fprintf(stderr, "Error en la función del cliente de mayúsculas. No ha devuelto EXIT_SUCCESS.\n");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        fprintf(stderr, "Argumentos mal especificados.\n");
        exit(EXIT_FAILURE);
    }
}
