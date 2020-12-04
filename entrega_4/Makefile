ARCHS_C := P4.c

OUTPUTS := $(ARCHS_C:.c=.out)

OBJS := $(ARCHS_C:.c=.o) $(OBJS_HEADERS)


ARGS := -Wall -Wextra -pedantic

all: $(OUTPUTS)

$(OUTPUTS): $(OBJS)
	gcc $(@:.out=.o) -o $@ $(ARGS)

%.o: %.c
	gcc -o $@ -c $< $(ARGS)

clean:
	-find . -type f -name '*.o' -delete
	-find . -type f -name '*.out' -delete
	-rm $(OUTPUTS:.out=)