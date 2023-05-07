#include <lpc17xx.h>

#define RS_CTRL 0x08000000 //P0.27, 1<<27
#define EN_CTRL 0x10000000 //P0.28, 1<<28
#define DT_CTRL 0x07800000 //P0.26 to P0.23 data lines, F<<23

unsigned long int row, col, temp = 0, temp1 = 0, temp2 = 0, temp3 = 0, temp4 = 0, i, j, z, f = 0, t = 0, led, l;
unsigned char flag = 0, flag1 = 0, flag2 = 0;
const char * temp10;
unsigned char msg[4][4] = {
    'A',
    'B',
    'C',
    'D',
    'E',
    'F',
    'G',
    'H',
    'I',
    'J',
    'K',
    'L',
    'M',
    'N',
    'O',
    'P'
};
const char * LETTER_TO_MORSE[4][4] = {
    ".- ", // A
    "-... ", // B
    ".-.- ", // C
    "-.. ", // D
    ". ", // E
    "..-. ", // F
    "--. ", // G
    ".... ", // H
    ".. ", // I
    ".--- ", // J
    "-.- ", // K
    ".-.. ", // L
    "-- ", // M
    "-. ", // N
    "--- ", // O
    ".--. ", // P
};
void lcd_write(void);
void port_write(void);
void keyboard(void);
void scan(void);
void delay_lcd(unsigned int);
unsigned long int init_command[] = {
    0x30,
    0x30,
    0x30,
    0x20,
    0x28,
    0x0c,
    0x06,
    0x01,
    0x80
};
int main(void) {
    SystemInit();
    SystemCoreClockUpdate();
    LPC_GPIO1 -> FIODIR = 0;
    LPC_GPIO2 -> FIODIR = 0x00003C00;

    LPC_GPIO0 -> FIODIR = DT_CTRL | RS_CTRL | EN_CTRL;
    flag1 = 0;
    for (i = 0; i < 9; i++) {
        temp1 = init_command[i];
        lcd_write();
    }
    flag1 = 1;
    i = 0;
    for (i = 0; i < 15; i++) {
        keyboard();

        //if(i==16)
        //{flag1=0;temp1=0xC0;lcd_write(); flag1=1;}
    }
    while (1);
}
void keyboard(void) {
    while (1) {
        for (row = 0; row < 4; row++) {
            if (row == 0)
                temp = 1 << 10;
            if (row == 1)
                temp = 1 << 11;
            if (row == 2)
                temp = 1 << 12;
            if (row == 3)
                temp = 1 << 13;
            LPC_GPIO2 -> FIOPIN = temp;
            flag = 0;
            scan();
            if (flag == 1 && flag1 == 1) {
                //delay_lcd(3000000);

                temp10 = LETTER_TO_MORSE[row][col];
                led = 0;
                l = 0;
                for (z = 0; temp10[z] != '\0'; z++) {
                    temp3 = temp10[z];

                    if (f == 16) {
                        if (t == 0) {
                            t = (t + 1) % 2;
                            flag1 = 0;
                            temp1 = 0xC0;
                            lcd_write();
                            flag1 = 1;
                            temp1 = 0;
                        }
                        f = 0;
                    }
                    temp1 = temp3;
                    if (temp1 == 45) {
                        led = led | (1 << l);
                    }
                    l++;
                    lcd_write();
                    temp1 = 0;
                    f++;
                }
                led = led << 4;
                LPC_GPIO0 -> FIODIRL = 0x0FF0; // P0.11 P0.4 as output
                LPC_GPIO0 -> FIOPINL = led;

                /*LPC_GPIO0->FIOMASK=0xFFF78FFF;
                LPC_GPIO0->FIOPIN=0xFFF78FFF;
                temp3=temp3>>4;
                LPC_GPIO0->FIOMASK = 0xFFFFF00F;
                LPC_GPIO0->FIOPIN=temp3;*/

            }

        }

    }
}
void scan(void) {
    temp4 = LPC_GPIO1 -> FIOPIN;
    temp4 &= 0x07800000;
    if (temp4 != 0x00000000) {
        flag = 1;
        if (temp4 == 1 << 23)
            col = 0;
        if (temp4 == 1 << 24)
            col = 1;
        if (temp4 == 1 << 25)
            col = 2;
        if (temp4 == 1 << 26)
            col = 3;
    }
    delay_lcd(1000000);
}

void lcd_write(void) {
		flag2 = (flag1 == 1) ? 0 : ((temp1 == 0x30) || (temp1 == 0x20)) ? 1 : 0;
    temp2 = temp1 & 0xf0;
    temp2 = temp2 << 19;
    port_write();
    if (flag2 == 0) {
        temp2 = temp1 & 0x0f;
        temp2 = temp2 << 23;
        port_write();
    }
}

void port_write(void){
    LPC_GPIO0 -> FIOPIN = temp2;
    if (flag1 == 0)
        LPC_GPIO0 -> FIOCLR = RS_CTRL;
    else
        LPC_GPIO0 -> FIOSET = RS_CTRL;

    LPC_GPIO0 -> FIOSET = EN_CTRL;
    delay_lcd(25);
    LPC_GPIO0 -> FIOCLR = EN_CTRL;
    delay_lcd(15000);

}

void delay_lcd(unsigned int r1) {
    unsigned int r;
    for (r = 0; r < r1; r++);

    return;
}