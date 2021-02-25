/*---Configuration---*/
//unused yet
#define TRIG_PORT	PORTA
#define TRIG_PIN	PIN0_bm
#define ECHO_PORT	PORTB
#define ECHO_PIN	PIN0_bm



/*--------------------*/
#define F_CPU     32000000UL 



#include "hc-sr04.h"
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>
#include "clock.h"
#include "serialF0.h"


volatile long int time = 0;


ISR(PORTB_INT0_vect) {
	TCE0.CNT = 0;
	
	while ((bit_is_set(PORTB.IN,PIN0_bp))) {
		time = TCE0.CNT;
		if ( (TCE0.CNT > 35000) ) {
			break;
		}
	}
	
}

void init_us(void) {
	PORTA.DIRCLR = PIN0_bm;				//define/clear ports
	PORTB.DIRCLR = PIN0_bm;
	
	TCE0.CTRLA = TC_CLKSEL_DIV8_gc;		//start counter
	TCE0.PER = 0XFFFF;					//65535 (max) counts
	
	
	PORTB.INT0MASK = PIN0_bm;			//Port B, Interrupt 0 to Pin 0
	PORTB.PIN0CTRL = PORT_ISC_RISING_gc;//activate on up
	PORTB.INTCTRL = PORT_INT0LVL_LO_gc;	//Low level interrupts
	
	sei();								//activate interrupts
}

long int getDistance(void) {
	time = 0;
	float calc = 0.042875;
	PORTA.DIRSET = PIN0_bm;
	_delay_us(12);
	PORTA.DIRCLR = PIN0_bm;
	while (time == 0) {
		asm volatile("nop");
	}
	if (time > 34998) {
		return 65535;
	}
	return (time * calc - 14);
}