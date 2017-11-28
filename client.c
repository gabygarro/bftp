#include "client.h"

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

int client_init(char _username[], char _password[], char _ip_address[], char _url[]) {
    debug_client = true;
    // Guardar valores
    strcpy(username, _username);
    strcpy(password, _password);
    strcpy(ip_address, _ip_address);
    strcpy(url, _url);

	//int sock; //Descriptor del socket
	//struct sockaddr_in server; // Struct con la información del servidor
	char buffer[100 + 1]; // +1 for '\0'
    int nbytes; // Num de bytes recibidos

	// Crear socket
	if ((sock = socket(AF_INET, SOCK_STREAM , 0)) == -1) {
        printf("\n** ERROR: Cliente: No se puede crear socket.");
        return 1;
	}
	if (debug_client) printf("\n* Cliente: Socket creado.\n> ");

	server.sin_addr.s_addr = inet_addr(ip_address);
	server.sin_family = AF_INET;
	server.sin_port = htons(8889);

	// Conectar al servidor
	if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
        close(sock);
	    perror("\n** ERROR: Cliente: Conexión con el servidor falló");
	    return 1;
	}

    if (debug_client) printf("\n* Cliente: Conectado al servidor.\n> ");

    
    nbytes = recv(sock, buffer, sizeof(buffer) - 1, 0); 
    if (nbytes == 0) {
        perror("\n** ERROR: Cliente: Servidor cerró la conexión.");
        close(sock);
        exit(-1);
    }
    else if (nbytes == -1) {
        perror("\n** ERROR: Cliente: Socket recv() failed");
        close(sock);
        exit(-1);
    }
    else {
        buffer[nbytes] = '\0'; //Por si acaso
        printf("Mensaje: %s \n", buffer);
    }
    
    
    //close(sock);
    
    return 0;
}

int client_close() {
    close(sock);
    if (debug_client) printf("\n * Cliente: Conexión cerrada.\n> ");
    return 0;
}

int client_open(char _ip_address[]) {
    client_init(username, password, _ip_address, url);
}