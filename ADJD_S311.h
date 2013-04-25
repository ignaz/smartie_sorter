/*
 * =====================================================================================
 *
 *       Filename:  ADJD_S311.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  24.08.2010 21:21:39
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ignaz Laepple (mn), ignaz.laepple@gmx.de
 *        Company:  FH-Regensburg
 *
 * =====================================================================================
 */

#ifndef ADJD_S311_H
#define ADJD_S311_H

#include <stddef.h>

/**************************************************************************************
    Defines: Register addresses of the ADJD_S311

*************************************************************************************/

#define ADJD_S311_ADDRESS       0x74

// bits of the CTRL register:
#define ADJD_S311_BIT_GSSR      0   // GetSenSorReading
#define ADJD_S311_BIT_GOFS      1   // GetOffSetReading

// bits of the CONFIG-register:
#define ADJD_S311_BIT_TOFS      0   // TrimOffSet mode
#define ADJD_S311_BIT_SLEEP     1   // Sleep mode


// addresses of the different registers
#define ADJD_S311_REG_CTRL      0x00
#define ADJD_S311_REG_CONFIG    0x01
#define ADJD_S311_REG_DATA      0x40
#define ADJD_S311_REG_PARAM     0x06
#define ADJD_S311_REG_OFFSET    0x48




/***************************************************************************************
    Data structure that holds sensor parameters like Cap(acity)for integration of the
    photocurrent and Int(egration time slots)
***************************************************************************************/
typedef struct ADJD_S311_Param_s{
		unsigned CapRed:	8;
		unsigned CapGreen: 	8;
        unsigned CapBlue:  	8;
		unsigned CapClear:  8;
		unsigned IntRed: 	16;
		unsigned IntGreen:	16;
		unsigned IntBlue: 	16;
		unsigned IntClear:	16;
}ADJD_S311_Param_t;

//Data structure that holds the color data
typedef struct ADJD_S311_Data_s{
		uint16_t	Red 	:16;
		uint16_t	Green	:16;
		uint16_t	Blue 	:16;
		uint16_t	Clear 	:16;
}ADJD_S311_Data_t;

//Data structure that holds the color offset (reference has to be set first)
typedef struct ADJD_S311_Offset_s{
		int8_t Red 		:8;
		int8_t Green 	:8;
		int8_t Blue 	:8;
		int8_t Clear 	:8;
}ADJD_S311_Offset_t;


/*****************************************************************************
   Function:        ADJD_S311_Data_Get
   Parameters:      pointer to ADJD_S311_Data_t

   Return value:    error

   Purpose: Initialise the Color driver.

******************************************************************************/
    extern void
ADJD_S311_Data_Get(ADJD_S311_Data_t *SensorData);

/*****************************************************************************
   Function: ADJD_S311_Offset_Get
   Parameters:      pointer to ADJD_S311_Offset_t

   Return value:    error

   Purpose: Initialise the Color driver.

******************************************************************************/
    extern void
ADJD_S311_Offset_Get(ADJD_S311_Offset_t *SensorOffset);


/*****************************************************************************
   Function:        ADJD_S311_Param_Set
   Parameters:      pointer to ADJD_S311_Param_t

   Return value:    none

   Purpose: Initialise the Color driver.

******************************************************************************/

    extern void
ADJD_S311_Param_Set(ADJD_S311_Param_t *SensorParam);

/*****************************************************************************
   Function:        ADJD_S311_Sensor_Start
   Parameters:      none

   Return value:    none

   Purpose: Initialise the Color driver.

******************************************************************************/

    extern void
ADJD_S311_Sensor_Start(void);

/*****************************************************************************
   Function:        ADJD_S311_Offset_Clear
   Parameters:      none

   Return value:    none

   Purpose: Initialise the Color driver.

******************************************************************************/

    extern void
ADJD_S311_Offset_Clear(void);


/*****************************************************************************
   Function:        ADJD_S311_Ctrl_Set
   Parameters:      none

   Return value:    none

   Purpose: Initialise the Color driver.

******************************************************************************/

    extern void
ADJD_S311_Reg_Set(uint8_t reg,uint8_t value);
/*****************************************************************************
   Function:        ADJD_S311_Reg_Get
   Parameters:      register to read

   Return value:    value of the register

   Purpose: Initialise the Color driver.

******************************************************************************/

   extern uint8_t
ADJD_S311_Reg_Get(uint8_t reg);

#endif //ADJD_S311_H
