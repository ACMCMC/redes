#ifndef LIB_DIRECCIONESIP
#define LIB_DIRECCIONESIP

// Obtiene información del host y la imprime por pantalla
// name: Nombre del host (p. ej., "www.google.es")
// Por pantalla imprimiría "Nombre canónico: www.google.es\nDirección IPv4: 172.217.14.227 (SOCK_STREAM)" y demás líneas para el resto de información sobre el host
// Devuelve EXIT_SUCCESS en caso de éxito, EXIT_FAILURE en caso de error
int get_host_info(char* name);

// Obtiene información del servicio y la imprime por pantalla
// service: Nombre del servicio (p. ej., "http")
// Por pantalla imprimiría "Servicio http: puerto 80"
// Devuelve EXIT_SUCCESS en caso de éxito, EXIT_FAILURE en caso de error
int get_service_info(char* service);

// Obtiene información de la IP y la imprime por pantalla
// addr: Dirección IPv4 o IPv6 (p. ej, "192.168.1.1")
// Por pantalla imprimiría "Dirección IPv4 192.168.1.1: host (NOMBRE DEL HOST)"
// Devuelve EXIT_SUCCESS en caso de éxito, EXIT_FAILURE en caso de error
int get_addr_info(char* addr);

// Obtiene información del puerto y la imprime por pantalla
// port: Puerto (p. ej, "80")
// Por pantalla imprimiría "Puerto 80: servicio http"
// Devuelve EXIT_SUCCESS en caso de éxito, EXIT_FAILURE en caso de error
int get_port_info(char* port);

// Obtiene información del host conjuntamente con un puerto y la imprime por pantalla
// name: Nombre del host (p. ej., "www.google.es")
// service: Nombre del servicio (p. ej., "http")
// Por pantalla imprimiría "www.google.es (http): (IP):(PUERTO)"
// Devuelve EXIT_SUCCESS en caso de éxito, EXIT_FAILURE en caso de error
int get_host_and_port_info(char* name, char* service);

// Obtiene información del host conjuntamente con un puerto y la imprime por pantalla
// addr: Dirección IPv4 o IPv6 (p. ej, "192.168.1.1")
// port: Puerto (p. ej, "80")
// Por pantalla imprimiría "192.168.1.1:80: (NOMBRE DEL HOST) (SERVICIO)"
// Devuelve EXIT_SUCCESS en caso de éxito, EXIT_FAILURE en caso de error
int get_name_and_service_info(char* addr, char* port);

#endif