#include "server.h"

/* Inicializa el thread del servidor */
int server_init() {
    debug_server = true;
    port = "8889";
	
	//pthread_t server_thread;

	// Ignorar la señal SIGPIPE de cuando un cliente se desconecta
	// para que no mate el proceso
	sigset_t new;
    sigemptyset (&new);
    sigaddset(&new, SIGPIPE);
    if (pthread_sigmask(SIG_BLOCK, &new, NULL) != 0) {
        perror("\n** ERROR: Unable to mask SIGPIPE");
        exit(-1);
    }

	// Crear el thread del servidor que a su vez hace más threads para atender clientes
	if (pthread_create(&server_thread, NULL, server_main_thread, NULL) < 0) {
        perror("\n** ERROR: Servidor: No se puede crear thread del servidor");
        exit(-1);
    }

    //pthread_join(server_thread, NULL);
    //pthread_exit(NULL);
    return 0;
}

/* Función del thread del servidor que hace más threads para atender a clientes */
void * server_main_thread(void *args){
    int serverSocket;
    int clientSocket;
    pthread_t worker_thread;
    struct addrinfo hints, *res, *p;
    struct sockaddr_storage *clientAddr;
    socklen_t sinSize = sizeof(struct sockaddr_storage);
    struct workerArgs *wa;
    int yes = 1;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
	// Retorna la dirección para que se le pueda hacer bind
    hints.ai_flags = AI_PASSIVE; 

    if (getaddrinfo(NULL, port, &hints, &res) != 0) { // El parámetro del host es NULL
        perror("\n** ERROR: Servidor: getaddrinfo() falló");
        pthread_exit(NULL);
    }

	// Crear los sockets
    for (p = res; p != NULL; p = p->ai_next) {
        if ((serverSocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("\n** ERROR: Servidor: No se pudo abrir el socket");
            continue;
        }
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("\n** ERROR: Servidor: Socket setsockopt() falló");
            close(serverSocket);
            continue;
        }
        if (bind(serverSocket, p->ai_addr, p->ai_addrlen) == -1) {
            perror("\n** ERROR: Servidor: Socket bind() falló");
            close(serverSocket);
            continue;
        }
        if (listen(serverSocket, 5) == -1) {
            perror("\n** ERROR: Servidor: Socket listen() falló");
            close(serverSocket);
            continue;
        }
        break;
    }
    
    freeaddrinfo(res);

    if (p == NULL) {
        fprintf(stderr, "\n** ERROR: Servidor: No se encontró un socket al cual hacer bind.\n> ");
        pthread_exit(NULL);
    }

    // Esperar clientes
    while (1) {
        //if (debug_server) printf("Servidor: Esperando conexiones...\n>");
		// El thread se bloquea hasta que se hace una conexión
        clientAddr = malloc(sinSize);
        if ((clientSocket = accept(serverSocket, (struct sockaddr *) clientAddr, &sinSize)) == -1) {
            free(clientAddr); // Si la conexión falla, no se muere el thread del servidor :)
            perror("\n** ERROR: Servidor: No se pudo aceptar conexión");
            continue;
        }

		// Se conectó con un cliente
		// Hay que pasarle al thread el struct workerArgs, que tiene el socket para conectarse
        wa = malloc(sizeof(struct workerArgs));
        wa->socket = clientSocket;

		// Crear un thread para esta conexión
        if (pthread_create(&worker_thread, NULL, server_client_thread, wa) != 0) {
            perror("\n** ERROR: Servidor: No se pudo crear un thread para la conexión");
			// Liberar todo
            free(clientAddr);
            free(wa);
            close(clientSocket);
            close(serverSocket);
            pthread_exit(NULL);
        }
    }

    pthread_exit(NULL);
}

/* Thread que atiende a cada cliente */
void * server_client_thread (void *args) {
    struct workerArgs *wa;
    int socket, nbytes;
    char tosend[100]; // Mensaje a enviar
	// Traer la conexión del socket
    wa = (struct workerArgs*) args;
    socket = wa->socket;

	// Este thread es independiente y no se tiene que comunicar con otro cliente
    pthread_detach(pthread_self());

    if (debug_server) fprintf(stderr, "\n* Servidor: Socket %d conectado\n> ", socket);

	// Loop que manda mensajes al cliente
    while(1) {
        sprintf(tosend,"%d -- Hellow, socket!\n", (int) time(NULL));

        nbytes = send(socket, tosend, strlen(tosend), 0); // enviar mensaje

		// Si no se envió el mensaje
        if (nbytes == -1 && (errno == ECONNRESET || errno == EPIPE)) {
            fprintf(stderr, "\n* Servidor: Socket %d desconectado\n> ", socket);
            close(socket);
            free(wa);
            pthread_exit(NULL);
        }
        else if (nbytes == -1) { // Si ocurrió algún error?
            perror("\n** ERROR: Servidor: Unexpected error in send()");
            free(wa);
            pthread_exit(NULL);
        }
        sleep(5);
    }

    pthread_exit(NULL);
}