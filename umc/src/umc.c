#include "umc.h"
//#define manejarError(msg) {perror(msg); abort();} Quien garcha hizo esto?

//Ideas locas para crear las estructuras para manejar la memoria DR Mengueche

int calcularIDPagina(int inicio) {

	if (inicio == 0)
		return 0;
	else {
		return (inicio / marcos);
	}

}

void iniciarEstructurasUMC() {
	int counter = 0;
	bitMap = malloc(sizeof(bool) * marcos);
	crearListas();
	while (counter != marcos) {
		bitMap[counter] = 0;
		counter++;
	}

}

void crearListas() {
	listaEspacioAsignado = list_create();
}

bool inicializarPrograma(int pid, int paginas, char*codigo) { //todo falta enviar el programa al swap nico chupame la pija
	if (verificarSiHayEspacio(paginas)) {
		if (paginasContiguasDeUMC(paginas) == -1) {
			compactarUMC();
			reservarPaginas(paginasContiguasDeUMC(paginas), pid, paginas);
		} else {
			reservarPaginas(paginasContiguasDeUMC(paginas), pid, paginas);
		}
		SocketBuffer*buffer;
		StrUmcSwa*streamUmcSwap;
		espacioAsignado pagina;
		StrSwaUmc * streamSwapUmc;
		pagina->numDePag=0;
		streamUmcSwap=newStrUmcSwa(UMC_ID,INICIALIZAR_PROGRAMA,pagina,paginas,codigo,0,pid);
		buffer=serializeUmcSwa(streamUmcSwap);
		socketSend(socketSwap->ptrSocket,buffer);

		buffer = socketReceive(socketSwap->ptrSocket);
		streamSwapUmc = unserializeSwaUmc(buffer->data);

		if(streamSwapUmc->action == PROGRAMA_RECIBIDO)
			return TRUE;

	} else
		return FALSE;

}

void reservarPaginas(int paginaDeComienzo, int pid, int cantidadDePaginas) {
	int numInternoDePagina = 0;
	int paginaActual = paginaDeComienzo; // donde empieza toodo.
	int lugarEnDondeDeboColocarMiNodo = 0; // aca se en donde tengo que meter esto
	int nodosQueDeboReventar = 0; // los nodos que quiero fusilar en donde empiezan
	int contadorDePaginas = 0; //cuento para el while
	espacioAsignado* paginaAReservar; //nodo para agarrar cosas
	if (list_size(listaEspacioAsignado) != 0) {
		paginaAReservar = list_get(listaEspacioAsignado,
				lugarEnDondeDeboColocarMiNodo);
		while (((paginaAReservar->IDPaginaInterno) < paginaDeComienzo)) {
			lugarEnDondeDeboColocarMiNodo++;
			paginaAReservar = list_get(listaEspacioAsignado,
					lugarEnDondeDeboColocarMiNodo);
			if ((lugarEnDondeDeboColocarMiNodo
					== (listaEspacioAsignado->elements_count))) {
				break;
			}
		}

	}

	while (contadorDePaginas < cantidadDePaginas) {

		paginaAReservar = malloc(sizeof(espacioAsignado));
		if (contadorDePaginas == 0)
			paginaAReservar->punteroAPagina = 1;
		else
			paginaAReservar->punteroAPagina = 0;
		(paginaAReservar->pid) = pid;
		(paginaAReservar->IDPaginaInterno) = paginaActual;
		(paginaAReservar->numDePag) = numInternoDePagina;
		(paginaAReservar->bitModificado) = 0;
		(paginaAReservar->bitUso) = 1;
		if (lugarEnDondeDeboColocarMiNodo < list_size(listaEspacioAsignado))
			list_add_in_index(listaEspacioAsignado,
					lugarEnDondeDeboColocarMiNodo, paginaAReservar);
		else
			list_add(listaEspacioAsignado, paginaAReservar);
		bitMap[paginaActual] = 1;
		numInternoDePagina++;
		contadorDePaginas++;
		paginaActual++;
		lugarEnDondeDeboColocarMiNodo++;
	}

}

void compactarUMC() {

	int paginasContiguas = 0;
	espacioAsignado* nodoActual = malloc(sizeof(espacioAsignado));
	int contadorParaCadenaActual;
	int contadorParaCadenaVieja;
	nodoActual = list_get(listaEspacioAsignado, paginasContiguas);
	while (paginasContiguas < list_size(listaEspacioAsignado)) {
		if ((nodoActual->IDPaginaInterno) != paginasContiguas) {
			contadorParaCadenaActual = paginasContiguas * marcos;
			contadorParaCadenaVieja = (nodoActual->IDPaginaInterno) * marcos;
			while (contadorParaCadenaActual < (paginasContiguas + 1) * marcos) {
				memoriaReal[contadorParaCadenaActual] =
						memoriaReal[contadorParaCadenaVieja];
				memoriaReal[contadorParaCadenaVieja] = '\0';
				contadorParaCadenaActual++;
				contadorParaCadenaVieja++;
			}
			bitMap[nodoActual->IDPaginaInterno] = 0;
			(nodoActual->IDPaginaInterno) = paginasContiguas;
			bitMap[paginasContiguas] = 1;

		}
		paginasContiguas++;
		nodoActual = list_get(listaEspacioAsignado, paginasContiguas);

	}
	while (paginasContiguas != marcos) {

		bitMap[paginasContiguas] = 0;
		paginasContiguas++;

	}

}

