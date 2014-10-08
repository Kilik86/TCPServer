#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr for INADDR_ANY
#include <string.h> //for splitting (strtok)
#include <pthread.h> //thread library
#include <time.h>
#include <unistd.h> //for function close()

void* SocketHandler(void*);

int main(void) {
	//socket parameters
	int server_socket_desc;
	int clientAddressLength = sizeof(struct sockaddr_in);
	struct sockaddr_in server_addr, client_addr;

	const unsigned short int PORT_NUMBER = 8000;

//Create socket
	// socket(int domain, int type, int protocol)
	//returns a descriptor for the new socket. On error -1 is returned
	//socket() is a linux system call
	//"AF_INET" means that the protocol used is IPv4,"SOCK_STREAM"
	//0 stands for TCP
	//DONE try to put IPPROTO_TCP instead of 0
	server_socket_desc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket_desc < -1) {
		printf("Could not create socket");
	}
	puts("Socket created");

	//Prepare the sockaddr_in structure
	server_addr.sin_family = AF_INET;  //it should be always set to AF_INET
	//set the server address
    server_addr.sin_addr.s_addr = inet_addr("192.168.123.240");
	 //server_addr.sin_addr.s_addr = inet_addr("31.185.101.35");
	//server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(PORT_NUMBER);

	//Bind
	if (bind(server_socket_desc, (struct sockaddr *) &server_addr,
			sizeof(server_addr)) < 0) {
		//print the error message
		perror("bind failed. Error");
		return 1;
	}
	puts("bind done");

	//Listen
	listen(server_socket_desc, 10);

	//Accept and incoming connection
	puts("Waiting for incoming connections...");

	//accept connection from an incoming client
	while (1) {
		
		int temp_socket_desc = accept(server_socket_desc,(struct sockaddr *) &client_addr, (socklen_t*) &clientAddressLength);
		if(temp_socket_desc != -1){

		printf("----------\nConnection accepted \n");

		pthread_t thread_id;
		int *client_socket_desc = (int*)malloc(sizeof(int));
		*client_socket_desc=temp_socket_desc;

		pthread_create(&thread_id, NULL, &SocketHandler,(void*) &client_socket_desc);
		pthread_detach(thread_id);
		 puts("handler assigned");
		 //if thread has not terminated, pthread_detach() shall not cause it to terminate

		}
		else
			puts("connection refused");

	}

	close(server_socket_desc);
	//mysql_close(mysql_conn);
	return 0;
}

/*
 * This will handle connection for each client
 * */
void* SocketHandler(void* lp) {
	int *csock = (int*) lp;

	char buffer[128];
	int buffer_len = 128;
	int bytecount;
	memset(buffer, 0, buffer_len);
	if ((bytecount=read(*csock, buffer, buffer_len) == -1)) {
		fprintf(stderr, "Error receiving data\n");
		close(*csock);
		return 0;
	}
	printf("Received bytes %d\nReceived string \"%s\"\n", bytecount, buffer);

	close(*csock);

	puts("exiting thread");
	pthread_exit(0);
	return 0;
}
