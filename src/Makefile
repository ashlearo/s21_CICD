CFLAGS=-Wall -Werror -Wextra
.PHONY: s21_cat s21_grep

all: s21_grep s21_cat #s21_cat_tests s21_grep_tests

s21_cat:
	gcc $(CFLAGS) s21_cat.c -o s21_cat

s21_grep:
	gcc $(CFLAGS) s21_grep.c -o s21_grep

#s21_grep_tests:
#	bash tests/s21_grep_tests.sh

#s21_cat_tests:
#	bash tests/s21_cat_tests.sh

clean:
	rm -f s21_cat s21_grep
