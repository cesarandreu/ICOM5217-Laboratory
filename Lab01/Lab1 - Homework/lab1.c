
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "drivers/rit128x96x4.h"
#include <string.h>


    //Array of strings.
    char *line[17] = {
                        "Hello world!! <3",
                        "I am a banana!!!",
                        "I like potatoes!",
                        "I am a potato!!!",
                        "You're a potato!",
                        "I am the eggman!",
                        "Are you real????",
                        "Is this real????",
                        "Is this fantasy?",
                        "You're awesome!!",
                        "I'm awesome :D!!",
                        "Banananananana!!",
                        "Batman! nanana!!",
                        "You're Batman!!!",
                        "That's cool bro!",
                        "Cool story bro!!",
                        "You're evil!!!!!"};

//Current position on the list.
signed long currentPosition = 0;

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
  for(i=0; i<=strlen(str); i++){
    
    GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, 0x00);
    
    //Write the ith letter to the Data Lines (Port A, Pins 0-7)
    GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, str[i]);
  
    //Toggle the Enable Bit to write to the LCD
    toggleEnable();
    
    SysCtlDelay(250000);
  }
  
  //Disable RS (Pin 0), and disable E (Pin 2)
  GPIOPinWrite(GPIO_PORTD_BASE, (GPIO_PIN_0 | GPIO_PIN_2), 0x00);
  
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
    
    //Prints the first two lines.
    moveLine(true);
    printStringLCD(line[currentPosition%17]);
    moveLine(false);
    printStringLCD(line[(currentPosition+1)%17]);
    currentPosition = (currentPosition+1)%17;
    
    
    // Loop forever.
    while(1)
    {
      //This activates if the up button is pressed.
      if(GPIOPinRead(GPIO_PORTG_BASE, GPIO_PIN_2) == 0x04){
      moveLine(true);
      //printStringLCD(line[(abs(currentPosition - 2) % 17)]);
      printStringLCD(line[(currentPosition + 15) % 17]);
      moveLine(false);
      //printStringLCD(line[((abs(currentPosition - 1)) % 17)]);
      printStringLCD(line[(currentPosition + 16) % 17]);
      //currentPosition = abs(currentPosition-1) % 17;
      currentPosition = (currentPosition + 16) % 17;
      SysCtlDelay(1000000);
      }
      //This activates if the down button is pressed.
      else if(GPIOPinRead(GPIO_PORTG_BASE, GPIO_PIN_1) == 0x02){
      moveLine(true);
      printStringLCD(line[(currentPosition)%17]);
      moveLine(false);
      printStringLCD(line[(currentPosition + 1)%17]);
      currentPosition = (currentPosition + 1)%17;
      SysCtlDelay(1000000);      
      };
      
      
    }
}
