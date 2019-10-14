// UARTTestMain.c
// Runs on LM4F120/TM4C123
// Used to test the UART.c driver
// Daniel Valvano
// September 12, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013

 Copyright 2013 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// U0Rx (VCP receive) connected to PA0
// U0Tx (VCP transmit) connected to PA1

#include "PLL.h"
#include "UART.h"
#include "tm4c123gh6pm.h"

//---------------------OutCRLF---------------------
// Output a CR,LF to UART to go to a new line
// Input: none
// Output: none
volatile unsigned int delay;
unsigned int i;
char string[20];  // global to assist in debugging  
unsigned long n;
unsigned char c, previous;
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
void Delay (unsigned int);
void EnableInterrupts(void);
void SysTick_Handler(void);
void SysTick_Init(unsigned long period); //Initialize sysTick

void OutCRLF(void){
  UART_OutChar(CR);
  UART_OutChar(LF);
}
//debug code
int main(void){
  
  PLL_Init();
  UART_Init();
	PortB_Init();
	PortF_Init();
	SysTick_Init(50);	
	
	previous = 'r';
	
	while(1){
		
		c = UART_InChar();
		
		if (c =='r') 
		{	
			previous = 'r';
		}
		else if (c =='t') 
		{
			previous = 't';
		}
		else if (c =='s') 
		{
			previous = 's';
		}
		else if (c =='q') 
		{
			previous = 'q';
		}
		else if (c =='0') 
		{
			previous = previous;
		}
		
		if (previous == 'r')
		{
			//sawtooth
			GPIO_PORTF_DATA_R = 0x04;
			for (i = 0; i < 256; i++){
				GPIO_PORTB_DATA_R++; 
				Delay(63);
			}
			
			UART_OutString("sawtooth wave is displayed");
			OutCRLF();
		}
		
		else if (previous == 't')
		{
			//triangle
			GPIO_PORTF_DATA_R = 0x0A;
			
			GPIO_PORTB_DATA_R = 0x00;
			for (i = 0; i < 255; i++) {
				GPIO_PORTB_DATA_R++;
				Delay(31);				
			}
			GPIO_PORTB_DATA_R = 0xFF;
			for (i = 0; i < 255 ; i++) {
				GPIO_PORTB_DATA_R--;
				Delay(31);
			}
			
			UART_OutString("traingle wave is displayed");
			OutCRLF();
		}
		
		else if (previous == 's')
		{
			//sine
			GPIO_PORTF_DATA_R = 0x08;
			
			for (i = 0; i < 255; i++){
				GPIO_PORTB_DATA_R = SineWave[i];
				Delay(62);
			}
			UART_OutString("sine wave is displayed");
			OutCRLF();
		}
		
		else if (previous == 'q')
		{
			//square
			GPIO_PORTF_DATA_R = 0x02;
			
			GPIO_PORTB_DATA_R = 0xFF;
			Delay(8330);
			GPIO_PORTB_DATA_R = 0x00;
      Delay(8330);
			
			UART_OutString("square wave is displayed");
			OutCRLF();
		}
		else{ // 0 case
			// previous = previous
		}
	}				
}


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

// Subroutine to initialize port F pins for input and output
void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay  
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0 
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0   
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R |= 0x10;     //     enable weak pull-up on PF4
  GPIO_PORTF_IS_R &= ~0x10;     // (d) PF4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x10;    //     PF4 is not both edges
  GPIO_PORTF_IEV_R &= ~0x10;    //     PF4 falling edge event
  GPIO_PORTF_ICR_R = 0x10;      // (e) clear flag4
  GPIO_PORTF_IM_R |= 0x10;      // (f) arm interrupt on PF4
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
  EnableInterrupts();           // (i) Clears the I bit
}
// Subroutine to initialize port SysTick
void SysTick_Init(unsigned long period){
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = period-1;// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x0FFFFFFF)|0x40000000; // priority 2
                              // enable SysTick with core clock and interrupts
  NVIC_ST_CTRL_R = 0x07;
  
}
void SysTick_Handler(void){
if (delay > 0)
    delay--; 
    
}
void Delay (unsigned int count){
 delay = count;
 while(delay !=0);
}
