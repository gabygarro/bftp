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

	// Loop que espera mensajes de los clientes
    while (1) {
        int flag = server_recv(socket);
        if (flag == -1) { // Si falló
            free(wa);
            close(socket);
            pthread_exit(NULL);
        }
        server_buffer[nbytes] = '\0'; //Por si acaso
        printf("\n* Socket %d: %s \n", socket, server_buffer);

        //Enviar mensaje con lo que pide
        //char * pwd = server_pwd();
        if (strcmp(server_buffer, "pwd") == 0) {
            char * pwd = "dir";
            flag = server_send(socket, pwd);
            if (flag == -1) {
                free(wa);
                pthread_exit(NULL);
            }
        }
        else {
            printf("No implementado\n");
            server_send(socket, "No implementado");
        }
            
    }

    /*while(1) {
        sprintf(tosend,"%d -- Hellow, socket!\n", (int) time(NULL));

         // enviar mensaje

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
    }*/

    pthread_exit(NULL);
}

/* Recibe un mensaje y lo guarda en server_buffer
    Si retorna un -1, no pudo recibir nada */
int server_recv(int _socket) {
    nbytes = recv(_socket, server_buffer, sizeof(server_buffer) - 1, 0);
    if (nbytes == 0) {
        perror("\n** ERROR: Servidor: Cliente cerró la conexión.");
        return -1;
    }
    else if (nbytes == -1) {
        perror("\n** ERROR: Servidor: Socket recv() failed");
        return -1;
    }
    return 0;
}

/* Envía un mensaje */
int server_send(int _socket, char * msg) {
    int bytes = send(_socket, msg, strlen(msg), 0);
    if (bytes == -1 && (errno == ECONNRESET || errno == EPIPE)) {
        fprintf(stderr, "\n* Servidor: Socket %d desconectado\n> ", _socket);
        close(_socket);
        return -1;
    }
    else if (bytes == -1) { // Si ocurrió algún error?
        perror("\n** ERROR: Servidor: Unexpected error in send()");
        return -1;
    }
    return 0;
}

/* Retorna el directorio actual */
char * server_pwd() {
    return url;
}

/* Cambia el directorio actual del servidor */
int server_change_dir(char * dir) {
    // Si es vacío, cambiar el directorio a home
    if (dir == NULL) {
        chdir(getenv("HOME"));
        //url = getenv("HOME");
        strcpy(url, getenv("HOME"));
		return 1;
	}
	else { 
		if (chdir(dir) == -1) {
			printf("ERROR: Directorio %s no existe\n", dir);
            return -1;
        }
        else {
            //url = dir; //Actualizar el nombre del directorio actual
            strcpy(url, dir);
        }
	}
	return 0;
}