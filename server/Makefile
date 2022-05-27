all:
	make libhmp221.a
	make server.o
	make server

server: libhmp221.a server.o
	g++ build/objects/release/server.o -o server -lhmp221 -Lbuild/lib/release -std=c++11
	mkdir -p build/bin/release
	mv server build/bin/release/server

libhmp221.a:
	g++ src/lib.cpp -c -Iinclude -std=c++11
	ar rs libhmp221.a lib.o 
	mkdir -p build/lib/release
	mkdir -p build/objects/release
	mv *.o build/objects/release
	mv libhmp221.a build/lib/release

server.o:
	g++ src/bin/server.cpp -c -Iinclude -Ilib -std=c++11
	mkdir -p build/objects/release
	mv server.o build/objects/release

clean:
	rm -f *.a
	rm -f *.o
	rm -rf build