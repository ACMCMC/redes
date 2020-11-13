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

int serv_mayusculas(char *puerto);

int main(int argc, char **argv)
{

    int opt;
    // port: Puerto
    char *port = NULL;

    // Comprueba que exista al menos un operando
    // En caso de error salimos de la función main con el codigo EXIT_FAILURE
    if (argc < 2)
    {
        printf("Falta un operando\n");
        printf("Usar: %s [-p, puerto del servidor]\n", argv[0]);
        return (EXIT_FAILURE);
    }

    // La funcion getopt() permite de forma facil manejar operandos en linea de comandos
    // Las opciones n: s: i: p: indican que esos "flags" (nsip) deben de ir seguidos de un argumento
    // Ese parametro se guarda en la variable externa optarg
    while ((opt = getopt(argc, argv, "p:")) != -1)
    {
        switch (opt)
        {
        case 'p':
            port = optarg; // Argumento numero de puerto
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
    if (port)
    {
        if (serv_mayusculas(port) == EXIT_SUCCESS)
        {
            exit(EXIT_SUCCESS);
        }
        else
        {
            fprintf(stderr, "Error en la función del servidor de mayúsculas. No ha devuelto EXIT_SUCCESS.\n");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        fprintf(stderr, "Debe especificarse un puerto\n");
        exit(EXIT_FAILURE);
    }
}

/*
Función que realiza el lado del servidor del apartado 2 de la práctica.
puerto: el puerto en el que queremos escuchar por conexiones entrantes
*/
int serv_mayusculas(char *puerto)
{
    int socket_servidor, socket_conexion, i;                  // Identificadores de los sockets y una variable auxiliar i
    ssize_t bytes_recibidos, bytes_enviados;                  // Lo guardaremos para llevar información de contabilidad, lo imprimiremos por consola
    int num_clientes = 0;                                     // El número de clientes que son atendidos
    struct sockaddr_in direccion_servidor, direccion_cliente; // Aquí guardamos las direcciones tanto de cliente como servidor
    socklen_t tam_direccion_cliente;                          // Aquí guardaremos el tamaño de la dirección del cliente, que será de entrada y salida a accept()
    char mensaje_recibido[MAX_TAM_MSG];                       // El mensaje en minúsculas
    char mensaje_procesado[MAX_TAM_MSG];                      // El mensaje en mayúsculas
    char *ip_cliente = NULL;                                  // La IP del cliente en formato humano

    socket_servidor = socket(AF_INET, SOCK_STREAM, 0); // Creamos el socket de escucha de conexiones
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

    if (listen(socket_servidor, 5) != 0) // Marcamos el socket como pasivo, para que pueda escuchar conexiones de clientes. Se pueden mantener hasta 5 en cola de espera.
    {
        perror("Error abriendo el socket para escucha");
        close(socket_servidor);
        return (EXIT_FAILURE);
    }

    tam_direccion_cliente = sizeof(struct sockaddr_in); // Parámetro de entrada a accept()

    while (num_clientes < MAX_CLIENTES_SERV) // Vamos a contestar en total solo 5 clientes, este es un valor arbitrario
    {

        socket_conexion = accept(socket_servidor, (struct sockaddr *)&direccion_cliente, &tam_direccion_cliente); // socket_servidor es el identificador del socket donde escuchamos conexiones, &direccion_cliente es un puntero a un struct sockaddr donde guardaremos la dirección del cliente que se conecta, &tam_direccion_cliente es un parámetro de entrada y salida a la función que nos indica (como entrada) el tamaño de la estructura direccion_cliente (es un sockaddr_in), y que como salida nos indica el tamaño que se ha consumido realmente

        if (socket_conexion < 0) // Hubo un error, abortamos
        {
            perror("Error aceptando la conexión");
            close(socket_conexion); // Cerramos los sockets antes de salir
            close(socket_servidor);
            return (EXIT_FAILURE);
        }

        ip_cliente = (char *)realloc(ip_cliente, (direccion_cliente.sin_family == AF_INET6) ? sizeof(char) * INET6_ADDRSTRLEN : sizeof(char) * INET_ADDRSTRLEN);                                                               // Guardamos espacio para la IP del cliente en formato texto
        if (inet_ntop(direccion_cliente.sin_family, (void *)&(direccion_cliente.sin_addr), ip_cliente, (direccion_cliente.sin_family == AF_INET6) ? sizeof(char) * INET6_ADDRSTRLEN : sizeof(char) * INET_ADDRSTRLEN) == NULL) // Convertimos la IP del cliente a formato texto
        {
            close(socket_conexion); // Hubo un error, abortamos. Cerramos los sockets antes de salir
            close(socket_servidor);
            perror("Error en inet_ntop");
            return (EXIT_FAILURE);
        }
        printf("Conectado el cliente %s al puerto %d\n", ip_cliente, ntohs(direccion_cliente.sin_port)); // Mensaje por consola

        while ((bytes_recibidos = recv(socket_conexion, mensaje_recibido, sizeof(char) * MAX_TAM_MSG, 0)) > 0)
        {

            for (i = 0; i < (int)strnlen(mensaje_recibido, MAX_TAM_MSG); i++) // Pasamos el mensaje a mayúsculas
            {
                mensaje_procesado[i] = toupper(mensaje_recibido[i]);
            }
            mensaje_procesado[i] = '\0'; // Introducimos el terminador de cadena en el mensaje procesado

            bytes_enviados = send(socket_conexion, mensaje_procesado, sizeof(char) * (strnlen(mensaje_procesado, MAX_TAM_MSG) + 1), 0); // Enviamos el mensaje al cliente. El mensaje es la concatenación de la ip en formato texto, ": ", y el mensaje que se pasó como argumento a la función

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

    free(ip_cliente);

    return (EXIT_SUCCESS);
}
