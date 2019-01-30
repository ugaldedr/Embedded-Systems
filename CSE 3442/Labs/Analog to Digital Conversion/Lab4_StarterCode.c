/*******************************
 * Name: Dario Ugalde
 * Student ID#: 1001268068
 * Lab Day: 3/20/18
 * CSE 3442/5442 - Embedded Systems 1
 * Lab x: Lab 4
 ********************************/

#include <xc.h> // For the XC8 Compiler (automatically finds/includes the specific PIC18F452.h header file)

#define _XTAL_FREQ  10000000     // Running at 10MHz (external oscillator/crystal), REQUIRED for delay functions

#define LED_LEFT    PORTAbits.RA3  // QwikFlash red LED (left) to toggle
#define LED_CENTER  PORTAbits.RA2  // QwikFlash red LED (center) to toggle
#define LED_RIGHT   PORTAbits.RA1  // QwikFlash red LED (right) to toggle

// PIC18F452 Configuration Bit Settings
#pragma config OSC     = HS     // Oscillator Selection bits (HS oscillator)
#pragma config OSCS    = OFF    // Oscillator System Clock Switch Enable bit (Oscillator system clock switch option is disabled (main oscillator is source))
#pragma config PWRT    = OFF    // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOR     = OFF    // Brown-out Reset Enable bit (Brown-out Reset disabled)
#pragma config WDT     = OFF    // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config CCP2MUX = ON     // CCP2 Mux bit (CCP2 input/output is multiplexed with RC1)
#pragma config STVR    = ON     // Stack Full/Underflow Reset Enable bit (Stack Full/Underflow will cause RESET)
#pragma config LVP     = OFF    // Low Voltage ICSP Enable bit (Low Voltage ICSP disabled)

/*******************************
 * Global Vars
 * 
 * Strings for LCD Initialization and general use
 * For stability reasons, each array must have 10 total elements (no less, no more)
 ********************************/
const char LCDstr[]  = {0x33,0x32,0x28,0x01,0x0c,0x06,0x00,0x00}; // LCD Initialization string (do not change)

//Never change element [0] or [9] of the following char arrays
//You may only change the middle 8 elements for displaying on the LCD
char Str_1[] = {0x80,' ',' ',' ',' ',' ',' ',' ',' ',0};    // First line of LCD
char Str_2[] = {0xC0,' ',' ',' ',' ',' ',' ',' ',' ',0};    // Second line of LCD

const char Clear1[] = {0x80,' ',' ',' ',' ',' ',' ',' ',' ',0};  // Clear first line of LCD
const char Clear2[] = {0xC0,' ',' ',' ',' ',' ',' ',' ',' ',0};  // Clear second line of LCD


/*******************************
 * Function prototypes
 ********************************/
void Initialize_PIC(void);
void Initialize_LCD(void);
void Print_To_LCD(const char *);
void Toggle_LEDs(void);
long Read_External_Pot(void);
long Read_Internal_Pot(void);

void main(void)
{
    Initialize_PIC();       //Initialize all settings required for general QwikFlash and LCD operation
	Print_To_LCD(Clear1);   //Clear the LCD line 1 to start your program
	Print_To_LCD(Clear2);   //Clear the LCD line 2 to start your program
    
    
    
    // Main routine, forever in an infinite while(true) loop
    while(1)
    {
        __delay_ms(200); // 2 second delay as stated in instructions
        if(Read_External_Pot() > Read_Internal_Pot()) // 10k pot higher voltage
        {
            LED_LEFT = 1;
            LED_CENTER = 0;
            LED_RIGHT = 0;
        }
        else if(Read_External_Pot() == Read_Internal_Pot()) // 10k and 5k pots equivalent
        {
            LED_LEFT = 0;
            LED_CENTER = 1;
            LED_RIGHT = 0;
        }
        else //5k pot > 10k pot
        {
            LED_LEFT = 0;
            LED_CENTER = 0;
            LED_RIGHT = 1;
        }
    }
}

/*******************************
 * Initialize_PIC(void)
 *
 * This function performs all initializations of variables and registers
 * for the PIC18F452 when specifically on the QwikFlash board.
 *
 * DO NOT CHANGE ANYTHING IN THIS FUNCTION
 ********************************/
