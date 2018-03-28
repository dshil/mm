all:
	make buddy
	make kr
	make buddy_allocator_example
	make heap_allocator_example

buddy:
	gcc -std=c99 -Werror -Wpedantic -lm -I. \
		lib/buddy.c lib/utils.c lib/tester.c \
		test/test_buddy.c && ./a.out
	make clean

kr:
	gcc -std=c99 -Werror -Wpedantic -I. \
		lib/kr.c lib/utils.c lib/tester.c \
		test/test_kr.c && ./a.out
	make clean

buddy_allocator_example:
	g++ -Werror -Wpedantic -lm -I. \
		-x c lib/buddy.c lib/utils.c \
		-I ./_examples/cpp -x c++ \
		_examples/cpp/lib/iallocator.cpp \
		_examples/cpp/lib/heap_buddy_allocator.cpp \
		_examples/cpp/lib/heap_allocator_test.cpp \
		&& ./a.out
	make clean

heap_allocator_example:
	g++ -Werror -Wpedantic -lm -I. \
		-x c lib/buddy.c lib/utils.c \
		-I ./_examples/cpp -x c++ \
		_examples/cpp/lib/iallocator.cpp \
		_examples/cpp/lib/heap_allocator.cpp \
		_examples/cpp/lib/heap_allocator_test.cpp \
		&& ./a.out
	make clean

clean:
	rm -rf *.o *.out

