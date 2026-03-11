tests: string_buffer_tests editor_state_tests

.PHONY: tests

%: %.c
	gcc -ggdb -I../ -o $@ $^ && ./$@

clean:
	rm -rf vgcore* string_buffer_tests editor_state_tests
