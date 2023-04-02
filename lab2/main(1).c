#include "stm32f3xx.h"
#include "pinAccess.h"
#include "adc.h"
#include "stdio.h"

#define OFF 0
#define PUSH 1
#define ON 2
#define RELEASE 3
short int state;
char z;
short unsigned int count_seqs = 0;
const unsigned char seq[]={8,0xc,4,6,2,3,1,9};

static int n = 0;
void wait() {
    volatile int i = 0;
    for (i = 0; i < 2000000; i++);
}

void setup() {
	// pinMode(GPIOB,3,OUTPUT);
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN_Msk;
	__asm("nop");	//make sure clock is ok.

    RCC->AHBENR |= RCC_AHBENR_GPIOBEN_Msk;
	__asm("nop");	//make sure clock is ok.

    //GPIOA->MODER = 0x00000000; 
    GPIOA->MODER |= 0x1 << (2*8); //Setting the mode of PB8 as output
    GPIOA->MODER |= 0x1 << (2*7); //Setting the mode of PB7 as output
    GPIOA->MODER |= 0x1 << (2*6); //Setting the mode of PB6 as output
    GPIOA->MODER |= 0x1 << (2*5); //Setting the mode of PB5 as output

    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN; 
    __asm("nop");
    RCC->APB1RSTR |=  RCC_APB1RSTR_TIM6RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM6RST; // 0: No effect
    __asm("nop");

    GPIOB->MODER &= ~(0x3U << 2); //Setting 00 in the mode of PB1 to be a input
    GPIOB->PUPDR |= 1 << 2; //Setting the 01 in the pullUp of PB1

    //Timer for 40 hz
    TIM6->PSC = 64-1; // prescaler
    TIM6->CR1 |= TIM_CR1_CEN; // config reg : enable
    //TIM6->ARR = 25000-1; 
    
    ADCInit();



}
void stepCCW(){
    //GPIOA->ODR = (GPIOA->ODR | (seq[n]<<5)) & (seq[n]<<5);// GPIOA = GPIOA | (8<<5) & 8<<5 - Questao 3 - ok

    
    GPIOA->BSRR |= (seq[n]<<5) | ((~seq[n])<<(5+16));// GPIOA = GPIOA | (8<<5) & 8<<5        - Questao 4 - ok
    
    /*if((GPIOB->IDR & 0x02) == 0)
    {
        n--;
        if (n == -1) n = 7;
    }
    else
    {
        n++;
        if (n == 8) n = 0;
    }*/
    
}



/* main function */
int main(void) {
    setup();
    /* Infinite loop */
    while (1) {
        /* Add application code here */


        
        TIM6->SR &= ~TIM_CR1_CEN; //reinit overflow flag => start timer motorStep(); //application stuff, when the
        
        TIM6->ARR = ((500-25000)*ADCRead())/4095+25000;
        stepCCW(); //Update motor
        while(! (TIM6->SR & TIM_SR_UIF)); // now wait ! }

        switch (state)
        {
        case OFF:
            if((GPIOB->IDR & 0x02) == 0){
                state = PUSH;
            }
            break;
        case PUSH:
            state = ON;
            z = 0x1;
            break;
        case ON:
            if((GPIOB->IDR & 0x02) == 0x02){
                state = RELEASE;
            }
            break;
        case RELEASE:
            state = OFF;
            break;
        }
      
        if(z){
            n--;
            if (n == -1){ 
                n = 7;
                count_seqs++;
                if (count_seqs == 64) z = 0;
            }
        }
        //else
        //{
        //    n++;
        //    if (n == 8) n = 0;
        //}
        
    }
}

