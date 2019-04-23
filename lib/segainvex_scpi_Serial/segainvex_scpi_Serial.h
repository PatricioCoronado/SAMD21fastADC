/************************************************************************************
 	DEPARTAMENTO DE ELECTRÓNICA DE SEGAINVEX. UNIVERSIDA AUTONOMA DE MADRID				
 	LIBRERIA PARA ARDUINO Segainvex_scpi_Serial V1.0
 	SISTEMA PARA COMUNICAR UNA COMPUTADORA CON ARDUINO MEDIANTE PUERTO SERIE 
 	Fichero de cabecera segainvex_scpi_Serial.h


	Copyright © 2017 Mariano Cuenca, Patricio Coronado
	
	This file is part of segainvex_scpi_Serial.

    segainvex_scpi_Serial is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    segainvex_scpi_Serial is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with segainvex_scpi_Serial.  If not, see <http://www.gnu.org/licenses/>.


 
		Mejoras futuras:
		1)Poner el menú de segainvex_scpi_Serial en flash para no consumir RAM
		2)Mejorar la función int cambia_variable_double_del_sistema(double *,double,double);
 *******************************************************************************************/
 
 
#ifndef SEGAINVEX_SCPI_SERIAL_H_INCLUDED
#define SEGAINVEX_SCPI_SERIAL_H_INCLUDED
//
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


