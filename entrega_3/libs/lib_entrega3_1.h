#ifndef LIB_ENTREGA3
#define LIB_ENTREGA3

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/*
Esta función crea un servidor en el puerto indicado, para responder a todos los clientes que se conecten a él con el mensaje que se le pasa como segundo parámetro. Si el segundo parámetro apunta a NULL, entonces se responde con un mensaje genérico
*/
int sender(char* puerto, char* mensaje_enviar, char* ip_envio, char * puerto_receptor);

/*
Esta función se conecta al servidor con el puerto y la dirección que se le pasan en formato texto, e imprime por pantalla lo que recibe
*/
int cliente(char* puerto, char* direccion);

#endif
