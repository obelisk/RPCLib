// C Headers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// C++ Headers
#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <vector>

// Local headers
#include "rpc.h"
#include "rpc_consts.h"
#include "util.h"

using namespace std;
char *binderAddr;
char *binderPort;
int bindDescriptor;
int serverDescriptor;
int serverPort;
map<string, skeleton> myMap;

int setupListener() {
	int s = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in sAddr;
	sAddr.sin_family = AF_INET;
	sAddr.sin_addr.s_addr = INADDR_ANY;
	sAddr.sin_port = 0;
	int success;
	success = bind(s, (struct sockaddr *)&sAddr, sizeof(sAddr));
	if(success == -1){
		perror("Bind: ");
		return -1;
	}

	socklen_t addrlen = sizeof(sAddr);
	success = getsockname(s, (struct sockaddr *)&sAddr, &addrlen);
	if(success == -1){
		perror("getsockname: ");
		return -1;
	}
	if(VERBOSE_OUTPUT == 1){
		printf("Server is Waiting on Port: %d\n", ntohs(sAddr.sin_port));
	}
	serverPort = ntohs(sAddr.sin_port);
	return s;
}

int connectBinder() {
	struct hostent *he;
	struct sockaddr_in server;

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
	} else if (VERBOSE_OUTPUT == 1) {
		printf("Client Init: Connected To Binder\n");
	}
	return 0;
}

int rpcInit() {
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
	} else if (VERBOSE_OUTPUT == 1) {
		printf("RPC Init: Connected To Binder\n");
	}
	serverDescriptor = setupListener();
	return 0;
}

