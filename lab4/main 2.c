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

void wait() 
{
    volatile int i = 0;
    for (i = 0; i < 20000; i++); // normal value  i < 2000000
}



//write to a MCP register, using spi.
void writeReg(uint8_t reg, uint8_t val)
{
    beginTransaction(); //CS = 0
    write8(0x40); //Write Mode
    write8(reg); //Where to write
    write8(val); //What to write
    endTransaction(); //CS = 1 
}
//read a MCP register , using spi. 

uint8_t readReg(uint8_t reg)
{
    beginTransaction(); //CS = 0
    write8(0x41); //Read Mode
    write8(reg); //Which register to read
    uint8_t value = transfer8(0); //read the value in reg, 0 is arbitrary
    endTransaction(); //CS = 1 
    return value; //return the vqlue from the function
}

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


int main(void) 

{
    uint8_t up = 0; //variable for changing the bit LED up
    uint8_t down = 7; //variable for changing the bit LED down
    setupSPI();
    //Set PORTA[0:7] as OUTPUTS
    PinMode(PORTA, 0, MCP_OUTPUT);
    PinMode(PORTA, 1, MCP_OUTPUT);
    PinMode(PORTA, 2, MCP_OUTPUT);
    PinMode(PORTA, 3, MCP_OUTPUT);
    PinMode(PORTA, 4, MCP_OUTPUT);
    PinMode(PORTA, 5, MCP_OUTPUT);
    PinMode(PORTA, 6, MCP_OUTPUT); 
    PinMode(PORTA, 7, MCP_OUTPUT);

    PinMode(PORTB, 0, MCP_INPUT_PULLUP); //Set PORTB[0] as INPUT with Pull-up resistor
    while (1) 
    {
        if (readBits(PORTB) &  0x01)
        {
            
            DigitalWrite(PORTA, down, 1);
            if (down<7)
            {
                DigitalWrite(PORTA, down+1,0);
            }
            down = down - 1;;
        }
        else
        {
            DigitalWrite(PORTA, up, 1);
            if (up > 0)
            {
                DigitalWrite(PORTA, up-1,0);
            }
            up = up + 1;
        }  
        if (up == 8)
        {
            up = 0;
        }
        if (down == 0)
        {
            down = 7;
        }
        wait();
    }
}

