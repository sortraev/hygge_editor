bin=my_editor

compile: $(bin)

SRCS = ${wildcard *.c}
OBJS = ${SRCS:.c=.o}

%.o: %.c
	gcc -c $^

$(bin): $(OBJS)
	gcc -o $@ $^

vg: $(bin)
	valgrind --log-file=valgrind.log --leak-check=full ./$(bin)

debug: $(bin)
	sudo gdb -p $(shell pgrep $(bin))

clean:
	rm -f $(bin) *.o vgcore* valgrind.log
