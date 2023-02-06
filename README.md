# Part 4: Occupancy Detection and Alert System
The Occupancy Detection program begins by defining the three states that are present within the system as well as including the MSP430 library.
```c
// States used for alarm system
#define ARMED_STATE 0
#define WARNING_STATE 1
#define ALERT_STATE 2

#include <msp430.h>
```

Next, inside the main function, the basic initializations and configurations are performed. The system is initialized to the armed state. The variable that counts how many seconds the system is in the warning state is inialized to zero. The watchdog timer is included. The inputs and outputs are configured properly. Each LED is initialized to an output state.
```c
int main(void)
{
    char state = ARMED_STATE;               // initialized state
    int count_seconds = 0;                  // variable to count 10 seconds in warning state
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    // Setting pin directions
    P1DIR |= BIT0;               // Configure P1.0 to an Output
    P6DIR |= BIT6;               // Configure P6.6 to an Output
    P2DIR &= ~BIT5;              // Configure P2.5 to an Input <- the pin that the sensor is plugged into
    P4DIR &= ~BIT1;              // Configure P4.1 to an Input
    P4REN |= BIT1;               // Enable Resistor on P4.1
    P4OUT |= BIT1;               // Configure Resistor on P4.1 to be Pullup

    P1OUT &= ~BIT0;              // Initialize Red LED to off state
    P6OUT &= ~BIT6;              // Initialize Green LED to off state

    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings
```

In the while loop, a switch statement is used for what is performed in each state. First, for the armed state, if a person is not detected, the green LED will toggle on and off, blinking on every 3 seconds while the red LED is constantly turned off. However, if a person is detected, the system will move into the warning state.
```c
    while(1)
    {
        switch(state)
        {
        case ARMED_STATE:
            count_seconds = 0;                  // reset counting value to 0
            if (!(P2IN & BIT5))                    // if person not detected
            {
                P6OUT ^= BIT6;                  // Toggle Green LED
                P1OUT &= ~BIT0;                 // Turn Red LED off
                __delay_cycles(1500000);        // Delay for 1500000*(1/MCLK)=1.5s or blinking every 3 seconds
            }
            else                                // if person is detected
            {
                state = WARNING_STATE;          // move into warning state
            }
            break;
```

Second is the warning state. If a person is not detected at any point, the system will move back into the armed state. However, if a person is detected, there are one of two options that will be taken. If it has been ten seconds since moving to the warning state, then the system will move into the alert state. If it has not been ten seconds, the red LED will toggle on and off, blinking every 1 second while the green LED is constantly off. Additonally, the variable count_seconds, which counts how many seconds it has been since moving to this state, increases by 1 every half-second. Therefore, when this variable reaches 20 and a person is still detected, the system will move to the armed state as it has been 20 half-seconds, or 10 seconds.
```c
        case WARNING_STATE:
            if ((P2IN & BIT5) && (!(count_seconds == 20)))     // if person is detected but not 10 seconds after moving into state
            {
                P1OUT ^= BIT0;                  // Toggle Red LED
                P6OUT &= ~BIT6;                 // Turn Green LED off
                count_seconds++;                // increase value by 1 every 0.5 seconds
                __delay_cycles(500000);         // Delay for 500000*(1/MCLK)=0.5s or blinking every 1 second
            }
            else if ((P2IN & BIT5) && (count_seconds == 20))   // if person is detected and 10 seconds after moving into state
            {
                state = ALERT_STATE;            // move into alert state
            }
            else if (!(P2IN & BIT5))                                // if person is not detected
            {
                state = ARMED_STATE;            // move back to armed state
            }
            break;
```

Last is the alert state. If the reset button is pressed, the system will return to the armed state. If, however, this button is not pressed, the red LED will constantly be on to show that the alert is active. For the case of a real security system, this is the point where the local authorities would be alerted.
```c
        case ALERT_STATE:
            if (!(P4IN & BIT1))                 // if reset button is pressed
            {
                state = ARMED_STATE;            // move back to armed state
            }
            else                                // if reset is not pressed
            {
                P1OUT |= BIT0;                  // Turn Red LED on
            }
            break;
        }
    }
}
```

The implementation of this occupancy detection system is rather simple as it uses polling rather that interputs. In the future, I would like to try to implement this system while using interrupts as it would be good practice.
