/*
 * pcb.h
 *
 *  Created on: 6/6/2016
 *      Author: utnso
 */

#ifndef LIBRERIASCOMPARTIDAS_PCB_H_
#define LIBRERIASCOMPARTIDAS_PCB_H_
#include <parser/metadata_program.h>
#include <parser/parser.h>
#include <commons/collections/list.h>
#include "pointerSocketes.h"

typedef struct {
	int comienzo;
	int longitud;
} arrayBidimensional;

typedef struct {
	char idVar;
	int pagVar;
	int offVar;
	int sizeVar;
} variables;


typedef struct {
	char idVarRet;
	int pagVarRet;
	int offVarRet;
	int sizeVarRet;
} variablesRetorno;

typedef struct {
	int pagArg;
	int offArgs;
	int sizeArgs;
} argumentos;

typedef struct {
	int pos;
	int retPos;

	int cantArgs;
	int cantVars;
	int cantRetvars;
	t_list* args;
	t_list *vars;
	t_list* retVars;

} paginaDeStack;

typedef enum {
	NEW = 0, READY = 1, EXEC = 2, BLOCK = 3, EXIT = 4
} estadoPrograma;

typedef struct {
	Socket* consola;
	Socket * cpu;
	int id;
	int tamanioArchivoOriginal;
	int cantPagCod;
	t_puntero_instruccion programCounter;
	int paginasDeCodigo;
	arrayBidimensional * indiceDeCodigo;
	t_size indiceDeCodigoSize;
	char * indiceDeEtiquetas;
	t_size indiceDeEtiquetasSize;
	t_size etiquetaSize;
	t_size instruccionesTotales;
	t_size instruccionesRestantes;
	t_list* indiceDelStack;
	t_size cantElementsStack;
	estadoPrograma estado;
} pcb;

pcb* newPcb(Socket* consola,Socket* cpu, int id, int tamanioArchivoOriginal,int cantPagCod ,t_puntero_instruccion programCounter,
		int paginasDeCodigo, arrayBidimensional* indiceDeCodigo, char* indiceDeEtiquetas,t_size indiceDeEtiquetasSize,t_size etiquetas,t_size instruccionesTotales,t_size instruccionesRestantes,
		t_list* indiceDelStack, estadoPrograma estado, t_size indiceDeCodigoSize, t_size cantElementsStack);
pcb* newEmptyPcb();

char * pedirCodigoAUMC();

void moverProgramCounterPcb(pcb * pcbLoca);

#endif /* LIBRERIASCOMPARTIDAS_PCB_H_ */
