#include <Arduino.h>

/*
Captura de muestras con Arduino SAMD21 captura_muestras_SAMD21.ino
Copyright (C) 2019  Patricio Coronado Collado
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License at http://www.gnu.org/licenses .
AUTHOR: Patricio Coronado Collado
WEBSITE: 
HISTORY:
*/
#include "ZeroConfigureADC.h" 
#include <Arduino.h>
#include <segainvex_scpi_Serial.h> // funciones y variables de segainvex_scpi_serial
#include <avdweb_SAMDtimer.h> //Para utilizar los Timers

 /**************** CONFIGURACION DE COMANDOS *******************************/
 tipoNivel INSTRUMENTO[] = //Comandos específicos del instrumento
{
	SCPI_COMANDO(PIDEMUESTRAS,PMS,fs1)//Comando para pedir muestras
	SCPI_COMANDO(NMUESTRAS,NMS,fs2)//Comando para programar el número de muestas
	SCPI_COMANDO(TIEMPOS,TPS,fs3)//Envía al PC el tiempo de conversión y el periodo de muestreo en microsegundos 
	SCPI_COMANDO(ADCCONFIG,ADC,fs4)//Recibe del PC el prescaler y el SampleNum del ADC
	SCPI_COMANDO(DATODAC,DAC,fs5)//Recibe un dato para el DAC
	SCPI_COMANDO(PARAMADC,PAD,fs6)//Recibe un dato para el DAC
};
 tipoNivel NivelDos[] = //Array de estructuras tipo Nivel
{
	SCPI_SUBMENU(INSTRUMENTO,INS)	
	SCPI_COMANDO(ERROR,ERR,fs243)// Envía el ultimo error
	SCPI_COMANDO(*IDN,*IDN,fs240)// Identifica el instrumento
	SCPI_COMANDO(*OPC,*OPC,fs248)// Devuelve un 1 al PC
	SCPI_COMANDO(*CLS,*CLS,fs255)// Borra la pila de errores
};
SCPI_NIVEL_RAIZ// Macro que hace la declaración obligatoria del nivel Raiz
tipoCodigoError CodigoError=
{
  // Errores del sistema SCPI 0...6
  " ",						// ERROR N. 0
  "1 Caracter no valido",                // ERROR N. 1
  "2 Comando desconocido",               // ERROR N. 2
  "3 Cadena demasiado larga",            // ERROR N. 3
  "4 Parametro inexistente",             // ERROR N. 4
  "5 Formato de parametro no valido",    // ERROR N. 5
  "6 Parametro fuera de rango",          // ERROR N. 6
  // Errores personalizados por el usuario 
  "7 La variable no se ha cambiado",		// ERROR N. 7
};	
/**************** FIN CONFIGURACION DE COMANDOS ****************************/
/***************************************************************************
*						CONSTANTES
 * **************************************************************************/
#define MUESTRAS 200
#define ADCPIN A1
/***************************************************************************
*					VARIABLES GLOBALES
 * **************************************************************************/
int NumeroDeMuestras=80;
unsigned int Array[MUESTRAS];//Array para poner las muestras
unsigned int IndiceArray=0;
int TiempoDeConversion,TiempoDeMuestreo;//Tiempos medidos en microsegundos
short int TimeSampling=1000;
/***************************************************************************
 *				PROTOTIPOS DE FUNCIONES
 * **************************************************************************/
void escribe_DAC(uint16_t);	
void configura_DAC(void);
int configura_ADC(unsigned long,unsigned long, uint8_t); 
int configura_parametros_ADC(void);
void calibracion_ADC_SAM(int offset, uint16_t gain);//No se utiliza
void ISR_timer_adquisicion(struct tc_module *const module_inst);
/*	pero si quieres calibrar tu ADC un ejemplo sería
	calibracion_ADC_SAM(6,2057);
	Donde 6 es para corregir el offset de cero y 2057 la pendiente
	Pero primero tienes que saber que valores son los apropiados
*/
/***************************************************************************
*							OBJETOS
 * **************************************************************************/
