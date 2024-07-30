

#include "SPI.h"

#include <avr/io.h>
#include <stdint.h>


void InitSPI(Spi_Type sType, Spi_Data_Order sDataOrder, Spi_Clock_Polarity sClockPolarity, Spi_Clock_Phase sClockPhase)
{
	// PB2 -> D10 -> SS
	// PB3 -> D11 -> MOSI
	// PB4 -> D12 -> MISO
	// PB5 -> D13 -> SCK
	
	if(sType & (1<<MSTR))		// IF MASTER MODE
	{
		DDRB |= (1<<DDB3)|(1<<DDB5)|(1<<DDB2);		// MOSI, SCK, SS
		DDRB &= ~(1<<DDB4); 						//MISO
		SPCR |= (1<<MSTR);							//MASTER
		
		uint8_t temp = sType & 0b00000111;
		switch(temp){
			case 0:	//DIV2
			SPCR &= ~((1<<SPR1)|(1<<SPR0));
			SPSR |= (1<<SPI2X);
			break;
			case 1:	//DIV4
			SPCR &= ~((1<<SPR1)|(1<<SPR0));
			SPSR &= ~(1<<SPI2X);
			break;
			case 2:	//DIV8
			SPCR |= (1<<SPR0);
			SPCR &= ~(1<<SPR1);
			SPSR |= (1<<SPI2X);
			break;
			case 3:	//DIV16
			SPCR |= (1<<SPR0);
			SPCR &= ~(1<<SPR1);
			SPSR &= ~(1<<SPI2X);
			break;
			case 4:	//DIV32
			SPCR &= ~(1<<SPR0);
			SPCR |= (1<<SPR1);
			SPSR |= (1<<SPI2X);
			break;
			case 5:	//DIV64
			SPCR &= ~(1<<SPR0);
			SPCR |= (1<<SPR1);
			SPSR &= ~(1<<SPI2X);
			break;
			case 6:	//DIV128
			SPCR |= ((1<<SPR1)|(1<<SPR0));
			SPSR &= ~(1<<SPI2X);
			break;
		}
	}
	else		//IF SLAVE MODE
	{
		DDRB |= (1<<DDB4);	//MISO
		DDRB &= ~((1<<DDB3)|(1<<DDB5)|(1<<DDB2));	//MOSI, SCK, SS
		SPCR &= ~(1<<MSTR);	//SLAVE
	}
	//ENABLE DATA ORDER, CLOCK POLARITY, CLOCK PHASE
	SPCR |= (1<<SPE)|sDataOrder|sClockPolarity|sClockPhase;

}

/*static void spiReceiveWait()
{
	while (!(SPSR & (1<<SPIF)));	//WAIT FOR DATA RECIEVE COMPLETE
}*/

void spiWrite(uint8_t dat)		//WRITE DATA TO SPI BUS
{
	SPDR = dat;
}

unsigned spiDataReady() //CHECKEA SI LOS DATOS ESTAN LISTOS PARA LEERSE
{
	if (SPSR & (1<<SPIF))
	return 1;
	else
	return 0;
}

uint8_t spiRead(void)
{
	while (!(SPSR & (1<<SPIF)));	//ESPERA POR QUE EL RECIBIMIENTO DE DATOS ESTE COMPLETO
	return(SPDR);					//LEE LA INFORMACIÓN RECIBIDA POR EL BUFFER
}
