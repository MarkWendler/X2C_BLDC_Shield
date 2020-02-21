/**
  Generated main.c file from MPLAB Code Configurator

  @Company
    Microchip Technology Inc.

  @File Name
    main.c

  @Summary
    This is the generated main.c using PIC24 / dsPIC33 / PIC32MM MCUs.

  @Description
    This source file provides main entry point for system initialization and application code development.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.166.0
        Device            :  dsPIC33EP256MC502
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.41
        MPLAB 	          :  MPLAB X v5.30
*/

/*
    (c) 2019 Microchip Technology Inc. and its subsidiaries. You may use this
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

/**
  Section: Included Files
*/
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/X2CScope/X2CScope.h"
#include "mcc_generated_files/adc1.h"
#include "mcc_generated_files/pwm.h"
#include "mcc_generated_files/interrupt_manager.h"
#include "mcc_generated_files/pin_manager.h"

/*
                         Main application
 */

uint8_t hallState;
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
uint8_t stepToPhaseShiftCW[] = { 0,3,4,5,6,1,2};
uint8_t stepToPhaseShiftCCW[] = { 0,6,1,2,3,4,5};
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
void hallStateChange(void);

uint16_t pot_val;
int main(void)
{
    // initialize the device
    SYSTEM_Initialize();

    //register change notificcation interrupt handler function
    CN_SetInterruptHandler(hallStateChange);
    INTERRUPT_GlobalEnable();
    ADC1_ChannelSelect(AN_POT);
    
    while (1)
    {
        X2CScope_Communicate();
        
        if(ADC1_IsConversionComplete(AN_POT)){
            pot_val = ADC1_ConversionResultGet(AN_POT);
            PWM_MasterDutyCycleSet(pot_val<<2);
        }
        

    }
}



