/* Include *********************************************************************************/
#include <avr/io.h>
#include <compat/deprecated.h>
#define	F_CPU 16000000
#include <util/delay.h>
#include "5110_IO.h"

/* User required */
void delay_ms(unsigned int);
void SystemInitial(void);

/* Tracing */
#define 	START_SW		!(PIND & 0x01)
#define 	LIMIT_SW		!(PIND & 0x02)	
#define 	FREQ_PWM 		20
#define 	PRESCALER 		8
#define 	TOP_PWM 		F_CPU / ( 2 * PRESCALER * FREQ_PWM )
#define 	GROW_SPEED		(TOP_PWM/2 - TOP_PWM/5 )
#define 	TURN_SPEED		( TOP_PWM / 2 - TOP_PWM / 8 )
#define 	TURN_SPEED2		( TOP_PWM / 2 - TOP_PWM / 8 )

int		Sensor(void);
void	SettingLine(void);
void	BackWard(void); 
void 	TurnRight(void);
void 	TurnLeft(void);
void 	TurnOver(void);
void 	StopBreak(void);
void	Stop(void);
unsigned char 	MovePoint(void);
void RountBack(void);


/* Algorithm */
unsigned char prv_walk=0, prv_found=0, now_found=0;
unsigned char rounting_count = 0;
unsigned char rounting[50];
unsigned char rounting_back[50];
unsigned char turnover_num = 0;

/* LCD Debug Mode */
void 	LcdNum(unsigned int);
void 	MakeGraph(void);
void 	Debug(void);
void 	ShowArray(unsigned char*);

/**
  * @brief Main Funtion.
  */
int main(void)
{
	SystemInitial();
	delay_ms(1000);	
	lcd_clear_screen();
	lcd_gotoxy(0,0);
	while(1)
	{		
		now_found = MovePoint();							// Move and Get Intersection Value
		if(now_found == 8)
		{
			break;
		}
		switch(now_found)									// Calculate Rounting							
		{
			case 1:	TurnRight();	// Right 	To 	TurnRight
					rounting[rounting_count++] = 2;
					turnover_num = 0;
					break;
			case 2:	TurnLeft();		// Left 	To	TurnLeft
					rounting[rounting_count++] = 1;
					turnover_num = 0;
					break;
			case 3:	TurnLeft();		// left-T 	To	TurnLeft
					rounting[rounting_count++] = 1;
					turnover_num = 0;
					break;
			case 4:					// Right-T 	To	Forward // SP Case
					rounting[rounting_count++] = 4;
					turnover_num = 0;
					break;
			case 5:	TurnLeft();		// X 		To	TurnLeft
					rounting[rounting_count++] = 1;
					turnover_num = 0;
					break;
			case 6:	TurnLeft();		// T 		To	TurnLeft
					rounting[rounting_count++] = 1;
					turnover_num = 0;
					break;
			case 7:	TurnOver();		// Noline 	To 	Turnover	
					turnover_num++;
					break;
		}
		ShowArray(rounting);
		prv_found = now_found;
		/*	Step Delay	*/		
		delay_ms(50);
	}
	while(1)
	{
		Stop();
		RountBack();
		ShowArray(rounting_back);
		//while(!START_SW);
		LCD5110_LED_HIGH();
		delay_ms(1000);

		/* Backward To Line */
		BackWard();

		/* Run First Step */
		switch(rounting_back[0])
		{
			case 1:	TurnRight();
					break;
			case 2: TurnLeft();
					break;
			case 3: TurnLeft();
					TurnLeft();
					break;
			case 4: TurnRight();
					TurnRight();
					break;
		}
		unsigned int i;
		for(i=1;rounting_back[i] != '\0'; i++ )
		{
			LCD5110_LED_LOW();
			MovePoint();
			LCD5110_LED_HIGH();
			switch(rounting_back[i])
			{
				case 1: TurnLeft();
						break;
				case 2: TurnRight();
						break;
				case 3: 
						break;
				case 4: 
						break;

			}
		}
		MovePoint();	
		delay_ms(200);
		LCD5110_LED_LOW();
		/* Finish */
		while(1)
		{
			Stop();
			delay_ms(100);
		}
	}
	return 0;
}

