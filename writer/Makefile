
CFLAGS=
CFLAGS+=-Ilibcintelhex/include

LDLIBS=
LDLIBS+=-Llibcintelhex/bin/ -lcintelhex

all: make run

main.hex:
	make -C ../hello
	cp ../hello/$@ .

make: libcintelhex/bin/libcintelhex.so
	gcc -o prog main.c $(CFLAGS) $(LDLIBS)

libcintelhex/bin/libcintelhex.so:
	cd libcintelhex && autoconf 
	cd libcintelhex && ./configure
	cd libcintelhex && make

run: main.hex
	LD_LIBRARY_PATH=`pwd`/libcintelhex/bin/ ./prog main.hex
