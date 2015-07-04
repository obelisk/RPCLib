// C Headers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <map>

//C++ Headers
#include <iostream>
#include <string>
#include <cstring>

// Local headers
#include "rpc.h"
#include "rpc_consts.h"
#include "util.h"

using namespace std;
char * binderAddr;
char * binderPort;
int bindDescriptor;
int serverDescriptor;
map<string, skeleton> myMap;
int setupListener() {
        int s = socket(AF_INET,SOCK_STREAM, 0);
        struct sockaddr_in sAddr;
        sAddr.sin_family = AF_INET;
        sAddr.sin_addr.s_addr = INADDR_ANY;
        sAddr.sin_port = 0;
        bind (s, (struct sockaddr *)&sAddr, sizeof(sAddr));
        return s;
}

int connectBinder() {
	struct hostent     *he;
	struct sockaddr_in  server;

	if ((he = gethostbyname(binderAddr)) == NULL) {
		return -1;
	}

	memcpy(&server.sin_addr, he->h_addr_list[0], he->h_length);
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(binderPort));
	bindDescriptor = socket(AF_INET, SOCK_STREAM, 0);

	if (connect(bindDescriptor, (struct sockaddr *)&server, sizeof(server)) != 0) {
		return -1;
	}
	printf("bindDescriptor: %d\n", bindDescriptor);
	return 0;
}
int clientInit() { 
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
    if (connectBinder() != 0) {
            printf("Could not connect to binder\n");
            perror("Connect: ");
    }else if(VERBOSE_OUTPUT == 1){
            printf("Connecting to binder succeeded2\n");
    }
	return 0;
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
	if (connectBinder() != 0) {
		printf("Could not connect to binder\n");
		perror("Connect: ");
	}else if(VERBOSE_OUTPUT == 1){
		printf("Connecting to binder succeeded\n");
	}
	serverDescriptor = setupListener();
	return 0;
}

