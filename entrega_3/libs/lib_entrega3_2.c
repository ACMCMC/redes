#include "lib_entrega3_2.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

#define MAX_TAM_MSG 100 // El tamaño máximo del mensaje, tanto en cliente como en servidor

/*
Función que realiza el lado del cliente del apartado 2 de la práctica.
file: el archivo con los caracteres a enviar
host: la IP del servidor de mayúsculas
puerto: el puerto de escucha en el servidor
*/
int cliente_mayusculas(char *file, char *host, char *puerto_propio, char* puerto_servidor)
{
    FILE *fp, *fp_out;
    int socket_servidor, error_check;
    unsigned long i;
    ssize_t bytes_enviados, bytes_recibidos;
    struct sockaddr_in direccion_envio, direccion_cliente;
    socklen_t tam_direccion;
    char linea[MAX_TAM_MSG], linea_respuesta[MAX_TAM_MSG];
    char *file_out;

    fp = fopen(file, "r");
    if (!fp)
    {
        perror("Error abriendo el archivo de lectura. Abortamos.");
        return (EXIT_FAILURE);
    }

    file_out = (char *)malloc(sizeof(char) * (strlen(file) + 1));
    for (i = 0; i <= strlen(file); i++)
    {
        file_out[i] = toupper(file[i]);
    }
    fp_out = fopen("file_out", "w");
    if (!fp)
    {
        perror("Error abriendo el archivo de escritura. Abortamos.");
        return (EXIT_FAILURE);
    }
    free(file_out);

    socket_servidor = socket(AF_INET, SOCK_DGRAM, 0); // Creamos un socket para conectarnos al servidor
    if (socket_servidor < 0)
    {
        perror("Error al crear el socket");
        fclose(fp);
        fclose(fp_out);
        return (EXIT_FAILURE);
    }

    memset(&direccion_envio, 0, sizeof(direccion_envio)); // La estructura tiene todo 0s
    direccion_envio.sin_family = AF_INET;                 // IPv4
    direccion_envio.sin_port = htons(atoi(puerto_servidor));       // El puerto estaba como una cadena de caracteres ASCII, lo convertimos a entero y en orden de red

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
        fclose(fp_out);
        close(socket_servidor);
        return (EXIT_FAILURE);
    }
    
    memset(&direccion_cliente, 0, sizeof(direccion_cliente)); // Llenamos la estructura de 0s
    direccion_cliente.sin_family = AF_INET;                    // IPv4
    direccion_cliente.sin_port = htons(atoi(puerto_propio));          // El puerto estaba como una cadena de caracteres ASCII, lo convertimos a entero y en orden de red
    direccion_cliente.sin_addr.s_addr = htonl(INADDR_LOOPBACK);     // En el caso del servidor debe ponerse INADDR_ANY para que pueda aceptar conexiones a través de cualquiera de las interfaces del mismo

    if (bind(socket_servidor, (struct sockaddr *)&direccion_cliente, sizeof(direccion_cliente)) < 0)
    {
        perror("No se ha podido asignar la dirección al socket");
        close(socket_servidor);
        return (EXIT_FAILURE);
    }


    while (!feof(fp) && (fgets(linea, MAX_TAM_MSG, fp) != NULL))
    { // Repetimos esto mientras queden líneas en el archivo
    sleep(1);
        tam_direccion = sizeof(direccion_envio);
        bytes_enviados = sendto(socket_servidor, linea, sizeof(char) * (strnlen(linea, MAX_TAM_MSG) + 1), 0, (struct sockaddr *)&direccion_envio, tam_direccion);
        // Enviamos la línea al servidor de mayúsculas. Parámetros:
        // socket_servidor: el id del socket que hemos abierto
        // linea: puntero a la cadena de caracteres que le enviamos
        // tamaño de la cadena
        // 0, porque no especificamos flags
        // puntero al struct que contiene la dirección del servidor
        // tamaño del struct

        if (bytes_enviados < 0) // Hubo un error
        {
            perror("Error enviando la línea");
            close(socket_servidor); // Cerramos el socket de conexión al servidor
            fclose(fp);
            fclose(fp_out);
            return (EXIT_FAILURE);
        }

        bytes_recibidos = recvfrom(socket_servidor, linea_respuesta, sizeof(char) * MAX_TAM_MSG, 0, (struct sockaddr *) &direccion_envio, &tam_direccion);
        // Recibimos el mensaje. Parámetros:
        // socket_servidor: identificador del socket
        // linea_respuesta: puntero al string donde guardaremos la línea en minúsculas que nos mandó el cliente, y que vamos a convertir a mayúsculas (por eso la llamo línea_respuesta, al principio es la recepción, pero posteriormente esa misma línea pero en mayúsculas va a ser con la que respondamos al cliente)
        // el tercer parámetro es el tamaño máximo del mensaje que podemos guardar en esa cadena
        // el 4o es 0 porque no queremos especificar ningún flag
        // el 5o, la dirección del servidor (struct sockaddr *)
        // el 6o, el tamaño del struct

        if (bytes_recibidos < 0) // Miramos si hubo error recibiendo el mensaje
        {
            perror("Error al recibir el mensaje");
            close(socket_servidor); // Cerramos el socket de conexión al servidor
            fclose(fp);
            fclose(fp_out);
            return (EXIT_FAILURE);
        }

        fputs(linea_respuesta, fp_out);

        if (linea_respuesta[strnlen(linea_respuesta, MAX_TAM_MSG) - 1] == '\n' && linea[strnlen(linea, MAX_TAM_MSG) - 1] == '\n')
        { // fgets también lee el carácter de nueva línea, y eso antes se lo enviamos también al servidor, pero de cara a hacer el printf revisamos si la cadena acaba en '\n', y en caso afirmativo, la reemplazamos por '\0', para que la presentación sea más bonita y no tengamos un salto extraño de línea. Si la línea que mandamos acaba en '\n' antes del '0', la que recibimos también, pero siempre es mejor asegurarse.
            linea[strnlen(linea, MAX_TAM_MSG) - 1] = '\0';
            linea_respuesta[strnlen(linea_respuesta, MAX_TAM_MSG) - 1] = '\0';
        }
        printf("\nEnviados %zd bytes: %s.\n\tRecibidos %zd bytes: %s\n", bytes_enviados, linea, bytes_recibidos, linea_respuesta); // Imprimimos los mensajes por consola
    }

    close(socket_servidor); // Cerramos el socket del servidor y los archivos
    fclose(fp);
    fclose(fp_out);

    return (EXIT_SUCCESS);
}