//Timer para adquisición. función que atiende a la interrupción
SAMDtimer TimerAdquisicion = SAMDtimer(4, ISR_timer_adquisicion, TimeSampling);
/***************************************************************************
								SETUP
****************************************************************************/
void setup() 
{
	NOMBRE_DEL_SISTEMA_64B(Captura de muestras con Arduino SAMD21)
	SerialUSB.begin(115200);
	analogReadResolution(12);
	analogReference(AR_INTERNAL1V0);
	calibracion_ADC_SAM(0,2049);
    //configura_ADC(6,2);
	configura_DAC();
	escribe_DAC(0x200);	//0.5 voltios de salida
	TimerAdquisicion.enable(true);
}
//
/***************************************************************************
								LOOP
****************************************************************************/
void loop()
{
  // Si recibe algo por el puerto serie lo procesa con segainvex_scpi_serial 
  if (SerialUSB.available()){scpi();}
}
/****************************************************************
		Comando INSTRUMENTO:PIDEMUESTRAS Ó INS:PMS
		Envía "NumeroDeMuestras" muestas al PC
*****************************************************************/
int fs1(void)
{
	 int n;
	 char UnaMuestra[32]; // Cadena auxiliar, para poner una sola muestra
	 char CadenaDeMuestras[2048];//Cadena a enviar
	 CadenaDeMuestras[0]='\0'; // Inicializa la cadena de muestras
	 UnaMuestra[0]='\0';  // Inicializa la cadena auxiliar
	 strcat (CadenaDeMuestras,"DAT");//Pone el identificador
	 sprintf (UnaMuestra," %u",TimeSampling); // Añade el periodo de muestreo a la cadena intermedia
	 strcat (CadenaDeMuestras,UnaMuestra); // Añade el TimeSampling
	 //
	 Array[0]=Array[1];
	 for(n=0;n < NumeroDeMuestras;n++)// Recorre el array de muestras
	 {//El formato es "Identificador espacio muestra espacio muestra ...espacio muestra'\0'"
		 sprintf (UnaMuestra," %u",Array[n]); // Añade la muestra a la cadena intermedia
		 strcat (CadenaDeMuestras,UnaMuestra); // Añade la muestra a la cadena a enviar
	 }
	 strcat (CadenaDeMuestras,"\0"); // Añade a la cadena a enviar un final de cadena
	 SerialUSB.println(CadenaDeMuestras); // Envía la cadena de muestras
return 0;	 
}
/*****************************************************************
	Comando INSTRUMENTO:NMUESTRAS Ó INS:NMS
	Cambia el número de muestras a recibir
*****************************************************************/
int fs2(void)
{
	int ValoresPosibles[]={20,40,60,80,100,120,140,160,180,200};
	if(!cambia_variable_int_discreta_del_sistema	(&NumeroDeMuestras,ValoresPosibles,sizeof(ValoresPosibles))) 
	errorscpi(7);
return 0;
}
/****************************************************************
	Comando INSTRUMENTO:TIEMPOS Ó INS:TPS
	Envía al PC los tiempos de conversión y muestreo medidos
	en microsegundos
*****************************************************************/
int fs3(void)
{
	SerialUSB.print("Tiempo de conversion= ");SerialUSB.println(TiempoDeConversion);	
	//SerialUSB.print("Tiempo de muestreo= ");SerialUSB.println(TiempoDeMuestreo);
	SerialUSB.print("CTRLB ");SerialUSB.println(ADC->CTRLB.reg,BIN);
	SerialUSB.print("AVGCTRL ");SerialUSB.println(ADC->AVGCTRL.reg,BIN);
	SerialUSB.print("SAMPCTRL ");SerialUSB.println(ADC->SAMPCTRL.reg,BIN);
  //SerialUSB.print(ADC->CTRLB.reg,BIN);	
return 0;
}
/****************************************************************
	SCPI_COMANDO(ADCCONFIG,ADC,fs4)//Recibe del PC el prescaler y el SampleNum del ADC
	Comando INSTRUMENTO:ADCCONFIG Ó INS:ADC
	lee del PC SampleNum y Prescaler y configura el ADC
*****************************************************************/
int fs4(void) 
{
static unsigned int Prescaler=7;
static unsigned int SampleNum=0;
static unsigned int Samplen=63;

unsigned int np; // número de parámetros leido por sscanf
// Si solo pregunta por el dato, se responde y sale
	if(FinComando[0]=='?')  
	{
		SerialUSB.println(Prescaler);
		SerialUSB.println(SampleNum);
		return 2;
	}      
	if(FinComando[0]==' ') // Si el primer carácter de FinComado es 'espacio' lee los parámetros 
  	{   
    	np = sscanf(FinComando,"%u %u %u",&Prescaler,&SampleNum,&Samplen);// Lee la cadena de parámetros
		if(np != 3){errorscpi(5);return 0;}// Si no lee 2 parámetros ¡Error!:Formato de parámetro no válido
  	}   
	  else {errorscpi(4);return 0;} // Si el comando no empieza por 'espacio'¡Error!Parámetros inexistentes.
//
	configure_ADC(Prescaler,SampleNum,Samplen);
	return 0;
}
/*****************************************************************
 *	 			CAMBIA LA SALIDA DEL DAC
 * ***************************************************************/
