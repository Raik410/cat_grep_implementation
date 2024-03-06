CC = gcc
FLAGS = -Wall -Wextra -Werror -std=c11 --pedantic
EF = s21_cat.c s21_cat.h
FF = -o s21_cat

all: s21_cat

s21_cat: $(EF)
	$(CC) $(FLAGS) $(EF) $(FF)

clear:
	rm -rf s21_cat

check:
	clang-format -n ./*.c ./*.h

clang:
	clang-format -i ./*.c ./*.h

cpp:
	cppcheck --enable=all --suppress=missingIncludeSystem --std=c11 *.c *.h

val:
	valgrind --tool=memcheck --leak-check=yes ./s21_cat tests/test_case_cat.txt -benstvET

test:
	./tests/test_cat.sh

test_full: check cpp val

