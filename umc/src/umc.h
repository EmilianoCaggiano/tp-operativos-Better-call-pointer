/*
 * umc.h
 *
 *  Created on: 23/4/2016
 *      Author: utnso
 */

#ifndef UMC_H_
#define UMC_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/config.h>
#include <commons/log.h>
#include <libreriasCompartidas/socket.h>

#define FAIL -1

//parametros
char* puertoTCP;
char* ip_Swap;
char* puerto_Swap;
char* marcos;
char* marco_Size;
char* marco_x_proc;
char* entradas_TLB;
char* espera;
char* puertoNucleo;
char* puertoCPU;

//prototipos
void leerArchivoDeConfiguracion();
int verificarExistenciaDeArchivo(char* rutaArchivoConfig);
void setearValores(t_config * archivoConfig);



#endif /* UMC_H_ */