/* Interrupt service routine for the CNI interrupt. */
void hallStateChange(void)
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
        currentStep = stepToPhaseShiftCW[currentStep];
        switch (currentStep){
            case 0: //Step neutral: p1-M p2-N p3-N
                PWM_OverrideDataSet(PWM_GENERATOR_1,0b00); //high side pnp
                PWM_OverrideDataSet(PWM_GENERATOR_2,0b00); //high side pnp
                PWM_OverrideDataSet(PWM_GENERATOR_3,0b00); //high side pnp
                PWM_OverrideLowEnable(PWM_GENERATOR_1);
                PWM_OverrideLowEnable(PWM_GENERATOR_2);
                PWM_OverrideLowEnable(PWM_GENERATOR_3);
                PWM_OverrideHighEnable(PWM_GENERATOR_1);
                PWM_OverrideHighEnable(PWM_GENERATOR_2);
                PWM_OverrideHighEnable(PWM_GENERATOR_3);
                break;
            case 1:    //Step 1: p1-VDD(PWM) p2-N p3-GND
                PWM_OverrideDataSet(PWM_GENERATOR_1,0b00); //high side pnp
                PWM_OverrideHighDisable(PWM_GENERATOR_1);
                PWM_OverrideLowEnable(PWM_GENERATOR_1);
                //p2: neutral
                PWM_OverrideDataSet(PWM_GENERATOR_2,0b00); //high side pnp
                PWM_OverrideHighEnable(PWM_GENERATOR_2);
                PWM_OverrideLowEnable(PWM_GENERATOR_2);
                //p3: GND
                PWM_OverrideDataSet(PWM_GENERATOR_3,0b01); //high side pnp
                PWM_OverrideHighEnable(PWM_GENERATOR_3);
                PWM_OverrideLowEnable(PWM_GENERATOR_3);
                break;
            case 2:  //Step 2: p1-N p2-VDD(PWM) p3-GND
                PWM_OverrideDataSet(PWM_GENERATOR_1,0b00); //high side pnp
                PWM_OverrideHighEnable(PWM_GENERATOR_1);
                PWM_OverrideLowEnable(PWM_GENERATOR_1);
                //p2: VDD (PWM)
                PWM_OverrideDataSet(PWM_GENERATOR_2,0b00); //high side pnp
                PWM_OverrideHighDisable(PWM_GENERATOR_2);
                PWM_OverrideLowEnable(PWM_GENERATOR_2);
                //p3: GND
                PWM_OverrideDataSet(PWM_GENERATOR_3,0b01); //high side pnp
                PWM_OverrideHighEnable(PWM_GENERATOR_3);
                PWM_OverrideLowEnable(PWM_GENERATOR_3);
                break;
            case 3://Step 3: p1-GND p2-VDD p3-N
                PWM_OverrideDataSet(PWM_GENERATOR_1,0b01); //high side pnp
                PWM_OverrideHighEnable(PWM_GENERATOR_1);
                PWM_OverrideLowEnable(PWM_GENERATOR_1);
                //p2: VDD (PWM)
                PWM_OverrideDataSet(PWM_GENERATOR_2,0b00); //high side pnp
                PWM_OverrideHighDisable(PWM_GENERATOR_2);
                PWM_OverrideLowEnable(PWM_GENERATOR_2);
                //p3: Neutral
                PWM_OverrideDataSet(PWM_GENERATOR_3,0b00); //high side pnp
                PWM_OverrideHighEnable(PWM_GENERATOR_3);
                PWM_OverrideLowEnable(PWM_GENERATOR_3);
                break;
            case 4:    //Step 4: p1-GND p2-N p3-VDD
                PWM_OverrideDataSet(PWM_GENERATOR_1,0b01); //high side pnp
                PWM_OverrideHighEnable(PWM_GENERATOR_1);
                PWM_OverrideLowEnable(PWM_GENERATOR_1);
                //p2: Neutral
                PWM_OverrideDataSet(PWM_GENERATOR_2,0b00); //high side pnp
                PWM_OverrideHighEnable(PWM_GENERATOR_2);
                PWM_OverrideLowEnable(PWM_GENERATOR_2);
                //p3: VDD(PWM)
                PWM_OverrideDataSet(PWM_GENERATOR_3,0b00); //high side pnp
                PWM_OverrideHighDisable(PWM_GENERATOR_3);
                PWM_OverrideLowEnable(PWM_GENERATOR_3);
                break;
            case 5:     //Step 5: p1-N p2-GND p3-VDD
                PWM_OverrideDataSet(PWM_GENERATOR_1,0b00); //high side pnp
                PWM_OverrideHighEnable(PWM_GENERATOR_1);
                PWM_OverrideLowEnable(PWM_GENERATOR_1);
                //p2: GND
                PWM_OverrideDataSet(PWM_GENERATOR_2,0b01); //high side pnp
                PWM_OverrideHighEnable(PWM_GENERATOR_2);
                PWM_OverrideLowEnable(PWM_GENERATOR_2);
                //p3: VDD(PWM)
                PWM_OverrideDataSet(PWM_GENERATOR_3,0b00); //high side pnp
                PWM_OverrideHighDisable(PWM_GENERATOR_3);
                PWM_OverrideLowEnable(PWM_GENERATOR_3);
                break;
            case 6:     //Step 6: p1-VDD(PWM) p2-GND p3-N
                PWM_OverrideDataSet(PWM_GENERATOR_1,0b00); //high side pnp
                PWM_OverrideHighDisable(PWM_GENERATOR_1);
                PWM_OverrideLowEnable(PWM_GENERATOR_1);
                //p2: GND
                PWM_OverrideDataSet(PWM_GENERATOR_2,0b01); //high side pnp
                PWM_OverrideHighEnable(PWM_GENERATOR_2);
                PWM_OverrideLowEnable(PWM_GENERATOR_2);
                //p3: Neutral
                PWM_OverrideDataSet(PWM_GENERATOR_3,0b00); //high side pnp
                PWM_OverrideHighEnable(PWM_GENERATOR_3);
                PWM_OverrideLowEnable(PWM_GENERATOR_3);
                break;
            default://Step neutral: p1-M p2-N p3-N
                PWM_OverrideDataSet(PWM_GENERATOR_1,0b00); //high side pnp
                PWM_OverrideDataSet(PWM_GENERATOR_2,0b00); //high side pnp
                PWM_OverrideDataSet(PWM_GENERATOR_3,0b00); //high side pnp                
                PWM_OverrideLowEnable(PWM_GENERATOR_1);
                PWM_OverrideLowEnable(PWM_GENERATOR_2);
                PWM_OverrideLowEnable(PWM_GENERATOR_3);
                PWM_OverrideHighEnable(PWM_GENERATOR_1);
                PWM_OverrideHighEnable(PWM_GENERATOR_2);
                PWM_OverrideHighEnable(PWM_GENERATOR_3);

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