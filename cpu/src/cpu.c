/*
 * cpu.c
 *
 *  Created on: 23/4/2016
 *      Author: utnso
 */

/*******************************************************
 * DEPENDENCIAS
 *******************************************************/
#include "cpu.h"
#include <libreriasCompartidas/pointerStream.h>
#include <commons/collections/dictionary.h>
#define manejarError(msg) {perror(msg); abort();}
//======================================================

/*******************************************************
 * CONSTANTES
 *******************************************************/

//======================================================
//Creo structs necesarias para el parser BY DRMENGUECHE
void setearValores(t_config * archivoConfig) {
	ipUMC = config_get_string_value(archivoConfig, "IP_UMC");
	umcPort = config_get_string_value(archivoConfig, "PUERTO_UMC");
	ipNucleo = config_get_string_value(archivoConfig, "IP_NUCLEO");
	nucleoPort = config_get_string_value(archivoConfig, "PUERTO_NUCLEO");
	puts("hola");
}

void cuerpoDelCpu() {

}

/*******************************************************
 * FUNCIONES AnSISOP
 *******************************************************/

/*
 * definirVariable
 */
t_puntero definirVariable(t_nombre_variable identificador_variable) {
	puts("CPU: Pidio DEFINIR VARIABLE");
	int pagina;
	paginaDeStack*nuevaVariable = crearPaginaDeStackVaciaPiola();
	StrCpuUmc*streamCpuUmc;
	SocketBuffer*buffer;
	StrUmcCpu*streamUmcCpu;
	int tam = 100;
	variables*variable = malloc(sizeof(variables));
	if (1 == list_size(pcbProceso->indiceDelStack))
		tam = list_size(pcbProceso->indiceDelStack);
	if (0 == list_size(pcbProceso->indiceDelStack)
			|| (tam == 0 && 1 == list_size(pcbProceso->indiceDelStack))) {
		pagina = (pcbProceso->cantPagCod);
		asignadoVacio->numDePag = (pcbProceso->cantPagCod);
		asignadoVacio->bitUso = 4;
		nuevaVariable->pos = 0;
		streamCpuUmc = newStrCpuUmc(CPU_ID, SOLICITAR_BYTES, *asignadoVacio, 0,
				0, NULL, 0);

		buffer = serializeCpuUmc(streamCpuUmc);

		if (!socketSend(socketUMC->ptrSocket, buffer)) {
			log_error(getLogger(), "No se pudo realizar DEFINIR VARIABLE.");
			return FALSE;
		}

		if ((buffer = socketReceive(socketUMC->ptrSocket)) == NULL) {
			log_error(getLogger(), "No se pudo recibir el stream de la UMC.");
			return FALSE;
		}

		streamUmcCpu = unserializeCpuUmc(buffer);

		if (streamUmcCpu->action == ABORTAR_PROGRAMA) {
			seguirEjecutando = FALSE;
		} else {
			if (asignadoVacio->numDePag != stackSize + pcbProceso->cantPagCod) {
				if (0 == list_size(pcbProceso->indiceDelStack)) {
					nuevaVariable->pos = 0;
					variable->idVar = identificador_variable;
					variable->pagVar = asignadoVacio->numDePag;
					variable->offVar = 0;
					variable->sizeVar = 4;
					list_add(nuevaVariable->vars, variable);
					list_add((pcbProceso->indiceDelStack), nuevaVariable);
				} else {
					nuevaVariable = list_get(pcbProceso->indiceDelStack, 0);
					variable->idVar = identificador_variable;
					variable->pagVar = asignadoVacio->numDePag;
					variable->offVar = 0;
					variable->sizeVar = 4;
					list_add(nuevaVariable->vars, variable);
				}

			} else
				seguirEjecutando = FALSE;
		}
	} else {
		paginaDeStack*ultimaPaginaStack = list_get(pcbProceso->indiceDelStack,
				list_size(pcbProceso->indiceDelStack) - 1);
		if (list_size(ultimaPaginaStack->vars) == 0)
			ultimaPaginaStack = list_get(pcbProceso->indiceDelStack,
					list_size(pcbProceso->indiceDelStack) - 2);
		variables*ultimaPagina = list_get(ultimaPaginaStack->vars,
				list_size(ultimaPaginaStack->vars) - 1);
		if (ultimaPagina->offVar + 4 == tamanioPaginaUmc) {
			asignadoVacio->numDePag = ultimaPagina->pagVar + 1;
			asignadoVacio->bitUso = 4;
			streamCpuUmc = newStrCpuUmc(CPU_ID, SOLICITAR_BYTES, *asignadoVacio,
					0, 0, 0, 0);

			buffer = serializeCpuUmc(streamCpuUmc);

			if (!socketSend(socketUMC->ptrSocket, buffer)) {
				log_error(getLogger(), "No se pudo realizar DEFINIR VARIABLE.");
				return FALSE;
			}

			if ((buffer = socketReceive(socketUMC->ptrSocket)) == NULL) {
				log_error(getLogger(),
						"No se pudo recibir el stream de la UMC.");
				return FALSE;
			}

			streamUmcCpu = unserializeCpuUmc(buffer);

			if (streamUmcCpu->action == ABORTAR_PROGRAMA) {
				seguirEjecutando = FALSE;
			} else {
				if (asignadoVacio->numDePag
						!= stackSize + pcbProceso->cantPagCod) {
					ultimaPaginaStack = list_get(pcbProceso->indiceDelStack,
							list_size(pcbProceso->indiceDelStack) - 1);
					variable->idVar = identificador_variable;
					variable->pagVar = asignadoVacio->numDePag;
					variable->offVar = 0;
					variable->sizeVar = 4;
					list_add(ultimaPaginaStack->vars, variable);
				} else
					seguirEjecutando = FALSE;
			}
		} else {
			asignadoVacio->numDePag = ultimaPagina->pagVar;
			asignadoVacio->bitUso = 4;

			streamCpuUmc = newStrCpuUmc(CPU_ID, SOLICITAR_BYTES, *asignadoVacio,
					ultimaPagina->offVar + 4, 0, 0, 0);
			buffer = serializeCpuUmc(streamCpuUmc);

			if (!socketSend(socketUMC->ptrSocket, buffer)) {
				log_error(getLogger(), "No se pudo realizar DEFINIR VARIABLE.");
				return FALSE;
			}

			if ((buffer = socketReceive(socketUMC->ptrSocket)) == NULL) {
				log_error(getLogger(),
						"No se pudo recibir el stream de la UMC.");
				return FALSE;
			}

			streamUmcCpu = unserializeCpuUmc(buffer);

			if (streamUmcCpu->action == ABORTAR_PROGRAMA) {
				seguirEjecutando = FALSE;
			} else {
				if (asignadoVacio->numDePag
						!= stackSize + pcbProceso->cantPagCod) {
					ultimaPaginaStack = list_get(pcbProceso->indiceDelStack,
							list_size(pcbProceso->indiceDelStack) - 1);
					variable->idVar = identificador_variable;
					variable->pagVar = asignadoVacio->numDePag;
					variable->sizeVar = 4;
					variable->offVar = ultimaPagina->offVar + 4;
					list_add(ultimaPaginaStack->vars, variable);
				} else {
					seguirEjecutando = FALSE;
				}
			}
		}
	}
	return variable->pagVar * tamanioPaginaUmc + variable->offVar;
}

