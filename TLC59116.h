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

#ifndef _TLC59116_H
#define _TLC59116_H

#include <avr/io.h>
#include <stddef.h>
#include "twi_master.h"


#define TLC59116_ADDRESS    0x62



#define TLC59116_REG_MODE1      0x00
#define TLC59116_REG_MODE2      0x01
#define TLC59116_REG_PWM0       0x02
// ...
#define TLC59116_REG_GRPPWM     0x12
#define TLC59116_REG_GRPFREQ    0x13
#define TLC59116_REG_LEDOUT0    0x14
#define TLC59116_REG_LEDOUT1    0x15
#define TLC59116_REG_LEDOUT2    0x16
#define TLC59116_REG_LEDOUT3    0x17

#define TLC59116_REG_IREF       0x1C

#define TLC59116_REG_EFLAG1     0x1D
#define TLC59116_REG_EFLAG2     0x1E

#define TLC59116_AUTO_INC       0x80

/*****************************************************************************
   Function: TLC59116_Init
   Parameters:

   Return value: none

   Purpose: Initialise the LED driver.

******************************************************************************/
extern void
TLC59116_Init(void);

/*****************************************************************************
   Function: TLC59116_Set_Channel
   Parameters:  uint8_t channel     selects channel to set (0..15)
                uint8_t value       sets channelvalue (0..255)
   Return value: none

   Purpose: Sets single PWM value .

******************************************************************************/
// extern void
# define TLC59116_Set_PWM_Channel(channel,value) \
    TWI_Master_Write_Register(((channel&15)+2),value,TLC59116_ADDRESS)

/*****************************************************************************
   Function: TLC59116_Set_Block
   Parameters:  uint8_t * block     poiter to array of PWM values
                uint8_t blockstart  first channel to start with block transfer
                uint8_t blocksize   size of block
   Return value: none

   Purpose: Sets PWM block.

******************************************************************************/
extern void
TLC59116_Set_PWM_Block (uint8_t *block,uint8_t blockstart,uint8_t blocksize);


/*****************************************************************************
   Function: TLC59116_Register_Get
   Parameters:  uint8_t reg      selects register to read
   Return value: none

   Purpose: Read a single register of the TLC59116
******************************************************************************/

#define TLC59116_Register_Get(reg) TWI_Master_Read_Register(reg,TLC59116_ADDRESS)

/*****************************************************************************
   Function: TLC59116_GRP_PWM_Set
   Parameters:  uint8_t pwm
   Return value: none

   Purpose: sets the grp_pwm register to the given value
******************************************************************************/

#define TLC59116_GRP_PWM_Set(pwm) \
    TWI_Master_Write_Register(TLC59116_REG_GRPPWM,pwm,TLC59116_ADDRESS)


#endif // _TLC59116_H
