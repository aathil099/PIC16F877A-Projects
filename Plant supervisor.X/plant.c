#include "config.h"
#include <stdio.h>
#include <stdlib.h>

#define RS RB0
#define EN RB1
#define D4 RB2
#define D5 RB3
#define D6 RB4
#define D7 RB5

char s[20];

unsigned int soil_moisture_value;
unsigned int water_level;
unsigned int temperature_ADC;

float soilMoisture;
float temperature;
float water;

float soil_moisture_percent;
float water_level_percent;
float temperature_celsius;
float temp_volt;

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

void main(void) {
	TRISB = 0x00; // LCD port selected as output
    LCD();
    Lcd_Init();
	// Selected as analog pins
	TRISA0 = 1;
	TRISA1 = 1;
	TRISA2 = 1;
	ADCInit();
    TRISD2 = 0;
    
	while (1) {
		soil_moisture_value = ADCRead(0);
        water_level = ADCRead(1);
		temperature_ADC = ADCRead(2);
        
        soil_moisture_percent = (100.0 * soil_moisture_value) / 1024.0;
		water_level_percent = (100.0 * water_level) / 1024.0;
        temp_volt = (temperature_ADC / 1024.0) * 5.0; // Vref+ = 5V
		temperature_celsius = (temp_volt - 0.5) * 10.0; // temperature sensor output is 10 mV/C
        
		Lcd_Clear();
		Lcd_Set_Cursor(1, 1);
        sprintf(s,"Soil: %.1f %%",soil_moisture_percent);
		Lcd_Write_String(s);
		
        sprintf(s, "Water: %.2f%%", water_level_percent);
		Lcd_Set_Cursor(2, 1);
		Lcd_Write_String(s);

		sprintf(s, "Temp: %.2f C", temperature_celsius);
		Lcd_Set_Cursor(3, 1);
		Lcd_Write_String(s);
        
        Lcd_Set_Cursor(4, 1);
		Lcd_Write_String("aathil");
        __delay_ms(30);
        
        if (water_level_percent<50){
            PORTDbits.RD2=1;
        }else{
            PORTDbits.RD2=0;
        }                
	}
	return;
}