void RountBack(void){
	unsigned char temp;
	unsigned char i_rount=0;
	unsigned char i_inv=0;

	/* Invert Direction */
	for(i_rount = 0 ; rounting[i_rount] != '\0' ; i_rount++)
	{
		switch(rounting[i_rount])
		{
			case 1 : rounting_back[i_rount] = 2;		// TurnLeft To TurnRight.
					break;	
			case 2 : rounting_back[i_rount] = 1;		// TurnRight To TurnLeft
					break;
			case 3 : rounting_back[i_rount] = 3;		// Forward To Forward
					break;	
			case 4 : rounting_back[i_rount] = 4;		// Forward To Forward // SP Case
					break;
		}
	}
	i_rount--;	
	for(i_inv = 0;i_inv <= i_rount/2; i_inv++)
	{
		temp = rounting_back[i_inv];
		rounting_back[i_inv] = rounting_back[i_rount-i_inv];
		rounting_back[i_rount-i_inv] = temp;
	}
}

/* Function ********************************************************************************/
void TurnRight(void)
{
	PORTB = 0x30;						// turnRight
	OCR1B = TURN_SPEED;					// Left	
	OCR1A = TURN_SPEED;					// Right	
	while(PINC != 43);
	SettingLine();
	/*
	while(PINC != 25);
	PORTB = 0x09;						// TurnLeft
	delay_ms(30);
	PORTB = 0x00;	OCR1B = 0;	OCR1A = 0;
	*/
	delay_ms(300);
}
void TurnLeft(void)
{
	PORTB = 0x09;						// TurnLeft	
	OCR1A = TURN_SPEED;					// Right
	OCR1B = TURN_SPEED;					// Left	
	while(PINC != 7);
	SettingLine();
	/*
	while(PINC != 25);
	PORTB = 0x30;						// TurnRight
	delay_ms(30);
	PORTB = 0x00;	OCR1B = 0;	OCR1A = 0;
	*/
	delay_ms(300);
}
void SettingLine(void)
{							
	while(!((PINC > 24)&&(PINC < 26))){		// Set Direction to Setpoint
		OCR1B = TURN_SPEED2;	OCR1A = TURN_SPEED2;
		if(PINC < 25)
		{
			PORTB = 0x09;	
		}
		else if(PINC > 25)
		{
			PORTB = 0x30;
		}
		delay_ms(15);
	}
	PORTB = 0x00;	OCR1B = 0;	OCR1A = 0;
}

void BackWard(void)
{
	PORTB = 0x28;	OCR1B = (GROW_SPEED);	OCR1A = (GROW_SPEED);	// Backward
	while(!((PINC >= 52)&&(PINC <= 54)));							// Checkline
	PORTB = 0x00;	OCR1B = (0);			OCR1A = (0);			// Stop
	PORTB = 0x11; 	OCR1B = (TOP_PWM);		OCR1A = (TOP_PWM);		// Break
	delay_ms(20);													// Break Time
	PORTB = 0x11; 	OCR1B = (GROW_SPEED);	OCR1A = (GROW_SPEED);	// Forward
	delay_ms(300);													// Forward Time
	PORTB = 0x00;	OCR1B = (0);			OCR1A = (0);			// Break
	SettingLine();
}
void TurnOver(void)
{	
	PORTB = 0x28;	OCR1B = (GROW_SPEED);	OCR1A = (GROW_SPEED);	// Backward
	while(!((PINC >= 52)&&(PINC <= 54)));							// Checkline
	PORTB = 0x00;	OCR1B = (0);			OCR1A = (0);			// Stop
	PORTB = 0x11; 	OCR1B = (TOP_PWM);		OCR1A = (TOP_PWM);		// Break
	delay_ms(20);													// Break Time
	PORTB = 0x11; 	OCR1B = (GROW_SPEED);	OCR1A = (GROW_SPEED);	// Forward
	delay_ms(300);													// Forward Time
	PORTB = 0x00;	OCR1B = (0);			OCR1A = (0);			// Break
	SettingLine();
	delay_ms(300);
	if(turnover_num == 0)
	{
		if(prv_walk == 4)						// Check prv T-Right
		{	
			TurnRight();
			rounting[rounting_count-1] = 2;		// Change Prv Rounting to TurnRight 	
		}
		else if(prv_walk == 3)					// Check prv T-Left
		{
			TurnRight();				
			rounting[rounting_count-1] = 3;		// Change Prv Rounting to Forward 
		}
		else if(prv_walk == 6)					// Check prv T
		{
			TurnLeft();						
			TurnLeft();
			rounting[rounting_count-1] = 2;		// Change Prv Rounting to TurnRight 
		}
		else if(prv_walk == 5)					// Check prv XX
		{
			TurnRight();					
			rounting[rounting_count-1] = 3;		// Change Prv Rounting to Forward 
		}
	}
	else if(turnover_num == 1)
	{
		if(prv_walk == 4)						// Check prv T-Right
		{	
			TurnRight();
			TurnRight();
			rounting[rounting_count-1] = 1;		// Change Prv Rounting to UTurn 	
		}
		else if(prv_walk == 3)					// Check prv T-Left
		{
			TurnLeft();
			TurnLeft();				
			rounting[rounting_count-1] = 2;		// Change Prv Rounting to UTurn 
		}
		else if(prv_walk == 6)					// Check prv T , I'm sure It's not happen.
		{
			TurnLeft();						
			TurnLeft();
			//rounting[rounting_count-1] = 2;		// Change Prv Rounting to TurnRight 
		}
		else if(prv_walk == 5)					// Check prv XX
		{
			TurnRight();					
			rounting[rounting_count-1] = 2;		// Change Prv Rounting to TurnRight 
		}		
	}
}