// devuelve 1 si la página está vacía
bool espacioMemoriaVacio(int tamanio, char*bytes) {
	int counter = 0;
	while (counter < tamanio) {
		if (bytes[counter] != '\0')
			break;
		counter++;
	}
	if (counter == tamanio)
		return TRUE;
	else
		return FALSE;
}

/*
 * obtenerPosicionVariable
 */ //NO TOCAR YA ESTÁ TERMINADA
t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable) {
	puts("CPU: Pidio OBTENER POSICION VARIABLE");
	if (list_size(pcbProceso->indiceDelStack) != 0) {
		int i;
		paginaDeStack* aux = list_get(pcbProceso->indiceDelStack,
				list_size(pcbProceso->indiceDelStack) - 1);
		variables*varNew;
		for (i = 0; i < list_size(aux->vars); ++i) {
			varNew = list_get(aux->vars, i);
			if (varNew->idVar == identificador_variable) {
				return varNew->pagVar * tamanioPaginaUmc + varNew->offVar;
			}
		}
	} else {
		return -1;
	}
	return -1;
}

/*
 * dereferenciar
 */
t_valor_variable dereferenciar(t_puntero direccion_variable) {
	puts("CPU: Pidio DEREFERENCIAR");
	asignadoVacio->numDePag = direccion_variable / tamanioPaginaUmc;
	asignadoVacio->bitUso = 4;
	int offset = direccion_variable % tamanioPaginaUmc;

	StrCpuUmc* streamCpuUMC;
	streamCpuUMC = newStrCpuUmc(CPU_ID, SOLICITAR_BYTES, *asignadoVacio, offset,
			0, NULL, 0);
	SocketBuffer* buffer = serializeCpuUmc(streamCpuUMC);

	if (!socketSend(socketUMC->ptrSocket, buffer)) {
		log_error(getLogger(), "No se pudo realizar DEREFERENCIAR.");
		return FALSE;
	}

	if ((buffer = socketReceive(socketUMC->ptrSocket)) == NULL) {
		log_error(getLogger(), "No se pudo recibir el stream de la UMC.");
		return FALSE;
	}

	StrUmcCpu*streamUmcCpu;

	streamUmcCpu = unserializeCpuUmc(buffer);

	if (streamUmcCpu->action == ABORTAR_PROGRAMA) {
		seguirEjecutando = FALSE;
		return 0;
	} else
		return atoi(streamUmcCpu->data);

}

