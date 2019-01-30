; Dario Ugalde
; 1001268068
; CSE 3442 - 001 Embedded Systems
	
#include "p18f452.inc"
	VAR_1	EQU	0x20	;My 1st "variable" stored at File Register address 0x20
	
		ORG	0x00
;START-------------------------------------------------------------------------
LOOP_1		MOVLW   0	    ;Clear the Working Register
		MOVWF	TRISB	    ;Clear TRISB initially
		MOVWF	PORTB	    ;Clear PORTB initially
		MOVWF	TRISD
		MOVWF	PORTD
		MOVLW	B'11111111' ;Make PORTB an INPUT for every pin
		MOVWF   TRISB	    ;Make PORTB an INPUT for every pin
		NOP
		NOP		    ;FIRE Stimulus around here when stepping through
		NOP
		NOP
		MOVF	PORTB, W    ;Copy PORTB's value into the Working Register
		MOVWF	VAR_1	    ;Copy the WREG's value into my "variable" 
		NOP
		NOP
		MOVLW	0	    ;Clear the working register
		MOVWF	TRISB	    ;Make PORTB an OUTPUT for every pin
		MOVWF	PORTB	    ;Clear PORTB
		NOP
		NOP
		MOVWF	TRISD	    ;Make PORTD and OUTPUT for every pin
		MOVWF	PORTD	    ;Clear PORTD
		NOP
		NOP
		BTFSS	VAR_1, 2    ;Check pin RB2 if high or low	    
		GOTO	LOW_P	    ;Jump to LOW_P label
		GOTO	HIGH_P	    ;Jump to HIGH_P label
		NOP
		NOP
HIGH_P		MOVLW	B'00100000' ;Values to set RB5 to 1 
		MOVWF	PORTB	    ;Push above values to RB pins
		NOP
		NOP
		MOVLW	B'00000000' ;Values to set RD3 to 0
		MOVWF	PORTD	    ;Push above values to RD pins
		NOP
		NOP
		GOTO	LOOP_1
LOW_P		MOVLW	B'00001000' ;Values to set RB5 to 0 
		MOVWF	PORTB	    ;Push above values to RB pins
		NOP
		NOP
		MOVLW	B'00001000' ;Values to set RD3 to 1
		MOVWF	PORTD	    ;Push above values to RD pins
		NOP
		NOP
		GOTO	LOOP_1	    ;Go back to beginning
END