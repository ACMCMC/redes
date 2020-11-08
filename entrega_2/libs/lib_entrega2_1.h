#ifndef LIB_ENTREGA2
#define LIB_ENTREGA2

/*
Esta función crea un servidor en el puerto indicado, para responder a todos los clientes que se conecten a él con el mensaje que se le pasa como segundo parámetro. Si el segundo parámetro apunta a NULL, entonces se responde con un mensaje genérico
*/
int servidor(char* puerto, char* mensaje_enviar);

/*
Esta función se conecta al servidor con el puerto y la dirección que se le pasan en formato texto, e imprime por pantalla lo que recibe
*/
int cliente(char* puerto, char* direccion);

#endif
