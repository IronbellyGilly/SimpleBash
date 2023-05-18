CI = gcc
CC = $(CI) $(OPT)

all :s21_grep 
	rm -rf *.o

s21_grep: s21_grep.o
	$(CC) s21_grep.o -o s21_grep

s21_grep.o: s21_grep.c 
	$(CC) -c $< -o $@

clean: 
	rm -rf *.o
	rm -rf s21_grep
	rm -rf *.log

rebuild:
	rm -rf *.o 
	rm s21_grep
	make

leaks: s21_grep
	leaks -atExit -- ./s21_grep

check: s21_grep
	sh test_func_grep.sh
	clang-format -style=google -n s21_grep.c