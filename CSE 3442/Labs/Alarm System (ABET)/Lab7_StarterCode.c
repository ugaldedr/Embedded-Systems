/*******************************
 * Name: Dario Ugalde
 * Student ID#: 1001268068
 * Lab Day: Tuesday, May 1st, 2018
 * CSE 3442/5442 - Embedded Systems 1
 * Lab 7 (ABET): Building a PIC18F4520 Standalone Alarm System with EUSART Communication 
 ********************************/

 /**
 NOTE: 	
	*Your comments need to be extremely detailed and as professional as possible
	*Your code structure must be well-organized and efficient
	*Use meaningful naming conventions for variables, functions, etc.
	*Your code must be cleaned upon submission (no commented out sections of old instructions not used in the final system)
	*Your comments and structure need to be detailed enough so that someone with a basic 
            embedded programming background could take this file and know exactly what is going on
 **/

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Defining _XTAL_FREQ and selecting the XC8 compiler allows you to use the delay functions __delay_ms(x) and __delay_us(x)
#define _XTAL_FREQ 20000000 //If using the External Crystal of 20 MHz

#define YELLOW_LED PORTBbits.RB5 // Yellow LED to toggle
#define BLUE_LED   PORTBbits.RB4 // Blue LED to toggle
#define GREEN_LED  PORTBbits.RB3 // Green LED to toggle
#define RED_LED    PORTBbits.RB2 // Red LED to toggle
#define PASSCODE_1 0xA0          // EEPROM location of first passcode element
#define PASSCODE_2 0xA1          // EEPROM location of second passcode element
#define PASSCODE_3 0xA2          // EEPROM location of third passcode element
#define PASSCODE_4 0xA3          // EEPROM location of fourth passcode element
#define FRESH_BOOT 0xA4          // EEPROM location of new boot flag
#define LOCKED     0xA5          // EEPROM location of system lock flag
#define PIC_ONLINE 0xA7          // EEPROM location of PIC_Online flag
#define TEMP_FLAG  0xA6          // EEPROM location of temperature sensor flag
#define THRES_1    0xA8          // EEPROM location of temperature threshold first element
#define THRES_2    0xA9          // EEPROM location of temperature threshold second element
#define THRES_3    0xAA          // EEPROM location of temperature threshold third element
#define I_SOURCE   0xAB          // EEPROM location of input source
#define MOTN_FLAG  0xB0          // EEPROM location of motion sensor flag

// PIC18F4520 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1H
#pragma config OSC = HS         // Oscillator Selection bits (HS oscillator)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown Out Reset Voltage bits (Minimum setting)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF      // PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON      // MCLR Pin Enable bit (RE3 input pin enabled; MCLR enabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) not protected from table reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

/*******************************
 * Global Variables
 ********************************/
    long fullAD = 0;
/*******************************
 * Function prototypes
 ********************************/
void Initialize_PIC(void);
void USART_Settings(void);
unsigned char Read_EEPROM(unsigned char address);
void Write_EEPROM(unsigned char address, unsigned char data);
void Clear(void);
void System_Boot(void);
void Login(void);
void Set_Passcode(void);
char Get_Input(void);
char Keyboard_Input(void);
char Keypad_Input(void);
void Main_Menu(void);
void Change_Passcode(void);
void Motion_Sensor_Menu(void);
void Toggle_Motion();
void Temperature_Sensor_Menu(void);
void Toggle_Temp();
void Change_Temp();
void Input_Settings_Menu(void);
void interrupt My_ISR_High(void);
void interrupt low_priority My_ISR_Low(void);
void putch(char c)
{
    while(TXSTAbits.TRMT == 0)
    {
        
    };
    TXREG = c;
}

