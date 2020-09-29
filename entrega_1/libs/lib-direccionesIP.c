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
        return (EXIT_FAILURE);                                                // Salimos con EXIT_FAILURE, para indicar un error
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
            return (EXIT_FAILURE);
        }

        printf("\tDirección IPv%s: %s (%s)\n", (p_addrinfo->ai_family == AF_INET6) ? "6" : "4", socket_addr_ip_text, (p_addrinfo->ai_socktype == SOCK_STREAM) ? "SOCK_STREAM" : (p_addrinfo->ai_socktype == SOCK_DGRAM) ? "SOCK_DGRAM" : "otro tipo de socket");
    }

    freeaddrinfo(result);      // Libera toda la lista enlazada de structs sockaddrinfo
    free(socket_addr_ip_text); // También hay que liberar la cadena donde guardamos la IP
    result = NULL;             // Todos estos punteros ahora apuntan a zonas de memoria basura
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
    struct addrinfo hints;       // Struct para encapsular los parámetros de llamada a getaddrinfo()
    struct addrinfo *res = NULL; // Lista enlazada de structs de tipo addrinfo
    int error_check;             // Usaremos esta variable para comprobar errores en la llamada a funciones del sistema

    printf("****************************************************************\n"); // Mensaje para el usuario

    memset(&hints, 0, sizeof(struct addrinfo)); // Llenamos la estructura de 0s
    hints.ai_family = AF_INET6;                 // No buscamos una familia concreta de direcciones IP
    hints.ai_socktype = 0;                      // No nos importa el tipo de socket
    hints.ai_protocol = 0;                      // Tampoco nos importa el protocolo
    hints.ai_flags = AI_PASSIVE;                // Aquí se pueden especificar flags adicionales haciendo un OR bitwise, pero como no queremos hacerlo ponemos un 0 (ningún parámetro)

    error_check = getaddrinfo(NULL, service, &hints, &res);

    if (error_check != 0)
    {                                                                         // Si hubo un error...
        fprintf(stderr, "Se tuvo el error: %s\n", gai_strerror(error_check)); // Imprimimos el error en un formato legible por personas a stderr, el error lo podemos obtener a partir del código numérico usando gai_strerror()
        return (EXIT_FAILURE);                                                // Salimos con EXIT_FAILURE, para indicar un error
    }

    printf("Servicio %s: puerto %d\n", service, ntohs(((struct sockaddr_in6 *)res->ai_addr)->sin6_port));

    freeaddrinfo(res); // Liberamos memoria

    return (EXIT_SUCCESS); // Todo fue bien, devolvemos EXIT_SUCCESS
}

// Obtiene información de la IP y la imprime por pantalla
// addr: Dirección IPv4 o IPv6 (p. ej, "192.168.1.1")
// Devuelve EXIT_SUCCESS en caso de éxito, EXIT_FAILURE en caso de error
int get_addr_info(char *addr)
{
    char addr_ip_text[100];           // Aquí guardaremos el nombre de host que buscamos. No sabemos a priori cuánto puede ocupar
    struct addrinfo addr_info_params; // Lo usaremos para encapsular los parámetros de llamada a getaddrinfo()
    struct addrinfo *addr_info_ret;   // Aquí estará el resultado de la llamada a getaddrinfo()
    int ai_family;                    // We will store the address family here
    int error_check;                  // Usaremos esta variable para comprobar errores en la llamada a funciones del sistema

    printf("****************************************************************\n"); // Mensaje para el usuario

    memset(&addr_info_params, 0, sizeof(struct addrinfo));
    addr_info_params.ai_family = AF_UNSPEC;     // Este parámetro nos indica que getaddrinfo() debe devolvernos un resultado para cualquiera que sea la familia de la IP con la que llamamos a la función
    addr_info_params.ai_flags = AI_NUMERICHOST; // Este parámetro indica que la dirección que le pasamos como parámetro de búsqueda tiene formato numérico (es una IP)

    error_check = getaddrinfo(addr, NULL, &addr_info_params, &addr_info_ret); // Llamamos a getaddrinfo para obtener la familia de la dirección IP, ya que a priori no la conocemos. No nos importa el resultado de la búsqueda, solo que nos diga la familia

    if (error_check)
    {                                                                                                                 // Si el valor no es 0, es que hubo un error (probablemente en el formato de la dirección)
        fprintf(stderr, "Error en el formato de la dirección %s: %s. Abortando.\n", addr, gai_strerror(error_check)); // gai_strerror() nos da información en formato legible por humanos sobre el error
        return (EXIT_FAILURE);
    }

    ai_family = addr_info_ret->ai_family; // Guardamos ai_family del retorno de getaddrinfo()

    error_check = getnameinfo(addr_info_ret->ai_addr, addr_info_ret->ai_addrlen, addr_ip_text, sizeof(addr_ip_text), NULL, 0, 0); // Obtenemos información sobre el host de la dirección IP que recibimos como argumento. Ya la tenemos encapsulada de antes en addr_info_ret->ai_addr. addr_info_ret->ai_addrlen es la longitud. El cuarto parámetro es el puntero a la cadena de texto donde guardaremos el nombre de host traducido, de longitud sizeof(la cadena). El quinto parámetro es NULL porque no estamos buscando un servicio, lo mismo para el sexto (en este caso es 0, porque como pasamos un puntero nulo tiene longitud 0). El séptimo es 0 porque no queremos especificar ningún flag en concreto.

    if (error_check)
    {
        fprintf(stderr, "Error llamando a getnameinfo. Error: %s. Abortando.\n", gai_strerror(error_check)); // gai_strerror() nos da información en formato legible por humanos sobre el error
        return (EXIT_FAILURE);
    }

    printf("Dirección IPv%s %s: host %s\n", (ai_family == AF_INET6) ? "6" : (ai_family == AF_INET) ? "4" : "?", addr, addr_ip_text);

    freeaddrinfo(addr_info_ret); // Liberamos memoria

    return (EXIT_SUCCESS); // Todo fue bien, devolvemos EXIT_SUCCESS
}

