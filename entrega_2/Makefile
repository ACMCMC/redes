ARCHS_C := P2_1_cliente.c P2_1_servidor.c P2_serv_mayusculas.c P2_cliente_mayusculas.c

OUTPUTS := $(ARCHS_C:.c=.out)

HEADERS := ./libs/lib_entrega2_1.h ./libs/lib_entrega2_2.h
HEADERS_DIR := ./libs
IMPLS_HEADERS := $(HEADERS:.h=.c)
OBJS_HEADERS := $(HEADERS:.h=.o)

OBJS := $(ARCHS_C:.c=.o) $(OBJS_HEADERS)


ARGS := -Wall -Wextra -pedantic

all: $(OUTPUTS)

$(OUTPUTS): $(OBJS)
	gcc $(@:.out=.o) $(OBJS_HEADERS) -o $(@:.out=) -L $(HEADERS_DIR) $(ARGS)

%.o: %.c
	gcc -o $@ -c $< -L $(HEADERS_DIR) $(ARGS)

clean:
	-find . -type f -name '*.o' -delete
	-find . -type f -name '*.out' -delete
	-rm $(OUTPUTS:.out=)