void main(void)
{
    Initialize_PIC();                               // Initialize settings for Alarm System
    USART_Settings();                               // Initialize necessary settings for alarm system
    Clear();                                        // Clear out the terminal for new input
    GREEN_LED = 1;                                  // Turn on GREEN LED to indicate system is active
    if(!Read_EEPROM(I_SOURCE))
    {
        BLUE_LED = 1;
    }
    Write_EEPROM(LOCKED,1);                         // Lock system on reset
    if(Read_EEPROM(FRESH_BOOT))                     // Check if the PIC has been reprogrammed
    {
        printf("PIC18F4520 Alarm System\r");        // Print system name to user
        System_Boot();                              // System boot function to create passcode
        Write_EEPROM(FRESH_BOOT,0);                 // Turn off system boot flag
        Write_EEPROM(MOTN_FLAG,0);                  // Default setting for motion sensor OFF
        Write_EEPROM(TEMP_FLAG,0);                  // Default setting for temperature sensor OFF
        Write_EEPROM(I_SOURCE,1);                   // Default input source keyboard
        Write_EEPROM(LOCKED,0);                     // Unlock the system for first use
        printf("\r");
    }
    T0CONbits.TMR0ON = 1;
    while(1)                                        // Infinite loop for shell-like menu
    {
        if(Read_EEPROM(LOCKED))                     // Check if system is currently locked
        {
            printf("Enter passcode: ");             // Prompt user to enter passcode
            Login();                                // Function to handle login attempts
            printf("\rSuccessful login.\r");        // Notify user of successful login
            
        }
        Main_Menu();                                // Display main menu options to user
    }
}

/*******************************
 * Initialize_PIC(void)
 *
 * This function performs all initializations of variables and registers
 * for the PIC18F4520
 *
 ********************************/
void Initialize_PIC(void)
{
    // TRIS settings for the alarm system
    TRISA = 0b10000001;         // Set I/O for PORTA
    TRISB = 0b11000011;         // Set I/O for PORTB
    TRISC = 0b10000000;         // Set I/O for PORTC
    TRISD = 0b11110000;         // Set I/O for PORTD
    // All LEDs initially OFF
    YELLOW_LED = 0;             // All LEDs initially OFF
    BLUE_LED = 0;               // All LEDs initially OFF
    GREEN_LED = 0;              // All LEDs initially OFF
    RED_LED = 0;                // All LEDs initially OFF
    // Timer settings for alarm system
    INTCONbits.TMR0IE = 1;      // TMR0 Overflow interrupt enable
    INTCON2bits.TMR0IP = 0;     // Make TMR0 overflow a low priority interrupt
    T0CON = 0b00000111;         // TMR0 Settings for Temperature Sensor
    // ADCON settings for alarm system
    ADCON0 = 0b00000001;        // Set up ADCON settings for the system
    ADCON1 = 0b00001111;
    ADCON2 = 0b10111110;        // Set up ADCON settings for the system
    // Interrupt settings for alarm system
    INTCONbits.PEIE = 1;        // Enable peripheral interrupts
    INTCONbits.GIE = 1;         // Enable global interrupts
    RCONbits.IPEN = 1;          // Enable interrupt priority
    INTCON2 = 0x00;             // Set interrupt detection on falling edge
    INTCONbits.INT0IE = 1;      // Enable RB0 as an external hardware interrupt
    INTCONbits.INT0IF = 0;      // Clear INT0IF flag
    
}

/*******************************
 * USART_Settings(void)
 *
 * This function performs all initializations of the
 * settings necessary to transmit and receive data
 * between PIC18F4520 and the serial device
 *
 ********************************/
void USART_Settings(void)
{
    TRISCbits.RC6 = 0;          // TX is OUTPUT
    TRISCbits.RC7 = 1;          // RC is INPUT    
    SPBRG = 33;                 // Low speed: 9,600 BAUD rate
    TXSTAbits.SYNC = 0;         // Async. Mode
    TXSTAbits.BRGH = 0;         // Low speed BAUD rate
    TXSTAbits.TX9 = 0;          // 8-bit transmission
    RCSTAbits.RX9 = 0;          // 8-bit reception
    PIE1bits.RCIE = 0;
    PIE1bits.TXIE = 0;
    PIE1bits.RCIE = 1;          // Enable USART Receive
    RCSTAbits.SPEN = 1;         // Serial Port ENABLED (RX and TX active)
    TXSTAbits.TXEN = 1;         // Enable data transmission
    RCSTAbits.CREN = 1;         // Enable continuous receiver (turned ON)
}

/*******************************
 * Read_EEPROM(unsigned char address)
 *
 * This function checks the data stored at a provided
 * memory address in EEPROM and returns the data at 
 * that location
 *
 ********************************/
unsigned char Read_EEPROM(unsigned char address)
{
    EEADR = address;            // Store provided address in EEADR register
    EECON1bits.EEPGD = 0;       // Set EEPGD bit to access EEPROM memory
    EECON1bits.CFGS = 0;        // Set CFGS bit to access EEPROM memory
    EECON1bits.RD = 1;          // Initiates an EEPROM read at EEADR
    Nop();                      // Stall to give time to read
    Nop();                      // Stall to give time to read
    return EEDATA;              // Return the data at the specified address
}
 
