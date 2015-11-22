/*
		Data Export
			Left 				 	= 5 - 24
			Center 				 	= 25
			Right 				 	= 26 - 45
			No Line					= 51
			T - Intersection 	 	= 52
			Left - Intersection  	= 53
			Right - Intersection 	= 54 	
			Center Check			= 55
		Command = 55 - 63
			56 	= 	Memory Setpoint
			57 	= 	Momory Errorpoint 

		setpoint = Blackline;
		errorpoint = whiteline;

*/
/*--- Include ---*/
#include <avr/io.h>
#define F_CPU 8000000
#include <util/delay.h>
#include <compat/deprecated.h>
#include <avr/eeprom.h>

//#define white_sp_enable
#define resol	10

/*--- Memory ---*/
#define Sp_Address	(0x0000+1)
#define Ep_Address	(0x0000+20)

/*--- Button ---*/
#define ButtonPIN	PINB
#define ButtonDDR	DDRB
#define ButtonPORT	PORTB
#define	Button0		!(ButtonPIN&0x01)					//Connect to SW3 on Sensor-Board
#define Button1		!(ButtonPIN&0x02)					//Connect to SW2 on Sensor-Board

/*--- Data ---*/
#define DataDDR		DDRD
#define DataPORT	PORTD

/*--- Span & Zero ---*/


unsigned int uiADCData[8];
unsigned int meanb0;
unsigned int meanb1;
unsigned char wh_sp_flag = 0;
unsigned char setpoint[8];
unsigned char errorpoint[8];
unsigned char i = 0;
unsigned int m[8];
unsigned int b[8];
int y[8];
int center = 0;

void delay_ms(uint32_t i){
	while(i-->0){
		_delay_ms(1);
	}
}

void InitADC(void){										

	ADCSRA 	= (1 << 7)|(0 << 5);							//ADEN,ADATE
	ADCSRA |= (0 << ADPS2)|(1 << ADPS1)|(1 << ADPS0);

}

unsigned int ReadADC(volatile unsigned char ucMUX){		//With this InitADC and ReadADC you will got 10 bits of ADC data.

	ADMUX = (0 << REFS1)|(1 << REFS0)|(ucMUX);
	ADCSRA |= (1 << ADSC);
	while(!(ADCSRA&(1 << ADIF)));

	return ADCW;

}

void GetAllADCData(unsigned int* adc){			//If you want ADCData ,get it from uiADCDAta variable ,don't directly call ReadADC(x).

	*(adc + 0) = ReadADC(0)/10;
	*(adc + 1) = ReadADC(1)/10;
	*(adc + 2) = ReadADC(2)/10;
	*(adc + 3) = ReadADC(3)/10;
	*(adc + 4) = ReadADC(4)/10;
	*(adc + 5) = ReadADC(5)/10;
	*(adc + 6) = ReadADC(7)/10;	//	T_T My PCB designed mistake.
	*(adc + 7) = ReadADC(6)/10;	//
}

unsigned int EEPROMReadWord(volatile unsigned int uiAddress){

	volatile unsigned char ucBuffer;
	volatile unsigned int uiBuffer;

	uiBuffer = eeprom_read_byte((void *)(uiAddress+1));
	uiBuffer <<= 8;
	ucBuffer = eeprom_read_byte((void *)uiAddress);
	uiBuffer |= ucBuffer;

	return uiBuffer;

}

void EEPROMWriteWord(volatile unsigned int uiAddress,volatile unsigned int uiData){

	eeprom_write_byte((void *)uiAddress,(char)uiData);
	eeprom_write_byte((void *)(uiAddress+1),(char)(uiData>>8));

}

void startup(void){
	unsigned char i = 0;
	for(i = 0 ; i <=7 ;i++){
		setpoint[i] = EEPROMReadWord(Sp_Address + (i*2));
		errorpoint[i] = EEPROMReadWord(Ep_Address + (i*2));
		m[i] = (resol)*(1000)/(setpoint[i] - errorpoint[i]);
		b[i] = m[i]*(errorpoint[i] - 1)/1000;
		//m[i] = (resol)*(100)/(errorpoint[i] - setpoint[i]);
		//b[i] = m[i]*errorpoint[i];
	}
}

/**
  *  Main Func.
  */
int main(void){
	DataDDR = 0xFF;
	DataPORT = 0x00;
	ButtonDDR = 0x00;
	ButtonPORT = 0xFF;

	InitADC();
	
	startup();
	while(1){
		// Memo Session 
		if(Button0&&Button1){
			char bi = 0;
			do{
				DataPORT = 3-(bi/10) ;
				delay_ms(100);
				bi++;
			}while((bi<=30)&&(Button0)&&(Button1));
			while((Button0)&&(Button1)){
				delay_ms(500);
			}
			if(bi >= 30){
				// SetPoint
				do{
					DataPORT = 56;
					GetAllADCData(uiADCData);
				}while(!Button0);
				for(i = 0; i<=7 ;i++){
					EEPROMWriteWord(Sp_Address + (i*2),uiADCData[i]);	
				}
				// ErrorPoint
				do{
					DataPORT = 57;
					GetAllADCData(uiADCData);
				}while(!Button1);
				for(i = 0; i<=7 ;i++){
					EEPROMWriteWord(Ep_Address + (i*2),uiADCData[i] + 10);	
				}
			}
			// Start Up
			startup();
			
		}else{		
			// update status	
			GetAllADCData(uiADCData);
			for(i = 0; i<=7 ;i++){
				y[i] = (m[i]*uiADCData[i]/1000) - b[i];
				// inner hole
				if(y[i] > resol)
					y[i] = resol;
				else if(y[i] < 0)
					y[i] = 0;
			}
			center = (y[5] - y[4]) - (y[2] - y[3]) + ((y[4] - y[3])*2);
			// For Sensor Out Of Line
			if((y[2] >= 9)||(y[1] >= 1)){
				center = - 18;
			}
			if((y[5] >= 9)||(y[6] >= 1)){
				center = 18;
			}
			// negative data to positive data
 			center = center + 25;
			if((y[0]+y[2]+y[3]+y[4]+y[5]+y[7]) >= 65){
				DataPORT = 0;		// Over Flow Data
			}else if((y[0] >= 8)&&(y[7] >= 8)){
				DataPORT = 52;		// T - Intersection 	 	= 52
			}else if(y[0] >= 8){
				DataPORT = 53;		// Left - Intersection  	= 53
			}else if(y[7] >= 8){
				DataPORT = 54;		// Right - Intersection 	= 54 
			}else if((y[0]+y[2]+y[3]+y[4]+y[5]+y[7]) <= 2){
				DataPORT = 51;		// No Line					= 51	
			}else{
				DataPORT = center;	// Normal Data
			}
		}
	}
	return 0;
}