// Obtiene información del puerto y la imprime por pantalla
// port: Puerto (p. ej, "80")
// Devuelve EXIT_SUCCESS en caso de éxito, EXIT_FAILURE en caso de error
int get_port_info(char *port)
{
    struct sockaddr_in6 sockaddr; // Struct para encapsular los parámetros de llamada a getaddrinfo()
    int error_check;              // Usaremos esta variable para comprobar errores en la llamada a funciones del sistema
    char service[50];             // Longitud arbitraria porque no sabemos cómo de grande puede ser el nombre del servicio

    printf("****************************************************************\n"); // Mensaje para el usuario

    memset(&sockaddr, 0, sizeof(sockaddr));           // Llenamos la estructura de 0s
    sockaddr.sin6_family = AF_INET6;                  // Especificamos una familia de direcciones aunque no estemos buscando una IP sino un puerto
    sockaddr.sin6_port = htons((uint16_t)atoi(port)); // Para buscar el puerto, tenemos que convertirlo a número (int) con atoi, y después a formato de red con htons

    error_check = getnameinfo((struct sockaddr *)&sockaddr, sizeof(sockaddr), NULL, 0, service, sizeof(service), 0); // Obtenemos la información del servicio asociado al puerto. No especificamos nada en host porque no estamos buscando esa información. Tampoco necesitamos ningún flag.

    if (error_check != 0)
    {                                                                         // Si hubo un error...
        fprintf(stderr, "Se tuvo el error: %s\n", gai_strerror(error_check)); // Imprimimos el error en un formato legible por personas a stderr, el error lo podemos obtener a partir del código numérico usando gai_strerror()
        return (EXIT_FAILURE);                                                // Salimos con EXIT_FAILURE, para indicar un error
    }

    printf("Puerto %s: servicio %s\n", port, service);

    // No hay que liberar memoria

    return (EXIT_SUCCESS); // Todo fue bien, devolvemos EXIT_SUCCESS
}

// Obtiene información del host conjuntamente con un puerto y la imprime por pantalla
// name: Nombre del host (p. ej., "www.google.es")
// service: Nombre del servicio (p. ej., "http")
// Devuelve EXIT_SUCCESS en caso de éxito, EXIT_FAILURE en caso de error
int get_host_and_port_info(char* name, char* service) {
    
  
    return (EXIT_SUCCESS); // Todo fue bien, devolvemos EXIT_SUCCESS
}

