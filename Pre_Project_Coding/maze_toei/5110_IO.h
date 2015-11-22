#include <stdlib.h>  //for founction itoa 

/*********************************************/
/* Example Program For ET-AVR STAMP ATMEGA64 */
/* MCU      : ATMEGA64(XTAL=16 MHz)          */
/*          : Frequency Bus = 16 MHz         */
/* Compiler : CodeVisionAVR 1.24.8d          */
/* Write By : Adisak Choochan(ETT CO.,LTD.)  */
/* Function : Demo Interface LCD Nokia-5110  */ 
/*          : Use Pin I/O Generate SPI Signal*/
/*********************************************/
/* Interface LCD NOKIA-5110 By AVR Function  */
/* -> ATMEGA64   --> LCD Nokia-5110          */
/* -> PB0(I/O)   --> SCE(Active "0")         */
/* -> PB1(I/O)   --> RES(Active "0")         */
/* -> PB2(I/O)   --> D/C(1=Data,0=Command)   */
/* -> PB3(I/O)   --> SDIN                    */
/* -> PB4(I/O)   --> SCLK                    */
/* -> PB5(I/O)   --> LED(Active "1")         */
/*********************************************/
#include "font.h"
/*************************************/
/* ATMEGA64 Interface LCD Nokia-5110 */
/* -> ATMEGA64   --> LCD Nokia-5110  */
/* -> PB0(I/O)   --> SCE(Active "0") */
/* -> PB1(I/O)   --> RES(Active "0") */
/* -> PB2(I/O)   --> D/C("1"=Data    */
/*                       "0"=Command)*/
/* -> PB3(I/O)   --> SDIN            */
/* -> PB4(I/O)   --> SCLK            */
/* -> PB5(I/O)   --> LED(Active "1") */
/*************************************/

// Define LCD Nokia-5110 PinI/O Interface Mask Bit 
#define  LCD5110_SCE_HIGH()  	PORTD |= 0b00000100	    // SCE(PB0) = '1'(Disable)  
#define  LCD5110_SCE_LOW()  	PORTD &= 0b11111011 	// SCE(PB0) = '0'(Enable)
#define  LCD5110_RES_HIGH()  	PORTD |= 0b00001000		// RES(PB1) = '1'(Normal) 
#define  LCD5110_RES_LOW()  	PORTD &= 0b11110111		// RES(PB1) = '0'(Reset)
#define  LCD5110_DC_HIGH() 		PORTD |= 0b00010000		// D/C(PB2) = '1'(Data) 
#define  LCD5110_DC_LOW() 		PORTD &= 0b11101111		// D/C(PB2) = '0'(Command)
#define  LCD5110_SDIN_HIGH() 	PORTD |= 0b00100000		// LED(PB3) = '1'(Logic "1") 
#define  LCD5110_SDIN_LOW() 	PORTD &= 0b11011111		// LED(PB3) = '0'(Logic "0")
#define  LCD5110_SCLK_HIGH() 	PORTD |= 0b01000000		// LED(PB4) = '1'(Shift Data) 
#define  LCD5110_SCLK_LOW() 	PORTD &= 0b10111111		// LED(PB4) = '0'(Stand By)
#define  LCD5110_LED_HIGH() 	PORTD |= 0b10000000		// LED(PB5) = '1'(LED ON) 
#define  LCD5110_LED_LOW() 		PORTD &= 0b01111111		// LED(PB5) = '0'(LED OFF)
// End of Define For LCD Nokia-5110

/**************************/
/* Delay SPI Clock Signal */
/**************************/
void SPI_Delay(void)
{
  int x=0;  						// Short Delay Counter
  x++;
  x++;
}

/********************************/
/* Write Data or Command to LCD */
/* D/C = "0" = Write Command    */
/* D/C = "1" = Write Display    */
/********************************/ 
void lcd_write_data(unsigned char DataByte) 
{                  
  unsigned char Bit = 0;				// Bit Counter

  LCD5110_DC_HIGH(); 					// Active DC = High("1"=Data)
        
  for (Bit = 0; Bit < 8; Bit++)		                        // 8 Bit Write
  {                                                                     
    if ((DataByte & 0x80) == 0x80)                                      // MSB First of Data Bit(7..0)
    {
      LCD5110_SDIN_HIGH();                                              // SPI Data = "1"
    }
    else
    {
      LCD5110_SDIN_LOW();                                               // SPI Data = "0"
    }	
    	
    LCD5110_SCLK_HIGH();		   		// Strobe Bit Data

    SPI_Delay();				            // Delay Clock

    LCD5110_SCLK_LOW();  				// Next Clock
    DataByte <<= 1;	 			            // Next Bit Data
  }
}    

