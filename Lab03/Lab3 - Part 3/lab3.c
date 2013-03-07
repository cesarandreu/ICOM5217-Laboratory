
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include <stdio.h>
#include <string.h>


unsigned int displayedNumber = 0;
unsigned int numberToDisplay = 0;
unsigned long timerValue = 0;
char string[15];
int len = 0;

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

//Toggles the Enable. (Pin 2)
void toggleEnable(){

  GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 0x04);
  SysCtlDelay(50000);
    
  GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2,0x00);
  SysCtlDelay(50000);
}

//Moves to the top line when it's true, bottom line when it's false. 
void moveLine(_Bool top){
  
  //If it's true you go to the top line.
  if(top==true){
    GPIOPinWrite(GPIO_PORTD_BASE, (GPIO_PIN_0 | GPIO_PIN_2), 0x00);
    GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, 0x80);
    toggleEnable();
    GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, 0x00);
  }
  
  //If it's false you go to the bottom line.
  if(top==false){
    GPIOPinWrite(GPIO_PORTD_BASE, (GPIO_PIN_0 | GPIO_PIN_2), 0x00);
    GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, 0xC0);
    toggleEnable();
    GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, 0x00);
  }
  
}

//Prints the string on the LCD. (Port A, Pins 0 to 7)
void printStringLCD(char* str){
  
  //Enable RS Pin (Pin 0) and disables E (Pin 2)
  GPIOPinWrite(GPIO_PORTD_BASE, (GPIO_PIN_0 | GPIO_PIN_2), 0x01);
               
  //For each letter in the string parameter
  int i;
  
  //We replaced this:   for(i=0; i<=strlen(str); i++){
  for(i=0; i<strlen(str); i++){
    
    GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, 0x00);
    
    //Write the ith letter to the Data Lines (Port A, Pins 0-7)
    GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, str[i]);
  
    //Toggle the Enable Bit to write to the LCD
    toggleEnable();
    
  }
  
  //Disable RS (Pin 0), and disable E (Pin 2)
  GPIOPinWrite(GPIO_PORTD_BASE, (GPIO_PIN_0 | GPIO_PIN_2), 0x00);
  
}

//Function that gets called if anything is changed in Port G
void intPortG(){
 
  //Clears interrupts
  GPIOPinIntClear(GPIO_PORTG_BASE, (GPIO_PIN_2 | GPIO_PIN_1));
  
  if(GPIOPinRead(GPIO_PORTG_BASE, GPIO_PIN_2) == 0x04){
    
     numberToDisplay = (int)(TimerValueGet(TIMER0_BASE, TIMER_A));
     numberToDisplay = (numberToDisplay & 0x00FF);
     numberToDisplay = numberToDisplay % 10;
    
  }
  
  if(GPIOPinRead(GPIO_PORTG_BASE, GPIO_PIN_1) == 0x02){
    
     numberToDisplay = (int)(TimerValueGet(TIMER0_BASE, TIMER_A));
     numberToDisplay = (numberToDisplay & 0x00FF);
     numberToDisplay = numberToDisplay % 10;
     
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
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);

    // Enable the peripherals used by the application.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    //Sets Port D and Port A as output.
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, 0x05);
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, 0xFF);
    
    //Sets Port G as input. Pin 1 and Pin 2.
    GPIOPinTypeGPIOInput(GPIO_PORTG_BASE, (GPIO_PIN_1 | GPIO_PIN_2));
    
    //Sets all in Port D and Port A as zero.
    GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, 0x00);
    GPIOPinWrite(GPIO_PORTD_BASE, 0x05, 0x00);
    
    //**********************
    //LCD Initialization
    //**********************
    //With RS and E at zero, set Data (Port A) to 0x38.
    GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, 0x38);
    toggleEnable();
    
    //With RS and RW at zero, set Data (Port A) to 0x0F.
    GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, 0x0F);
    toggleEnable();
    //**********************
    //End LCD Initialization
    //**********************
    
    //Sets everything to zero.
    GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, 0x00);
    GPIOPinWrite(GPIO_PORTD_BASE, 0x05, 0x00);
    
    
    //Configured Timer zero to only use Timer A.
    //The clock is periodic. 
    TimerConfigure(TIMER0_BASE, TIMER_CFG_A_PERIODIC);
    
    //Makes the timer count down from 2^16 - 1 to 0.
    TimerLoadSet(TIMER0_BASE, TIMER_A, 0xFFFF);
    
    //Turns on Timer zero's Timer A.
    TimerEnable(TIMER0_BASE, TIMER_A);
    
    //Sets up Port G interrupt and enables it for Pin 1 and Pin 2.
    GPIOIntTypeSet(GPIO_PORTG_BASE, (GPIO_PIN_2 | GPIO_PIN_1), GPIO_RISING_EDGE);
    GPIOPortIntRegister(GPIO_PORTG_BASE, intPortG);
    GPIOPinIntEnable(GPIO_PORTG_BASE, (GPIO_PIN_1 | GPIO_PIN_2));
    
    
    len = sprintf(string, "%d", displayedNumber);
    moveLine(true);
    printStringLCD(string);
     
   
    // Loop forever.
    while(1)
    {
      
      //This occurs if the displayed number is different from the number that should be displayed.
      if(displayedNumber != numberToDisplay){
        
        //Turns displayed number into the number to display.
        displayedNumber = numberToDisplay;
        
        //Pushes that number into a string.
        len = sprintf(string, "%d", displayedNumber);
        
        //Moves to the top line and prints the string.
        moveLine(true);
        printStringLCD(string);
        
      }

      
    }
}
