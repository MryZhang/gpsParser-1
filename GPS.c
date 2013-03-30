//#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "SPI.h"

#define F_CPU 16000000UL
#define USART_BAUDRATE 9600
#define UBRR_VALUE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
//define max buffer size
#define BUF_SIZE 300



// /**************************************************************************************************
//  Initializes the SPI interface as a slave with interrupts with the following options:
// 		-MSB or LSB first
// **************************************************************************************************/
// void Initialize_SPI_Slave(unsigned int dataMode){
// 	
// 	SPCR |= (1 << SPE); //Enable SPI
// 	SPCR |= (1 << SPIE); //Enable SPI interrupts
// 	//SPCR |= (1 << MSTR); //set to SPI master
// 	SPCR |= (dataMode << DORD); //LSB or MSB first
// 	DDRB |= (1 << DDB4); //set MISO (PB4) as an output, all others are inputs
// 	SPDR = 0; //clear out the SPI data register before it is used	
// }
// 
// //*****************************************************************************************************
// // Receives a byte over SPI and returns it
// //*****************************************************************************************************
// 
// char SPI_Slave_Receive(void){
// 	
// 	while(!(SPSR & (1<<SPIF))) {}; // Wait for reception to finish
// 
// 	return SPDR; //return data in register
// }


//type definition of buffer structure
typedef struct{
	//Array of chars
	uint8_t buffer[BUF_SIZE];
	//array element index
	uint8_t index;
}u8buf;

// typedef struct{
// 	//Array of chars
// 	char* [];
// 	//array element index
// 	uint8_t index;
// }gps_packet;

//declare buffer

u8buf buf;
uint8_t new_line = 0;


//initialize buffer
void BufferInit(u8buf *buf)
{
	//set index to start of buffer
	buf->index=0;
}


//write to buffer routine
uint8_t BufferWrite(u8buf *buf, uint8_t u8data)
{
	if (buf->index<BUF_SIZE)
	{
		buf->buffer[buf->index] = u8data;
		//increment buffer index
		buf->index++;
		return 0;
	}
	else return 1;
}
uint8_t BufferRead(u8buf *buf, volatile uint8_t *u8data)
{
	if(buf->index>0)
	{
		buf->index--;
		*u8data=buf->buffer[buf->index];
		return 0;
	}
	else return 1;
}


void USART_send( unsigned char data)
{
	
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
	
}

void USART_putstring(char* StringPtr, int length){
	
	for(int i=0; i < length; i++)		 
		{					  
			USART_send(StringPtr[i]);
		}								 
}



// char* Build_Data(u8buf *buf)
// {
// 	int length = buf->index;
// 	
// 	char* Data = malloc(buf->index);
// 	
// 	int i;
// 	
// 	for(i=0;i<length;i++)
// 	{
// 		Data[i]=buf->buffer[i];	
// 	}
// 	
// 	return Data;
// }

char* Build_Data(u8buf *buf)
{
	int length = buf->index;
	
	char* Data = malloc(buf->index);
	
	int i;
	
	for(i=0;i<length;i++)
	{
		Data[i]=buf->buffer[i];	
	}
	
	return Data;
}



void USART0Init(void)
{
	// Set baud rate
	UBRR0H = (uint8_t)(UBRR_VALUE>>8);
	UBRR0L = (uint8_t)UBRR_VALUE;
	// Set frame format to 8 data bits, no parity, 1 stop bit
	UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);
	//enable reception and RC complete interrupt
	UCSR0B |= (1<<RXEN0)|(1<<RXCIE0);
}
//RX Complete interrupt service routine
ISR(USART_RX_vect)
{
	uint8_t u8temp;
	u8temp=UDR0;
	
	if(u8temp == 76)
	{
		new_line = 1;					
	}
	
	
	if(new_line == 1)
	{
		//check if period char or end of buffer
		if ((BufferWrite(&buf, u8temp)==1)||(u8temp==10))
		{
			
			//We have all the data for one sample.
			new_line = 0;
						
			//disable reception and RX Complete interrupt to process the data received
			UCSR0B &= ~((1<<RXEN0)|(1<<RXCIE0));

			char* Returned_Data	= Build_Data(&buf);
			
			char* Processed_Data = malloc(buf.index+50);			
					
			parseGpsMsg(Returned_Data,Processed_Data, buf.index);	
			
			USART_putstring(Processed_Data, 29); // Do something with the Data.
			
			free(Returned_Data);
			free(Processed_Data);
						
			BufferInit(&buf);
			
			//enable reception and RC complete interrupt
			UCSR0B |= (1<<RXEN0)|(1<<RXCIE0);
			
		}
	}	
	
}
//UDR0 Empty interrupt service routine
ISR(USART_UDRE_vect)
{
	//if index is not at start of buffer
	if (BufferRead(&buf, &UDR0)==1)
	{
		//start over
		//reset buffer
		BufferInit(&buf);
		//disable transmission and UDR0 empty interrupt
		UCSR0B &= ~((1<<TXEN0)|(1<<UDRIE0));
		
		//enable reception and RC complete interrupt
		UCSR0B |= (1<<RXEN0)|(1<<RXCIE0);
	}
}