bool verificarSiHayEspacio(int cantidadDePaginas) {

	int contador = 0;
	int cantidad = 0;
	while (contador < marcos) {
		if (bitMap[contador] == 0) {
			cantidad++;
		}
		contador++;
	}
	if (cantidad >= cantidadDePaginas)
		return TRUE;
	else
		return FALSE;
}

int paginasContiguasDeUMC(int cantidadDePaginas) {
	int paginaADevolver = 0;
	int contador = 0;
	int paginasSeguidasLibres = 0;
	while ((contador < marcos) && (paginasSeguidasLibres != 1)) {
		if (bitMap[contador] == 0) {
			paginasSeguidasLibres = 1;
			paginaADevolver = contador;
		}
		contador++;
	}
	if (cantidadDePaginas == paginasSeguidasLibres)
		return paginaADevolver;
	while (contador < marcos) {
		if (bitMap[contador] == 0) {
			if (paginasSeguidasLibres == 0) {
				paginasSeguidasLibres = 1;
				paginaADevolver = contador;
			} else {
				paginasSeguidasLibres++;
			}
		} else {
			paginasSeguidasLibres = 0;
		}
		if (cantidadDePaginas == paginasSeguidasLibres) {
			return paginaADevolver;
		}
		contador++;
	}
	return -1;
}

//devuelve el nodo del espacio en memoria
int reemplazarPaginaClock(int pid, int pagina) {

	int inicio = lugarAsignadoInicial(pid);
	int fin = lugarAsignadoFinal(pid);
	int contador = encontrarPuntero(pid);
	int posicionDePaginaLibre;
	espacioAsignado*nodoActual = list_get(listaEspacioAsignado, contador);
	while ((nodoActual->bitUso) == 1) {
		nodoActual->bitUso = 0;
		contador++;
		if (contador > fin)
			contador = inicio;
		nodoActual = list_get(listaEspacioAsignado, contador);
	}
	posicionDePaginaLibre = nodoActual->IDPaginaInterno;
	espacioAsignado*nodoSiguiente = (list_get(listaEspacioAsignado,
			(contador + 1)));
	nodoSiguiente->punteroAPagina = 1;
	//todo llevar pagina al swap
	limpiarPagina(nodoActual->IDPaginaInterno * marco_Size);
	//todo traer pagina del swap
	nodoActual->numDePag = pagina;
	nodoActual->bitUso = 1;
	//todo acomodar pagina
	return posicionDePaginaLibre;
}

int reemplazarPagina(int pid, int pagina, bool lectoEscritura) {
	if (!strcmp(algoritmoDeReemplazo, "CLOCK"))
		return reemplazarPaginaClock(pid, pagina);
	else
		return reemplazarPaginaClockModificado(pid, pagina, lectoEscritura);
}

//devuelve el nodo del espacio en memoria. 0 lectura 1 escritura
int reemplazarPaginaClockModificado(int pid, int pagina, bool lectoEscritura) {

	int inicio = lugarAsignadoInicial(pid);
	int fin = lugarAsignadoFinal(pid);
	int comienzoDelPuntero = encontrarPuntero(pid);
	int contador = comienzoDelPuntero;
	int posicionDePaginaLibre;
	espacioAsignado*nodoActual;
	while (!(((nodoActual->bitUso) == 0) && ((nodoActual->bitModificado) == 0))) {

		nodoActual = list_get(listaEspacioAsignado, contador);
		contador++;
		if (contador > fin)
			contador = inicio;
		if (contador == comienzoDelPuntero)
			break;
	}
	if (contador == comienzoDelPuntero) {
		while (!(((nodoActual->bitUso) == 1)
				&& ((nodoActual->bitModificado) == 0))) {
			nodoActual->bitUso = 0;
			nodoActual = list_get(listaEspacioAsignado, contador);
			contador++;
			if (contador > fin)
				contador = inicio;
			if (contador == comienzoDelPuntero)
				break;
		}
		if (contador == comienzoDelPuntero) {
			while (!(((nodoActual->bitUso) == 0)
					&& ((nodoActual->bitModificado) == 0))) {

				nodoActual = list_get(listaEspacioAsignado, contador);
				contador++;
				if (contador > fin)
					contador = inicio;
				if (contador == comienzoDelPuntero)
					break;
			}
			if (contador == comienzoDelPuntero) {
				while (!(((nodoActual->bitUso) == 0)
						&& ((nodoActual->bitModificado) == 1))) {

					nodoActual = list_get(listaEspacioAsignado, contador);
					contador++;
					if (contador > fin)
						contador = inicio;
					if (contador == comienzoDelPuntero)
						break;
				}
			}
		}
	}
	posicionDePaginaLibre = nodoActual->IDPaginaInterno;
	espacioAsignado*nodoSiguiente = (list_get(listaEspacioAsignado,
			(contador + 1)));
	nodoSiguiente->punteroAPagina = 1;
	//todo llevar pagina al swap
	limpiarPagina(nodoActual->IDPaginaInterno * marco_Size);
	//todo traer pagina del swap
	nodoActual->numDePag = pagina;
	nodoActual->bitUso = 1;
	nodoActual->bitModificado = lectoEscritura;
	//todo acomodar pagina
	return posicionDePaginaLibre;
}

