#include "binder.h"
#include "rpc_consts.h"
#include "rpc.h"

int readNBytes(int des, int amount, char* buffer){
	int so_far = 0, result = 0;
	while(so_far != amount){
		result = read(des, buffer+so_far, 1);
		if(result <= 0){
			return -1;
		}else{
			so_far += result;
		}
	}
	return so_far;
}

void handleHostnameAndError(char* hostname){
	int error = gethostname(hostname, 255);
	if(error != 0){
		printf("Could not get hostname. Error: %d\n", error);
		exit(error);
	}	
}

int bindAndListen(struct sockaddr_in ssock_s){
	int ssock = socket(AF_INET, SOCK_STREAM, 0);
	int error = bind(ssock, (struct sockaddr *)&ssock_s, sizeof(struct sockaddr));
	if(-1 == error){
		printf("bind Error: %d\n", error);
		exit(error);
	}
	error = listen(ssock, 1);
	if(-1 == error){
		printf("listen Error: %d\n", error);
		exit(error);
	}
	return ssock;
}

int main(int argc, char ** argv){
	// Variables
	int port = 0, error = 0, result = 0, connection = 0, first_decriptor = 0, num_connections = 10, length = 0, read_so_far=0;
	char* hostname = (char*)malloc(255), *len_buffer = (char*)malloc(4);
	char portStr[5];

	struct sockaddr_in ssock_s;
	struct sockaddr_in csock_s;
	socklen_t len = sizeof(ssock_s);
	fd_set fdactive, fdread;

	// House keeping
	socklen_t socksize = sizeof(struct sockaddr_in);
	memset(&ssock_s, socksize, 0);

	// Configuration
	handleHostnameAndError(hostname);

	ssock_s.sin_family = AF_INET;
	ssock_s.sin_addr.s_addr = INADDR_ANY;
	ssock_s.sin_port = htons(port);
	FD_ZERO (&fdactive);

	// Initialization
	int ssock = bindAndListen(ssock_s);

	error = getsockname(ssock, (struct sockaddr *)&ssock_s, &len);
	if(-1 == error){
		printf("getsockname Error: %d\n", error);
		return error;
	}
	port = ntohs(ssock_s.sin_port);

	// Main code
	FD_SET (ssock, &fdactive);
	// Info printout
	printf("BINDER_ADDRESS %s\n", hostname);
	printf("BINDER_PORT %d\n", port);
	if(VERBOSE_OUTPUT == 1){
		printf("export BINDER_ADDRESS=\"%s\"; export BINDER_PORT=\"%d\"\n", hostname, port);
	}
	sprintf(portStr,"%d",port);

	while (1){
		fdread = fdactive;
		if (select (FD_SETSIZE, &fdread, 0x0, 0x0, 0x0) < 0){
			printf("FD Error. Bummer.\n");
			exit (1);
		}
		for (int des = 0; des < FD_SETSIZE; ++des){
			if (FD_ISSET (des, &fdread)){
				if(des == ssock){
					if(first_decriptor == 0){
						first_decriptor = des;
					}
					int new_des = accept(ssock, (struct sockaddr *)&csock_s, &len);
					FD_SET(new_des, &fdactive);
					printf("New connection on descriptor %d\n", new_des);
				}else{
					//Structure of calls
					//1 byte for call type
					//Rest of bytes dependent on call type
					char call_type;
					result = readNBytes(des, 1, &call_type);

					if(call_type == RPC_REGISTER){
						std::string name, function_data;
						char* f_data;

						// Read Length of Name
						if(readNBytes(des, 4, len_buffer) == -1){break;}
						length = fourBytesToInt(len_buffer);
						f_data = (char*)malloc(length*sizeof(char));

						// Read name
						result = readNBytes(des, length, f_data);
						if(result == -1){free(f_data);break;}

						// Put into better data type
						name = std::string(f_data, length);
						free(f_data);

						// Read length of function data
						if(readNBytes(des, 4, len_buffer) == -1){break;}
						length = fourBytesToInt(len_buffer);
						f_data = (char*)malloc(length*sizeof(char));

						// Read function data
						result = readNBytes(des, length, f_data);
						if(result == -1){free(f_data);break;}

						// Put into better data type
						function_data = std::string(f_data, length);
						free(f_data);

						int error = getpeername(des, (struct sockaddr *)&csock_s, &len);
						if(-1 == error){
							perror("getpeername: ");
							return error;
						}

						struct sockaddr_in *s = (struct sockaddr_in *)&csock_s;
						std::string ip = inet_ntoa(csock_s.sin_addr);

						func_def new_function;
						new_function.name = name;
						new_function.function_data = function_data;
						new_function.server_ip = ip;

						if(VERBOSE_OUTPUT == 1){
							printf("Added new function called: %s, from server at: %s\n", name.c_str(), ip.c_str());
						}
						function_database.push_back(new_function);

					}else if(call_type == RPC_CALL){
					}
				}
			}
		}
	}
	// Closing housekeeping
	close(ssock);
	free(hostname);
	free(len_buffer);
	return 0;
}