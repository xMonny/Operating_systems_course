ifndef CC
	CC=gcc
endif
CFLAGS=-std=c99 -Werror -Wall -Wpedantic -Wextra
SRCS=foo.c bar.c
OBJS=$(subst .c,.o,$(SRCS))
RM=rm -f

all: foo bar

foo: foo.o
	$(CC) $(CFLAGS) -o foo foo.c

bar: bar.o
	$(CC) $(CFLAGS) -o bar bar.c

clean:
	$(RM) $(OBJS) foo bar

