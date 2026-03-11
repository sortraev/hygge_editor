main=my_editor

.PHONY: $(main) clean

run: $(main)
	./$<

compile: $(main)

$(main): main.c
	gcc -DDEBUG -Wall -Wextra -pedantic $< -ggdb -o $@

vg: $(main)
	valgrind --log-file=valgrind.log --leak-check=full ./$(main)

debug: $(main)
	sudo gdb -p $(shell pgrep $(main))

clean:
	rm -f $(main) vgcore* valgrind.log
