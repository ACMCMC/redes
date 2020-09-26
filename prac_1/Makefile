ARCHS_C = inet4_pton.c inet6_pton.c getaddrinfo.c

OUTPUTS = $(ARCHS_C:.c=.out)

OBJS = $(ARCHS_C:.c=.o)

ARGS = -Wall -Wextra -pedantic

all: $(OUTPUTS)

$(OUTPUTS): $(OBJS)
	gcc $(@:.out=.o) -o $@ -I $(HEADERS) $(ARGS)

%.o: %.c
	gcc -o $@ -c $< -I $(HEADERS) $(ARGS)

clean:
	-rm *.o
	-rm *.out