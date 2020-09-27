#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <netdb.h>

// Obtiene información del host y la imprime por pantalla
// name: Nombre del host (p. ej., "www.google.es")
// Devuelve EXIT_SUCCESS en caso de éxito, EXIT_FAILURE en caso de error
int get_host_info(char *name)
{
    struct addrinfo addrinfo;         // Struct para encapsular los parámetros de llamada a getaddrinfo()
    struct addrinfo *result = NULL;   // Lista enlazada de structs de tipo addrinfo
    struct addrinfo *p_addrinfo;      // Puntero a un struct addrinfo, lo usaremos para recorrer la lista enlazada
    struct in_addr *socket_addr_ip;   // Puntero a la dirección ip del socket, lo usaremos también después
    char *socket_addr_ip_text = NULL; // La dirección a la que apunta el puntero anterior, como un string legible por humanos. Al principio es NULL, después usamos realloc()
    int error_check;                  // Usaremos esta variable para comprobar errores en la llamada a funciones del sistema

    printf("****************************************************************\nNombre canónico: %s\n", name); // Mensaje para el usuario

    memset(&addrinfo, 0, sizeof(struct addrinfo)); // Llenamos la estructura de 0s
    addrinfo.ai_family = AF_UNSPEC;                // No buscamos una familia concreta de direcciones IP
    addrinfo.ai_socktype = 0;                      // No nos importa el tipo de socket
    addrinfo.ai_protocol = 0;                      // Tampoco nos importa el protocolo
    addrinfo.ai_flags = 0;                         // Aquí se pueden especificar flags adicionales haciendo un OR bitwise, pero como no queremos hacerlo ponemos un 0 (ningún parámetro)

    // getaddrinfo es una función para obtener la información sobre las direcciones IP asociadas a nombres de dominio, llamando a un DNS. También se puede usar para obtener información sobre servicios, o sobre servicios de un host, pero en este caso no se aplica.
    // name: Nombre del host, el parámetro que se pasa a esta función
    // service: Nombre del servicio, en este caso NULL porque queremos obtener solamente las IPs de un host
    // hints: Puntero a struct de tipo addrinfo, en el que se especifican los parámetros de arriba.
    // res: El puntero a lista enlazada que se declara arriba
    // Devuelve 0 si todo fue bien, en caso de error devuelve un valor diferente
    error_check = getaddrinfo(name, NULL, &addrinfo, &result);

    if (error_check != 0)
    {                                                                         // Si hubo un error...
        fprintf(stderr, "Se tuvo el error: %s\n", gai_strerror(error_check)); // Imprimimos el error en un formato legible por personas a stderr, el error lo podemos obtener a partir del código numérico usando gai_strerror()
        exit(EXIT_FAILURE);                                                   // Salimos con EXIT_FAILURE, para indicar un error
    }

    for (p_addrinfo = result; p_addrinfo != NULL; p_addrinfo = p_addrinfo->ai_next)
    {

        if (p_addrinfo->ai_family == AF_INET) // Si la familia de la dirección es IPv4, tenemos que castear el puntero a struct sockaddr_in
        {
            socket_addr_ip = (struct in_addr *)&(((struct sockaddr_in *)p_addrinfo->ai_addr)->sin_addr);
        }
        else if (p_addrinfo->ai_family == AF_INET6) // Si la familia de la dirección es IPv6, tenemos que castear el puntero a struct sockaddr_in6
        {
            socket_addr_ip = (struct in_addr *)&(((struct sockaddr_in6 *)p_addrinfo->ai_addr)->sin6_addr);
        }
        else // Si ai_family no es IPv4 o IPv6, no podemos castear el puntero a un tipo adecuado de struct. Lo tratamos de forma genérica
        {
            fprintf(stderr, "No se reconoce el valor de ai_family.\n");
            socket_addr_ip = (struct in_addr *)&((p_addrinfo->ai_addr)->sa_data);
        }

        socket_addr_ip_text = (char *)realloc(socket_addr_ip_text, p_addrinfo->ai_addrlen); // Guardamos la memoria necesaria para la cadena de texto de la IP

        if (inet_ntop(p_addrinfo->ai_family, (void *)(socket_addr_ip), socket_addr_ip_text, p_addrinfo->ai_addrlen) == NULL)
        {
            fprintf(stderr, "Error en inet_ntop: %s. Abortando.\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        printf("\tDirección IPv%s: %s (%s)\n", (p_addrinfo->ai_family == AF_INET6) ? "6" : "4", socket_addr_ip_text, (p_addrinfo->ai_socktype == SOCK_STREAM) ? "SOCK_STREAM" : (p_addrinfo->ai_socktype == SOCK_DGRAM) ? "SOCK_DGRAM" : "otro tipo de socket");
    }

    freeaddrinfo(result); // Libera toda la lista enlazada de structs sockaddrinfo
    free(socket_addr_ip_text); // También hay que liberar la cadena donde guardamos la IP
    result = NULL; // Todos estos punteros ahora apuntan a zonas de memoria basura
    p_addrinfo = NULL;
    socket_addr_ip = NULL;
    socket_addr_ip = NULL;

    return (EXIT_SUCCESS); // Todo fue bien, devolvemos EXIT_SUCCESS
}

// Obtiene información del servicio y la imprime por pantalla
// service: nombre del servicio (p. ej., "http")
// Devuelve EXIT_SUCCESS en caso de éxito, EXIT_FAILURE en caso de error
int get_service_info(char *service)
{
    struct protoent* protocol; // Guardará información sobre el protocolo

    printf("****************************************************************\n"); // Mensaje para el usuario

    protocol = getprotobyname(service); // Obtener una estructura protoent alojada estáticamente por la propia función getprotobynumber(), que describa al protocolo del nombre que se le pasa como parámetro. El valor de retorno es la dirección en memoria de la estructura, a la que apuntar el puntero.

    if ( protocol == NULL ) { // Si el puntero apunta a NULL, es que algo fue mal en la función
        fprintf(stderr, "Error obteniendo información de protocolo con nombre %s.\n", service);
        exit(EXIT_FAILURE); // Salimos con error
    }

    printf("Servicio %s: puerto %d\n", service, protocol->p_proto);

    return (EXIT_SUCCESS); // Todo fue bien, devolvemos EXIT_SUCCESS
}

// Obtiene información de la IP y la imprime por pantalla
// addr: Dirección IPv4 o IPv6 (p. ej, "192.168.1.1")
// Devuelve EXIT_SUCCESS en caso de éxito, EXIT_FAILURE en caso de error
int get_addr_info(char *addr)
{
    printf("%s\n", addr);

    return (EXIT_SUCCESS); // Todo fue bien, devolvemos EXIT_SUCCESS
}

// Obtiene información del puerto y la imprime por pantalla
// port: Puerto (p. ej, "80")
// Devuelve EXIT_SUCCESS en caso de éxito, EXIT_FAILURE en caso de error
int get_port_info(char *port)
{
    struct protoent* protocol; // Guardará información sobre el protocolo

    printf("****************************************************************\n"); // Mensaje para el usuario

    protocol = getprotobynumber(atoi(port)); // Obtener una estructura protoent alojada estáticamente por la propia función getprotobynumber(), que describa al protocolo del número que se le pasa como parámetro. Antes hay que convertirlo a int para que sea el tipo de dato adecuado, usando atoi(). El valor de retorno es la dirección en memoria de la estructura, a la que apuntar el puntero.

    if ( protocol == NULL ) { // Si el puntero apunta a NULL, es que algo fue mal en la función
        fprintf(stderr, "Error obteniendo información de protocolo con número %s.\n", port);
        exit(EXIT_FAILURE); // Salimos con error
    }

    printf("Puerto %s: servicio %s\n", port, protocol->p_name);

    return (EXIT_SUCCESS); // Todo fue bien, devolvemos EXIT_SUCCESS
}
