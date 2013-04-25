/*
 * =====================================================================================
 *
 *       Filename:  smarites.c
 *
 *    Description:  Defines for the datatypes rgbw_t and the enum COLOR
 *                  Function to get the distance to the reference values of a smartie in
 *                  rgbw color space.
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


#include <stdlib.h>
#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include "smarties.h"

#define debug   1
#define RGBW    1

rgbw_t sm_color_table[COLOR_MAX] =
{
    {80,80,60,90},      //none
    {348,127,81,241},   //red
    {386,165,96,279},   //orange
    {406,316,147,373},  //yellow
    {257,290,143,294},  //gree0n
    {245,325,282,353},  //blue
    {209,230,228,283},  //violett
    {296,188,182,285},  //pink
    {219,147,109,203}
};  //brown

const rgbw_t EEMEM sm_color_table_ee[COLOR_MAX];


rgbw_t sm_color_avarage_sum;




/********** SM_Color_Attach *********************************************
Function:   SM_Color_Attach()
Purpose:    Call this function to get the nearest color of the refernce
            respective the actual values in the p_smartie_color....
Input:      pointer to Sensor_Data_t
Returns:    COLOR
**************************************************************************/
uint8_t //enum COLOR
SM_Color_Attach(ADJD_S311_Data_t* p_smartie_color)
{
    int16_t difference;
    int32_t distance_square, distance_square_min = 0x0FFFFFFF;
    uint8_t nearest_col = 0;

    for(uint8_t col=0; col < COLOR_MAX; col++)
    {
#if SM_DEBUG
        uart_puts_P("\t\nch:");
        uart_put_uint16((uint16_t)col);
#endif
        difference       =  (sm_color_table[col].red    - p_smartie_color->Red);
#if SM_DEBUG
        uart_puts_P("\tdR:");
        uart_put_uint16(difference);
#endif
        distance_square =  ((int32_t) difference)*((int32_t) difference);

        difference       =  (sm_color_table[col].green  - p_smartie_color->Green);
#if SM_DEBUG
        uart_puts_P("\tdG:");
        uart_put_uint16(difference);
#endif
        distance_square +=  ((int32_t) difference)*((int32_t) difference);

        difference       =  (sm_color_table[col].blue   - p_smartie_color->Blue);
#if SM_DEBUG
        uart_puts_P("\tdB:");
        uart_put_uint16(difference);
#endif
        distance_square +=  ((int32_t) difference)*((int32_t) difference);

#if RBGW_DISTANCE
        difference       =  (sm_color_table[col].clear  - p_smartie_color->Clear);
#if SM_DEBUG
        uart_puts_P("\tdC:");
        uart_put_uint16(difference);
#endif
        distance_square +=  ((int32_t) difference)*((int32_t) difference);
#endif
#if SM_DEBUG
        uart_puts_P("\tdAll:");
        uart_put_uint16((uint16_t)(distance_square>>8));
        //uart_put_uint16((uint16_t)(distance_square));
#endif

        if(distance_square < distance_square_min)
        {
            distance_square_min = distance_square;
            nearest_col = col;
#if SM_DEBUG
            uart_puts_P("\ncolor:");
            uart_put_uint16(col);
#endif

        }
    }
    return nearest_col;
}


/********** SM_Color_Correct *********************************************
Function:   SM_Color_Correct()
Purpose:    Call this function to Correct the color_table ....
            The entry of the color_table is replaced by:
            7/8 old entry +1/7 actual value of the *p_smartie_color
Input:      pointer to Sensor_Data_t
Returns:    COLOR
**************************************************************************/
void //enum COLOR
SM_Color_Correct(ADJD_S311_Data_t* p_smartie_color,enum COLOR color)
{
    sm_color_avarage_sum.red   = ((7*sm_color_table[color].red   +p_smartie_color->Red)>>3);
    sm_color_avarage_sum.green = ((7*sm_color_table[color].green +p_smartie_color->Green)>>3);
    sm_color_avarage_sum.blue  = ((7*sm_color_table[color].blue  +p_smartie_color->Blue)>>3);
    sm_color_avarage_sum.clear = ((7*sm_color_table[color].clear +p_smartie_color->Clear)>>3);

    sm_color_table[color].red     = sm_color_avarage_sum.red;
    sm_color_table[color].green   = sm_color_avarage_sum.green;
    sm_color_table[color].green   = sm_color_avarage_sum.blue;
    sm_color_table[color].green   = sm_color_avarage_sum.clear;
}


/********** SM_Colors_Store *********************************************
Function:   SM_Colors_Store()
Purpose:    Call this function to store the actual color table to the EEPROM
Input:      pointer to Sensor_Data_t
Returns:    COLOR
**************************************************************************/
void //enum COLOR
SM_Colors_Store(void)
{
    eeprom_busy_wait();
    eeprom_write_block((const void*)sm_color_table,
                       (void*) sm_color_table_ee,
                       sizeof(sm_color_table[COLOR_MAX]));
}
/********** SM_Colors_Restore *********************************************
Function:   SM_Colors_Restore()
Purpose:    Call this function to store the actual color table to the EEPROM
Input:      pointer to Sensor_Data_t
Returns:    COLOR
**************************************************************************/
void //enum COLOR
SM_Colors_Restore(void)
{
    eeprom_busy_wait();
    eeprom_read_block(  (void*)sm_color_table,
                        (const void*) sm_color_table_ee,
                        sizeof(sm_color_table[COLOR_MAX]));
    eeprom_busy_wait();
}

