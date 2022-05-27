static:
	mkdir -p build
	mkdir -p build/lib
	mkdir -p build/lib/release
	mkdir -p build/objects
	g++ lib.cpp -c
	ar rs liblinkedlist.a lib.o
	mv *.o build/objects
	mv *.a build/lib/release
shared:
	mkdir -p build
	mkdir -p build/lib
	mkdir -p build/lib/release
	mkdir -p build/objects
	g++ -fPIC lib.cpp -c
	g++ -shared -o liblinkedlist.so lib.o
	mv *.o build/objects
	mv *.so build/lib/release
debug:
	mkdir -p build
	mkdir -p build/lib
	mkdir -p build/lib/release
	mkdir -p build/objects
	g++ -g -fPIC lib.cpp -c
	g++ -shared -o liblinkedlist.so lib.o
	mv *.o build/objects
	mv *.so build/lib/release
clean:
	rm -rf build