void limpiarPagina(int comienzoDePagina) {
	int contador = comienzoDePagina;
	int finDePagina = comienzoDePagina + marco_Size;
	while (contador != finDePagina) {
		memoriaReal[contador] = '\0';
	}

}

int encontrarPuntero(int pid) {
	int inicio = lugarAsignadoInicial(pid);
	int fin = lugarAsignadoFinal(pid);
	int contador = inicio;
	espacioAsignado*nodoActual = list_get(listaEspacioAsignado, inicio);
	while ((nodoActual->punteroAPagina) != 1) {
		contador++;
		if (contador > fin)
			contador = inicio;
		nodoActual = list_get(listaEspacioAsignado, contador);
	}
	nodoActual->punteroAPagina = 0;
	return contador;
}

int lugarAsignadoFinal(int pid) {
	int inicio = lugarAsignadoInicial(pid);
	espacioAsignado*nodoFinal;
	nodoFinal = list_get(listaEspacioAsignado, inicio);
	while (nodoFinal->pid == pid) {
		inicio++;
		nodoFinal = list_get(listaEspacioAsignado, inicio);
	}
	return (inicio - 1);
}

int lugarAsignadoInicial(int pid) {
	int contador = 0;
	espacioAsignado*nodoActual = list_get(listaEspacioAsignado, contador);
	while (nodoActual->pid != pid) {
		contador++;
		nodoActual = list_get(listaEspacioAsignado, contador);
	}
	return contador;
}

char* solicitarBytes(int pid, int pagina, int offset, int cantidad) {
	char paginaADevolver[cantidad];
	char*punteroADevolver = (&paginaADevolver[0]);
	espacioAsignado* nodoALeer;
	int posicionActualDeNodo = 0;
	nodoALeer = list_get(listaEspacioAsignado, posicionActualDeNodo);
	while (((nodoALeer->pid) != pid) && (nodoALeer->numDePag != pagina)) {
		posicionActualDeNodo++;
		nodoALeer = list_get(listaEspacioAsignado, posicionActualDeNodo);
	}
	if (posicionActualDeNodo >= list_size(listaEspacioAsignado)) {
		int frame = reemplazarPagina(pid, pagina, 1);
		int comienzoDeCadena = frame * marco_Size + offset;
		int finDeCadena = frame * marco_Size + offset;
		int lugarDeLaCadena = 0;
		while (lugarDeLaCadena < cantidad) {
			paginaADevolver[lugarDeLaCadena] = memoriaReal[comienzoDeCadena];
			comienzoDeCadena++;
			lugarDeLaCadena++;
		}
		return (punteroADevolver);
	} else {
		(nodoALeer->bitUso) = 1;
		int lugarDeLaCadena = 0;
		int posicionDeChar = (nodoALeer->IDPaginaInterno) * marco_Size + offset;
		while (lugarDeLaCadena < cantidad) {
			paginaADevolver[lugarDeLaCadena] = memoriaReal[posicionDeChar];
			posicionDeChar++;
			lugarDeLaCadena++;
		}
		return (punteroADevolver);
	}
}