/********************************/
/* Write Data or Command to LCD */
/* D/C = "0" = Write Command    */
/* D/C = "1" = Write Display    */
/********************************/ 
void lcd_write_command(unsigned char CommandByte) 
{                
  unsigned char Bit = 0;   				// Bit Counter

  LCD5110_DC_LOW(); 	   				// Active DC = Low("0"=Command)
  
  for (Bit = 0; Bit < 8; Bit++)		                        // 8 Bit Write
  {                                                                     
    if ((CommandByte & 0x80) == 0x80)                                   // MSB First of Data Bit(7..0)
    {
      LCD5110_SDIN_HIGH();                                              // SPI Data = "1"
    }
    else
    {
      LCD5110_SDIN_LOW();                                               // SPI Data = "0"
    }	
    	
    LCD5110_SCLK_HIGH();		   		// Strobe Bit Data

    SPI_Delay();				            // Delay Clock

    LCD5110_SCLK_LOW();  				// Next Clock
    CommandByte <<= 1;	 			            // Next Bit Data
  }
}    

 

/****************************/
/* Clear Screen Display LCD */
/****************************/
void lcd_clear_screen(void)       
{  
  unsigned int  i=0; 					// Memory Display(Byte) Counter
    
  lcd_write_command(128+0);  				// Set X Position = 0(0..83)
  lcd_write_command(64+0);   				// Set Y Position = 0(0..5)
  
  for(i=0;i<504;i++)   	     				// All Display RAM = 504 Byte  
  lcd_write_data(0);  	     				// Clear Screen Display
}   

/***************************/
/* Set Cursor X,Y Position */
/* X[0-83]: 84 Column Data */
/* Y[0-5] : 6 Row(48 Dot)  */
/***************************/
void lcd_gotoxy(unsigned char x,unsigned char y)  
{  
  lcd_write_command(128+x);  				// Set X Position(1+x6,x5,x4,x3,x2,x1,x0)
  lcd_write_command(64+y);   				// Set Y Position(01000+y2,y1,y0)
}  

/***************************/
/* Put Char to LCD Display */
/***************************/
void lcd_put_char(unsigned char character) 
{  
  unsigned char font_size_count = 0; 			// Font Size Counter
  unsigned int  font_data_index;  	 			// Font Data Pointer

  font_data_index = character-32;    			// Skip 0x00..0x1F Font Code
  font_data_index = font_data_index*5;			// 5 Byte / Font       
  
  while(font_size_count<5)                     			// Get 5 Byte Font & Display on LCD
  {  													
    lcd_write_data(tab_font[font_data_index]);  		// Get Data of Font From Table & Write LCD
    font_size_count++;  				// Next Byte Counter
    font_data_index++;  				// Next	Byte Pointer
  }  
  lcd_write_data(0);					// 1 Pixel Dot Space
}    

/*******************************/
/* Print String to LCD Display */
/*******************************/
void lcd_print_string(const char *string , unsigned char CharCount) 
{          
  unsigned char i=0;  					// Dummy Character Count

  while(i<CharCount)  
  {    
    lcd_put_char(string[i]);				// Print 1-Char to LCD
    i++;                           				// Next Character Print
  }  
}

void lcd_print_int(unsigned int number )
{
    char temp[15];
    itoa(number,temp,10);
    lcd_print_string(temp, 4);
}



/**************************/
/* Initial LCD Nokia-5110 */
/**************************/          
void lcd_initial(void)      
{   
  LCD5110_RES_LOW();					// Active Reset
  LCD5110_RES_HIGH();					// Normal Operation

  lcd_write_command(32+1); 				// Function Set = Extend Instruction(00100+PD,V,H=00100+0,0,1)
  lcd_write_command(128+38);				// Set VOP(1+VOP[6..0] = 1+0100110)
  lcd_write_command(4+3);   				// Temp Control(000001+TC1,TC0=000001+1,1)
  lcd_write_command(16+3);  				// Bias System(00010,BS2,BS1,BS0=00010,0,1,1)

  lcd_write_command(32+0);  				// Function Set = Basic Instruction(00100+PD,V,H = 00100+0,0,0)
  lcd_write_command(12);    				// Display Control = Normal Mode(00001D0E=00001100)
}  

void init_5110_IO()
{

  DDRD = 0b11111100;                                                    // PB[7,6] = Input,PB[5..0] = Output     
  
  /* Initial GPIO Signal Interface LCD Nokia-5110 */
  LCD5110_RES_LOW();					// Active Reset
  LCD5110_RES_HIGH();					// Normal Operation  
  LCD5110_DC_HIGH(); 					// D/C = High("1"=Data)
  LCD5110_LED_HIGH();					// LED = High(ON LED)
  LCD5110_SDIN_LOW();                                                   // Standby SPI Data 
  LCD5110_SCLK_LOW();                                                   // Standby SPI Clock
  LCD5110_SCE_LOW();					// SCE = Low(Enable)

  /* Start Initial & Display Character to LCD */
  lcd_initial();                				// Initial LCD
  lcd_clear_screen();              				// Clear Screen Display

}
