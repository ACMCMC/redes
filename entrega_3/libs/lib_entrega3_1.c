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
     _______. _______ .__   __.  _______   _______ .______      
    /       ||   ____||  \ |  | |       \ |   ____||   _  \     
   |   (----`|  |__   |   \|  | |  .--.  ||  |__   |  |_)  |    
    \   \    |   __|  |  . `  | |  |  |  ||   __|  |      /     
.----)   |   |  |____ |  |\   | |  '--'  ||  |____ |  |\  \----.
|_______/    |_______||__| \__| |_______/ |_______|| _| `._____|                                                

Realiza el funcionamiento del host que envía un mensaje por UDP
    puerto: el puerto desde el que enviamos el datagrama
    mensaje_enviar: el string que queremos enviar (si no lo especificamos, se envía uno por defecto)
    ip_envio: la dirección IP del host al que queremos enviar el datagrama
    puerto_receptor: el puerto donde escucha por datagramas entrantes el host que recibe
*/

int sender(char *puerto_propio, char *mensaje_enviar, char *ip_receptor, char *puerto_receptor)
{
    int socket_propio_envio;                           // El número del socket que vamos a abrir para realizar el envío
    struct sockaddr_in socket_propio, socket_receptor; // Structs que usaremos para almacenar las direcciones de sender y receptor
    ssize_t bytes_enviados;                            // Lo usaremos para llevar la cuenta de cuántos bytes hemos enviado

    printf("Este host es el que envía. Vamos a enviar el mensaje a " ANSI_COLOR_BLUE "%s" ANSI_COLOR_RESET ":" ANSI_COLOR_CYAN "%s" ANSI_COLOR_RESET ".\n", ip_receptor, puerto_receptor);

    socket_propio_envio = socket(AF_INET, SOCK_DGRAM, 0); // Abrimos el socket
    if (socket_propio_envio < 0)                          // Comprobamos si hubo error
    {
        perror("Error al crear el socket");
    }

    memset(&socket_receptor, 0, sizeof(struct sockaddr_in)); // Llenamos la estructura de 0s
    socket_receptor.sin_family = AF_INET;                    // IPv4
    socket_receptor.sin_port = htons(atoi(puerto_receptor)); // El puerto_receptor estaba como una cadena de caracteres ASCII, lo convertimos a entero y en orden de red

    memset(&socket_propio, 0, sizeof(struct sockaddr_in)); // Llenamos la estructura de 0s
    socket_propio.sin_family = AF_INET;                    // IPv4
    socket_propio.sin_port = htons(atoi(puerto_propio));   // El puerto_propio estaba como una cadena de caracteres ASCII, lo convertimos a entero y en orden de red
    socket_propio.sin_addr.s_addr = htonl(INADDR_ANY);     // Debe ponerse INADDR_ANY para que pueda aceptar conexiones a través de cualquiera de las interfaces del mismo

    if (bind(socket_propio_envio, (struct sockaddr *)&socket_propio, sizeof(struct sockaddr_in)) < 0) // Le asignamos dirección al socket
    {
        perror("No se ha podido asignar la dirección al socket");
        return (EXIT_FAILURE);
    }

    if (mensaje_enviar == NULL)
    {
        mensaje_enviar = "Mensaje de prueba";
        fprintf(stderr, "No se ha especificado un mensaje para enviar las respuestas. Usando \"%s\" como mensaje.\n", mensaje_enviar);
    }

    if (inet_ntop(socket_receptor.sin_family, (void *)&(socket_receptor.sin_addr), ip_receptor, INET_ADDRSTRLEN) == NULL)
    {
        close(socket_propio_envio); // Hubo un error, abortamos. Cerramos los sockets antes de salir
        perror("Error en inet_ntop");
        return (EXIT_FAILURE);
    }

    bytes_enviados = sendto(socket_propio_envio, mensaje_enviar, (strlen(mensaje_enviar) + 1) * sizeof(char), 0, (struct sockaddr *)&socket_receptor, sizeof(struct sockaddr_in)); // Enviamos usando el socket_propio_envio el mensaje especificado, con su tamaño +1 (para el '\0'), sin flags especificados, al socket remoto que especifica el struct que pasamos

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
.______       _______   ______  _______ .______   .___________.  ______   .______      
|   _  \     |   ____| /      ||   ____||   _  \  |           | /  __  \  |   _  \     
|  |_)  |    |  |__   |  ,----'|  |__   |  |_)  | `---|  |----`|  |  |  | |  |_)  |    
|      /     |   __|  |  |     |   __|  |   ___/      |  |     |  |  |  | |      /     
|  |\  \----.|  |____ |  `----.|  |____ |  |          |  |     |  `--'  | |  |\  \----.
| _| `._____||_______| \______||_______|| _|          |__|      \______/  | _| `._____|

Realiza el funcionamiento del host que recibe un mensaje por UDP
    puerto: el puerto donde escuchamos por datagramas entrantes
*/

int receptor(char *puerto)
{

    int id_socket_propio;                            // El número de socket que escuchará conexiones
    struct sockaddr_in socket_sender, socket_propio; // socket_sender guardará la dirección del que envía el mensaje; socket_propio guarda la del socket de este host
    socklen_t tam_socket_sender;                     // Parámetro de entrada y salida a recvfrom()
    char mensaje_recibido[MAX_TAM_MSG];              // Aquí guardaremos el mensaje que recibimos
    ssize_t bytes_recibidos;                         // Lo usaremos para saber la longitud del mensaje recibido
    char *ip_sender = NULL;                          // Para guardar la IP del que envía el mensaje como cadena de caraceteres y mostrarla al usuario

    printf("Este host es el que recibe. Vamos a escuchar en el puerto " ANSI_COLOR_CYAN "%s" ANSI_COLOR_RESET ".\n", puerto);

    id_socket_propio = socket(AF_INET, SOCK_DGRAM, 0); // Creamos un socket para escuchar por conexiones entrantes

    if (id_socket_propio < 0)
    {
        perror("Error al crear el socket");
        return (EXIT_FAILURE);
    }

    memset(&socket_propio, 0, sizeof(struct sockaddr_in)); // Llenamos de 0s la estructura del socket propio
    socket_propio.sin_family = AF_INET;                    // IPv4
    socket_propio.sin_port = htons(atoi(puerto));          // El puerto de escucha
    socket_propio.sin_addr.s_addr = INADDR_ANY;            // Escuchamos por cualquier interfaz

    if (bind(id_socket_propio, (struct sockaddr *)&socket_propio, sizeof(socket_propio)) < 0) // Asignamos el puerto que nos pasaron como argumento al socket que declaramos antes
    {
        perror("No se ha podido asignar la dirección al socket");
        return (EXIT_FAILURE);
    }

    memset(&socket_sender, 0, sizeof(struct sockaddr_in)); // Inicializamos la estructura que guardará la dirección del que envía a todo 0s
    tam_socket_sender = sizeof(struct sockaddr_in);        // El tamaño de la estructura que guarda la dirección del que envía. No es realmente necesaria la variable, ya que podríamos poner el valor directamente en recvfrom(), pero creo que es más claro declararlo de forma explícita

    bytes_recibidos = recvfrom(id_socket_propio, mensaje_recibido, sizeof(mensaje_recibido), 0, (struct sockaddr *)&socket_sender, &tam_socket_sender); // Recibimos un mensaje a través del socket que hemos abierto, con los parámetros:
    // id_socket_propio: la ID del socket donde recibimos el mensaje (el que abrimos antes)
    // mensaje_recibido: puntero al buffer donde guardamos el mensaje
    // sizeof(mensaje_recibido): el tamaño del buffer
    // 0: no queremos especificar flags
    // &socket_sender: puntero a un struct para guardar la dirección del que envía
    // &tam_socket_sender: parámetro de entrada (tamaño del struct cuando lo pasamos a la función) y de salida (tamaño del struct que nos devuelve)

    close(id_socket_propio); // Cerramos el socket de conexión al servidor

    if (bytes_recibidos < 0) // Miramos si hubo error recibiendo el mensaje
    {
        perror("Error al recibir el mensaje");
        return (EXIT_FAILURE);
    }

    ip_sender = (char *)realloc(ip_sender, (socket_sender.sin_family == AF_INET6) ? sizeof(char) * INET6_ADDRSTRLEN : sizeof(char) * INET_ADDRSTRLEN);                                                        // Guardamos espacio para la IP en formato string, dependiendo de si es IPv4 o IPv6 (en esta práctica sólo usamos IPv4, pero así es más general)
    if (inet_ntop(socket_sender.sin_family, (void *)&(socket_sender.sin_addr), ip_sender, (socket_sender.sin_family == AF_INET6) ? sizeof(char) * INET6_ADDRSTRLEN : sizeof(char) * INET_ADDRSTRLEN) == NULL) // Guardamos la IP del que envía en ip_sender, como string
    {
        perror("Error en inet_ntop");
        return (EXIT_FAILURE);
    }

    printf(ANSI_COLOR_GREEN "Recibidos %zd bytes de " ANSI_COLOR_BLUE "%s" ANSI_COLOR_RESET ":" ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET ": %s\n", bytes_recibidos, ip_sender, ntohs(socket_sender.sin_port), mensaje_recibido); // Imprimimos IP, puerto y mensaje

    free(ip_sender); // Liberamos el string de la IP del que envía

    return (EXIT_SUCCESS);
}
