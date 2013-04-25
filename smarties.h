/*
 * =====================================================================================
 *
 *       Filename:  smarites.h
 *
 *    Description:  Defines for the datatypes rgbw_t and the enum COLOUR
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
#include "colour_sensor.h"

enum COLOUR {Unknown=0,Red,Orange,Yellow,Green,Blue,Violett,Pink,Brown,COLOUR_MAX};
typedef struct rgbw_s
{
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    uint16_t clear;
} rgbw_t;

extern rgbw_t sm_colour_table[COLOUR_MAX];
extern rgbw_t sm_colour_avarage_sum;

extern uint8_t //enum COLOUR
SM_Colour_Attach (ADJD_S311_Data_t * p_smartie_colour);

/********** SM_Colour_Correct *********************************************
Function:   SM_Colour_Correct()
Purpose:    Call this function to Correct the colour_table ....
            The entry of the colour_table is replaced by:
            7/8 old entry +1/7 actual value of the *p_smartie_colour
Input:      pointer to Sensor_Data_t
Returns:    COLOUR
**************************************************************************/
extern void //enum COLOUR
SM_Colour_Correct(ADJD_S311_Data_t* p_smartie_colour,enum COLOUR colour);
/********** SM_Colours_Store *********************************************
Function:   SM_Colours_Store()
Purpose:    Call this function to store the actual colour table to the EEPROM
Input:      pointer to Sensor_Data_t
Returns:    COLOUR
**************************************************************************/
extern void //enum COLOUR
SM_Colours_Store(void);

/********** SM_Colours_Restore *********************************************
Function:   SM_Colours_Restore()
Purpose:    Call this function to store the actual colour table to the EEPROM
Input:      pointer to Sensor_Data_t
Returns:    COLOUR
**************************************************************************/
extern void //enum COLOUR
SM_Colours_Restore(void);


#endif // _SMARTIES_H

