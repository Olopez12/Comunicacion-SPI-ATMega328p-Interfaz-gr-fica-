/*
 * Maestro.c
 *
 * Created: 29/07/2024 23:23:47
 * Author : Usuario
 */ 

/*---------------------------Mestro----------------------------*/

/* ----------------Definicion de librerias a utilizar-----------------------*/
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "SPI/SPI.h"
#include "UART/UART.h"


//---------------Variables utilizadas a para los valores de ADC---------------
uint8_t valorSPI2 = 0;
uint8_t valorSPI = 0;

//---------Array utilizado para la conversion de variable de 8 bits a string -----
char CONV[8];

//----Funciones para refrescar el puerto del contador y envio de valores AD-------

void refresh_port(uint8_t value);
void send_voltage_via_uart(uint8_t value, char id);

//-------Valor y bandera para el puerto-------------
volatile uint8_t received_value = 0;
volatile uint8_t Nuevo_Valor = 0;


//Interrupcion que salta cuando se le manda un valor por usart

ISR(USART_RX_vect) {
	received_value = UDR0; // Leer el valor recibido
	Nuevo_Valor = 1; // Indicar que un nuevo valor ha sido recibido
}

//Loop

int main(void) {
	UART_init();
	InitSPI(SPI_MASTER_OSC_DIV16, SPI_DATA_ORDER_MSB, SPI_CLOCK_IDLE_LOW, SPI_CLOCK_FIRST_EDGE);
	
	// Configuración de los pines como salida
	DDRD |= (1 << PIND2) | (1 << PIND3) | (1 << PIND4) | (1 << PIND5) | (1 << PIND6) | (1 << PIND7);
	DDRB |= (1 << PINB0) | (1 << PINB1);
	
	PORTD &= ~((1 << PIND2) | (1 << PIND3) | (1 << PIND4) | (1 << PIND5) | (1 << PIND6) | (1 << PIND7));
	PORTB &= ~((1 << PINB0) | (1 << PINB1));

	sei(); // Habilitar interrupciones globales

	while (1) {
		if (Nuevo_Valor) {
			Nuevo_Valor = 0;
			refresh_port(received_value); // Actualizar el puerto con el valor recibido
		}
		
		PORTB &= ~(1 << PORTB2); //Prender la comunicacion con el slave 
		spiWrite('c');  //Referencia para identificar valor del slave 
		valorSPI = spiRead();
		send_voltage_via_uart(valorSPI, '1'); //Recibir valor ADC con identificador 
		
		spiWrite('d');  //Referencia 2
		valorSPI2 = spiRead();
		send_voltage_via_uart(valorSPI2, '2');
		
		PORTB |= (1 << PORTB2);
		_delay_ms(100);
	}
}

void refresh_port(uint8_t value) {
	// Colocar los bits correspondientes en los pines correctos
	PORTD = (PORTD & 0x03) | (value << 2); // Los bits PD2 a PD7
	PORTB = (PORTB & 0xFC) | (value >> 6); // Los bits PB0 y PB1
}

void send_voltage_via_uart(uint8_t value, char id) {
	float voltage = value * (5.0 / 255.0); // Convertir el valor ADC a voltaje
	dtostrf(voltage, 4, 2, CONV); // Convertir el valor a string con 2 decimales
	UART_write_txt("Voltage");
	UART_write(id); //identificador 
	UART_write_txt(": ");
	UART_write_txt(CONV); //Conversion a uart
	UART_write_txt("V\r\n");
}
