/*
 * cpu.h
 *
 *  Created on: 23/4/2016
 *      Author: utnso
 */

#ifndef CPU_H_
#define CPU_H_

/*******************************************************
 * DEPENDENCIAS
 *******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/config.h>
#include <parser/parser.h>
#include <commons/log.h>
#include <pthread.h>
#include <libreriasCompartidas/archivosYLogsYMas.h>
#include <libreriasCompartidas/socket.h>
#include <commons/collections/list.h>
#define FAIL -1
//======================================================

/*******************************************************
 * VARIABLES GLOBALES
 *******************************************************/
char* ipUMC;
char* umcPort;
char* nucleoPort;
char* ipNucleo;
int clienteNucleo;
t_list* coreList;
//======================================================

/*******************************************************
 * TIPOS DE DATOS
 *******************************************************/
typedef void* Parameter;

typedef struct function {
	char* name;
	Parameter param[1];
} Function;

typedef struct functionParameters {
	char param[1];
	void (*func)(void*, void*);
} FunctionParameters;

typedef struct core {
	bool online;
	bool busy;
	int socket;
} t_core;
//======================================================

/*******************************************************
 * FUNCIONES AnSISOP
 *******************************************************/

/*
 * definirVariable
 */
t_puntero definirVariable(t_nombre_variable identificador_variable);

/*
 * obtenerPosicionVariable
 */
t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable);

/*
 * dereferenciar
 */
t_valor_variable dereferenciar(t_puntero direccion_variable);

/*
 * asignar
 */
void asignar(t_puntero direccion_variable, t_valor_variable valor);

/*
 * obtenerValorCompartida
 */
t_valor_variable obtenerValorCompartida(t_nombre_compartida variable);

/*
 * 	asignarValorCompartida
 */
t_valor_variable asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor);

/*
 * 	irAlLabel
 */
t_puntero_instruccion irAlLabel(t_nombre_etiqueta etiqueta);

/*
 * llamarConRetorno
 */
void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar);

/*
 * retornar
 *
 */
t_puntero_instruccion retornar(t_valor_variable retorno);

/*
 * imprimir
 */
int imprimir(t_valor_variable valor_mostrar);

/*
 * imprimirTexto
 */
int imprimirTexto(char* texto);

/*
 * entradaSalida
 */
int entradaSalida(t_nombre_dispositivo dispositivo, int tiempo);

/*
 * wait
 */
int wait(t_nombre_semaforo identificador_semaforo);

/*
 * signal
 */
int signal(t_nombre_semaforo identificador_semaforo);

//======================================================
#endif /* CPU_H_ */