/*******************************
 * Write_EEPROM(unsigned char address, unsigned char data)
 *
 * This function sends data out be stored in the EEPROM
 * at the provided memory location
 *
 ********************************/
void Write_EEPROM(unsigned char address, unsigned char data)
{
    INTCONbits.GIE = 0;             // Disable Global Interrupt
    EEADR = address;                // Set EEADR register to look at EEPROM address
    EEDATA = data;                  // Set EEDATA to data to be stored at EEPROM address
    EECON1bits.EEPGD = 0;           // Set EEPGD bit to access EEPROM memory
    EECON1bits.CFGS = 0;            // Set CFGS bit to access EEPROM memory
    EECON1bits.WREN = 1;            // Set Write Enable bit to one to allow writes to EEPROM memory
    EECON2 = 0x55;                  // Prepare EEPROM SFRs to write
    EECON2 = 0xAA;                  // Prepare EEPROM SFRs to write
    EECON1bits.WR = 1;              // Start EEPROM write
    while(EECON1bits.WR);           // Wait for EEPROM write to terminate
    EECON1bits.WREN = 0;            // Turn off writing to EEPROM
    INTCONbits.GIE = 1;             // Enable Global Interrupts
}
/*******************************
 * Set_Password(void)
 *
 * This function clears the output terminal
 *
 ********************************/
void Clear()
{
    printf("\033[2J");      //  Clear Screen
    printf("\033[0;0H");

}

/*******************************
 * System_Boot(void)
 *
 * This function  prompts the user to set the passcode
 * and calls the function to allow the user to set the 
 * system function
 *
 ********************************/
void System_Boot()
{
    printf("Set the system passcode: ");            // Prompt user for input
    Set_Passcode();                                 // Function call to set the password
}

/*******************************
 * Login(void)
 *
 * This function handles user input and compares to the stored
 * passcode values. If the input does not match the user is 
 * prompted to input the passcode again.
 *
 ********************************/
void Login()
{
    char user_pass[4];                                                                          // Char array to store user input               
    while(Read_EEPROM(LOCKED))                                                                  // Check if system is still locked after attempted passcode input
    {

        user_pass[0] = Get_Input();                                                             // Get input for first digit of passcode
        printf("*");                                                                            // Print '*' for user to know input was accepted
        user_pass[1] = Get_Input();                                                             // Get input for second digit of passcode
        printf("*");                                                                            // Print '*' for user to know input was accepted
        user_pass[2] = Get_Input();                                                             // Get input for third digit of passcode
        printf("*");                                                                            // Print '*' for user to know input was accepted
        user_pass[3] = Get_Input();                                                             // Get input for fourth digit of passcode
        printf("*");                                                                            // Print '*' for user to know input was accepted
        if(user_pass[0] == Read_EEPROM(PASSCODE_1) && user_pass[1] == Read_EEPROM(PASSCODE_2)   // Check if user input matches system passcode
          && user_pass[2] == Read_EEPROM(PASSCODE_3) && user_pass[3] == Read_EEPROM(PASSCODE_4))
        {
            Write_EEPROM(LOCKED,0);                                                             // Unlock the system
        }
        else
        {
            printf("\rIncorrect passcode. Try again.\r");                                       // Notify user of incorrect passcode
            printf("\rEnter passcode: ");                                                       // Prompt user for next input attempt
        }
    }
}

/*******************************
 * Set_Password(void)
 *
 * This function changes the system password data based on user
 * provided input
 *
 ********************************/
void Set_Passcode()
{
    Write_EEPROM(PASSCODE_1,Get_Input());       // Store the first char of the passcode at EEPROM address
    printf("%c",Read_EEPROM(PASSCODE_1));       // Print the data that was sent to EEPROM
    Write_EEPROM(PASSCODE_2,Get_Input());       // Store the second char of the passcode at EEPROM address
    printf("%c",Read_EEPROM(PASSCODE_2));       // Print the data that was sent to EEPROM
    Write_EEPROM(PASSCODE_3,Get_Input());       // Store the third char of the passcode at EEPROM address
    printf("%c",Read_EEPROM(PASSCODE_3));       // Print the data that was sent to EEPROM
    Write_EEPROM(PASSCODE_4,Get_Input());       // Store the fourth char of the passcode at EEPROM address
    printf("%c",Read_EEPROM(PASSCODE_4));       // Print the data that was sent to EEPROM   
    __delay_ms(100);                            // Delay to allow user to view the last digit input in terminal
}

