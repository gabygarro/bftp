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

#define MAX_NUM_THREADS 1

#define MAX_USERNAME_LENGTH 48
#define MAX_PASSWORD_LENGTH 32
#define MAX_IPADRESS_LENGTH 45
#define MAX_URL_LENGTH 512

char username[MAX_USERNAME_LENGTH];
char password[MAX_PASSWORD_LENGTH];
char ip_address[MAX_IPADRESS_LENGTH];
char url[MAX_URL_LENGTH];

bool debug_client;

int client_init(char _username[], char _password[], char _ip_address[], char _url[]);



#endif // CLIENT_H_INCLUDED