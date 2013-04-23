/*
 * =====================================================================================
 *
 *       Filename:  smarites.c
 *
 *    Description:  Defines for the datatypes rgbw_t and the enum COLOUR
 *                  Function to get the distance to the reference values of a smartie in
 *                  rgbw colour space.
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
#define debug_1 1
#define RGBW    1

rgbw_t sm_colour_table[COLOUR_MAX] ={
        {80,80,60,90},      //none
        {348,127,81,241},   //red
        {386,165,96,279},   //orange
        {406,316,147,373},  //yellow
        {257,290,143,294},  //gree0n
        {245,325,282,353},  //blue
        {209,230,228,283},  //violett
        {296,188,182,285},  //pink
        {219,147,109,203}};  //brown

const rgbw_t EEMEM sm_colour_table_ee[COLOUR_MAX];


rgbw_t sm_colour_avarage_sum;




/********** SM_Colour_Attach *********************************************
Function:   SM_Colour_Attach()
Purpose:    Call this function to get the nearest colour of the refernce
            respective the actual values in the p_smartie_colour....
Input:      pointer to Sensor_Data_t
Returns:    COLOUR
**************************************************************************/
    uint8_t //enum COLOUR
SM_Colour_Attach(ADJD_S311_Data_t* p_smartie_colour)
{
    int16_t difference;
    int32_t distance_square, distance_square_min = 0x0FFFFFFF;
    uint8_t nearest_col = 0;

    for(uint8_t col=0;col < COLOUR_MAX;col++)
    {
        #if debug_1
        uart_puts_P("\t\nch:");
        uart_put_uint16((uint16_t)col);
        #endif
        difference       =  (sm_colour_table[col].red    - p_smartie_colour->Red);
        #if debug_1
        uart_puts_P("\tdR:");
        uart_put_uint16(difference);
        #endif
        distance_square =  ((int32_t) difference)*((int32_t) difference);

        difference       =  (sm_colour_table[col].green  - p_smartie_colour->Green);
        #if debug_1
        uart_puts_P("\tdG:");
        uart_put_uint16(difference);
        #endif
        distance_square +=  ((int32_t) difference)*((int32_t) difference);

        difference       =  (sm_colour_table[col].blue   - p_smartie_colour->Blue);
        #if debug_1
        uart_puts_P("\tdB:");
        uart_put_uint16(difference);
        #endif
        distance_square +=  ((int32_t) difference)*((int32_t) difference);

#ifdef RBGW_DISTANCE
        difference       =  (sm_colour_table[col].clear  - p_smartie_colour->Clear);
        #if debug_1
        uart_puts_P("\tdC:");
        uart_put_uint16(difference);
        #endif
        distance_square +=  ((int32_t) difference)*((int32_t) difference);
#endif
        #if debug_1
        uart_puts_P("\tdAll:");
        uart_put_uint16((uint16_t)(distance_square>>8));
        //uart_put_uint16((uint16_t)(distance_square));
        #endif

        if(distance_square < distance_square_min)
        {
            distance_square_min = distance_square;
            nearest_col = col;
            #if debug_1
            uart_puts_P("\ncolour:");
            uart_put_uint16(col);
            #endif

        }
    }
    return nearest_col;
}


/********** SM_Colour_Correct *********************************************
Function:   SM_Colour_Correct()
Purpose:    Call this function to Correct the colour_table ....
            The entry of the colour_table is replaced by:
            7/8 old entry +1/7 actual value of the *p_smartie_colour
Input:      pointer to Sensor_Data_t
Returns:    COLOUR
**************************************************************************/
    void //enum COLOUR
SM_Colour_Correct(ADJD_S311_Data_t* p_smartie_colour,enum COLOUR colour)
{
    sm_colour_avarage_sum.red   = ((7*sm_colour_table[colour].red   +p_smartie_colour->Red)>>3);
    sm_colour_avarage_sum.green = ((7*sm_colour_table[colour].green +p_smartie_colour->Green)>>3);
    sm_colour_avarage_sum.blue  = ((7*sm_colour_table[colour].blue  +p_smartie_colour->Blue)>>3);
    sm_colour_avarage_sum.clear = ((7*sm_colour_table[colour].clear +p_smartie_colour->Clear)>>3);

    sm_colour_table[colour].red     = sm_colour_avarage_sum.red;
    sm_colour_table[colour].green   = sm_colour_avarage_sum.green;
    sm_colour_table[colour].green   = sm_colour_avarage_sum.blue;
    sm_colour_table[colour].green   = sm_colour_avarage_sum.clear;
}


/********** SM_Colours_Store *********************************************
Function:   SM_Colours_Store()
Purpose:    Call this function to store the actual colour table to the EEPROM
Input:      pointer to Sensor_Data_t
Returns:    COLOUR
**************************************************************************/
    void //enum COLOUR
SM_Colours_Store(void)
{
    eeprom_busy_wait();
    eeprom_write_block((const void*)sm_colour_table,
                       (void*) sm_colour_table_ee,
                       sizeof(sm_colour_table[COLOUR_MAX]));
}
/********** SM_Colours_Restore *********************************************
Function:   SM_Colours_Restore()
Purpose:    Call this function to store the actual colour table to the EEPROM
Input:      pointer to Sensor_Data_t
Returns:    COLOUR
**************************************************************************/
    void //enum COLOUR
SM_Colours_Restore(void)
{
    eeprom_busy_wait();
    eeprom_read_block(  (void*)sm_colour_table,
                        (const void*) sm_colour_table_ee,
                        sizeof(sm_colour_table[COLOUR_MAX]));
    eeprom_busy_wait();
}

