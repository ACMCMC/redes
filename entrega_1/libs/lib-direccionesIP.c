#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>

// Obtiene información del host y la imprime por pantalla
// name: Nombre del host (p. ej., "www.google.es")
// Devuelve EXIT_SUCCESS en caso de éxito, EXIT_FAILURE en caso de error
int get_host_info(char *name)
{
    struct addrinfo addrinfo; // Struct para encapsular los parámetros de llamada a getaddrinfo()
    struct addrinfo** result; // Puntero a lista enlazada de structs de tipo addrinfo
    int error_check; // Usaremos esta variable para comprobar errores en la llamada a funciones del sistema

    addrinfo.ai_family = AF_UNSPEC; // No buscamos una familia concreta de direcciones IP
    addrinfo.ai_socktype = 0; // No nos importa el tipo de socket
    addrinfo.ai_protocol = 0; // Tampoco nos importa el protocolo
    addrinfo.ai_flags = 0; // Aquí se pueden especificar flags adicionales haciendo un OR bitwise, pero como no queremos hacerlo ponemos un 0 (ningún parámetro)

    // getaddrinfo es una función para obtener la información sobre las direcciones IP asociadas a nombres de dominio, llamando a un DNS. También se puede usar para obtener información sobre servicios, o sobre servicios de un host, pero en este caso no se aplica.
    // name: Nombre del host, el parámetro que se pasa a esta función
    // service: Nombre del servicio, en este caso NULL porque queremos obtener solamente las IPs de un host
    // hints: Puntero a struct de tipo addrinfo, en el que se especifican los parámetros de arriba.
    // res: El puntero a lista enlazada que se declara arriba
    // Devuelve 0 si todo fue bien, en caso de error devuelve un valor diferente
    error_check = getaddrinfo(name, NULL, &addrinfo, result);

    if (error_check != 0) { // Si hubo un error...
        fprintf(stderr, "Se tuvo el error: %s\n", gai_strerror(error_check)); // Imprimimos el error en un formato legible por personas a stderr, el error lo podemos obtener a partir del código numérico usando gai_strerror()
        return (EXIT_FAILURE); // Devolvemos EXIT_FAILURE, para indicar un error (1)
    }

    printf("%s\n", name);
}

// Obtiene información del servicio y la imprime por pantalla
// service: nombre del servicio (p. ej., "http")
// Devuelve EXIT_SUCCESS en caso de éxito, EXIT_FAILURE en caso de error
int get_service_info(char *service)
{
    printf("%s\n", service);
}

// Obtiene información de la IP y la imprime por pantalla
// addr: Dirección IPv4 o IPv6 (p. ej, "192.168.1.1")
// Devuelve EXIT_SUCCESS en caso de éxito, EXIT_FAILURE en caso de error
int get_addr_info(char *addr)
{
    printf("%s\n", addr);
}

// Obtiene información del puerto y la imprime por pantalla
// port: Puerto (p. ej, "80")
// Devuelve EXIT_SUCCESS en caso de éxito, EXIT_FAILURE en caso de error
int get_port_info(char *port)
{
    printf("%s\n", port);
}