#ifdef __cplusplus            
    extern "C" {              
#endif    
                    
/**********************************************************************************
 
		La librería funciona con 3 puertos serie: Serial, SerialUSB y Serial1
		Pero antes de hacer el include de la librería. Hay que indicar que
		puerto se va a utilizar con un define.
		
		#define SERIALUSB
		o
		#define SERIAL1
		
		Si no se incluye el #define el puerto por defecto es Serial
 
	HAY QUE DEJAR DESCOMENTADO EL DEFINE QUE SE CORRESPONDA CON EL PUERTO A UTILIZAR
	SI NO SE DESCOMENTA NINGUNA LINEA, EL PUERTO ES "Serial"
*************************************************************************************/
  #define SERIALUSB
 //#define SERIAL1
/***********************************************************************************
  DESDE AQUÍ ESTE FICHERO NO DEBE SER EDITADO. ES EL MISMO EN TODAS LAS APLICACIONES
 ***********************************************************************************/                             
                              

/*******************************************************************************
							CONSTANTES EXPORTADAS
Hay que tener en cuenta que el buffer de puerto serie de Arduino es 64 bytes							
Por lo que BUFFCOM_SIZE y LONG_SCPI no pueden ser mayores de 64 bytes.
Estas constantes pueden ser redefinidas.
********************************************************************************/
#define BUFFCOM_SIZE 32 //Longitud del buffer de lectura de de segainvex_scpi_Serial
#define LONG_SCPI 32 // Longitud máxima del comando mas largo sin contar parámetros
#define MAXERR 3   // Profundidad de la pila de errores
/*******************************************************************************
						DEFINICION DE TIPOS EXPORTADOS
*******************************************************************************/            
//TIPOS PARA LA ESTRUCTURA DEL MENU DE SEGAINVEX-SCPI
//typedef void (*tpf)();// tpf es ahora puntero a función
// Declaración de tipo estructura para menús de comandos SEGAINVEX-SCPI
typedef struct tipoNivel
{             
  int   NumNivInf;       // Número de niveles por debajo de este
  char *largo, *corto;   // Nombre largo y nombre corto del comando
  /*tpf pf*/int (*pf)();// Puntero a función a ejecutar
  tipoNivel *sub;     // Puntero a la estructura de niveles inferiores
};
// TIPO PARA EL ARRAY DE ERRORES
typedef char *tipoCodigoError[];
/********************************************************************************
      PROTOTIPOS DE FUNCIONES DE SEGAINVEX-SCPI QUE TIENEN QUE SER VISIBLES
*********************************************************************************/
/*********************************************************************************
	Funcion que gestiona la pila de errores
	El parámetro de entrada es el número del error a poner el la pila de errores
**********************************************************************************/
extern void errorscpi(int); // 
/*********************************************************************************
	Funcion principal de SEGAINVEX-SCPI
	Se debe ejecutar cuando se detecten datos en el puerto serie
**********************************************************************************/
extern void scpi(void);     
/**********************************************************************************
 FUNCIONES AUXILIARES, PARA LEER UN ARGUMENTO ENTERO QUE SE PASA CON EL COMANDO,
 CUYO FIN ES ACTUALIZAR EL VALOR DE UNA VARIABLE ENTERA O BOOLENAN DEL SISTEMA.
 SI EL COMANDO VA SEGUIDO DE ? LO QUE HACE LA FUNCIÓN ES DEVOLVER EL VALOR DE 
 LA VARIABLE.
 Leen el parámetro del array de char FinComando[]. El carácter FinComando[0] se 
 espera que sea un espacio. A continuación tiene que estar el parámetro a leer.
*********************************************************************************/
/*******************************************************************************
	Cambia la variable entera cuya dirección se pasa como argumento.
	Los argumentos de entrada son:
	La dirección de la variable entera, el valor máximo, el valor mínimo.
	Devuelve 1 si cambió la variable. 0, si no cambió la variable por errores.
	Y 2 si devolvió el valor de la variable.
*******************************************************************************/
extern int cambia_variable_int_del_sistema(int *,int,int);//
/*******************************************************************************
	Cambia la variable entera discreta cuya dirección se pasa como argumento.
	Los argumentos de entrada son:
	La dirección de la variable entera,array de entero con los valores posibles,
	tamaño del array.
	Devuelve 1 si cambió la variable. 0, si no cambió la variable por errores.
	Y 2 si devolvió el valor de la variable.
*******************************************************************************/
extern int cambia_variable_int_discreta_del_sistema(int *,int*,int);//
/*******************************************************************************
	Cambia la variable booleana cuya dirección se pasa como argumento.
	Los argumentos de entrada son:
	La dirección de la variable entera, el valor máximo, el valor mínimo y el
	número de error a anotar si la variable está fuera de rango.
	Devuelve 1 si cambió la variable. 0, si no cambió la variable por errores.
	Y 2 si devolvió el valor de la variable.
********************************************************************************/	
extern int cambia_variable_bool_del_sistema(bool *);
/*******************************************************************************
	Cambia la variable double cuya dirección se pasa como argumento.
	Los argumentos de entrada son:
	La dirección de la variable float, el valor máximo, el valor mínimo.
	Devuelve 1 si cambió la variable. 0, si no cambió la variable por errores.
	Y 2 si devolvió el valor de la variable. 
*******************************************************************************/
extern int cambia_variable_double_del_sistema(double *,double,double);
/********************************************************************************
   VARIABLES GLOBALES EXPORTADAS DE SEGAINVEX-SCPI QUE TIENEN QUE SER VISIBLES
  Son globales todos los modulos con el include a segainvex_scpi_Serial.h  						
********************************************************************************/
//Esta variable tiene que estar definida en el código del usuario.
extern tipoNivel Raiz[]; //Array de la estructura raiz de comandos
//Estas variable tiene que estar definida en el código del usuario.
extern tipoCodigoError CodigoError;//Puntero al array de cadenas explicativas del error
// FinComando es un array definido en seginvex_scpi.c de char que contiene los 
//parámetros que envia el PC tras el comando.
extern char *FinComando;// Puntero al final del comando para leer parámetros
//Cadena donde el usuario debe escribir el nombre de su sistema. La cadena se 
// envía al PC cuando este envíe a Arduino el comando *IDN
extern char IdentificacionDelSistema[];
/********************************************************************************
									MACROS EXPORTADAS 
*********************************************************************************/      
// Para definir submenús 
#define SCPI_SUBMENU(X,Y) sizeof(X)/sizeof(*X), #X,#Y,NULL,X,  
// Para definir comandos
#define SCPI_COMANDO(X,Y,Z) 0, #X,#Y,Z,NULL, //Para definir comandos 
//Para definir el nivel raiz de comandos
#define SCPI_NIVEL_RAIZ tipoNivel Raiz[]={sizeof(NivelDos)/sizeof(*NivelDos),"","",NULL,NivelDos};
//Macro que pone en el array "IdentificacionDelSistema" el nombre del sistema que implementa Arduino
#define NOMBRE_DEL_SISTEMA_64B(X) strcpy(IdentificacionDelSistema,#X);
/*********************************************************************************
 EDITABLE
 PROTOTIPOS DE FUNCIONES DEL PROGRAMA PRINCIPAL LLAMADAS POR SEGAINVEX-SCPI
 Añadir tantas funciones como se necesiten. El límite es las direcciones
 que puede acceder un puntero que será del tamaño de los bits que sea el 
 microprocesador/microcontrolador
 Aquí hay de fs0 a fs100 y de fs240 a fs255
 *********************************************************************************/
int fs0(void);
int fs1(void);
int fs2(void);
int fs3(void);
int fs4(void);
int fs5(void);
int fs6(void);
int fs7(void);
int fs8(void);
int fs9(void);
int fs10(void);
int fs11(void);
int fs12(void);
int fs13(void);
int fs14(void);
int fs15(void);
int fs16(void);
int fs17(void);
int fs18(void);
int fs19(void);
int fs20(void);
int fs21(void);
int fs22(void);
int fs23(void);
int fs24(void);
int fs25(void);
int fs26(void);
int fs27(void);
int fs28(void);
int fs29(void);
int fs30(void);
int fs31(void);
int fs32(void);
int fs33(void);
int fs34(void);
int fs35(void);
int fs36(void);
int fs37(void);
int fs38(void);
int fs39(void);
int fs40(void);
int fs41(void);
int fs42(void);
int fs43(void);
int fs44(void);
int fs45(void);
int fs46(void);
int fs47(void);
int fs48(void);
int fs49(void);
int fs50(void);
int fs51(void);
int fs52(void);
int fs53(void);
int fs54(void);
int fs55(void);
int fs56(void);
int fs57(void);
int fs58(void);
int fs59(void);
int fs60(void);
int fs61(void);
int fs62(void);
int fs63(void);
int fs64(void);
int fs65(void);
int fs66(void);
int fs67(void);
int fs68(void);
int fs69(void);
int fs70(void);
int fs71(void);
int fs72(void);
int fs73(void);
int fs74(void);
int fs75(void);
int fs76(void);
int fs77(void);
int fs78(void);
int fs79(void);
int fs80(void);
int fs81(void);
int fs82(void);
int fs83(void);
int fs84(void);
int fs85(void);
int fs86(void);
int fs87(void);
int fs88(void);
int fs89(void);
int fs90(void);
int fs91(void);
int fs92(void);
int fs93(void);
int fs94(void);
int fs95(void);
int fs96(void);
int fs97(void);
int fs98(void);
int fs99(void);
int fs100(void);
// definir tantas funciones como sean necesarias
int fs240(void);
int fs241(void);
int fs242(void);
int fs243(void);
int fs244(void);
int fs245(void);
int fs246(void);
int fs247(void);
int fs248(void);
int fs249(void);
int fs250(void);
int fs251(void);
int fs252(void);
int fs253(void);
int fs254(void);
int fs255(void);
/**************************************************************************/
/*
NOTAS:
1)  El buffer de recepción de Arduino es de 64 bytes, por lo que hay que dimensionar
	la cadena de recepción como mucho a 64 bytes. BUFFCOM_SIZE=<64 
2)  Si se lee el puerto serie desde Arduino con 
	SerialUSB.readBytesUntil('r',buffer,tamaño_buffer);
	hay que	asegurarse de que el terminador con que el PC cierre las cadenas a enviar
	sea '\r'.
3)	Si Arduino lee con SerialUSB.readBytesUntil('\r',buffer,tamaño_buffer);.
	Esta función no mete	el terminador '\r' en la cadena que devuelve, por lo que 
	hay que ponerselo, ya que segainvex_scpi_Serial necesita un terminador para dar por 
	buena una cadena. Al hacerlo hay que incrementar la variable que indica
	la longitud del comando "locCom" en 1. 
						
  Se ha definido un tipo de estructura denominado "tipoNivel". Las variables de este 
  tipo puede contener un nivel de comandos o un comando.
  
  Si contiene un nivel, el formato es:
  
  int número_de_comandos_del_nivel,
  char[] Nombre_largo_del_nivel,  
  char[] Nombre_corto_del_nivel,
  (*tpf)() NULL;                    //puntero a función   
  tipoNivel* Nombre_del_nivel,		//puntero a tipoNivel
  
  Y si contiene un comando, el formato es:
  
  int 0,
  char[] Nombre_largo_del_comando,
  char[] Nombre_corto_del_comando,
  (*tpf)() funcion_a_ejecutar,       //puntero a función
  tipoNivel* NULL,					//puntero a tipoNivel  
  
  Como se vé la diferencia es que si es un nivel, el puntero a función está a NULL, ya que
  al no ser un comando, no ejecuta ninguna función. Si es un comando, su puntero a función
  si que apunta a la función que se quiere ejecutar con ese comando. Pero su puntero a
  "tipoNivel" está a NULL, ya que de el no cuelga un nivel, porque es solo un comando.
  
  Se declaran array de variables tipoNivel.Siempre ha de existir este array:
  
  tipoNivel Raiz[] = {sizeof(NivelDos)/(5*sizeof(int)),"","",NULL, NivelDos};
	  
  Que tiene una única estructura "tipoNivel" y siempre el mismo formanto. 
  Es el nivel raiz del menú de submenús y comandos.
  
  Otro array que debe existir siempre es el denominado "NivelDos" (como se ve, 
  último miembro de la única estructura del "Raiz") 
	  
  El formato del array de estructuras "tipoNivel" "NivelDos" es:
  tipoNivel NivelDos[] ={
	  estructura_nivel_1,estructura_nivel_2,...,estructura_nivel_n,
	  estructura_comandos_1,estructura_comandos_2,..,estructura_comandos_n,
  };
  Cada nivel_estructura_i y nivel_comando_i tiene los formatos vistos arriba.
  
      El formato de cada array con un nivel de comandos es:
	tipoNivel NOMBRE_SUBNIVEL1[]={ 
		estructura_comandos_1,estructura_comandos_2,..,estructura_comandos_n,
  };
  
  Hay que añadir tantos arrays  "tipoNivel" como se necesiten. Y a su vez, en los
  arrays incluir tantos comandos como se necesiten. 
  
  Todo esto se puede declara en un fichero .h o dentro del .ino, lo que resulte
  más apropiado en cada caso.
*/
/**************************************************************************/
#ifdef __cplusplus            
    }                         
#endif                        


#endif // SEGAINVEX_SCPI_SERIAL_H_INCLUDED

