CFLAGS = -g -Wall -O3 -D_GNU_SOURCE -DNDEBUG -std=gnu99
LDLIBS+= -levent

all: xiod stress_test

xiod: xiod.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

stress_test: stress_test.c
	$(CC) $(CFLAGS) -o $@ $^
