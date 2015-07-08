// C Headers
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <pthread.h>

// C++ Headers
#include <string>
#include <map>

// Local headers
#include "rpc.h"
#include "rpc_consts.h"
#include "util.h"

char *binderAddr;
char *binderPort;
int bindDescriptor;
int serverDescriptor;
int serverPort;
std::map<std::string, skeleton> myMap;
int switchHelper(int variableType, int variableLength) {
	int size = 0;
	switch (variableType) {
	case ARG_CHAR:
		size = sizeof(char) * variableLength;
		break;
	case ARG_SHORT:
		size = sizeof(short) * variableLength;
		break;
	case ARG_INT:
		size = sizeof(int) * variableLength;
		break;
	case ARG_LONG:
		size = sizeof(long) * variableLength;
		break;
	case ARG_DOUBLE:
		size = sizeof(double) * variableLength;
		break;
	case ARG_FLOAT:
		size = sizeof(float) * variableLength;
		break;
	}
	return size;
}
struct thread_args{ 
	std::string mapKey;
	int * tempArgsArray;
	int i;
	std::string name;
	int param_count;
	void ** tempArgs;	
};

int skeletonHelper(std::string mapKey, int * tempArgsArray, int i, std::string name, int param_count, void ** tempArgs) { 
	skeleton newf = myMap.find(mapKey)->second;
	int skeletonResult = newf(tempArgsArray, tempArgs);
	if (VERBOSE_OUTPUT == 1) {
		printf("The Skeleton Function returned: ");
		switch (skeletonResult) {
		case 0:
			printf("Successful\n");
			break;
		default:
			printf("Failure\n");
		}
	}
	int responseSize = 0;
	int responseCounter = 0;
	responseSize += sizeof(char); // rpc call

	if (skeletonResult < 0) {
		char responseBuffer[responseSize];
		char call_type = RPC_FAILURE;
		memcpy(responseBuffer + responseCounter, &call_type, sizeof(char));
		int written = 0, result = 0;
		while (written < responseSize) {
			result = write(i, responseBuffer + written, responseSize - written);
			if (result == -1) {
				return result;
			}
			written += result;
		}

	}

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
		responseSize += switchHelper(variableType, variableLength);
	}
	char responseBuffer[responseSize];
	if (VERBOSE_OUTPUT == 1) {
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
		if (VERBOSE_OUTPUT == 1) {
			printf("Our Buffer Pointer is At: %d\n", responseCounter);
		}
		int variableType = (tempArgsArray[argsIndex] >> 16) & 255;
		int variableLength = tempArgsArray[argsIndex] & 65535;
		if (variableLength == 0) {
			variableLength = 1;
		}
		size = switchHelper(variableType, variableLength);
		int testValue = 0;
		arrToInt(&testValue, (char *)(tempArgs[argsIndex]));
		memcpy(responseBuffer + responseCounter, tempArgs[argsIndex], size);
		responseCounter += size;
	}
	if (VERBOSE_OUTPUT == 1) {
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
	return 0;
}
void * skeletonThread(void * args) {
        struct thread_args * temp = (struct thread_args *) args;
    	if(VERBOSE_OUTPUT == 1){
    		printf("Starting a Thread to Handle Function Call\n");
    		printf("Args Struct At: %p\n", args);
    		printf("Descriptor(?): %d\n", temp->i);
    		printf("Function Map Key: %s\n", temp->mapKey.c_str());
    		printf("Param Count: %d\n", temp->param_count);
    	}
        skeletonHelper (temp->mapKey, temp->tempArgsArray, temp->i, temp->name, temp->param_count, temp->tempArgs);
        free(temp->tempArgs);
        free(temp->tempArgsArray);
        free(temp);
        pthread_exit(0);
}