void StopBreak(void)
{
	PORTB = 0x28;
	OCR1B  = (TOP_PWM);	OCR1A  = (TOP_PWM);
	delay_ms(30);
	PORTB = 0x00;
	OCR1B  = (0);		OCR1A  = (0);
}


void Stop(void)
{
	PORTB = 0x00;
	OCR1B  = (0);
	OCR1A  = (0);
}


/**
  * @brief move to point
  */
unsigned char MovePoint(void)
{
	unsigned char return_value = 0;
	char rightFlag = 0;
	char leftFlag = 0;
	char tFlag = 0;	
	int erp = 0;												
	int er = 0;													// Error Value
	int pi = 0;													// Output Value
	int kp = 600;
	while(1)
	{
		if(LIMIT_SW)
		{
			StopBreak();
			delay_ms(200);
			return_value = 8;
			break;
		}
		er = Sensor();											// Get Error From Sensor
		if((er>=-15)&&(er <= 15))								// Check Error for Forward
		{
			pi = er*kp;											// Controller
			PORTB = 0x11;										// Forward
			if((er >= -20)&&(er <= 20))
			{
				OCR1B = (GROW_SPEED + pi);	
				OCR1A = (GROW_SPEED - pi);	
			}
			else
			{
				OCR1B = (GROW_SPEED);
				OCR1A = (GROW_SPEED);	
			}
			erp = er;
		}
		else if((PINC >= 51)&&(PINC <= 54))						// Special Case
		{
			/*		First Check Condition		*/
			delay_ms(60);//50										// Decrease Position Error
			if(PINC == 52)										// Check T
			{													
				tFlag = 1;
			}
			else if(PINC == 54)									// Check Right
			{					
				rightFlag = 1;
			}
			else if(PINC == 53)									// Check Left
			{					
				leftFlag = 1;
			}
			else												// Check NoLine
			{		
										
			}
			OCR1B = GROW_SPEED;		OCR1A = GROW_SPEED;			// Forward
			delay_ms(220);//										// Forward Time

			/*		Second Check Condition		*/
			if((rightFlag == 1)&&(PINC == 51))							// Check Right Intersection
			{							
				prv_walk = 1;
				return_value = 1;
			}
			else if((leftFlag == 1)&&(PINC == 51))						// Check Left Intersection
			{						
				prv_walk = 2;
				return_value = 2;
			}
			else if((leftFlag == 1)&&((PINC >= 15)&&(PINC <= 35)))		// Check T-Left Intersection
			{		
				prv_walk = 3;
				return_value = 3;
			}
			else if((rightFlag == 1)&&((PINC >= 15)&&(PINC <= 35)))		// Check T-Right Intersection
			{	
				prv_walk = 4;
				return_value = 4;
			}
			else if((tFlag == 1)&&((PINC >= 15)&&(PINC <= 35)))			// Check X Intersection
			{	
				prv_walk = 5;
				return_value = 5;
			}
			else if((tFlag == 1)&&(PINC == 51))							// Check T Intersection
			{						
				prv_walk = 6;
				return_value = 6;
			}
			else if(PINC == 51)											// Check Noline
			{									
				return_value = 7;
			}
			StopBreak();			// Break											
			delay_ms(100);			// delay for each mission
			break;
		}
		/* IF it's BOX */
		delay_ms(20);		// Sampling Time
	}
	return return_value;
}