/*
 * asignar
 */
void asignar(t_puntero direccion_variable, t_valor_variable valor) {
	puts("CPU: Pidio ASIGNAR");
	asignadoVacio->numDePag = direccion_variable / tamanioPaginaUmc;
	asignadoVacio->bitUso = direccion_variable % tamanioPaginaUmc;
	int offset = direccion_variable % tamanioPaginaUmc;

	Byte* dataAMandar = malloc(sizeof(char) * 4);
	sprintf(dataAMandar, "%d", valor);

	StrCpuUmc* streamCpuUMC;
	streamCpuUMC = newStrCpuUmc(CPU_ID, ALMACENAR_BYTES, *asignadoVacio, offset,
			4, dataAMandar, 0);
	SocketBuffer* buffer = serializeCpuUmc(streamCpuUMC);

	if (!socketSend(socketUMC->ptrSocket, buffer)) {
		log_error(getLogger(), "No se pudo realizar ASIGNAR.");
	}

	if ((buffer = socketReceive(socketUMC->ptrSocket)) == NULL) {
		log_error(getLogger(), "No se pudo recibir el stream de la UMC.");
	}

	free(dataAMandar);
	StrUmcCpu*streamUmcCpu;
	streamUmcCpu = unserializeUmcCpu(buffer);
	if (streamUmcCpu->action == ABORTAR_PROGRAMA)
		seguirEjecutando = FALSE;
}

/*
 * obtenerValorCompartida
 */
t_valor_variable obtenerValorCompartida(t_nombre_compartida variable) {
	printf("CPU: Pidio OBTENER VALOR COMPARTIDA %s\n", variable);
	char* variableMod;
	variableMod = sinEspacioAlFinal(variable, strlen(variable));

	StrCpuKer*streamCpuKer;
	streamCpuKer = newStrCpuKer(CPU_ID, OBTENER_VALOR_COMPARTIDA, *pcbProceso,
			pcbProceso->id, strlen(variableMod), variableMod,
			NULL /*NOMBRE DISPOSITIVO*/, 0 /*LEN NOMBRE DISPOSITIVO*/);
	SocketBuffer*buffer = serializeCpuKer(streamCpuKer);

	if (!socketSend(socketNucleo->ptrSocket, buffer)) {
		log_error(getLogger(), "No se pudo realizar OBTENER VALOR COMPARTIDA.");
		return FALSE;
	}

	if ((buffer = socketReceive(socketNucleo->ptrSocket)) == NULL) {
		log_error(getLogger(), "No se pudo recibir el stream del NUCLEO.");
		return FALSE;
	}

	free(variableMod);

	StrKerCpu*streamKerCpu = unserializeKerCpu(buffer);
	if (streamKerCpu->action == OBTENER_VALOR_COMPARTIDA)
		return (streamKerCpu->quantum);
	else
		seguirEjecutando = FALSE;
	return -1;

}

