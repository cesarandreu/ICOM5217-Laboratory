
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include <stdio.h>

    /*
    C = 10213
    D = 9067
    E = 8080
    F = 7653
    G = 6800
    A = 6053
    B = 5387
    */

/*
//Array of musical notes.
int notes[] = {10213, 9067, 8080, 7653, 6800, 6053, 5387};
*/

/*
//Plays the musical note that is sent
void playNote(char note){
  
  switch(note){
    case 'c': 
      GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, 0x01);
      SysCtlDelay(notes[0]);
      GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, 0x00);
      SysCtlDelay(notes[0]);
      break;
    case 'd':
      GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, 0x01);
      SysCtlDelay(notes[1]);
      GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, 0x00);
      SysCtlDelay(notes[1]);
      break;
    case 'e':
      GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, 0x01);
      SysCtlDelay(notes[2]);
      GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, 0x00);
      SysCtlDelay(notes[2]);
      break;
    case 'f':
      GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, 0x01);
      SysCtlDelay(notes[3]);
      GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, 0x00);
      SysCtlDelay(notes[3]);
      break;
    case 'g':
      GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, 0x01);
      SysCtlDelay(notes[4]);
      GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, 0x00);
      SysCtlDelay(notes[4]);
      break;
    case 'a':
      GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, 0x01);
      SysCtlDelay(notes[5]);
      GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, 0x00);
      SysCtlDelay(notes[5]);
      break;
    case 'b':
      GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, 0x01);
      SysCtlDelay(notes[6]);
      GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, 0x00);
      SysCtlDelay(notes[6]);
      break;
    default:
      SysCtlDelay(8000);
      break;
  }
  
}
*/

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

    /*
    //C-C-G-G-A-A-G_, F-F-E-E-D-D-C_, G-G-F-F-E-E-D_, G-G-F-F-E-E-D_, C-C-G-G-A-A-G_, F-F-E-E-D-D-C_. 
    char song[] = {'c','c','g','g','a','a','g','f','f','e','e','d','d','c','g','g','f','f','e','e','d','c','c','g','g','a','a','g','f','f','e','e','d','d','c'};
    int location = 0;
    int count = 0;
    */
    
    unsigned int frequency = 7813;
    
void intPortG(void){
  GPIOPinIntClear(GPIO_PORTG_BASE, 0xFF);
  
  if(GPIOPinRead(GPIO_PORTG_BASE, GPIO_PIN_2) == 0x04 ){
    if(frequency < 65000){
        frequency += 100;
    }
  }
  
  if(GPIOPinRead(GPIO_PORTG_BASE, GPIO_PIN_1) == 0x02){
    if(frequency > 1000){
        frequency -= 100;
    }
  }
  
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
    
    //Sets Port G, Pin 0 as output.
    GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, (GPIO_PIN_0));
    
    //Sets Port G, Pin 1 and Pin 2 and input.
    GPIOPinTypeGPIOInput(GPIO_PORTG_BASE, (GPIO_PIN_1 | GPIO_PIN_2));
    
    //Sets Port G as interrupt
    GPIOPinIntEnable(GPIO_PORTG_BASE, (GPIO_PIN_2 | GPIO_PIN_1));
    
    //Gets Port G's interrupt function
    GPIOPortIntRegister(GPIO_PORTG_BASE, intPortG);
    
    //Sets Interrupt as Rising Edge
    GPIOIntTypeSet(GPIO_PORTG_BASE, (GPIO_PIN_2 | GPIO_PIN_1), GPIO_RISING_EDGE);
    
    
    //Configure the timer
    TimerConfigure(TIMER0_BASE, TIMER_CFG_A_PERIODIC);
    
    
    //The counter will count from zero to whatever value we set.
    //Since our MCU is running at 8MHz, and we want to tick every 1024Hz:
    // X/8000000 = 1/1024
    // X = 7813
    TimerLoadSet(TIMER0_BASE, TIMER_A, frequency);
    
    TimerEnable(TIMER0_BASE, TIMER_A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    

    
    // Loop forever.
    while(1)
    {

      
      if(TimerIntStatus(TIMER0_BASE, true) == TIMER_TIMA_TIMEOUT){
        TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
        GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, ~(GPIOPinRead(GPIO_PORTG_BASE, GPIO_PIN_0)));
        TimerLoadSet(TIMER0_BASE, TIMER_A, frequency);
      }
      

      
    }
}
