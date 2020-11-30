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

task tasks[5];
const unsigned short num_task=5;
const unsigned long task_period=100;
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
	unsigned char i;
	for(i=0;i<num_task;++i){
		if(tasks[i].elapsedTime>=tasks[i].period){
			tasks[i].state=tasks[i].TickFct(tasks[i].state);
			tasks[i].elapsedTime=0;
		}
		tasks[i].elapsedTime += task_period;
	}
}
ISR(TIMER1_COMPA_vect)
{
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


unsigned char go=0;

enum festival_1{start_1,lights_1};
unsigned char light = 0;
unsigned char temp=0;
int tick_1(int state_1){
	switch(state_1){
		case start_1:
			light = 0;
			if(go==1){
				state_1 = lights_1;
				temp=0;
			}
			break;
		case lights_1:
			if(go==1){
				state_1 = lights_1;
			}
			else{
				state_1=start_1;
			}
			break;
		default:
			break;
	}
	switch(state_1){
		case start_1:
			break;
		case lights_1:
			if(temp<8){
				light = (light|0x01<<temp);
			}

			temp=temp+1;

			if(temp >= 9){
				temp = 0;
				light = 0x00;
			}
			break;
		default:
			break;
	}
	return state_1;
}

enum festival_2{start_2,lights_2};
unsigned char light2=0;
unsigned char temp2=0;
int tick_2(int state_2){
switch(state_2){
    case start_2:
        light2 = 0;
        if(go==2){
            temp2=0;
            state_2 = lights_2;
        }
        break;
    case lights_2:
        if(go==2){
            state_2 = lights_2;
        }
        else{
            state_2 = start_2;
        }
        break;

    default:
        break;
}
switch(state_2){
    case start_2:
            break;
    case lights_2:

        if(temp2 == 0){
            light2 = 0x0F;
            temp2 = 1;
        }
        else if(temp2 == 1){
            light2 = 0xF0;
            temp2 = 0;
        }
        break;
    
    default:
        break;
}

return state_2;

}



enum festival_3{start_3,lights_3};
unsigned char light3=0;
unsigned char temp3=0;
int tick_3(int state_3){
switch(state_3){
    case start_3:
        light3 = 0;
        if(go==3){
            temp3=0;
            state_3 = lights_3;
        }
        break;
    case lights_3:
        if(go==3){
            state_3 = lights_3;
        }
        else{
            state_3 = start_3;
        }
        break;

    default:
        break;
}
switch(state_3){
    case start_3:
            break;
    case lights_3:
	light3 = ((0x01 << temp3)|(0x80 >> temp3));
	if(temp3<7){                 
    		temp3 = temp3 + 1;
	}
	else{
    		temp3 = 0;
	}


    default:
        break;
}
return state_3;

}



#define button0 (~PINA&0x01)
#define button1 ((~PINA>>1)&0x01)


enum buttonPress{start_4,off_1,off_2,on_1,on_2,inc,dec};
int tick_4(int state_4){
	switch(state_4){
		case start_4:
			state_4 = off_1;
			go=0;
			break;

		case off_1:
			if((!button0) && (!button1)){
				state_4 = off_1;
			}
			else if(button0 && button1){
				state_4=off_2;
			}
			break;

		case off_2:
			if((!button0)&&(!button1)){
                state_4 = on_1;
				go=1;
            }
            else if(button0 && button1){
                state_4 = off_2;
            }
            break;

		case on_1:
			if(button0 && button1){
				state_4 = on_2;
			}
			else if((!button0) && (!button1)){
				state_4 = on_1;
			}
			else if(button0 && (!button1)){
				state_4 = inc;
			}
			else if((!button0) && button1){
				state_4 = dec;
			}
			break;

		case on_2:
			if(button0 && button1){
                state_4 = on_2;
            }
			else{
				state_4 = off_1;
				go=0;
			}
			break;

		case inc:
			if(button0 && (!button1)){
				state_4 = inc;
			}
			else if((!button0)&&(!button1)){
				state_4 = on_1;
				if(go<3){
                    go = go + 1;
                }
                else{
                    go = 1;
                }
            }
			else if(button0 && button1){
				state_4 = on_2;
				go=0;
			}
			break;

		case dec:
        if( (!button0) && button1){
                state_4 = dec;
        }
        else if((!button0)&&(!button1)){
            state_4 = on_1;
            if(go > 1){
                go = go - 1;
            }
            else{
                go = 3;
            }
        }
		else if(button0 && button1){
			state_4 = on_2;
			go = 0;
		}
        break;

		default:
		break;
	}


	switch(state_4){
		case off_1:
			go = 0;
			break;
		default:
			break;
	}

	return state_4;
}

enum C_SM{start_c, output};
int tick_c(int state_c){
	switch(state_c){
		case start_c:
			state_c = output;
			break;
		case output:
			state_c = output;
			break;
		default:
			break;
	}
	switch(state_c){
		case start_c:
			break;
		case output:
			if(go == 1){
				transmit_data(light);
			}
			else if(go == 2){
				transmit_data(light2);
			}
			else if(go == 3){
				transmit_data(light3);
			}
			else if(go == 0){
				transmit_data(0x00);
			}
			break;
		default:
			break;
	}
	return state_c;
}

int main(void) {
	DDRA=0x00;PORTA=0xFF;
	DDRC=0xFF;PORTC=0x00;
	unsigned char i=0;

	tasks[i].state = start_1;
	tasks[i].period=200;
	tasks[i].elapsedTime=0;
	tasks[i].TickFct=&tick_1;
	i++;
        tasks[i].state=start_2;
        tasks[i].period=300;
        tasks[i].elapsedTime=0;
        tasks[i].TickFct=&tick_2;
	i++;
	tasks[i].state=start_3;
        tasks[i].period=200;
        tasks[i].elapsedTime=0;
        tasks[i].TickFct=&tick_3;
        i++;
	tasks[i].state = start_4;
        tasks[i].period=100;
        tasks[i].elapsedTime=0;
        tasks[i].TickFct=&tick_4;
        i++;
	tasks[i].state=start_c;
        tasks[i].period=100;
        tasks[i].elapsedTime=0;
        tasks[i].TickFct=&tick_c;
	TimerSet(100);
	TimerOn();
	while (1) {
	}
	return 1;
}
