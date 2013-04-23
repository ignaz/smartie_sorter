/*
 * =====================================================================================
 *
 *       Filename:  motion_controll.h
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

#ifndef  _MOTION_CONTROLL_H
#define _MOTION_CONTROLL_H



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
#include "smarties.h"

#define MC_IO_EXPANDER_ADDRESS          0x20
#define MC_IO_EXPANDER_DIR_MASK         0x07 // Used to write back inputs as 1!!
#define MC_IO_EXPANDER_BIT_CATCHER      0
#define MC_IO_EXPANDER_BIT_CONVEYOR     1
#define MC_IO_EXPANDER_BIT_SOLENOID     4
#define MC_IO_EXPANDER_BIT_VIBRATOR     5


/****** Defines *********************************************************/

#define MC_CONVEYOR_SLOTS               10


extern enum COLOUR mc_smartie_table[];



extern uint8_t mc_conveyor_position_index ;



/*************************************************************************
Function: MC_Catcher_Init()
Purpose:  Sets working parameters of the TMC222 for the catcher stepper.
          When Done the Motor searches the reference mark on the catcher,
          and resets the position counter.
Input:    none
Returns:  error
**************************************************************************/
    extern void
MC_Catcher_Init(void);

/*************************************************************************
Function: MC_Catcher_Set_Position()
Purpose:
Input:    enum COLOUR
Returns:  none
**************************************************************************/
    extern void
MC_Catcher_Set_Position(enum COLOUR colour);

/*************************************************************************
Makro: MC_Catcher_Off_Reference()
Purpose:  read out the status of the lightbarrier
Input:    none
Returns:  1 when the light barrier is at reference mark, 0 else
**************************************************************************/
#define MC_Catcher_Off_Reference() \
    ((TWI_Master_Read_Byte(MC_IO_EXPANDER_ADDRESS)) & _BV(MC_IO_EXPANDER_BIT_CATCHER))

/*************************************************************************
Function: MC_Is_Catcher_Idle()
Purpose:  get motion status
Input:    none
Returns:  1 if catcher is idle,0 else
**************************************************************************/
    extern uint8_t
MC_Is_Catcher_Idle(void);

/*************************************************************************
Function: MC_Conveyor_Init()
Purpose:  Sets working parameters of the TMC222 for the conveyor stepper.
          When Done the Motor searches the reference mark on the conveyor,
          and resets the position counter.
Input:    none
Returns:  error
**************************************************************************/
    extern void
MC_Conveyor_Init(void);

/*************************************************************************
Makro: MC_Conveyor_Off_Reference()
Purpose:  read out the status of the lightbarrier
Input:    none
Returns:  1 when the light barrier is at reference mark, 0 else
**************************************************************************/
#define MC_Conveyor_Off_Reference() \
    ((TWI_Master_Read_Byte(MC_IO_EXPANDER_ADDRESS)) & _BV(MC_IO_EXPANDER_BIT_CONVEYOR))


/*************************************************************************
Function: MC_Catcher_Set_Position()
Purpose:  used to move the conveyor back or forward (in steps of 18°)
Input:    none
Returns:  enum COLOUR
**************************************************************************/
    extern void
MC_Conveyor_Set_Position(int8_t step);

/*************************************************************************
Function: MC_Is_Conveyor_Idle()
Purpose:  get motion status
Input:    none
Returns:  1 if conveyor is idle, 0 else
**************************************************************************/
    extern uint8_t
MC_Is_Conveyor_Idle(void);

/*************************************************************************
Makro:    MC_Solenoid_On
Purpose:  switch the solenoid of the smartie silo
Input:    none
Returns:  none
**************************************************************************/
    extern void
MC_Solenoid_On(void);

/*************************************************************************
Makro:    MC_Solenoid_Off
Purpose:  switch off the solenoid of the smartie silo
Input:    none
Returns:  none
**************************************************************************/
    extern void
MC_Solenoid_Off(void);

/*************************************************************************
Makro:    MC_Vibrator_On
Purpose:  switch off the vibrator of the smartie silo
Input:    none
Returns:  none
**************************************************************************/
    extern void
MC_Vibrator_On(void);

/*************************************************************************
Makro:    MC_Vibrator_Off
Purpose:  switch off the vibrator of the smartie silo
Input:    none
Returns:  none
**************************************************************************/
    extern void
MC_Vibrator_Off(void);

/*************************************************************************
Function: MC_Eject_Smartie()
Purpose:  switch off the vibrator of the smartie silo
Input:    none
Returns:  none
**************************************************************************/
    extern void
MC_Eject_Smartie(void);

/*************************************************************************
Function: MC_Is_Smartie_Ejected()
Purpose:  returns 1 if the Solenoid is activ for at least MC_SOLENOID_ON TIME
Input:    none
Returns:  1 if smaretie should lie in the chamber of the conveyor
**************************************************************************/
    extern uint8_t
MC_Is_Smartie_Ejected(void);

/*************************************************************************
ISR_Init:   MC_Timer_0Init()
Purpose:    Initialisation of the timer0 interrupt that should occour
            every ms. Used for non blocking delays!!
**************************************************************************/
    extern void
MC_Timer0_Init(void);

    extern void
MC_FSM_Execute(void);


#endif // _MOTION_CONTROLL_H
