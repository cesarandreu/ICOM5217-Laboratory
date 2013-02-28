
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "drivers/rit128x96x4.h"
#include <string.h>


/*
When you spin it clockwise, the list goes up. (Bottom -> Up)
When you spin it counter-clockwise, the list goes down. (Up -> Down)

*/

    //Array of strings.
    char *line[17] = {
                        "Hello world!! <3",
                        "Bananas are good",
                        "I love you ^_^  ",
                        "Please save me! ",
                        "I'm trapped here",
                        "Are you there?  ",
                        "Test, pls ignore",
                        "Go away         ",
                        "Come back here  ",
                        "Some text line  ",
                        "I'm awesome :D  ",
                        "I like candy    ",
                        "Hey I just met u",
                        "This is crazy   ",
                        "Heres my number ",
                        "Call me, maybe? ",
                        "I went there.   "};

//Current position on the list.
signed long currentPosition = 0;

//Variables to define position
unsigned int oldPos = 0;
unsigned int newPos = 0;

unsigned int identifier = 0;

//This variable represents direction.
//0 means it has not moved, 1 means CW, 2 means CCW
unsigned int direction = 0;

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

//Function that gets called if anything is changed in Port G
void intPortG(){
 
  //Clears interrupts
  GPIOPinIntClear(GPIO_PORTG_BASE, (GPIO_PIN_0 | GPIO_PIN_1));
  
  //Gets new position
  newPos = GPIOPinRead(GPIO_PORTG_BASE, (GPIO_PIN_0|GPIO_PIN_1));
  
  //Making sure I only have the two least significant bits.
  newPos = 0x03 & newPos;
  
  //Sets value of identifier
  identifier = 0;
  identifier = 0x0F & ( oldPos | newPos );
  
  oldPos = newPos;
  oldPos = oldPos << 2;
  
   //If currentNumber is 1, 7, 14, 8: Clockwise
  if( (identifier==1) || (identifier==7) || (identifier==8) || (identifier==14) ){
  
    direction = 1;
    
  }
  //If currentNumber is 2, 4, 11, 13: Counter-Clockwise
  else if( (identifier==2) || (identifier==4) || (identifier==11) || (identifier==13) ){
  
    direction = 2;
    
  }
  //If anything else: No movement
  else{
    
    direction = 0;
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
    
    //Sets Port G as input. Pin 1 and Pin 0
    GPIOPinTypeGPIOInput(GPIO_PORTG_BASE, (GPIO_PIN_1 | GPIO_PIN_0));
    
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
    
    //Gets the initial position
    oldPos = GPIOPinRead(GPIO_PORTG_BASE, (GPIO_PIN_0 | GPIO_PIN_1));
    
    //Shifts oldPos two bits to the left
    oldPos = oldPos << 2;
    
    //Sets up Port G interrupt
    GPIOPortIntRegister(GPIO_PORTG_BASE, intPortG);
    GPIOPinIntEnable(GPIO_PORTG_BASE, (GPIO_PIN_0 | GPIO_PIN_1));
    GPIOIntTypeSet(GPIO_PORTG_BASE, (GPIO_PIN_0 | GPIO_PIN_1), GPIO_BOTH_EDGES);
    
    
    // Loop forever.
    while(1)
    {
      
      //This activates if the up button is pressed.
      if(direction==1){
      moveLine(true);
      printStringLCD(line[(currentPosition + 15) % 17]);
      moveLine(false);
      printStringLCD(line[(currentPosition + 16) % 17]);
      currentPosition = (currentPosition + 16) % 17;
      direction=0;
      
      }
     
      //This activates if the down button is pressed.
      if(direction==2){
      moveLine(true);
      printStringLCD(line[(currentPosition)%17]);
      moveLine(false);
      printStringLCD(line[(currentPosition + 1)%17]);
      currentPosition = (currentPosition + 1)%17;
      direction=0;
      };
      
    }
}