/*
 * 	asignarValorCompartida
 */
t_valor_variable asignarValorCompartida(t_nombre_compartida variable,
		t_valor_variable valor) {
	printf("CPU: Pidio ASIGNAR VALOR %d COMPARTIDA %s\n", valor, variable);
	char* variableMod;
	variableMod = sinEspacioAlFinal(variable, strlen(variable));

	char* buffer;
	StrCpuKer*streamCpuKer;
	streamCpuKer = newStrCpuKer(CPU_ID, ASIGNAR_VALOR_COMPARTIDA, *pcbProceso,
			valor, 0 /*LOGLEN*/,
			NULL /*LOG*/, variableMod /*NOMBRE DISPOSITIVO*/,
			strlen(variableMod) /*LEN NOMBRE DISPOSITIVO*/);
	buffer = serializeCpuKer(streamCpuKer);

	if (!socketSend(socketNucleo->ptrSocket, buffer)) {
		log_error(getLogger(), "No se pudo realizar ASIGNAR VALOR COMPARTIDA.");
		return FALSE;
	}

	if ((buffer = socketReceive(socketNucleo->ptrSocket)) == NULL) {
		log_error(getLogger(), "No se pudo recibir el stream del NUCLEO.");
		return FALSE;
	}
	StrKerCpu*streamKerCpu = unserializeKerCpu(buffer);
	free(variableMod);
	if (streamKerCpu->action == ASIGNAR_VALOR_COMPARTIDA)
		return streamKerCpu->quantum;
	else {
		seguirEjecutando = FALSE;
		return -1;
	}
}

/*
 * 	irAlLabel
 */
void irAlLabel(t_nombre_etiqueta etiqueta) {

	puts("CPU: Pidio IR A LABEL");

	etiquetaMod = sinEspacioAlFinal(etiqueta, strlen(etiqueta));

	pcbProceso->programCounter = metadata_buscar_etiqueta(etiquetaMod,
			pcbProceso->indiceDeEtiquetas, pcbProceso->etiquetaSize);

	free(etiquetaMod);

	saltoDeLinea = TRUE;
}

char* sinEspacioAlFinal(char* linea, int tamanio) {

	char* lineaLoca = (char*) malloc(100);
	memcpy(lineaLoca, linea, tamanio);
	lineaLoca[tamanio] = '\0';
	int i;
	for (i = tamanio; i > 0; i--) {
		if (isspace(lineaLoca[i])) {
			lineaLoca[i] = '\0';
		}
	}
	return lineaLoca;
}

/*
 * llamarConRetorno
 */
void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar) {
	puts("CPU: Pidio LLAMAR CON RETORNO");

	char* etiquetaMod;
	etiquetaMod = sinEspacioAlFinal(etiqueta, strlen(etiqueta));

	paginaDeStack* aux = crearPaginaDeStackVaciaPiola();
	variablesRetorno*dondeRetorno = malloc(sizeof(variablesRetorno));
	dondeRetorno->pagVarRet = donde_retornar / tamanioPaginaUmc;
	dondeRetorno->offVarRet = donde_retornar % tamanioPaginaUmc;
	dondeRetorno->idVarRet = '\0';
	dondeRetorno->sizeVarRet = 1;
	aux->retPos = pcbProceso->programCounter;
	aux->pos = list_size(pcbProceso->indiceDelStack);
	list_add(aux->retVars, dondeRetorno);
	list_add(pcbProceso->indiceDelStack, aux);

	pcbProceso->programCounter = metadata_buscar_etiqueta(etiquetaMod,
			pcbProceso->indiceDeEtiquetas, pcbProceso->etiquetaSize);

	free(etiquetaMod);
	saltoDeLinea = TRUE;
}

