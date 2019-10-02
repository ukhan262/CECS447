// CECS 447 Project

#include "TExaS.h"
#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "ADCSWTrigger.h"

unsigned long mode; // Current mode 
unsigned long i;		// variable used for loop
volatile unsigned int delay;
volatile unsigned long ADCvalue;	
double time;

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
void freqCal(void);

// mode 1 = sawtooth
// mode 2 = triangle
// mode 3 = square
// mode 4 = sine
// mode 5 = tone generator
	
int main(void){
	PLL_Init();
	SysTick_Init(50);
	PortB_Init();
	PortF_Init();
	ADC0_InitSWTriggerSeq3_Ch1();
	mode = 5;	
  
	
	while(1){
    while (mode == 1){
			 GPIO_PORTF_DATA_R = 0x08; 
			//sawtooth
			for (i = 0; i < 256; i++){
				 GPIO_PORTB_DATA_R++; 
				 Delay(63);
			}// for
		}// mode == 1
		
		while(mode == 2){ 
			 GPIO_PORTF_DATA_R = 0x0A;
			//triangle
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
    }// mode == 2
		
		while(mode == 3){
			 GPIO_PORTF_DATA_R = 0x02;
			//square
				GPIO_PORTB_DATA_R = 0xFF;
			  Delay(8330);
				GPIO_PORTB_DATA_R = 0x00;
        Delay(8330);		
		}// mode == 3
	 
			while(mode == 4){         
			 GPIO_PORTF_DATA_R = 0x08;         
      //sine 
			    for (i = 0; i < 255; i++){
           	 GPIO_PORTB_DATA_R = SineWave[i];
					   Delay(62);
				 }
    }// mode == 4
			
			while(mode == 5){ 
			 GPIO_PORTF_DATA_R = 0x04; 				
      //tone generator 
			   freqCal();
				 for (i = 0; i < 255; i++){
           	 GPIO_PORTB_DATA_R = SineWave[i];
					   Delay(time);
				 }
             				 
    }// mode == 5
	}// while(1)
}// main

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
   delay --; 
    
}
void Delay (unsigned int count){
 delay = count;
 while(delay !=0);
}
//ISR handler for port F
void GPIOPortF_Handler(void){
  if (GPIO_PORTF_RIS_R&0x10){    //SW1 pressed
     GPIO_PORTF_ICR_R = 0x10;    // acknowledge flag4	
		 if (mode >= 5) {
			mode = 1;
			// GPIO_PORTF_DATA_R = 0x04;       // toggle PF2
		} // if
		 else {
			 //GPIO_PORTF_DATA_R = 0x02;       // toggle PF2
		   mode = mode + 1;
		} // else 
	} // if 
}
void freqCal(void){// y =0.0567x+262
	     double freq;
       ADCvalue = ADC0_InSeq3();
		   freq = 0.0567*ADCvalue +262;
			 time = (1/freq/256/0.000001)-3.895;
}
