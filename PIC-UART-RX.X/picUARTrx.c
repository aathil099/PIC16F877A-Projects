/*
 * File:   picUARTrx.c
 * Author: aathi
 * Created on December 16, 2024, 1:33 PM
 */
#include <stdlib.h>
#include <stdio.h>

#include "CONFIG.h"
#define RS RB0
#define EN RB1
#define D4 RB2
#define D5 RB3
#define D6 RB4
#define D7 RB5

char s[20];

void Lcd_Port(char a) {
	if(a & 1)
		D4 = 1;
	else
		D4 = 0;
 
	if(a & 2)
		D5 = 1;
	else
		D5 = 0;
 
	if(a & 4)
		D6 = 1;
	else
		D6 = 0;
 
	if(a & 8)
		D7 = 1;
	else
		D7 = 0;
}

void Lcd_Cmd(char a){
	RS = 0;             // => RS = 0
	Lcd_Port(a);
	EN  = 1;             // => E = 1
    __delay_ms(4);
    EN  = 0;             // => E = 0
}

void Lcd_Clear(){
	Lcd_Cmd(0);
	Lcd_Cmd(1);
}

void Lcd_Set_Cursor(char a, char b){
	char temp,z,y;
	if(a == 1){
	  temp = 0x80 + b - 1;
		z = temp>>4;
		y = temp & 0x0F;
		Lcd_Cmd(z);
		Lcd_Cmd(y);
	}
	else if(a == 2){
		temp = 0xC0 + b - 1;
		z = temp>>4;
		y = temp & 0x0F;
		Lcd_Cmd(z);
		Lcd_Cmd(y);
	}
    // Add support for the third line
    else if (a == 3){
        temp = 0x90 + b - 1;
        z = temp >> 4;
        y = temp & 0x0F;
        Lcd_Cmd(z);
        Lcd_Cmd(y);
    }
    // Add support for the fourth line
    else if (a == 4){
        temp = 0xD0 + b - 1;
        z = temp >> 4;
        y = temp & 0x0F;
        Lcd_Cmd(z);
        Lcd_Cmd(y);
    }

}

void Lcd_Init(){
    Lcd_Port(0x00);
    __delay_ms(20);
    Lcd_Cmd(0x03);
	__delay_ms(5);
    Lcd_Cmd(0x03);
	__delay_ms(11);
    Lcd_Cmd(0x03);
    Lcd_Cmd(0x02);
    Lcd_Cmd(0x02);
    Lcd_Cmd(0x08);
    Lcd_Cmd(0x00);
    Lcd_Cmd(0x0C);
    Lcd_Cmd(0x00);
    Lcd_Cmd(0x06);
}

void Lcd_Write_Char(char a){
   char temp,y;
   temp = a&0x0F;
   y = a&0xF0;
   RS = 1;             // => RS = 1
   Lcd_Port(y>>4);             //Data transfer
   EN = 1;
   __delay_us(40);
   EN = 0;
   Lcd_Port(temp);
   EN = 1;
   __delay_us(40);
   EN = 0;
}

void Lcd_Write_String(char *a){
	int i;
	for(i=0;a[i]!='\0';i++)
	   Lcd_Write_Char(a[i]);
}

void Lcd_Shift_Right(){
	Lcd_Cmd(0x01);
	Lcd_Cmd(0x0C);
}

void Lcd_Shift_Left(){
	Lcd_Cmd(0x01);
	Lcd_Cmd(0x08);
}

void LCD(){
    unsigned int a;
    TRISB = 0x00;   
}


void UART_Init_Rx() {

    TXSTAbits.CSRC=0;
    TXSTAbits.TX9=0;
    TXSTAbits.TXEN=1;
    TXSTAbits.SYNC=0;
    TXSTAbits.BRGH=1;
    TXSTAbits.TRMT=0;
    TXSTAbits.TX9D=0;
        
    RCSTAbits.SPEN=1;   // Enable serial port
    RCSTAbits.RX9=0;
    RCSTAbits.SREN=0;
    RCSTAbits.CREN=1;   // Enable continuous receive
    RCSTAbits.ADDEN=0;
    RCSTAbits.FERR=0;
    RCSTAbits.OERR=0;
    RCSTAbits.RX9D=0;
    
    SPBRG = 129;          // Baud rate
    TRISCbits.TRISC7 = 1; // RX pin as input 
    TRISCbits.TRISC6 = 0; // TX pin as output 

}

char UART_Read() {
    while (!PIR1bits.RCIF); // Wait for data to be received
    return RCREG;
}

void UART_Read_Text(char *buffer) {
    char received_char;
    int i = 0;
    
    while (1) {
        received_char = UART_Read(); // Read a character
        if (received_char == 13) {   // Carriage return as end of string
            buffer[i] = '\0';        // Null-terminate the string
            break;
        }
        buffer[i++] = received_char;
    }
}

char receivedData[20];
unsigned int received_value;
void main() {
    
    LCD();
    Lcd_Init();
    UART_Init_Rx();
    Lcd_Clear();
    while (1) {
        unsigned int adcValues[4];
        char adcDisplay[4][20];
        
        for(int i=0; i<4; i++){
            unsigned char high_byte = UART_Read();
            unsigned char low_byte = UART_Read();
            // Combine high and low bytes to form the ADC value
            adcValues[i] = (high_byte << 8) | low_byte;
            sprintf(adcDisplay[i], "ADC%d=%u", i, adcValues[i]);
        }
        
        //Lcd_Clear();
        Lcd_Set_Cursor(1, 1);
        Lcd_Write_String(adcDisplay[0]);
        Lcd_Set_Cursor(2, 1);
        Lcd_Write_String(adcDisplay[1]);
        Lcd_Set_Cursor(3, 1);
        Lcd_Write_String(adcDisplay[2]);
        Lcd_Set_Cursor(4, 1);
        Lcd_Write_String(adcDisplay[3]);
        
        __delay_us(100); 
    }
}