/*
Función que realiza el lado del servidor del apartado 2 de la práctica.
puerto: el puerto en el que queremos escuchar por conexiones entrantes
*/
int serv_mayusculas(char *puerto)
{
    int socket_servidor;                                      // Identificador del socket en el que vamos a estar a la escucha de conexiones
    int i;                                                    // Variable auxiliar
    ssize_t bytes_recibidos, bytes_enviados;                  // Lo guardaremos para llevar información de contabilidad, lo imprimiremos por consola
    struct sockaddr_in direccion_servidor, direccion_cliente; // Aquí guardamos las direcciones tanto de cliente como servidor
    socklen_t tam_direccion_cliente;                          // Aquí guardaremos el tamaño de la dirección del cliente, que será de entrada y salida a accept()
    char mensaje_recibido[MAX_TAM_MSG];                       // El mensaje en minúsculas
    char mensaje_procesado[MAX_TAM_MSG];                      // El mensaje en mayúsculas
    char *ip_cliente = NULL;                                  // La IP del cliente en formato humano

    socket_servidor = socket(AF_INET, SOCK_DGRAM, 0); // Creamos el socket de escucha de conexiones
    if (socket_servidor < 0)
    {
        perror("Error al crear el socket");
    }

    memset(&direccion_servidor, 0, sizeof(direccion_servidor)); // Llenamos la estructura de 0s
    direccion_servidor.sin_family = AF_INET;                    // IPv4
    direccion_servidor.sin_port = htons(atoi(puerto));          // El puerto estaba como una cadena de caracteres ASCII, lo convertimos a entero y en orden de red
    direccion_servidor.sin_addr.s_addr = htonl(INADDR_ANY);     // En el caso del servidor debe ponerse INADDR_ANY para que pueda aceptar conexiones a través de cualquiera de las interfaces del mismo

    if (bind(socket_servidor, (struct sockaddr *)&direccion_servidor, sizeof(direccion_servidor)) < 0)
    {
        perror("No se ha podido asignar la dirección al socket");
        close(socket_servidor);
        return (EXIT_FAILURE);
    }

    printf("Listo para recibir datagramas en el puerto %s\n", puerto);

    tam_direccion_cliente = sizeof(struct sockaddr_in); // Parámetro de entrada a accept()

    while (1) // Vamos a contestar en total solo 5 clientes, este es un valor arbitrario
    {
        bytes_recibidos = recvfrom(socket_servidor, mensaje_recibido, sizeof(char) * MAX_TAM_MSG, 0, (struct sockaddr *)&direccion_cliente, &tam_direccion_cliente);
        // Nos quedamos esperando a recibir un datagrama de cualquier cliente. Esto lo hacemos con recvfrom(), que acepta como parámetros:
        // el número del socket que hemos abierto para recibir mensajes
        // puntero al mensaje en sí
        // el tamaño máximo del mensaje
        // 0, porque no queremos especificar ningún flag en especial
        // puntero a un struct donde guardaremos información sobre el host que nos ha mandado el mensaje
        // el tamaño de ese struct, como entrada; es también un parámetro de salida que nos indicará el tamaño tras la recepción

        if (bytes_recibidos < 0)
        {
            perror("Hubo un error al recibir el mensaje.");
        }

        ip_cliente = (char *)realloc(ip_cliente, (direccion_cliente.sin_family == AF_INET6) ? sizeof(char) * INET6_ADDRSTRLEN : sizeof(char) * INET_ADDRSTRLEN);                                                               // Guardamos espacio para la IP del cliente en formato texto
        if (inet_ntop(direccion_cliente.sin_family, (void *)&(direccion_cliente.sin_addr), ip_cliente, (direccion_cliente.sin_family == AF_INET6) ? sizeof(char) * INET6_ADDRSTRLEN : sizeof(char) * INET_ADDRSTRLEN) == NULL) // Convertimos la IP del cliente a formato texto
        {
            close(socket_servidor); // Hubo un error, abortamos. Cerramos los sockets antes de salir
            perror("Error en inet_ntop");
            return (EXIT_FAILURE);
        }

        printf("Recibidos %zd bytes de %s: %s\n", bytes_recibidos, ip_cliente, mensaje_recibido); // Info para el usuario

        for (i = 0; i < (int)strnlen(mensaje_recibido, MAX_TAM_MSG); i++) // Pasamos el mensaje a mayúsculas
        {
            mensaje_procesado[i] = toupper(mensaje_recibido[i]);
        }
        mensaje_procesado[i] = '\0'; // Introducimos el terminador de cadena en el mensaje procesado

        bytes_enviados = sendto(socket_servidor, mensaje_procesado, sizeof(char) * (strnlen(mensaje_procesado, sizeof(char) * MAX_TAM_MSG) + 1), 0, (struct sockaddr *)&direccion_cliente, tam_direccion_cliente);

        if (bytes_enviados < 0) // Hubo un error, pero no abortamos.
        {
            perror("Error enviando la respuesta");
        }

        printf("Devueltos %zd bytes: %s\n", bytes_enviados, mensaje_procesado); // Info para el usuario
    }

    close(socket_servidor); // Cerramos el socket del servidor
    free(ip_cliente);

    return (EXIT_SUCCESS);
}
