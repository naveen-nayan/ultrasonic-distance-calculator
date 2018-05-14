#include<reg51.h>		//Header file inclusion for 8051

#include<intrins.h>    // for using _nop_() function



void delay(unsigned int rtime);
void lcdcmd(unsigned char DATA);
void initialize(void);
void lcddat(unsigned char DATA);
void display_lcd(unsigned char location, unsigned char *d);
void send_pulse(void);
void lcd_number(int val);
void get_range(void);

#define LCDdata P1		//Declaring LCDdata	

sbit trig=P3^5;//timer 1
sbit echo=P3^2;//INTR 0

sbit LCDrs = P2^0;		//The Register select Pin
sbit LCDrw = P2^1;		//The Read/Write Pin
sbit LCDen = P2^2;		//The Enable Pin

void delay(unsigned int rtime)
{
	unsigned int r,s;
	for(r=0;r<rtime;r++)
	for(s=0;s<1275;s++);
}

void lcdcmd(unsigned char DATA)
{
  	LCDrs=0;
	LCDrw=0;
	LCDen=1;			//Strobe the enable pin
	LCDdata = DATA;			//Put the value on the pins
	LCDrs=0;
	LCDrw=0;
	LCDen=0;
}

void initialize(void) 
{
	lcdcmd(0x30);		//1 line and 5x7 matrix 
	delay(1);

	lcdcmd(0x38);		//2 line and 5x7 matrix 
	delay(1);

	lcdcmd(0x0c);		//Display on, cursor off
	delay(1);

	lcdcmd(0x01);		//Clear display Screen 
	delay(1);

	lcdcmd(0x06);		//shift cursor to right
	delay(1);
}

void lcddat(unsigned char DATA)
{
	LCDrs = 1;
	LCDrw = 0;
	LCDen = 1;		//Strobe the enable pin
	LCDdata = DATA;		//Put the value on the pins
	LCDrs = 1;
	LCDrw = 0;
	LCDen = 0;
}

void display_lcd(unsigned char location, unsigned char *d)
{
	lcdcmd(0x00 | location);
	delay(1);				//10mS delay generation
	while(*d)
 	{
	lcddat(*d++);
  	delay(1);				//10mS delay generation
	}
}

void send_pulse(void) //to generate 10 microseconds delay
{
	TH0=0x00;
	TL0=0x00;
	trig=0;
	trig=1;
 _nop_();_nop_();_nop_();_nop_();_nop_();	//each _nop_() generates 1u sec of delay 
 _nop_();_nop_();_nop_();_nop_();_nop_();
 	trig=0;
}

void lcd_number(int val)			// Function to display number 
{
	int i=3;
	char str[7]={"0000 CM"};
	while(val)
	{
	str[i]=0x30 | val%10;
	val=val/10;
	i--;
	}
display_lcd(0xC5,str);
}

void get_range(void)
{
	int range=0;
	int timerval;
	send_pulse();
	while(INT0==0);
	while(INT0==1);
	timerval = TH0;
	timerval = (timerval << 8) | TL0;
	TH0=0xFF;
	TL0=0xFF;
 	if(timerval<35000)  //Makimum 38000us work at higher levels
	range=timerval/59;
	else
	range = 0;
	lcd_number(range);
 }

 void main(void)
{
	//unsigned char string[]={"distance"};
	initialize();			  		//initilaze LCD
	display_lcd(0x80,"  OBSTACLE  AT  ");		//Display character String from location specified 
	TMOD=0x09;					//timer0 in 16 bit mode with gate enable
    	TR0=1;						//timer run enabled
    	TH0=0x00;
	TL0=0x00;
    	echo = 1;					//setting pin P3.2 as input 
 while(1)
 {
	 get_range();
   	 delay(2);
 }
}

