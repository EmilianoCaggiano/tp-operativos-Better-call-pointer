/*
 * nucleo.h
 *
 *  Created on: 23/4/2016
 *      Author: utnso
 */

#ifndef NUCLEO_H_
#define NUCLEO_H_


#include <stdio.h>
#include <stdlib.h>
#include <commons/temporal.h>
#include <commons/config.h>
#include <commons/config.h>
#include <commons/log.h>
#include <libreriasCompartidas/archivosYLogsYMas.h>
#include <libreriasCompartidas/socket.h>

#define FAIL -1

//variables


   char*  puertoPropio;
   char*  cpuPort;
   int  quantum;
   int  quantumSleep;
   char**   idSemaforos;
   char**  viSemaforos;
   char**  idIO;
   char** retardoIO;
   char** idVariableCompartida;
   char* ipUMC;
   char* UMCPort;




#endif /* NUCLEO_H_ */