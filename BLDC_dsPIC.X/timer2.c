/*
 * File:   timer2.c
 * Author: mawendle
 *
 * Created on November 14, 2025, 2:51 PM
 */


#include <xc.h>
#include <stdint.h>

/*
 * Timer2+Timer3 in 32-bit mode, free-running on Fcy (TCS=0), prescaler 1:1.
 * TMR3:TMR2 form the 32-bit counter; PR3:PR2 set to 0xFFFFFFFF for continuous wrap.
 */
void Timer23_Init(void)
{
    T2CONbits.TON = 0;        // Stop Timer2/3 block
    T2CONbits.TCS = 0;        // Internal clock (Fcy = Fosc/2)
    T2CONbits.TCKPS = 0b00;   // Prescaler 1:1 (fastest)
    T2CONbits.T32 = 1;        // 32-bit mode (Timer2+Timer3 concatenated)

    // Max period for free-run
    PR2 = 0xFFFF;
    PR3 = 0xFFFF;

    // Clear counter
    TMR2 = 0;
    TMR3 = 0;

    // (Optional but harmless) make sure T3 interrupt is off in case previously used
    _T3IF = 0;
    _T3IE = 0;

    T2CONbits.TON = 1;        // Start the 32-bit timer
}

void Timer23_Clear(void)
{
    // In 32-bit mode, write high word first, then low word
    TMR3HLD = 0;
    TMR2 = 0;
}

/* Atomic-ish 32-bit read: read HI-LO-HI and retry if rollover detected */
uint32_t Timer23_Read(void)
{
    return ((uint32_t)TMR3HLD << 16) | TMR2;
}
