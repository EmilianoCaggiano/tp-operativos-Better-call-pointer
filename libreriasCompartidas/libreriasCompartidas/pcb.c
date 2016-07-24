/*
 * pcb.c
 *
 *  Created on: 9/6/2016
 *      Author: utnso
 */

#include "pcb.h"

void moverProgramCounterPcb(pcb * pcbLoca) {
	pcbLoca->programCounter++;
	pcbLoca->instruccionesRestantes--;

}

pcb* newPcb(Socket* consola, int id, int tamanioArchivoOriginal, t_puntero_instruccion programCounter, int paginasDeCodigo,
	arrayBidimensional* indiceDeCodigo, char* indiceDeEtiquetas, t_size indiceDeEtiquetasSize, t_size sizeEti, t_size insTotales,
	t_size insRestantes, t_list* indiceDelStack, estadoPrograma estado, t_size indiceDeCodigoSize, t_size cantElementsStack) {

		pcb* pcb = malloc(sizeof(pcb));

		pcb->consola = consola;
		pcb->id = id;
		pcb->tamanioArchivoOriginal = tamanioArchivoOriginal;
		pcb->programCounter = programCounter;
		pcb->paginasDeCodigo = paginasDeCodigo;
		pcb->indiceDeCodigo = indiceDeCodigo;
		pcb->etiquetaSize = sizeEti;
		pcb->instruccionesTotales = insTotales;
		pcb->instruccionesRestantes = insRestantes;
		pcb->indiceDeEtiquetas = indiceDeEtiquetas;
		pcb->indiceDeEtiquetasSize = indiceDeEtiquetasSize;
		pcb->indiceDelStack = indiceDelStack;
		pcb->estado = estado;
		pcb->indiceDeCodigoSize = indiceDeCodigoSize;
		pcb->cantElementsStack = cantElementsStack;
		return pcb;
}

paginaDeStack*crearPaginaDeStackVaciaPiola(){
	paginaDeStack* soyUnaPaginaDeStackVacia = malloc(sizeof (paginaDeStack));


	soyUnaPaginaDeStackVacia->pos=0;
	soyUnaPaginaDeStackVacia->retPos=0;
	soyUnaPaginaDeStackVacia->cantArgs=0;
	soyUnaPaginaDeStackVacia->cantVars=0;
	soyUnaPaginaDeStackVacia->cantRetvars=0;

	soyUnaPaginaDeStackVacia->vars =NULL;
	soyUnaPaginaDeStackVacia->retVars = NULL;
	soyUnaPaginaDeStackVacia->args = NULL;


	return soyUnaPaginaDeStackVacia;
}

pcb* newEmptyPcb() {

	pcb* pcbLoco = malloc(sizeof(pcb));
	//Socket * auxSocket = socketCreate();
	pcbLoco->consola = socketCreate();

	pcbLoco->id = 0;
	pcbLoco->tamanioArchivoOriginal = 0;
	pcbLoco->programCounter = 0;
	pcbLoco->paginasDeCodigo = 0;

	pcbLoco->indiceDeCodigoSize = (sizeof(arrayBidimensional));
	pcbLoco->indiceDeEtiquetas = malloc(sizeof(char) * 17);
	//char * auxIndice = malloc(sizeof(char)*17);
	char * auxIndice;
	auxIndice = "MUERTE A WINDOWS";
	memcpy(pcbLoco->indiceDeEtiquetas, auxIndice, 17);
	pcbLoco->indiceDeEtiquetasSize = 17;
	pcbLoco->etiquetaSize = 0;
	pcbLoco->instruccionesRestantes = 0;
	pcbLoco->instruccionesTotales = 1;
	pcbLoco->indiceDeCodigo = sizeof(arrayBidimensional)
			* pcbLoco->instruccionesTotales;
	pcbLoco->indiceDelStack = list_create();

	paginaDeStack * aux = crearPaginaDeStackVaciaPiola();


	list_add(pcbLoco->indiceDelStack, aux);

	paginaDeStack * aux2 = crearPaginaDeStackVaciaPiola();


	list_add(pcbLoco->indiceDelStack, aux2);

	pcbLoco->cantElementsStack = 2;

	pcbLoco->estado = NEW;

	arrayBidimensional * auxArray = (arrayBidimensional*) malloc(
			sizeof(arrayBidimensional));
	//arrayBidimensional auxArray;
	auxArray[0].comienzo = 0;
	auxArray[0].longitud = 0;
	pcbLoco->indiceDeCodigo = (arrayBidimensional*) malloc(
			sizeof(arrayBidimensional));
	memcpy(&(pcbLoco->indiceDeCodigo[0]), &auxArray[0],
			sizeof(arrayBidimensional));

	//free(auxArray);
	//free(auxSocket);
	//free(auxIndice);
	puts("Cree un pcb Vacio");

	return pcbLoco;
}