int fs5(void)
{
	int DatoDAC=0;
	cambia_variable_int_del_sistema(&DatoDAC,1023,0);
	while (DAC->STATUS.reg & DAC_STATUS_SYNCBUSY);
	DAC->DATA.reg = DatoDAC;
	return 1;
}
/*****************************************************************
 *	 			CAMBIA LA SALIDA DEL DAC
 * ***************************************************************/
int fs6(void)
{
	 uint16_t Registro16Bits;
	 char UnRegistro[32]; // Cadena auxiliar, para poner un solo dato
	 char SRegistros[64];//Cadena a enviar
	 SRegistros[0]='\0'; // Inicializa la cadena de registros
	 UnRegistro[0]='\0';  // Inicializa la cadena auxiliar
	 strcat (SRegistros,"REG");//Pone el identificador
	 Registro16Bits=ADC->CTRLB.reg,BIN>>8;
	 sprintf (UnRegistro," %x",Registro16Bits); //Pone el registro de control B
	 strcat (SRegistros,UnRegistro); // Lo pone en la cadena a enviar
	 sprintf (UnRegistro," %x", ADC->AVGCTRL.reg,BIN); //Pone el registro de promediado
	 strcat (SRegistros,UnRegistro); // Lo pone en la cadena a enviar
	 sprintf (UnRegistro," %x", ADC->SAMPCTRL.reg,BIN); //Pone el registro de periodos de muestreo
	 strcat (SRegistros,UnRegistro); // Lo pone en la cadena a enviar
	 strcat (SRegistros,"\0"); // Añade a la cadena a enviar un final de cadena
	 SerialUSB.println(SRegistros); // Envía la cadena de muestras
}
/*****************************************************************
		FUNCION DE LA INTERRUPCION DEL TIMER
		Captura un dato y lo pone en el array de tamaño MUESTRAS
		Cuando rellena posición MUESTRAS-1 empieza por la 0
		También mide los tiempos de conversión y el periodo
		de muestreo real 
*****************************************************************/
void ISR_timer_adquisicion(struct tc_module *const module_inst)
{
	int short MedidaADC;
	int TiempoActual;
	static int TiempoAnterior;
//	
	TiempoActual=-micros();
	TiempoDeMuestreo=TiempoAnterior-TiempoActual;
	TiempoAnterior=TiempoActual;
	TiempoDeConversion=micros();
  Array[IndiceArray] = analogRead(A1);  
	TiempoDeConversion=micros()-TiempoDeConversion;
	IndiceArray++;
	if(IndiceArray>MUESTRAS)IndiceArray=0;
}
/*******************************************************************
 				 	ESCRIBE EN EL DAC PARA 10BITS
 *******************************************************************/	
void escribe_DAC(uint16_t DatoDAC)	
{
	if (DatoDAC >= 0x400) {DatoDAC=0x400;}
	while (DAC->STATUS.reg & DAC_STATUS_SYNCBUSY);
	DAC->DATA.reg = DatoDAC;
	//analogWrite(A0, DatoDAC); // Inicializa el DAC
}
/*******************************************************************
 * 			CONFGURA EL DAC  10BITS 1 VOLTIO
 *******************************************************************/
void configura_DAC(void)
{
  // Referencia del DAC 1V
  while ( DAC->STATUS.bit.SYNCBUSY == 1 ); //Sincronismo
  // Referencia 1V habilita la salida
  DAC->CTRLB.reg = DAC_CTRLB_REFSEL_INT1V | DAC_CTRLB_EOEN ;
  analogWriteResolution(10); //DAC a máxima resolución
    analogWrite(A0, 128); // Inicializa el DAC
}
/********************************************************************
FUNCION QUE HACE LA CALIBRACION TOCANDO LOS REGISTROS DEL SISTEMA
********************************************************************/
void calibracion_ADC_SAM(int offset, uint16_t gain)
{
	// Set correction values
	ADC->OFFSETCORR.reg = ADC_OFFSETCORR_OFFSETCORR(offset);
	ADC->GAINCORR.reg = ADC_GAINCORR_GAINCORR(gain);
	// Enable digital correction logic
	ADC->CTRLB.bit.CORREN = 1;
	while(ADC->STATUS.bit.SYNCBUSY);
}
/*******************************************************************/
