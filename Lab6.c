// Lab6.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 3-key digital piano
// MOOC lab 13 or EE319K lab6 starter
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 3/5/18 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********


#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"
#define		B7	10124		//246.9 Hz
#define		C7	9556		//261.6 Hz
#define		D0	8513		//293.7 Hz
#define		EF0 8035		//311.1 Hz
#define		E0	7584		//329.6 Hz	
#define		F0	7159		//349.2 Hz
#define		FS0 6757		//370   Hz
#define		G0	6378		//392   Hz
#define		AF0	6020		//415.3 Hz
#define		A0	5682		//440   Hz
#define		BF0	5363		//466.2 Hz
#define		B0	5062		//493.9 Hz
#define		C0	4778		//523.3 Hz
#define		D		4257		//587.3 Hz
#define		EF	4018		//622.3 Hz
#define		qtr		2
#define		eth		4
#define		half	1
#define		rest	128

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Song_Play(uint32_t notes[], uint32_t lengths[], uint16_t size);

uint32_t cycles[8] = {0,C0,D0,E0,F0,G0,A0,B0};
uint32_t heartbeatCount = 0;

uint32_t songNotes[46] = {G0,C0,EF,D,C0,EF,C0,D,C0,AF0,BF0,G0,0,G0,C0,EF,D,C0,EF,C0,
		D,C0,G0,FS0,F0,0,F0,AF0,B0,D,F0,AF0,B0,C0,C7,EF0,BF0,AF0,G0,BF0,AF0,AF0,G0,G0,B7,C7};
uint32_t noteTimes[46] = {qtr,eth,eth,eth,eth,eth,eth,eth,eth,eth,eth,half,rest,qtr,eth,eth,eth,eth,eth,
		eth,eth,eth,eth,eth,half,rest,qtr,eth,eth,half,qtr,eth,eth,half,qtr,eth,eth,eth,eth,eth,eth,eth,eth,eth,eth,half};

void heartbeat_init(){
		volatile uint16_t dummy;
		SYSCTL_RCGCGPIO_R |= 0x20;
		dummy = 420;
		GPIO_PORTF_DIR_R |= 0x02;
		GPIO_PORTF_DEN_R |= 0x02;
	}

void song_init(){
	GPIO_PORTF_DIR_R &= ~0x11;
	GPIO_PORTF_DEN_R |= 0x11;
}

int main(void){      
  TExaS_Init(SW_PIN_PE3210,DAC_PIN_PB3210,ScopeOn);    // bus clock at 80 MHz
  Piano_Init();
  Sound_Init();
  // other initialization
	heartbeat_init();
	song_init();
  EnableInterrupts();
  while(1){ 
		uint32_t note = Piano_In();
		Sound_Play(cycles[note]);
		GPIO_PORTF_DATA_R ^= (heartbeatCount >> 15);
		heartbeatCount = (heartbeatCount + 1) % 0x10001;
		
		if((GPIO_PORTF_DATA_R & 0x10) == 0x10){
			while(1){
				if((GPIO_PORTF_DATA_R & 0x10) == 0x10){
					Song_Play(songNotes,noteTimes,46);
					break;
				}
			}    
		}
	}
}
