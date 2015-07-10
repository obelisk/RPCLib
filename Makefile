.PHONY: library release

clean:
	-rm -rf build/*
	-rm -rf release/*

library:
	-rm -rf build/*.a
	clang++ -pthread -Iinclude -Wall -g -c rpc/rpc.cc -o build/librpc.o
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

release:
	# Clean
	-rm -rf release/*
	# Library Compile
	clang++ --std=c++11 -Iinclude -Wall -pthread -c rpc/rpc.cc -o release/librpc.o
	clang++ --std=c++11 -Iinclude -Wall -c rpc/util.cc -o release/util.o
	# Library Package
	ar rcs release/librpc.a release/librpc.o release/util.o
	# Clean
	rm release/*.o
	# Binder
	clang++ -Iinclude -Wall -o release/binder binder/binder.cc release/librpc.a
	# Copy Header
	cp include/rpc.h release/rpc.h


releasecopy:
	-rm build/librpc.a
	cp release/librpc.a build/librpc.a
	cp release/binder build/binder