void almacenarBytes(int pid, int pagina, int offset, int tamanio, char*buffer) {
	espacioAsignado* nodoALeer;
	int posicionActualDeNodo = 0;
	nodoALeer = list_get(listaEspacioAsignado, posicionActualDeNodo);
	while ((((nodoALeer->pid) != pid) && (nodoALeer->numDePag))
			|| (posicionActualDeNodo == list_size(listaEspacioAsignado))) {
		posicionActualDeNodo++;
		nodoALeer = list_get(listaEspacioAsignado, posicionActualDeNodo);
	}
	if (posicionActualDeNodo >= list_size(listaEspacioAsignado)) {
		//todo ir a buscar al swap la pagina
	} else {
		(nodoALeer->bitUso) = 1;
		(nodoALeer->bitModificado) = 1;
		int dondeEscribo = (nodoALeer->IDPaginaInterno) * marco_Size + offset;
		int enDondeEstoyDeLoQueMeMandaron = 0;
		int contador = 0;
		while (contador < tamanio) {
			memoriaReal[dondeEscribo] = buffer[enDondeEstoyDeLoQueMeMandaron];
			dondeEscribo++;
			enDondeEstoyDeLoQueMeMandaron++;
			contador++;
		}
	}
}

void finalizarPrograma(int pid) { //todo avisar al swap que tiene que reventar pid

	espacioAsignado*nodoAReventar;
	//int enDondeAgregarEspacio = 0;
	int nodoActualAReventar = 0;
	nodoAReventar = list_get(listaEspacioAsignado, nodoActualAReventar);
	while ((nodoAReventar->pid) != pid) {
		nodoActualAReventar++;
		nodoAReventar = list_get(listaEspacioAsignado, nodoActualAReventar);
	}
	int contador;
	int posicion;
	while ((nodoAReventar->pid) == pid) {
		contador = 0;
		posicion = nodoAReventar->pid * marco_Size;
		while (contador < marco_Size) {
			memoriaReal[posicion] = '\0';
			posicion++;
			contador++;
		}

		bitMap[nodoAReventar->IDPaginaInterno] = 0;
		nodoAReventar = list_remove(listaEspacioAsignado, nodoActualAReventar);
		free(nodoAReventar);
		nodoAReventar = list_get(listaEspacioAsignado, nodoActualAReventar);
	}

}

//Funcion básica del tp
//void cambioDeProcesoActivo(int pid, int fd) {
//	espacioAsignado*espacio;
//	int contador = 0;
//	espacio = list_get(listaEspacioLibre, contador);
//	while (pid != (espacio->pid)) {
//		contador++;
//		espacio = list_get(listaEspacioLibre, contador);
//	}
//	while (pid == (espacio->pid)) {
//		espacio->socket = fd;
//		contador++;
//		espacio = list_get(listaEspacioLibre, contador);
//	}
//}

void setearValores(t_config * archivoConfig) {
	puertoEscucha = config_get_string_value(archivoConfig, "PUERTO");
	ip_Swap = config_get_string_value(archivoConfig, "IP_SWAP");
	puerto_Swap = config_get_string_value(archivoConfig, "PUERTO_SWAP");
	marcos = config_get_int_value(archivoConfig, "MARCOS");
	marco_Size = config_get_int_value(archivoConfig, "MARCO_SIZE");
	marco_x_proc = config_get_int_value(archivoConfig, "MARCO_X_PROC");
	algoritmoDeReemplazo = config_get_string_value(archivoConfig, "ALGORITMO");
	entradas_TLB = config_get_int_value(archivoConfig, "ENTRADAS_TLB");
	espera = config_get_int_value(archivoConfig, "RETARDO");
}

char * reservarMemoria(int cantidadFrames, int capacidadFrames) {
	char * memory = calloc(1, cantidadFrames * capacidadFrames);
	log_info(logger, "Memoria real reservada", NULL);
	return memory;
}

void liberarMemoria(char * memoria_para_liberar) {
	free(memoria_para_liberar);
	puts("Memoria liberada");
}

void manageSocketConnections() {
	socketConnections = list_create();
	Socket* s = socketCreateServer(puertoEscucha);
	while (TRUE) {
		pthread_t socketConnection;
		puts("Escuchando conexiones del Kernel o CPUs.");
		socketListen(s);
		Socket* socketClient;
		socketClient = socketAcceptClient(s);
		if (socketClient != NULL) {
			puts("Alguien se conecto.");
			pthread_create(&socketConnection, NULL, manageSocketConnection,
					(void*) socketClient);
			list_add(socketConnections, &socketConnection);
		}
	}

}

void* manageSocketConnection(void* param) {
	Socket* socket = (Socket*) param;
	Boolean connected = TRUE;
	puts("Gestion de conexiones.");
	while (TRUE) {
		puts("Esperando el request.");
		SocketBuffer* sb = socketReceive(socket);
		puts("Entro el request.");
		if (sb != NULL) {
			Char id = getStreamId((Stream) sb->data);
			StrKerUmc* sku = NULL;
			StrCpuUmc* scu = NULL;
			switch (id) {
			case KERNEL_ID:
				sku = unserializeKerUmc((Stream) sb->data);
				//connected =
				manageKernelRequest(socket, sku);
				break;
			case CPU_ID:
				scu = unserializeCpuUmc((Stream) sb->data);
				connected = manageCpuRequest(socket, scu);
				break;
			default:
				connected = FALSE; //todo loggear algun error.
				break;
			}
		} else {
			puts("No pudo recibir request, desconectando al cliente.");
			connected = FALSE;
		}
	}
	return NULL;
}