/*******************************
 * Get_Input(void)
 *
 * This function determines whether to take input from the 
 * keyboard or the keypad
 *
 ********************************/
char Get_Input()
{
    if(Read_EEPROM(I_SOURCE))       // Determine if system is using keyboard(!0) or keypad(0) for input
    {
        return Keyboard_Input();    // Return a single char of input from the keyboard
    }
    else
    {
        __delay_ms(500);
        return Keypad_Input();      // Return a single char of input from keypad
    }
}

/*******************************
 * Keyboard_Input(void)
 *
 * This function returns a single char of 
 * input that was received from the user
 * via keyboard input
 *
 ********************************/
char Keyboard_Input()
{
    char ret;
    INTCONbits.GIE = 0;         // Disable global interrupts
    INTCONbits.PEIE = 0;        // Disable peripheral interrupts
    while(PIR1bits.RCIF == 0);  // Wait for user input
    ret = RCREG;                // Store user input char
    INTCONbits.GIE = 1;         // Enable global interrupts
    INTCONbits.PEIE = 1;        // Enable peripheral interrupts
    return ret;                 // Return char to Get_Input()
}

/*******************************
 * char Keypad_Input(void)
 *
 * This function returns a single char of input that
 * was received from the user via keypad input
 *
 ********************************/
char Keypad_Input()
{
    do
    {
        PORTD = 0x0F;                                                           // Send power to 4 x 4 matrix keypad rows
        Nop();                                                                  // Give PIC time to detect input from keypad
        Nop();                                                                  // Give PIC time to detect input from keypad
    } while(!PORTDbits.RD4&&!PORTDbits.RD5&&!PORTDbits.RD6&&!PORTDbits.RD7);    // Polling upper nibble of PORTD
    
    PORTD = 0x01;                                           /*//////////////////////////////////////////////////////////////////////*/
    if(PORTDbits.RD4 && PORTDbits.RD0)      
    {
        return '1';
    } 
    else if(PORTDbits.RD5 && PORTDbits.RD0)
    {
        return '2';
    }
    else if(PORTDbits.RD6 && PORTDbits.RD0)
    {
        return '3';
    }
    PORTD = 0x02;
    if(PORTDbits.RD4 && PORTDbits.RD1)
    {
        return '4';
    } 
    else if(PORTDbits.RD5 && PORTDbits.RD1)
    {
        return '5';
    }
    else if(PORTDbits.RD6 && PORTDbits.RD1)                           // Testing each row against each column to determine key pressed
    {
        return '6';
    }
    PORTD = 0x04;
    if(PORTDbits.RD4 && PORTDbits.RD2)
    {
        return '7';
    } 
    else if(PORTDbits.RD5 && PORTDbits.RD2)
    {
        return '8';
    }
    else if(PORTDbits.RD6 && PORTDbits.RD2)
    {
        return '9';
    }
    PORTD = 0x08;
    if(PORTDbits.RD3 && PORTDbits.RD5)
    {
        return '0';
    }                                                          /*//////////////////////////////////////////////////////////////////////*/
}

/*******************************
 * Main_Menu(void)
 *
 * This function is the main menu of the alarm system interface
 *
 ********************************/
void Main_Menu()
{
    char input;                                                         // Variable to store user input
    Clear();                                                            // Clear out terminal for clean menu
    printf("\r////////////////////////////////////////////\r");                    /*///////////////////////////////////////////*/
    printf("Main Menu\r");
    if((100*(Read_EEPROM(THRES_1)-48)+(Read_EEPROM(THRES_2)-48)*10+(Read_EEPROM(THRES_3)-48)) > 250 || fullAD == 0)
    {
        printf("Establish temperature settings to view data.\r");
    }
    else
    {
        printf("Threshold Temp: %dF\t System Temp: %dF\r",(100*(Read_EEPROM(THRES_1)-48)+(Read_EEPROM(THRES_2)-48)*10+(Read_EEPROM(THRES_3)-48)),fullAD);
    }
    printf("Currently accepting input from: ");
    if(Read_EEPROM(I_SOURCE))
    {
        printf("Keyboard\r");
    }
    else
    {
        printf("Keypad\r");
    }
    if(Read_EEPROM(MOTN_FLAG))
    {
        printf("Motion sensor is online\r");
    }
    else
    {
        printf("Motion sensor is offline\r");
    }
    printf("////////////////////////////////////////////////\r");
    printf("1) Change Passcode\r");
    printf("2) PIR Sensor Alarm Options\r");                                      //User interface output       
    printf("3) Temperature Sensor Options\r");
    printf("4) Switch Input Method\r");
    printf("\rMake your selection from the choices shown above: ");     /*//////////////////////////////////////////*/
    
    
    
    input = Get_Input();                                                // Get user input to drive alarm system
    printf("%c\r",input);                                               // Print out user input
    
    if(input == '1')                                                    // Check if the user wants to change the passcode
    {
        Change_Passcode();                                              // Function to handle passcode change
    }
    else if(input == '2')                                               // Check if the user wants to change the PIR motion sensor settings
    {
        Motion_Sensor_Menu();
    }
    else if(input == '3')                                               // Check if the user wants to change the Temperature Sensor Method
    {
        Temperature_Sensor_Menu();
    }
    else if(input == '4')                                               // Check if the user wants to change the input settings
    {
        Input_Settings_Menu();
    }
}

