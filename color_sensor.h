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

#ifndef _COLOR_SENSOR_H
#define _COLOR_SENSOR_H

#include <stdlib.h>
#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/pgmspace.h>
#include <util/twi.h>

#include "uart.h"
#include "twi_master.h"
#include "TLC59116.h"
#include "ADJD_S311.h"
#include "debug.h"

// definition of the nummbers of colormesurements for avverage measurement
#define CS_MEASURE_CNTS     (1<<CS_MEASURE_EXP) //
#define CS_MEASURE_EXP       2                   //Bitte eintragen!! 0->1 time
// 1-> 2times; 2->4times

#define CS_MIN_VAL      31      // This value is the maximum value at dark measurement
#define CS_MAX_VAL      600     // This value is achived at single channel LED addapttion


typedef struct CS_Sensor_LED_s
{
    unsigned Blue0:     8;
    unsigned Blue1:     8;
    unsigned Green0:    8;
    unsigned Green1:    8;
    unsigned Red0:      8;
    unsigned Red1:      8;
} CS_Sensor_LED_t;


/******** global variables of the color sensor ***************************
**************************************************************************/

extern CS_Sensor_LED_t     cs_sensor_led;
extern ADJD_S311_Data_t    cs_sensor_data;
extern ADJD_S311_Param_t   cs_sensor_param;


/*************************************************************************
Function: CS_Init()
Purpose:  initialize the color sensor
Input:    none
Returns:  none
**************************************************************************/
extern void
CS_Init(void);

/*************************************************************************
Function:   CS_Gain_Addapt()
Purpose:    Call this function to addapt the sensor gain.
            It´s thougt to be called when only passive light reaches the
            sensor area, which should be placed over a referecne white!
            According to the ambient light, this function will decrease
            the sensitivity until every color channel is beyond a given
            threshold
Input:      pointer to Sensor_Param_t,threshold
Returns:    none
**************************************************************************/
extern void
CS_Gain_Addapt(ADJD_S311_Param_t *p_parameter,uint8_t dark_max);


/*************************************************************************
Function:   CS_LED_Addapt()
Purpose:    Call this function to addapt the light to the sensetivity.
            It´s thougt to be called when only passive light reaches the
            sensor area, which should be placed over a referecne white!
            According to the sensitivity and ambient light, this function
            will decrease PWM values of the single LED until every color
            channel is beyond a given threshold
Input:      pointer to LED-PWM-Values,threshold
Returns:    none
**************************************************************************/
extern void
CS_LED_Addapt(CS_Sensor_LED_t *p_sensor_led,
              uint16_t sensor_val_max);

/*************************************************************************
Function:   CS_Color_Avarerage_Get()
Purpose:    Call this function to get an avarage color of a smartie
            This function switches on the LED...
            makes a dfined number of measurements wich are summed to an
            avg value
Input:      pointer to Sensor_Data_t
Returns:    none
**************************************************************************/
extern void
CS_Color_Average_Get(ADJD_S311_Data_t* p_smartie_color);

#endif // _COLOR_SENSOR_H

