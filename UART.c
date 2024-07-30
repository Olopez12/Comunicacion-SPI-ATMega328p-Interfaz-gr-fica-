/*
 * UART.c
 *
 * Created: 23/07/2024 00:54:56
 *  Author: Usuario
 */ 
#define F_CPU 16000000UL
#include "UART.h"
#include <util/delay.h>
#include <avr/io.h>

void UART_init(void)
{
	DDRD |= (1<<1);							//TX se activa como salida 
	DDRD &= ~(1<<0);						//RX se activa como entrada 
	
	
	UCSR0A = (0<<TXC0)|(0<<U2X0)|(0<<MPCM0);
	UCSR0B = (1<<RXCIE0)|(0<<TXCIE0)|	(0<<UDRIE0)|(1<<RXEN0)|(1<<TXEN0)|(0<<UCSZ02)|(0<<TXB80); //Recepción y transmision de datos 
	UCSR0C = (0<<UMSEL01)|(0<<UMSEL00)|(0<<UPM01)|(0<<UPM00)|(0<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00)|(0<<UCPOL0);
	
	UBRR0 = 103; //Baudrate 9600
}

unsigned char UART_read(void)
{
	if(UCSR0A&(1<<7))	//Bit7 en 1 del registro 
	return UDR0;		//Devuelve el valor almacenado
	else
	return 0;
}

void UART_write(unsigned char caracter)
{
	while(!(UCSR0A&(1<<5)));				// Cuando UDR0 esté lleno espera
	UDR0 = caracter;						// Se vacía y lo regresa
}

void UART_write_txt(char* cadena)			//cadena de caracteres 
{
	while(*cadena !=0x00)
	{
		UART_write(*cadena);		
		cadena++;
	}
}

