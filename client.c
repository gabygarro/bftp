#include "client.h"

int client_init(char _username[], char _password[], char _ip_address[], char _url[]) {
    debug_client = true;
    // Guardar valores
    strcpy(username, _username);
    strcpy(password, _password);
    strcpy(ip_address, _ip_address);
    strcpy(url, _url);

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

    /* Escuchar un mensaje*/
    /*nbytes = recv(sock, buffer, sizeof(buffer) - 1, 0); 
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
    }*/

    /* Directorio actual */
    client_cd(NULL); // Por si acaso, poner el directorio actual en home
    client_cd(_url);
    
    
    //close(sock);
    
    return 0;
}

int client_close() {
    close(sock);
    if (debug_client) printf("\n * Cliente: Conexión cerrada.\n> ");
    return 0;
}

int client_open(char _ip_address[]) {
    client_close();
    client_init(username, password, _ip_address, url);
}

char * client_pwd() {
    int nbytes;

    // Enviar mensaje
    nbytes = send(sock, "pwd", 3, 0);
    if (nbytes == -1 && (errno == ECONNRESET || errno == EPIPE)) {
        fprintf(stderr, "\n* Cliente: Socket %d desconectado\n> ", sock);
        close(sock);
        exit(-1);
    }
    else if (nbytes == -1) { // Si ocurrió algún error?
        perror("\n** ERROR: Cliente: Unexpected error in send()");
        close(sock);
        exit(-1);
    }

    // Esperar mensaje
    nbytes = recv(sock, client_buffer, sizeof(client_buffer) - 1, 0);
    if (nbytes == 0) {
        perror("\n** ERROR: Servidor: Cliente cerró la conexión.");
        close(sock);
        exit(-1);
    }
    else if (nbytes == -1) {
        perror("\n** ERROR: Servidor: Socket recv() failed");
        close(sock);
        exit(-1);
    }
    else {
        client_buffer[nbytes] = '\0'; //Por si acaso
        return client_buffer;
    }
    return "";
}

int client_cd(char * dir) {
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