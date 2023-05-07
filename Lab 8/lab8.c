// Write a c program to display the digital value representing the difference in analog voltages at ADC channel 4 and channel 5 on LCD.

#include <LPC17xx.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define Ref_Vtg  3.3
#define Full_Scale 0xFFF

unsigned char str[]={"0123456789"};
unsigned long x,temp,t1=0,t2=0;
unsigned int channel,result,i,j, flag1;
unsigned char Msg1[10]={"ADC value"};
unsigned long int temp1=0, temp2=0 ;

#define RS_CTRL 0x08000000 //P0.27
#define EN_CTRL 0x10000000 //P0.28
#define DT_CTRL 0x07800000 //P0.23 to P0.26 data lines
void lcd_init(void);
void wr_cn(void);
void clr_disp(void);
void delay_lcd(unsigned int);
void lcd_com(void);
void wr_dn(void);
void lcd_data(void);
void clear_ports(void);
void lcd_puts(unsigned char *);

void ADC_IRQHandler(void)
{
	x=LPC_ADC->ADSTAT&(3<<4);
	if(x == (x & 1 << 4)) {
		t1 = (LPC_ADC->ADDR4&(0xFFF<<4)>>4);
	} else {
		t2 = (LPC_ADC->ADDR5&(0xFFF<<4)>>4);
	}
	t1 = ceil((t1*3.3)/4096);
	t2 = ceil((t2*3.3)/4096);
	result = abs(t1-t2);
	Msg1[0] = str[result];
	Msg1[1] = '\0';
	temp1 = 0xc0;
	lcd_com();
	delay_lcd(800);
	lcd_puts(&Msg1[0]);
}

int main(void)
{
	
	lcd_init();
	LPC_PINCON->PINSEL3=(0xF)<<28; //P1.31 as AD0.5
	LPC_SC->PCONP |=(1<<12);
	LPC_ADC->ADCR = (1 << 4 | 1 << 5 | 1 << 16 | 1 << 21);
	LPC_ADC->ADINTEN = (1<<4|1<<5);
	flag1 = 1;
	temp1=0x80;
	lcd_com();
	delay_lcd(800);
	lcd_puts(Msg1);
	NVIC_EnableIRQ(ADC_IRQn);
	while(1);
}

void lcd_init()
{
		/* Ports initialized as GPIO */
		LPC_PINCON->PINSEL3 &= 0xFC003FFF; //P0.23 to P0.28
		/* Setting the directions as output */
		LPC_GPIO0->FIODIR |= DT_CTRL;
		LPC_GPIO0->FIODIR |= RS_CTRL;
		LPC_GPIO0->FIODIR |= EN_CTRL;
		clear_ports();
		delay_lcd(10000);
		temp2 = (0x30<<19);
		wr_cn();
		delay_lcd(10000);
		temp2 = (0x30<<19);
		wr_cn();
		delay_lcd(30000);
		temp2 = (0x30<<19);
		wr_cn();
		delay_lcd(30000);
		temp2 = (0x20<<19);
			wr_cn();
		delay_lcd(30000);
		temp1 = 0x28;
		lcd_com();
		delay_lcd(30000);
		temp1 = 0x0c;
		lcd_com();
		delay_lcd(10000);
		temp1 = 0x06;
		lcd_com();
		delay_lcd(10000);
		temp1 = 0x01;
		lcd_com();
		delay_lcd(10000);
		temp1 = 0x80;
		lcd_com();
		delay_lcd(10000);
		return;
}

void lcd_com(void)
{
		temp2 = temp1 & 0xf0;//move data (26-8+1) times : 26 - HN
		//place, 4 - Bits
		temp2 = temp2 << 19; //data lines from 23 to 26
		wr_cn();
		temp2 = temp1 & 0x0f; //26-4+1
		temp2 = temp2 << 23;
		wr_cn();
		delay_lcd(100000);
		return;
}

void wr_cn(void) //write command reg
{
		clear_ports();
		LPC_GPIO0->FIOPIN = temp2; // Assign the value to the data
		//lines
		LPC_GPIO0->FIOCLR = RS_CTRL; // clear bit RS
		LPC_GPIO0->FIOSET = EN_CTRL; // EN=1
		delay_lcd(25);
		LPC_GPIO0->FIOCLR = EN_CTRL; // EN =0
		return;
}

void lcd_data(void)
{
		temp2 = temp1 & 0xf0;
		temp2 = temp2 << 19;
		wr_dn();
		temp2= temp1 & 0x0f;
		temp2= temp2 << 23;
		wr_dn();
		delay_lcd(100000);
		return;
}

void wr_dn(void)
{
		clear_ports();
		LPC_GPIO0->FIOPIN = temp2; // Assign the value to the
		//data lines
		LPC_GPIO0->FIOSET = RS_CTRL; // set bit RS
		LPC_GPIO0->FIOSET = EN_CTRL; // EN=1
		delay_lcd(100000);
		LPC_GPIO0->FIOCLR = EN_CTRL; // EN =0
		return;
}

void delay_lcd(unsigned int r1)
{
		unsigned int r;
		for(r=0;r<r1;r++);
		return;
}

void clr_disp(void)
{
		temp1 = 0x01;
		lcd_com();
		delay_lcd(100000);
		return;
}

void clear_ports(void)
{
		/* Clearing the lines at power on */
		LPC_GPIO0->FIOCLR = DT_CTRL; //Clearing data lines
		LPC_GPIO0->FIOCLR = RS_CTRL; //Clearing RS line
		LPC_GPIO0->FIOCLR = EN_CTRL; //Clearing Enable line
		return;
}

void lcd_puts(unsigned char *buf1)
{
		unsigned int i=0;
		while(buf1[i]!='\0')
		{
		temp1 = buf1[i];
		lcd_data();
		i++;
		if(i==16)
		{
		temp1 = 0xc0;
		lcd_com();
		}
		}
		return;
}