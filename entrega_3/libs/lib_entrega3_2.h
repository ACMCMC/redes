#ifndef LIB_ENTREGA3
#define LIB_ENTREGA3

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

/*
Función que realiza el lado del cliente del apartado 2 de la práctica.
file: el archivo con los caracteres a enviar
host: la IP del servidor de mayúsculas
puerto_servidor: el puerto de escucha en el servidor
puerto_propio: el puerto desde donde enviaremos los datagramas UDP
*/
int cliente_mayusculas(char *file, char *host, char *puerto_propio, char *puerto_servidor);

/*
Función que realiza el lado del servidor del apartado 2 de la práctica.
puerto: el puerto en el que queremos escuchar por conexiones entrantes
*/
int serv_mayusculas(char *puerto);

#endif