int rpcCall(char *name, int *argTypes, void **args) {
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
	memcpy(buffer + counter, &call_type, sizeof(char));
	counter += sizeof(char);

	// writting namelength
	int nameLength = strlen(name);
	char int_arr[4];
	intToArr(nameLength, int_arr);
	memcpy(buffer + counter, int_arr, 4);
	counter += sizeof(4);

	// writting name
	memcpy(buffer + counter, name, nameLength);
	counter += nameLength;

	// writting arg size
	intToArr(argSize, int_arr);
	memcpy(buffer + counter, int_arr, 4);
	counter += sizeof(4);

	// writing args
	for (int i = 0; i < argSize; i++) {
		intToArr(argTypes[i], int_arr);
		memcpy(buffer + counter, int_arr, 4);
		counter += sizeof(4);
	}

	int written = 0, result = 0;
	while (written < binderMsgSize) {
		result = write(bindDescriptor, buffer + written, binderMsgSize - written);
		if (result == -1) {
			printf("Failed writing to binder\n");
			return result;
		}
		written += result;
	}

	char call;
	int result2 = 0;
	result2 = readNBytes(bindDescriptor, 1, &call);
	char hostnameLength[4];
	char *hostname;
	char port[4];
	int portNumber = 0;
	// if (call == RPC_CALL) {
	result2 = readNBytes(bindDescriptor, 4, hostnameLength);
	int hlength;
	arrToInt(&hlength, hostnameLength);
	// This needs to be freed
	hostname = (char *)malloc((hlength + 1) * sizeof(char));
	hostname[hlength] = '\0';
	result2 = readNBytes(bindDescriptor, hlength, hostname);
	result2 = readNBytes(bindDescriptor, 4, port);
	arrToInt(&portNumber, port);
	// Done with binder
	if (VERBOSE_OUTPUT == 1) {
		printf("Binder Responsed with Host: %s, Port: %d\n", hostname, portNumber);
		printf("Closing Connection To Binder.\n");
	}
	close(bindDescriptor);
	//}

	int s = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in sAddr;
	sAddr.sin_family = AF_INET;
	struct hostent *he = gethostbyname(hostname);
	memcpy(&sAddr.sin_addr, he->h_addr_list[0], he->h_length);
	int sPort = portNumber;
	sAddr.sin_port = htons(sPort);
	int check = connect(s, (struct sockaddr *)&sAddr, sizeof(sAddr));
	if (check == -1) {
		perror("Connection Failure");
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
			callMsgSize += sizeof(char) * variableLength;
		}
		if (variableType == 2) {
			callMsgSize += sizeof(short) * variableLength;
		}
		if (variableType == 3) {
			callMsgSize += sizeof(int) * variableLength;
		}
		if (variableType == 4) {
			callMsgSize += sizeof(long) * variableLength;
		}
		if (variableType == 5) {
			callMsgSize += sizeof(double) * variableLength;
		}
		if (variableType == 6) {
			callMsgSize += sizeof(float) * variableLength;
		}
		index++;
	}
	char callBuffer[callMsgSize];
	// writing rpc call
	counter = 0;
	memcpy(callBuffer + counter, &call_type, sizeof(char));
	counter += sizeof(char);

	// writting namelength
	nameLength = strlen(name);
	char int_arr2[4];
	intToArr(nameLength, int_arr2);
	memcpy(callBuffer + counter, int_arr2, 4);
	counter += sizeof(4);

	// writting name
	memcpy(callBuffer + counter, name, nameLength);
	counter += nameLength;

	// writting arg size
	intToArr(argSize, int_arr2);
	memcpy(callBuffer + counter, int_arr2, 4);
	counter += sizeof(4);

	// writing args
	for (int i = 0; i < argSize; i++) {
		intToArr(argTypes[i], int_arr2);
		memcpy(callBuffer + counter, int_arr2, 4);
		counter += sizeof(4);
	}
	int argsIndex = 0;
	int size = 0;
	while (argTypes[argsIndex]) {
		int variableType = (argTypes[argsIndex] >> 16) & 255;
		int variableLength = argTypes[argsIndex] & 65535;
		if (variableLength == 0) {
			variableLength = 1;
		}
		if (variableType == 1) {
			size = sizeof(char) * variableLength;
		}
		if (variableType == 2) {
			size = sizeof(short) * variableLength;
		}
		if (variableType == 3) {
			size = sizeof(int) * variableLength;
			cout << "b1 " << *(int*)args[argsIndex] << endl; 
			int testValue = 0;
			memcpy(&testValue, args[argsIndex], sizeof(int));
			cout << "b2 " << testValue << endl;
		}
		if (variableType == 4) {
			size = sizeof(long) * variableLength;
		}
		if (variableType == 5) {
			size = sizeof(double) * variableLength;
		}
		if (variableType == 6) {
			size = sizeof(float) * variableLength;
		}
		memcpy(callBuffer + counter, args[argsIndex], size);
		argsIndex++;
		counter += size;
	}
	written = 0, result = 0;
	while (written < callMsgSize) {
		result = write(s, callBuffer + written, callMsgSize - written);
		if (result == -1) {
			perror("RPC CALL Write Error: ");
			return result;
		}
		written += result;
	}
	char rpcEXEC;
	result = readNBytes(s, 1, &rpcEXEC);
	char len_buffer[4];
	int length = 0;
	int param_count = 0;
	char *f_data;
	if (rpcEXEC == RPC_EXECUTE) {
		if (readNBytes(s, 4, len_buffer) == -1) {
			close(s);
		}
		arrToInt(&length, len_buffer);
		f_data = (char *)malloc((length + 1) * sizeof(char));
		f_data[length] = '\0';
		result = readNBytes(s, length, f_data);
		if (result == -1) {
			free(f_data);
		}
		free(f_data);
		if (readNBytes(s, 4, len_buffer) == -1) {
			close(s);
		}
		arrToInt(&param_count, len_buffer);
		param_t *params = (param_t *)malloc(sizeof(param_t) * param_count);
		int bad = 0;
		int param;
		int totalLength = 0;
		int paramSize[param_count];
		for (int x = 0; x < param_count; ++x) {
			if (readNBytes(s, 4, len_buffer) == -1) {
				bad = 1;
				close(s);
				printf("Connection Closed While Getting Data From Server\n");
			}

			arrToInt(&param, len_buffer);
			params[x].input = (unsigned char)((param & INPUT_BIT) > 0);
			params[x].output = (unsigned char)((param & OUTPUT_BIT) > 0);
			params[x].type = (0x00FF0000 & param) >> 16;
			params[x].length = 0x0000FFFF & param;
			int tempLength = 1;
			if (params[x].length > tempLength) {
				tempLength = params[x].length;
			}
			int paramLength = 0;
			switch (params[x].type) {
			case ARG_CHAR:
				paramLength = sizeof(char) * tempLength;
			case ARG_SHORT:
				paramLength = sizeof(short) * tempLength;
			case ARG_INT:
				paramLength = sizeof(int) * tempLength;
			case ARG_LONG:
				paramLength = sizeof(long) * tempLength;
			case ARG_DOUBLE:
				paramLength = sizeof(double) * tempLength;
			case ARG_FLOAT:
				paramLength = sizeof(float) * tempLength;
			}
			totalLength += 4;
			paramSize[x] = paramLength;
		}
		if (bad == 1) {
			free(params);
			return -1;
		}
		for (int x = 0; x < param_count; x++) {
			char temp_buffer[paramSize[x]];
			if (readNBytes(s, paramSize[x], temp_buffer) == -1) {
				close(s);
				printf("Connection closed while reading param data from server\n");
				return -1;
			}
			int test = 0;
			memcpy(&test, temp_buffer, sizeof(int));
			if (x == 3) { 
				cout << "b3 " << test << endl; 
			}
			memcpy(args[x], temp_buffer, paramSize[x]);
		}
	}
	return 0;
}