typedef enum { false, true } bool;

double toDouble(char* s, int start, int stop) {
    int m = 1;
    int i = 0;
    double ret = 0;
    for (i = stop; i >= start; i--) {
        ret += (s[i] - '0') * m;
        m *= 10;
    }
    return ret;
}


double toDoubleTenths(char* s, int start, int stop) {
    int m = 1;
    int i = 0;
    char newChar[] = ".";
    for (i = start; i <= stop; i++) {
        newChar[m] = s[i];
        m++;
    }
    return atof(newChar);
}


double getSign(char sign_char) {
    switch(sign_char){
      case 'N':
        return 1.0;
      case 'S':
        return -1.0;
      case 'W':
        return -1.0;
      case 'E':
        return 1.0;
      default:
        printf("%s\n", "BROKEN");
      return 0.0;
	  ;
    }
}


void parseGpsMsg(char* in_string, char* out_string, int str_length) 
{
	USART_putstring(in_string, str_length); // Do something with the Data.
	
  double lat_value = 0.0;
  double lon_value = 0.0;
  double temp_lon = 0.0;
  double temp_lat = 0.0;
  int in_size = str_length;
  int i = 0;
  bool reading = false;
  int value_count = 0;
  int value_start = 0;
  int value_seconds = 0;
  int value_end = 0;
  double sign = 0.0;
  
  for(i = 0; i <= in_size; i++) 
  {
	USART_putstring(in_string,str_length);  

	char* temp_pt = &(in_string[i]);
	USART_putstring(temp_pt,1); // Do something with the Data.
				
    if(value_count < 2) 
	{
      switch(in_string[i])
	  {
        case ',':
          if(!reading) 
		  {
            reading = true;
            value_start = i+1;
          }
          else 
		  {
            reading = false;
            value_end = i;
            double sign = getSign(in_string[i+1]);
            if(value_count == 0) 
			{
              temp_lat += toDoubleTenths(in_string, value_seconds, i-1);
              lat_value += temp_lat/60.0;
              lat_value *= sign;
            }
            else 
			{
              temp_lon += toDoubleTenths(in_string, value_seconds, i-1);
              lon_value += temp_lon/60.0;
              lon_value *= sign;
            }
            value_count++;
          }
          break;
        case '.':
          if(reading) 
		  {
            if(value_count == 0) 
			{
              lat_value += toDouble(in_string, value_start, i-3);
              temp_lat += toDouble(in_string, i-2, i-1);
            }
            else 
			{
              lon_value += toDouble(in_string, value_start, i-3);
              temp_lon += toDouble(in_string, i-2, i-1);
            }
            value_seconds = i+1;
          }
          break;
        default :
            ;
      }
    }
  }
  char message[in_size];
  sprintf(message, "LAT=%f LON=%f", lat_value, lon_value);
  for(i = 0; i <= 29; i++) {
    out_string = message[i];
	out_string++;
  }
  
}

// ISR(SPI_STC_vect) 
// {
// 
// }

int main (void)
{
	//Init buffer
	BufferInit(&buf);
	
//	Initialize_SPI_Slave(LSB);


	//set sleep mode
	set_sleep_mode(SLEEP_MODE_IDLE);
	//Initialize USART0
	USART0Init();
	
	//enable global interrupts
	sei();
	
	while(1)
	{
		//put MCU to sleep
		sleep_mode();
	}
}

