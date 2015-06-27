#include "rpc.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>
using namespace std;
char * binderAddr;
char * binderPort;
int bindDescriptor;
int connectBinder() {
	int bindDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in bAddr;
	bAddr.sin_family = AF_INET;
	struct hostent *he = gethostbyname(binderAddr);
	memcpy(&bAddr.sin_addr, he->h_addr_list[0], he->h_length);
	int bPort = atoi(binderPort);
	bAddr.sin_port = htons(bPort);
	return connect(bindDescriptor, (struct sockaddr *)&bAddr, sizeof(bAddr)); 
	
}
int rpcInit(){
	binderAddr = getenv("BINDER_ADDRESS");
	binderPort = getenv("BINDER_PORT");
	if (binderAddr == NULL) { 
		cerr << "Env variable BINDER_ADDRESS not found" << endl;
		return 1;
	}
	if (binderPort == NULL) { 
		cerr << "Env variable BINDER_PORT not found" << endl;
		return 1;
	}
//	if (connectBinder == ) { 
//
//	}
//	if (setupListener == ) {
//
//	}		
	return 0;
}

int rpcCall(char* name, int* argTypes, void** args){
	return 0;
}

int rpcCacheCall(char* name, int* argTypes, void** args){
	return 0;
}

int rpcRegister(char* name, int* argTypes, skeleton f){
	int totalMsgSize = 0;
	totalMsgSize = sizeof(int) + totalMsgSize; // rpc type
	totalMsgSize = sizeof(int) + totalMsgSize; // length of name
	totalMsgSize = strlen(name) + totalMsgSize; // name
	totalMsgSize = sizeof(int) + totalMsgSize; // size of argsarray
	int argSize = 0;
	while (argTypes[argSize]) { 
		argSize++;
	}	
	totalMsgSize = argSize * sizeof(int) + totalMsgSize; // args
	totalMsgSize = sizeof(int) + totalMsgSize; // 0
	totalMsgSize = sizeof(int) + totalMsgSize; // skeleton int*
	totalMsgSize = sizeof(int) + totalMsgSize; // skeleton void**
	char buffer[totalMsgSize*2];
	int n = 1;
	int counter = 0;
	memcpy(buffer, &n, sizeof(int));
	counter = counter + sizeof(int);
	int nameLength = 0;
	while (name[nameLength]) { 
		nameLength++;
	}
	memcpy(buffer+counter, &nameLength, sizeof(int));
	counter = counter + sizeof(int);
	memcpy(buffer+counter, name, nameLength);
	counter = counter + nameLength;
	memcpy(buffer+counter, &argSize, sizeof(int));
	counter = counter + sizeof(int);
	for (int i = 0; i < argSize; i++) { 
		memcpy(buffer+counter, &argTypes[i], sizeof(int));
		counter = counter + sizeof(int);
	}	

	return 0;
}

int rpcExecute(){
	return 0;
}

int rpcTerminate(){
	return 0;
}
