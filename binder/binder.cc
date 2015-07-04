// Public Headers
#include "rpc_consts.h"
#include "rpc.h"

// Private Headers
#include "binder.h"

void handleHostnameAndError(char *hostname) {
	int error = gethostname(hostname, 255);
	if (error != 0) {
		printf("Could not get hostname. Error: %d\n", error);
		exit(error);
	}
}

int bindAndListen(struct sockaddr_in ssock_s) {
	int ssock = socket(AF_INET, SOCK_STREAM, 0);
	int error = bind(ssock, (struct sockaddr *)&ssock_s, sizeof(struct sockaddr));
	if (-1 == error) {
		printf("bind Error: %d\n", error);
		exit(error);
	}
	error = listen(ssock, 1);
	if (-1 == error) {
		printf("listen Error: %d\n", error);
		exit(error);
	}
	return ssock;
}

int functionsEqual(func_def_t f1, func_def_t f2) {
	if (f1.param_count != f2.param_count) {
		return 0;
	}
	if (f1.name != f2.name) {
		return 0;
	}
	param_t *new_params = f1.params;
	param_t *check_params = f2.params;

	// Already checked for the same length
	for (int j = 0; j < f1.param_count; ++j) {
		if (new_params[j].input != check_params[j].input) {
			return 0;
		}
		if (new_params[j].output != check_params[j].output) {
			return 0;
		}
		if (new_params[j].type != check_params[j].type) {
			return 0;
		}
		if (new_params[j].length != check_params[j].length) {
			return 0;
		}
	}
	return 1;
}

int doesServerHave(std::vector<func_def_t> supported_functions, func_def_t new_function) {
	for (std::vector<func_def_t>::iterator i = supported_functions.begin(); i != supported_functions.end(); ++i) {
		if (functionsEqual(*i, new_function) == 1) {
			return 1;
		}
	}
	return 0;
}

int addFunction(std::string server, int port, func_def_t new_function) {
	for (std::deque<server_data_t>::iterator i = server_list.begin(); i != server_list.end(); ++i) {
		// This is the same server
		if (i->server == server && i->port == port) {
			if (doesServerHave(i->supported_functions, new_function) == 1) {
				return RE_REGISTER;
			}
			i->supported_functions.push_back(new_function);
			return NEW_FUNCTION;
		}
	}
	//	There is no server
	server_data_t new_server;
	new_server.server = server;
	new_server.port = port;
	new_server.supported_functions.push_back(new_function);
	server_list.push_back(new_server);
	return NEW_SERVER;
}

server_t findFunction(func_def_t new_function) {
	for (std::deque<server_data_t>::iterator i = server_list.begin(); i != server_list.end(); ++i) {
		if (doesServerHave(i->supported_functions, new_function) == 1) {
			server_data_t move = *i;
			server_list.erase(i);
			server_list.push_back(move);

			server_t server;
			server.server = move.server;
			server.port = move.port;
			return server;
		}
	}
	server_t server;
	server.server = std::string("");
	server.port = 0;
	return server;
}

