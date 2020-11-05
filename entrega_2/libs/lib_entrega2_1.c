#include "lib_entrega2_1.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

int crear_servidor(char *puerto, char *mensaje_enviar)
{
    int socket_servidor, socket_conexion;
    struct sockaddr_in direccion_servidor, direccion_cliente;
    socklen_t tam_direccion_cliente;
    int bytes_enviados;     // Lo usaremos para llevar la cuenta de cuántos bytes hemos enviado
    int total_mensajes = 0; // Lo usaremos para contar el total de solicitudes contestadas
    char *mensaje_procesado = NULL;
    char *ip_cliente = NULL;

    socket_servidor = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_servidor < 0)
    {
        perror("Error al crear el socket");
    }

    memset(&direccion_servidor, 0, sizeof(direccion_servidor));
    direccion_servidor.sin_family = AF_INET;
    direccion_servidor.sin_port = htons(atoi(puerto));
    direccion_servidor.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socket_servidor, (struct sockaddr *)&direccion_servidor, sizeof(direccion_servidor)) < 0)
    {
        perror("No se ha podido asignar la dirección al socket");
        return (EXIT_FAILURE);
    }

    if (listen(socket_servidor, 5) != 0)
    {
        perror("Error abriendo el socket para escucha");
        return (EXIT_FAILURE);
    }

    tam_direccion_cliente = sizeof(direccion_cliente);

    while (total_mensajes < 5) // Vamos a contestar en total solo 5 solicitudes, este es un valor arbitrario
    {

        socket_conexion = accept(socket_servidor, (struct sockaddr *)&direccion_cliente, &tam_direccion_cliente);

        if (socket_conexion < 0) // Hubo un error, abortamos
        {
            perror("Error aceptando la conexión");
            close(socket_conexion); // Cerramos los sockets antes de salir
            close(socket_servidor);
            return (EXIT_FAILURE);
        }

        if (mensaje_enviar == NULL)
        {
            mensaje_enviar = "Mensaje de prueba";
            fprintf(stderr, "No se ha especificado un mensaje para enviar las respuestas. Usando \"%s\" como mensaje.", mensaje_enviar);
        }

        ip_cliente = (char *)realloc(ip_cliente, (direccion_cliente.sin_family == AF_INET6) ? sizeof(char) * INET6_ADDRSTRLEN : sizeof(char) * INET_ADDRSTRLEN);

        if (inet_ntop(direccion_cliente.sin_family, (void *)&(direccion_cliente.sin_addr), ip_cliente, (direccion_cliente.sin_family == AF_INET6) ? sizeof(char) * INET6_ADDRSTRLEN : sizeof(char) * INET_ADDRSTRLEN) == NULL)
        {
            close(socket_conexion); // Hubo un error, abortamos. Cerramos los sockets antes de salir
            close(socket_servidor);
            perror("Error en inet_ntop");
            return (EXIT_FAILURE);
        }

        printf("Dirección: %s\n", ip_cliente); // Imprimimos la IP del cliente

        mensaje_procesado = (char*) realloc(mensaje_procesado, sizeof(char) * (strlen(ip_cliente) + strlen(": ") + strlen(mensaje_enviar) + 1)); // Reservamos memoria para hacer el mensaje procesado. Reservamos la suma de la longitud de las cadenas (más 1, para el '0' del final).
        mensaje_procesado = strcat(strcat(strcpy(mensaje_procesado, ip_cliente), ": "), mensaje_enviar);
        printf("%s\n", mensaje_procesado);

        bytes_enviados = send(socket_conexion, mensaje_procesado, sizeof(char) * (strlen(mensaje_procesado) + 1), 0); // Enviamos el mensaje al cliente. El mensaje es la concatenación de la ip en formato texto, ": ", y el mensaje que se pasó como argumento a la función
        bytes_enviados = send(socket_conexion, mensaje_procesado, sizeof(char) * (strlen(mensaje_procesado) + 1), 0); // Enviamos el mensaje al cliente. El mensaje es la concatenación de la ip en formato texto, ": ", y el mensaje que se pasó como argumento a la función

        if (bytes_enviados < 0) // Hubo un error, pero no abortamos.
        {
            perror("Error enviando la respuesta");
        }

        printf("Enviados %d bytes (dos veces): %s\n", bytes_enviados, mensaje_enviar); // Info para el usuario

        close(socket_conexion); // Cerramos el socket para la conexión con el cliente

        total_mensajes++;
    }

    close(socket_servidor); // Cerramos el socket del servidor

    free(mensaje_procesado); // Liberamos memoria
    free(ip_cliente);

    return (EXIT_SUCCESS); // Todo fue bien
}

int enviar_paquete(char *puerto, char *direccion)
{

    int socket_servidor;                // El número de socket para la conexión con el servidor
    struct sockaddr_in direccion_envio; // Esto lo usaremos para conectarnos con el otro host (el servidor). La dirección la sacaremos de los argumentos de la función.
    char mensaje_recibido[1000];        // Aquí guardaremos el mensaje que recibimos
    int error_check;                    // La usaremos para comprobar códigos de error
    int bytes_recibidos;                // Lo usaremos para saber la longitud del mensaje recibido

    socket_servidor = socket(AF_INET, SOCK_STREAM, 0); // Creamos un socket para conectarnos al servidor

    if (socket_servidor < 0)
    {
        perror("Error al crear el socket");
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

    while( (bytes_recibidos=recv(socket_servidor, mensaje_recibido, sizeof(char) * 1000, 0)) > 0 ) {
        printf("Recibidos %d bytes: %s\n", bytes_recibidos, mensaje_recibido);
    }

    close(socket_servidor); // Cerramos el socket de conexión al servidor

    // No hace falta liberar memoria

    return (EXIT_SUCCESS);
}
