
all: make run

main.hex:
	make -C ../hello
	cp ../hello/$@ .

make: libcintelhex/bin/libcintelhex.so
	gcc main.c

libcintelhex/bin/libcintelhex.so:
	cd libcintelhex && autoconf 
	cd libcintelhex && ./configure
	cd libcintelhex && make

run: main.hex
	./a.out main.hex
