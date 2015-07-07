all: library

clean:
	-rm -rf build/*

library:
	-rm -rf build/*.a
	clang++ --std=c++11 -pthread -Iinclude -Wall -g -c rpc/rpc.cc -o build/librpc.o
	clang++ -Iinclude -Wall -g -c rpc/util.cc -o build/util.o
	ar rcs build/librpc.a build/librpc.o build/util.o
	rm build/*.o

client: build/librpc.a
	clang++ -Iinclude -pthread -Wall -g -o build/client client/client1.c build/librpc.a

server: build/librpc.a
	-rm build/server
	-rm -rf build/server.dSYM
	clang++ -Iinclude -pthread -Wall -g -o build/server server/server.c server/server_function_skels.c server/server_functions.c build/librpc.a

server-alt: build/librpc.a
	-rm build/server-alt
	-rm -rf build/server-alt.dSYM
	clang++ -Iinclude -Wall -g -o build/server-alt server-alt/server.c server-alt/server_function_skels.c server-alt/server_functions.c build/librpc.a


binder: build/librpc.a
	-rm build/binder
	-rm -rf build/binder.dSYM
	clang++ -Iinclude -Wall -g -o build/binder binder/binder.cc build/librpc.a

format:
	clang-format -i rpc/rpc.cc
	clang-format -i rpc/util.cc
	clang-format -i include/util.h
	clang-format -i binder/binder.cc
	clang-format -i binder/binder.h
