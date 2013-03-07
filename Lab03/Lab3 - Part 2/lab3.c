
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include <stdio.h>



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

    
void intTimerA(void){
  
        TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
        GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, ~(GPIOPinRead(GPIO_PORTG_BASE, GPIO_PIN_0)));
        
}

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
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    
    //Sets Port G as input.
    GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_0);

    
    //Configure the timer
    TimerConfigure(TIMER0_BASE, TIMER_CFG_A_PERIODIC);
    
    
    //2^16 = 65 535.
    //Since our MCU is running at 8MHz, and we want to tick every 1024Hz:
    //8MHz = 125ns
    //1024Hz = 977us
    //977us / 125ns = 7813
    // 7813
    TimerLoadSet(TIMER0_BASE, TIMER_A, 7813);
    
    TimerEnable(TIMER0_BASE, TIMER_A);
    
    TimerIntRegister(TIMER0_BASE, TIMER_A, intTimerA);
    
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    

    
    // Loop forever.
    while(1)
    {
      
    }
}
