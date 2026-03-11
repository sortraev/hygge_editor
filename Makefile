main=my_editor
src=main.c editor.h terminal.h editor_state.h string_buffer.h util.h lines.h

.PHONY: $(src) clean

run: $(main)
	./$<

debug: $(main)
	sudo gdb -p $(shell pgrep $(main))

compile: $(main)

vg: $(main)
	valgrind --log-file=valgrind.log --leak-check=full ./$(main)

$(main): $(src)
	gcc -Wall -Wextra -pedantic $< -ggdb -o $@

clean:
	rm -f $(main) vgcore* valgrind.log