/*
 * finalizar
 */
void finalizar(void) {
	puts("CPU: Pidio FINALIZAR");
	StrCpuKer*streamCpuKer;
	streamCpuKer = newStrCpuKer(CPU_ID, FINALIZAR_PROGRAMA, *pcbProceso,
			pcbProceso->id, 0, NULL, NULL /*NOMBRE DISPOSITIVO*/,
			0 /*LEN NOMBRE DISPOSITIVO*/);

	SocketBuffer*buffer = serializeCpuKer(streamCpuKer);

	if (!socketSend(socketNucleo->ptrSocket, buffer)) {
		log_error(getLogger(), "No se pudo realizar FINALIZAR.");
	}

	esperarConfirmacion(socketNucleo);

	finalizoCorrectamente = TRUE;
}

/*
 * retornar
 *
 */
void retornar(t_valor_variable retorno) {
	puts("CPU: Pidio RETORNAR");
	asignadoVacio = newEspacioAsignado();
	paginaDeStack *aux = list_remove(pcbProceso->indiceDelStack,
			(pcbProceso->indiceDelStack->elements_count) - 1);
	pcbProceso->programCounter = aux->retPos;

	variablesRetorno*dondeVuelvo = list_get(aux->retVars, 0);
	StrCpuUmc*streamCpuUmc;
	asignadoVacio->numDePag = dondeVuelvo->pagVarRet;
	streamCpuUmc = newStrCpuUmc(CPU_ID, ALMACENAR_BYTES, *asignadoVacio,
			dondeVuelvo->offVarRet, 4, intToStr(retorno), 0);
	SocketBuffer*buffer = serializeCpuUmc(streamCpuUmc);

	if (!socketSend(socketUMC->ptrSocket, buffer)) {
		log_error(getLogger(), "No se pudo realizar RETORNAR.");
	}

	if ((buffer = socketReceive(socketUMC->ptrSocket)) == NULL) {
		log_error(getLogger(), "No se pudo recibir el stream de la UMC.");
	}

	StrUmcCpu*streamUmcCpu;
	streamUmcCpu = unserializeCpuUmc(buffer);
	if (streamUmcCpu->action == ABORTAR_PROGRAMA)
		seguirEjecutando = FALSE;
}

String intToStr(Int32U integer) {
	String result = malloc(sizeof(Byte) * 10);
	sprintf(result, "%d", integer);
	return result;
}

/*
 * imprimir
 */
void imprimir(t_valor_variable valor_mostrar) {
	puts("CPU: Pidio IMPRIMIR");
	StrCpuKer*streamCpuKer;
	String aux;
	aux = intToStr(valor_mostrar);
	streamCpuKer = newStrCpuKer(CPU_ID, IMPRIMIR, *pcbProceso, valor_mostrar,
			strlen(aux), aux, NULL /*NOMBRE DISPOSITIVO*/,
			0 /*LEN NOMBRE DISPOSITIVO*/);
	SocketBuffer*buffer = serializeCpuKer(streamCpuKer);

	if (!socketSend(socketNucleo->ptrSocket, buffer)) {
		log_error(getLogger(), "No se pudo realizar IMPRIMIR.");
	}

	esperarConfirmacion(socketNucleo);
	free(aux);
}

/*
 * imprimirTexto
 */
int imprimirTexto(char* texto) {
	puts("CPU: Pidio IMPRIMIR TEXTO");
	StrCpuKer*streamCpuKer;
	streamCpuKer = newStrCpuKer(CPU_ID, IMPRIMIRTEXTO, *pcbProceso,
			pcbProceso->id, strlen(texto), texto, NULL /*NOMBRE DISPOSITIVO*/,
			0 /*LEN NOMBRE DISPOSITIVO*/);
	SocketBuffer*buffer = serializeCpuKer(streamCpuKer);

	if (!socketSend(socketNucleo->ptrSocket, buffer)) {
		log_error(getLogger(), "No se pudo realizar IMPRIMIR TEXTO.");
		return FALSE;
	}
	esperarConfirmacion(socketNucleo);
	return strlen(texto);
}

/*
 * entradaSalida
 */
