s21_cat: s21_cat.o
	gcc s21_cat.o -o s21_cat
	
s21_cat.o: s21_cat.c 
	gcc -Wall -Werror -Wextra -c s21_cat.c 

clean:
	rm -rf *.o s21_cat

rebuild:
	make clean
	make s21_cat

leaks: s21_cat
	sh test_leak_cat.sh

check: s21_cat
	sh test_func_cat.sh
	clang-format -style=google -n s21_cat.c
