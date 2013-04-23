/*
 * =====================================================================================
 *
 *       Filename:  twi_mmi.c
 *
 *    Description:  Read out of rotation encoder and pins of the twi2lcd modul.
 *
 * 			 Uage:  See the C included twi_slave file for description of pins and
 *                  functions.
 *
 *        Version:  1.0
 *        Created:  22.06.2010 20:15:59
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ignaz Laepple (mn), ignaz.laepple@gmx.de
 *        Company:  FH-Regensburg
 *
 * =====================================================================================
 */

#ifndef _TWI_MMI_H
#define _TWI_MMI_H

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "twi_master.h"
#include "twi_slave.h"
#include <util/twi.h>
#include "uart.h"

#define debug 0

extern volatile int8_t rot_enc_dir;

/*************************************************************************
Function: RotEnc_Get_Dir()
Purpose:
Input:    none
Returns:  -1 when turned left, +1 when turned right
**************************************************************************/
    extern int8_t
RotEnc_Get_Dir(void);

/*************************************************************************
Function: RotEnc_Pushed()
Purpose:
Input:    none
Returns:  1 when rot_enc is pushed down, 0 else
**************************************************************************/
    extern uint8_t
RotEnc_Pushed(void);

#endif //_TWI_MMI_H


