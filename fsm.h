/*
 * =====================================================================================
 *
 *       Filename:  collour_sensor.c
 *    Description:  this file contains function to handle the color sensor_that was
                    designed for the smarties sorter. This file is based on the two
                    moduls :
                    ADJD_S311 (the sensor library) and the
                    TLC59116  (led driver library).
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

#ifndef _FSM_H
#define _FSM_H

extern volatile uint8_t fsm_pause;

enum fsm_mode { md_idle=0,
                md_init,
                md_learning,
                md_running,
                md_pause
              };
extern enum fsm_mode cur_mode;


/*************************************************************************
Function: FSM_check_state()
Purpose:
Input:    none
Returns:  none
**************************************************************************/
extern void
FSM_Check_State(void);


#endif
