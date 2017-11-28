# bftp
Basic ftp in C

**Proyecto III - Sistemas Operativos**


**Para compilar:**
 * gcc -pthread -o bftp bftp.c client.c server.c


**Uso del programa:**
 * ./bftp

**Opciones:**
 * bftp://<usuario>:<contraseña>@<dirección-ip>/<url-ruta> : Conectar con una máquina remota
 * open <dirección-ip> : establece una conexión remota
 * close : cierra la conexión actual
 * pwd : muestra el directorio activo remoto