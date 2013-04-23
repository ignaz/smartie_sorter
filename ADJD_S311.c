
/*
 * =====================================================================================
 *
 *       Filename:  ADJD_S311.c
 *
 *    Description:  Library for using a ADJD_S311.c Colour sensor together with an
 *                  AVR processor. Its controlled via TWI, using my TWI_Master.c/.h
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


#include <avr/io.h>
#include <stddef.h>
#include "uart.h"
#include "twi_master.h"
#include "ADJD_S311.h"
#include <util/twi.h>



/*****************************************************************************
   Function:        ADJD_S311_Data_Get
   Parameters:      pointer to ADJD_S311_Data_t

   Return value:    error

   Purpose: Initialise the Colour driver.

******************************************************************************/
void ADJD_S311_Data_Get(ADJD_S311_Data_t *SensorData)
{
    uint8_t buffer[3],ui8_i,ui8_temp;

    // write a 1 (GSSR-bit) into the CTRL-register of the ADJD-S311 to initiate
    // a coulor - measurement (integration of photo current and ad-conversion)

    ADJD_S311_Sensor_Start();

    // read back the value of the CTRL register
    // when CTRL is 0, data is ready for readout

    while(ADJD_S311_Reg_Get(ADJD_S311_REG_CTRL));

    // now measurement is done so let`s read data from ADJD-S311
    // address the first ADJD-S311 data register...
    buffer[0] = (ADJD_S311_ADDRESS << 1) | TW_WRITE;
    buffer[1] = ADJD_S311_REG_DATA;
    TWI_Master_Transceive_Message(buffer,2);

            //...and copy the data to the struct SensorData
    buffer[0] = (ADJD_S311_ADDRESS << 1) | TW_READ;
    TWI_Master_Transceive_Message(buffer,sizeof(ADJD_S311_Data_t));
    TWI_Master_Get_Transceiver_Data(SensorData,sizeof(ADJD_S311_Data_t));
};

/*****************************************************************************
   Function: ADJD_S311_Offset_Get
   Parameters:      pointer to ADJD_S311_Offset_t

   Return value:    error

   Purpose: Initialise the Colour driver.

******************************************************************************/
    void
ADJD_S311_Offset_Get(ADJD_S311_Offset_t *SensorOffset)
{
    uint8_t buffer[3],ui8_i,ui8_temp;


    // write a 1 (GSSR-bit) into the CTRL-register of the ADJD-S311 to initiate
    // a coulor - measurement (integration of photo current and ad-conversion)
    // and read back the value of the CTRL register
    // when CTRL is 0, data is ready for readout

    while(ADJD_S311_Reg_Get(ADJD_S311_REG_CTRL));


    // now measurement is done so let`s read data from ADJD-S311
    // address the first ADJD-S311 data register...
    buffer[0] = (ADJD_S311_ADDRESS << 1) | TW_WRITE;
    buffer[1] = ADJD_S311_REG_OFFSET;
    TWI_Master_Transceive_Message(buffer,2);

            //...and copy the data to the struct SensorData
    buffer[0] = (ADJD_S311_ADDRESS << 1) | TW_READ;
    TWI_Master_Transceive_Message(buffer,sizeof(ADJD_S311_Offset_t));
    TWI_Master_Get_Transceiver_Data(SensorOffset,sizeof(ADJD_S311_Offset_t));
}

/*****************************************************************************
   Function:        ADJD_S311_Param_Set
   Parameters:      pointer to ADJD_S311_Param_t

   Return value:    none

   Purpose: Initialise the Colour driver.

******************************************************************************/

void ADJD_S311_Param_Set(ADJD_S311_Param_t *SensorParam)
{
    uint8_t buffer[(sizeof(ADJD_S311_Param_t)+2)],ui8_i;
    uint8_t * p_byte = SensorParam;

    buffer[0] = (ADJD_S311_ADDRESS<<1)| TW_WRITE;
    buffer[1] = ADJD_S311_REG_PARAM;

    for (ui8_i=0;ui8_i<(sizeof(ADJD_S311_Param_t));ui8_i++)
        //buffer[ui8_i+2] = (*(((uint8_t*)SensorParam)+ui8_i));
        buffer[ui8_i+2] = p_byte[ui8_i];

    TWI_Master_Transceive_Message(buffer,(sizeof(ADJD_S311_Param_t)+2));

}

/*****************************************************************************
   Function:        ADJD_S311_Sensor_Start
   Parameters:      none

   Return value:    none

   Purpose: Initialise the Colour driver.

******************************************************************************/

    void
ADJD_S311_Sensor_Start(void)
{
    // write a 1 (GSSR-bit) into the CTRL-register of the ADJD-S311 to initiate
    // a coulor - measurement (integration of photo current and ad-conversion)
    ADJD_S311_Reg_Set(ADJD_S311_REG_CTRL,(_BV(ADJD_S311_BIT_GSSR)));
}

/*****************************************************************************
   Function:        ADJD_S311_Offset_Clear
   Parameters:      none

   Return value:    none

   Purpose: Initialise the Colour driver.

******************************************************************************/

    void
ADJD_S311_Offset_Clear(void)
{
    // write a 1 (GSSR-bit) into the CTRL-register of the ADJD-S311 to initiate
    // a coulor - measurement (integration of photo current and ad-conversion)
    ADJD_S311_Reg_Set(ADJD_S311_REG_CTRL,(_BV(ADJD_S311_BIT_GOFS)));
}


/*****************************************************************************
   Function:        ADJD_S311_Reg_Set
   Parameters:      none

   Return value:    none

   Purpose: Initialise the Colour driver.

******************************************************************************/

    void
ADJD_S311_Reg_Set(uint8_t reg,uint8_t value)
{
    uint8_t buffer[3];

    // write a 1 (GSSR-bit) into the CTRL-register of the ADJD-S311 to initiate
    // a coulor - measurement (integration of photo current and ad-conversion)

    buffer[0] = (ADJD_S311_ADDRESS << 1) | TW_WRITE;
    buffer[1] = reg;
    buffer[2] = value;
    TWI_Master_Transceive_Message(buffer,3);
}

/*****************************************************************************
   Function:        ADJD_S311_Reg_Get
   Parameters:      register to read

   Return value:    value of the register

   Purpose: Initialise the Colour driver.

******************************************************************************/

    uint8_t
ADJD_S311_Reg_Get(uint8_t reg)
{
    uint8_t buffer[2],value;

    // write a 1 (GSSR-bit) into the CTRL-register of the ADJD-S311 to initiate
    // a coulor - measurement (integration of photo current and ad-conversion)

    buffer[0] = (ADJD_S311_ADDRESS << 1) | TW_WRITE;
    buffer[1] = reg;
    TWI_Master_Transceive_Message(buffer,2);

    buffer[0] = (ADJD_S311_ADDRESS << 1) | TW_READ;
    TWI_Master_Transceive_Message(buffer,1);
    TWI_Master_Get_Transceiver_Data(&value,1);
    return value;
}