Boolean manageCpuRequest(Socket* socket, StrCpuUmc* scu) {
	int pidActivo;
	SocketBuffer*buffer;
	StrCpuUmc*streamCpuUmc = scu;
	StrUmcCpu*streamUmcCpu;
	char* bytes;
	while (!CIERRE_CONEXION_CPU) {
		switch (streamCpuUmc->action) {
		case CAMBIO_PROCESO_ACTIVO:
			pidActivo = streamCpuUmc->pid;
			break;
		case SOLICITAR_BYTES:
			bytes = solicitarBytes(pidActivo, scu->pageComienzo.numDePag,
					scu->offset, scu->dataLen);
			streamUmcCpu=newStrUmcCpu(UMC_ID,SOLICITAR_BYTES,NULL,scu->offset,scu->dataLen,bytes,scu->pid);
			buffer = serializeUmcCpu(streamUmcCpu);
			socketSend(socket,buffer);
			break;
		case 26 /*ALMACENAR_BYTES*/:
			almacenarBytes(pidActivo,scu->pageComienzo.numDePag,scu->offset,scu->dataLen,scu->data);
			break;
		default:
			printf("No se pudo identificar la accion de la CPU");
			break;
		}
		buffer = socketReceive(socket);
		if (buffer == NULL)
			puts("Problemas al recibir del cpu");//todo break
		streamCpuUmc = unserializeCpuUmc(buffer);
	}
//StrUmcCpu* suc;/*= newStrUmcCpu();*/
//Boolean result = sendResponse(CPU_ID, suc, socket);

//return result;
}

void manageKernelRequest(Socket* socket, StrKerUmc* sku) {
	StrUmcKer*streamAlKerner;
	SocketBuffer*buffer;
	switch (sku->action) {
	case 20 /*INICIALIZAR_PROGRAMA*/:
		if (0 == inicializarPrograma(sku->pid, sku->cantPage, sku->data)) {
			streamAlKerner = newStrUmcKer(UMC_ID,
					21/*PROGRAMA_NO_INICIALIZADO*/,
					NULL, 0, 0, 0);
			buffer = serializeUmcKer(streamAlKerner);
			socketSend(socket, buffer);
		}
		break;
	case 22 /*FINALIZAR_PROGRAMA*/:
		finalizarPrograma(sku->pid);
		break;
	default:
		printf("No se pudo identificar la accion del Kernel");
		break;
	}

//StrUmcKer* suk ;/*= newStrUmcKer();*/
//Boolean result = sendResponse(KERNEL_ID, suk, socket);

//return result;
}
Boolean sendResponse(Char target, void* stream, Socket* socket) {
	SocketBuffer* sb = NULL;
	puts("Serializando.");
	switch (target) {
	case CPU_ID:
		sb = serializeUmcCpu((StrUmcCpu*) stream);
		puts("Enviando respuesta a la CPU.");
		break;
	case KERNEL_ID:
		sb = serializeUmcKer((StrUmcKer*) stream);
		puts("Enviando respuesta al KERNEL.");
		break;
	default:
		return FALSE;
	}

//Envio respuesta
	if (!socketSend(socket, sb)) {
		puts("No se pudo enviar el Stream.");
		if (sb != NULL) {
			free(sb);
			sb = NULL;
		}
		return FALSE;
	}
	puts("Enviado con exito");

	return TRUE;
}

