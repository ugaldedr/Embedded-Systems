/*******************************
 * Name:Dario Ugalde
 * Student ID#: 1001268068
 * CSE 3442/5442 - Embedded Systems 1
 * Homework 2
 * 
 * Homework 2 Description:
 *  	Use PIC18F452 and XC8 for this assignment
 * 
 *  	Use CCP1's Capture Mode and Interrupts
 *  	to determine the frequency of a signal
 *  	coming through PIN RC2 (CCP1)
 * 
 *  	printf() the calculated frequency continuously
 * 
 *  	Use Clock Stimulus to generate the signal
 *  	within MPLAB's Simulator (50Hz to 500Hz)
 ********************************/ 

// PIC18F452 Configuration Bit Settings
#pragma config OSC = HS         // Oscillator Selection bits (HS oscillator)
#pragma config OSCS = OFF       // Oscillator System Clock Switch Enable bit (Oscillator system clock switch option is disabled (main oscillator is source))
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOR = OFF        // Brown-out Reset Enable bit (Brown-out Reset disabled)
#pragma config BORV = 20        // Brown-out Reset Voltage bits (VBOR set to 2.0V)
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 128      // Watchdog Timer Postscale Select bits (1:128)
#pragma config CCP2MUX = ON     // CCP2 Mux bit (CCP2 input/output is multiplexed with RC1)
#pragma config STVR = OFF       // Stack Full/Underflow Reset Enable bit (Stack Full/Underflow will not cause RESET)
#pragma config LVP = OFF        // Low Voltage ICSP Enable bit (Low Voltage ICSP disabled)

#include <xc.h>
#include <stdio.h>
#include <pic18f452.h>
#include <pic18f452.h> // To use the printf() statement for Simulator's USART Output Window
                   // printf XC8 details: http://microchipdeveloper.com/tls2101:printf

#define _XTAL_FREQ 4000000 // Fosc = 4MHz, required for __delay_ms() and __delay_us() functions
unsigned int period = 0;
unsigned int diffPeriod;
unsigned int prevPeriod;
unsigned int TCurrentHigh = 0;
unsigned int TCurrentLow = 0;
double freq;

// To allow regular console printing within MPLAB's Simulator (override the putch function)
// Go to File > Project Properties > Simulator > Option Categories > Uart1 IO Options > Enable Uart IO (check the box)
// For full info go here: http://microchipdeveloper.com/xc8:console-printing
void putch(unsigned char data) 
{
    while(!PIR1bits.TXIF) // wait until the transmitter is ready
        continue;
    
    TXREG = data;   // send one character
}

void main(void) 
{
    // Needed for printing in MPLAB's UART Console window of the Simulator
    TXSTAbits.TXEN = 1; // enable transmitter
    RCSTAbits.SPEN = 1; // enable serial port
	
	//other settings and main routine...
    
    PIE1bits.CCP1IE = 1; // enable CCP1 interrupts
    TRISC = 0b11111111; // Make TRISC input
    CCP1CON = 0b00000101; // adjust CCP1 control register
    INTCONbits.GIE = 1; // enable global interrupts
    INTCONbits.PEIE = 1; // enable peripheral interrupts
    T1CONbits.TMR1ON = 1; // turn on Timer3
    
    while(1) // wait for interrupts
    {
        printf("freq = %0.3f Hz\n",freq); // print out statement
    }
}

void interrupt ISR(void)
{   
    if(PIR1bits.CCP1IF) // CCP1 interrupt handling
    {
        TCurrentHigh = CCPR1H << 8; // store CCPR1 values
        TCurrentLow = CCPR1L;
        period = TCurrentHigh + TCurrentLow; // push CCPR1 values into period variable
        diffPeriod = period - prevPeriod; // calculate the difference between the two times
        prevPeriod = period; // store period for future calculations
        freq = 1000000/diffPeriod; // frequency calculation
        PIR1bits.CCP1IF = 0; // Reset interrupt flag
    }
}

