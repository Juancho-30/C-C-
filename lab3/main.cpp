#include "stm32f3xx.h"
#include "tft.h"
#include "Adafruit_GFX.h" //tft - font
#include "adc.h"
extern "C" {


static uint16_t measure;
static uint16_t var;
bool isOk;


void setup_GPIO()
{
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN_Msk; //Clock for GPIOA
    __asm("nop");
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // 0x01 << 0 
	__asm("nop");
	SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI10_PA;
	EXTI->RTSR |= EXTI_RTSR_TR10; 	//Enable Rising Edge on Interrupt
	EXTI->FTSR |= EXTI_FTSR_TR10;	//Enable Falling Edge on Interrupt
	NVIC_SetPriority (EXTI15_10_IRQn, 3 ) ; //NVIC config
	NVIC_EnableIRQ(EXTI15_10_IRQn); //NVIC validation
}

void wait() {
    volatile int i = 0;
    for (i = 0; i < 70; i++);
}

void setup_timer10HZ()
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    __asm("nop");
    RCC->APB1RSTR |= RCC_APB1RSTR_TIM6RST_Msk;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM6RST_Msk;
	__asm("nop");
    TIM6->PSC =  64000-1; //TICK AT 1ms
    TIM6->CR1 |= TIM_CR1_CEN;
    TIM6->ARR = 100-1; //each 1ms 100 ticks so 100 ms - 10Hz
	TIM6->DIER |= TIM_DIER_UIE; //local validation
	NVIC_EnableIRQ(TIM6_DAC1_IRQn); //NVIC validation
    
}

void timer7_10HZ()
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
    __asm("nop");
    RCC->APB1RSTR |= RCC_APB1RSTR_TIM7RST_Msk;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM7RST_Msk;
	__asm("nop");
    TIM7->PSC =  640-1; //TICK AT 1us, resolution of 1us
    TIM7->ARR = 5000-1; //each 50ms as timer OVERFLOW
	TIM7->CR1 |= TIM_CR1_CEN;	//enable Timer7
	TIM7->DIER |= TIM_DIER_UIE; //local validation
	NVIC_EnableIRQ(TIM7_DAC2_IRQn); //NVIC validation, enable TIMER overflow interruption
    
}

void TIM6_DAC1_IRQHandler()
{
	TIM6->SR &= ~TIM_SR_UIF; 			//acknowledge, reset flag
    EXTI->IMR &= ~EXTI_IMR_MR10;		//Interrupt Mask register reset EXTI10
	GPIOA->MODER  |= 0x01 << 10*2; 		//Output PA10 
	GPIOA->ODR |= 0x01 << 10; 			// set PA10, sending of trigger signal
	wait();
	GPIOA->ODR &= 0b0 << 10; 			// reset PA10
	GPIOA->MODER  &= ~(3 << 10*2); 		//Input PA10 
    EXTI->IMR |= EXTI_IMR_MR10;			//Interrupt Mask Register set EXTI10

}

//Timer overflow saying sensor is not available if echo doesn't come back before 50ms
void TIM7_DAC2_IRQHandler() 
{
	TIM7->SR &= ~TIM_SR_UIF; 	//acknowledge, reset flag
	var = 0;					//setting var to 0 to don't be able to print
	isOk = 0;					//bool isOk to 0 to don't be able to save measure

}


void EXTI15_10_IRQHandler()
{
	EXTI->PR |= EXTI_PR_PR10; 					//Acknowledge, reset flag

	if (GPIOA->IDR & (1<<10)) 					//Rising edge (sending echo)
    {
		TIM7->CNT = 0; 							//reset timer counter
		NVIC_EnableIRQ(TIM7_DAC2_IRQn);			//Enable timer overflow with sending of echo
		var = 1;								//set int var 1 to be able to print measure
		isOk = 1;								//set bool isOk
	}
	else if ((GPIOA->IDR & ~(1<<10)) && isOk) 	//falling edge (receiving echo) and isOk is 1
	{
		measure = TIM7->CNT; 					//store the value of the counter in measure variable
		measure = measure *10 / 58; 			//Measurement in mm assuming 58 us/cm 
		NVIC_DisableIRQ(TIM7_DAC2_IRQn); 		//Disable timer overflow when we acquire measurement value

	}

}


int main()
{
	setup_GPIO();
	Tft.setup();
	Tft.setTextColor(ST7735_BLUE);
	Tft.setTextCursor(1, 2);
	Tft.print("Distance Measurement:");
	Tft.setTextCursor(2, 5); //set text cursor on next row to display the measurement
	setup_timer10HZ(); //setuptimer6 at 10hz
	timer7_10HZ(); //setuptimer7 at 50ms overflow, resolution at 1us

	while(1)
	{
		Tft.setTextCursor(2, 4);

		if (var == 1 && measure < 532 ) // If we don't have overflow(var=1 and measure is not more thqn 532)
		{	
			Tft.eraseText(15); 			//Erase text, in case of "Not available"
			Tft.println(measure,DEC);	//Print measure in decimal
		}
		else
		{
			Tft.eraseText(2); 				//Erase the measurement numbers only (2 chars)
			Tft.println("Not available");	//Print "Not available"
		}
		for (long int i = 0; i < 200000; i++); //Wait to be able to appreciate the value on screen due to human eye
	}
}
}