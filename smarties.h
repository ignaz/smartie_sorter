/*
 * =====================================================================================
 *
 *       Filename:  smarites.h
 *
 *    Description:  Defines for the datatypes rgbw_t and the enum COLOR
 *
 *        Version:  1.0
 *        Created:  24.08.2010 22:37:16
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ignaz Laepple (mn), ignaz.laepple@gmx.de
 *        Company:  FH-Regensburg
 *
 * =====================================================================================
 */

#ifndef _SMARTIES_H
#define _SMARTIES_H



#include <stdlib.h>
#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "uart.h"
#include "TLC59116.h"
#include "ADJD_S311.h"
#include "color_sensor.h"

enum COLOR {Unknown=0,Red,Orange,Yellow,Green,Blue,Violett,Pink,Brown,COLOR_MAX};
typedef struct rgbw_s
{
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    uint16_t clear;
} rgbw_t;

extern rgbw_t sm_color_table[COLOR_MAX];
extern rgbw_t sm_color_avarage_sum;

extern uint8_t //enum COLOR
SM_Color_Attach (ADJD_S311_Data_t * p_smartie_color);

/********** SM_Color_Correct *********************************************
Function:   SM_Color_Correct()
Purpose:    Call this function to Correct the color_table ....
            The entry of the color_table is replaced by:
            7/8 old entry +1/7 actual value of the *p_smartie_color
Input:      pointer to Sensor_Data_t
Returns:    COLOR
**************************************************************************/
extern void //enum COLOR
SM_Color_Correct(ADJD_S311_Data_t* p_smartie_color,enum COLOR color);
/********** SM_Colors_Store *********************************************
Function:   SM_Colors_Store()
Purpose:    Call this function to store the actual color table to the EEPROM
Input:      pointer to Sensor_Data_t
Returns:    COLOR
**************************************************************************/
extern void //enum COLOR
SM_Colors_Store(void);

/********** SM_Colors_Restore *********************************************
Function:   SM_Colors_Restore()
Purpose:    Call this function to store the actual color table to the EEPROM
Input:      pointer to Sensor_Data_t
Returns:    COLOR
**************************************************************************/
extern void //enum COLOR
SM_Colors_Restore(void);


#endif // _SMARTIES_H

