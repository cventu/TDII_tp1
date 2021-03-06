/* Copyright 2016
 * All rights reserved.
 *
 * This file is part of lpc1769_template.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.*/


/*==================[inclusions]=============================================*/

#include "../../tp2_ejercicio2/inc/main.h"

#include "../../tp2_ejercicio2/inc/FreeRTOSConfig.h"
#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

xQueueHandle xQueue = NULL;

//Estructura de información el evento tecla
typedef struct _TECLA
{
	unsigned int 	codigo;
	int 			tiempo;
}TECLA;

/*==================[internal functions declaration]=========================*/


static void initHardware(void);

/*==================[internal data definition]===============================*/
#define mainQUEUE_LENGTH					( 5 )

#define SCAN_RATE_ms 150
#define TIEMPO_DE_DEBOUNCE_ms 20

#define MASK_REDLIGHT	1
#define MASK_GREENLIGHT	2
#define MASK_BLUELIGHT	4

#define BOTON_NO_PRESIONADO 0
#define BOTON_PRESIONADO	1

unsigned int tiempo_de_diagnostico = TIEMPO_DE_DIAGNOSTICO_ms;
int tiempo_de_pulsacion = -1;
#define REFRESH_RATE_ms 700
#define TIEMPO_DE_DIAGNOSTICO_ms 2000

#define NO_OPRIMIDO	0
#define DEBOUNCE	1
#define VALIDAR		2
#define OPRIMIDO	3

#define ASCENDENTE  0
#define DESCENDENTE	1

#define	FUNCIONANDO	0
#define	FRENADO			1

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void initHardware(void)
{
	SystemCoreClockUpdate();

	   Board_Init();
	   // Chip_GPIO_SetPinDirOutput(LPC_GPIO, num_puerto, num_pin);
	   // Chip_GPIO_SetPinDirInput(LPC_GPIO, num_puerto, num_pin);
	   // Chip_GPIO_GetPinState(LPC_GPIO, num_puerto, num_pin);
	    Chip_IOCON_PinMuxSet(LPC_IOCON,0,26,FUNC0);
	    Chip_IOCON_PinMuxSet(LPC_IOCON,2,0,FUNC0);
	    Chip_IOCON_PinMuxSet(LPC_IOCON,2,1,FUNC0);
	    Chip_IOCON_PinMuxSet(LPC_IOCON,2,10,FUNC0);
	    Chip_GPIO_SetPinDIROutput(LPC_GPIO,0,26); //Azul
	    Chip_GPIO_SetPinDIROutput(LPC_GPIO,2,0); // Rojo
	    Chip_GPIO_SetPinDIROutput(LPC_GPIO,2,1); // Verde
	    Chip_GPIO_SetPinDIRInput(LPC_GPIO,2,10); // Pulsador para detener secuencia
			Chip_GPIO_SetPinDIRInput(LPC_GPIO,2,11); // Pulsador para arrancar secuencia
			Chip_GPIO_SetPinDIRInput(LPC_GPIO,2,12); // Pulsador para invertir secuencia

}

