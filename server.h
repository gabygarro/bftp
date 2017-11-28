#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <sys/types.h>
#include <pthread.h> 
#include <sys/socket.h>
//#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

#define MAX_PROMPT_LENGTH 256
#define MAX_URL_LENGTH 512

bool debug_server;
const char * port;

pthread_t server_thread;

// Para pasar parámetros al thread del servidor
struct workerArgs {
    int socket;
};

char url[MAX_URL_LENGTH];
char server_buffer [MAX_PROMPT_LENGTH + 1 ]; // +1 for '\0'
int nbytes; // Num de bytes recibidos

int server_init();
void * server_main_thread(void *args);
void * server_client_thread(void *args);

char * server_pwd();
int server_change_dir(char * dir);
int server_cd();
int server_recv(int socket);
int server_send(int socket, char * msg);

#endif // SERVER_H_INCLUDED