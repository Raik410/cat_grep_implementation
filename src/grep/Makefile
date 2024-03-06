FLAGS=-Wall -Wextra -Werror

all: s21_grep

s21_grep:
	gcc $(FLAGS) s21_grep.c -o s21_grep

check:
	clang-format -n ./*.c ./*.h

clang:
	clang-format -i ./*.c ./*.h

cpp:
	cppcheck --enable=all --suppress=missingIncludeSystem --std=c11 *.c *.h

val:
	valgrind --tool=memcheck --leak-check=yes ./s21_grep Biba text.txt

clean:
	rm s21_grep

test: 
	./tests.sh

test_leaks:
	./test_leaks.sh

chmod:
	chmod +x tests.sh

check_full: check cpp val chmod test