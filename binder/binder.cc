// Public Headers
#include "rpc_consts.h"
#include "rpc.h"
#include "util.h"

// Private Headers
#include "binder.h"

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

int findFunction(func_def_t new_function){
	for (int i = 0; i < function_database.size(); ++i){
		if(new_function.param_count != function_database[i].param_count){
			continue;
		}
		if(new_function.name != function_database[i].name){
			continue;
		}
		param_t * new_params = new_function.params;
		param_t * check_params = function_database[i].params;

		// Already checked for the same length
		for(int j=0; j < new_function.param_count; ++j){
			if(new_params[j].input != check_params[j].input){
				break;
			}
			if(new_params[j].output != check_params[j].output){
				break;
			}
			if(new_params[j].type != check_params[j].type){
				break;
			}
			if(new_params[j].length != check_params[j].length){
				break;
			}
			return i;
		}
	}
	return NO_FUNCTION;
}

int main(int argc, char ** argv){
	// Variables
	int port = 0, server_port = 0, error = 0, result = 0, first_decriptor = 0, length = 0, param_count = 0;
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
					if(VERBOSE_OUTPUT == 1){
						printf("New connection\n");
					}
				}else{
					char call_type;
					result = readNBytes(des, 1, &call_type);
					if(result == -1){
						FD_CLR (des, &fdactive);
						close(des);
						break;
					}
					char* f_data;
					std::string name;

					if(call_type == RPC_REGISTER){
						// Read server port
						if(readNBytes(des, 4, len_buffer) == -1){FD_CLR (des, &fdactive);close(des);break;}
						server_port = fourBytesToInt(len_buffer);

						// Read Length of Name
						if(readNBytes(des, 4, len_buffer) == -1){FD_CLR (des, &fdactive);close(des);break;}
						length = fourBytesToInt(len_buffer);
						f_data = (char*)malloc(length*sizeof(char));

						// Read name
						result = readNBytes(des, length, f_data);
						if(result == -1){free(f_data);break;}

						// Put into better data type
						name = std::string(f_data, length);
						free(f_data);

						// Read length of function data
						if(readNBytes(des, 4, len_buffer) == -1){FD_CLR (des, &fdactive);close(des);break;}
						param_count = fourBytesToInt(len_buffer);
						param_t * params = (param_t *)malloc(sizeof(param_t) * param_count);
						int bad = 0;
						int param;
						for(int i=0; i < param_count; ++i){
							if(readNBytes(des, 4, len_buffer) == -1){bad = 1; FD_CLR(des, &fdactive); close(des); break;}
							arrToInt(&param, len_buffer);
							params[i].input  = (unsigned char)((param & INPUT_BIT) > 0);
							params[i].output = (unsigned char)((param & OUTPUT_BIT) > 0);
							params[i].type = (0x00FF0000 & param) >> 16;
							params[i].length = 0x0000FFFF & param;
						}
						if(bad == 1){
							free(params);
							break;
						}

						int error = getpeername(des, (struct sockaddr *)&csock_s, &len);
						if(-1 == error){
							perror("getpeername: ");
							return error;
						}

						std::string ip = inet_ntoa(csock_s.sin_addr);

						func_def_t new_function;
						new_function.name = name;
						new_function.param_count = param_count;
						new_function.params = params;

						if(VERBOSE_OUTPUT == 1){
							printf("Added new function:\n");
							printf("\tName:\t%s\n", new_function.name.c_str());
							printf("\tParams:\t%d\n", new_function.param_count);
							for(int i = 0; i < new_function.param_count; ++i){
								printf("\t\tType:\t");
								englishType(new_function.params[i].type);
								printf("\n");
								printf("\t\tInput:\t%d\n", new_function.params[i].input);
								printf("\t\tOutput:\t%d\n", new_function.params[i].output);
								printf("\t\tLength:\t%u\n\n", new_function.params[i].length);
							}
							printf("\tServer:\t%s\n", ip.c_str());
							printf("\tPort:\t%d\n", server_port);
						}
						int have_function = findFunction(new_function);

						server_t new_location;
						new_location.server = ip;
						new_location.port = server_port;

						if(have_function == NO_FUNCTION){
							if(VERBOSE_OUTPUT == 1){
								printf("\tThis is a new function, adding to database\n");
							}
							new_function.servers.push_back(new_location);
							function_database.push_back(new_function);
						}else{
							if(VERBOSE_OUTPUT == 1){
								printf("\tThis is an already existing function, adding to server array\n");
							}
							function_database[have_function].servers.push_back(new_location);
						}

					}else if(call_type == RPC_CALL){
						// Read Length of Name
						if(readNBytes(des, 4, len_buffer) == -1){FD_CLR (des, &fdactive);close(des);break;}
						length = fourBytesToInt(len_buffer);
						f_data = (char*)malloc(length*sizeof(char));

						// Read name
						result = readNBytes(des, length, f_data);
						if(result == -1){free(f_data);break;}

						// Put into better data type
						name = std::string(f_data, length);
						free(f_data);

						// Read length of function data
						if(readNBytes(des, 4, len_buffer) == -1){FD_CLR (des, &fdactive);close(des);break;}
						param_count = fourBytesToInt(len_buffer);
						param_t * params = (param_t *)malloc(sizeof(param_t) * param_count);
						int bad = 0;
						int param;
						for(int i=0; i < param_count; ++i){
							if(readNBytes(des, 4, len_buffer) == -1){bad = 1; FD_CLR(des, &fdactive); close(des); break;}
							arrToInt(&param, len_buffer);
							params[i].input  = (unsigned char)((param & INPUT_BIT) > 0);
							params[i].output = (unsigned char)((param & OUTPUT_BIT) > 0);
							params[i].type = (0x00FF0000 & param) >> 16;
							params[i].length = 0x0000FFFF & param;
						}

						func_def_t new_function;
						new_function.name = name;
						new_function.param_count = param_count;
						new_function.params = params;

						int have_function = findFunction(new_function);

						if(have_function == NO_FUNCTION){
							//TBD
						}else{
							server_t server = function_database[have_function].servers.front();
							function_database[have_function].servers.pop_front();
							function_database[have_function].servers.push_back(server);
							unsigned char message_type = RPC_CALL;
							write(des, &message_type, 1);
							char int_arr[4];
							intToArr(server.server.length(), int_arr);
							write(des, &int_arr, 4);
							write(des, server.server.c_str(), server.server.length());

							intToArr(server.port, int_arr);
							write(des, &int_arr, 4);
						}
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