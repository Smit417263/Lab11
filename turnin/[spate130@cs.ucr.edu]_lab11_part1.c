/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section: 023
 *	Assignment: Lab #11  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: 
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif



typedef struct task{
        int state;
        unsigned long period;
        unsigned long elapsedTime;
        int(*TickFct)(int);
}task;

task tasks[2];
const unsigned short num_task=2;
const unsigned long period_task=100;
volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
void TimerOn() {
	TCCR1B 	= 0x0B;
	OCR1A 	= 125;
	TIMSK1 	= 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B 	= 0x00;
}

void TimerISR() {
	for(unsigned char i = 0;i < num_task; ++i){
		if(tasks[i].elapsedTime >= tasks[i].period){
			tasks[i].state = tasks[i].TickFct(tasks[i].state);
			tasks[i].elapsedTime = 0;
		}
		tasks[i].elapsedTime += period_task;
	}
}

ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}



void transmit_data(unsigned char data) {
    int i;
    for (i = 0; i < 8 ; ++i) {
   	 PORTC = 0x08;
   	 PORTC |= ((data >> i) & 0x01);
   	 PORTC |= 0x02;  
    }
    PORTC |= 0x04;
    PORTC = 0x00;
}


enum Light_Sys{start,read,inc,dec,reset};
unsigned char light = 0x00;;

int tick_1(int state_1){
switch(state_1){
	case start:
		light = 0x00;
		state_1 = read;
		break;

	case read:
		if((~PINA&0x01) & !((~PINA>>1)&0x01)){ 
			state_1 = inc;
			if(light < 0xFF){
				light = light + 1;
			}
		}
		else if((~PINA&0x01)&((~PINA>>1)&0x01)){
			state_1 = reset;
			light = 0x00;
		}
		else if(!(~PINA&0x01)&((~PINA>>1)&0x01)){
			state_1 = dec;
			if(light > 0){
				light = light - 1;;
			}
		}
		else{
			state_1 = read;
		}
		break;


	case inc:
		if((~PINA&0x01) & !((~PINA>>1)&0x01)){
			state_1=inc;
		}
		else if(!(~PINA&0x01) & !((~PINA>>1)&0x01)){
			state_1 = read;
		}
		else if((~PINA&0x01) & ((~PINA>>1)&0x01)){
			state_1 = reset;
			light = 0x00;
		}
		break;


	case dec:
		if(!(~PINA&0x01)&((~PINA>>1)&0x01)){
			state_1 = dec;
		}
		else if(!(~PINA&0x01)&!((~PINA>>1)&0x01)){
			state_1 = read;
		}
		else if((~PINA&0x01)&((~PINA>>1)&0x01)){
			state_1 = reset;
			light = 0x00;
		}
		break;


	case reset:
		if(!(~PINA&0x01)&!((~PINA>>1)&0x01)){
			state_1 = read;
		}
		else{
			state_1 = reset;
		}
		break;
		
	default:
		break;

	// switch(state_1){
	// 	case start:
	// 		break;

	// 	case ready:
			
	// 		break;

	// 	case inc:
	// 		if(light < 0xFF){
	// 			light = light + 1;
	// 		}
	// 		break;

	// 	case dec:
	// 		if(light > 0){
	// 			light = light - 1;;
	// 		}
	// 		break;

	// 	case reset:
	// 		light = 0x00;
	// 		break;

	// 	default:
	// 		break:
	// }
}
return state_1;
}


enum C_SM{init,trans};
int tick_C(int state_c){
	switch(state_c){
		case init:
			state_c = trans;
			break;
		case trans:
			state_c = trans;
			break;
		default:
			break;
	}

	switch(state_c){
		case init:
			break;

		case trans:
			transmit_data(light);
			break;

		default:
			break;
	}
	return state_c;
}

int main(void) {
	DDRA=0x00; PORTA=0xFF;
	DDRC=0xFF; PORTC=0x00;

	unsigned char i=0;
	tasks[i].state = start;
	tasks[i].period=100;
	tasks[i].elapsedTime=0;
	tasks[i].TickFct=&tick_1;
	i++;
	tasks[i].state = init;
    	tasks[i].period=100;
    	tasks[i].elapsedTime=0;
    	tasks[i].TickFct=&tick_C;

	TimerSet(100);
	TimerOn();
	while (1) {}
	return 1;
}
