#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

#define MAX_CLIENTES_SERV 5
#define MAX_TAM_MSG 100 // El tamaño máximo del mensaje, tanto en cliente como en servidor

int cliente_mayusculas(char *file, char *host, char *puerto);

int main(int argc, char **argv)
{

    int opt;
    // port: Puerto
    // direccion: IP del servidor
    // arch_enviar: el archivo de líneas de texto a enviarle al servidor
    char *port = NULL, *direccion = NULL, *arch_enviar = NULL;

    // Comprueba que exista al menos un operando
    // En caso de error salimos de la función main con el codigo EXIT_FAILURE
    if (argc < 4)
    {
        printf("Falta un operando\n");
        printf("Usar: %s [-f, nombre del archivo de texto a enviar] [-h, dirección del servidor] [-p, puerto del servidor]\n", argv[0]);
        return (EXIT_FAILURE);
    }

    // La funcion getopt() permite de forma facil manejar operandos en linea de comandos
    // Las opciones n: s: i: p: indican que esos "flags" (nsip) deben de ir seguidos de un argumento
    // Ese parametro se guarda en la variable externa optarg
    while ((opt = getopt(argc, argv, "p:h:f:")) != -1)
    {
        switch (opt)
        {
        case 'f':
            arch_enviar = optarg; // El archivo de texto
            break;
        case 'p':
            port = optarg; // Argumento numero de puerto
            break;
        case 'h':
            direccion = optarg; // IP del servidor
            break;
        case ':': // Se introdujo un flag sin argumento obligatorio
            fprintf(stderr, "La opción -%c requiere un argumento.\n", optopt);
            return (EXIT_FAILURE);
            break;
        case '?':
            if (isprint(optopt)) // Se introdujo un flag incorrecto
                fprintf(stderr, "Opción desconocida `-%c'.\n", optopt);
            else // Hay un caracter no legible en las opciones
                fprintf(stderr, "Caracter de opción desconocido `\\x%x'.\n", optopt);
            return (EXIT_FAILURE);
            break;
        default: // Se produjo un error indeterminado. Nunca se deberia llegar aqui.
            abort();
        }
    }

    printf("\n");

    // Llamamos a la función adecuada
    if (direccion && port && arch_enviar)
    {
        if (cliente_mayusculas(arch_enviar, direccion, port) == EXIT_SUCCESS)
        {
            exit(EXIT_SUCCESS);
        }
        else
        {
            fprintf(stderr, "Error en la función del cliente de mayúsculas. No ha devuelto EXIT_SUCCESS.\n");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        fprintf(stderr, "Argumentos mal especificados.\n");
        exit(EXIT_FAILURE);
    }
}

/*
Función que realiza el lado del cliente del apartado 2 de la práctica.
file: el archivo con los caracteres a enviar
host: la IP del servidor de mayúsculas
puerto: el puerto de escucha en el servidor
*/
int cliente_mayusculas(char *file, char *host, char *puerto)
{
    FILE *fp, *fp_out;
    int socket_servidor, error_check;
    unsigned long i;
    ssize_t bytes_enviados, bytes_recibidos;
    struct sockaddr_in direccion_envio;
    char linea[MAX_TAM_MSG], linea_respuesta[MAX_TAM_MSG];
    char *file_out;

    fp = fopen(file, "r");
    if (!fp)
    {
        perror("Error abriendo el archivo de lectura. Abortamos.");
        return (EXIT_FAILURE);
    }

    file_out = (char *)malloc(sizeof(char) * (strlen(file) + 1));
    for (i = 0; i <= strlen(file); i++)
    {
        file_out[i] = toupper(file[i]);
    }
    fp_out = fopen(file_out, "w");
    if (!fp)
    {
        perror("Error abriendo el archivo de escritura. Abortamos.");
        return (EXIT_FAILURE);
    }
    free(file_out);

    socket_servidor = socket(AF_INET, SOCK_STREAM, 0); // Creamos un socket para conectarnos al servidor
    if (socket_servidor < 0)
    {
        perror("Error al crear el socket");
        fclose(fp);
        fclose(fp_out);
        return (EXIT_FAILURE);
    }

    memset(&direccion_envio, 0, sizeof(direccion_envio)); // La estructura tiene todo 0s
    direccion_envio.sin_family = AF_INET;                 // IPv4
    direccion_envio.sin_port = htons(atoi(puerto));       // El puerto estaba como una cadena de caracteres ASCII, lo convertimos a entero y en orden de red

    if ((error_check = inet_pton(AF_INET, host, &(direccion_envio.sin_addr))) != 1) // Convertimos la ip de formato texto a formato máquina
    {
        if (error_check == -1)
        {
            perror("Error procesando la dirección IP");
        }
        else if (error_check == 0)
        {
            fprintf(stderr, "La dirección IP especificada no es válida.\n");
        }
        fclose(fp);
        fclose(fp_out);
        close(socket_servidor);
        return (EXIT_FAILURE);
    }

    if (connect(socket_servidor, (struct sockaddr *)&direccion_envio, sizeof(direccion_envio)) != 0) // Nos conectamos al servidor y comprobamos que todo fue correctamente
    {
        perror("Error al conectarse al servidor");
        close(socket_servidor); // Cerramos el socket de conexión al servidor
        fclose(fp);
        fclose(fp_out);
        return (EXIT_FAILURE);
    }

    while (!feof(fp) && (fgets(linea, MAX_TAM_MSG, fp) != NULL))
    { // Repetimos esto mientras queden líneas en el archivo

        bytes_enviados = send(socket_servidor, linea, sizeof(char) * (strnlen(linea, MAX_TAM_MSG) + 1), 0); // Enviamos la línea de texto al servidor. socket_servidor: identificador del socket, linea: la cadena de texto, el tercer parámetro es su longitud, y el 4o es un 0 porque no especificamos flags

        if (bytes_enviados < 0) // Hubo un error
        {
            perror("Error enviando la línea");
            close(socket_servidor); // Cerramos el socket de conexión al servidor
            fclose(fp);
            fclose(fp_out);
            return (EXIT_FAILURE);
        }

        bytes_recibidos = recv(socket_servidor, linea_respuesta, sizeof(char) * MAX_TAM_MSG, 0); // Recibimos el mensaje. socket_servidor: identificador del socket, linea_respuesta: puntero al string donde guardaremos la línea en minúsculas que nos mandó el cliente, y que vamos a convertir a mayúsculas (por eso la llamo línea_respuesta, al principio es la recepción, pero posteriormente esa misma línea pero en mayúsculas va a ser con la que respondamos al cliente), el tercer parámetro es el tamaño máximo del mensaje que podemos guardar en esa cadena, y el 4o es 0 porque no queremos especificar ningún flag

        if (bytes_recibidos < 0) // Miramos si hubo error recibiendo el mensaje
        {
            perror("Error al recibir el mensaje");
            close(socket_servidor); // Cerramos el socket de conexión al servidor
            fclose(fp);
            fclose(fp_out);
            return (EXIT_FAILURE);
        }

        fputs(linea_respuesta, fp_out);

        if (linea_respuesta[strnlen(linea_respuesta, MAX_TAM_MSG) - 1] == '\n' && linea[strnlen(linea, MAX_TAM_MSG) - 1] == '\n')
        { // fgets también lee el carácter de nueva línea, y eso antes se lo enviamos también al servidor, pero de cara a hacer el printf revisamos si la cadena acaba en '\n', y en caso afirmativo, la reemplazamos por '\0', para que la presentación sea más bonita y no tengamos un salto extraño de línea. Si la línea que mandamos acaba en '\n' antes del '0', la que recibimos también, pero siempre es mejor asegurarse.
            linea[strnlen(linea, MAX_TAM_MSG) - 1] = '\0';
            linea_respuesta[strnlen(linea_respuesta, MAX_TAM_MSG) - 1] = '\0';
        }
        printf("\nEnviados %zd bytes: %s.\n\tRecibidos %zd bytes: %s\n", bytes_enviados, linea, bytes_recibidos, linea_respuesta); // Imprimimos los mensajes por consola
    }

    close(socket_servidor); // Cerramos el socket del servidor y los archivos
    fclose(fp);
    fclose(fp_out);

    return (EXIT_SUCCESS);
}
