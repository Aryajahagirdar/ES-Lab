//8-bit up/down counter on LEDs using key press

#include <LPC17XX.H>

unsigned int i,j;

unsigned long LED = 0x00000000;

unsigned long x;


int main()

{

	int i = 0;

	SystemInit();

	SystemCoreClockUpdate();

	LPC_PINCON-> PINSEL0 &= 0xFF0000FF;

	LPC_GPIO0->FIODIR |= 0x00000FF0;


	while(1){

		x = LPC_GPIO2->FIOPIN &1 << 12;

		if(x){

		LPC_GPIO0->FIOPIN = LED<<4;

		for(j=0;j<1000000;j++);

		LED += 1;

	}

	else{

		LPC_GPIO0->FIOPIN = LED<<4;

		for(j=0;j<1000000;j++);

		LED -= 1;

	}

}

}


