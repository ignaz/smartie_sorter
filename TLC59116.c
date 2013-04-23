/*
 * =====================================================================================
 *
 *       Filename:  TLC59116.c
 *
 *    Description:  Library for using a TLC59116 LED-driver together with an
 *                  AVR processor. Its controlled via TWI using my TWI_Master.c/.h
 *
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

#include <util/twi.h>


#include <avr/io.h>
#include <stddef.h>
#include "twi_master.h"
#include "TLC59116.h"



/*****************************************************************************
   Function: TLC59116_Init
   Parameters:

   Return value: none

   Purpose: Initialise the LED driver.

******************************************************************************/
    void
TLC59116_Init(void)
{
    uint8_t buffer[4];

    buffer[0]=(TLC59116_ADDRESS<<1);
	buffer[1]=0x80;      // address MODE0 register: auto-increment-mode bit7=1
	buffer[2]=0x80;      // set normal mode(bit4=0) disable "ALL-CALL" Bit0=0
	TWI_Master_Transceive_Message(buffer,3);

    buffer[0]=(TLC59116_ADDRESS<<1);
	buffer[1]=TLC59116_REG_LEDOUT0 | TLC59116_AUTO_INC; // address the LED0
                                        //reg with auto addresincrementation
	buffer[2]=0xFF;     // enable single and global dimming for ch0..ch3 and
	buffer[3]=0xAF;     // for ch4..ch5
                        // and enable single dimming for ch6..ch7 and
	buffer[4]=0x02;     // ch8

	TWI_Master_Transceive_Message(buffer,5);
}

/*****************************************************************************
   Function: TLC59116_Set_Block
   Parameters:  uint8_t * block     poiter to array of PWM values
                uint8_t blockstart  first channel to start with block transfer
                uint8_t blocksize   size of block
   Return value: none

   Purpose: Sets PWM block.

******************************************************************************/
    void
TLC59116_Set_PWM_Block (uint8_t *block,uint8_t blockstart,uint8_t blocksize)
{
    uint8_t buffer[16],ui8_i;
    blockstart &= 0x0F;
    blocksize  &= 0x0F;

        buffer[0] = (TLC59116_ADDRESS<<1);
        buffer[1] = (blockstart+0xA2);        //set $02++ with autorollover

        for (ui8_i=0;ui8_i<blocksize;ui8_i++)
            buffer[2+ui8_i]=*(((uint8_t *)block)+ui8_i);    // set channel value

	TWI_Master_Transceive_Message(buffer,2+blocksize);        // send buffer
}








