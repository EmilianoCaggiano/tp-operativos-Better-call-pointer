#ifndef UMC_H_
#define UMC_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/config.h>
#include <commons/log.h>
#include <libreriasCompartidas/archivosYLogsYMas.h>
#include <libreriasCompartidas/espacioAsignado.h>
#include <libreriasCompartidas/pointerStream.h>
#include <pthread.h>
#include <semaphore.h>
#include <commons/collections/list.h>
#define FAIL -1
#define PROGRAMA_NO_INICIADO -2
#define TRUE 1
#define FALSE 0

int thread_socket;


//parametros
char* memoriaReal;
char* puertoEscucha;
char* ip_Swap;
char* puerto_Swap;
int marcos;
int marco_Size;
int marco_x_proc;
char* algoritmoDeReemplazo;
int entradas_TLB;
int espera;
char* memoriaTLB;
bool*bitMap;
bool* bitMapTLB;
bool paginaEncontrada;
int aciertosTLB;
int accesosTLB;
t_list* conexionSockets ;
SocketClient * socketSwap;
t_log* umclog;
pthread_mutex_t* mutexPedidos;
pthread_mutex_t* mutexThreadSockets;


  typedef struct{
	  int pid;
	  int pagina;
	  int frameTLB;
	  int momentoEntrada;
  }t_tlb;

  typedef struct{
	  int pid;
	  int cantPaginasEnMemoria;
  }paginasPorPrograma;

t_list * listaPaginasPorPrograma;
t_list * listaEspacioAsignado;
t_list * TLB;

//estructuras para los hilos de CPU y nucleo

typedef struct{
	//en estos campos van las cosas que recibe el hilo para laburar con el nucleo
}t_nucleo;

typedef struct{
	//en estos campos van las cosas que recibe el hilo para laburar con el CPU
}t_CPU;

//Prototipos

int encontrarPuntero(int pid);

void limpiarPagina(int comienzoDePagina);

int lugarAsignadoFinal(int pid);

int lugarAsignadoInicial(int pid) ;

void insertarNodoOrdenadoEnTLB(t_tlb*unNodo);

void insertarNodoOrdenadoLibre(int inicio, int cantidad, int IDFrame);

void eliminarFramesOcupadosContiguos(int cantidad, int frame);

//espacioLibre * crearEspacioLibre(int inicio) ;

void agregarEspacioLibre(int inicio);

void iniciarTLB();

void sacarPaginaDeTelebe(int pid, int pg) ;

int calcularIDPagina(int inicio);

void iniciarEstructurasUMC();

int cantidadDePaginasLibres();

bool verificarSiHayEspacio(int cantidadDePaginas) ;

void reservarPaginas(int paginaDeComienzo, int pid, int cantidadDePaginas) ;

void compactarUMC();

bool inicializarPrograma(int pid, int paginas, char*codigo);

bool hayMemoriaSuficiente(int paginas);

char*solicitarBytes(int pid, int pagina, int offset, int cantidad) ;

int paginasContiguasDeUMC(int cantidadDePaginas);

void finalizarPrograma(int pid);

void almacenarBytes(int pid, int pagina, int offset, int tamanio, char*buffer) ;

bool insertarEnListaAsignadoOrdenado(int pid, int cantidadDeNodos, int idFrame) ;

char * reservarMemoria(int , int );

void comandosUMC ();

void menuUMC();

void retardoUMC (int);

void dump ();

void flushTLB ();

void flushMemory ();

void liberarMemoria(char * );

void eliminarProcesoTLB(int PID) ;

void escuchoMuchasConexiones();

void meHablaKernel();

void crearListas();

void meHablaCPU();

void meHablaKernelPrueba ();

void meHablaCPUPrueba ();

void elDestructorDeNodosTLB(int);

void elDestructorDeNodosMemoria(int);

void inicioTLB();

void dumpContenidoDeMemoriaProcesoEnParticular(int );

void dumpContenidoDeMemoriaTodosLosProcesos();

void dumpEstructuraDeMemoriaTodosLosProcesos();

void dumpEstructuraDeMemoriaProcesoEnParticular(int );

t_list * creoTLB();

//SI ENCUENTRA DEVUELVE LA ENTRADA, SI NO ENCUENTRA DEVUELVE NULL
t_tlb * buscarEnTLB(int, int );

char* leerEnTLB(int PID, int pagina, int posicion, int tamanio) ;

//DEVUELVE 1 SI ESTA LLENA, DEVUELVE 0 SI NO ESTA LLENA
int tlbLlena();

//DEVUELVE 1 SI ESTA HABILITADA, DEVUELVE 0 SI NO ESTA HABILITADA
int tlbHabilitada();

//SI ENCUENTRA DEVUELVE LA PAGINA, SI NO ENCUENTRA DEvUELVE NULL
espacioAsignado * buscarEnMemoriaReal(int , int);

void manageSocketConnections();

void* manageSocketConnection(void* param);

bool escribirEnTLB(int , int , int , int , char*);

void llevarPaginaATLB(int , int , char* );

int reemplazarPaginaLRU();

int buscarPaginaVaciaEnTLB();

int reemplazarPaginaClock(int pid, int pagina);

int reemplazarPaginaClockModificado(int pid, int pagina, bool lectoEscritura);

int reemplazarPagina(int pid, int pagina,bool lectoEscritura);

int lugarAsignadoFinal(int pid);

int lugarAsignadoInicial(int pid);

#endif /* UMC_H_ */