// Obtiene información del host conjuntamente con un puerto y la imprime por pantalla
// addr: Dirección IPv4 o IPv6 (p. ej, "192.168.1.1")
// port: Puerto (p. ej, "80")
// Devuelve EXIT_SUCCESS en caso de éxito, EXIT_FAILURE en caso de error
int get_name_and_service_info(char* addr, char* port) {
    char addr_ip_text[100];           // Aquí guardaremos el nombre de host que buscamos. No sabemos a priori cuánto puede ocupar
    struct addrinfo addr_info_params; // Lo usaremos para encapsular los parámetros de llamada a getaddrinfo()
    struct addrinfo *addr_info_ret;   // Aquí estará el resultado de la llamada a getaddrinfo()
    int ai_family;                    // We will store the address family here
    int error_check;                  // Usaremos esta variable para comprobar errores en la llamada a funciones del sistema

    printf("****************************************************************\n"); // Mensaje para el usuario

    memset(&addr_info_params, 0, sizeof(struct addrinfo));
    addr_info_params.ai_family = AF_UNSPEC;     // Este parámetro nos indica que getaddrinfo() debe devolvernos un resultado para cualquiera que sea la familia de la IP con la que llamamos a la función
    addr_info_params.ai_flags = AI_NUMERICHOST; // Este parámetro indica que la dirección que le pasamos como parámetro de búsqueda tiene formato numérico (es una IP)

    error_check = getaddrinfo(addr, NULL, &addr_info_params, &addr_info_ret); // Llamamos a getaddrinfo para obtener la familia de la dirección IP, ya que a priori no la conocemos. No nos importa el resultado de la búsqueda, solo que nos diga la familia

    if (error_check)
    {                                                                                                                 // Si el valor no es 0, es que hubo un error (probablemente en el formato de la dirección)
        fprintf(stderr, "Error en el formato de la dirección %s: %s. Abortando.\n", addr, gai_strerror(error_check)); // gai_strerror() nos da información en formato legible por humanos sobre el error
        return (EXIT_FAILURE);
    }

    ai_family = addr_info_ret->ai_family; // Guardamos ai_family del retorno de getaddrinfo()

    error_check = getnameinfo(addr_info_ret->ai_addr, addr_info_ret->ai_addrlen, addr_ip_text, sizeof(addr_ip_text), NULL, 0, 0); // Obtenemos información sobre el host de la dirección IP que recibimos como argumento. Ya la tenemos encapsulada de antes en addr_info_ret->ai_addr. addr_info_ret->ai_addrlen es la longitud. El cuarto parámetro es el puntero a la cadena de texto donde guardaremos el nombre de host traducido, de longitud sizeof(la cadena). El quinto parámetro es NULL porque no estamos buscando un servicio, lo mismo para el sexto (en este caso es 0, porque como pasamos un puntero nulo tiene longitud 0). El séptimo es 0 porque no queremos especificar ningún flag en concreto.

    if (error_check)
    {
        fprintf(stderr, "Error llamando a getnameinfo. Error: %s. Abortando.\n", gai_strerror(error_check)); // gai_strerror() nos da información en formato legible por humanos sobre el error
        return (EXIT_FAILURE);
    }

    struct sockaddr_in6 sockaddr; // Struct para encapsular los parámetros de llamada a getaddrinfo()
    char service[50];             // Longitud arbitraria porque no sabemos cómo de grande puede ser el nombre del servicio

    printf("****************************************************************\n"); // Mensaje para el usuario

    memset(&sockaddr, 0, sizeof(sockaddr));           // Llenamos la estructura de 0s
    sockaddr.sin6_family = AF_INET6;                  // Especificamos una familia de direcciones aunque no estemos buscando una IP sino un puerto
    sockaddr.sin6_port = htons((uint16_t)atoi(port)); // Para buscar el puerto, tenemos que convertirlo a número (int) con atoi, y después a formato de red con htons

    error_check = getnameinfo((struct sockaddr *)&sockaddr, sizeof(sockaddr), NULL, 0, service, sizeof(service), 0); // Obtenemos la información del servicio asociado al puerto. No especificamos nada en host porque no estamos buscando esa información. Tampoco necesitamos ningún flag.

    if (error_check != 0)
    {                                                                         // Si hubo un error...
        fprintf(stderr, "Se tuvo el error: %s\n", gai_strerror(error_check)); // Imprimimos el error en un formato legible por personas a stderr, el error lo podemos obtener a partir del código numérico usando gai_strerror()
        return (EXIT_FAILURE);                                                // Salimos con EXIT_FAILURE, para indicar un error
    }

    printf("%s:%s: %s (%s)\n", addr, port, addr_ip_text, service);

    freeaddrinfo(addr_info_ret); // Liberamos memoria

    return (EXIT_SUCCESS); // Todo fue bien, devolvemos EXIT_SUCCESS
}