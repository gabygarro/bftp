#ifndef CLIENT_H_INCLUDED
#define CLIENT_H_INCLUDED

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#define MAX_BUFFER_LENGTH 256

#define MAX_USERNAME_LENGTH 48
#define MAX_PASSWORD_LENGTH 32
#define MAX_IPADRESS_LENGTH 45
#define MAX_URL_LENGTH 512

char username[MAX_USERNAME_LENGTH];
char password[MAX_PASSWORD_LENGTH];
char ip_address[MAX_IPADRESS_LENGTH];
char url[MAX_URL_LENGTH];

bool debug_client;

int sock; //Descriptor del socket
struct sockaddr_in server; // Struct con la información del servidor
/*  struct addrinfo {
		int ai_flags;
		int ai_family;
		int ai_socktype;
		int ai_protocol;
		size_t ai_addrlen;
		struct sockaddr *ai_addr;
		char *ai_canonname;
		struct addrinfo *ai_next;
	};
*/
char client_buffer[MAX_BUFFER_LENGTH + 1]; // +1 for '\0'
int nbytes; // Num de bytes recibidos

int client_init(char _username[], char _password[], char _ip_address[], char _url[]);
int client_close();
int client_open(char _ip_address[]);
char * client_pwd();
int client_cd(char * dir);


#endif // CLIENT_H_INCLUDED