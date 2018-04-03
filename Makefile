LINTFLAGS = \
	 -Werror \
	 -Wextra \
	 -Wcast-qual \
	 -Wpointer-arith \
	 -Wpedantic

CFLAGS = -std=c99
CPPFLAGS = -std=c++98

all:
	make prep
	make buddy
	make kr
	make pool
	make buddy_allocator_example
	make heap_allocator_example

prep:
	mkdir -p build; \
	gcc -c -I . $(CFLAGS) $(LINTFLAGS) lib/buddy.c lib/utils.c; \
	ar crs build/libbuddy.a buddy.o utils.o; \
	rm -rf buddy.o utils.o

buddy:
	make prep
	gcc -I. -L ./build $(CFLAGS) $(LINTFLAGS) \
		lib/tester.c test/test_buddy.c -lbuddy -lm; ./a.out
	make clean

pool:
	gcc -I. $(CFLAGS) $(LINTFLAGS) \
		lib/pool.c lib/utils.c test/test_pool.c; ./a.out
	make clean

kr:
	gcc -I. $(CFLAGS) $(LINTFLAGS) \
		lib/kr.c lib/utils.c lib/tester.c test/test_kr.c; ./a.out
	make clean

buddy_allocator_example:
	make prep
	g++ $(LINTFLAGS) $(CPPFLAGS) -c -I. -I ./_examples/cpp \
		_examples/cpp/lib/iallocator.cpp \
		_examples/cpp/lib/heap_buddy_allocator.cpp \
		_examples/cpp/lib/heap_allocator_test.cpp; \
	ar crs build/libbuddycpp.a \
		iallocator.o heap_buddy_allocator.o heap_allocator_test.o; \
	g++ -L ./build -lbuddycpp -lbuddy -lm; ./a.out
	make clean

heap_allocator_example:
	g++ $(LINTFLAGS) $(CPPFLAGS) \
		-I ./_examples/cpp \
		_examples/cpp/lib/iallocator.cpp \
		_examples/cpp/lib/heap_allocator.cpp \
		_examples/cpp/lib/heap_allocator_test.cpp; \
		./a.out
	make clean

clean:
	rm -rf *.o *.out

