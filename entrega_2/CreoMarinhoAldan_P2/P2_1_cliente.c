#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

#define MAX_CLIENTES_SERV 5
#define MAX_TAM_MSG 100 // El tamaño máximo del mensaje, tanto en cliente como en servidor

int cliente(char *puerto, char *direccion);

int main(int argc, char **argv)
{

    int opt;
    // port: Puerto
    // direccion: IP del servidor. Solo se usa si soy un cliente
    // mensaje_enviar: Si soy un servidor, qué mensaje enviarle a los que se me conectan
    char *port = NULL, *direccion = NULL;

    // Comprueba que exista al menos un operando
    // En caso de error salimos de la función main con el codigo EXIT_FAILURE
    if (argc < 2)
    {
        printf("Falta un operando\n");
        printf("Usar: %s [-h dirección del servidor] [-p numero de puerto del servidor]\n", argv[0]);
        return (EXIT_FAILURE);
    }

    // La funcion getopt() permite de forma facil manejar operandos en linea de comandos
    // Las opciones n: s: i: p: indican que esos "flags" (nsip) deben de ir seguidos de un argumento
    // Ese parametro se guarda en la variable externa optarg
    while ((opt = getopt(argc, argv, "h:p:")) != -1)
    {
        switch (opt)
        {
        case 'p':
            port = optarg; // Argumento numero de puerto
            break;
        case 'h':
            direccion = optarg; // IP del servidor. Solo se usa si soy un cliente
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
        if (direccion)
        {
            printf("Este host es un cliente. Vamos a conectarnos al servidor.\n");
            if (cliente(port, direccion) != EXIT_SUCCESS)
            {
                fprintf(stderr, "Error ejecutando cliente\n");
            }
        }
        else
        {
            fprintf(stderr, "Es necesario especificar una IP para realizar el envío.\n");
        }
    }
    else
    {
        fprintf(stderr, "Es necesario especificar el puerto.\n");
    }
}

/*
Esta función se conecta al servidor con el puerto y la dirección que se le pasan en formato texto, e imprime por pantalla lo que recibe
*/
int cliente(char *puerto, char *direccion)
{

    int socket_servidor;                // El número de socket para la conexión con el servidor
    struct sockaddr_in direccion_envio; // Esto lo usaremos para conectarnos con el otro host (el servidor). La dirección la sacaremos de los argumentos de la función.
    char mensaje_recibido[MAX_TAM_MSG]; // Aquí guardaremos el mensaje que recibimos
    int error_check;                    // La usaremos para comprobar códigos de error
    ssize_t bytes_recibidos;            // Lo usaremos para saber la longitud del mensaje recibido

    socket_servidor = socket(AF_INET, SOCK_STREAM, 0); // Creamos un socket para conectarnos al servidor

    if (socket_servidor < 0)
    {
        perror("Error al crear el socket");
        return (EXIT_FAILURE);
    }

    memset(&direccion_envio, 0, sizeof(direccion_envio));
    direccion_envio.sin_family = AF_INET;
    direccion_envio.sin_port = htons(atoi(puerto));

    if ((error_check = inet_pton(AF_INET, direccion, &(direccion_envio.sin_addr))) != 1) // Convertimos la ip de formato texto a formato máquina
    {
        if (error_check == -1)
        {
            perror("Error procesando la dirección IP");
        }
        else if (error_check == 0)
        {
            fprintf(stderr, "La dirección IP especificada no es válida.\n");
        }
        return (EXIT_FAILURE);
    }

    if (connect(socket_servidor, (struct sockaddr *)&direccion_envio, sizeof(direccion_envio)) != 0) // Nos conectamos al servidor y comprobamos que todo fue correctamente
    {
        perror("Error al conectarse al servidor");
        close(socket_servidor); // Cerramos el socket de conexión al servidor
        return (EXIT_FAILURE);
    }

    bytes_recibidos = recv(socket_servidor, mensaje_recibido, sizeof(char) * MAX_TAM_MSG, 0); // Recibimos el mensaje. socket_servidor: identificador del socket, mensaje_recibido: puntero al string donde guardaremos la respuesta, el tercer parámetro es el tamaño máximo del mensaje que podemos guardar en esa cadena, y el 4o es 0 porque no queremos especificar ningún flag

    if (bytes_recibidos < 0) // Miramos si hubo error recibiendo el mensaje
    {
        perror("Error al recibir el mensaje");
        close(socket_servidor); // Cerramos el socket de conexión al servidor
        return (EXIT_FAILURE);
    }

    close(socket_servidor); // Cerramos el socket de conexión al servidor

    // No hace falta liberar memoria

    printf("Recibidos %zd bytes: %s\n", bytes_recibidos, mensaje_recibido);

    return (EXIT_SUCCESS);
}
