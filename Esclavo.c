/*------------Programación para el esclavo---------------*/

//Declaracion de librerias y frecuencia 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "SPI/SPI.h"

//Valores para los 2 potenciometros 
volatile uint8_t ValorAdc = 0;
volatile uint8_t ValorAdc2 = 0;

//Funciones modificando como se inicia el ADC

void initADC(void);
void start_conversion(uint8_t channel);

int main(void)
{
	InitSPI(SPI_SLAVE_SS, SPI_DATA_ORDER_MSB, SPI_CLOCK_IDLE_LOW, SPI_CLOCK_FIRST_EDGE); //Configuracion para el SPI
	initADC();
	SPCR |= (1<<SPIE);
	sei();

	while (1)
	{
		start_conversion(0); // Iniciar la conversión del canal 0
		_delay_ms(100);
		start_conversion(1); // Iniciar la conversión del canal 1
		_delay_ms(100);
	}
}

void initADC(void)
{
	//Configuraciones para el adc
	ADMUX = 0;
	ADMUX |= (1<<REFS0);
	ADMUX &=~ (1<<REFS1);
	ADMUX |= (1<<ADLAR);
	ADCSRA = 0;
	ADCSRA |= (1<<ADEN);
	ADCSRA |= (1<<ADIE);
	ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	DIDR0 |= (1<<ADC0D);
	DIDR0 |= (1<<ADC1D);
}

void start_conversion(uint8_t channel)
{
	ADMUX = (ADMUX & 0xF8) | channel; // Seleccionar el canal
	ADCSRA |= (1<<ADSC); // Iniciar la conversión
}

ISR(ADC_vect)
{
	if ((ADMUX & 0x07) == 0) {
		ValorAdc = ADCH;
		} else if ((ADMUX & 0x07) == 1) {
		ValorAdc2 = ADCH;
	}
	ADCSRA |= (1<<ADIF); // Limpiar la bandera de interrupción ADC
}

ISR(SPI_STC_vect)
{
	uint8_t spiValor = SPDR; //Comunicacion spi con su respectiva representacion
	if (spiValor == 'c') {
		spiWrite(ValorAdc);
		} else if (spiValor == 'd') {
		spiWrite(ValorAdc2);
	}
}
