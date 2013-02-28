
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include <string.h>
#include <stdbool.h>


/*
LCD Display
  Pin 1: Ground
  Pin 2: VCC (4.5V)
  Pin 3: Ground
  Pin 4: Register Select (Port D, Pin 0)
  Pin 5: Read/Write (Ground)
  Pin 6: Enable (Port D, Pin 2)
  Pin 7-14: Data Bus (Port A, Pin 0-7)
*/

  //Count. Starts at 48 which is 0 in ASCII.
  int count = 48;
  int currentCount = 48;
  
  bool btnFlagPin2 = false;
  bool btnFlagPin1 = false;

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

  /* Approach 1
  //*************************************************
  //If Pin 2 has a 1, it increments the count.
  if(GPIOPinRead(GPIO_PORTG_BASE, GPIO_PIN_2)==0x04){
    btnFlagPin2 = true;
  }
  
  //If Pin 1 has a 1, it decrements the count.
  if(GPIOPinRead(GPIO_PORTG_BASE, GPIO_PIN_1)==0x02){
    btnFlagPin1 = true;
  }
  //*************************************************
  */
  
  //Approach 2
  //*************************************************
  if(GPIOPinRead(GPIO_PORTG_BASE, GPIO_PIN_2)==0x04){
    SysCtlDelay(800000);
    if(GPIOPinRead(GPIO_PORTG_BASE, GPIO_PIN_2)==0x04){
    count++;
    }
  }
  
  if(GPIOPinRead(GPIO_PORTG_BASE, GPIO_PIN_1)==0x02){
    SysCtlDelay(800000);
    if(GPIOPinRead(GPIO_PORTG_BASE, GPIO_PIN_1)==0x02){
    count--;
    }
  }
  //*************************************************
 
}

//Toggles the Enable. (Pin 2)
void toggleEnable(){

  GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 0x04);
  SysCtlDelay(50000);
    
  GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2,0x00);
  SysCtlDelay(50000);
}

//Moves to the top line when it's true, bottom line when it's false. 
void moveLine(bool top){
  
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
      
/* Approach Number 1
 ****************************************************************************************
      //If currentCount is not equal to count, it draws count and updates currentCount
      //currentCount is the count being shown in the LCD Display
      //count is the number of times the button has been pressed
      if(currentCount != count){
        moveLine(true);
        printCharLCD((char)(count));
        currentCount = count;
      }
      
      //
      // MCU at 80MHz:
      //         SysCtlDelay has a value of 800000.
      //         This is because the MCU is running at 80MHz, so that's 1.25e-8 seconds.
      //         SysCtlDelay takes 3 cycles per loop.
      //         So each loop in SysCtlDelay is 3.75e-8 seconds. 
      //         We want to wait 30e-3 seconds, so we divide these two numbers.
      //         30e-3 / 3.75e-8 = 800,000 loops.
      

      
      if(btnFlag1==true){
        
        btnFlag1=false;
        SysCtlDelay(800000);
        if(btnFlag1==true){
          count--;
          btnFlag1=false;
        }
        
        btnFlag1=false;
      }
      
      if(btnFlag2==true){
        
        btnFlag2=false;
        SysCtlDelay(800000);
        if(btnFlag2==true){
          count++;
          btnFlag2=false;
        }
        
      }
 //*************************************************************************************     
 */
      
 //Approach 2
 //*************************************************************************************
      //If currentCount is not equal to count, it draws count and updates currentCount
      //currentCount is the count being shown in the LCD Display
      //count is the number of times the button has been pressed
      if(currentCount != count){
        moveLine(true);
        printCharLCD((char)(count));
        currentCount = count;
      }
      
 //*************************************************************************************
      
    }
}
