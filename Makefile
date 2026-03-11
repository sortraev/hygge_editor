main=my_editor
src=main.c editor.h editor_state.h screen.h string_buffer.h util.h terminal.h

.PHONY: $(src) clean

run: $(main)
	./$<

debug: $(main)
	sudo gdb -p $(shell pgrep $(main))

compile: $(main)

vg: $(main)
	valgrind --log-file=valgrind.log --leak-check=full ./$(main)

$(main): $(src)
	gcc -DDEBUG -Wall -Wextra -pedantic $< -ggdb -o $@

clean:
	rm -f $(main) vgcore* valgrind.log