void comandosUMC() {

	int opcionPrincipal;
	int opcionmemoryoTLB;
	int retardo;
	int opcionDump;
	int procesoElegido; //Los declaro aca porque no podes declarar algo adentro de un case de un switch
	int procesoQueElijo; //Los declaro aca porque no podes declarar algo adentro de un case de un switch

	while (1) {
		puts("Opcion 1: Retardo\n");
		puts("Opcion 2: Dump\n");
		puts("Opcion 3: Flush\n");

		scanf("%d", &opcionPrincipal);

		switch (opcionPrincipal) {
		case 1:
			system("clear");
			puts("Ingresar retardo\n");
			scanf("%d", &retardo);
			retardoUMC(retardo);
			system("clear");
			break;

		case 2:
			puts("seleccione el tipo de dump:\n");
			puts(
					"opción 1 : Dump de estructuras de memoria de todos los procesos\n");
			puts(
					"opción 2 : Dump de estructuras de memoria de un proceso en particular\n");
			puts(
					"opción 3 : Dump del contenido de la memoria de todos los procesos\n");
			puts(
					"opción 4 : Dump del contenido de la memoria de un proceso en particular\n");

			scanf("%d", &opcionDump);

			switch (opcionDump) {
			case 1:
				system("clear");
				dumpEstructuraDeMemoriaTodosLosProcesos();
				system("clear");
				break;
			case 2:
				system("clear");
				puts("Coloque el número de proceso: \n");
				scanf("%d", &procesoElegido);
				system("clear");
				dumpEstructuraDeMemoriaProcesoEnParticular(procesoElegido);
				break;
			case 3:
				system("clear");
				dumpContenidoDeMemoriaTodosLosProcesos();
				system("clear");
				break;
			case 4:
				system("clear");
				puts("Ingresar process ID: \n");
				scanf("%d", &procesoQueElijo);
				system("clear");
				dumpContenidoDeMemoriaProcesoEnParticular(procesoQueElijo);
				break;
			}
			break;

		case 3:
			system("clear");
			puts("Opcion 1: TLB\n");
			puts("Opcion 2: Memory\n");
			scanf("%d", &opcionmemoryoTLB);

			switch (opcionmemoryoTLB) {
			case 1:
				system("clear");
				flushTLB();
				system("clear");
				break;

			case 2:
				system("clear");
				flushMemory();
				system("clear");
				break;

			default:

				break;
			}
			break;

		default:

			break;
		}
		system("clear");
	}
}

void retardoUMC(int retardo) {
	espera = retardo;
	log_info(logger, "El retardo ha sido modificado\n");
}

void dumpEstructuraDeMemoriaProcesoEnParticular(int pid) {
	int i = 0;
	espacioAsignado * nodoActualDeAsignados;
//IMPRIMO EN PANTALLA
	printf("Paginas Asignadas al proceso: %d\n", pid);
	while (i < list_size(listaEspacioAsignado)) {
		nodoActualDeAsignados = list_get(listaEspacioAsignado, i);
		if (nodoActualDeAsignados->pid == pid) {
			printf("ID Frame: %d\n", nodoActualDeAsignados->IDPaginaInterno);
			printf("PID: %d\n", nodoActualDeAsignados->pid);
			printf("Pagina: %d\n", nodoActualDeAsignados->numDePag);
			log_info(logger, "Frame Asignado: %d \nPID: %d\n Pagina: %d\n\n",
					nodoActualDeAsignados->IDPaginaInterno,
					nodoActualDeAsignados->pid);
		}
		i++;
	}

}

void dumpEstructuraDeMemoriaTodosLosProcesos() {
	int i = 0;
	espacioAsignado * nodoActualDeAsignados;

//IMPRIMO EN PANTALLA
	puts("Paginas Asignadas:\n");
	while (i < list_size(listaEspacioAsignado)) {
		nodoActualDeAsignados = list_get(listaEspacioAsignado, i);
		printf("ID Frame: %d\n", nodoActualDeAsignados->IDPaginaInterno);
		printf("PID: %d\n", nodoActualDeAsignados->pid);
		printf("Pagina: %d\n", nodoActualDeAsignados->numDePag);
		log_info(logger, "Frame Asignado: %d \nPID: %d\n Pagina: %d\n\n",
				nodoActualDeAsignados->IDPaginaInterno,
				nodoActualDeAsignados->pid);
		i++;
	}

	puts("Paginas Libres:\n");
	i = 0;
	while (i < marcos) {
		if (bitMap[i] == 0) {
			printf("ID Frame: %d \n", i);
			log_info(logger, "Frame libre: %d\n", i);
		}
	}

}

void dumpContenidoDeMemoriaTodosLosProcesos() {
	int i = 0;
	int direccionFisica;
	int hastaDondeLeer;
	espacioAsignado * nodoActual;

	while (i < list_size(listaEspacioAsignado)) {
		nodoActual = list_get(listaEspacioAsignado, i);
		direccionFisica = nodoActual->IDPaginaInterno * marco_Size;
		hastaDondeLeer = (nodoActual->IDPaginaInterno + 1) * marco_Size;

		printf("Frame: %d\nPID: %d\nPagina: %d\nContenido:\n",
				nodoActual->IDPaginaInterno, nodoActual->pid, nodoActual,
				nodoActual->numDePag);

		log_info(logger, "Frame: %d\nPID: %d\nPagina: %d\nContenido: \n",
				nodoActual->IDPaginaInterno, nodoActual->pid, nodoActual,
				nodoActual->numDePag);

		while (direccionFisica < hastaDondeLeer) {
			printf("%c", memoriaReal[direccionFisica]);
			log_info(logger, "%c", memoriaReal[direccionFisica]);
			direccionFisica++;
		}
		printf("\n");
		log_info(logger, "\n");

		i++;
	}
}

