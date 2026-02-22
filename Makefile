main=my_editor
src=main.c editor.h terminal.h state.h string_buffer.h util.h

.PHONY: $(src)

run: $(main)
	./$<

debug: $(main)
	gdb -p $(shell pgrep $(main))

compile: $(main)

$(main): $(src)
	gcc -Wall -Wextra -pedantic $< -g3 -o $@
