#include "stm32f3xx.h"
#include "pinAccess.h"
#include "spi.h"
#define setbit 1
#define clearbit 0
enum port {PORTA = 0, PORTB = 1};
enum mode {MCP_OUTPUT = 0,  MCP_INPUT = 1, MCP_INPUT_PULLUP = 2};
enum itType {RISING, FALLING, BOTH};
enum reg { //All possible registers to be accessed
    IODIRA = 0x00,
    IODIRB = 0x01,
    IPOLA  = 0x02,
    IPOLB  = 0x03,
    GPIINTENA = 0x04,
    GPIINTENB = 0x05,
    DEFVALA = 0x06,
    DEFVALB = 0x07,
    INTCONA = 0x08,
    INTCONB = 0x09,
    IOCON = 0x0A,
    GPPUA = 0x0C,
    GPPUB = 0x0D,
    INTFA = 0x0E,
    INTFB = 0x0F,
    INTCAPA = 0x10,
    INTCAPB = 0x11,
    GPIO_A = 0x12,
    GPIO_B = 0x13,
    OLATA = 0x14,
    OLATB = 0x15,

};

    
unsigned char index = 0;
unsigned char dir = 0; 
typedef void (* mcpCallBack)();

mcpCallBack callbacks [8];
enum itType typesCbacks [8];
void wait() 
{
    volatile int i = 0;
    for (i = 0; i < 2000000; i++);
}
//Question1
void writeReg(uint8_t reg, uint8_t val)
{
    beginTransaction(); //CS = 0
    write8(0x40); //Write Mode
    write8(reg); //Where to write
    write8(val); //What to write
    endTransaction(); //CS = 1 
}
uint8_t readReg(uint8_t reg)
{
    beginTransaction(); //CS = 0
    write8(0x41); //Read Mode
    write8(reg); //Which register to read
    uint8_t value = transfer8(0); //read the value in reg
    endTransaction(); //CS = 1 
    return value; //return the vqlue from the function
}

//Question 2
void setBitInReg(uint8_t reg, uint8_t bitNum)
{
    uint8_t data = readReg(reg)|(setbit<<bitNum);
    writeReg(reg, data);
}
void clearBitInReg(uint8_t reg, uint8_t bitNum)
{
    uint8_t data = readReg(reg)&(clearbit<<bitNum);
    writeReg(reg, data);
}

//Question 3
void PinMode(enum port p, unsigned char bitNum, enum mode m)
{
    
    switch (m)
    {
    case MCP_OUTPUT:
            if (p == PORTA)
            {
                clearBitInReg(IODIRA,bitNum);
            }
            else
            {
                clearBitInReg(IODIRB,bitNum);
            }
        break;
    
    case MCP_INPUT:
            if (p == PORTA)
            {
                setBitInReg(IODIRA,bitNum);
                clearBitInReg(GPPUA,bitNum);
            }
            else
            {
                setBitInReg(IODIRB,bitNum);
                clearBitInReg(GPPUB,bitNum);
            }
        break;

    case MCP_INPUT_PULLUP:
            if (p ==PORTA)
            {
                setBitInReg(IODIRA,bitNum);
                setBitInReg(GPPUA,bitNum);                
            }
            else
            {
                setBitInReg(IODIRB,bitNum);
                setBitInReg(GPPUB,bitNum);
            }
        break;

    default:
        break;
    }
}
void DigitalWrite(enum port p, unsigned char bitNum , uint8_t value )
{
    switch (p)
    {
    case PORTA:
        if (value == 1 )
        {
            setBitInReg(GPIO_A,bitNum);
        }
        else
        {
            clearBitInReg(GPIO_A,bitNum);
        }           
        break;

    case PORTB:
        if (value == 1 )
        {
            setBitInReg(GPIO_B,bitNum);
        }
        else
        {
            clearBitInReg(GPIO_B,bitNum);
        }
        break;

    default:
        break;
    }
}

