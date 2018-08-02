/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC24 / dsPIC33 / PIC32MM MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - pic24-dspic-pic32mm : 1.55
        Device            :  dsPIC33EP256MC502
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.34
        MPLAB             :  MPLAB X v4.15
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

#include "mcc_generated_files/mcc.h"

/*
                         Main application
 */

char hallState;
bool hall1,hall2,hall3;
char currentStep;
char pwmState;

typedef enum tag{
    GND = 0,
    VDD = 1,
    NEUTRAL = 2
} PhaseState;
PhaseState phase1,phase2,phase3;

/* steps are the following: 1,3,2,6,4,5 */
char hallToStepTranslator[] = {0,1,3,2,5,6,4};
char stepToPhaseTranslate[] = {
    //Step neutral: p1-M p2-N p3-N
    0b010101,

    //Step 1: p1-VDD p2-N p3-GND
    0b000111,
    //Step 2: p1-N p2-VDD p3-GND
    0b010011,    
    //Step 3: p1-GND p2-VDD p3-N
    0b110001,
    //Step 4: p1-GND p2-N p3-VDD
    0b110100,   
    //Step 5: p1-N p2-GND p3-VDD
    0b011100,
    //Step 6: p1-VDD p2-GND p3-N
    0b001101,    

            
};

int main(void)
{
    // initialize the device
    SYSTEM_Initialize();


    INTERRUPT_GlobalEnable();

        hallState = (PORTB>>6) & 0b111;
        
    currentStep = hallToStepTranslator[hallState];    
    if (currentStep > 4) pwmState = stepToPhaseTranslate[currentStep-4]; // phase shifting
    else pwmState = stepToPhaseTranslate[currentStep+2]; // phase shifting
    LATB = ((((unsigned int) pwmState)<<10) & (unsigned int)0xFC00) | (LATB & 0x3FF);
    
    while (1)
    {
        X2CScope_Communicate();
        /*
         * switch (phase1) {
            case 0:
                IO_PWM1H_SetHigh(); //Switch OFF High side
                IO_PWM1L_SetHigh(); //Switch ON Low side
                break;
            case 1:
                IO_PWM1H_SetLow(); //Switch ON High side
                IO_PWM1L_SetLow(); //Switch OFF Low side
                break;
            case 2:
                IO_PWM1H_SetHigh(); //Switch OFF High side
                IO_PWM1L_SetLow(); //Switch OFF Low side  
                break;
        }
        switch (phase2) {
            case 0:
                IO_PWM2H_SetHigh(); //Switch OFF High side
                IO_PWM2L_SetHigh(); //Switch ON Low side
                break;
            case 1:
                IO_PWM2H_SetLow(); //Switch ON High side
                IO_PWM2L_SetLow(); //Switch OFF Low side
                break;
            case 2:
                IO_PWM2H_SetHigh(); //Switch OFF High side
                IO_PWM2L_SetLow(); //Switch OFF Low side
                break;
        }
        switch (phase3) {
            case 0:
                IO_PWM3H_SetHigh(); //Switch OFF High side
                IO_PWM3L_SetHigh(); //Switch ON Low side
                break;
            case 1:
                IO_PWM3H_SetLow(); //Switch ON High side
                IO_PWM3L_SetLow(); //Switch OFF Low side
                break;
            case 2:
                IO_PWM3H_SetHigh(); //Switch OFF High side
                IO_PWM3L_SetLow(); //Switch OFF Low side
                break;
        }          
         */ 
    }
}



/* Interrupt service routine for the CNI interrupt. */
void __attribute__ (( interrupt, no_auto_psv )) _CNInterrupt ( void )
{
    if(IFS1bits.CNIF == 1)
    {
        // Clear the flag
        IFS1bits.CNIF = 0;
        
        hall1 = PORTBbits.RB6;
        hall2 = PORTBbits.RB7;
        hall3 = PORTBbits.RB8;
        hallState = (PORTB>>6) & 0b111;
        IO_LED_Toggle();
        
        currentStep = hallToStepTranslator[hallState];
        if(IO_S2_GetValue()){
            if(IO_S1_GetValue()){
                if (currentStep > 4) pwmState = stepToPhaseTranslate[currentStep-4]; // phase shifting
                else pwmState = stepToPhaseTranslate[currentStep+2]; // phase shifting
            }
            else {
                if (currentStep < 2) pwmState = stepToPhaseTranslate[currentStep+5]; // phase shifting
                else pwmState = stepToPhaseTranslate[currentStep-1]; // phase shifting 
            }
        LATB = ((((unsigned int) pwmState)<<10) & (unsigned int)0xFC00) | (LATB & 0x3FF);            
        }
        else{
            pwmState = stepToPhaseTranslate[0];
            LATB = ((((unsigned int) pwmState)<<10) & (unsigned int)0xFC00) | (LATB & 0x3FF);  
        }
         
    }
}



void TMR1_CallBack(void)
{
    //1ms sample time period
    X2CScope_Update();
}


/**
 End of File
*/