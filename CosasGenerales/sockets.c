#include "sockets.h"
#include <string.h>

#define IP "127.0.0.1"
#define PUERTO "6667"

// *******************************
// *           Servidor			 *
// *******************************

/*
int newSocket() {
	int fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
		if (fileDescriptor == FAIL) manejaError("[ERROR] Funcion socket");
	return fileDescriptor;
}

struct sockaddr_in asociarSocket(int sockfd, int puerto) {
	struct sockaddr_in miDireccion;
	miDireccion.sin_family = AF_INET;
	miDireccion.sin_port = htons(puerto);
	miDireccion.sin_addr.s_addr = 0; // htonl(INADDR_ANY); // Usa mi direccion IP
	memset(&(miDireccion.sin_zero), '\0', 8);

	// Si el puerto esta en uso, lanzamos error
	int enUso = 1;
	int estaEnUso = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*) &enUso, sizeof(enUso));
		if (estaEnUso == FAIL)	manejaError("[ERROR] No es posible reutilizar el socket");
	



	// Funcion bind
	int funcionBind = bind(sockfd, (struct sockaddr *) &miDireccion,sizeof(struct sockaddr));
		if ( funcionBind == FAIL) manejaError("[ERROR] Funcion BIND: No se pudo asociar con el puerto");

	return miDireccion;
}

*/
int crearSocketServidor(int puerto, int ip){


	struct addrinfo hints;
	struct addrinfo *serverInfo;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // No importa si uso IPv4 o IPv6
	hints.ai_flags = AI_PASSIVE; // Asigna el address del localhost: 127.0.0.1 //tambien podria usarse la variable ip
	hints.ai_socktype = SOCK_STREAM; // Indica que usaremos el protocolo TCP
	getaddrinfo(NULL, PUERTO, &hints, &serverInfo); // Notar que le pasamos NULL como IP, ya que le indicamos que use localhost en AI_PASSIVE podria ir si no la variable ip tambien
	int sockfd = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);


	// Si el puerto esta en uso, lanzamos error
		int enUso = 1;
		int estaEnUso = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*) &enUso, sizeof(enUso));
			if (estaEnUso == FAIL)	manejaError("[ERROR] No es posible reutilizar el socket");

			// Funcion bind
				int funBind = bind(sockfd,serverInfo->ai_addr, serverInfo->ai_addrlen);
					if ( funBind == FAIL) manejaError("[ERROR] Funcion BIND: No se pudo asociar con el puerto");



	return sockfd;
}


void escucharSocket(int sockfd, int conexionesEntrantesPermitidas) {
	printf("Estoy escuchando...");
	int funcionListen = listen(sockfd, conexionesEntrantesPermitidas);
		if ( funcionListen == FAIL) manejaError("[ERROR] Funcion listen");
}

int aceptarConexionSocket(int sockfd) {
	struct sockaddr_storage cliente;
	unsigned int addres_size = sizeof(cliente);

	int fdCliente = accept(sockfd, (struct sockaddr*) &cliente, &addres_size);
		if(fdCliente == FAIL) manejaError("[ERROR] Funcion accept");

	return fdCliente;
}

// *******************************
// *           Cliente			 *
// *******************************


int crearSocketCliente(int fdServidor, int puerto, int ip)
{

	 struct addrinfo hints;
	 struct addrinfo *serverInfo;
	 memset(&hints, 0, sizeof(hints));
	 hints.ai_family = AF_UNSPEC; // Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	 hints.ai_socktype = SOCK_STREAM; // Indica que usaremos el protocolo TCP
	 getaddrinfo(IP, PUERTO, &hints, &serverInfo); // Carga en serverInfo los datos de la conexion

	 int serverSocket;
	 serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);

	// connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
	 //freeaddrinfo(serverInfo);

return serverSocket;

}


int conectarSocket(int sockfd, const char * ipDestino, int puerto){
	struct sockaddr_in datosServidor;

	datosServidor.sin_family = AF_INET;
	datosServidor.sin_port = htons(puerto);
	datosServidor.sin_addr.s_addr = inet_addr(ipDestino);
	memset(&(datosServidor.sin_zero), '\0', 8);

	int funcionConnect = connect(sockfd, (struct sockaddr *) &datosServidor, sizeof(struct sockaddr));
		if ( funcionConnect == FAIL) return FAIL;
	return 0;
}
// *******************************
// *       Enviar y Recibir		 *
// *******************************

int enviarPorSocket(int fdCliente, const void * mensaje, int tamanio) {
	int bytes_enviados;
	int total = 0;

	while (total < tamanio) {
		bytes_enviados = send(fdCliente, mensaje + total, tamanio, 0);
		if (bytes_enviados == FAIL) {
			break;
		}
		total += bytes_enviados;
		tamanio -= bytes_enviados;
	}
	if (bytes_enviados == FAIL) manejaError ("[ERROR] Funcion send");

	return bytes_enviados;
}

int recibirPorSocket(int fdCliente, void * buffer, int tamanio) {
	int total = 0;
	int bytesRecibidos;

	while (total < tamanio) {
		bytesRecibidos = recv(fdCliente, buffer + total, tamanio, 0);
		if (bytesRecibidos == FAIL) {
			// Error
			perror("[ERROR] Funcion recv");
			break;
		}
		if (bytesRecibidos == 0) {
			// Desconexion
			break;
		}
		total += bytesRecibidos;
		tamanio -= bytesRecibidos;
	}
	return bytesRecibidos;
}	// retorna -1 si fallo, 0 si se desconecto o los bytes recibidos

// *******************************
// *    Cerrar File Descriptor	 *
// *******************************
void cerrarSocket(int sockfd) {
	int funcionClose = close(sockfd);
		if (funcionClose == FAIL) manejaError("[ERROR] Funcion CLOSE: Error al cerrar el file descriptor");
}

// *******************************
// *       	  Select			 *
// *******************************
void seleccionarSocket(int maxNumDeFD,
					   fd_set *fdListoLectura, fd_set *fdListoEscritura, fd_set *fdListoEjecucion,
					   int* segundos, int* milisegundos) {
	inicio:
	if(segundos == NULL || milisegundos == NULL){
		// Si segundos = NULL o miliSegundos = NULL => espera infinitamente
		int funcionSelect = select((maxNumDeFD + 1), fdListoLectura, fdListoEscritura, fdListoEjecucion, NULL);
		if(funcionSelect == FAIL && errno != EINTR) manejaError("[ERROR] Funcion select");

	}else{
		struct timeval tv;
		// Si segundos y milisegundos apuntan a 0 => select regresa inmediatamente despues de interrogar a todos sus fd
		int sec = *segundos;
		int mic = (*milisegundos) * 1000;
		tv.tv_sec  = sec;
		tv.tv_usec = mic;

		int selectConTiempo = select((maxNumDeFD + 1), fdListoLectura,
								  fdListoEscritura, fdListoEjecucion, &tv);
		if(selectConTiempo == FAIL && errno != EINTR) manejaError("[ERROR] Funcion select");
	}
	if (errno == EINTR){
		errno = 0;
		goto inicio;
	}
}
