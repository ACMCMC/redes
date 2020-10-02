// Aldán Creo Mariño, Redes 2020/21

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include "libs/lib-direccionesIP.h"

int main(int argc, char *argv[])
{
  int opt;
  // name: Nombre del host
  // service: Nombre del servicio
  // addr: Direccion IPv4 o IPv6
  // port: Puerto
  char *name = NULL, *service = NULL, *addr = NULL, *port = NULL;

  // Comprueba que exista al menos un operando
  // En caso de error salimos de la función main con el codigo EXIT_FAILURE
  if (argc < 2)
  {
    printf("Falta un operando\n");
    printf("Usar: %s [-n Nombre del host] [-s Nombre del servicio (p.e. http)] [-i Direccion ip] [-p Numero de puerto]\n", argv[0]);
    return (EXIT_FAILURE);
  }

  // La funcion getopt() permite de forma facil manejar operandos en linea de comandos
  // Las opciones n: s: i: p: indican que esos "flags" (nsip) deben de ir seguidos de un argumento
  // Ese parametro se guarda en la variable externa optarg
  while ((opt = getopt(argc, argv, ":n:s:i:p:")) != -1)
  {
    switch (opt)
    {
    case 'n':
      name = optarg; // Argumento nombre de host
      break;
    case 's':
      service = optarg; // Argumento nombre de servicio
      break;
    case 'i':
      addr = optarg; // Argumento direccion ip
      break;
    case 'p':
      port = optarg; // Argumento numero de puerto
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
  // Llamamos a las funciones correspondientes a los argumentos solicitados (que son != NULL)

  if (name && service)
  {                                                            // Nunca se llegará a ejecutar get_host_and_port_info() a menos que se cumpla que tenemos name y service, que es la condición necesaria que queremos que se cumpla para ejecutar la función. Si no tenemos name o service, ejecutamos lo del else.
    if (get_host_and_port_info(name, service) != EXIT_SUCCESS) // Si aun así, get_host_and_port_info() falla, entonces ejecutamos las funciones por separado.
    {
      fprintf(stderr, "\nError llamando a get_host_and_port_info. Reintentándolo con las funciones get_host_info y get_service_info por separado.\n\n");
      get_host_info(name);       // No nos interesa controlar el error esta vez
      get_service_info(service); // No nos interesa controlar el error esta vez
    }
  }
  else // name y service no están ambos definidos
  {
    if (name)
      // Llamar a la funcion para obtener informacion de la IP
      if (get_host_info(name) != EXIT_SUCCESS)
        fprintf(stderr, "\nError llamando a get_host_info.\n\n");
    if (service)
      // Llamar a la funcion para obtener informacion del puerto
      if (get_service_info(service) != EXIT_SUCCESS)
        fprintf(stderr, "\nError llamando a get_service_info.\n\n");
  }

  if (addr && port)
  {                                                            // Nunca se llegará a ejecutar get_name_and_service_info() a menos que se cumpla que tenemos addr y port, que es la condición necesaria que queremos que se cumpla para ejecutar la función. Si no tenemos addr o port, ejecutamos lo de abajo.
    if (get_name_and_service_info(addr, port) != EXIT_SUCCESS) // Si aun así, get_name_and_service_info() falla, entonces ejecutamos las funciones por separado.
    {
      fprintf(stderr, "\nError llamando a get_name_and_service_info. Reintentándolo con las funciones get_addr_info y get_port_info por separado.\n\n");
      get_addr_info(addr); // No nos interesa controlar el error esta vez
      get_port_info(port); // No nos interesa controlar el error esta vez
    }
  }
  else // addr y port no están ambos definidos
  {
    if (addr)
      // Llamar a la funcion para obtener informacion de la IP
      if (get_addr_info(addr) != EXIT_SUCCESS)
        fprintf(stderr, "\nError llamando a get_addr_info.\n\n");
    if (port)
      // Llamar a la funcion para obtener informacion del puerto
      if (get_port_info(port) != EXIT_SUCCESS)
        fprintf(stderr, "\nError llamando a get_port_info.\n\n");
  }

  printf("****************************************************************\n\n");

  // Finalizamos correctamente, con codigo de salida EXIT_SUCCESS
  return (EXIT_SUCCESS);
}
