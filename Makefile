all: library

clean:
	-rm -rf build/*

library:
	-rm -rf build/*.a
	g++ -Iinclude -std=c++0x -Wall -g -c rpc/rpc.cc -o build/librpc.o
	g++ -Iinclude -Wall -g -c rpc/util.cc -o build/util.o
	ar rcs build/librpc.a build/librpc.o build/util.o
	rm build/*.o

client: build/librpc.a
	g++ -Iinclude -Wall -g -o build/client client/client1.c build/librpc.a

server: build/librpc.a
	-rm build/server
	-rm -rf build/server.dSYM
	g++ -Iinclude -Wall -g -o build/server server/server.c server/server_function_skels.c server/server_functions.c build/librpc.a

binder: build/librpc.a
	-rm build/binder
	-rm -rf build/binder.dSYM
	g++ -Iinclude -Wall -g -o build/binder binder/binder.cc build/librpc.a
