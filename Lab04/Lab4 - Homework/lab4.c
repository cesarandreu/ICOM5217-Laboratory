
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include <stdio.h>
#include <math.h>

//Result of the calculator
int result = 0;

//Says if the screen needs to be cleared or not.
//If it's a 1, it needs to be cleared.
int clear = 0;

//Display[0] is left, Display[1] is right
int displayValue[2] = {0x3F, 0x3F};
int display[2] = {0, 0};

//The number in the array represents the number
int valueArray[16] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

//Left 1, Right 0
int position = 0;

//Last button pressed
int lastPress = 0;

//Expression
//If it's 0 then no expression has been set
//If it's 1 then it's addition
//If it's 2 then it's subtraction
int expression = 0;

void rollValue(void){
  displayValue[1] = displayValue[0];
  display[1] = display[0];
}

//Timer that draws one display every millisecond. 
void timer1Int(void){
  TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
  
  if(position==0){
    
    GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, ~(displayValue[0]));
    GPIOPinWrite(GPIO_PORTG_BASE, (GPIO_PIN_0|GPIO_PIN_1), 0x01);
    position=1;
      
  }
  else{
      GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, ~(displayValue[1]));
      GPIOPinWrite(GPIO_PORTG_BASE, (GPIO_PIN_0|GPIO_PIN_1), 0x02);
      position=0;
  }
  
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
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    
   
    //Configures Timer 1 to interrupt every millisecond
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER1_BASE, TIMER_BOTH, 8000);  
    TimerEnable(TIMER1_BASE, TIMER_BOTH);
    TimerIntRegister(TIMER1_BASE, TIMER_BOTH, timer1Int);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    
    //Sets Port A as output.
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, 0xFF);

    //Sets Port G as output
    GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, (GPIO_PIN_0 | GPIO_PIN_1));
    
    //Sets Port D as input
    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, (GPIO_PIN_0 | GPIO_PIN_1));
    
    //Sets Port F Pins 1-4 to output
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, (GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4));
    
    //Sets Port F Pins 5-7 to input
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, (GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7));
    
    //Sets Port G's Pin 1 and Pin 0 to zero.
    GPIOPinWrite(GPIO_PORTG_BASE, (GPIO_PIN_0|GPIO_PIN_1), 0x00);
    
        
    
    // Loop forever.
    while(1)
    {

    if(clear==0){
   
   //******************************
  //Pin 1 of Port F is set to high
  //******************************
  GPIOPinWrite(GPIO_PORTF_BASE, 0xFF, GPIO_PIN_1);
    lastPress = GPIOPinRead(GPIO_PORTF_BASE, (GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7));
    switch(lastPress){
      case 0x80: //#3 is being pressed
        rollValue();
        display[0] = 3;
        displayValue[0] = valueArray[3];
        break;
      case 0x40: //#2 is being pressed
        rollValue();
        display[0] = 2;
        displayValue[0] = valueArray[2];
        break;
      case 0x20: //#1 is being pressed
        rollValue();
        display[0] = 1;
        displayValue[0] = valueArray[1];
        break;
    }
    
    while(GPIOPinRead(GPIO_PORTF_BASE, (GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7))==lastPress && lastPress!=0x00){
    }
    
  //******************************
    
  //******************************
  //Pin 2 of Port F is set to high
  //******************************
  GPIOPinWrite(GPIO_PORTF_BASE, 0xFF, GPIO_PIN_2);
    lastPress = GPIOPinRead(GPIO_PORTF_BASE, (GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7));
    switch(lastPress){
      case 0x80: //#6 is being pressed
        rollValue();
        display[0] = 6;
        displayValue[0] = valueArray[6];
        break;
      case 0x40: //#5 is being pressed
        rollValue();
        display[0] = 5;
        displayValue[0] = valueArray[5];
        break;
      case 0x20: //#4 is being pressed
        rollValue();
        display[0] = 4;
        displayValue[0] = valueArray[4];
        break;
    }
    
    while(GPIOPinRead(GPIO_PORTF_BASE, (GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7))==lastPress && lastPress!=0x00){
    }
  //******************************
    
  //******************************
  //Pin 3 of Port F is set to high
  //******************************
  GPIOPinWrite(GPIO_PORTF_BASE, 0xFF, GPIO_PIN_3);
    lastPress = GPIOPinRead(GPIO_PORTF_BASE, (GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7));
    switch(lastPress){
      case 0x80: //#9 is being pressed
        rollValue();
        display[0] = 9;
        displayValue[0] = valueArray[9];
        break;
      case 0x40: //#8 is being pressed
        rollValue();
        display[0] = 8;
        displayValue[0] = valueArray[8];
        break;
      case 0x20: //#7 is being pressed
        rollValue();
        display[0] = 7;
        displayValue[0] = valueArray[7];
        break;
    }
    
    while(GPIOPinRead(GPIO_PORTF_BASE, (GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7))==lastPress && lastPress!=0x00){
    }
    
  //******************************
    
  //******************************
  //Pin 4 of Port F is set to high
  //******************************
  GPIOPinWrite(GPIO_PORTF_BASE, 0xFF, GPIO_PIN_4);
    lastPress = GPIOPinRead(GPIO_PORTF_BASE, (GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7));
    switch(lastPress){
      case 0x80: //# is being pressed
        expression=2;
        break;
      case 0x40: //0 is being pressed
        rollValue();
        display[0] = 0;
        displayValue[0] = valueArray[0];
        break;
      case 0x20: //* is being pressed
        expression=1;
        break;
    }
    
    while(GPIOPinRead(GPIO_PORTF_BASE, (GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7))==lastPress && lastPress!=0x00){
    }
    
  //******************************
      
    }
    
  //******************************
  // Reads the button
  //******************************
    
    //Clears the screen.
    if(GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_1)==0x02){
      clear=0;
      expression=0;
      displayValue[0] = valueArray[0];
      displayValue[1] = valueArray[0];
    }
    
    //Evaluates the expression
    if(GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_0)==0x01){
      
      //Addition
      if(expression==1){
        result = display[0] + display[1];
        displayValue[0] = valueArray[result % 10];
        displayValue[1] = valueArray[result / 10];
        clear=1;
      }
      
      //Subtraction
      if(expression==2){
        result = display[1] - display[0];
        result = abs(result);
        if(display[1] < display[0]){
          displayValue[1] = 0x40;
        }
        
        if(display[1] >= display[0]){
          displayValue[1] = valueArray[0];
        }
        displayValue[0] = valueArray[result % 10];
        clear=1;
      }
      
      
    }
    
  SysCtlDelay(100000);
    
    }
}
