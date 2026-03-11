tests: string_buffer_tests editor_state_tests

.PHONY: tests

%: %.c
	gcc -DDEBUG -ggdb -I../src -o $@ $^ && ./$@

clean:
	rm -rf vgcore* string_buffer_tests editor_state_tests
