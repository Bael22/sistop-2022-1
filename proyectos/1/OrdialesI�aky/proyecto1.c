/*
  Proyecto 1 para la materia de Sistemas Operativos, grupo 6.

  Desarrollado por I�AKY ORDIALES CABALLERO

  Profesor: Gunnar Eyal Wolf Iszaevich
  Semestre: 2022-1
  Facultad de Ingenier�a, UNAM.

  Instrucciones.
  Implementen un programa que simule un proceso de la vida real
  que presente caracter�sticas de concurrencia, necesariamente
  empleando hilos o procesos simult�neos. 
  Implementen una interfaz de ususario profesional, que demuestre
  c�mo la sincronizaci�n puede ser bella y elegante.

*/

/*
         "Simulaci�n de centro comercial en pandemia"

  Para este desarrollo se decicdi� el simular la afluencia y 
  compras de un centro comercial durante esta �poca de pandemia. 
  El usuario indicar� el sem�foro que limita el cupo m�ximo de
  las instalaciones, el n�mero de clientes que desean acceder al
  centro comercial y el tiempo en segundos que permanecer�n 
  abiertas las instalaciones.


 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <ncurses.h>


// Declaraci�n funciones prototipo
void* hiloCliente(void* args);
void* agenteSeguridad(void* args);
void departamental(int);
void supermercado(int);
void ropa(int);
void tecnologia(int);
void kiosko(int);

// Declaraci�n sem�foros (torniquetes y mutex) globales
sem_t torniqueteLlegada;
sem_t multiplexSeguridad;
sem_t multiplexDepaEntrada;
sem_t multiplexDepaCajero;
sem_t mutexDepaVentas;
sem_t multiplexSuperEntrada;
sem_t multiplexSuperCajero;
sem_t mutexSuperVentas;
sem_t multiplexRopaEntrada;
sem_t multiplexRopaCajero;
sem_t mutexRopaVentas;
sem_t multiplexTecnoEntrada;
sem_t mutexTecnoCajero;
sem_t mutexTecnoVentas;
sem_t multiplexKioskoEntrada;
sem_t mutexKioskoCajero;
sem_t mutexKioskoVentas;
sem_t mutexClientesFuera;
sem_t mutexClientesDentro;

// Declaraci�n e inicializaci�n de variables de condici�n
pthread_mutex_t mutexAbrirCerrar = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cvAbrirCerrar = PTHREAD_COND_INITIALIZER;


// Declaraci�n variables globales
bool abierto;
int capPlaza;
int dentroPlaza;
int capDepa;
int dentroDepa;
int capSuper;
int dentroSuper;
int capRopa;
int dentroRopa;
int capTecno;
int dentroTecno;
int capKiosko;
int dentroKiosko;

int numCliente = 1;
int clientesFuera = 0;
int clientesDentro = 0;
int ventasDepa=0;
int ventasSuper=0;
int ventasRopa=0;
int ventasTecno=0;
int ventasKiosko=0;

// Hilo cliente
void* hiloCliente(void* args) {
  int cliente;
  int tienda;
  
  sem_wait(&torniqueteLlegada);
  cliente = numCliente;
  numCliente++;
  printf(" Ya estoy formado para entrar, soy el cliente %d\n", cliente);
  sleep(0.3);
  sem_post(&torniqueteLlegada);

  sem_wait(&multiplexSeguridad);

  if(abierto){
    printf(" Entr� a la plaza, con el n�mero de cliente %d\n", cliente);

    srand(time(NULL));

    sleep((rand()-cliente)%2+0.6);
    tienda = rand()%100;

    if(tienda<=32){
      sem_wait(&multiplexDepaEntrada);
      departamental(cliente);
      sem_post(&multiplexDepaEntrada);
	
    }else if(tienda<=58){
      sem_wait(&multiplexSuperEntrada);
      supermercado(cliente);
      sem_post(&multiplexSuperEntrada);
	
    }else if(tienda<=79){
      sem_wait(&multiplexRopaEntrada);
      ropa(cliente);
      sem_post(&multiplexRopaEntrada);
	
    }else if(tienda<=89){
      sem_wait(&multiplexTecnoEntrada);
      tecnologia(cliente);
      sem_post(&multiplexTecnoEntrada);
	
    }else{
      sem_wait(&multiplexKioskoEntrada);
      kiosko(cliente);
      sem_post(&multiplexKioskoEntrada);
    }      

    printf(" He acabado mis compras, me voy :), fui el cliente %d\n\n", cliente);

  }else{
    printf(" Me qued� afuera :( , iba a ser el cliente %d\n\n", cliente);
  }
  
  sem_post(&multiplexSeguridad);

  pthread_exit(NULL);
}

  
// Hilo seguridad
void* agenteSeguridad(void* args){

  while(!abierto){
    pthread_cond_wait(&cvAbrirCerrar, &mutexAbrirCerrar);
  }
  sem_post(&torniqueteLlegada);
  printf("\n\n\n ************************************************************************\n \n La plaza ha sido ABIERTA.\n\n ************************************************************************\n\n\n");
  while(abierto){
    pthread_cond_wait(&cvAbrirCerrar, &mutexAbrirCerrar);
  }
  printf("\n\n\n ************************************************************************\n \n La entrada a la plaza y a las tiendas se han CERRADO.\n Acabe sus compras y retirese.\n\n ************************************************************************\n\n\n");

  pthread_exit(NULL);

}

 
// Tienda departamental
void departamental(int cliente){
  printf(" -Ding- El cliente %d ha entrado a la tienda departamental.\n", cliente);
  sleep(0.3);
  sem_wait(&multiplexDepaCajero);
  sleep(0.5);
  printf(" Se hizo una compra en la tienda departamental.\n");
  sem_post(&multiplexDepaCajero);
  sem_wait(&mutexDepaVentas);
  ventasDepa++;
  sem_post(&mutexDepaVentas);
}

 
// Tienda super
void supermercado(int cliente){
  printf(" -Dong- El cliente %d ha entrado al super.\n", cliente);
  sleep(0.3);
  sem_wait(&multiplexSuperCajero);
  sleep(0.5);
  printf(" Se hizo una compra en el supermercado.\n");
  sem_post(&multiplexSuperCajero);
  sem_wait(&mutexSuperVentas);
  ventasSuper++;
  sem_post(&mutexSuperVentas);
}

 
// Tienda ropa
void ropa(int cliente){
  printf(" -Clink- El cliente %d ha entrado a comprar ropa.\n", cliente);
  sleep(0.3);
  sem_wait(&multiplexRopaCajero);
  sleep(0.5);
  printf(" Se hizo una compra en la tienda de ropa.\n");
  sem_post(&multiplexRopaCajero);
  sem_wait(&mutexRopaVentas);
  ventasRopa++;
  sem_post(&mutexRopaVentas);
}

 
// Tienda tecno
void tecnologia(int cliente){
  printf(" -Clank- El cliente %d ha entrado a la tienda de tecnolog�a.\n", cliente);
  sleep(0.3);
  sem_wait(&mutexTecnoCajero);
  sleep(0.5);
  printf(" Se hizo una compra tecnol�gica.\n");
  sem_post(&mutexTecnoCajero);
  sem_wait(&mutexTecnoVentas);
  ventasTecno++;
  sem_post(&mutexTecnoVentas);
}

 
// Kiosko
void kiosko(int cliente){
  printf(" -Clonk- El cliente %d est� en el kiosko.\n", cliente);
  sleep(0.3);
  sem_wait(&mutexKioskoCajero);
  sleep(0.6);
  printf(" Se hizo un tramite/venta en el kiosko.\n");
  sem_post(&mutexKioskoCajero);
  sem_wait(&mutexKioskoVentas);
  ventasKiosko++;
  sem_post(&mutexKioskoVentas);
}


 

int main() {

  pthread_t *pidClientes;
  pthread_t pidSeguridad;
  
  int capacidad;
  int semaforoPandemia;
  int clientes;
  int tiempo;
  int aux;
  abierto = 0;

  sem_init(&torniqueteLlegada,0,0);
  sem_init(&multiplexDepaCajero,0,5);
  sem_init(&multiplexSuperCajero,0,4);
  sem_init(&multiplexRopaCajero,0,2);
  sem_init(&mutexTecnoCajero,0,1);
  sem_init(&mutexKioskoCajero,0,1);
  sem_init(&mutexDepaVentas,0,1);
  sem_init(&mutexSuperVentas,0,1);
  sem_init(&mutexRopaVentas,0,1);
  sem_init(&mutexTecnoVentas,0,1);
  sem_init(&mutexKioskoVentas,0,1);
  sem_init(&mutexClientesFuera,0,1);

  initscr();
  cbreak();
  noecho();
  WINDOW * portada = newwin(28, 70, 2, 5);
  WINDOW * instrucciones = newwin(34, 80, 3, 7);
  WINDOW * datos = newwin(34, 80, 3, 7);
  WINDOW * iniciar = newwin(34, 80, 3, 7);
  
  start_color();
  init_pair(1, COLOR_CYAN, COLOR_BLACK);
  init_pair(2, COLOR_BLACK, COLOR_CYAN);
  init_pair(3, COLOR_BLACK, COLOR_WHITE);
  
  wbkgd(portada, COLOR_PAIR(2));
  wborder(portada, '|', '|', '_', '_', '|', '|', '_', '_');
  mvwprintw(portada, 3, 12, "*** SIMULADOR del Centro Comercial SISTOP ***");
  mvwprintw(portada, 5, 26, " Octubre del 2021.");
  mvwprintw(portada, 8, 17, "_-_-_-_-_-_-_-_-|-_|_-_-_-_-_-_-_-_-_");
  mvwprintw(portada, 9, 17, "-_-_-_-_-_-_-_-_|_-|-_-_-_-_-_-_-_-_-");
  mvwprintw(portada, 10, 17, "_-_-_-_-_-_-_-_-|-_|_-_-_-_-_-_-_-_-_");
  mvwprintw(portada, 11, 17, "-_-_-_-_-_-_-___|_-|____-_-_-_-_-_-_-");
  mvwprintw(portada, 12, 17, "_-_-_-_-_-_-_\\-_-_-_-_//-_-_-_-_-_-_");
  mvwprintw(portada, 13, 17, "-_-_-_-_-_-_-_\\-_-_-_//-_-_-_-_-_-_-");
  mvwprintw(portada, 14, 17, "_-_-_-_-_-_-_-_\\-_-_//-_-_-_-_-_-_-_");
  mvwprintw(portada, 15, 17, "-_-_-_-_-_-_-_-_\\__//_-_-_-_-_-_-_-_-");
  wattron(portada, A_REVERSE);
  mvwprintw(portada, 18, 17, " Presione ENTER para continuar ... ");
  wattroff(portada, A_REVERSE);
  mvwprintw(portada, 22, 4, "Desarrollado por: I�AKY ORDIALES CABALLERO");
  mvwprintw(portada, 24, 4, "Para la materia de sistemas operativos,");
  mvwprintw(portada, 25, 4, "Facultad de Ingenier�a, UNAM.");
  wrefresh(portada);
  keypad(portada, true);
  char c = wgetch(portada);
  while(c != 10){
    c = wgetch(portada);
  }

  clear();
  refresh();
  wbkgd(instrucciones, COLOR_PAIR(3));
  wborder(instrucciones, '|', '|', '=', '=', '=', '=', '=', '=');
  wprintw(instrucciones, "\n\n       *** Bienvenido al simulador de Centro Comercial SISTOP ***\n\n\n");
  wprintw(instrucciones, "  ~ Se simular� el flujo de un centro comercial durante la �poca de\n");
  wprintw(instrucciones, "    pandemia. La plaza tendr� un cupo m�ximo delimitado por su capacidad\n");
  wprintw(instrucciones, "    m�xima y por el sem�foro epidemiol�gico.\n\n");
  wprintw(instrucciones, "  ~ El centro comercial cuenta con 5 tiendas:\n");
  wprintw(instrucciones, "\t- Una tienda departamental grande (5 cajeros)\n");
  wprintw(instrucciones, "\t- Un supermercado grande (4 cajeros)\n");
  wprintw(instrucciones, "\t- Una tienda de ropa mediana (2 cajeros)\n");
  wprintw(instrucciones, "\t- Una tienda de tecnolog�a chica (1 cajero)\n");
  wprintw(instrucciones, "\t- Un kiosko para tr�mites del gobierno (1 ventanilla)\n\n");
  wprintw(instrucciones, "  ~ Los clientes podr�n entrar tanto a la plaza como a las tiendas s�lo\n");
  wprintw(instrucciones, "    si van a realizar una compra y el cupo m�ximo no ha sido superado.\n\n\n");
  wprintw(instrucciones, "  *** A conitnuaci�n se le solicitar�n los valores para la simulaci�n,\n");
  wprintw(instrucciones, "      por favor ingr�selos seg�n se le solicitan.\n\n");
  wprintw(instrucciones, "     (Tome en cuenta las siguientes capacidades para las tiendas:\n");
  wprintw(instrucciones, "     - Departamental -> 40%% de la capacidad de la plaza.\n");
  wprintw(instrucciones, "     - Supermercado  -> 20%% de la capacidad de la plaza.\n");
  wprintw(instrucciones, "     - Ropa -> 10%% de la capacidad de la plaza.\n");
  wprintw(instrucciones, "     - Tecnolog�a -> 5%% de la capacidad de la plaza.\n");
  wprintw(instrucciones, "     - Kiosko -> 5%% de la capacidad de la plaza.)\n\n\n                  ");
  wattron(instrucciones, A_REVERSE);
  wprintw(instrucciones, " Presione ENTER para ingresar los datos ... ");
  wattroff(instrucciones, A_REVERSE);
  move(28,30);
  wborder(instrucciones, '|', '|', '=', '=', '=', '=', '=', '=');
  wrefresh(instrucciones);
  keypad(portada, false);
  keypad(instrucciones, true);
  c = 0;
  c = wgetch(instrucciones);
  while(c != 10){
    c = wgetch(instrucciones);
  }

  clear();
  curs_set(0);
  refresh();
  wbkgd(datos, COLOR_PAIR(3));
  wborder(datos, '|', '|', '=', '=', '=', '=', '=', '=');
  wprintw(datos, "\n\n                          ~ ~ SELECCIONA LOS VALORES ~ ~\n\n");
  wprintw(datos, "         (Utiliza las flechas del teclado para colocarte en la opci�n\n");
  wprintw(datos, "                     y la tecla ENTER para seleccionar.)\n");
  wprintw(datos, "\n\n  - �Cu�l ser� la capacidad m�xima del centro comercial?\n");
  wprintw(datos, "    Capacidad:\n");
  wprintw(datos, "     ");
  wattron(datos, A_REVERSE);
  wprintw(datos, " 20 ");
  wattroff(datos, A_REVERSE);
  wprintw(datos, "       50        100        150        200 \n");
  wborder(datos, '|', '|', '=', '=', '=', '=', '=', '=');
  refresh();
  keypad(instrucciones, false);
  keypad(datos, true);
  c=0;
  int auxFlecha=0;
  while( c != 10) {
    c = wgetch(datos);
    if(c == 10){
      break;
    }
    if(c == 5 && auxFlecha < 4){
      auxFlecha++;
    }
    else if(c == 4 && auxFlecha > 0){
      auxFlecha--;
    }
    wmove(datos, 10,2);
    wprintw(datos, "   ");
    switch(auxFlecha){
      case 0:
	wattron(datos, A_REVERSE);
	wprintw(datos, " 20 ");
	wattroff(datos, A_REVERSE);
	wprintw(datos, "       50        100        150        200 \n");
	break;
      case 1:
	wprintw(datos, " 20       ");
	wattron(datos, A_REVERSE);
	wprintw(datos, " 50 ");
	wattroff(datos, A_REVERSE);
	wprintw(datos, "       100        150        200 \n");
        break;
      case 2:
	wprintw(datos, " 20        50       ");
	wattron(datos, A_REVERSE);
	wprintw(datos, " 100 ");
	wattroff(datos, A_REVERSE);
	wprintw(datos, "       150        200 \n");
	break;
      case 3:
	wprintw(datos, " 20        50        100       ");
	wattron(datos, A_REVERSE);
	wprintw(datos, " 150 ");
	wattroff(datos, A_REVERSE);
	wprintw(datos, "       200 \n");
	break;
      case 4:
	wprintw(datos, " 20        50        100        150       ");
	wattron(datos, A_REVERSE);
	wprintw(datos, " 200 \n");
	wattroff(datos, A_REVERSE);
	break;
    }
    wborder(datos, '|', '|', '=', '=', '=', '=', '=', '=');
    refresh();
    
  }

  
  switch(auxFlecha){
    case 0:
      capacidad=20;
      break;
    case 1:
      capacidad=50;
      break;
    case 2:
      capacidad=100;
      break;
    case 3:
      capacidad=150;
      break;
    case 4:
      capacidad=200;
      break;
  }
  
  wprintw(datos, "\n\n  - Sem�foro epidemiol�gico (aforo permitido).\n");
  wprintw(datos, "    (verde = (80%%), amarillo = (60%%), naranja = (40%%), rojo = (30%%))\n");
  wprintw(datos, "    �En qu� sem�foro estamos?\n\n");
  wprintw(datos, "     ");
  wattron(datos, A_REVERSE);
  wprintw(datos, " verde ");
  wattroff(datos, A_REVERSE);
  wprintw(datos, "       amarillo        naranja        rojo \n");
  wborder(datos, '|', '|', '=', '=', '=', '=', '=', '=');
  refresh();
  c=0;
  auxFlecha=0;
  while( c != 10) {
    c = wgetch(datos);
    if(c == 10){
      break;
    }
    if(c == 5 && auxFlecha < 3){
      auxFlecha++;
    }
    else if(c == 4 && auxFlecha > 0){
      auxFlecha--;
    }
    wmove(datos, 17,2);
    wprintw(datos, "   ");
    switch(auxFlecha){
      case 0:
	wattron(datos, A_REVERSE);
	wprintw(datos, " verde ");
	wattroff(datos, A_REVERSE);
	wprintw(datos, "       amarillo        naranja        rojo \n");
	break;
      case 1:
	wprintw(datos, " verde       ");
	wattron(datos, A_REVERSE);
	wprintw(datos, " amarillo ");
	wattroff(datos, A_REVERSE);
	wprintw(datos, "       naranja        rojo \n");
        break;
      case 2:
	wprintw(datos, " verde        amarillo       ");
	wattron(datos, A_REVERSE);
	wprintw(datos, " naranja ");
	wattroff(datos, A_REVERSE);
	wprintw(datos, "       rojo \n");
	break;
      case 3:
	wprintw(datos, " verde        amarillo        naranja       ");
	wattron(datos, A_REVERSE);
	wprintw(datos, " rojo \n");
	wattroff(datos, A_REVERSE);
	break;
    }
    wborder(datos, '|', '|', '=', '=', '=', '=', '=', '=');
    refresh();
  }

  semaforoPandemia=auxFlecha+1;


  wprintw(datos, "\n\n  - �Cu�ntas horas al d�a abrir� sus puerta la plaza SISTOP?\n");
  wprintw(datos, "    (1 hora = 3 segundos de simulaci�n aprox.)\n");
  wprintw(datos, "    Horas abierto:\n");
  wprintw(datos, "     ");
  wattron(datos, A_REVERSE);
  wprintw(datos, " 2 ");
  wattroff(datos, A_REVERSE);
  wprintw(datos, "       4        6        8        10 \n");
  wborder(datos, '|', '|', '=', '=', '=', '=', '=', '=');
  refresh();
  c=0;
  auxFlecha=0;
  while( c != 10) {
    c = wgetch(datos);
    if(c == 10){
      break;
    }
    if(c == 5 && auxFlecha < 4){
      auxFlecha++;
    }
    else if(c == 4 && auxFlecha > 0){
      auxFlecha--;
    }
    wmove(datos, 23,2);
    wprintw(datos, "   ");
    switch(auxFlecha){
      case 0:
	wattron(datos, A_REVERSE);
	wprintw(datos, " 2 ");
	wattroff(datos, A_REVERSE);
	wprintw(datos, "       4        6        8        10 \n");
	break;
      case 1:
	wprintw(datos, " 2       ");
	wattron(datos, A_REVERSE);
	wprintw(datos, " 4 ");
	wattroff(datos, A_REVERSE);
	wprintw(datos, "       6        8        10 \n");
        break;
      case 2:
	wprintw(datos, " 2        4       ");
	wattron(datos, A_REVERSE);
	wprintw(datos, " 6 ");
	wattroff(datos, A_REVERSE);
	wprintw(datos, "       8        10 \n");
	break;
      case 3:
	wprintw(datos, " 2        4        6       ");
	wattron(datos, A_REVERSE);
	wprintw(datos, " 8 ");
	wattroff(datos, A_REVERSE);
	wprintw(datos, "       10 \n");
	break;
      case 4:
	wprintw(datos, " 2        4        6        8       ");
	wattron(datos, A_REVERSE);
	wprintw(datos, " 10 \n");
	wattroff(datos, A_REVERSE);
	break;
    }
    wborder(datos, '|', '|', '=', '=', '=', '=', '=', '=');
    refresh();
    
  }

  tiempo=2*(auxFlecha+1);

  wprintw(datos, "\n\n  - �Cu�ntas personas querr�n entrar a la plaza durante el d�a?\n");
  wprintw(datos, "    Clientes esperados:\n\n");
  wprintw(datos, "     ");
  wattron(datos, A_REVERSE);
  wprintw(datos, " 50 ");
  wattroff(datos, A_REVERSE);
  wprintw(datos, "       100        200        300        350 \n");
  wborder(datos, '|', '|', '=', '=', '=', '=', '=', '=');
  refresh();
  c=0;
  auxFlecha=0;
  while( c != 10) {
    c = wgetch(datos);
    if(c == 10){
      break;
    }
    if(c == 5 && auxFlecha < 4){
      auxFlecha++;
    }
    else if(c == 4 && auxFlecha > 0){
      auxFlecha--;
    }
    wmove(datos, 29,2);
    wprintw(datos, "   ");
    switch(auxFlecha){
      case 0:
	wattron(datos, A_REVERSE);
	wprintw(datos, " 50 ");
	wattroff(datos, A_REVERSE);
	wprintw(datos, "       100        200        300        350 \n");
	break;
      case 1:
	wprintw(datos, " 50       ");
	wattron(datos, A_REVERSE);
	wprintw(datos, " 100 ");
	wattroff(datos, A_REVERSE);
	wprintw(datos, "       200        300        350 \n");
        break;
      case 2:
	wprintw(datos, " 50        100       ");
	wattron(datos, A_REVERSE);
	wprintw(datos, " 200 ");
	wattroff(datos, A_REVERSE);
	wprintw(datos, "       300        350 \n");
	break;
      case 3:
	wprintw(datos, " 50        100        200       ");
	wattron(datos, A_REVERSE);
	wprintw(datos, " 300 ");
	wattroff(datos, A_REVERSE);
	wprintw(datos, "       350 \n");
	break;
      case 4:
	wprintw(datos, " 50        100        200        300       ");
	wattron(datos, A_REVERSE);
	wprintw(datos, " 350 \n");
	wattroff(datos, A_REVERSE);
	break;
    }
    wborder(datos, '|', '|', '=', '=', '=', '=', '=', '=');
    refresh();
    
  }

  switch(auxFlecha){
    case 0:
      clientes=50;
      break;
    case 1:
      clientes=100;
      break;
    case 2:
      clientes=200;
      break;
    case 3:
      clientes=300;
      break;
    case 4:
      clientes=350;
      break;
  }
  wprintw(datos, "\n\n                   ");
  wattron(datos, A_REVERSE);
  wprintw(datos, "  ~ ~ Presione ENTER para continuar ... ~ ~ ");
  wattroff(datos, A_REVERSE);
  wborder(datos, '|', '|', '=', '=', '=', '=', '=', '=');
  refresh();
  c=0;
  while(c != 10){
    c = wgetch(datos);
  }
  
  
  switch (semaforoPandemia){
    case 1:
      capPlaza=capacidad*0.8;
      break;
    case 2:
      capPlaza=capacidad*0.6;
      break;
    case 3:
      capPlaza=capacidad*0.4;
      break;
    case 4:
      capPlaza=capacidad*0.3;
      break;
    default:
      break;
  }
  
  capDepa = 0.4*capPlaza;
  capSuper = 0.2*capPlaza;
  capRopa = 0.1*capPlaza;
  capTecno = 0.05*capPlaza;
  capKiosko = 0.05*capPlaza;
  if(!capDepa) capDepa++;
  if(!capSuper) capSuper++;
  if(!capRopa) capRopa++;
  if(!capTecno) capTecno++;
  if(!capKiosko) capKiosko++;

  sem_init(&multiplexSeguridad, 0, capPlaza);
  sem_init(&multiplexDepaEntrada, 0, capDepa);
  sem_init(&multiplexSuperEntrada, 0, capSuper);
  sem_init(&multiplexRopaEntrada, 0, capRopa);
  sem_init(&multiplexTecnoEntrada, 0, capTecno);
  sem_init(&multiplexKioskoEntrada, 0, capKiosko);


  clear();
  refresh();
  wbkgd(iniciar, COLOR_PAIR(2));
  wborder(iniciar, '|', '|', '_', '_', '|', '|', '_', '_');

  wprintw(iniciar, "\n\n\n                          ~ ~ VALORES SELECCIONADOS ~ ~\n"); 
  
  wprintw(iniciar, "\n\n\n   Los valores ingresados para la simulaci�n fueron:\n\n");
  wprintw(iniciar, "\n     Capacidad m�xima de la plaza: %d\n", capacidad);
  
  switch (semaforoPandemia){
    case 1:
      wprintw(iniciar, "     Sem�foro epidemiol�gico: verde = (80%%)\n");
      break;
    case 2:
      wprintw(iniciar, "     Sem�foro epidemiol�gico: amarillo = (60%%)\n");
      break;
    case 3:
      wprintw(iniciar, "     Sem�foro epidemiol�gico: naranja = (40%%)\n");
      break;
    case 4:
      wprintw(iniciar, "     Sem�foro epidemiol�gico: rojo = (30%%)\n");
      break;
    default:
      break;
  }


  wborder(iniciar, '|', '|', '_', '_', '|', '|', '_', '_');
  refresh();
  
  wprintw(iniciar, "\n     Aforo plaza debido al sem�foro: %d\n", capPlaza);
  wprintw(iniciar, "     Aforo tienda departamental: %d\n", capDepa);
  wprintw(iniciar, "     Aforo supermercado: %d\n", capSuper);
  wprintw(iniciar, "     Aforo tienda de ropa: %d\n", capRopa);
  wprintw(iniciar, "     Aforo tienda de tecnolog�a: %d\n", capTecno);
  wprintw(iniciar, "     Aforo kiosko: %d\n", capKiosko);
  wprintw(iniciar, "\n     Clientes que querr�n ingresar: %d\n", clientes);
  wprintw(iniciar, "     Tiempo abierto: %d hrs simuladas  = %d seg reales aprox.\n", tiempo, tiempo*3);

  wprintw(iniciar, "\n\n\n\n\n               ");
  wattron(iniciar, A_REVERSE);
  wprintw(iniciar, " \"Presione ENTER para comenzar la simulaci�n ...\" \n\n");
  wattroff(iniciar, A_REVERSE);
  wborder(iniciar, '|', '|', '_', '_', '|', '|', '_', '_');
  refresh();
  c=0;
  while( c!= 10){
    c=wgetch(iniciar);
  }
  clear();
  refresh();

  endwin();

  system("clear");

  if(pthread_create(&pidSeguridad, NULL, &agenteSeguridad, NULL)){
    printf(" ERROR al crear hilo del agente de seguridad !!!!\n\n");
    return -1;
  }
  
  sleep(1);
  
  pidClientes=malloc(clientes*sizeof(pthread_t));
  aux=clientes;
  for(int i=0; i<clientes; i++){
    if(pthread_create(&pidClientes[clientes-aux-1], NULL, &hiloCliente, NULL)){
      printf(" ERROR al crear un hilo de cliente !!!!\n\n");
      return -1;
    }
  }
  c=0;
  printf("\n\n   Presione ENTER para abrir la plaza  ... \n\n");
  while(c != 10){
    c = getchar();
  }

  printf("\n\n Se abren las puertas...\n\n");
  abierto = 1;
  pthread_cond_signal(&cvAbrirCerrar);
  sleep(tiempo*3);
  abierto = 0;
  pthread_cond_signal(&cvAbrirCerrar);
  sleep(1);

  printf("\n\n Se termin� la simulaci�n.");
  printf("\n Presione ENTER para ver los resultados  ... \n\n");
  c=0;
  while(c != 10){
    c = getchar();
  }

  
  WINDOW * resultados = newwin(28, 70, 2, 5);
  initscr();

  start_color();
  init_pair(2, COLOR_BLACK, COLOR_CYAN);
  
  wbkgd(resultados, COLOR_PAIR(2));
  wborder(resultados, '|', '|', '_', '_', '|', '|', '_', '_');
  
  int total = ventasDepa+ventasSuper+ventasRopa+ventasTecno+ventasKiosko;
  wprintw(resultados, "\n\n\n                        ~ ~ ~ RESULTADOS ~ ~ ~ \n");
  wprintw(resultados, "\n\n   Los resultados de la simulaci�n fueron los siguientes:\n\n");
  wprintw(resultados, "   - Entraron %d de %d, faltaron %d personas de entrar a la plaza.\n",total, clientes, clientes-total );
  wprintw(resultados, "\n     -- La tienda departamental hizo %d ventas.\n", ventasDepa);
  wprintw(resultados, "     -- El supermercado hizo %d ventas.\n", ventasSuper);
  wprintw(resultados, "     -- La tienda de ropa hizo %d ventas.\n", ventasRopa);
  wprintw(resultados, "     -- La tienda de tecnolog�a logr� %d ventas.\n", ventasTecno);
  wprintw(resultados, "     -- El kiosko realiz� %d tr�mites/ventas.\n", ventasKiosko);
  wprintw(resultados, "\n   - En total fueron %d transacciones en la simulaci�n.\n", total);
  wprintw(resultados, "\n\n\n\n                   Gracias por usar el simulador :)     \n\n");
  wprintw(resultados, "\n\n\n                   ");
  wattron(resultados, A_REVERSE);
  wprintw(resultados, " \"Presione ENTER para SALIR ...\" \n\n");
  wattroff(resultados, A_REVERSE);
  wborder(resultados, '|', '|', '_', '_', '|', '|', '_', '_');
  wborder(resultados, '|', '|', '_', '_', '|', '|', '_', '_');
  refresh();

  c=0;
  while(c != 10) {
    c = wgetch(resultados);
  }

  endwin();
  
  return 0;
}