void Initialize_PIC(void)
{
    // Reference the QwikFlash schematic (pdf) to understand analog/digital IO decisions
    
    ADCON1 = 0b10001110;    // Enable PORTA & PORTE digital I/O pins
    TRISA  = 0b11100001;    // Set I/O for PORTA
    TRISB  = 0b11011100;    // Set I/O for PORTB
    TRISC  = 0b11010000;    // Set I/0 for PORTC
    TRISD  = 0b00001111;    // Set I/O for PORTD
    TRISE  = 0b00000000;    // Set I/O for PORTE
    PORTA  = 0b00010000;    // Turn off all four LEDs driven from PORTA    
    LED_LEFT    = 0;        // All LEDs initially OFF
    LED_CENTER  = 0;        // All LEDs initially OFF
    LED_RIGHT   = 0;        // All LEDs initially OFF
    Initialize_LCD();       // Initialize LCD
}

/*******************************
 * Initialize_LCD(void)
 *
 * Initialize the Optrex 8x2 character LCD.
 * First wait for 0.1 second, to get past the displays power-on reset time.
 *
 * DO NOT CHANGE ANYTHING IN THIS FUNCTION
 *******************************/
void Initialize_LCD(void)
{
    char currentChar;
    char *tempPtr;

    __delay_ms(100);                // wait 0.1 sec (100 ms)

    PORTEbits.RE0 = 0;              // RS=0 for command
    tempPtr = LCDstr;

    while (*tempPtr)                // if the byte is not zero (end of string)
    {
        currentChar = *tempPtr;
        PORTEbits.RE1 = 1;          // Drive E pin high
        PORTD = currentChar;        // Send upper nibble
        PORTEbits.RE1 = 0;          // Drive E pin low so LCD will accept nibble          
        __delay_ms(10);             // wait 10 ms
        currentChar <<= 4;          // Shift lower nibble to upper nibble
        PORTEbits.RE1 = 1;          // Drive E pin high again
        PORTD = currentChar;        // Write lower nibble
        PORTEbits.RE1 = 0;          // Drive E pin low so LCD will process byte        
        __delay_ms(10);             // wait 10 ms	
        tempPtr++;                  // Increment pointer to next character
    }
}

/*******************************
 * Print_To_LCD(const char * tempPtr) 
 *
 * This function is called with the passing in of an array of a constant
 * display string.  It sends the bytes of the string to the LCD.  The first
 * byte sets the cursor position.  The remaining bytes are displayed, beginning
 * at that position.
 * This function expects a normal one-byte cursor-positioning code, 0xhh, or
 * an occasionally used two-byte cursor-positioning code of the form 0x00hh.
 *
 * DO NOT CHANGE ANYTHING IN THIS FUNCTION
 ********************************/
void Print_To_LCD(const char * tempPtr)
{
	char currentChar;
    PORTEbits.RE0 = 0;          // Drive RS pin low for cursor-positioning code

    while (*tempPtr)            // if the byte is not zero (end of string)
    {
        currentChar = *tempPtr;
        PORTEbits.RE1 = 1;      // Drive E pin high
        PORTD = currentChar;    // Send upper nibble
        PORTEbits.RE1 = 0;      // Drive E pin low so LCD will accept nibble
        currentChar <<= 4;      // Shift lower nibble to upper nibble
        PORTEbits.RE1 = 1;      // Drive E pin high again
        PORTD = currentChar;    // Write lower nibble
        PORTEbits.RE1 = 0;      // Drive E pin low so LCD will process byte
        __delay_ms(10);         // wait 10 ms	
        PORTEbits.RE0 = 1;      // Drive RS pin high for displayable characters
        tempPtr++;              // Increment pointerto next character
    }
}

/*******************************
 * Toggle_LEDs(void)
 *
 * This function simply toggles the QwikFlash's red LEDs in a simple sequence
 * The LED_X defines are at the top of this .c file
 * 
 * You may alter this function if you like or directly manipulate LEDs in other functions
 ********************************/
