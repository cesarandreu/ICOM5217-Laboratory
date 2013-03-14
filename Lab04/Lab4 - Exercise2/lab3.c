
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include <stdio.h>



//Display[0] is left, Display[1] is right
int displayValue[2] = {0, 0};

//The number in the array represents the number
int valueArray[16] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

int count = 0;


void timerInt(void){
  TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
      GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, ~(valueArray[count]));

      if(count == 15){
        count=0;
      }
      else{
        count++;
      }

}

//Sets the display's value to whatever value is passed
//The first parameter is the value being set
//The second parameter is the display
//0 is the left display
//1 is the right display
void setDisplay(int value, int display){
  displayValue[display] = valueArray[value];
}

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************


#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

    
    

int main(void){
    //
    // If running on Rev A2 silicon, turn the LDO voltage up to 2.75V.  This is
    // a workaround to allow the PLL to operate reliably.
    //
    if(REVISION_IS_A2)
    {
        SysCtlLDOSet(SYSCTL_LDO_2_75V);
    }

    // Set the clocking to run directly from the crystal.
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);
    
    
    // Enable the peripherals used by the application.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    
    //Configures Timer 0 to interrupt every second
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_BOTH, 8000000);
    TimerEnable(TIMER0_BASE, TIMER_BOTH);
    TimerIntRegister(TIMER0_BASE, TIMER_BOTH, timerInt);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
   
    //Sets Port A as output.
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, 0xFF);

    //Sets Port G as output
    GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_0);
    
    //Sets Port G's Pin 0 as 1
    GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, 0x01);
    
    
    // Loop forever.
    while(1)
    {

      
            
      
    }
}