int rpcCall(char* name, int* argTypes, void** args){
	clientInit();
	int binderMsgSize = 0; 
	binderMsgSize += sizeof(char);
	binderMsgSize += sizeof(int);
	binderMsgSize += strlen(name);
	binderMsgSize += sizeof(int);
	int argSize = 0;

	while (argTypes[argSize]) { 
		argSize++;
	}
	binderMsgSize += argSize * sizeof(int);

	char buffer[binderMsgSize];
	// writing rpc call
	char call_type = RPC_CALL;
	int counter = 0;
	memcpy(buffer+counter, &call_type, sizeof(char));
	counter += sizeof(char);

	// writting namelength
	int nameLength = strlen(name);
	char int_arr[4];
	intToArr(nameLength, int_arr);
	memcpy(buffer+counter, int_arr, 4);
	counter += sizeof(4);

	// writting name
	memcpy(buffer+counter, name, nameLength);
	counter += nameLength;

	// writting arg size
	intToArr(argSize, int_arr);
	memcpy(buffer+counter, int_arr, 4);
	counter += sizeof(4);

	// writing args
	for (int i = 0; i < argSize; i++) { 
		intToArr(argTypes[i], int_arr);
		memcpy(buffer+counter, int_arr, 4);
		counter += sizeof(4);
	}

	int written = 0, result = 0;
	while (written < binderMsgSize) { 
		result = write(bindDescriptor, buffer+written, binderMsgSize-written);
		if (result == -1) { 
			return result;
		}
		written += result;
	}

	char call;
	int result2 = 0;
	result2 = readNBytes(bindDescriptor, 1, &call);
	char hostnameLength[4];
	char * hostname;
	char port[4];
	int portNumber = 0;
	if (call == RPC_CALL) { 
		result2 = readNBytes(bindDescriptor, 4, hostnameLength);
		int length;
		arrToInt(&length, hostnameLength);
		hostname = (char*)malloc((length+1)*sizeof(char));
		hostname[length] = '\0';
		result2 = readNBytes(bindDescriptor,length, hostname);

		result2 = readNBytes(bindDescriptor,4,port);

		arrToInt(&portNumber, port);
	}
	int s = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in sAddr;
	sAddr.sin_family = AF_INET;
	struct hostent *he = gethostbyname(hostname);
	memcpy(&sAddr.sin_addr, he->h_addr_list[0], he->h_length);
	int sPort = portNumber;
	sAddr.sin_port = htons(sPort);
	int check = connect(s, (struct sockaddr *)&sAddr, sizeof(sAddr));
	if (check == -1){
		printf("Connection Failure\n");
		return -1;
	}

	int callMsgSize = 0;
	callMsgSize += sizeof(char);
        callMsgSize += sizeof(int);
        callMsgSize += strlen(name);
        callMsgSize += sizeof(int);
        int argSize2 = 0;

        while (argTypes[argSize2]) {
                argSize2++;
        }
        callMsgSize += argSize2 * sizeof(int);

	int index = 0;
	while (argTypes[index]) { 
		int variableType = (argTypes[index] >> 16) & 255;
		int variableLength = argTypes[index] & 65535;
		if (variableLength == 0) { 
			variableLength = 1;
		}
		if (variableType == 1) { 
			callMsgSize += sizeof(char)*variableLength;
		}
		if (variableType == 2) { 
			callMsgSize += sizeof(short)*variableLength;
		}
		if (variableType == 3) {
			callMsgSize += sizeof(int)*variableLength;
		} 
		if (variableType == 4) { 
			callMsgSize += sizeof(long)*variableLength;
		} 
		if (variableType == 5) { 
			callMsgSize += sizeof(double)*variableLength;
		}
		if (variableType == 6){ 
			callMsgSize += sizeof(float)*variableLength;
		}
		index++;
	}
	char callBuffer[callMsgSize];
	
	// writing rpc call
	counter = 0;
	memcpy(callBuffer+counter, &call_type, sizeof(char));
	counter += sizeof(char);

	// writting namelength
	nameLength = strlen(name);
	char int_arr2[4];
	intToArr(nameLength, int_arr2);
	memcpy(callBuffer+counter, int_arr2, 4);
	counter += sizeof(4);

	// writting name
	memcpy(callBuffer+counter, name, nameLength);
	counter += nameLength;

	// writting arg size
	intToArr(argSize, int_arr2);
	memcpy(callBuffer+counter, int_arr2, 4);
	counter += sizeof(4);

	// writing args
	for (int i = 0; i < argSize; i++) {
		intToArr(argTypes[i], int_arr2);
		memcpy(callBuffer+counter, int_arr2, 4);
		counter += sizeof(4);
	}
	int argsIndex = 0;
	
	while (argTypes[argsIndex]) {
                int variableType = (argTypes[argsIndex] >> 16) & 255;
                int variableLength = argTypes[argsIndex] & 65535;
		int size = 0;
		if (variableLength == 0) { 
			variableLength = 1;
		}
                if (variableType == 1) {
			size = sizeof(char)*variableLength; 
                }
                if (variableType == 2) {
                        size = sizeof(short)*variableLength;
                }
                if (variableType == 3) {
                        size = sizeof(int)*variableLength;
                }
                if (variableType == 4) {
                        size = sizeof(long)*variableLength;
                }
                if (variableType == 5) {
                        size = sizeof(double)*variableLength;
                }
                if (variableType == 6){
                        size = sizeof(float)*variableLength;
                }
		memcpy(callBuffer+counter, args[argsIndex], size);
		argsIndex++;
		counter += size;
        }
	written = 0, result = 0;
	while (written < callMsgSize) {
		cout << "sent " << endl;
		result = write(s, callBuffer+written, callMsgSize-written);
		cout << "result " << result << endl;
		cout << "server " << s << endl;
		if (result == -1) {
			cout << "shit " << endl; 
			return result;
		}
		written += result;
	} 

	return 0;
}

int rpcCacheCall(char* name, int* argTypes, void** args){
	return 0;
}