void JuegoDeLuces(void *pvParameters)
{
	unsigned int sentido = ASCENDENTE;
	unsigned int estado = FUNCIONANDO;
	unsigned int estado_de_salidas = 1; //random
	TECLA tecla;
	tecla.codigo = BOTON_NO_PRESIONADO;
	//Loop infinito
	while(1)
	{
		if(xQueueReceive( xQueue, &tecla, portMAX_DELAY ) == pdTRUE)
		{
			if(tecla.codigo == BOTON_PRESIONADO)
			{
				estado = FRENADO;
			}
		}
		
		if(xQueueReceive( xQueue_2, &tecla_2, portMAX_DELAY ) == pdTRUE)
		{
			if(tecla_2.codigo == BOTON_PRESIONADO)
			{
				estado = FUNCIONANDO;
			}
		}
		
		if(xQueueReceive( xQueue_3, &tecla_3, portMAX_DELAY ) == pdTRUE)
		{
			if(tecla_3.codigo == BOTON_PRESIONADO)
			{
				if (sentido == ASCENDETE)
				{
					sentido = DESCENDENTE;
				}
				else
				{
					sentido = ASCENDETE;
				}
			}
		}
		
		if(estado == FUNCIONANDO)
		{
			switch (estado_de_salidas)
			{
				case 1:
								Chip_GPIO_SetPinOutLow(LPC_GPIO,2,0);
								Chip_GPIO_SetPinOutLow(LPC_GPIO,2,1);
								Chip_GPIO_SetPinOutHigh(LPC_GPIO,0,26);
								break;
								
								
				case 2:
								Chip_GPIO_SetPinOutLow(LPC_GPIO,2,0);
								Chip_GPIO_SetPinOutHigh(LPC_GPIO,2,1);
								Chip_GPIO_SetPinOutLow(LPC_GPIO,0,26);
								break;
								
								
				case 3:
								Chip_GPIO_SetPinOutHigh(LPC_GPIO,2,0);
								Chip_GPIO_SetPinOutLow(LPC_GPIO,2,1);
								Chip_GPIO_SetPinOutLow(LPC_GPIO,0,26);
								break;
								
								
				case 4:
								Chip_GPIO_SetPinOutHigh(LPC_GPIO,2,0);
								Chip_GPIO_SetPinOutLow(LPC_GPIO,2,1);
								Chip_GPIO_SetPinOutHigh(LPC_GPIO,0,26);
								break;
								
								
				case 5:
								Chip_GPIO_SetPinOutLow(LPC_GPIO,2,0);
								Chip_GPIO_SetPinOutHigh(LPC_GPIO,2,1);
								Chip_GPIO_SetPinOutHigh(LPC_GPIO,0,26);
								break;
								
							
				case 6:
								Chip_GPIO_SetPinOutHigh(LPC_GPIO,2,0);
								Chip_GPIO_SetPinOutHigh(LPC_GPIO,2,1);
								Chip_GPIO_SetPinOutLow(LPC_GPIO,0,26);
								break;
								
								
				case 7:
								Chip_GPIO_SetPinOutHigh(LPC_GPIO,2,0);
								Chip_GPIO_SetPinOutHigh(LPC_GPIO,2,1);
								Chip_GPIO_SetPinOutHigh(LPC_GPIO,0,26);
								break;
								
			}
			
			if (sentido == ASCENDENTE)
			{
				estado_de_salidas++;
			
				if (estado_de_salidas == 8)
				{
					estado_de_salidas =1;
				}
			}
			else
			{
				estado_de_salidas--;
				
				if (estado_de_salidas == 0)
				{
					estado_de_salidas=7;
				}
			}
						
			vTaskDelayUntil(&xMeDesperte,REFRESH_RATE_ms/portTICK_RATE_MS);
		}
	}
}

void Diagnostico(void *pvParameters)
{

	//	vPrint("Comienza el diagnostico\n")
	//Loop infinito
	while(1)
	{
		//IMPORTANTE:
		//Este loop pretende simular un conjunto de funciones que
		//insumen un tiempo para realizar el diagnostico del equipo
		//Pero NO es aceptable tener al micro dentro de esta tarea
		//haciendo nada, esto es solo un ejemplo.
		if(!tiempo_de_diagnostico)
		{
			//vPrintString("Termina el diagnostico\n");
			Board_LED_Set(0, 1);
			vTaskDelay(TIEMPO_DE_DIAGNOSTICO_ms/portTICK_RATE_MS);
			tiempo_de_diagnostico = TIEMPO_DE_DIAGNOSTICO_ms;
			Board_LED_Set(0, 0);
			//vPrintString("Comienza el diagnostico\n");
		}

	}

}



