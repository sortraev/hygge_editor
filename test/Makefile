TEST_SRC = ${wildcard *.c}
TEST_BINS = ${TEST_SRC:.c=}

.PHONY: $(TEST_SRC)

run_tests: $(TEST_BINS)

%: %.c
	@gcc -DDEBUG -ggdb -I../src -o $@ $^ \
		&& ./$@

clean:
	rm -rf vgcore* $(TEST_BINS)
