#include <msp430.h>

#define DIN BIT7
#define CLK BIT0
#define CS  BIT1

void write(unsigned int value){
    int i=0;
    for(i=0;i<8;i++){
        P2OUT&=~CLK;           //logic low the P2.0
        P1OUT=(value & DIN);   //logic high the P1.7, when MSB is 1 of the "value" otherwise logic low
        value=value<<1;        //right shift the value
        P2OUT|=CLK;            //logic high the P2.0
    }
}

//Generate the signal to write data on registers in MAX7221
void DiN(unsigned int value,unsigned int address){
    P1OUT&=~CS;                 //logic low the P1.1
    write(address);
    write(value);
    P1OUT=CS;                   //logic high P1.1 after generate signal
}

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

    //array of segments data of letters to display "En20380496"
	unsigned int val[18]={0b01001111,0b01110110,0b01101101,0b01111110,0b01111001,0b01111111,0b01111110,0b00110011,0b01111011,0b01011111};

    //setting directions
    P2DIR=CLK;       //pin 2.0 CLK
    P1DIR=DIN+CS;    //pin 1.7 din, pin 1.1 cs

    //setting up the important registers of the MAX7221 to use display
    DiN(0b00000000,0b00001001);   //decode mode(no decode)
    DiN(0b00000111,0b00001011);   //scan limit (digit0 -digit7)
    DiN(0b00001110,0b00001010);   //intensity  (29/32 duty cycle)
    DiN(0b00000001,0b00001100);   //power on

    //n digit address,t index of val
    unsigned int n;
    int t=0;
    /*
     * This is infinity loop
     * Scroll letters
     */
    for(;;){
        for(n=0b00000001;0b00001000>=n;n=n+0b00000001){
            t=(t==18) ? 0:t; //reset the t index if it is 18
            DiN(val[t++],n);
        }
        t=(t<=9) ? t+9:t-9; //if index is less than or equal 9 than add 9 to t otherwise subtract 9
        __delay_cycles(1000000); //speed of the scrolling
    }
}
