CFLAGS = -g -Wall -O3 -D_GNU_SOURCE -DNDEBUG -std=gnu99
LDLIBS+=-lm

.PHONY: all clean run

TARGETS=libfable.so test

all: $(TARGETS)

libfable.so: fable.o atomicio.o xutil.o
	$(LD) -shared -soname $@ -o $@ -lc $^ -lrt -lnuma

test: test.c
	$(CC) $(CFLAGS) -o $@ $^ libfable.so

#%_lat: atomicio.o xutil.o %_lat.o
#	$(CC) $(CFLAGS) -o $@ $^ -lrt -lnuma
#
#%_thr: atomicio.o xutil.o %_thr.o
#	$(CC) $(CFLAGS) -o $@ $^ -lrt -lnuma

clean:
	rm -f *~ core *.o $(TARGETS)
