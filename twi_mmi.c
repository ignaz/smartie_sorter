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



#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "twi_master.h"
#include "twi_slave.h"
#include "twi_lcd.h"
#include <util/twi.h>
#include "uart.h"

#define debug 0

volatile int8_t rot_enc_dir=0;

/*************************************************************************
Function: RotEnc_Get_Dir()
Purpose:
Input:    none
Returns:  -1 when turned left, +1 when turned right
**************************************************************************/
int8_t
RotEnc_Get_Dir(void)
{
    static int8_t old_value=0,new_value=0;
    int8_t dif_value;
    if((TWI_Master_Read_Byte(TWI_LCD_ADRESS))&(_BV(ROT_ENC_EVENT)))
    {
        new_value = TWI_Master_Read_Register(0x02,TWI_LCD_ADRESS);
        if((dif_value = new_value - old_value)>0) rot_enc_dir = 1;
        else if(dif_value<0) rot_enc_dir = -1;
        old_value = new_value;
    }
    return rot_enc_dir;
}

/*************************************************************************
Function: RotEnc_Pushed()
Purpose:
Input:    none
Returns:  -1 when turned left, +1 when turned right
**************************************************************************/
uint8_t
RotEnc_Pushed(void)
{
    uint8_t tmp_value;
    if((TWI_Master_Read_Byte(TWI_LCD_ADRESS))&(_BV(PORT_X_EVENT)))
    {
        tmp_value = TWI_Master_Read_Register(0x01,TWI_LCD_ADRESS);
        if(tmp_value & _BV(5)) return 1;
        else return 0;
    }
}
