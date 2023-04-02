#include "stm32f3xx.h"
#include "pinAccess.h"
#define OFF 0
#define PUSH 1
#define ON   2
#define RELEASE 3


const unsigned char seq[]={8,0xc,4,6,2,3,1,9};

void wait() {
    volatile int i = 0;
    for (i = 0; i < 2000000; i++);
}

void setup()
{
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN_Msk;
    __asm("nop");
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN_Msk;
    __asm("nop");
    GPIOA->MODER  &= ~ 0XFFFF; //Be sure 00 at everything
    GPIOA->MODER  |= 0x55 << 10; //Outputs PA5,6,7,8  
    // Configuration of PB1 = D6 Input button
    GPIOB->MODER  &= ~GPIO_MODER_MODER1; //Input GPIOB1
    GPIOB->PUPDR  &= ~GPIO_PUPDR_PUPDR1; //Pullup resistor for GPIOB1
    GPIOB->PUPDR  |= GPIO_PUPDR_PUPDR1_0; //Pullup resistor for GPIOB1
}

void stepCCW()
{
    static uint8_t i = 0;

    //GPIOA->ODR &= ~ 0XFFFF; //Be sure reset bits to 0
    //GPIOA->ODR |= seq[i]<<5;
    //GPIOA->BSRR |= ~ seq[i]<<20;  
    GPIOA->BSRR |= seq[i]<<5 |((~seq[i]<<21)) ;
    i=(i+1)%8; 
}
void stepCW()
{
    static uint8_t n = 7;

    //GPIOA->ODR &= ~ 0XFFFF; //Be sure reset bits to 0
    //GPIOA->ODR |= seq[i]<<5;
    //GPIOA->BSRR |= ~ seq[i]<<20;  
    GPIOA->BSRR |= seq[n]<<5 |((~seq[n]<<21)) ;
    n=(n-1)%8; 
}

void setup_timer10HZ()
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    __asm("nop");
    RCC->APB1RSTR |= RCC_APB1RSTR_TIM6RST_Msk;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM6RST_Msk;
    TIM6->PSC =  64000-1; //TICK AT 1ms
    TIM6->CR1 |= TIM_CR1_CEN;
    TIM6->ARR = 10-1; //each .1s 100 ticks
    
}



/* main function */
int main(void)
{
    setup();
    setup_timer10HZ();

    while (1)
    {
       if(TIM6->SR & TIM_SR_UIF)
       {
         TIM6->SR &= ~TIM_SR_UIF; // reset flag

                if ((GPIOB->IDR & 0x02))
                {
                    stepCCW();
                }
                else
                {
                    stepCW();
                }
                
        }

    }
}

