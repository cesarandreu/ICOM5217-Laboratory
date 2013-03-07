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


int randomNumber = 0;
char string[16];
int len = 0;
int playerValue = 0;
int triesLeft = 3;
int win = 0;
int lose = 0;

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

//Function that moves to the second line and prints:
//Your guess: NUMBER
//Where number is the number the player wishes to guess. 
void printPlayerValue(){
     moveLine(false);
     sprintf(string, "Your guess: %d ", playerValue);
     printStringLCD(string);
}

//Function tha moves to the second line and prints:
//Tries left: NUMBER
//Where number is the number of tries left.
void printTriesLeft(){
     moveLine(false);
     sprintf(string, "Tries left: %d ", triesLeft);
     printStringLCD(string);
}

void printRandomNum(void){
    moveLine(true);
    sprintf(string, "Random #: %d    ", randomNumber);
    printStringLCD(string);
}

//Function that populates the randomNumber variable with a new random number.
void randomNum(void){
  
    //Gets a random number using the timer.
     randomNumber = (int)(TimerValueGet(TIMER0_BASE, TIMER_A));
     randomNumber = randomNumber % 10;
     
}

//Function redraws the screen.
void redrawScreen(void){
      moveLine(true);
      printStringLCD("Guess # 0 to 9 ");
      printPlayerValue();
}

//Function that gets called if anything is changed in Port G
void intPortG(){
 
  //Clears interrupts
  GPIOPinIntClear(GPIO_PORTG_BASE, (GPIO_PIN_2 | GPIO_PIN_1));
  
  SysCtlDelay(750000);
  
  //If both up and down buttons are pressed
  if(GPIOPinRead(GPIO_PORTG_BASE, (GPIO_PIN_1|GPIO_PIN_2)) == 0x06){
    
    // Checks to make sure the user still has tries left.
    if(triesLeft > 1){
      //Reduces the user's tries and informes them of how many they have left.
        triesLeft--;
        printTriesLeft();
        SysCtlDelay(1330000);
        
        //Checks the user's number to determines if its larger and informes them.
        if(playerValue > randomNumber){
          moveLine(true);
          sprintf(string, "%d is too big    ", playerValue);
          printStringLCD(string);
          
        } else if(playerValue < randomNumber){
          //Checks the user's number to determines if its smaller and informes them.
          moveLine(true);
          sprintf(string, "%d is too small  ", playerValue);
          printStringLCD(string);
          
          
        } else if(playerValue == randomNumber){
          //Checks the user's number to determines if its the same and informes them of their victory.
          moveLine(true);
          sprintf(string, "%d is correct!   ", playerValue);
          printStringLCD(string);
          moveLine(false);
          printStringLCD("You win! :D     ");
          win=1;
        }
        
        //If the player hasn't won, resets the screen to continue the game.
        if(win==0){
          SysCtlDelay(8000000);
          redrawScreen();
        }
        
    } else if(triesLeft == 1){
      //If the user is down to their last try and their guess wasn't correct, informes them they lost.
        if(playerValue > randomNumber){
          moveLine(true);
          sprintf(string, "%d is too big    ", playerValue);
          printStringLCD(string);
          moveLine(false);
          printStringLCD("YOU LOSE!        ");
          printRandomNum();
          lose=1;
          
        } else if(playerValue < randomNumber){
          moveLine(true);
          sprintf(string, "%d is too small  ", playerValue);
          printStringLCD(string);
          moveLine(false);
          printStringLCD("YOU LOSE!        ");
          printRandomNum();
          lose=1;
          
        } else if(playerValue == randomNumber){
          //If they guessed correctly, Tells them they won!
          moveLine(true);
          sprintf(string, "%d is correct!     ", playerValue);
          printStringLCD(string);
          moveLine(false);
          printStringLCD("You win! :D     ");
          win=1;
        }
        
    }
    
    //Waits 3 seconds.
    SysCtlDelay(8000000);
    //Resets the game when informed of a win or lose.
    if((win==1) || (lose==1)){
      win=0;
      lose=0;
      randomNum();
      triesLeft=3;
      playerValue=0;
      redrawScreen();
    }
    
  }  
  
  //If only the up button is pressed, increases the user's number by 1.
  if(GPIOPinRead(GPIO_PORTG_BASE, (GPIO_PIN_2|GPIO_PIN_1)) == 0x04){
    if(playerValue<9){
      playerValue++;
      printPlayerValue();
    }
    
  }
  
  //If only the down button is pressed, decreases the user's by 1.
  if(GPIOPinRead(GPIO_PORTG_BASE, (GPIO_PIN_1|GPIO_PIN_2)) == 0x02){
    if(playerValue>0){
      playerValue--;
      printPlayerValue();
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
    
    //Generates the random number.
    randomNum();
     
    moveLine(true);
    printStringLCD("Guess # 0 to 9 ");
     
    printPlayerValue();
   
    // Loop forever.
    while(1)
    {
      
    }
}
