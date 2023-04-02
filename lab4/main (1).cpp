#include "stm32f3xx.h"
// #include "pinAccess.h"
// #include "acess_mcp.cpp"
#include "spi.h"
#include "tft.h"
#include "Adafruit_GFX.h" //tft - font

typedef void (* mcpCallBack )();
bool direction = 1;

//write to a MCP register, using spi.
void writeReg( uint8_t reg, uint8_t val){
    beginTransaction();

    transfer8(0x40);       // 0100 000 0 (0x40)-  Device Op code
    transfer8(reg);        // Register Address 
    transfer8(val);        // Value to be writen in the registers 

    endTransaction();
}

//read a MCP register , using spi. 
uint8_t readReg(uint8_t reg){
    beginTransaction();

    transfer8(0x41);                    // 0100 000 1 (0x41)-  Device Op code
    transfer8(reg);                     // Register Address  
    uint8_t val = transfer8(0x00);      // Read of values of registers

    endTransaction();

    return val;
}

void setBitInReg(uint8_t reg, uint8_t bitNum){

    uint8_t valActual = readReg(reg);             // Read values of all register pins
    uint8_t val = valActual | 0x1<<bitNum;        // Mask value to set only one bit in the register 
    writeReg(reg, val);                             

}

void clearBitInReg(uint8_t reg, uint8_t bitNum){

    uint8_t valActual = readReg(reg);             // Read values of all register pins
    uint8_t val = valActual & ~(0x1<<bitNum);     // Mask value to clear only one bit in the register 
    writeReg(reg, val);

}

class mcp23s17 { 
    public:
        enum port {PORTA=0, PORTB=1};
        enum mode {OUTPUT=0,INPUT=1,INPUT_PULLUP=2}; 
        enum itType {RISING, FALLING, BOTH};
        enum reg { //use mode 0 (default)
            IODIRA = 0x0,    //Set mode (OUTPUT/INPUT) of pins of port A 
            IPOLA = 0x2,
            GPINTENA = 0x4,
            DEFVALA = 0x6,
            INTCONA = 0x8,
            IOCON = 0xA,
            GPPUA = 0xC,     //Set PullUp
            INTFA = 0xE,    
            INTCAPA = 0x10,
            _GPIOA = 0x12,   //Set Values of register pins of port A
            OLATA = 0x14
        }; 
        mcpCallBack callbacks [8];
        itType typesCallbacks [8];
    public:
        // Constructor 
        mcp23s17 () {
            begin();
        };
        // Start SPI communication
        void begin (){
            setupSPI();
            
        };

        // Set the mode of a specific pin 
        void pinMode( port p, unsigned char bitNum , mode m);
        // Write the logical value in a specific pin
        void digitalWrite(port p, unsigned char bitNum , bool value );
        // read the whole port.
        uint8_t readBits(port p);
        //attach an interrupt to an input pin (port/bitNum)
        void attachInterrupt(port p, uint8_t bitNum , itType type , mcpCallBack callback);

        
};

void mcp23s17::digitalWrite( port p, unsigned char bitNum , bool value ) {
    if(value){
        setBitInReg(reg::_GPIOA + p, bitNum);    
    } else {
        clearBitInReg(reg::_GPIOA + p, bitNum);
    }
}

uint8_t mcp23s17::readBits(port p){
    uint8_t val = readReg(reg::_GPIOA + p);
    return val;
}

void mcp23s17::pinMode( port p, unsigned char bitNum , mode m) {
    switch (m)
    {
    case OUTPUT:
        clearBitInReg(reg::IODIRA + p, bitNum);
        break;
    case INPUT:
        setBitInReg(reg::IODIRA + p, bitNum);
        clearBitInReg(reg::GPPUA + p, bitNum);
        break;
    case INPUT_PULLUP:
        setBitInReg(reg::IODIRA + p, bitNum);
        setBitInReg(reg::GPPUA + p, bitNum);
        break;
    }
}

