/*
 * =====================================================================================
 *
 *       Filename:  motion_controll.c
 *    Description:  this file contains functions to handle the transportation of the
 *                  smarties. As there are the silo, the transport_revolver and the
 *                  catcher revolver.
 *                  It reads out the lightbarriers, controlls the steppers, the magnetic coil
 *                  of the seperator and the vibration motor.
                    All Communication is done via TWI (I²C);
 *
 *        Version:  1.0
 *        Created:  29.08.2010 19:49:30
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ignaz Laepple (mn), ignaz.laepple@gmx.de
 *        Company:  FH-Regensburg
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/pgmspace.h>
#include <util/twi.h>
#include <util/delay.h>

#include "uart.h"
#include "twi_master.h"
#include "TMC222.h"
#include "smarties.h"
#include "debug.h"
#include "motion_controll.h"

#define debug 1

#define MC_SOLENOID_ON_TIME    400
#define MC_SOLENOID_OFF_TIME   400


// defines and defaults of the parameters of the catcher motor:
    //defs
#define CATCHER_ADDRESS         0x61

    //defaults
    static TMC222_Parameters_t catcher_parameters={
        8,15,2,8,2,0,4,0,0,3,0,0};
    /*
    .IHold    =8;
    .IRun     =15;
    .VMin     =2;
    .VMax     =8;
    .Acc      =2;1
    .Shaft    =0;
    .SecPosHi =4; -> Dont use Secure position!!!
    .SecPosLo =0;
    .NA1      =0;
    .StepMode =3;
    .AccShape =0;
    .NA2      =0;*/

    TMC222_Status_t mc_catcher_status;
    int16_t mc_catcher_position_cnt = 0;
    uint8_t mc_catcher_position_index = 0;
    int16_t mc_catcher_position_table[9] = {0,356,711,1067,1422,1778,2133,2489,2844};

//

// defines and defaults of the parameters of the catcher motor:
    //defs
#define CONVEYOR_ADDRESS         0x60

    //defaults
    TMC222_Parameters_t conveyor_parameters={
        8,15,2,7,2,0,4,0,0,3,0,0};
        /*
        .IHold    =8;
        .IRun     =15;
        .VMin     =2;
        .VMax     =7;
        .Acc      =3;
        .Shaft    =0;
        .SecPosHi =4; -> Dont use Secure position!!!
        .SecPosLo =0;
        .NA1      =0;
        .StepMode =3;
        .AccShape =0;
        .NA2      =0;*/

    TMC222_Status_t mc_conveyor_status;
    int16_t mc_conveyor_position_cnt = 0;
    uint8_t mc_conveyor_position_index = 0;
    enum COLOUR mc_smartie_table[MC_CONVEYOR_SLOTS];

//




/*************************************************************************
variables defines and enums for the small MC-Statemachine
**************************************************************************/
enum MC_states{SOLENOID_IDLE=0,SOLENOID_BUSY};
enum MC_states MC_State = SOLENOID_IDLE;

enum MC_events{NO_EVENT = 0,SOLENOID_IS_ON,SOLENOID_IS_OFF,ACTIVATE_SOLENOID};
enum MC_events MC_Event = NO_EVENT;


volatile uint8_t    mc_solenoid_status=0;
volatile uint16_t   mc_solenoid_off_timer = 0, mc_solenoid_on_timer = 0;
#define TIMER0_RELOAD  67//(UINT8_MAX-(F_CPU / 64 / 1000)) // should be 67

    void
MC_Catcher_Init(void)
{
    // GetFullStatus1 to reset the errors
    TMC222_GetFullStatus1(&mc_catcher_status,CATCHER_ADDRESS);

    // disable acceleration and run with vmin
    catcher_parameters.AccShape = 1;
    catcher_parameters.IRun = 10; // reduce current for slow motion

    // set motorparameters
    TMC222_SetMotorParameters(&catcher_parameters,CATCHER_ADDRESS);

    // Reset actual position for easy ref search
    TMC222_ResetPosition(CATCHER_ADDRESS);

    // initiate a single turn of the catcher to find the reference mark
    TMC222_SetPosition(3200,CATCHER_ADDRESS);

    // wait for the reference mark to pass the light barrier
    while(MC_Catcher_Off_Reference());

    // Stop the stepper and reset the position counter when stopped

    TMC222_SoftStop(CATCHER_ADDRESS);

    while(TMC222_GetMotionStatus(&mc_catcher_status,CATCHER_ADDRESS)!=0);

    mc_catcher_position_index=0;
    mc_catcher_position_cnt=0;
    TMC222_ResetPosition(CATCHER_ADDRESS);

    // enable accelerated motion
    catcher_parameters.AccShape = 0;
    catcher_parameters.IRun = 15;
    TMC222_SetMotorParameters(&catcher_parameters,CATCHER_ADDRESS);
}


