CC = gcc
CFLAGS = -Wall -g #-Wextra
LDFLAGS =

SRCS = fdt_lib_header.c fdt_lib_mem_rev_block.c fdt_lib_test_parser.c fdt_lib_tools.c
OBJS = $(SRCS:.c=.o)
DEPS = fdt_lib.h fdt_lib_header.h fdt_lib_mem_rev_block.h fdt_lib_tools.h

TARGET = fdt_lib_test

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)