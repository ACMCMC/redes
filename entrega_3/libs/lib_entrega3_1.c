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
Esta función crea un servidor en el puerto_propio indicado, para responder a todos los clientes que se conecten a él con el mensaje que se le pasa como segundo parámetro. Si el segundo parámetro apunta a NULL, entonces se responde con un mensaje genérico
*/
int sender(char *puerto_propio, char *mensaje_enviar, char *ip_receptor, char *puerto_receptor)
{
    printf("Este host es el que envía. Vamos a enviar el mensaje a " ANSI_COLOR_BLUE "%s" ANSI_COLOR_RESET ":" ANSI_COLOR_CYAN "%s" ANSI_COLOR_RESET ".\n", ip_receptor, puerto_receptor);
    int socket_propio_envio;                           // Los números de socket de servidor (escucha) y conexión con el receptor (porque estamos en TCP, así que establecemos un nuevo socket por cada conexión con cada receptor)
    struct sockaddr_in socket_propio, socket_receptor; // Structs que usaremos para almacenar las direcciones de receptor y servidor
    socklen_t tamano_sock_receptor;
    ssize_t bytes_enviados;           // Lo usaremos para llevar la cuenta de cuántos bytes hemos enviado
    char ip_cliente[INET_ADDRSTRLEN]; // Aquí guardaremos la IP del receptor, pero en formato legible por humanos

    socket_propio_envio = socket(AF_INET, SOCK_DGRAM, 0);

    if (socket_propio_envio < 0)
    {
        perror("Error al crear el socket");
    }

    memset(&socket_receptor, 0, sizeof(struct sockaddr_in)); // Llenamos la estructura de 0s
    socket_receptor.sin_family = AF_INET;                    // IPv4
    socket_receptor.sin_port = htons(atoi(puerto_receptor)); // El puerto_propio estaba como una cadena de caracteres ASCII, lo convertimos a entero y en orden de red

    memset(&socket_propio, 0, sizeof(struct sockaddr_in)); // Llenamos la estructura de 0s
    socket_propio.sin_family = AF_INET;                    // IPv4
    socket_propio.sin_port = htons(atoi(puerto_propio));   // El puerto_propio estaba como una cadena de caracteres ASCII, lo convertimos a entero y en orden de red
    socket_propio.sin_addr.s_addr = htonl(INADDR_ANY);     // En el caso del servidor debe ponerse INADDR_ANY para que pueda aceptar conexiones a través de cualquiera de las interfaces del mismo

    if (bind(socket_propio_envio, (struct sockaddr *)&socket_propio, sizeof(socket_propio)) < 0)
    {
        perror("No se ha podido asignar la dirección al socket");
        return (EXIT_FAILURE);
    }

    if (mensaje_enviar == NULL)
    {
        mensaje_enviar = "Mensaje de prueba";
        fprintf(stderr, "No se ha especificado un mensaje para enviar las respuestas. Usando \"%s\" como mensaje.\n", mensaje_enviar);
    }

    tamano_sock_receptor = sizeof(socket_receptor);

    if (inet_ntop(socket_propio.sin_family, (void *)&(socket_receptor.sin_addr), ip_cliente, INET_ADDRSTRLEN) == NULL)
    {
        close(socket_propio_envio); // Hubo un error, abortamos. Cerramos los sockets antes de salir
        perror("Error en inet_ntop");
        return (EXIT_FAILURE);
    }

    bytes_enviados = sendto(socket_propio_envio, mensaje_enviar, (strlen(mensaje_enviar) + 1) * sizeof(char), 0, (struct sockaddr *)&socket_receptor, tamano_sock_receptor); // Enviamos usando el socket_propio_envio el mensaje especificado, con su tamaño +1 (para el '\0'), sin flags especificados, al socket remoto que especifica el struct que pasamos

    if (bytes_enviados < 0) // Hubo un error, abortamos.
    {
        perror("Error enviando la respuesta");
        close(socket_propio_envio); // Hubo un error, abortamos. Cerramos los sockets antes de salir
        return (EXIT_FAILURE);
    }

    printf(ANSI_COLOR_GREEN "Enviados %zd bytes" ANSI_COLOR_RESET ": %s\n", bytes_enviados, mensaje_enviar); // Info para el usuario

    close(socket_propio_envio); // Cerramos el socket de envio
    return (EXIT_SUCCESS);      // Todo fue bien, devolvemos EXIT_SUCCESS
}

/*
Esta función se conecta al servidor con el puerto_propio y la dirección que se le pasan en formato texto, e imprime por pantalla lo que recibe
*/
int receptor(char *puerto)
{

    int id_socket_propio;                // El número de socket para la conexión con el servidor
    struct sockaddr_in socket_sender, socket_propio; // Esto lo usaremos para conectarnos con el otro host (el servidor). La dirección la sacaremos de los argumentos de la función.
    socklen_t tam_socket_sender;
    char mensaje_recibido[MAX_TAM_MSG]; // Aquí guardaremos el mensaje que recibimos
    ssize_t bytes_recibidos;            // Lo usaremos para saber la longitud del mensaje recibido

    printf("Este host es el que recibe. Vamos a escuchar en el puerto " ANSI_COLOR_CYAN "%s" ANSI_COLOR_RESET ".\n", puerto);

    id_socket_propio = socket(AF_INET, SOCK_DGRAM, 0); // Creamos un socket para conectarnos al servidor

    if (id_socket_propio < 0)
    {
        perror("Error al crear el socket");
        return (EXIT_FAILURE);
    }

    memset(&socket_propio, 0, sizeof(struct sockaddr_in));
    socket_propio.sin_family = AF_INET;
    socket_propio.sin_port = htons(atoi(puerto));
    socket_propio.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(id_socket_propio, (struct sockaddr *)&socket_propio, sizeof(socket_propio)) < 0)
    {
        perror("No se ha podido asignar la dirección al socket");
        return (EXIT_FAILURE);
    }

    memset(&socket_sender, 0, sizeof(struct sockaddr_in));
    socket_sender.sin_family = AF_INET;

    bytes_recibidos = recvfrom(id_socket_propio, mensaje_recibido, sizeof(mensaje_recibido), 0, (struct sockaddr*) &socket_sender, &tam_socket_sender);

    if (bytes_recibidos < 0) // Miramos si hubo error recibiendo el mensaje
    {
        perror("Error al recibir el mensaje");
        close(id_socket_propio); // Cerramos el socket de conexión al servidor
        return (EXIT_FAILURE);
    }

    close(id_socket_propio); // Cerramos el socket de conexión al servidor

    // No hace falta liberar memoria

    printf(ANSI_COLOR_GREEN "Recibidos %zd bytes" ANSI_COLOR_RESET ": %s\n", bytes_recibidos, mensaje_recibido);

    return (EXIT_SUCCESS);
}