int rpcRegister(char* name, int* argTypes, skeleton f){
	int totalMsgSize = 0;
	totalMsgSize += sizeof(char); 	// RPC call
	totalMsgSize += sizeof(int);    // server port;
	totalMsgSize += sizeof(int); 	// Length of funtion name
	totalMsgSize += strlen(name); 	// Name
	totalMsgSize += sizeof(int); 	// Size of argsarray
	string str = name;
	int argSize = 0;
	while (argTypes[argSize]) {
		str += itoa(argTypes[argSize]); 
		argSize++;
	}	
	myMap[name] = f;	
	//The size of int thing is nice, but everything
	//will break if it's ever not 4. This is because
	//of both client and server side issues. Wishlist

	totalMsgSize += argSize * sizeof(int); // Args
	char buffer[totalMsgSize];

	char call_type = RPC_REGISTER;
	int counter = 0;
	memcpy(buffer+counter, &call_type, sizeof(char));

	counter += sizeof(char);
	int nameLength = strlen(name);
	
	// copying port
 	char int_arr[4];
	struct sockaddr_in sin;
    socklen_t addrlen = sizeof(sin);
    getsockname(serverDescriptor, (struct sockaddr *)&sin, &addrlen);
	intToArr(ntohs(sin.sin_port), int_arr);
    memcpy(buffer+counter, int_arr, sizeof(int));
	counter += sizeof(int);

	// Copy in name length
	intToArr(nameLength, int_arr); 
	memcpy(buffer+counter, int_arr, 4);
	counter += sizeof(int);

	// Copy in name
	memcpy(buffer+counter, name, nameLength);
	counter += nameLength;

	// Copy in arg size
	intToArr(argSize, int_arr); 
	memcpy(buffer+counter, int_arr, 4);
	counter += sizeof(int);

	// Copy in args
	for (int i = 0; i < argSize; i++) {
		intToArr(argTypes[i], int_arr); 
		memcpy(buffer+counter, int_arr, 4);
		counter += sizeof(int);
	}

	// Write loop
	int written = 0, result = 0;
	while(written < totalMsgSize){
		result = write(bindDescriptor, buffer+written, totalMsgSize-written);
		if(result == -1){return result;}
		written += result;
	}
	// Return number of bytes written
	return written;
}

int rpcExecute(){
	listen(serverDescriptor, 5);
	struct sockaddr_in csock_s;
	socklen_t len = sizeof(csock_s);
	fd_set master;
	fd_set read_fds;
	int fdmax;
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	FD_SET(serverDescriptor, &master);
	fdmax = serverDescriptor;
	for(;;) { 
		read_fds = master;
		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) { 
			exit(4);
		}
		for (int i = 0; i <= fdmax; i++) { 
			if (FD_ISSET(i, &read_fds)) {
				if (i == serverDescriptor) { 
					int newfd = accept(serverDescriptor, (struct sockaddr*)&csock_s, (socklen_t*)&len);
					if (VERBOSE_OUTPUT == 1) { 
						cout << "new connection s is " << serverDescriptor<< endl;				 
					}
					if (newfd == -1) { 

					}
					FD_SET(newfd, &master);
					if (newfd > fdmax) { 
						fdmax = newfd;
					}
				}
				else { 
					cout << "Reading " << endl;
					char call;
					int result = readNBytes(i, 1, &call);
					cout << "got rpc call " << endl;
					if (result == -1) { 
						close(i);
						FD_CLR(i, &master);
						break;
					}
					char* f_data;
					string name;
					char len_buffer[4];
					int length = 0;
					int param_count = 0;
					if (call == RPC_CALL) { 
						cout << "got rpc call " << endl;
						if (readNBytes(i, 4, len_buffer) == -1) {
							cout << "shit " << endl; 
							close(i);
							FD_CLR(i, &master);
							break;
						}
						cout << "got length" << endl;	
						arrToInt(&length, len_buffer);
						cout << "copied length correctly " << length << endl;
						f_data = (char*)malloc((length+1) * sizeof(char));
						f_data[length] = '\0';
						result = readNBytes(i, length, f_data);
						cout << "result " << result << endl;
						if (result == -1) { 
							free(f_data);
							break;
						}
						cout << "got name" << endl;
						name = string(f_data, length);
						cout << "function name " << name << endl;
						free(f_data);
						
						if (readNBytes(i, 4, len_buffer) == -1) { 
							close(i);
							FD_CLR(i, &master);
							break;
						} 	
						arrToInt(&param_count, len_buffer);
						param_t * params = (param_t *)malloc(sizeof(param_t) * param_count);
						int bad = 0;
						int param;
						for (int i = 0; i < param_count; ++i) { 
							if(readNBytes(i, 4, len_buffer) == -1) { 
								bad = 1;
								FD_CLR(i, &master);
								close (i);
								break;
							}
							arrToInt(&param, len_buffer);
							params[i].input = (unsigned char)((param & INPUT_BIT) > 0);
							params[i].output = (unsigned char)((param & OUTPUT_BIT) > 0);
							params[i].type = (0x00FF0000 & param) >> 16;
							params[i].length = 0x0000FFFF & param;
						}					
						if (bad == 1) { 
							free(params);
							break;
						}
						
					}
					
				}
			}
		}
	}
	return 0;
}

int rpcTerminate(){
	return 0;
}
