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


bool debug_server;
const char *port;

pthread_t server_thread;

// Para pasar parámetros al thread del servidor
struct workerArgs {
    int socket;
};

int server_init();
void * server_main_thread(void *args);
void * server_client_thread(void *args);


#endif // SERVER_H_INCLUDED