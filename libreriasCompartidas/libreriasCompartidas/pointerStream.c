/*
 * pointerStream.c
 *
 *  Created on: 1/6/2016
 *      Author: utnso
 */
#include "pointerStream.h"
/*******************************************************
 * Dependencias
 *******************************************************/

/***********************************************/
/* Para Handshake
/***********************************************/

Char getStreamId(Stream dataSerialized) {
	Char id;
	Stream ptrByte = dataSerialized;
	memcpy(&id,ptrByte, sizeof(id));
	return id;
}