void TeclaEvent (void *pvParameters)
{
	unsigned int EstadoDebounce = NO_OPRIMIDO;
	TECLA tecla;
	portTickType xMeDesperte;

	//Inicio de variables y recuperación de parámetros

	xMeDesperte = xTaskGetTickCount();
	tecla.codigo = BOTON_NO_PRESIONADO;
	tecla.tiempo = 0;

	while(1){
	//debo verificar rebote
	switch(EstadoDebounce){
		case NO_OPRIMIDO:
			vTaskDelayUntil(&xMeDesperte,SCAN_RATE_ms/portTICK_RATE_MS);
			if(Chip_GPIO_GetPinState(LPC_GPIO,2,10))	//Si retorna una opresión
				EstadoDebounce = DEBOUNCE;		//Indico que esta corriendo el tiempo Debounce
			break;

		case DEBOUNCE:
			vTaskDelay(TIEMPO_DE_DEBOUNCE_ms/portTICK_RATE_MS);
			EstadoDebounce = VALIDAR;

		case VALIDAR:
			if(Chip_GPIO_GetPinState(LPC_GPIO,2,10))			//Si retorna algo sigue presionado
			{
				EstadoDebounce=OPRIMIDO;
				tiempo_de_pulsacion = 0;
			}
			else							// fue error
				EstadoDebounce=NO_OPRIMIDO;
			break;

		case OPRIMIDO:
			if(!Chip_GPIO_GetPinState(LPC_GPIO,2,10)) //No envio mensaje hasta no soltar
			{
				tecla.codigo = BOTON_PRESIONADO;
				tecla.tiempo = tiempo_de_pulsacion;
				tiempo_de_pulsacion = -1;
				//Envío a la cola, optamos por no bloquear si está llena
				xQueueSend( xQueue, &tecla, 0 );
				EstadoDebounce = NO_OPRIMIDO;
			}
			break;
		default:  break;
		}
	}
}

void TeclaEvent_2 (void *pvParameters)
{
	unsigned int EstadoDebounce = NO_OPRIMIDO;
	TECLA tecla_2;
	portTickType xMeDesperte;

	//Inicio de variables y recuperación de parámetros

	xMeDesperte = xTaskGetTickCount();
	tecla_2.codigo = BOTON_NO_PRESIONADO;
	tecla_2.tiempo = 0;

	while(1){
	//debo verificar rebote
	switch(EstadoDebounce){
		case NO_OPRIMIDO:
			vTaskDelayUntil(&xMeDesperte,SCAN_RATE_ms/portTICK_RATE_MS);
			if(Chip_GPIO_GetPinState(LPC_GPIO,2,11))	//Si retorna una opresión
				EstadoDebounce = DEBOUNCE;		//Indico que esta corriendo el tiempo Debounce
			break;

		case DEBOUNCE:
			vTaskDelay(TIEMPO_DE_DEBOUNCE_ms/portTICK_RATE_MS);
			EstadoDebounce = VALIDAR;

		case VALIDAR:
			if(Chip_GPIO_GetPinState(LPC_GPIO,2,11))			//Si retorna algo sigue presionado
			{
				EstadoDebounce=OPRIMIDO;
				tiempo_de_pulsacion = 0;
			}
			else							// fue error
				EstadoDebounce=NO_OPRIMIDO;
			break;

		case OPRIMIDO:
			if(!Chip_GPIO_GetPinState(LPC_GPIO,2,11)) //No envio mensaje hasta no soltar
			{
				tecla_2.codigo = BOTON_PRESIONADO;
				tecla_2.tiempo = tiempo_de_pulsacion;
				tiempo_de_pulsacion = -1;
				//Envío a la cola, optamos por no bloquear si está llena
				xQueueSend( xQueue_2, &tecla_2, 0 );
				EstadoDebounce = NO_OPRIMIDO;
			}
			break;
		default:  break;
		}
	}
}

