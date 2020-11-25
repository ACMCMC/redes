# Práctica 3
## Aldán Creo Mariño, Redes 2020/21

`P3_1_receptor.c` se corresponde con el apartado 1.a; `P3_1_receptor.c`, con el 1.b. Sus códigos están en una librería, definida en `libs/lib_entrega3_1.c`.

`P3_cliente_mayusculas.c` y `P3_serv_mayusculas.c` se corresponden con el apartado 2. Sus códigos están en una librería, definida en `libs/lib_entrega3_2.c`.

Compilar con `make`.

Uso:
- `./P3_1_receptor.out [-p puerto de escucha]`
- `./P3_1_sender.out [-p numero de puerto propio del que envía] [-r numero de puerto de escucha del receptor] [-h IP del receptor] [-m el mensaje a enviar]`
- `./P3_cliente_mayusculas.out [-c, puerto propio del cliente] [-h, dirección del servidor] [-p, puerto del servidor] [-f, nombre del archivo de texto a enviar]`
- `./P3_serv_mayusculas.out [-p puerto de escucha]`