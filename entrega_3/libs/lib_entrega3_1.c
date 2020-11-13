#include "lib_entrega3_1.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#define MAX_CLIENTES_SERV 5
#define MAX_TAM_MSG 100 // El tamaño máximo del mensaje, tanto en cliente como en servidor

/*
Esta función crea un servidor en el puerto indicado, para responder a todos los clientes que se conecten a él con el mensaje que se le pasa como segundo parámetro. Si el segundo parámetro apunta a NULL, entonces se responde con un mensaje genérico
*/
int sender(char *puerto, char *mensaje_enviar, char *ip_receptor, char * puerto_receptor)
{
    printf("Este host es el que envía. Vamos a enviar el mensaje a " ANSI_COLOR_BLUE "%s" ANSI_COLOR_RESET ":" ANSI_COLOR_YELLOW "%s" ANSI_COLOR_RESET ".\n", ip_receptor, puerto_receptor);
    int socket_propio_envio;        // Los números de socket de servidor (escucha) y conexión con el cliente (porque estamos en TCP, así que establecemos un nuevo socket por cada conexión con cada cliente)
    struct sockaddr_in socket_propio, socket_remoto; // Structs que usaremos para almacenar las direcciones de cliente y servidor
    socklen_t tamano_sock_receptor;
    ssize_t bytes_enviados;         // Lo usaremos para llevar la cuenta de cuántos bytes hemos enviado
    char *ip_cliente = NULL;        // Aquí guardaremos la IP del cliente, pero en formato legible por humanos

    socket_propio_envio = socket(AF_INET, SOCK_DGRAM, 0);

    if (socket_propio_envio < 0)
    {
        perror("Error al crear el socket");
    }

    memset(&socket_remoto, 0, sizeof(struct sockaddr_in));  // Llenamos la estructura de 0s
    socket_propio.sin_family = AF_INET;                // IPv4
    socket_propio.sin_port = htons(atoi(puerto));      // El puerto estaba como una cadena de caracteres ASCII, lo convertimos a entero y en orden de red
    socket_propio.sin_addr.s_addr = htonl(INADDR_ANY); // En el caso del servidor debe ponerse INADDR_ANY para que pueda aceptar conexiones a través de cualquiera de las interfaces del mismo

    memset(&socket_propio, 0, sizeof(struct sockaddr_in));  // Llenamos la estructura de 0s
    socket_propio.sin_family = AF_INET;                // IPv4
    socket_propio.sin_port = htons(atoi(puerto));      // El puerto estaba como una cadena de caracteres ASCII, lo convertimos a entero y en orden de red
    socket_propio.sin_addr.s_addr = htonl(INADDR_ANY); // En el caso del servidor debe ponerse INADDR_ANY para que pueda aceptar conexiones a través de cualquiera de las interfaces del mismo

    if (bind(socket_propio_envio, (struct sockaddr *)&socket_propio, sizeof(socket_propio)) < 0)
    {
        perror("No se ha podido asignar la dirección al socket");
        return (EXIT_FAILURE);
    }

    if (mensaje_enviar == NULL)
    {
        mensaje_enviar = "Mensaje de prueba";
        fprintf(stderr, "No se ha especificado un mensaje para enviar las respuestas. Usando \"%s\" como mensaje.", mensaje_enviar);
    }

    tamano_sock_receptor = sizeof(socket_remoto);

    bytes_enviados = sendto(socket_propio_envio, mensaje_enviar, (strlen(mensaje_enviar) + 1)*sizeof(char), 0, (struct sockaddr*) &socket_remoto, tamano_sock_receptor);  // Enviamos usando el socket_propio_envio el mensaje especificado, con su tamaño +1 (para el '\0'), sin flags especificados, al socket remoto que especifica el struct que pasamos

    if (bytes_enviados < 0) // Hubo un error, pero no abortamos.
    {
        perror("Error enviando la respuesta");
    }

    printf("Enviados %zd bytes: %s\n", bytes_enviados, mensaje_enviar); // Info para el usuario

    close(socket_propio_envio); // Cerramos el socket del servidor
    free(ip_cliente);

    return (EXIT_SUCCESS); // Todo fue bien, devolvemos EXIT_SUCCESS
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