void entradaSalida(t_nombre_dispositivo dispositivo, int tiempo) {
	puts("CPU: Pidio IO");

	char* identificadorMod;
	identificadorMod = sinEspacioAlFinal(dispositivo, strlen(dispositivo));

	String auxTiempo;
	auxTiempo = intToStr(tiempo);
	StrCpuKer*streamCpuKer;
	moverProgramCounterPcb(pcbProceso);
	streamCpuKer = newStrCpuKer(CPU_ID, ENTRADA_SALIDA, *pcbProceso,
			pcbProceso->id, strlen(auxTiempo), auxTiempo,
			identificadorMod /*NOMBRE DISPOSITIVO*/,
			strlen(identificadorMod) /*LEN NOMBRE DISPOSITIVO*/);
	SocketBuffer*buffer = serializeCpuKer(streamCpuKer);

	if (!socketSend(socketNucleo->ptrSocket, buffer)) {
		log_error(getLogger(), "No se pudo realizar ENTRADA SALIDA.");
	} else {
		esperarConfirmacion(socketNucleo);
	}

	free(identificadorMod);
	free(auxTiempo);

	devolverPCB = TRUE;
}

/*
 * wait
 */
void wait(t_nombre_semaforo identificador_semaforo) {
	printf("CPU: Pidio WAIT semaforo %s", identificador_semaforo);

	char* identificadorMod;
	identificadorMod = sinEspacioAlFinal(identificador_semaforo,
			strlen(identificador_semaforo));
	moverProgramCounterPcb(pcbProceso);
	saltoDeLinea = TRUE;
	StrCpuKer*streamCpuKer;
	streamCpuKer = newStrCpuKer(CPU_ID, WAIT_SEM_ANSISOP, *pcbProceso,
			pcbProceso->id, strlen(identificadorMod), identificadorMod,
			NULL /*NOMBRE DISPOSITIVO*/, 0 /*LEN NOMBRE DISPOSITIVO*/);
	SocketBuffer*buffer = serializeCpuKer(streamCpuKer);

	if (!socketSend(socketNucleo->ptrSocket, buffer)) {
		log_error(getLogger(), "No se pudo enviar el WAIT.");
	}

	if ((buffer = socketReceive(socketNucleo->ptrSocket)) == NULL) {
		log_error(getLogger(), "No se pudo recibir el stream del Nucleo.");
	}

	StrKerCpu*StreamKerCpu = unserializeKerCpu(buffer);
	if (StreamKerCpu->action == WAIT_REALIZADO) {
		seguirEjecutando = TRUE;
	}
	if ((StreamKerCpu->action == PROGRAMA_BLOQUEADO)) {
		devolverPCB = TRUE;
	}
	free(identificadorMod);
}

/*
 * signal
 */
void signale(t_nombre_semaforo identificador_semaforo) {
	printf("CPU: Pidio SIGNAL semaforo %s", identificador_semaforo);

	char* identificadorMod;
	identificadorMod = sinEspacioAlFinal(identificador_semaforo,
			strlen(identificador_semaforo));

	StrCpuKer*streamCpuKer;
	streamCpuKer = newStrCpuKer(CPU_ID, 28 /*SIGNAL*/, *pcbProceso,
			pcbProceso->id, strlen(identificadorMod), identificadorMod,
			NULL /*NOMBRE DISPOSITIVO*/, 0 /*LEN NOMBRE DISPOSITIVO*/);
	SocketBuffer*buffer = serializeCpuKer(streamCpuKer);

	if (!socketSend(socketNucleo->ptrSocket, buffer)) {
		log_error(getLogger(), "No se pudo realizar SIGNAL.");
	}

	esperarConfirmacion(socketNucleo);

	free(identificadorMod);
}