/*******************************
 * Change_Passcode(void)
 *
 * This function handles passcode changes reqeuested by the user
 *
 ********************************/
void Change_Passcode()
{
    printf("Enter passcode: ");                 // Prompt user to enter passcode
    Write_EEPROM(LOCKED,1);                 // Lock the system to allow login attempt
    Login();                                // Call function to handle login
    printf("\r");                           // New line
    printf("Enter new passcode: ");         // Prompt user to change passcode
    Set_Passcode();                         // Function to handle new passcode
    printf("\r");                           // New Line
}

/*******************************
 * Motion_Sensor_Menu(void)
 *
 * This function that outputs Motion Sensor settings to user
 *
 ********************************/
void Motion_Sensor_Menu()
{
    Clear();																// Clear terminal for new menu
    char input;																// Variable to store input data
    printf("\r/////////////////////////////////////\r");					///////////////////////////////
    printf("\tMotion Sensor Settings\r");
    printf("//////////////////////////////////////\r");						// User interface output
    printf("1) Toggle Motion Sensor ON/OFF\r");
    printf("\rMake your selection from the choice shown above: ");  		///////////////////////////////
    
    input = Get_Input();													// Store user input
    printf("%c\r",input);													// Display user input
    
    if(input == '1')														// Check if user wants to toggle the motion sensor
    {
        Toggle_Motion();													// Function to toggle the motion sensor
    }
}

/*******************************
 * Toggle_Motion(void)
 *
 * This function turns the motion sensor off and on 
 *
 ********************************/
void Toggle_Motion()
{
    if(Read_EEPROM(MOTN_FLAG))								// Check if the motion flag is on
    {
        Write_EEPROM(MOTN_FLAG,0);							// Turn off motion sensor
        printf("Motion Sensor was disabled.\r");			// Notify user the motion sensor is disabled
        __delay_ms(500);									// Delay to let user see notification
    }
    else
    {
        Write_EEPROM(MOTN_FLAG,1);							// Turn on motion sensor flag	
        printf("Motion Sensor was enabled.\r");				// Let user know the motion sensor is available for use
        __delay_ms(500);									// Delay to let user see notification
    }
}
/*******************************
 * Temperature_Sensor_Menu(void)
 *
 * This function allows user to turn on the temperature sensor and
 * set the threshold temperature
 *
 ********************************/
void Temperature_Sensor_Menu()
{
    Clear();                                                                    // Clear out terminal 
    char input;                                                                 // Variable to store user input
    printf("\r/////////////////////////////////////\r");                        //////////////////////////////////////
    printf("\tTemperature Sensor Settings\r");
    printf("//////////////////////////////////////\r");                         // User interface output
    printf("1) Toggle Temperature Sensor ON/OFF\r");
    printf("2) Adjust temperature Sensor Threshold\r");
    printf("\rMake your selection from the choices shown above: ");             //////////////////////////////////////
    
    input = Get_Input();                                                        // Store user input
    printf("%c\r",input);                                                       // Display user input
    
    if(input == '1')                                                            // Check if user wants to toggle temperature sensor
    {
        Toggle_Temp();                                                          // Toggle temperature sensor
    }
    else if(input == '2')                                                       // Check if user wants to modify temperature threshold
    {
        Change_Temp();                                                          // Modify threshold
    }
}

/*******************************
 * Toggle Temp(void)
 *
 * This function turns the temperature flag off and on when it
 * is called
 *
 ********************************/
