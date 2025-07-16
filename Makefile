CC := gcc
CUNIT_DIR := cunit
CUNIT_FLAGS := -I$(CUNIT_DIR)/CUnit -L$(CUNIT_DIR)/build/CUnit -lcunit

test: test_*.c
	@for file in $^ ; do \
		$(CC) -D_GNU_SOURCE $${file} $(CUNIT_FLAGS) -o test.out ; \
		./test.out ; \
	done
	@rm -r test.out*

format:
	clang-format -style=Microsoft *.h *.c -i
