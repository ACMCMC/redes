#ifndef LIB_ENTREGA2
#define LIB_ENTREGA2

/*
Función que realiza el lado del cliente del apartado 2 de la práctica.
file: el archivo con los caracteres a enviar
host: la IP del servidor de mayúsculas
puerto: el puerto de escucha en el servidor
*/
int cliente_mayusculas(char* file, char* host, char* puerto);

/*
Función que realiza el lado del servidor del apartado 2 de la práctica.
puerto: el puerto en el que queremos escuchar por conexiones entrantes
*/
int serv_mayusculas(char* puerto);

#endif
