#include "binder.h"
#include "../include/rpc_consts.h"

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

int main(int argc, char ** argv){
	// Variables
	int port = 0, error = 0, result = 0, connection = 0, first_decriptor = 0, num_connections = 10, length = 0, read_so_far=0;
	char* hostname = (char*)malloc(255), *len_buffer = (char*)malloc(4);
	char portStr[5];
	cbuf_t buffers[num_connections];

	struct sockaddr_in ssock_s;
	struct sockaddr_in csock_s;
	socklen_t len = sizeof(ssock_s);
	fd_set fdactive, fdread;

	// House keeping
	socklen_t socksize = sizeof(struct sockaddr_in);
	memset(&ssock_s, socksize, 0);

	// Configuration
	error = gethostname(hostname, 255);
	if(error != 0){
		printf("Could not get hostname. Error: %d\n", error);
		return error;
	}
	ssock_s.sin_family = AF_INET;
	ssock_s.sin_addr.s_addr = INADDR_ANY;
	ssock_s.sin_port = htons(port);
	FD_ZERO (&fdactive);

	// Initialization
	for(int i=0; i < num_connections; ++i){
		buffers[i].length = 0;
		buffers[i].location = 0;
		buffers[i].buffer = 0x0;
	}
	int ssock = socket(AF_INET, SOCK_STREAM, 0);
	error = bind(ssock, (struct sockaddr *)&ssock_s, sizeof(struct sockaddr));
	if(-1 == error){
		printf("bind Error: %d\n", error);
		return error;
	}
	error = listen(ssock, 1);
	if(-1 == error){
		printf("listen Error: %d\n", error);
		return error;
	}

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

					switch(call_type){
						case RPC_REGISTER:
							result = readNBytes(des, 4, len_buffer);
							length = fourBytesToInt(len_buffer);
							break;
						case RPC_CALL:
							break;
					}
				}
			}
		}
	}
	// Closing housekeeping
	close(ssock);
	free(hostname);
	for(int i = 0; i < num_connections; ++i){
		if(buffers[i].length > 0){
			free(buffers[i].buffer);
		}
	}
	free(buffers);
	free(len_buffer);
	return 0;
}