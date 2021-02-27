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


volatile long int time = 0;
int i = 0; //debug
int it = 0;

ISR(PORTB_INT0_vect) {
	TCE0.CNT = 0;
	while ((bit_is_set(PORTB.IN,PIN0_bp)) && (TCE0.CNT < 35000)) {
		asm volatile("nop");
	}
	time = TCE0.CNT;
	while (bit_is_set(PORTB.IN,PIN0_bp)) {
		asm volatile("nop");		
	}
}

void init_us(void) {
	PORTA.DIRSET = 0xFF;				//define/clear ports
	PORTA.OUTCLR = 0xFF;
	PORTB.DIRCLR = PIN0_bm;
	
	TCE0.CTRLA = TC_CLKSEL_DIV8_gc;		//start counter
	TCE0.PER = 0XFFFF;					//65535 (max) counts
	
	
	PORTB.INT0MASK = PIN0_bm;			//Port B, Interrupt 0 to Pin 0
	PORTB.PIN0CTRL = PORT_ISC_RISING_gc;//activate on up
	PORTB.INTCTRL = PORT_INT0LVL_LO_gc;	//Low level interrupts
	PMIC.CTRL |=  PMIC_LOLVLEN_bm;
	sei();								//activate interrupts
}

long int getDistance(int device) {
	
	time = 0;
	int device_ = (1 << (device-1));
	drawRectangle(i, 200, i+18, 220); i+=20; //debug
	float calc = 0.042875;
	PORTA.OUTSET = device_;
	_delay_us(15);
	PORTA.OUTCLR = device_;
	while (time == 0) {
		asm volatile("nop");
	}
	if (time > 34998) {
		if (it < 2) {
			it++;
			return getDistance(device);
		} else {
			it=0;
			return 1500;
		}
		
	}
	it=0;
	return (time * calc - 14);
}