void dumpContenidoDeMemoriaProcesoEnParticular(int PID) {
	int i = 0;
	int direccionFisica;
	int hastaDondeLeer;
	espacioAsignado * nodoActual;

	while (i < list_size(listaEspacioAsignado)) {
		nodoActual = list_get(listaEspacioAsignado, i);
		if (nodoActual->pid == PID) {
			direccionFisica = nodoActual->IDPaginaInterno * marco_Size;
			hastaDondeLeer = (nodoActual->IDPaginaInterno + 1) * marco_Size;

			printf("Frame: %d\nPID: %d\nPagina: %d\nContenido:\n",
					nodoActual->IDPaginaInterno, nodoActual->pid,
					nodoActual->numDePag);

			log_info(logger, "Frame: %d\nPID: %d\nPagina: %d\nContenido: \n",
					nodoActual->IDPaginaInterno, nodoActual->pid,
					nodoActual->numDePag);

			while (direccionFisica < hastaDondeLeer) {
				printf("%c", memoriaReal[direccionFisica]);
				log_info(logger, "%c", memoriaReal[direccionFisica]);
				direccionFisica++;
			}
			printf("\n");
			log_info(logger, "\n");
		}
		i++;
	}
}

void flushTLB() {
	if (entradas_TLB == 0) {
		log_info(logger, "TLB Deshabilitado");
	} else {
		int i = 0;
		while (i < TLB->elements_count) {
			elDestructorDeNodosTLB(i);
			bitMapTLB[i] = FALSE; //Vacio toodo el bitMap de la tlb
			i++;
		}
		log_info(logger, "TLB acaba de vaciarse");
	}
}

void flushMemory() {
	int i = 0;
	espacioAsignado*nodoAModificar;
	while (i < listaEspacioAsignado->elements_count) {
		nodoAModificar = list_get(listaEspacioAsignado, i);
		nodoAModificar->bitModificado = 1;
		i++;
	}
}

void menuUMC(pthread_t hiloComandos, pthread_attr_t attrhiloComandos) {

	pthread_attr_init(&attrhiloComandos);

	pthread_attr_setdetachstate(&attrhiloComandos, PTHREAD_CREATE_DETACHED);
	int hiloParaComandos = pthread_create(&hiloComandos, &attrhiloComandos,
			(void *) comandosUMC, NULL);

	pthread_attr_destroy(&attrhiloComandos);
}

void inicioTLB() {
	int habilitada = tlbHabilitada();
	if (habilitada == 0) {
		log_info(logger, "TLB Deshabilitada");
	} else {
		log_info(logger, "TLB Habilitada con %d entradas", entradas_TLB);

		TLB = creoTLB();
		bitMapTLB = malloc(sizeof(bool) * entradas_TLB);
		iniciarTLB();
		memoriaTLB = calloc(1, marco_Size * entradas_TLB);
		//Inicializo la cantidad de aciertos y accesos
		aciertosTLB = 0;
		accesosTLB = 0;
	}
}

void iniciarTLB() {
	int i;
	for (i = 0; i < entradas_TLB; i++) {
		bitMapTLB[i] = FALSE;
	}
}

t_list * creoTLB() {
	t_list * TLB = list_create();
	return TLB;
}

void elDestructorDeNodosTLB(int i) {
	list_remove(TLB, i);
}

void elDestructorDeNodosMemoria(int i) {
	list_remove(listaEspacioAsignado, i);
}

bool escribirEnTLB(int pid, int pagina, int offset, int cantidad, char*buffer) {
	int i = 0;
	t_tlb*nodoActual = list_get(TLB, i);
	while (i < list_size(TLB)) {

		if ((nodoActual->pid) == pid && nodoActual->pagina == pagina)
			break;
		i++;
	}
	if (i >= list_size(TLB))
		return 0;
	else {
		int lugarActual = nodoActual->frameTLB * marco_Size + offset;
		for (i = 0; i < cantidad; i++) {
			memoriaTLB[lugarActual] = buffer[i];
			i++;
			lugarActual++;
		}
		return 1;
	}

}

