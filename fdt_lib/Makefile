CC = gcc
CFLAGS = -Wall -g 
LDFLAGS =

SRCS = fdt_lib_header.c fdt_lib_mem_rev.c fdt_lib_struct.c fdt_lib_parse.c fdt_lib_test_parser.c
OBJS = $(SRCS:.c=.o)
DEPS = fdt_lib.h fdt_lib_header.h fdt_lib_mem_rev.h fdt_lib_struct.h fdt_lib_parse.h

TARGET = fdt_lib_test

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)