int rpcCacheCall(char *name, int *argTypes, void **args) { return 0; }

int rpcRegister(char *name, int *argTypes, skeleton f) {
	int totalMsgSize = 0;
	totalMsgSize += sizeof(char); // RPC call
	totalMsgSize += sizeof(int);  // server port;
	totalMsgSize += sizeof(int);  // Length of funtion name
	totalMsgSize += strlen(name); // Name
	totalMsgSize += sizeof(int);  // Size of argsarray
	string str = name;
	int argSize = 0;
	while (argTypes[argSize]) {
		str += to_string(argTypes[argSize]);
		argSize++;
	}
	myMap[str] = f;

	// The size of int thing is nice, but everything
	// will break if it's ever not 4. This is because
	// of both client and server side issues. Wishlist

	totalMsgSize += argSize * sizeof(int); // Args
	char buffer[totalMsgSize];

	char call_type = RPC_REGISTER;
	int counter = 0;
	memcpy(buffer + counter, &call_type, sizeof(char));

	counter += sizeof(char);
	int nameLength = strlen(name);

	// copying port
	char int_arr[4];
	intToArr(serverPort, int_arr);
	memcpy(buffer + counter, int_arr, sizeof(int));
	counter += sizeof(int);

	// Copy in name length
	intToArr(nameLength, int_arr);
	memcpy(buffer + counter, int_arr, 4);
	counter += sizeof(int);

	// Copy in name
	memcpy(buffer + counter, name, nameLength);
	counter += nameLength;

	// Copy in arg size
	intToArr(argSize, int_arr);
	memcpy(buffer + counter, int_arr, 4);
	counter += sizeof(int);

	// Copy in args
	for (int i = 0; i < argSize; i++) {
		intToArr(argTypes[i], int_arr);
		memcpy(buffer + counter, int_arr, 4);
		counter += sizeof(int);
	}

	// Write loop
	int written = 0, result = 0;
	while (written < totalMsgSize) {
		result = write(bindDescriptor, buffer + written, totalMsgSize - written);
		if (result == -1) {
			return result;
		}
		written += result;
	}
	// Return number of bytes written
	return written;
}

