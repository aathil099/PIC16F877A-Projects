/*
 * File:   picUART.c
 * Author: aathi
 * Created on December 15, 2024, 9:13 PM
 */
#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#define RS RB0
#define EN RB1
#define D4 RB2
#define D5 RB3
#define D6 RB4
#define D7 RB5

char s[20],s1[20],s2[20],s3[20];
int val,val2,val3,val4;

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

void ADCInit(){
	ADCON0bits.ADCS0 = 1;
	ADCON0bits.ADCS1 = 0;
	ADCON1bits.ADCS2 = 1;

	ADCON1bits.ADFM = 1;
	ADCON1bits.PCFG0 = 0;
	ADCON1bits.PCFG1 = 0;
	ADCON1bits.PCFG2 = 0;
	ADCON1bits.PCFG3 = 0;

	ADCON0bits.ADON = 1;
	ADCON0bits.GO_nDONE = 1;
    TRISA0 = 1;
	TRISA1 = 1;
	TRISA2 = 1;
}

int ADCRead(int x){
	switch (x) {
	case 0:
		ADCON0bits.CHS = 0b0000;
		break;

	case 1:
		ADCON0bits.CHS = 0b0001;
		break;

	case 2:
		ADCON0bits.CHS = 0b0010;
		break;

	case 3:
		ADCON0bits.CHS = 0b0011;
		break;

	case 4:
		ADCON0bits.CHS = 0b0100;
		break;

	case 5:
		ADCON0bits.CHS = 0b0101;
		break;
            
	default:
		break;
	}
	__delay_us(100); // Delay a bit to allow channel selection

	GO_nDONE = 1;     // Start the conversion
	while (ADCON0bits.GO_nDONE); // Wait for the conversion to complete

	return ((unsigned int)(ADRESH << 8) | ADRESL);
}

void UART_int() {
           
    TRISCbits.TRISC6 = 0; // TX pin as output 
    TRISCbits.TRISC7 = 1; // RX pin as input 
    
    TXSTAbits.CSRC=0;
    TXSTAbits.TX9=0;
    TXSTAbits.TXEN=1;
    TXSTAbits.SYNC=0;
    TXSTAbits.BRGH=1;
    TXSTAbits.TRMT=0;
    TXSTAbits.TX9D=0;
    
    RCSTAbits.SPEN=1;
    RCSTAbits.RX9=0;
    RCSTAbits.SREN=0;
    RCSTAbits.CREN=1;
    RCSTAbits.ADDEN=0;
    RCSTAbits.FERR=0;
    RCSTAbits.OERR=0;
    RCSTAbits.RX9D=0;
    
    SPBRG = 129;
}

void UART_Write(char data) {
    while (!TXSTAbits.TRMT); // Wait until transmit register is empty
    TXREG = data;            // Load data into transmit register
}

void UART_Write_Text(char *text) {
    int i = 0;
    while (text[i] != '\0') {  // Transmit until null terminator
        UART_Write(text[i]);
        i++;
    }
}

void main(void) {
    UART_int();
    LCD();
    Lcd_Init();
    ADCInit();
    Lcd_Clear();
    while (1) {
        val=  ADCRead(0);
        val2= ADCRead(1);
        val3= ADCRead(2);
        val4= ADCRead(3);
        
        //Lcd_Clear();
		Lcd_Set_Cursor(1, 1);
        sprintf(s,"V1=%d",val);
		Lcd_Write_String(s);
        Lcd_Set_Cursor(2, 1);
        sprintf(s1,"V2=%d",val2);
		Lcd_Write_String(s1);
        Lcd_Set_Cursor(3, 1);
        sprintf(s2,"V3=%d",val3);
		Lcd_Write_String(s2);
        Lcd_Set_Cursor(4, 1);
        sprintf(s3,"V4=%d",val4);
		Lcd_Write_String(s3);
        
        //UART_Write_Text(s); // Send ADC value through UART
		//UART_Write(13);     // Send carriage return as terminator
        UART_Write(val >> 8);     // Send high byte
        UART_Write(val & 0xFF);  // Send low byte
        
        UART_Write(val2 >> 8);     // Send high byte
        UART_Write(val2 & 0xFF);  // Send low byte
        
        UART_Write(val3 >> 8);     // Send high byte
        UART_Write(val3 & 0xFF);  // Send low byte
        
        UART_Write(val4 >> 8);     // Send high byte
        UART_Write(val4 & 0xFF);  // Send low byte
        
        __delay_us(100);
    }
    return;
}
