main=my_editor
src=main.c init.h state.h

.PHONY: $(src)

run: $(main)
	./$<

debug: $(main)
	gdb -p $(shell pgrep $(main))

compile: $(main)

$(main): $(src)
	gcc -Wall -pedantic $< -O3 -o $@