int rpcExecute() {
	//	return 0;
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
	for (;;) {
		read_fds = master;
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
			exit(4);
		}
		for (int i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) {
				if (i == serverDescriptor) {
					int newfd = accept(serverDescriptor, (struct sockaddr *)&csock_s, (socklen_t *)&len);
					if (VERBOSE_OUTPUT == 1) {
						cout << "new connection s is " << serverDescriptor << endl;
					}
					if (newfd == -1) {
					}
					FD_SET(newfd, &master);
					if (newfd > fdmax) {
						fdmax = newfd;
					}
				} else {
					char call;
					int result = readNBytes(i, 1, &call);
					if (result == -1) {
						close(i);
						FD_CLR(i, &master);
						break;
					}
					char *f_data;
					string name;
					string mapKey;
					char len_buffer[4];
					int length = 0;
					int param_count = 0;
					if (call == RPC_CALL) {
						if (readNBytes(i, 4, len_buffer) == -1) {
							close(i);
							FD_CLR(i, &master);
							break;
						}
						arrToInt(&length, len_buffer);
						f_data = (char *)malloc((length + 1) * sizeof(char));
						f_data[length] = '\0';
						result = readNBytes(i, length, f_data);
						if (result == -1) {
							free(f_data);
							break;
						}
						name = string(f_data, length);
						mapKey = name;
						free(f_data);

						if (readNBytes(i, 4, len_buffer) == -1) {
							cout << "error " << endl;
							close(i);
							FD_CLR(i, &master);
							break;
						}
						arrToInt(&param_count, len_buffer);
						param_t *params = (param_t *)malloc(sizeof(param_t) * param_count);
						if(VERBOSE_OUTPUT == 1){
							printf("We have %d params for this function.\n", param_count);
						}
						int bad = 0;
						int param = 0;
						int paramSize[param_count];
						int tempArgsArray[param_count];
						for (int x = 0; x < param_count; ++x) {
							if (readNBytes(i, 4, len_buffer) == -1) {
								printf("Failed reading from server\n");
								bad = 1;
								close(i);
								FD_CLR(i, &master);
							}
							arrToInt(&param, len_buffer);
							mapKey += to_string(param);
							tempArgsArray[x] = param;
							params[x].input = (unsigned char)((param & INPUT_BIT) > 0);
							params[x].output = (unsigned char)((param & OUTPUT_BIT) > 0);
							params[x].type = (0x00FF0000 & param) >> 16;
							params[x].length = 0x0000FFFF & param;
							int tempLength = 1;
							if (params[x].length > tempLength) {
								tempLength = params[x].length;
							}
							int paramLength = 0;
							switch (params[x].type) {
							case ARG_CHAR:
								paramLength = sizeof(char) 		* tempLength;
								break;
							case ARG_SHORT:
								paramLength = sizeof(short) 	* tempLength;
								break;
							case ARG_INT:
								paramLength = sizeof(int) 		* tempLength;
								break;
							case ARG_LONG:
								paramLength = sizeof(long) 		* tempLength;
								break;
							case ARG_DOUBLE:
								paramLength = sizeof(double) 	* tempLength;
								break;
							case ARG_FLOAT:
								paramLength = sizeof(float) 	* tempLength;
								break;
							}
							paramSize[x] = paramLength;
						}
						for(int i=0; i< param_count; ++i){
							printf("Param Size of %d is %d\n", i, paramSize[i]);
						}
						if (bad == 1) {
							printf("Cleaning up after failed read\n");
							free(params);
							break;
						}
						if(VERBOSE_OUTPUT == 1){
							printf("The Function Being Accessed in Map is: %s\n", mapKey.c_str());
						}

						void *tempArgs[param_count];
						for (int x = 0; x < param_count; x++) {
							char *temp_buffer = (char *)malloc(paramSize[x]);
							tempArgs[x] = (void *)temp_buffer;
							if (readNBytes(i, paramSize[x], temp_buffer) == -1) {
								close(i);
								FD_CLR(i, &master);
							}
							//if(x == 3){
							//	int testInt = 0;
							//	memcpy(&testInt, temp_buffer, sizeof(int));
							//	printf("Our int value is: %d\n", testInt);
							//}
						}
						skeleton newf = myMap.find(mapKey)->second;
						int skeletonResult = newf(tempArgsArray, tempArgs);
						if(VERBOSE_OUTPUT == 1){
							printf("The Skeleton Function returned: ");
							switch(skeletonResult){
								case 0:
									printf("Successful\n");
									break;
								default:
									printf("Failure\n");
							}
						}
						int responseSize = 0;
						int responseCounter = 0;
						responseSize += sizeof(char);  // rpc call
						responseSize += sizeof(int);   // length of function name
						responseSize += name.length(); // name
						responseSize += sizeof(int);   // argsarray size
						responseSize += param_count * sizeof(int);
						for (int index = 0; index < param_count; ++index) {
							int variableType = (tempArgsArray[index] >> 16) & 255;
							int variableLength = tempArgsArray[index] & 65535;
							if (variableLength == 0) {
								variableLength = 1;
							}
							if (variableType == 1) {
								responseSize += sizeof(char) * variableLength;
							}
							if (variableType == 2) {
								responseSize += sizeof(short) * variableLength;
							}
							if (variableType == 3) {
								responseSize += sizeof(int) * variableLength;
							}
							if (variableType == 4) {
								responseSize += sizeof(long) * variableLength;
							}
							if (variableType == 5) {
								responseSize += sizeof(double) * variableLength;
							}
							if (variableType == 6) {
								responseSize += sizeof(float) * variableLength;
							}
						}
						char responseBuffer[responseSize];
						if(VERBOSE_OUTPUT == 1){
							printf("Our Response To The Client is %d bytes long\n", responseSize);
						}

						// writing rpc call
						char call_type = RPC_EXECUTE;
						memcpy(responseBuffer + responseCounter, &call_type, sizeof(char));
						responseCounter += sizeof(char);

						// writting name length
						int nameLength = name.length();
						char int_arr[4];
						intToArr(nameLength, int_arr);
						memcpy(responseBuffer + responseCounter, int_arr, 4);
						responseCounter += sizeof(4);

						// writing name
						memcpy(responseBuffer + responseCounter, name.c_str(), name.length());
						responseCounter += nameLength;

						// writing arg size
						intToArr(param_count, int_arr);
						memcpy(responseBuffer + responseCounter, int_arr, 4);
						responseCounter += sizeof(4);

						// writing args
						for (int x = 0; x < param_count; x++) {
							intToArr(tempArgsArray[x], int_arr);
							memcpy(responseBuffer + responseCounter, int_arr, 4);
							responseCounter += sizeof(4);
						}

						int size = 0;
						for (int argsIndex = 0; argsIndex < param_count; ++argsIndex) {
							if(VERBOSE_OUTPUT == 1){
								printf("Our Buffer Pointer is At: %d\n", responseCounter);
							}
							int variableType = (tempArgsArray[argsIndex] >> 16) & 255;
							int variableLength = tempArgsArray[argsIndex] & 65535;
							if (variableLength == 0) {
								variableLength = 1;
							}
							if (variableType == 1) {
								size = sizeof(char) * variableLength;
							}
							if (variableType == 2) {
								size = sizeof(short) * variableLength;
							}
							if (variableType == 3) {
								size = sizeof(int) * variableLength;
							}
							if (variableType == 4) {
								size = sizeof(long) * variableLength;
							}
							if (variableType == 5) {
								size = sizeof(double) * variableLength;
							}
							if (variableType == 6) {
								size = sizeof(float) * variableLength;
							}
							int testValue = 0;
							arrToInt(&testValue, (char *)(tempArgs[argsIndex]));
							memcpy(responseBuffer + responseCounter, tempArgs[argsIndex], size);
							responseCounter += size;
						}
						if(VERBOSE_OUTPUT == 1){
							printf("Our Buffer Pointer Ended At: %d\n", responseCounter);
						}
						int written = 0, result = 0;
						while (written < responseSize) {
							result = write(i, responseBuffer + written, responseSize - written);
							if (result == -1) {
								return result;
							}
							written += result;
						}
					}
				}
			}
		}
	}
	return 0;
}

int rpcTerminate() { return 0; }
