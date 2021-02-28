/*----- HC-SR04 for XMEGA-----
 *
 *	Uses interrupts and returns time in mm
 *	use init_us(); and then you can use long int getDistance(device(1-8));
 *
 *	Can use up to 8 devices connected to TRIG_PORT pin 0-7
 *
 *
*/

/*---Configuration---*/
#define TRIG_PORT		PORTA
#define ECHO_PORT		PORTB
#define ECHO_PIN		PIN0_bm
#define ECHO_PIN_IN		PIN0_bp
#define ECHO_PIN_CTRL	PIN0CTRL
#define ECHO_INTERRUPT	PORTB_INT0_vect
#define ECHO_INT_MASK	INT0MASK
#define ECHO_INT_EN		PORT_INT0LVL_LO_gc
#define TIMER_COUNTER	TCE0

#define CONSTANT 0.042875 //CLK(n) * PRESCALE(8) * CLKPERIOD(1/32MHz) * SOUNDSPEED(343000mm/s) / 2(back and forth) Gives distance in mm

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

ISR(ECHO_INTERRUPT) {
	TIMER_COUNTER.CNT = 0;
	while ((bit_is_set(ECHO_PORT.IN,ECHO_PIN_IN)) && (TIMER_COUNTER.CNT < 35000)) {
		asm volatile("nop");
	}
	time = TIMER_COUNTER.CNT;
	while (bit_is_set(ECHO_PORT.IN,ECHO_PIN_IN)) {
		asm volatile("nop");		
	}
}

void init_us(void) {
	TRIG_PORT.DIRSET = 0xFF;				//define/clear ports
	TRIG_PORT.OUTCLR = 0xFF;
	ECHO_PORT.DIRCLR = ECHO_PIN;
	
	TIMER_COUNTER.CTRLA = TC_CLKSEL_DIV8_gc;		//start counter
	TIMER_COUNTER.PER = 0XFFFF;					//65535 (max) counts
	
	
	ECHO_PORT.ECHO_INT_MASK = ECHO_PIN;			//Port B, Interrupt 0 to Pin 0
	ECHO_PORT.ECHO_PIN_CTRL = PORT_ISC_RISING_gc;//activate on up
	ECHO_PORT.INTCTRL = ECHO_INT_EN;	//Low level interrupts
	PMIC.CTRL |=  PMIC_LOLVLEN_bm;
	sei();								//activate interrupts
}

long int getDistance(int device) {
	
	time = 0;
	int device_ = (1 << (device-1));
	TRIG_PORT.OUTSET = device_;
	_delay_us(15);
	TRIG_PORT.OUTCLR = device_;
	while (time == 0) {
		asm volatile("nop");
	}
	if (time > 34998) {
		if (it < 1) {
			it++;
			return getDistance(device);
		} else {
			it=0;
			return 1500;
		}
		
	}
	it=0;
	return (time * CONSTANT);
}
