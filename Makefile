CC := gcc
CUNIT_DIR := cunit
CUNIT_FLAGS := -I$(CUNIT_DIR)/CUnit -L$(CUNIT_DIR)/build/CUnit -lcunit

test: $(wildcard test_*.c)
	$(CC) $? $(CUNIT_FLAGS) -o test.out && ./test.out
	rm -r test.out*

format:
	clang-format -style=Microsoft *.h *.c -i
