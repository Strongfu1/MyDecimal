GCC=-Wall -Wextra -Werror -g
CHKMK=clean_mode=1
OS=$(shell uname)
ifeq ($(OS), Linux)
	LIBS=-lcheck -lm -lsubunit -lrt -lpthread
else
	LIBS=-lcheck
endif

main: s21_decimal.a

all: s21_decimal.a gcov_report

s21_decimal.o: s21_decimal.c s21_decimal.h
	gcc -c s21_decimal.c -lm

s21_decimal.a: s21_decimal.o 
	ar rcs s21_decimal.a s21_decimal.o

test: clean 
	checkmk $(CHKMK) SharpTestsBig.check > test.c
	gcc --coverage -c s21_decimal.c $(LIBS)
	ar rcs s21_decimal.a s21_decimal.o
	gcc -c test.c $(LIBS)
	gcc --coverage -o test test.o s21_decimal.a $(LIBS)
	./test

gcov_report: test
	lcov -t "gcov_report" -o s21_decimal.info -c -d .
	genhtml -o gcov_report s21_decimal.info

clean:
	rm -rf *.o *.a *.gcda *.gcno *.info gcov_report tests a.out *.exe Output.txt test test.c cpplint.py CPPLINT.cfg

check: 
	cp ../materials/linters/CPPLINT.cfg CPPLINT.cfg
	cp ../materials/linters/cpplint.py cpplint.py
	python3 cpplint.py --extensions=c s21_decimal.c
	cppcheck --enable=all s21_decimal.c --suppress=unusedFunction --suppress=invalidPointerCast
	







.PHONY: clean check s21_decimal.a s21_decimal.o test gcov_report