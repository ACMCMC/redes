#ifndef LIB_ENTREGA3
#define LIB_ENTREGA3

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/* Realiza el funcionamiento del host que envía un mensaje por UDP
 * puerto: el puerto desde el que enviamos el datagrama
 * mensaje_enviar: el string que queremos enviar (si no lo especificamos, se envía uno por defecto)
 * ip_envio: la dirección IP del host al que queremos enviar el datagrama
 * puerto_receptor: el puerto donde escucha por datagramas entrantes el host que recibe
 */
int sender(char* puerto, char* mensaje_enviar, char* ip_envio, char * puerto_receptor);

/* Realiza el funcionamiento del host que recibe un mensaje por UDP
 * puerto: el puerto donde escuchamos por datagramas entrantes
 */
int receptor(char* puerto);

#endif
