
//******************************************************************************
//  MSP430FR235x Demo - Configure MCLK for 24MHz operation, and REFO sourcing
//                                     FLLREF and ACLK.
//
//  Description: Configure MCLK for 24MHz. FLL reference clock is REFO. At this
//                    speed, the FRAM requires wait states.
//                    ACLK = default REFO ~32768Hz, SMCLK = MCLK = 24MHz.
//                    Toggle LED to indicate that the program is running.
//
//           MSP430FR2355
//         ---------------
//     /|\|               |
//      | |               |
//      --|RST            |
//        |          P1.2 |---> LED
//        |               |
//        |          P1.0 |---> SMCLK = 24MHz
//        |          P1.1 |---> ACLK  = 32768Hz
//
//******************************************************************************
#include <msp430.h>

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                          // Stop watchdog timer

    // Configure two FRAM waitstate as required by the device datasheet for MCLK
    // operation at 24MHz(beyond 8MHz) _before_ configuring the clock system.
    FRCTL0 = FRCTLPW | NWAITS_2;

    __bis_SR_register(SCG0);                           // disable FLL
    CSCTL3 |= SELREF__REFOCLK;                         // Set REFO as FLL reference source
    CSCTL0 = 0;                                        // clear DCO and MOD registers
    CSCTL1 |= DCORSEL_7;                               // Set DCO = 24MHz
    CSCTL2 = FLLD_0 + 731;                             // DCOCLKDIV = 24MHz
    __delay_cycles(3);
    __bic_SR_register(SCG0);                           // enable FLL
    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1));         // FLL locked

    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;        // set default REFO(~32768Hz) as ACLK source, ACLK = 32768Hz
                                                       // default DCOCLKDIV as MCLK and SMCLK source

    P1DIR |= BIT0 | BIT1 | BIT2;                       // set ACLK SMCLK and LED pin as output
    P1SEL1 |= BIT0 | BIT1;                             // set ACLK and  SMCLK pin as second function

    PM5CTL0 &= ~LOCKLPM5;                              // Disable the GPIO power-on default high-impedance mode
                                                       // to activate previously configured port settings

    while(1)
    {
        P1OUT ^= BIT2;                                 // Toggle P1.2 using exclusive-OR
        __delay_cycles(12000000);                       // Delay for 12000000*(1/MCLK)=0.5s
    }
}
