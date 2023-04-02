#include "stm32f3xx.h"
#include "pinAccess.h"
#define OFF 0
#define PUSH 1
#define ON 2
#define RELEASE 3
#define LEDOFF 0
#define LED01 1
#define LED02 2
#define LED03 3
#define LED04 4
#define LED05 5

void wait() 
{
    volatile int i = 0;
    for (i = 0; i < 2000000; i++);
}

void setup() 
{

        RCC->AHBENR |= RCC_AHBENR_GPIOBEN_Msk; //
        __asm("nop");
        RCC->AHBENR |= RCC_AHBENR_GPIOFEN_Msk; //
        __asm("nop");
        //Configuration of PB3 = LD3 Output LED
        GPIOB->MODER  &= ~ GPIO_MODER_MODER3; //Be sure 00 at Moder3
        GPIOB->MODER  |= GPIO_MODER_MODER3_0; //Output GPIOB3

        // Configuration of PB1 = D6 Input button
        GPIOB->MODER  &= ~ GPIO_MODER_MODER1; //Input GPIOB1
        GPIOB->PUPDR  |= GPIO_PUPDR_PUPDR1_0; //Pullup resistor for GPIOB1

}
void setLed(int ledId){
         switch (ledId)
         {
         case LEDOFF:
             /* ALL HIGH IMPEDANCE OR 0 */
             break;
         case LED01:
             /* PB7-OUTPUT = 1, PF0-OUTPUT = 0, PF1 = Z(High Impedance) OR INPUT */
         GPIOB->MODER  &= ~ GPIO_MODER_MODER7_Msk; //Be sure 00 at Moder7
         GPIOB->MODER  |= GPIO_MODER_MODER7_0; //Output GPIOB7
         GPIOB->ODR = 1<<7; // Put 1 in PB7
         GPIOF->MODER  &= ~ GPIO_MODER_MODER0_Msk; //Be sure 00 at Moder0
         GPIOF->MODER  |= GPIO_MODER_MODER0_0; //Output GPIOF0
         GPIOF->ODR = 0; // Put 0 in PB0
         GPIOF->MODER  &= ~ GPIO_MODER_MODER1_Msk; //Be sure 00 at Moder0
         GPIOF->MODER  |= GPIO_MODER_MODER1_1; //INPUT GPIOF1 or High Impedance could also use open drain output

             break;
         case LED02:
             /* code */
         GPIOB->MODER  &= ~ GPIO_MODER_MODER7_Msk; //Be sure 00 at Moder7
         GPIOB->MODER  |= GPIO_MODER_MODER7_0; //Output GPIOB7
         GPIOB->ODR = 0<<7; // Put 0 in PB7
         GPIOF->MODER  &= ~ GPIO_MODER_MODER0_Msk; //Be sure 00 at Moder0
         GPIOF->MODER  |= GPIO_MODER_MODER0_0; //Output GPIOB7
         GPIOF->ODR = 1; // Put 1 in PB0
         GPIOF->MODER  &= ~ GPIO_MODER_MODER0_Msk; //Be sure 00 at Moder0
         GPIOF->MODER  |= GPIO_MODER_MODER1_1; //INPUT GPIOF1 or High Impedance could also use open drain output
             break;
         case LED03:
             /* code */
         GPIOB->MODER  &= ~ GPIO_MODER_MODER7_Msk; //Be sure 00 at Moder7
         GPIOB->MODER  |= GPIO_MODER_MODER7_1; //HIGH IMP GPIOB7
         //GPIOB->ODR = 0<<7; // Put 1 in PB7
         GPIOF->MODER  &= ~ GPIO_MODER_MODER0_Msk; //Be sure 00 at Moder0
         GPIOF->MODER  |= GPIO_MODER_MODER0_0; //Output GPIOB7
         GPIOF->ODR = 1; // Put 0 in PB0
         GPIOF->MODER  &= ~ GPIO_MODER_MODER0_Msk; //Be sure 00 at Moder0
         GPIOF->MODER  |= GPIO_MODER_MODER1_0; //OUTPUT 
         GPIOF->ODR = 0<<1; // Put 0 in PF1  
             break;
         case LED04:
             /* code */
         GPIOB->MODER  &= ~ GPIO_MODER_MODER7_Msk; //Be sure 00 at Moder7
         GPIOB->MODER  |= GPIO_MODER_MODER7_1; //HIGH IMP GPIOB7
         //GPIOB->ODR = 0<<7; // Put 1 in PB7
         GPIOF->MODER  &= ~ GPIO_MODER_MODER0_Msk; //Be sure 00 at Moder0
         GPIOF->MODER  |= GPIO_MODER_MODER0_0; //Output GPIOB7
         GPIOF->ODR = 1; // Put 0 in PB0
         GPIOF->MODER  &= ~ GPIO_MODER_MODER0_Msk; //Be sure 00 at Moder0
         GPIOF->MODER  |= GPIO_MODER_MODER1_0; //OUTPUT 
         GPIOF->ODR = 0<<1; // Put 0 in PF1  
             break;
         case LED05:
             /* code */
             break;
         
         default:
             break;
         }

        wait();
        wait();
}

/* main function */
int main(void) 
{
    setup();
    /* Infinite loop */
    int led;

    while (1) {
        /* Add application code here */
            led = 1;
            setLed(led);



    }
}

