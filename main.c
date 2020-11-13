#include <msp430g2553.h>

void send_message(unsigned char * TX_message, unsigned char length);

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	// set up clock frequency
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;

	P1DIR = BIT0;   // make P1.0 output
	P1REN = BIT3;   // enable pull up/down resistor
	P1OUT = 0x08;   // make P1.0 low

	P1IES = BIT3;   // high
	P1IFG &= ~BIT3; // clear flag
	P1IE = BIT3;    // enable interrupt pin 3

	// Configure UART
	P1SEL = BIT1 + BIT2 ; // P1.1 = RXD, P1.2=TXD
	P1SEL2 = BIT1 + BIT2 ; // P1.1 = RXD, P1.2=TXD
	UCA0CTL1 |= UCSSEL_2;  // SMCLK
	UCA0BR0 = 104;         // SMCLK 1MHZ 9600 baud rate
	UCA0BR1 = 0;           // SMCLK 1MHZ 9600 baud rate
	UCA0MCTL = UCBRS0;          // Modulation UCBRSx = 1
	UCA0CTL1 &= ~UCSWRST;   // Initialize USCI state machine

	_enable_interrupt();
}

#pragma vector = PORT1_VECTOR
__interrupt void P1_ISR(void)
{
    if(P1IFG & BIT3)
    {
        P1IFG &= ~BIT3; // clear flag
        send_message("Hello", 5);   // send message
        send_message("\r\n", 2);    // send message
        P1OUT ^= BIT0;  // toggle red LED
        volatile int i;
        for(i = 0; i < 20000; i++);
    }
}

void send_message(unsigned char * TX_message, unsigned char length)
{
    while(length--) // until end of string
    {
        while(!(IFG2 & UCA0TXIFG)); // until all previous data has been sent & TX buffer ready for new data
        UCA0TXBUF = *TX_message;    // send character pointed by pointer
        _enable_interrupt();
        TX_message++;   // increment pointer to next character
    }
}