void mcp23s17::attachInterrupt(port p, uint8_t bitNum , itType type , mcpCallBack callback){

    if(p == port::PORTB){
        callbacks[bitNum] = callback;
        typesCallbacks[bitNum] = type;
    }

}

mcp23s17 ioExt;

void EXTI9_5_IRQHandler(){
    Tft.setTextCursor(Tft.getTextWidth()/2-2,4);
	Tft.eraseText(4);	//remove previous value (4 digits)
	Tft.print("ESOTU AQUI");
    uint8_t valuePins = ioExt.readBits(ioExt.port::PORTB);
    uint8_t intPins = readReg(ioExt.reg::INTCAPB + ioExt.port::PORTB);    //Read the register of interrupt
    
    uint8_t i = 0;

    while(i < 8) {
        if ((intPins >> i) & 0x1) {
            if(&ioExt.callbacks[i] != 0){
                if(ioExt.typesCallbacks[i] == ioExt.itType::BOTH)
                {
                    ioExt.callbacks[i]();
                } 
                else if ((ioExt.typesCallbacks[i] == ioExt.itType::RISING) &&
                        ((valuePins>> i) & 0x1))
                {
                    ioExt.callbacks[i]();
                }
                else if ((ioExt.typesCallbacks[i] == ioExt.itType::FALLING) &&
                        ((valuePins>> i) & 0x0))
                {
                    ioExt.callbacks[i]();
                }
            }
            break;
        }
        i++;
    }
    EXTI->PR |= EXTI_PR_PR9;  /* mandatory! */ 
}

void callback4 () {
    direction = !direction;
}

void wait() {
    volatile int i = 0;
    for (i = 0; i < 2000000; i++);
}

void setup() {
    
    writeReg(0x00, 0x00);

    RCC->AHBENR |= RCC_AHBENR_GPIOAEN_Msk;
	__asm("nop");	//make sure clock is ok.
    
    EXTI->RTSR |= EXTI_RTSR_TR9; //rising on exti9
    EXTI->FTSR |= EXTI_FTSR_TR9; //falling on exti9
    SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI9_PA; //port A for exti9 
    NVIC_SetPriority (EXTI9_5_IRQn , 0 ) ; //NVIC config 
    NVIC_EnableIRQ(EXTI9_5_IRQn);
    EXTI->IMR |= EXTI_IMR_MR9;
   
    Tft.setup();
	Tft.setTextCursor(4, 1); //col,line
	Tft.print("Coro MICRO! ");

    ioExt.attachInterrupt(ioExt.port::PORTB, 4, ioExt.itType::RISING, callback4);
}

/* main function */
int main(void) {
    setup();

    
    int8_t i = 0;

    while(i <= 7) {
        ioExt.pinMode(ioExt.port::PORTA,i,ioExt.mode::OUTPUT);            // Set pins of PortA as OUTPUT
        ioExt.pinMode(ioExt.port::PORTB,i,ioExt.mode::INPUT_PULLUP);      // Set pins of PortB as INPUT and PULLUP in each port
        ioExt.digitalWrite(ioExt.port::PORTA,i,0);                        // Reset the pins of PortA  
        i++;
    }

    /* Infinite loop */
    while (1) {
        
        i = 0;

        // Chaser application
        while(i <= 7) {
            ioExt.digitalWrite(ioExt.port::PORTA,i,1);      
            wait();
            ioExt.digitalWrite(ioExt.port::PORTA,i,0);

            Tft.setTextCursor(Tft.getTextWidth()/2-2,4);
	        Tft.eraseText(4);	//remove previous value (4 digits)
	        Tft.print(ioExt.readBits(ioExt.port::PORTB) & (0x1<<6));

            if(direction){
                i++;
                if(i == 8) i=0;
            } else {
                i--;
                if(i == -1) i=7;
            }
            
            // Read the switch input0 of portB to determine the direction of the chaser application
            if(ioExt.readBits(ioExt.port::PORTB) & 0x1){
                direction = 1;
            } else {
                direction = 0;
            } 
        }
    }
}

