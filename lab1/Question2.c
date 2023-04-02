#include "stm32f3xx.h"
#include "pinAccess.h"

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
        GPIOB->PUPDR  |= GPIO_PUPDR_PUPDR1_0; //Pullup for GPIOB1

}

/* main function */
int main(void) 
{
    setup();
    /* Infinite loop */
    while (1) {
        /* Add application code here */
        if ((GPIOB->IDR & 0x02) == 0x00) //
        {
            GPIOB->ODR |= 0x08;

        }
        else
        {
            GPIOB->ODR &= ~(0x08);

        }
        
        wait();

    }
}

