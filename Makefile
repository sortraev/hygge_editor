main=my_editor
src=main.c editor.h terminal.h state.h string_buffer.h util.h lines.h

.PHONY: $(src) clean

run: $(main)
	./$<

debug: $(main)
	gdb -p $(shell pgrep $(main))

compile: $(main)

vg: $(main)
	valgrind ./$(main)

$(main): $(src)
	gcc -Wall -Wextra -pedantic $< -g3 -o $@

clean:
	rm -f $(main) vgcore*
