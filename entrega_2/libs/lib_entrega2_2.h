#ifndef LIB_ENTREGA2
#define LIB_ENTREGA2

/*
Función que realiza el lado del servidor del apartado 2 de la práctica.
file: el archivo con los caracteres a enviar
port: el puerto en el que queremos montar el socket
*/
int serv_mayusculas(char* file, char* port);

/*
Función que realiza el lado del cliente del apartado 2 de la práctica.
host: la IP a la que nos queremos conectar
port: el puerto al que nos queremos conectar
*/
int cliente_mayusculas(char* host, char* port);

#endif
