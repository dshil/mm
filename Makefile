LINTFLAGS = \
	 -Werror \
	 -Wextra \
	 -Wcast-qual \
	 -Wpointer-arith \
	 -Wpedantic

CFLAGS = -std=c99

all:
	make buddy
	make kr
	make pool
	make buddy_allocator_example
	make heap_allocator_example

buddy:
	gcc -I. $(CFLAGS) $(LINTFLAGS) \
		lib/buddy.c lib/utils.c lib/tester.c test/test_buddy.c \
		-lm \
		&& ./a.out
	make clean

pool:
	gcc -I. $(CFLAGS) $(LINTFLAGS) \
		lib/pool.c lib/utils.c test/test_pool.c \
		&& ./a.out
	make clean

kr:
	gcc -I. $(CFLAGS) $(LINTFLAGS) \
		lib/kr.c lib/utils.c lib/tester.c test/test_kr.c \
		&& ./a.out
	make clean

buddy_allocator_example:
	g++ $(LINTFLAGS) \
		-I. -x c lib/buddy.c lib/utils.c \
		-I ./_examples/cpp -x c++ \
		_examples/cpp/lib/iallocator.cpp \
		_examples/cpp/lib/heap_buddy_allocator.cpp \
		_examples/cpp/lib/heap_allocator_test.cpp \
		-lm \
		&& ./a.out
	make clean

heap_allocator_example:
	g++ $(LINTFLAGS) \
		-I ./_examples/cpp -x c++ \
		_examples/cpp/lib/iallocator.cpp \
		_examples/cpp/lib/heap_allocator.cpp \
		_examples/cpp/lib/heap_allocator_test.cpp \
		&& ./a.out
	make clean

clean:
	rm -rf *.o *.out

