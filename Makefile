main=my_editor
src=main.c editor.h init.h state.h string_buffer.h

.PHONY: $(src)

run: $(main)
	./$<

debug: $(main)
	gdb -p $(shell pgrep $(main))

compile: $(main)

$(main): $(src)
	gcc -Wall -pedantic $< -g3 -o $@