//Question 5
uint8_t readBits(enum port p)
{
    uint8_t data;
    if (p == PORTA)
    {
       data = readReg(GPIO_A);
    }
    else
    {
       data = readReg(GPIO_B);
    }
    return data;
}

//Question 4 && Question 6
void Chaser()
{
    char led;
    for (led = 0; led < 8; led++)
    {
        PinMode(PORTA,led,MCP_OUTPUT);
    }
    pinMode(GPIOB,0, OUTPUT); //TIM6 - input clock = 64MHz.
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN; 
    __asm("nop");
    //reset peripheral (mandatory!)
    RCC->APB1RSTR |= RCC_APB1RSTR_TIM6RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM6RST; 
    __asm("nop");
    TIM6->PSC = 64000-1; //tick@1ms 
    TIM6->ARR = 100-1; //counts 100 ticks
    TIM6->CR1 |= TIM_CR1_CEN;
    TIM6->DIER |= TIM_DIER_UIE;       //local validation
    NVIC_EnableIRQ(TIM6_DAC1_IRQn);   //NVIC validation
}
void TIM6_DAC1_IRQHandler() {
    TIM6->SR &= ~TIM_SR_UIF;
    if (dir == 1)
    {
        if (index>0)
        {
            DigitalWrite(PORTA,index-1,0);
        } 
        DigitalWrite(PORTA,index,1);
        index++;
        if(index==8) index=0;
    }
        if (dir == 0)
    {
        if (index<8)
        {
            DigitalWrite(PORTA,index+1,0);
        } 
        DigitalWrite(PORTA,index,1);
        if(index==0) index=7;
        index--;
    }
}

//Question 7
void EXTI9_5_IRQHandler() 
{
    uint8_t valB = readBits(PORTB);
    uint8_t valA = readReg(INTCAPA + PORTB);    //Read the register of interrupt
    
    uint8_t i = 0;

    while(i < 8) {
        if ((valA >> i) & 0x1) {
            if(&callbacks[i] != 0){
                if(typesCbacks[i] == BOTH)
                {
                    callbacks[i]();
                } 
                else if ((typesCbacks[i] == RISING) &&
                        ((valB >> i) & 0x1))
                {
                    callbacks[i]();
                }
                else if ((typesCbacks[i] == FALLING) &&
                        ((valB >> i) & 0x0))
                {
                    callbacks[i]();
                }
            }
            break;
        }
        i++;
    }
    EXTI->PR |= EXTI_PR_PR9;       
}

void attachInterrupt(enum port p, uint8_t bitNum , enum itType type ,mcpCallBack callback)
{
        if(p == PORTB){
        callbacks[bitNum] = &callback;
        typesCbacks[bitNum] = type;

        
    }

}

void callback()
{
    dir = !dir;     //Change direction of the Chaser
}

void setup() {


    RCC->AHBENR |= RCC_AHBENR_GPIOAEN_Msk;
	__asm("nop");	//make sure clock is ok.
    EXTI->RTSR |= EXTI_RTSR_TR9; //Rising edge on exti9
    EXTI->FTSR |= EXTI_FTSR_TR9; //Falling edge on exti9
    SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI9_PA; //port A for exti9 
    NVIC_SetPriority (EXTI9_5_IRQn , 1 ) ; //NVIC config priority
    NVIC_EnableIRQ(EXTI9_5_IRQn);
    EXTI->IMR |= EXTI_IMR_MR9;

    attachInterrupt(PORTB, 4, FALLING, callback);
}
int main(void) 
{
    setup();
    setupSPI();
    Chaser();
    PinMode(PORTB, 0, MCP_INPUT_PULLUP); //Set PORTB0 as INPUT w PULLUP to read state of the switch
    while (1)
    {
        if (readBits(PORTB) &  0x01)
        {
            dir = 1;
        }
        else
        {   
            dir = 0;
        }
    }
}


