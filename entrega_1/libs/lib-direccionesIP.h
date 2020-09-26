#ifndef LIB_DIRECCIONESIP
#define LIB_DIRECCIONESIP

// Obtiene información del host y la imprime por pantalla
// name: Nombre del host (p. ej., "www.google.es")
// Devuelve EXIT_SUCCESS en caso de éxito, EXIT_FAILURE en caso de error
int get_host_info(char* name);

// Obtiene información del servicio y la imprime por pantalla
// service: Nombre del servicio (p. ej., "http")
// Devuelve EXIT_SUCCESS en caso de éxito, EXIT_FAILURE en caso de error
int get_service_info(char* service);

// Obtiene información de la IP y la imprime por pantalla
// addr: Dirección IPv4 o IPv6 (p. ej, "192.168.1.1")
// Devuelve EXIT_SUCCESS en caso de éxito, EXIT_FAILURE en caso de error
int get_addr_info(char* addr);

// Obtiene información del puerto y la imprime por pantalla
// port: Puerto (p. ej, "80")
// Devuelve EXIT_SUCCESS en caso de éxito, EXIT_FAILURE en caso de error
int get_port_info(char* port);

#endif