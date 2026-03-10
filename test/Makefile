tests: string_buffer_tests lines_tests

.PHONY: tests

%: %.c
	@gcc -ggdb -I../ -o $@ $^ && ./$@

clean:
	rm -rf vgcore* string_buffer_tests lines_tests
