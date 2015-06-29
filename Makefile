all: library

clean:
	rm -rf build/*

library: 
	g++ -Iinclude -Wall -g -c -o build/librpc.o rpc/rpc.cc
	ar rcs build/librpc.a build/librpc.o
	rm build/*.o

client: build/librpc.a
	g++ -Iinclude -Wall -g -o build/client client/client1.c build/librpc.a

server: build/librpc.a
	g++ -Iinclude -Wall -g -o build/server server/server.c server/server_function_skels.c server/server_functions.c build/librpc.a