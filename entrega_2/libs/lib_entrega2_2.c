#include "lib_entrega2_2.h"

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
#define MAX_TAM_MSG 100

int cliente_mayusculas(char *file, char *host, char *puerto)
{
    FILE *fp;
    int socket_servidor, error_check;
    ssize_t bytes_enviados, bytes_recibidos;
    struct sockaddr_in direccion_envio;
    char linea[MAX_TAM_MSG], linea_respuesta[MAX_TAM_MSG];

    fp = fopen(file, "r");
    if (!fp)
    {
        perror("Error abriendo el archivo. Abortamos.");
        return (EXIT_FAILURE);
    }

    socket_servidor = socket(AF_INET, SOCK_STREAM, 0); // Creamos un socket para conectarnos al servidor
    if (socket_servidor < 0)
    {
        perror("Error al crear el socket");
        fclose(fp);
        return (EXIT_FAILURE);
    }

    memset(&direccion_envio, 0, sizeof(direccion_envio));
    direccion_envio.sin_family = AF_INET;
    direccion_envio.sin_port = htons(atoi(puerto));

    if ((error_check = inet_pton(AF_INET, host, &(direccion_envio.sin_addr))) != 1) // Convertimos la ip de formato texto a formato máquina
    {
        if (error_check == -1)
        {
            perror("Error procesando la dirección IP");
        }
        else if (error_check == 0)
        {
            fprintf(stderr, "La dirección IP especificada no es válida.\n");
        }
        fclose(fp);
        return (EXIT_FAILURE);
    }

    if (connect(socket_servidor, (struct sockaddr *)&direccion_envio, sizeof(direccion_envio)) != 0) // Nos conectamos al servidor y comprobamos que todo fue correctamente
    {
        perror("Error al conectarse al servidor");
        close(socket_servidor); // Cerramos el socket de conexión al servidor
        fclose(fp);
        return (EXIT_FAILURE);
    }

    while (!feof(fp))
    { // Repetimos esto mientras queden líneas en el archivo
        fgets(linea, MAX_TAM_MSG, fp);
        if (linea[strlen(linea) - 1] == '\n') { // fgets también lee el carácter de nueva línea, así que revisamos si la cadena acaba en '\n', y en caso afirmativo, lo reemplazamos por '\0'
            linea[strlen(linea) - 1] = '\0';
        }

        bytes_enviados = send(socket_servidor, linea, sizeof(char) * (strlen(linea) + 1), 0); // Enviamos la línea de texto al servidor

        if (bytes_enviados < 0) // Hubo un error
        {
            perror("Error enviando la línea");
            close(socket_servidor); // Cerramos el socket de conexión al servidor
            fclose(fp);
            return (EXIT_FAILURE);
        }

        bytes_recibidos = recv(socket_servidor, linea_respuesta, sizeof(char) * MAX_TAM_MSG, 0); // Recibimos el mensaje

        if (bytes_recibidos < 0) // Miramos si hubo error recibiendo el mensaje
        {
            perror("Error al recibir el mensaje");
            close(socket_servidor); // Cerramos el socket de conexión al servidor
            return (EXIT_FAILURE);
        }

        printf("\nEnviados %zd bytes: %s.\n\tRecibidos %zd bytes: %s\n", bytes_enviados, linea, bytes_recibidos, linea_respuesta);
    }

    close(socket_servidor);
    fclose(fp);

    return (EXIT_SUCCESS);
}

int serv_mayusculas(char *puerto)
{
    int socket_servidor, socket_conexion, i;
    ssize_t bytes_recibidos, bytes_enviados;
    int num_clientes = 0;
    struct sockaddr_in direccion_servidor, direccion_cliente;
    socklen_t tam_direccion_cliente;
    char mensaje_recibido[MAX_TAM_MSG];
    char mensaje_procesado[MAX_TAM_MSG];
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

    while (num_clientes < MAX_CLIENTES_SERV) // Vamos a contestar en total solo 5 clientes, este es un valor arbitrario
    {

        socket_conexion = accept(socket_servidor, (struct sockaddr *)&direccion_cliente, &tam_direccion_cliente);

        if (socket_conexion < 0) // Hubo un error, abortamos
        {
            perror("Error aceptando la conexión");
            close(socket_conexion); // Cerramos los sockets antes de salir
            close(socket_servidor);
            return (EXIT_FAILURE);
        }

        ip_cliente = (char *)realloc(ip_cliente, (direccion_cliente.sin_family == AF_INET6) ? sizeof(char) * INET6_ADDRSTRLEN : sizeof(char) * INET_ADDRSTRLEN);
        if (inet_ntop(direccion_cliente.sin_family, (void *)&(direccion_cliente.sin_addr), ip_cliente, (direccion_cliente.sin_family == AF_INET6) ? sizeof(char) * INET6_ADDRSTRLEN : sizeof(char) * INET_ADDRSTRLEN) == NULL)
        {
            close(socket_conexion); // Hubo un error, abortamos. Cerramos los sockets antes de salir
            close(socket_servidor);
            perror("Error en inet_ntop");
            return (EXIT_FAILURE);
        }
        printf("Conectado el cliente %s al puerto %d\n", ip_cliente, ntohs(direccion_cliente.sin_port));

        while ((bytes_recibidos = recv(socket_conexion, mensaje_recibido, sizeof(char) * MAX_TAM_MSG, 0)) > 0)
        {

            for (i = 0; i < (int) strnlen(mensaje_recibido, MAX_TAM_MSG); i++)
            {
                mensaje_procesado[i] = toupper(mensaje_recibido[i]);
            }
            mensaje_procesado[i] = '\0'; // Introducimos el terminador de cadena en el mensaje procesado

            bytes_enviados = send(socket_conexion, mensaje_procesado, sizeof(char) * (strlen(mensaje_procesado) + 1), 0); // Enviamos el mensaje al cliente. El mensaje es la concatenación de la ip en formato texto, ": ", y el mensaje que se pasó como argumento a la función

            if (bytes_enviados < 0) // Hubo un error, pero no abortamos.
            {
                perror("Error enviando la respuesta");
            }

            printf("Enviados %zd bytes: %s\n", bytes_enviados, mensaje_procesado); // Info para el usuario
        }

        if (bytes_recibidos < 0)
        {
            perror("Hubo un error al recibir el mensaje.");
        }

        close(socket_conexion); // Cerramos el socket para la conexión con el cliente

        printf("Fin de la conexión.\n\n");

        num_clientes++;
    }

    close(socket_servidor); // Cerramos el socket del servidor

    return (EXIT_SUCCESS);
}
