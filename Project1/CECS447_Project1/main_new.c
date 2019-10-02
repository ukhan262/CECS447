// CECS 447 Project 1
// Alex Wall - 013924948
// Umar Khan - 
// Due: 02/27/19

#include "TExaS.h"
#include "tm4c123gh6pm.h"

//Systick
#define NVIC_ST_CTRL_R          (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R        (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R       (*((volatile unsigned long *)0xE000E018))
#define NVIC_ST_CTRL_COUNT      0x00010000  // Count flag
#define NVIC_ST_CTRL_CLK_SRC    0x00000004  // Clock Source
#define NVIC_ST_CTRL_INTEN      0x00000002  // Interrupt enable
#define NVIC_ST_CTRL_ENABLE     0x00000001  // Counter mode
#define NVIC_ST_RELOAD_M       	0x00000141  // Counter load value
																						// (1/440Hz)/(1/16MHz) = 36,363 = 0x8E0B
																						// 0x8E0B / 256 = 0x8E = 141


unsigned long In;  	// input from PF4,PF0
unsigned long mode; // Current mode 
unsigned long i;		// variable used for loops
unsigned char Index;
// mode 1 = sawtooth
// mode 2 = triangle
// mode 3 = sine
// mode 4 = square

const unsigned char SineWave[256] = {128,131,134,137,140,143,146,149,152,155,158,161,165,167,170,173,176,179,182,185,188,
190,193,196,198,201,203,206,208,211,213,215,218,220,222,224,226,228,230,232,233,235,237,238,240,241,243,244,245,246,248,249,
249,250,251,252,253,253,254,254,254,255,255,255,255,255,255,255,254,254,254,253,253,252,251,250,249,249,248,246,245,244,243,
241,240,238,237,235,233,232,230,228,226,224,222,220,218,215,213,211,208,206,203,201,198,196,193,190,188,185,182,179,176,173,
170,167,165,161,158,155,152,149,146,143,140,137,134,131,127,124,121,118,115,112,109,106,103,100,97,93,90,88,85,82,79,76,73,
70,67,65,62,59,57,54,52,49,47,44,42,40,37,35,33,31,29,27,25,23,21,20,18,17,15,14,12,11,10,9,7,6,5,5,4,3,2,2,1,1,1,0,0,0,0,0,
0,0,1,1,1,2,2,3,4,5,5,6,7,9,10,11,12,14,15,17,18,20,21,23,25,27,29,31,33,35,37,40,42,44,47,49,52,54,57,59,62,65,67,70,73,76,
79,82,85,88,90,93,97,100,103,106,109,112,115,118,121,124};

//   Function Prototypes
void PortB_Init(void);
void PortF_Init(void);
void SysTick_Init(void);
void SysTick_Wait(unsigned long);
void SysTick_Wait10ms(unsigned long);
void DAC_Out(unsigned long);
void EnableInterrupts(void);  // Enable interrupts
void Delay(void);

int main(void){ 
	PortB_Init();
	PortF_Init();
	SysTick_Init();
	mode = 1;
	Index = 0;
	while(1){ 
		
    if(mode == 1){              	
			//sawtooth
			GPIO_PORTF_DATA_R = 0x02; //red
			for (i = 0; i < 256; i++) {
				if (GPIO_PORTB_DATA_R >= 255) {
					GPIO_PORTB_DATA_R = 0x00;
					SysTick_Wait(141);
				}
				else {
					GPIO_PORTB_DATA_R = GPIO_PORTB_DATA_R + 1;
					SysTick_Wait(141);
				}// else
			}// for
		}// mode == 1
		
		else if (mode == 2){  	
			//triangle
			GPIO_PORTF_DATA_R = 0x04; //blue
			GPIO_PORTB_DATA_R = 0x00;
			for (i = 0; i < 255; i++) {
				GPIO_PORTB_DATA_R = GPIO_PORTB_DATA_R + 1;
			}
			GPIO_PORTB_DATA_R = 0xFF;
			for (i = 0; i < 255 ; i++) {
				GPIO_PORTB_DATA_R = GPIO_PORTB_DATA_R - 1;
			}
    }// mode == 2
		
		else if (mode == 3){                  
      //sine
			GPIO_PORTF_DATA_R = 0x08; //green
			Index = (Index+1)&0xFF;  
			DAC_Out(SineWave[Index]);
			SysTick_Wait(141);
			
    }// mode == 3
		
		else if (mode == 4){
			GPIO_PORTF_DATA_R = 0x00; //nothing
			//square
      GPIO_PORTB_DATA_R = 0xFF;
			SysTick_Wait(141);
			GPIO_PORTB_DATA_R = 0x00;
			SysTick_Wait(141);
			
		}// mode == 4
	}// while(1)
}// main

//
void PortB_Init(void) { volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000002;     // B clock
	delay = SYSCTL_RCGC2_R;           // delay
	GPIO_PORTB_CR_R = 0xFF;						// allow changes to PB7-PB0
	GPIO_PORTB_AMSEL_R = 00;					// disable analog function
	GPIO_PORTB_PCTL_R = 0x00000000;   // GPIO clear bit PCTL
	GPIO_PORTB_DIR_R = 0xFF;					// PB7-PB0 Output
	GPIO_PORTB_AFSEL_R = 0x00;				// no alt function
	GPIO_PORTB_DEN_R = 0xFF;					// enable digital pins PB7-PB0
}

void PortF_Init(void){ volatile unsigned long delay; 
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay
	GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0
	GPIO_PORTF_IS_R &= ~0x11;					// PF4,PF0 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x11;      	// PF4,PF0 is not both edges
  GPIO_PORTF_IEV_R &= ~0x11;      	// PF4,PF0 falling edge event
  GPIO_PORTF_ICR_R = 0x11;        	// clear flags 4,0
  GPIO_PORTF_IM_R |= 0x11;        	// arm interrupt on PF4,PF0
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00400000; // priority 2
  NVIC_EN0_R = 0x40000000;       	  // enable interrupt 30 in NVIC
}

void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = NVIC_ST_RELOAD_M;  // maximum reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1
                              // enable SysTick with core clock and interrupts
  NVIC_ST_CTRL_R = 0x07;
  EnableInterrupts();
}

void GPIOPortF_Handler(void){
	In = GPIO_PORTF_RIS_R&0x10; 	// read PF4 into In
	Delay();	// Wait 20ms for button
	if(In == 0x00) {
		if (mode >= 4) {
			mode = 1;
		} // if
		else {
			mode = mode + 1;
		} // else 
	} // if
	GPIO_PORTF_ICR_R = 0x00; //acknowledge 
} 

void SysTick_Wait(unsigned long delay){
  volatile unsigned long elapsedTime;
  unsigned long startTime = NVIC_ST_CURRENT_R;
  do{
    elapsedTime = (startTime-NVIC_ST_CURRENT_R)&0x00FFFFFF;
  }
  while(elapsedTime <= delay);
}

void DAC_Out(unsigned long data){
  GPIO_PORTB_DATA_R = data;
}

void Delay(void){unsigned long volatile time;
  time = 28545*5;  // 20msec
  while(time){
		time--;
  }
}