char * pedirCodigoAUMC() {

	char*lineaDeCodigoADevolver =
			malloc(
					sizeof(char)
							* pcbProceso->indiceDeCodigo[pcbProceso->programCounter].longitud);
	int comienzo =
			pcbProceso->indiceDeCodigo[pcbProceso->programCounter].comienzo;

	int paginaDeComienzo = comienzo / tamanioPaginaUmc;

	int desplazamiento = comienzo
			+ pcbProceso->indiceDeCodigo[pcbProceso->programCounter].longitud;

	int paginaDeFin = desplazamiento / tamanioPaginaUmc;

	int dondeEmpiezo = comienzo - tamanioPaginaUmc * paginaDeComienzo;

	int longitudTotalAPedir =
			pcbProceso->indiceDeCodigo[pcbProceso->programCounter].longitud;

	if (paginaDeComienzo == paginaDeFin) {
	} else {
		int cantPaginas = paginaDeFin - paginaDeComienzo;
		int i;
		int tamanioAPedir;
		int paginaActual = paginaDeComienzo;
		for (i = 0; i <= cantPaginas; i++) {

			tamanioAPedir = tamanioPaginaUmc - dondeEmpiezo;
			if (tamanioAPedir > longitudTotalAPedir) {
				break;
			} else {
				longitudTotalAPedir = longitudTotalAPedir - tamanioAPedir;
				dondeEmpiezo = 0;
			}
			paginaActual++;
		}
	}
	return lineaDeCodigoADevolver;
}

AnSISOP_funciones funciones = { .AnSISOP_definirVariable = definirVariable,
		.AnSISOP_obtenerPosicionVariable = obtenerPosicionVariable,
		.AnSISOP_dereferenciar = dereferenciar, .AnSISOP_asignar = asignar,
		.AnSISOP_obtenerValorCompartida = obtenerValorCompartida,
		.AnSISOP_asignarValorCompartida = asignarValorCompartida,
		.AnSISOP_irAlLabel = irAlLabel, .AnSISOP_llamarConRetorno =
				llamarConRetorno, .AnSISOP_retornar = retornar,
		.AnSISOP_finalizar = finalizar, .AnSISOP_entradaSalida = entradaSalida,
		.AnSISOP_imprimir = imprimir, .AnSISOP_imprimirTexto = imprimirTexto, };

AnSISOP_kernel funcionesDeKernel = { .AnSISOP_wait = wait, .AnSISOP_signal =
		signale, };

void abortarCPU() {
//	if (seguirEjecutando) {
//		seguirEjecutando = FALSE;
	log_info(getLogger(), "Recibi SIGUSR1");
	elAbortador = TRUE;
	//}
}

// Devuelve el logger para loggear los logs para poder loggear
t_log* getLogger() {
	if (logger == NULL) {
		logger = log_create(LOGGER_FILE, "CPU", TRUE, LOG_LEVEL_INFO);
	}
	return logger;
}

void gestionoSIGINT() {
	StrCpuKer* streamck = newStrCpuKer(CPU_ID, ABORTAR_PROGRAMA, *pcbProceso, 0,
			0,
			NULL, NULL, 0);
	SocketBuffer* bufferr = serializeCpuKer(streamck);

	if (!socketSend(socketNucleo->ptrSocket, bufferr)) {
		log_info(getLogger(), "No se pudo enviar el buffer al nucleo.");
	}
	log_info(getLogger(), "Aborte el programa por SIGINT");
	int pid = getpid();
	kill(pid, SIGTERM);
}

bool esperarConfirmacion(SocketClient* socket) {
	SocketBuffer* sb;
	StrKerUmc* in_ker_msg;
	StrUmcCpu* in_umc_msg;

	if ((sb = socketReceive(socket->ptrSocket)) == NULL) {
		log_error(getLogger(),
				"No se pudo recibir el stream del socket que me envia.");
		return FALSE;
	}

	if (sb == NULL) {
		return FALSE;
	}

	Stream strReceived = (Stream) sb->data;
	Char id = getStreamId(strReceived);

	switch (id) {
	case UMC_ID:
		in_umc_msg = unserializeUmcCpu((Stream) sb->data);
		if (in_umc_msg->action == TODO_PIOLA) {
			return TRUE;
		}
		break;
	case KERNEL_ID:
		in_ker_msg = unserializeKerCpu((Stream) sb->data);
		if (in_ker_msg->action == TODO_PIOLA) {
			return TRUE;
		}
		break;
	default:
		return FALSE;
		break;
	}
}