void Toggle_Temp()
{
    if(Read_EEPROM(TEMP_FLAG))                                  // If temp flag is on turn it off
    {
        Write_EEPROM(TEMP_FLAG,0);
        printf("Temperature Sensor was disabled.\r");
        __delay_ms(500);
    }
    else                                                        // If temp flag is off turn it on
    {
        Write_EEPROM(TEMP_FLAG,1);
        printf("Temperature Sensor was enabled.\r");
        __delay_ms(500);
    }
}

/*******************************
 * Change_Temp(void)
 *
 * This function handles user input to modify the
 * current temperature threshold
 *
 ********************************/
void Change_Temp()
{
    char input;                                                 // Variable to store user input
    printf("Enter temperature threshold as 3 digits: ");        // Notify user to input data
    input = Get_Input();                                        // Store user input
    Write_EEPROM(THRES_1,input);                                // Push user data to EEPROM
    printf("%c",input);                                         // Print user input
    input = Get_Input();                                        // Store user input
    Write_EEPROM(THRES_2,input);                                // Push user data to EEPROM
    printf("%c",input);                                         // Print user input
    input = Get_Input();                                        // Store user input
    Write_EEPROM(THRES_3,input);                                // Push user input to EEPROM
    printf("%c\r", input);                                      // Print user input
    __delay_ms(500);                                            // Delay to allow user to view last input element
}

/*******************************
 * Input_Settings_Menu(void)
 *
 * This function handles transition between keyboard and keypad
 *
 ********************************/
void Input_Settings_Menu()
{
    Clear();                                                            // Clear out terminal for new menu
    char input;                                                         // Variable to store user input
    printf("\r/////////////////////////////////////\r");                // Print outs for user interface
    printf("\tInput Settings\r");
    printf("//////////////////////////////////////\r");
    printf("1) To use keyboard as input\r");
    printf("2) To use keypad as input\r");
    printf("\rMake your selection from the choices shown above: ");
    
    input = Get_Input();                                                // Get input from user
    printf("%c\r",input);                                               // Print user selection
    
    if(input == '1')                                                    // Check if user wants to use keyboard
    {
        Write_EEPROM(I_SOURCE,1);                                       // Change EEPROM input location to keyboard setting
        BLUE_LED = 0;                                                   // Turn off blue LED
    }
    if(input == '2')                                                    // Check if user wants to use the keypad
    {
        Write_EEPROM(I_SOURCE,0);                                       // Change EEPROM input location to keypad setting
        BLUE_LED = 1;                                                   // Turn on blue LED
    }
}

void interrupt My_ISR_High()
{
    if(INTCONbits.INT0IF && Read_EEPROM(MOTN_FLAG) && !(Read_EEPROM(LOCKED)))
    {
        RED_LED = 1;
        printf("\r\rMOTION SENSOR WAS TRIPPED!!!\r\r");
        printf("Enter passcode: ");
        Write_EEPROM(LOCKED,1);
        Login();
        RED_LED = 0;
    }
    INTCONbits.INT0IF = 0;
}

/*******************************
 * interrupt low_priority(void)
 *
 * This function is the low priority interrupt vector
 * it handles interrupts from managing system temperature
 *
 ********************************/
void interrupt low_priority My_ISR_Low()
{
    long adLow = 0;
    long adHigh = 0;
    int threshold = 100*(Read_EEPROM(THRES_1)-48)+(Read_EEPROM(THRES_2)-48)*10+(Read_EEPROM(THRES_3)-48);
    if(INTCONbits.TMR0IF && !Read_EEPROM(LOCKED) && Read_EEPROM(TEMP_FLAG))
    {
        if(YELLOW_LED)
        {
            YELLOW_LED = 0;
        }
        else
        {
            YELLOW_LED = 1;
        }
        __delay_ms(15);
        ADCON0bits.GO = 1;
        while(ADCON0bits.DONE == 1);
        adLow = ADRESL;
        adHigh = ADRESH;
        adHigh = adHigh << 8;
        fullAD = adHigh + adLow;
        fullAD = (fullAD*180/1023) + 43;
        if(fullAD >= threshold)
        {
            YELLOW_LED = 1;
            Clear();
            printf("\r\rWARNING: SYSTEM TEMPERATURE APPROCHING THRESHOLD!!!\r\r");
            printf("System locked.\r");
            printf("Enter passcode: ");
            Write_EEPROM(LOCKED,1);
            Login();
            Change_Temp();
            YELLOW_LED = 0;
        }
    }
    INTCONbits.TMR0IF = 0;
}