int main(int argc, char **argv) {
	// Variables
	int port = 0, server_port = 0, error = 0, result = 0, first_decriptor = 0, length = 0, param_count = 0;
	char *hostname = (char *)malloc(255), *len_buffer = (char *)malloc(4);
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
	FD_ZERO(&fdactive);

	// Initialization
	int ssock = bindAndListen(ssock_s);

	error = getsockname(ssock, (struct sockaddr *)&ssock_s, &len);
	if (-1 == error) {
		printf("getsockname Error: %d\n", error);
		return error;
	}
	port = ntohs(ssock_s.sin_port);

	// Main code
	FD_SET(ssock, &fdactive);
	// Info printout
	printf("BINDER_ADDRESS %s\n", hostname);
	printf("BINDER_PORT %d\n", port);
	if (VERBOSE_OUTPUT == 1) {
		printf("export BINDER_ADDRESS=\"%s\"; export BINDER_PORT=\"%d\"\n", hostname, port);
	}
	sprintf(portStr, "%d", port);

	while (1) {
		fdread = fdactive;
		if (select(FD_SETSIZE, &fdread, 0x0, 0x0, 0x0) < 0) {
			printf("FD Error. Bummer.\n");
			exit(1);
		}
		for (int des = 0; des < FD_SETSIZE; ++des) {
			if (FD_ISSET(des, &fdread)) {
				if (des == ssock) {
					if (first_decriptor == 0) {
						first_decriptor = des;
					}
					int new_des = accept(ssock, (struct sockaddr *)&csock_s, &len);
					FD_SET(new_des, &fdactive);
					if (VERBOSE_OUTPUT == 1) {
						printf("New connection\n");
					}
				} else {
					char call_type;
					result = readNBytes(des, 1, &call_type);
					if (result == -1) {
						FD_CLR(des, &fdactive);
						close(des);
						break;
					}
					char *f_data;
					std::string name;

					if (call_type == RPC_REGISTER) {
						// Read server port
						if (readNBytes(des, 4, len_buffer) == -1) {
							FD_CLR(des, &fdactive);
							close(des);
							break;
						}
						arrToInt(&server_port, len_buffer);

						// Read Length of Name
						if (readNBytes(des, 4, len_buffer) == -1) {
							FD_CLR(des, &fdactive);
							close(des);
							break;
						}
						arrToInt(&length, len_buffer);
						f_data = (char *)malloc(length * sizeof(char));

						// Read name
						result = readNBytes(des, length, f_data);
						if (result == -1) {
							free(f_data);
							break;
						}

						// Put into better data type
						name = std::string(f_data, length);
						free(f_data);

						// Read length of function data
						if (readNBytes(des, 4, len_buffer) == -1) {
							FD_CLR(des, &fdactive);
							close(des);
							break;
						}
						arrToInt(&param_count, len_buffer);
						param_t *params = (param_t *)malloc(sizeof(param_t) * param_count);
						int bad = 0;
						int param;
						for (int i = 0; i < param_count; ++i) {
							if (readNBytes(des, 4, len_buffer) == -1) {
								bad = 1;
								FD_CLR(des, &fdactive);
								close(des);
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

						int error = getpeername(des, (struct sockaddr *)&csock_s, &len);
						if (-1 == error) {
							perror("getpeername: ");
							return error;
						}

						std::string ip = inet_ntoa(csock_s.sin_addr);

						func_def_t new_function;
						new_function.name = name;
						new_function.param_count = param_count;
						new_function.params = params;

						if (VERBOSE_OUTPUT == 1) {
							printf("Added new function:\n");
							printf("\tName:\t%s\n", new_function.name.c_str());
							printf("\tParams:\t%d\n", new_function.param_count);
							for (int i = 0; i < new_function.param_count; ++i) {
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
						int have_function = addFunction(ip, server_port, new_function);

						if (VERBOSE_OUTPUT == 1) {
							if (have_function == NEW_SERVER) {
								printf("\tStatus: Added a new function to a new server.\n\n");
							} else if (have_function == NEW_FUNCTION) {
								printf("\tStatus: Added a new function to an old server.\n\n");
							} else if (have_function == RE_REGISTER) {
								printf("\tStatus: Server already has this function.\n\n");
							}
						}
					} else if (call_type == RPC_CALL) {
						// Read Length of Name
						if (readNBytes(des, 4, len_buffer) == -1) {
							FD_CLR(des, &fdactive);
							close(des);
							break;
						}
						arrToInt(&length, len_buffer);
						f_data = (char *)malloc(length * sizeof(char));

						// Read name
						result = readNBytes(des, length, f_data);
						if (result == -1) {
							free(f_data);
							break;
						}

						// Put into better data type
						name = std::string(f_data, length);
						free(f_data);

						// Read length of function data
						if (readNBytes(des, 4, len_buffer) == -1) {
							FD_CLR(des, &fdactive);
							close(des);
							break;
						}
						arrToInt(&param_count, len_buffer);
						param_t *params = (param_t *)malloc(sizeof(param_t) * param_count);
						int bad = 0;
						int param;
						for (int i = 0; i < param_count; ++i) {
							if (readNBytes(des, 4, len_buffer) == -1) {
								bad = 1;
								FD_CLR(des, &fdactive);
								close(des);
								break;
							}
							arrToInt(&param, len_buffer);
							params[i].input = (unsigned char)((param & INPUT_BIT) > 0);
							params[i].output = (unsigned char)((param & OUTPUT_BIT) > 0);
							params[i].type = (0x00FF0000 & param) >> 16;
							params[i].length = 0x0000FFFF & param;
						}

						func_def_t new_function;
						new_function.name = name;
						new_function.param_count = param_count;
						new_function.params = params;

						server_t server = findFunction(new_function);

						if (server.port == 0x0) {
							unsigned char message_type = RPC_FAILURE;
							write(des, &message_type, 1);
						} else {
							if (VERBOSE_OUTPUT == 1) {
								printf("Client Asked for Function, Gave Server: %s, Port: %d\n", server.server.c_str(),
									   server.port);
							}
							unsigned char message_type = RPC_CALL;
							write(des, &message_type, 1);
							char int_arr[4];
							intToArr(server.server.length(), int_arr);
							write(des, &int_arr, 4);
							write(des, server.server.c_str(), server.server.length());

							intToArr(server.port, int_arr);
							write(des, &int_arr, 4);
						}
					} else if (call_type == RPC_TERMINATE) {
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