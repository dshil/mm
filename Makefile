CC := gcc
CFLAGS := -std=c99
CPPFLAGS := -std=c++11
MODULE := memory
LINTFLAGS = \
	    -Werror \
	    -Wextra \
	    -Wcast-qual \
	    -Wpointer-arith \
	    -Wpedantic

SDIR := src/modules/$(MODULE)
ODIR := build/modules/$(MODULE)
SRC_FILES := $(wildcard $(SDIR)/*.c)
OBJ_FILES := $(patsubst $(SDIR)/%.c,$(ODIR)/%.o,$(SRC_FILES))

TSDIR := src/tests/$(MODULE)
TODIR := build/tests/$(MODULE)
TSRC_FILES := $(wildcard $(TSDIR)/*.c)
TOBJ_FILES := $(patsubst $(TSDIR)/%.c,$(TODIR)/%.o,$(TSRC_FILES))

ESDIR := _examples/cpp
EODIR := build/_examples/cpp
ESRC_FILES := $(wildcard $(ESDIR)/*.cpp)
EOBJ_FILES := $(patsubst $(ESDIR)/%.cpp,$(EODIR)/%.o,$(ESRC_FILES))

.PHONY: $(MODULE) tests clean prep examples buddy kr pool \
	buddy_allocator_example heap_allocator_example

all:
	make buddy
	make kr
	make pool
	make buddy_allocator_example
	make heap_allocator_example

prep:
	mkdir -p build/modules/$(MODULE); \
	mkdir -p tests/modules/$(MODULE); \
	mkdir -p $(EODIR)

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) -c $< $(CFLAGS) $(LINTFLAGS) -o $@

$(TODIR)/%.o: $(TSDIR)/%.c
	$(CC) -I ./src/modules -c $< $(CFLAGS) $(LINTFLAGS) -o $@

$(MODULE): $(OBJ_FILES)

$(EODIR)/%.o: $(ESDIR)/%.cpp
	g++ -I ./src/modules -c $< $(CPPFLAGS) $(LINTFLAGS) -o $@

buddy: $(TOBJ_FILES) $(OBJ_FILES)
	make prep
	gcc $(CFLAGS) $(ODIR)/utils.o $(ODIR)/buddy.o $(TODIR)/tester.o \
		$(TODIR)/test_buddy.o -lm; ./a.out; rm -rf ./a.out;

kr: $(TOBJ_FILES) $(OBJ_FILES)
	make prep
	gcc $(CFLAGS) $(ODIR)/utils.o $(ODIR)/kr.o $(TODIR)/tester.o \
		$(TODIR)/test_kr.o; ./a.out; rm -rf ./a.out;

pool: $(TOBJ_FILES) $(OBJ_FILES)
	make prep
	gcc $(CFLAGS) $(ODIR)/utils.o $(ODIR)/pool.o $(TODIR)/test_pool.o; \
		./a.out; rm -rf ./a.out;

buddy_allocator_example: $(EOBJ_FILES) $(OBJ_FILES)
	make prep
	g++ $(ODIR)/utils.o $(ODIR)/buddy.o \
		$(EODIR)/iallocator.o $(EODIR)/heap_buddy_allocator.o \
		$(EODIR)/heap_allocator_test.o -lm; ./a.out; rm -rf ./a.out

heap_allocator_example: $(EOBJ_FILES)
	make prep
	g++ $(EODIR)/iallocator.o $(EODIR)/heap_allocator.o \
		$(EODIR)/heap_allocator_test.o -lm; ./a.out; rm -rf ./a.out

clean:
	rm -rf $(ODIR)/*.o $(TODIR)/*.o $(EODIR)/*.o