void TeclaEvent_3 (void *pvParameters)
{
	unsigned int EstadoDebounce = NO_OPRIMIDO;
	TECLA tecla_3;
	portTickType xMeDesperte;

	//Inicio de variables y recuperación de parámetros

	xMeDesperte = xTaskGetTickCount();
	tecla_3.codigo = BOTON_NO_PRESIONADO;
	tecla_3.tiempo = 0;

	while(1){
	//debo verificar rebote
	switch(EstadoDebounce){
		case NO_OPRIMIDO:
			vTaskDelayUntil(&xMeDesperte,SCAN_RATE_ms/portTICK_RATE_MS);
			if(Chip_GPIO_GetPinState(LPC_GPIO,2,12))	//Si retorna una opresión
				EstadoDebounce = DEBOUNCE;		//Indico que esta corriendo el tiempo Debounce
			break;

		case DEBOUNCE:
			vTaskDelay(TIEMPO_DE_DEBOUNCE_ms/portTICK_RATE_MS);
			EstadoDebounce = VALIDAR;

		case VALIDAR:
			if(Chip_GPIO_GetPinState(LPC_GPIO,2,12))			//Si retorna algo sigue presionado
			{
				EstadoDebounce=OPRIMIDO;
				tiempo_de_pulsacion = 0;
			}
			else							// fue error
				EstadoDebounce=NO_OPRIMIDO;
			break;

		case OPRIMIDO:
			if(!Chip_GPIO_GetPinState(LPC_GPIO,2,12)) //No envio mensaje hasta no soltar
			{
				tecla_3.codigo = BOTON_PRESIONADO;
				tecla_3.tiempo = tiempo_de_pulsacion;
				tiempo_de_pulsacion = -1;
				//Envío a la cola, optamos por no bloquear si está llena
				xQueueSend( xQueue_3, &tecla_3, 0 );
				EstadoDebounce = NO_OPRIMIDO;
			}
			break;
		default:  break;
		}
	}
}


int main(void) {

	// TODO: insert code here

	//Incialización del Hardware
	initHardware();

	//Trace del RTOS
	//traceSTART();

	xQueue = xQueueCreate(5,sizeof(TECLA));
	xQueue_2 = xQueueCreate(5,sizeof(TECLA));
	xQueue_3 = xQueueCreate(5,sizeof(TECLA));

	//Creación de las tareas
	// xTaskCreate( pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask )
	xTaskCreate(	JuegoDeLuces, (signed portCHAR* )
					"Luces",
					configMINIMAL_STACK_SIZE,
					NULL,
					tskIDLE_PRIORITY+2,
					NULL );

	xTaskCreate(	Diagnostico, ( signed portCHAR* )
					"Diag",
					configMINIMAL_STACK_SIZE,
					NULL,
					tskIDLE_PRIORITY+1,
					NULL );
					
	xTaskCreate(	TeclaEvent,
					( signed portCHAR* )"Tecl",
					configMINIMAL_STACK_SIZE,
					NULL,
					tskIDLE_PRIORITY+2,
					NULL );
					
	xTaskCreate(	TeclaEvent_2,
					( signed portCHAR* )"Tecl_2",
					configMINIMAL_STACK_SIZE,
					NULL,
					tskIDLE_PRIORITY+2,
					NULL );
					
	xTaskCreate(	TeclaEvent_3,
					( signed portCHAR* )"Tecl_3",
					configMINIMAL_STACK_SIZE,
					NULL,
					tskIDLE_PRIORITY+2,
					NULL );

	//Inicio el Scheduler
	vTaskStartScheduler();

	// Enter an infinite loop, just incrementing a counter
	volatile static int i = 0 ;
	while(1) {
		i++ ;
	}
	return 0 ;
}

void vApplicationTickHook ( void )
{
	if(tiempo_de_diagnostico)
		tiempo_de_diagnostico--;
}


void vApplicationIdleHook ( void )
{
	__ASM volatile ("wfi");
}