void ShowArray(unsigned char* data)
{
	unsigned char i;
	lcd_clear_screen();
	for( i=0 ; *(data+i)!='\0'; i++)
	{
		lcd_put_char(*(data+i)+48);
		lcd_put_char(',');	
	}	
}


























/* Initial Function ------------------------------------------------------------------------------*/
void SystemInitial(void)
{
	// Motor
	DDRB = 0x3F;
	PORTB = 0x00;	
	
	cbi(PORTB,1);
	cbi(PORTB,2);
	
	TCCR1A = 0xA2;
	TCCR1B = 0x11;

	ICR1 = TOP_PWM;
	
	// Button
	cbi(DDRD,0);
	cbi(PORTD,0); 
	
	// Sensor
	DDRC = 0x00;

	// LCD
	init_5110_IO();
	Debug();
	// RUN
	lcd_gotoxy(20,2);
	lcd_print_string("RUNNING", 7);
	LCD5110_LED_LOW();

}


void LcdNum(unsigned int num)
{
	/*lcd_put_char(num/1000+48);
	num = num%1000;
	lcd_put_char(num/100+48);
	num = num%100;*/
	lcd_put_char(num/10+48);
	num = num%10;
	lcd_put_char(num+48);	
}


int Sensor()
{
	int sense = 0;
	sense = PINC - 25;
	return sense;	
}

void run(int l,int r)
{
	// Left
	if(l >= 0){
		sbi(PORTB,4);
		cbi(PORTB,3);
		OCR1B = (1000 - l)*TOP_PWM/1000; // Left
	}else{
		cbi(PORTB,4);
		sbi(PORTB,3);
		OCR1B = (1000 - (-1*l))*TOP_PWM/1000; // Left
	}
	// Right
	if(r >= 0){
		sbi(PORTB,0);
		cbi(PORTB,5);
		OCR1A = (1000 - r)*TOP_PWM/1000; // Right		
	}else{
		cbi(PORTB,0);
		sbi(PORTB,5);
		OCR1A = (1000 - (-1*r))*TOP_PWM/1000; // Right	
	}
}


void delay_ms(unsigned int i)
{
	while(i-->0){
		_delay_ms(1);
	}
}
void MakeGraph(void)
{
			// MakeGraph

			if((PINC >=5)&&(PINC <= 45)){
				lcd_gotoxy(15 + PINC,5);
				lcd_print_string("^", 1);
			}
			// No Line Detect
			else if(PINC ==51 ){
				lcd_gotoxy(40,5);
				lcd_print_string(" ", 1);
			}
			// T-Intersection
			else if(PINC == 52){
				lcd_gotoxy(40,5);
				lcd_print_string("T", 1);	
			}		
			// Left-Intersection
			else if(PINC == 53){
				lcd_gotoxy(40,5);
				lcd_print_string("#", 1);	
				lcd_gotoxy(35,5);
				lcd_print_string("<", 1);
			}
			// Right-Intersection
			else if(PINC == 54){
				lcd_gotoxy(40,5);
				lcd_print_string("#", 1);	
				lcd_gotoxy(45,5);
				lcd_print_string(">", 1);
			}
			// Memo Black
			else if(PINC == 56){
				lcd_gotoxy(13,5);
				lcd_print_string("MEMO BLACK", 10);
			}
			// Memo White
			else if(PINC == 57){
				lcd_gotoxy(13,5);
				lcd_print_string("MEMO WHITE", 10);
			}
			// Data Overflow
			else{
				lcd_gotoxy(35,5);
				lcd_print_string("#", 1);	
				lcd_gotoxy(40,5);
				lcd_print_string("#", 1);
				lcd_gotoxy(45,5);
				lcd_print_string("#", 1);
			}
			lcd_gotoxy(0,5);
			lcd_print_string("<", 1);				
			lcd_gotoxy(40,4);
			lcd_print_string("_", 1);
			lcd_gotoxy(79,5);
			lcd_print_string(">", 1);
			// END
}

void Debug(void)
{
	lcd_clear_screen();
	while(!START_SW){		
			lcd_gotoxy(7,0);
			if(LIMIT_SW)
			{
				lcd_print_string("BOX",3);
			}
			lcd_gotoxy(12,1);
			lcd_print_string(">>> ", 4);
			LcdNum(PINC);
			lcd_print_string(" <<<", 4);
			MakeGraph();
			delay_ms(100);
			lcd_clear_screen();
	}
}
