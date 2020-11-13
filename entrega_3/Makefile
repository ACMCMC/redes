ARCHS_C := P3_1_receptor.c P3_1_sender.c

OUTPUTS := $(ARCHS_C:.c=.out)

HEADERS := ./libs/lib_entrega3_1.h
HEADERS_DIR := ./libs
IMPLS_HEADERS := $(HEADERS:.h=.c)
OBJS_HEADERS := $(HEADERS:.h=.o)

OBJS := $(ARCHS_C:.c=.o) $(OBJS_HEADERS)


ARGS := -Wall -Wextra -pedantic

all: $(OUTPUTS)

$(OUTPUTS): $(OBJS)
	gcc $(@:.out=.o) $(OBJS_HEADERS) -o $@ -L $(HEADERS_DIR) $(ARGS)

%.o: %.c
	gcc -o $@ -c $< $(ARGS)

clean:
	-find . -type f -name '*.o' -delete
	-find . -type f -name '*.out' -delete
	-rm $(OUTPUTS:.out=)