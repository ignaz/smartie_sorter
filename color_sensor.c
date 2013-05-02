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
#include "TLC59116.h"
#include "ADJD_S311.h"
#include "debug.h"

#include "color_sensor.h"

#include "smarties.h"

// #define CS_DEBUG 0 //debug in debug.h en-/disabled


/******** Variables of the color sensor **********************************
**************************************************************************/

CS_Sensor_LED_t     cs_sensor_led;
ADJD_S311_Data_t    cs_sensor_data;
ADJD_S311_Param_t   cs_sensor_param;


/******** CS_Init *********************************************************
Function: CS_Init()
Purpose:  initialize the color sensor
Input:    none
Returns:  none
**************************************************************************/
void
CS_Init(void)
{

}

/********** CS_Gain_Addapt ************************************************
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
void
CS_Gain_Addapt(ADJD_S311_Param_t *p_parameter,uint8_t dark_max)
{
    //ADJD_S311_Offset_t cs_offset;
    uint16_t temp_int16 = 0x00F0;
    int8_t offset_clear;    // no real signed value: (no two´s complement)
    // MSB as signed bit is sufficent for comparison...

    // Set number of integration capacitors to max (for maximum Integration time)
    // Set the integraton time to usefull maximum.
    p_parameter->CapRed     = 0xF;
    p_parameter->CapGreen   = 0xF;
    p_parameter->CapBlue    = 0xF;
    p_parameter->CapClear   = 0xF;
    p_parameter->IntRed     = temp_int16;
    p_parameter->IntGreen   = temp_int16;
    p_parameter->IntBlue    = temp_int16;
    p_parameter->IntClear   = temp_int16;

    ADJD_S311_Param_Set(p_parameter);

    //Switch off LED lightsource
    // TLC59116_GRP_PWM_Set(0);

    // switch off offset and sleep function of color sensor
    ADJD_S311_Reg_Set(ADJD_S311_REG_CONFIG,0);
    do
    {
        // do offset measurement
        ADJD_S311_Offset_Clear();
        // get offset values
        while(ADJD_S311_Reg_Get(ADJD_S311_REG_CTRL));

        offset_clear = ADJD_S311_Reg_Get(0x4B); //ADJD_S311_REG_OFFSET_CLEAR);
#if CS_DEBUG
        uart_puts_P("\rOffset: ");
        uart_put_uint16(offset_clear);
        uart_putc(' ');
        uart_puts_P("Integration time slots: ");
        uart_put_uint16(temp_int16);
#endif

        if(offset_clear >= dark_max)    // no real signed value: (no two´s complement)
            // MSB as signed bit is sufficent for comparison...
        {
            temp_int16 --;
            p_parameter->IntRed     =temp_int16;
            p_parameter->IntRed     =temp_int16;
            p_parameter->IntGreen   =temp_int16;
            p_parameter->IntBlue    =temp_int16;
            p_parameter->IntClear   =temp_int16;

            ADJD_S311_Param_Set(p_parameter);
        }
    }
    while(offset_clear >= dark_max);

#if CS_DEBUG
    uart_puts_P("\nI-Timeslots:");
    uart_put_uint16(temp_int16);
#endif
    p_parameter->IntClear >>=1;
    ADJD_S311_Param_Set(p_parameter);


}
/********** CS_LED_Addapt *************************************************
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
void
CS_LED_Addapt(CS_Sensor_LED_t *p_sensor_led,
              uint16_t sensor_val_max)
{
    ADJD_S311_Data_t rgbw_data;
    uint8_t sensor_led_tmp=255;

    TLC59116_GRP_PWM_Set(0xFF);
    _delay_ms(500);


    p_sensor_led->Red0   =sensor_led_tmp;
    p_sensor_led->Red1   =0;
    p_sensor_led->Green0 =0;
    p_sensor_led->Green1 =0;
    p_sensor_led->Blue0  =0;
    p_sensor_led->Blue1  =0;
    TLC59116_Set_PWM_Block(p_sensor_led,0,6);


    // Addapt Red channel
    ADJD_S311_Data_Get(&rgbw_data);
    while(rgbw_data.Red > sensor_val_max)
    {
        sensor_led_tmp --;
        TLC59116_Set_PWM_Channel(4,sensor_led_tmp);
        ADJD_S311_Data_Get(&rgbw_data);
    }
    p_sensor_led->Red0 = sensor_led_tmp;
    TLC59116_Set_PWM_Channel(4,0);
#if CS_DEBUG
    uart_puts_P("\nPWM-Red:");
    uart_put_uint16(sensor_led_tmp);
#endif
//Addapt Green channel
    sensor_led_tmp = 255;
    TLC59116_Set_PWM_Channel(2,sensor_led_tmp);

    ADJD_S311_Data_Get(&rgbw_data);
    while(rgbw_data.Green > sensor_val_max)
    {
        sensor_led_tmp --;
        TLC59116_Set_PWM_Channel(2,sensor_led_tmp);
        ADJD_S311_Data_Get(&rgbw_data);
    }
    p_sensor_led->Green0 = sensor_led_tmp;
    TLC59116_Set_PWM_Channel(2,0);
#if CS_DEBUG
    uart_puts_P("\tPWM-Green:");
    uart_put_uint16(sensor_led_tmp);
#endif
// Addapt Blue channel
    sensor_led_tmp = 255;
    TLC59116_Set_PWM_Channel(0,sensor_led_tmp);

    ADJD_S311_Data_Get(&rgbw_data);
    while(rgbw_data.Blue > sensor_val_max)
    {
        sensor_led_tmp --;
        TLC59116_Set_PWM_Channel(0,sensor_led_tmp);
        ADJD_S311_Data_Get(&rgbw_data);
    }
    p_sensor_led->Blue0 = sensor_led_tmp;
    TLC59116_Set_PWM_Channel(2,0);
#if CS_DEBUG
    uart_puts_P("\tPWM-Blue:");
    uart_put_uint16(sensor_led_tmp);
#endif
    // write addapted channel values (saved @ p_sensor_led) to the TLC59116

    TLC59116_Set_PWM_Block(p_sensor_led,0,6);
    TLC59116_GRP_PWM_Set(0x00);
}


/********** CS_Color_Avarerage_Get ****************************************
Function:   CS_Color_Avarerage_Get()
Purpose:    Call this function to get an avarage color of a smartie
            This function switches on the LED...
            makes a dfined number of measurements wich are summed to an
            avg value
Input:      pointer to Sensor_Data_t
Returns:    none
**************************************************************************/
void
CS_Color_Average_Get(ADJD_S311_Data_t* p_smartie_color)
{
    ADJD_S311_Data_t color_data_sum= {0},color_data_temp;
    uint8_t ui8_tmp;
    //switch on LED
    TLC59116_GRP_PWM_Set(0xFF);
    _delay_ms(200);
    _delay_ms(200);
    _delay_ms(200);


    for (ui8_tmp = 0; ui8_tmp<CS_MEASURE_CNTS; ui8_tmp++)
    {
        ADJD_S311_Data_Get(&color_data_temp);
        color_data_sum.Red    += color_data_temp.Red;
        color_data_sum.Green  += color_data_temp.Green;
        color_data_sum.Blue   += color_data_temp.Blue;
        color_data_sum.Clear  += color_data_temp.Clear;
    }
    p_smartie_color->Red    = color_data_sum.Red >> CS_MEASURE_EXP;
    p_smartie_color->Green  = color_data_sum.Green >> CS_MEASURE_EXP;
    p_smartie_color->Blue   = color_data_sum.Blue >> CS_MEASURE_EXP;
    p_smartie_color->Clear  = color_data_sum.Clear >> CS_MEASURE_EXP;

#if CS_DEBUG
    uart_puts_P("\nred\tgreen\tblue\tclear\n:");
    uart_put_uint16((uint16_t)p_smartie_color->Red);
    uart_puts_P("\t:");
    uart_put_uint16((uint16_t)p_smartie_color->Green);
    uart_puts_P("\t:");
    uart_put_uint16((uint16_t)p_smartie_color->Blue);
    uart_puts_P("\t:");
    uart_put_uint16((uint16_t)p_smartie_color->Clear);
#endif

    _delay_ms(10);

    //switch off LED
    TLC59116_GRP_PWM_Set(0);
}
