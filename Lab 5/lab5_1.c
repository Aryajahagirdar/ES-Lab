//8-bit binary up counter on LEDs

#include <LPC17XX.H>

unsigned int i,j;

unsigned long LED = 0x00000000;

int main()

{
	int i = 0;

	SystemInit();

	SystemCoreClockUpdate();

	LPC_PINCON-> PINSEL0 &= 0xFF0000FF;

	LPC_GPIO0->FIODIR |= 0x00000FF0;

	while(1){

	//LED = 0x00000010;

	for(i=1;i<255;i++)

	{

	LPC_GPIO0->FIOPIN = LED<<4;

	for(j=0;j<50;j++);

	LED += 1;

	}

	/*LED = 0x00000010;

	for(i=255;i>=0;i--)

	{

	LPC_GPIO0->FIOPIN = LED<<4;

	for(j=0;j<50;j++);

	LED -= 1;

	}*/

}

}
