#include "stm32f3xx.h"
#include "pinAccess.h"
#define OFF 0
#define PUSH 1
#define ON
#define RELEASE 3

void wait() 
{
    volatile int i = 0;
    for (i = 0; i < 2000000; i++);
}

void setup() 
{
        RCC->AHBENR |= RCC_AHBENR_GPIOAEN_Msk;
        __asm("nop");
        //Configuration of PB3 = LD3 Output LED
        GPIOB->MODER  &= ~ GPIO_MODER_MODER3; //Be sure 00 at Moder3
        GPIOB->MODER  |= GPIO_MODER_MODER3_0; //Output GPIOB3

        // Configuration of PB1 = D6 Input button
        GPIOB->MODER  &= ~ GPIO_MODER_MODER1; //Input GPIOB1
        GPIOB->PUPDR  |= GPIO_PUPDR_PUPDR1_0; //Pullup resistor for GPIOB1

}

/* main function */
int Question4(void) 
{
    setup();
    /* Infinite loop */
    int8_t sc_state;
    while (1) {
        /* Add application code here */

            switch (sc_state)
            {
                case OFF:
                if ((GPIOB->IDR & 0x02) == 0x00)
                {
                    sc_state = PUSH;
                }
                    break;

                case PUSH:
                    sc_state = ON;
                    break;

                case ON:
                if ((GPIOB->IDR & 0x02) == 0x02)
                {
                    sc_state = RELEASE;
                }
                    break;
                
                case RELEASE:
                    sc_state = OFF;
                    break;

            }
            if(sc_state == PUSH)
            {
                GPIOB->ODR ^=0X08;
            }


    }
}