/*************************************************************************
Function: MC_Catcher_Set_Position()
Purpose:  initialize the
Input:    enum COLOUR
Returns:  none
**************************************************************************/
    void
MC_Catcher_Set_Position(enum COLOUR new_position)
{
    int16_t position_difference_cnt;

    if ((uint8_t)new_position != mc_catcher_position_index)
    {
        //new_position %= 9;
        // get differnce to destination
        position_difference_cnt =
        mc_catcher_position_table[(uint8_t)new_position] - mc_catcher_position_table[mc_catcher_position_index];
        // decide which way to go (of course the shortest one)
        if (position_difference_cnt > 1800)
        {
            position_difference_cnt -= 3200;
        }
        else if (position_difference_cnt < -1800)
        {
            position_difference_cnt += 3200;
        }
        // set stepper position to the computed value
        mc_catcher_position_cnt += position_difference_cnt;
        // save actual position index!!
        mc_catcher_position_index = new_position;

        TMC222_SetPosition(mc_catcher_position_cnt,CATCHER_ADDRESS);
    }


}

/*************************************************************************
Function: MC_Catcher_Get_Position()
Purpose:  initialize the
Input:    none
Returns:  enum COLOUR
**************************************************************************/
/*    void
MC_Catcher_Set_Position(enum COLOUR colour)
{

}*/


/*************************************************************************
Function: MC_Is_Catcher_Idle()
Purpose:  get motion status
Input:    none
Returns:  1 if catcher is idle,0 else
**************************************************************************/
    uint8_t
MC_Is_Catcher_Idle(void)
{
        if(TMC222_GetMotionStatus(&mc_catcher_status,CATCHER_ADDRESS))return 0;
        else return 1;
}


/*************************************************************************
Function: MC_Conveyor_Init()
Purpose:  Sets working parameters of the TMC222 for the catcher stepper.
          When Done the Motor searches the reference mark on the catcher,
          and resets the position counter.
Input:    none
Returns:  error
**************************************************************************/
    void
MC_Conveyor_Init(void)
{
    // GetFullStatus1 to reset the errors
    TMC222_GetFullStatus1(&mc_conveyor_status,CONVEYOR_ADDRESS);

    // disable acceleration and run with vmin
    conveyor_parameters.AccShape = 1;
    conveyor_parameters.IRun = 10; // reduce current for slow motion

    // set motorparameters
    TMC222_SetMotorParameters(&conveyor_parameters,CONVEYOR_ADDRESS);

    // Reset actual position for easy ref search
    TMC222_ResetPosition(CONVEYOR_ADDRESS);

    // initiate a turn of 36° to find the next reference mark on the conveyor
    TMC222_SetPosition(3200,CONVEYOR_ADDRESS);

    // wait for the reference mark to pass the light barrier
    while(MC_Conveyor_Off_Reference());

    // Stop the stepper and reset the position counter when stopped

    TMC222_SoftStop(CONVEYOR_ADDRESS);

    while(TMC222_GetMotionStatus(&mc_conveyor_status,CONVEYOR_ADDRESS)!=0);

    mc_conveyor_position_cnt=0;
    TMC222_ResetPosition(CONVEYOR_ADDRESS);

    // enable accelerated motion
    conveyor_parameters.AccShape = 0;
    conveyor_parameters.IRun = 15;
    TMC222_SetMotorParameters(&conveyor_parameters,CONVEYOR_ADDRESS);
}

/*************************************************************************
Function: MC_Catcher_Set_Position()
Purpose:  used to move the conveyor back or forward (in steps of 18°)
Input:    none
Returns:  enum COLOUR
**************************************************************************/
    void
MC_Conveyor_Set_Position(int8_t step)
{
    // wait until the stepper is ready with last job
    while (TMC222_GetMotionStatus(&mc_conveyor_status,CONVEYOR_ADDRESS));
    mc_conveyor_position_cnt += 160*step;
    mc_conveyor_position_index += (step>>1);
    mc_conveyor_position_index %= MC_CONVEYOR_SLOTS;
    TMC222_SetPosition(mc_conveyor_position_cnt,CONVEYOR_ADDRESS);
}

/*************************************************************************
Function: MC_Is_Conveyor_Idle()
Purpose:  get motion status
Input:    none
Returns:  1 if conveyor is idle, 0 else
**************************************************************************/
    uint8_t
MC_Is_Conveyor_Idle(void)
{
        if(TMC222_GetMotionStatus(&mc_conveyor_status,CONVEYOR_ADDRESS))return 0;
        else return 1;
}

/*************************************************************************
Makro: MC_Solenoid_On
Purpose:  switch on the solenoid of the smartie silo
Input:    none
Returns:  none
**************************************************************************/
     void