void llevarPaginaATLB(int PID, int pagina, char* buffer) {
	if (tlbHabilitada()) {
		int TLBLlena = tlbLlena();
		if (TLBLlena) {
			int paginaAEscribir = reemplazarPaginaLRU();
			memoriaTLB[paginaAEscribir] = 1;
			t_tlb* tlb = malloc(sizeof(t_tlb));
			tlb->pid = PID;
			tlb->pagina = pagina;
			tlb->momentoEntrada = accesosTLB;
			tlb->frameTLB = paginaAEscribir;
			insertarNodoOrdenadoEnTLB(tlb);
			int contador = 0;
			int posicionEnTLB = marco_Size * paginaAEscribir;
			while (contador < marco_Size) {
				memoriaTLB[posicionEnTLB] = buffer[contador];
				contador++;
			}
		} else {
			t_tlb* tlb = malloc(sizeof(t_tlb));
			tlb->pid = PID;
			tlb->pagina = pagina;
			tlb->momentoEntrada = accesosTLB;
			int paginaVacia = buscarPaginaVaciaEnTLB();
			bitMapTLB[paginaVacia] = 1;
			tlb->frameTLB = paginaVacia;
			insertarNodoOrdenadoEnTLB(tlb);
			int contador = 0;
			int posicionEnTLB = marco_Size * paginaVacia;
			while (contador < marco_Size) {
				memoriaTLB[posicionEnTLB] = buffer[contador];
				contador++;
			}

		}
	}

}

//detecta Que pagina debe reemplazar y la lleva a memoria si debe
int reemplazarPaginaLRU() {
	int paginaLibre;
	int contadorPrimerMomento;
	t_tlb*paginaAComparar;
	t_tlb*paginaAMatar;
	char* buffer = malloc(sizeof(char) * marco_Size);
	paginaAMatar = list_get(TLB, contadorPrimerMomento);
	int lugarDePaginaAMatar = 0;
	for (contadorPrimerMomento = 0; contadorPrimerMomento < list_size(TLB);
			contadorPrimerMomento++) {
		paginaAComparar = list_get(TLB, contadorPrimerMomento);
		if (paginaAComparar->momentoEntrada < paginaAMatar->momentoEntrada) {
			paginaAMatar = paginaAComparar;
			lugarDePaginaAMatar = contadorPrimerMomento;
		}
	}
	int comienzoDePagina = ((paginaAMatar->frameTLB) * marco_Size);
	int i = 0;
	while (comienzoDePagina
			< ((paginaAMatar->frameTLB) * marco_Size) + marco_Size) {
		buffer[i] = memoriaTLB[comienzoDePagina];
		i++;
		comienzoDePagina++;
	}

	almacenarBytes(paginaAMatar->pid, paginaAMatar->pagina, 0, marco_Size,
			buffer);
	paginaLibre = paginaAMatar->frameTLB;
	memoriaTLB[paginaLibre] = 0;
	free(list_remove(TLB, lugarDePaginaAMatar));
	return paginaLibre;
}

//nodo ordenado por PID
void insertarNodoOrdenadoEnTLB(t_tlb*unNodo) {
	int contador = 0;
	t_tlb*nodoActual = list_get(TLB, contador);
	while (nodoActual->pid < unNodo->pid && contador < list_size(TLB)) {
		contador++;
		nodoActual = list_get(TLB, contador);
	}
	list_add_in_index(TLB, contador, unNodo);
}

int buscarPaginaVaciaEnTLB() {
	int i = 0;
	while (bitMapTLB[i] != 0) {
		i++;
	}
	return i;
}

char* leerEnTLB(int PID, int pagina, int posicion, int tamanio) {

	int habilitada = tlbHabilitada();
	char* buffer;
	if (habilitada != 0) {
		t_tlb * entradaTLB = buscarEnTLB(PID, pagina);
		if (entradaTLB != NULL) {
			log_info(logger, "Acierto de TLB en el frame %d y pagina %d",
					entradaTLB->frameTLB, entradaTLB->pagina);
			buffer = malloc(sizeof(char) * tamanio);
			int i;
			int lugarEnTLB = entradaTLB->frameTLB * marco_Size + posicion;
			for (i = 0; i < tamanio; i++) {
				buffer[i] = memoriaTLB[lugarEnTLB];
				i++;
				lugarEnTLB++;
			}
			return buffer;
			//Creo que no hay ningun retardo pero si lo hay lo pongo en esta linea
			//Aca le tengo que mandar al CPU o al kernel que el pedido de lectura esta hecho todo
		} else {
			return solicitarBytes(PID, pagina, posicion, tamanio);
		}
	}
}

t_tlb * buscarEnTLB(int PID, int pagina) {
	int i = 0;
	int sizeTLB = list_size(TLB);
	while (i < sizeTLB) {
		t_tlb * entradaTLB = list_get(TLB, i);
		if (entradaTLB->pid == PID) {
			if (entradaTLB->pagina == pagina) {
				return entradaTLB; //La encontro
			}
		}
		i++;
	}
	return NULL;
}

//Devuelve 1 si esta llena, devuelve 0 si no esta llena
int tlbLlena() {
	if (TLB->elements_count == entradas_TLB) {
		return 1;
	}

	return 0;
}

//DEVUeLVE 1 SI ESTA HABILITADA, SI NO ESTA HBILITADA DEVUELVE 0
int tlbHabilitada() {
	if (entradas_TLB == 0) {
		return 0;
	}
	return 1;
}
