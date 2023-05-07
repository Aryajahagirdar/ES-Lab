#include<LPC17xx.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define RS_CTRL 0x08000000 //P0.27
#define EN_CTRL 0x10000000 //P0.28
#define DT_CTRL 0x07800000 //P0.23 to P0.26 data lines
unsigned long int temp1=0, temp2=0,i,j ;
unsigned char flag1 =0, flag2 =0;
unsigned long int lower = 1, upper = 6, count = 10;
unsigned char msg[] = {48, 49, 50, 51, 52, 53, 54};
unsigned int num;
void lcd_write(void);
void port_write(void);
void clear_lcd(void);
void lcd_com(void);
void wr_cn(void);
void clear_ports(void);

unsigned int ctr=0;
void delay_lcd(unsigned int);
unsigned long int init_command[] = {0x30,0x30,0x30,0x20,0x28,0x0c,0x06,0x01,0x80};

void TIMER0_IRQHandler(void)
{
	LPC_TIM0->IR = 1;
	num = (rand() % (upper - lower + 1)) + lower;
	temp1 = msg[num];
	lcd_write();
	//clr_disp();
}

void init_timer0(void)
{
	LPC_TIM0->TCR = 0x00000002; // Timer0 Reset
	LPC_TIM0->CTCR =0x00;//Timer
	LPC_TIM0->MR0 = 5999999; // For 1ms
	LPC_TIM0->EMR = 0X00;//Do nothing for EM0
	LPC_TIM0->PR = 0;
	LPC_TIM0->MCR = 0x00000003; //Reset TC upon Match-0 and generate INTR
	LPC_TIM0->TCR = 0x00000001; // Timer0 Enable
	return;
}

int main(void)
{
	SystemInit();
	SystemCoreClockUpdate();
	LPC_GPIO0->FIODIR = DT_CTRL | RS_CTRL | EN_CTRL; //Config output
	flag1 =0;//Command
	for (i=0; i<9;i++)
	{
		temp1 = init_command[i];
		lcd_write();	//send Init commands to LCD
	}
	flag1 =1;//Data
	i =0;
	init_timer0();
	NVIC_EnableIRQ(TIMER0_IRQn);
	while(1);
}
void lcd_write(void)
{
	flag2 = (flag1 == 1) ? 0 :((temp1 == 0x30) || (temp1 == 0x20)) ? 1 : 0;//If command is 0x30 (Working in 8-bit mode initially), send ‘3’ on D7-D4(D3-D0 already grounded)
	temp2 = temp1 & 0xf0;//
	temp2 = temp2 << 19;//data lines from 23 to 26. Shift left (26-8+1) times so that higher digit is sent on P0.26 to P0.23
	port_write(); // Output the higher digit on P0.26-P0.23
	if (!flag2) // Other than command 0x30, send the lower 4-bt also
	{
		temp2 = temp1 & 0x0f; //26-4+1
		temp2 = temp2 << 23;
		port_write(); // Output the lower digit on P0.26-P0.23
	}
}
void port_write(void)
{
	LPC_GPIO0->FIOPIN = temp2;
	if (flag1 == 0)
		LPC_GPIO0->FIOCLR = RS_CTRL; // Select command register
	else
		LPC_GPIO0->FIOSET = RS_CTRL; //Select data register
	LPC_GPIO0->FIOSET = EN_CTRL; //Apply -ve edge on Enable
	delay_lcd(50000);
	LPC_GPIO0->FIOCLR = EN_CTRL;
	delay_lcd(5000);
}
void delay_lcd(unsigned int r1)
{
	unsigned int r;
	for(r=0;r<r1;r++);
	return;
}