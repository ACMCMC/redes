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

int servidor(char *puerto, char *mensaje_enviar);

int main(int argc, char **argv)
{

    int opt;
    // port: Puerto
    // direccion: IP del servidor. Solo se usa si soy un cliente
    // mensaje_enviar: Si soy un servidor, qué mensaje enviarle a los que se me conectan
    char *port = NULL, *mensaje_enviar = NULL;

    // Comprueba que exista al menos un operando
    // En caso de error salimos de la función main con el codigo EXIT_FAILURE
    if (argc < 2)
    {
        printf("Falta un operando\n");
        printf("Usar: %s [-p numero de puerto] [-m el mensaje con el que responder]\n", argv[0]);
        return (EXIT_FAILURE);
    }

    // La funcion getopt() permite de forma facil manejar operandos en linea de comandos
    // Las opciones n: s: i: p: indican que esos "flags" (nsip) deben de ir seguidos de un argumento
    // Ese parametro se guarda en la variable externa optarg
    while ((opt = getopt(argc, argv, "p:m:")) != -1)
    {
        switch (opt)
        {
        case 'm':
            mensaje_enviar = optarg; // Si soy un servidor, qué mensaje enviarle a los que se me conectan
            break;
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

    if (port)
    {
        printf("Este host es un servidor. Vamos a aceptar conexiones entrantes.\n");
        if (servidor(port, mensaje_enviar) != EXIT_SUCCESS)
        {
            fprintf(stderr, "Error ejecutando servidor\n");
        }
    }
    else
    {
        fprintf(stderr, "Es necesario especificar el puerto.\n");
    }
}

/*
Esta función crea un servidor en el puerto indicado, para responder a todos los clientes que se conecten a él con el mensaje que se le pasa como segundo parámetro. Si el segundo parámetro apunta a NULL, entonces se responde con un mensaje genérico
*/
int servidor(char *puerto, char *mensaje_enviar)
{
    int socket_servidor, socket_conexion;                     // Los números de socket de servidor (escucha) y conexión con el cliente (porque estamos en TCP, así que establecemos un nuevo socket por cada conexión con cada cliente)
    struct sockaddr_in direccion_servidor, direccion_cliente; // Structs que usaremos para almacenar las direcciones de cliente y servidor
    socklen_t tam_direccion_cliente;
    ssize_t bytes_enviados;         // Lo usaremos para llevar la cuenta de cuántos bytes hemos enviado
    int clientes_atendidos = 0;     // Lo usaremos para contar el total de solicitudes contestadas
    char *mensaje_procesado = NULL; // El mensaje procesado, es decir, el mensaje, pero con la IP del cliente añadida al principio. Esto no se exige en el enunciado de la práctica, pero creo que es buena idea tenerlo.
    char *ip_cliente = NULL;        // Aquí guardaremos la IP del cliente, pero en formato legible por humanos

    socket_servidor = socket(AF_INET, SOCK_STREAM, 0);

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
        return (EXIT_FAILURE);
    }

    if (listen(socket_servidor, 5) != 0) // Marcamos el socket como pasivo, para que pueda escuchar conexiones de clientes. Se pueden mantener hasta 5 en cola de espera.
    {
        perror("Error abriendo el socket para escucha");
        return (EXIT_FAILURE);
    }

    tam_direccion_cliente = sizeof(direccion_cliente);

    while (clientes_atendidos < MAX_CLIENTES_SERV) // Vamos a contestar en total solo 5 solicitudes, este es un valor arbitrario
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

        printf("Conectado %s al puerto %d\n", ip_cliente, ntohs(direccion_cliente.sin_port)); // Imprimimos la IP del cliente

        mensaje_procesado = (char *)realloc(mensaje_procesado, sizeof(char) * (strlen(ip_cliente) + strlen(": ") + strlen(mensaje_enviar) + 1)); // Reservamos memoria para hacer el mensaje procesado. Reservamos la suma de la longitud de las cadenas (más 1, para el '0' del final).
        mensaje_procesado = strcat(strcat(strcpy(mensaje_procesado, ip_cliente), ": "), mensaje_enviar);
        printf("%s\n", mensaje_procesado); // Construimos el mensaje de respuesta de la siguiente forma: "[IP CLIENTE]: [MENSAJE]"

        bytes_enviados = send(socket_conexion, mensaje_procesado, sizeof(char) * (strlen(mensaje_procesado) + 1), 0); // Enviamos el mensaje al cliente. El mensaje es la concatenación de la ip en formato texto, ": ", y el mensaje que se pasó como argumento a la función

        if (bytes_enviados < 0) // Hubo un error, pero no abortamos.
        {
            perror("Error enviando la respuesta");
        }

        printf("Enviados %zd bytes: %s\n", bytes_enviados, mensaje_enviar); // Info para el usuario

        close(socket_conexion); // Cerramos el socket para la conexión con el cliente

        clientes_atendidos++; // Incrementamos el contador de clientes atendidos
    }

    close(socket_servidor); // Cerramos el socket del servidor

    free(mensaje_procesado); // Liberamos memoria
    free(ip_cliente);

    return (EXIT_SUCCESS); // Todo fue bien, devolvemos EXIT_SUCCESS
}
