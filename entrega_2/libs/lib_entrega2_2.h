#ifndef LIB_ENTREGA2
#define LIB_ENTREGA2

/*
Función que realiza el lado del cliente del apartado 2 de la práctica.
file: el archivo con los caracteres a enviar
host: el archivo con los caracteres a enviar
puerto: el puerto en el que queremos montar el socket
*/
int cliente_mayusculas(char* file, char* host, char* puerto);

/*
Función que realiza el lado del servidor del apartado 2 de la práctica.
host: la IP a la que nos queremos conectar
puerto: el puerto al que nos queremos conectar
*/
int serv_mayusculas(char* puerto);

#endif
