/*
 * consola.h
 *
 *  Created on: 23/4/2016
 *      Author: utnso
 */

#ifndef CONSOLA_H_
#define CONSOLA_H_

/*
 * Dependencias
 */
#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include <libreriasCompartidas/archivosYLogsYMas.h>
#include "libreriasCompartidas/pointerTipos.h"
#include "libreriasCompartidas/pointerSocketes.h"
#include "libreriasCompartidas/pointerStream.h"
#include <commons/string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

/*
 * Defines
 */
#define PARAM_LENGTH 2
#define IP_NUCLEO "IP_NUCLEO"
#define PUERTO_NUCLEO "PUERTO_NUCLEO"
#define ARCHIVO_CONF "configconsola"
#define LOGGER_FILE "consola_log.txt"

FILE * fp;

/*
 * Prototipos de Funciones del consola.c
 */
int tamArchivo(char* direccionArchivo);
char* leerProgramaAnSISOP(char* direccionArchivo);
t_log* getLogger();
void funcionHiloConsola();

#endif /* CONSOLA_H_ */
