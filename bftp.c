/*
 * Proyecto III - Sistemas Operativos
 * Gabriela Garro Abdykerimov
 * 26 de Noviembre 2017
 *
 * Para compilar: 
 * gcc -pthread -o bftp bftp.c client.c server.c
 * 
 * Uso del programa:
 * ./bftp
 *
 * Opciones:
 * * bftp://<usuario>:<contraseña>@<dirección-ip>/<url-ruta> : Conectar con una máquina remota
 * * open <dirección-ip> : establece una conexión remota
 * * close : cierra la conexión actual
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/time.h>

#include "client.h"
#include "server.h"

#define MAX_PROMPT_LENGTH 256
#define MAX_USERNAME_LENGTH 48
#define MAX_PASSWORD_LENGTH 32
#define MAX_IPADRESS_LENGTH 45
#define MAX_URL_LENGTH 512

// Variables globales
bool debug = true;
bool auto_login = false;
char username[MAX_USERNAME_LENGTH];
char password[MAX_PASSWORD_LENGTH];
char ip_address[MAX_IPADRESS_LENGTH];
char url[MAX_URL_LENGTH];

char input[MAX_PROMPT_LENGTH];

void print_help();
void print_error(char * error);
void manage_cmds();

int main(int argc, char * argv[]) {
	// Conexión inicial al principio del programa
	printf("Basic File Transfer Protocol\n");

	//Si no es bftp2 (cliente), no correr el servidor
	if (strcmp(argv[0], "./bftp2") != 0) server_init();

	printf("Ingrese la dirección de conexión remota: bftp://<usuario>:<contraseña>@<dirección-ip>/<url-ruta>\n");
	printf("> ");
	if (auto_login) {
		printf("bftp://yo:micontraseña@192.168.2.7/una/ruta\n");
		strcpy(username, "yo");
		strcpy(password, "micontraseña");
		strcpy(ip_address, "192.168.2.6");
		strcpy(url, "una/ruta");
		client_init(username, password, ip_address, url);
	}
	else {
		scanf("bftp://%[^:]:%[^@]@%[^/]/%s", username, password, ip_address, url);
		// Revisar que el formato logró almacenar los datos
		if (username[0] == 0 || password[0] == 0 || ip_address[0] == 0 || url[0] == 0) {
			print_error("Formato de conexión incorrecto.");
		}
		else {
			client_init(username, password, ip_address, url);
		}
	}
	
	// No entiendo qué cosa está mandando algo por stdin.
	fgets(input, MAX_PROMPT_LENGTH - 1, stdin);

	// Ciclo de otros comandos
	bool quit = false;
	while (!quit) {
		manage_cmds();
	}
	return 0;
}

/* Texto de ayuda */
void print_help() {
	printf("\nComandos:\n");
	printf(" * bftp://<usuario>:<contraseña>@<dirección-ip>/<url-ruta> : Conectar con una máquina remota\n");
	printf(" * open <dirección-ip>: establece una conexión remota\n");
	printf(" * close : cierra la conexión actual\n");
	//exit(0);
}

/* Imprimir un error y salir del programa */
void print_error(char * error) {
	printf("ERROR: %s\n", error);
	exit(-1);
}

void manage_cmds() {
	//Obtener comando
	char arg1[MAX_PROMPT_LENGTH];
	char arg2[MAX_PROMPT_LENGTH];
	printf("> ");
	fgets(input, MAX_PROMPT_LENGTH - 1, stdin);

	strcpy(arg1, strtok(input, " \n"));
	
	// Identificar el comando
	// Quit
	if (strcmp(arg1, "quit") == 0) {
		exit(0);
	}
	// Close **********************************
	else if (strcmp(arg1, "close") == 0) {
		client_close();
	}
	// Open ***********************************
	else if (strcmp(arg1, "open") == 0) {
		//Recuperar siguiente argumento
		strcpy(arg2, strtok(NULL, " \n"));
		
		if (arg2[0] != 0) {
			strcpy(ip_address, arg2);
			client_close();
			client_open(arg2);
		}
		else {
			print_error("Falta argumento.");
		}
	}
	// Help ************************************
	else if (strcmp(arg1, "help") == 0) { // help
		print_help();
	}
	// Default *********************************
	else {
		printf("Comando incorrecto. Use help para ver los comandos disponibles.\n");
	}
}