int setupListener() {
	int s = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in sAddr;
	sAddr.sin_family = AF_INET;
	sAddr.sin_addr.s_addr = INADDR_ANY;
	sAddr.sin_port = 0;
	int success;
	success = bind(s, (struct sockaddr *)&sAddr, sizeof(sAddr));
	if (success == -1) {
		perror("Bind: ");
		return -1;
	}

	socklen_t addrlen = sizeof(sAddr);
	success = getsockname(s, (struct sockaddr *)&sAddr, &addrlen);
	if (success == -1) {
		perror("getsockname: ");
		return -1;
	}
	if (VERBOSE_OUTPUT == 1) {
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
	return 0;
}
int clientInit() {
	binderAddr = getenv("BINDER_ADDRESS");
	binderPort = getenv("BINDER_PORT");
	if (binderAddr == NULL) {
		printf("Env variable BINDER_ADDRESS not found\n");
		return 1;
	}
	if (binderPort == NULL) {
		printf("Env variable BINDER_PORT not found\n");
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
		printf("Env variable BINDER_ADDRESS not found\n");
		return 1;
	}
	if (binderPort == NULL) {
		printf("Env variable BINDER_PORT not found\n");
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

	char call = '\0';
	int result2 = 0;
	result2 = readNBytes(bindDescriptor, 1, &call);
	if (call == RPC_FAILURE) { 
		return -1;
	}
	char hostnameLength[4];
	char *hostname;
	char port[4];
	int portNumber = 0;
	result2 = readNBytes(bindDescriptor, 4, hostnameLength);
	int hlength;
	arrToInt(&hlength, hostnameLength);
	// This needs to be freed
	hostname = (char *)malloc((hlength + 1) * sizeof(char));
	hostname[hlength] = '\0';
	result2 = readNBytes(bindDescriptor, hlength, hostname);
	result2 = readNBytes(bindDescriptor, 4, port);
	arrToInt(&portNumber, port);
	if (VERBOSE_OUTPUT == 1) {
		printf("Binder Responsed with Host: %s, Port: %d\n", hostname, portNumber);
		printf("Closing Connection To Binder.\n");
	}
	close(bindDescriptor);

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

		callMsgSize += switchHelper(variableType, variableLength);
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

	while (argTypes[argsIndex]) {
		int variableType = (argTypes[argsIndex] >> 16) & 255;
		int variableLength = argTypes[argsIndex] & 65535;
		int size = 0;
		if (variableLength == 0) {
			variableLength = 1;
		}
		size = switchHelper(variableType, variableLength);
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
	if (rpcEXEC == RPC_FAILURE) {
		return -1;
	}
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
			paramLength = switchHelper(params[x].type, tempLength);
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
	std::string str = name;
	int argSize = 0;
	while (argTypes[argSize]) {
		str += std::to_string(argTypes[argSize]);
		argSize++;
	}
	myMap[str] = f;

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
						printf("New Connection\n");
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
					std::string name;
					std::string mapKey;
					char len_buffer[4];
					int length = 0;
					int param_count = 0;
					if (call == RPC_TERMINATE) {
						int error = getpeername(i, (struct sockaddr *)&csock_s, &len);
						if (error == -1) {
							perror("getpeername: ");
							return error;
						}
						std::string ip = inet_ntoa(csock_s.sin_addr);
						struct hostent *he = gethostbyname(binderAddr);
						if (he->h_addr_list[0] == ip) {
							FD_ZERO(&master);
							close(serverDescriptor);
						}
						return 0;
					}
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
						name = std::string(f_data, length);
						mapKey = name;
						free(f_data);

						if (readNBytes(i, 4, len_buffer) == -1) {
							printf("Error in connection.\n");
							close(i);
							FD_CLR(i, &master);
							break;
						}
						arrToInt(&param_count, len_buffer);
						param_t *params = (param_t *)malloc(sizeof(param_t) * param_count);
						if (VERBOSE_OUTPUT == 1) {
							printf("We have %d params for this function.\n", param_count);
						}
						int bad = 0;
						int param = 0;
						int paramSize[param_count];
						int *tempArgsArray = (int*)malloc(sizeof(int) * param_count);
						for (int x = 0; x < param_count; ++x) {
							if (readNBytes(i, 4, len_buffer) == -1) {
								printf("Failed reading from server\n");
								bad = 1;
								close(i);
								FD_CLR(i, &master);
							}
							arrToInt(&param, len_buffer);
							mapKey += std::to_string(param);
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
							paramLength = switchHelper(params[x].type, tempLength);
							paramSize[x] = paramLength;
						}
						if (bad == 1) {
							printf("Cleaning up after failed read\n");
							free(params);
							break;
						}
						if (VERBOSE_OUTPUT == 1) {
							printf("The Function Being Accessed in Map is: %s\n", mapKey.c_str());
						}
						void ** tempArgs = (void**)malloc(sizeof(void*)*param_count);
						for (int x = 0; x < param_count; x++) {
							char *temp_buffer = (char *)malloc(paramSize[x]);
							tempArgs[x] = (void *)temp_buffer;
							if (readNBytes(i, paramSize[x], temp_buffer) == -1) {
								close(i);
								FD_CLR(i, &master);
							}
						}
						pthread_t pth;
						struct thread_args * skel_args = (struct thread_args*)malloc(sizeof(struct thread_args));
						skel_args->mapKey = mapKey;
						skel_args->tempArgsArray = tempArgsArray;
						skel_args->i = i;
						skel_args->name = name;
						skel_args->param_count = param_count;
						skel_args->tempArgs = tempArgs;
						if(VERBOSE_OUTPUT == 1){
							printf("Preparing for thread start...\n");
							printf("Structure At  : %p\n", skel_args);
							printf("Descriptor (?): %d\n", skel_args->i);
						}
						pthread_create(&pth, NULL, skeletonThread, (void *)skel_args);
					}
				}
			}
		}
	}
	return 0;
}

int rpcTerminate() {
	clientInit();
	char terminate = RPC_TERMINATE;
	int written = 0;
	while (written < 1) {
		int result = write(bindDescriptor, &terminate, 1);
		if (result == -1) {
			printf("Failed writing to binder\n");
			return result;
		}
		written += result;
	}

	return 0;
}
