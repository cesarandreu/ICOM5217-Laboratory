

#include "inc/lm3s1968.h"

//*****************************************************************************
//
// Blink the on-board LED.
//
//*****************************************************************************
int
main(void)
{
    volatile unsigned long ulLoop;

    //
    // Enable the GPIO port that is used for the on-board LED.
    //
    SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOG;

    //
    // Do a dummy read to insert a few cycles after enabling the peripheral.
    //
    ulLoop = SYSCTL_RCGC2_R;

    //
    // Enable the GPIO pin for the LED (PG2).  Set the direction as output, and
    // enable the GPIO pin for digital function.
    //
    GPIO_PORTG_DIR_R = 0x04;
    GPIO_PORTG_DEN_R = 0x04;

    // Loop forever.
    while(1)
    {
        //
        // Turn on the LED.
        //
        GPIO_PORTG_DATA_R |= 0x04;

        //
        // Delay for a bit.
        //
        for(ulLoop = 0; ulLoop < 200000; ulLoop++)
        {
        }

        //
        // Turn off the LED.
        //
        GPIO_PORTG_DATA_R &= ~(0x04);

        //
        // Delay for a bit.
        //
        for(ulLoop = 0; ulLoop < 200000; ulLoop++)
        {
        }
    }
}
