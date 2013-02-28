
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include <string.h>


  //Count. Starts at 48 which is 0 in ASCII.
  int count = 48;
  int currentCount = 48;



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

//Function that gets called if anything is received in Port G. 
void intPortG(){
  
  //Clears the interrupt.
  GPIOPinIntClear(GPIO_PORTG_BASE, 0x06);

  //If Pin 2 has a 1, it increments the count.
  if(GPIOPinRead(GPIO_PORTG_BASE, GPIO_PIN_2)==0x04){
    count++;
  }
  
  //If Pin 1 has a 1, it decrements the count.
  if(GPIOPinRead(GPIO_PORTG_BASE, GPIO_PIN_1)==0x02){
    count--;
  }
}

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

//Prints the char on the LCD. (Port A, Pins 0 to 7)
void printCharLCD(char myChar){
  
  //Sets up LCD to print characters.
  GPIOPinWrite(GPIO_PORTD_BASE, (GPIO_PIN_0 | GPIO_PIN_2), 0x01);
  
  //Prints the myChar character.
  GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, myChar);
  
  //Toggles the enable to write the char.
  toggleEnable();
  
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
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);

    // Enable the peripherals used by the application.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);

    //Sets Port D and Port A as output.
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, 0x05);
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, 0xFF);
    

    //Sets Port G as input.
    GPIOPinTypeGPIOInput(GPIO_PORTG_BASE, 0x06);
    

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
    
    //Writes the initial character.
    moveLine(true);
    printCharLCD((char)(count));
    
    
     /*==========
    *Interrupt configuration
    */
    
    //Sets Port G as interrupt
    GPIOPinIntEnable(GPIO_PORTG_BASE, (GPIO_PIN_2 | GPIO_PIN_1));
    
    //Gets Port G's interrupt function
    GPIOPortIntRegister(GPIO_PORTG_BASE, intPortG);
    
    //Sets Interrupt as Rising Edge
    GPIOIntTypeSet(GPIO_PORTG_BASE, (GPIO_PIN_2 | GPIO_PIN_1), GPIO_RISING_EDGE);
    
    /*===========
    *Interrupt configuration end
    */
    
    // Loop forever.
    while(1)
    {
      if(currentCount != count){
        moveLine(true);
        printCharLCD((char)(count));
        currentCount = count;
      }
    }
}
