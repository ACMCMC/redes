#ifndef LIB_DIRECCIONESIP
#define LIB_DIRECCIONESIP

// Obtiene información del host y la imprime por pantalla
// name: Nombre del host (p. ej., "www.google.es")
// No devuelve nada, ya que la información se imprime por pantalla
void get_host_info(char* name);

// Obtiene información del servicio y la imprime por pantalla
// service: Nombre del servicio (p. ej., "http")
// No devuelve nada, ya que la información se imprime por pantalla
void get_service_info(char* service);

// Obtiene información de la IP y la imprime por pantalla
// addr: Dirección IPv4 o IPv6 (p. ej, "192.168.1.1")
// No devuelve nada, ya que la información se imprime por pantalla
void get_addr_info(char* addr);

// Obtiene información del puerto y la imprime por pantalla
// No devuelve nada, ya que la información se imprime por pantalla
void get_port_info(char* port);

#endif