void Toggle_LEDs(void)
{
    LED_LEFT ^= 1;      // regardless of the bit's previous state, this flips it to 1 or 0
    __delay_ms(100);
    
    LED_CENTER ^= 1;
    __delay_ms(100);
    
    LED_RIGHT ^= 1;
    __delay_ms(100);
}

/*******************************
 * Read_External_Pot(void)
 *
 * This function reads input from the external potentiometer
 * A calculation is then done to determine the input voltage
 * 
 * It returns the value of voltage * 10
 ********************************/
long Read_External_Pot(void)
{
    long adLow = 0; // Variable to store ADRESL
    long adHigh = 0; // Variable to store ADRESH
    long fullAD = 0; // Variable to store full 10-bit result of voltage output
    TRISEbits.TRISE2 = 1; // E2 is an input
    
    ADCON0 = 0b00111001; // Set ADCON0 to proper conversion speed, read from channel 7, and power on
    ADCON1 = 0b11000000; // Set ADCON1 to be right justified, proper conversion speed, and set control bits
    __delay_ms(15); // Delay greater than or equal to TACQ of the PIC
    ADCON0bits.GO = 1; // Start analog to digital conversion
    while(ADCON0bits.DONE == 1); // Stall the program until the conversion is complete
    adLow = ADRESL; // Store the bits from ADRESL
    adHigh = ADRESH; // Store the bits from ADRESH
    adHigh = adHigh << 8; // Shift the high bits up to their proper locations
    fullAD = adHigh + adLow; // Add in the lower 8 bits of the result
    fullAD = (fullAD*50)/1023; // Voltage calculation
        
    Str_1[2] = '0' + (fullAD / 10); // Store the one's place value of the voltage in the LCD output
    Str_1[3] = '.'; // Add the decimal point to the output
    Str_1[4] = '0' + (fullAD % 10); // Store the tenths place value of the voltage in the LCD output
    Str_1[5] = 'V'; // Add the units to the LCD display
        
    ADCON1 = 0b10001110;    // Enable PORTA & PORTE digital I/O pins
    Print_To_LCD(Str_1); // Print the output
        
    __delay_ms(250); // Sampling rate (in ms)
    return fullAD;
}

/*******************************
 * Read_Internal_Pot(void)
 *
 * This function reads input from the internal potentiometer
 * A calculation is then done to determine the input voltage
 * 
 * It returns the value of voltage * 10
 ********************************/
long Read_Internal_Pot(void)
{
    long adLow = 0; // Variable to store ADRESL
    long adHigh = 0; // Variable to store ADRESH
    long fullAD = 0; // Variable to store full 10-bit result of voltage output
    TRISAbits.TRISA5 = 1; // A5 is an input
    
    ADCON0 = 0b00100001; // Set ADCON0 to proper conversion speed, read from channel 4, and power on
    ADCON1 = 0b11000000; // Set ADCON1 to be right justified, proper conversion speed, and set control bits
    __delay_ms(15); // Delay greater than or equal to TACQ of the PIC
    ADCON0bits.GO = 1; // Start analog to digital conversion
    while(ADCON0bits.DONE == 1); // Stall the program until the conversion is complete
    adLow = ADRESL; // Store the bits from ADRESL
    adHigh = ADRESH; // Store the bits from ADRESH
    adHigh = adHigh << 8; // Shift the high bits up to their proper locations
    fullAD = adHigh + adLow; // Add in the lower 8 bits of the result
    fullAD = (fullAD*50)/1023; // Voltage calculation
        
    Str_2[2] = '0' + (fullAD / 10); // Store the one's place value of the voltage in the LCD output
    Str_2[3] = '.'; // Add the decimal point to the output
    Str_2[4] = '0' + (fullAD % 10); // Store the tenths place value of the voltage in the LCD output
    Str_2[5] = 'V'; // Add the units to the LCD display
        
    ADCON1 = 0b10001110;    // Enable PORTA & PORTE digital I/O pins
    Print_To_LCD(Str_2); // Print the output
        
    __delay_ms(250); // Sampling rate (in ms)
    return fullAD;
}
