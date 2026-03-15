bin=my_editor

compile: $(bin)

SRCS = ${wildcard *.c}
OBJS = ${SRCS:.c=.o}

CCFLAGS=-ggdb -Wall -Wextra -pedantic
CCFLAGS+=-DDEBUG

.PHONY: main.h

%.o: %.c %.h
	$(CC) $(CCFLAGS) -c $<

$(bin): $(OBJS)
	$(CC) -o $@ $^

vg: $(bin)
	valgrind --log-file=valgrind.log --leak-check=full ./$(bin)

debug: $(bin)
	sudo gdb -p $(shell pgrep $(bin))

clean:
	rm -f $(bin) *.o *.gch vgcore* valgrind.log
