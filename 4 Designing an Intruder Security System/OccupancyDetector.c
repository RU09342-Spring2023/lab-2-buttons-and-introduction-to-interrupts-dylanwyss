/*
 * OccupancyDetector.c
 *
 *  Created on: Jan 30, 2023
 *      Author: Dylan Wyss
 */

// States used for alarm system
#define ARMED_STATE 0
#define WARNING_STATE 1
#define ALERT_STATE 2

#include <msp430.h>

int main(void)
{
    char state = ARMED_STATE;
    int count_seconds = 0;
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    // Setting pin directions
    P1DIR |= BIT0;               // Configure P1.0 to an Output
    P6DIR |= BIT6;               // Configure P6.6 to an Output
    P2DIR &= ~BIT3;              // Configure P2.3 to an Input
    P2REN |= BIT3;               // Enable Resistor on P2.3
    P2OUT |= BIT3;               // Configure Resistor on P2.3 to be Pullup
    P4DIR &= ~BIT1;              // Configure P4.1 to an Input
    P4REN |= BIT1;               // Enable Resistor on P4.1
    P4OUT |= BIT1;               // Configure Resistor on P4.1 to be Pullup

    P1OUT &= ~BIT0;              // Initialize Red LED to off state
    P6OUT &= ~BIT6;              // Initialize Green LED to off state

    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings

    while(1)
    {
        switch(state)
        {
        case ARMED_STATE:
            count_seconds = 0;
            if (P2IN & BIT3)
            {
                P6OUT ^= BIT6;                  // Toggle Green LED
                P1OUT &= ~BIT0;                 // Turn Red LED off
                __delay_cycles(1500000);             // Delay for 1500000*(1/MCLK)=1.5s or blinking every 3 seconds
            }
            else
            {
                state = WARNING_STATE;
            }
            break;
        case WARNING_STATE:
            if ((!(P2IN & BIT3)) & (!(count_seconds == 20)))
            {
                P1OUT ^= BIT0;                  // Toggle Red LED
                P6OUT &= ~BIT6;                 // Turn Green LED off
                count_seconds++;
                __delay_cycles(500000);             // Delay for 500000*(1/MCLK)=0.5s or blinking every 1 second
            }
            else if ((!(P2IN & BIT3)) & (count_seconds == 20))
            {
                state = ALERT_STATE;
            }
            else if (P2IN & BIT3)
            {
                state = ARMED_STATE;
            }
            break;
        case ALERT_STATE:
            if (!(P4IN & BIT1))
            {
                state = ARMED_STATE;
            }
            else
            {
                P1OUT |= BIT0;                  // Turn Red LED on
            }
            break;
        }
    }
}
