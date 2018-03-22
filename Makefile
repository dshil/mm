all:
	make buddy
	make kr

buddy:
	gcc -std=c99 -Werror buddy.c utils.c tester.c test_buddy.c && ./a.out
	make clean

kr:
	gcc -std=c99 -Werror kr.c utils.c tester.c test_kr.c && ./a.out
	make clean

clean:
	rm -rf *.o *.out