MC_Solenoid_On(void)
{
    register uint8_t temp;
    temp  = (TWI_Master_Read_Byte(MC_IO_EXPANDER_ADDRESS) | MC_IO_EXPANDER_DIR_MASK);
    temp |= _BV(MC_IO_EXPANDER_BIT_SOLENOID);
    TWI_Master_Write_Byte(temp,MC_IO_EXPANDER_ADDRESS);
}
/*************************************************************************
Makro:    MC_Solenoid_Off
Purpose:  switch off the solenoid of the smartie silo
Input:    none
Returns:  none
**************************************************************************/
     void
MC_Solenoid_Off(void)
{
    register uint8_t temp;
    temp  = (TWI_Master_Read_Byte(MC_IO_EXPANDER_ADDRESS) | MC_IO_EXPANDER_DIR_MASK);
    temp &= ~_BV(MC_IO_EXPANDER_BIT_SOLENOID);
    TWI_Master_Write_Byte(temp,MC_IO_EXPANDER_ADDRESS);
}
/*************************************************************************
Makro:    MC_Vibrator_On
Purpose:  switch on the vibrator of the smartie silo
Input:    none
Returns:  none
**************************************************************************/
     void
MC_Vibrator_On(void)
{
    register uint8_t temp;
    temp  = (TWI_Master_Read_Byte(MC_IO_EXPANDER_ADDRESS) | MC_IO_EXPANDER_DIR_MASK);
    temp &= ~_BV(MC_IO_EXPANDER_BIT_VIBRATOR);
    TWI_Master_Write_Byte(temp,MC_IO_EXPANDER_ADDRESS);
}
/*************************************************************************
Makro:    MC_Vibrator_Off
Purpose:  switch off the vibrator of the smartie silo
Input:    none
Returns:  none
**************************************************************************/
     void
MC_Vibrator_Off(void)
{
    register uint8_t temp;
    temp  = (TWI_Master_Read_Byte(MC_IO_EXPANDER_ADDRESS) | MC_IO_EXPANDER_DIR_MASK);
    temp |= _BV(MC_IO_EXPANDER_BIT_VIBRATOR);
    TWI_Master_Write_Byte(temp,MC_IO_EXPANDER_ADDRESS);
}

/*************************************************************************
Function: MC_Eject_Smartie()
Purpose:  switch off the vibrator of the smartie silo
Input:    none
Returns:  none
**************************************************************************/
    void
MC_Eject_Smartie(void)
{
    MC_Event = ACTIVATE_SOLENOID;
}
/*************************************************************************
Function: MC_Is_Smartie_Ejected()
Purpose:  returns 1 if the Solenoid is activ for at least MC_SOLENOID_ON TIME
Input:    none
Returns:  1 if smartie should lie in the chamber of the conveyor
**************************************************************************/
    uint8_t
MC_Is_Smartie_Ejected(void)
{
    if(MC_State == SOLENOID_IDLE) return 1;
    else return 0;
}

/*************************************************************************
ISR_Init:   MC_Timer_0Init()
Purpose:    Initialisation of the timer0 interrupt that should occour
            every ms. Used for non blocking delays!!
**************************************************************************/
    void
MC_Timer0_Init(void)
{
    TCCR0 = 3;              //Set prescaler to 64
    TCNT0 = TIMER0_RELOAD;  //Load reload value into counter register
    TIMSK |= _BV(TOIE0);     // Enable timer0 overflow onterrupt
}


/*************************************************************************
ISR:      TIMER0_OVF
Purpose:  Interrupt that should occour every ms.
            Used for non blocking delays!!
**************************************************************************/
ISR(TIMER0_OVF_vect)
{
    TCNT0 = TIMER0_RELOAD;

    if(mc_solenoid_on_timer)
    {
        mc_solenoid_on_timer--;
        if(mc_solenoid_on_timer == 0)
        {
            MC_Event = SOLENOID_IS_ON;
        }
    }

    if(mc_solenoid_off_timer)
    {
        mc_solenoid_off_timer--;
        if(mc_solenoid_off_timer == 0)
        {
            MC_Event = SOLENOID_IS_OFF;
        }
    }
}
    void
MC_FSM_Execute(void)
{
    switch(MC_State)
    {
        case SOLENOID_IDLE:
            if(MC_Event == ACTIVATE_SOLENOID)
            {
                MC_Solenoid_On();
                mc_solenoid_on_timer = MC_SOLENOID_ON_TIME;
                MC_Event = NO_EVENT;
                MC_State = SOLENOID_BUSY;
            }
            break;
        case SOLENOID_BUSY:
            if(MC_Event == SOLENOID_IS_ON)
            {
                MC_Solenoid_Off();
                mc_solenoid_off_timer = MC_SOLENOID_OFF_TIME;
                MC_Event = NO_EVENT;
                MC_State = SOLENOID_BUSY;
            }
            else if(MC_Event == SOLENOID_IS_OFF)
            {
                MC_Event = NO_EVENT;
                MC_State = SOLENOID_IDLE;
            }
            break;
    }
}








