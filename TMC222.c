/*
 * =====================================================================================
 *
 *       Filename:  TMC222.c
 *
 *    Description:  Library for using a TMC222 together with an AVR processor.
 *
 *                  This file is just a mdified version of the example from Trinamics.
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



#include <stddef.h>
#include "twi_master.h"
#include "TMC222.h"

/*****************************************************************************
   Function: GetFullStatus1()
   Parameters:

   Return value: twi-error

   Purpose: Read out the status information of the TMC222.

******************************************************************************/
uint8_t
TMC222_GetFullStatus1      (TMC222_Status_t *TMC222Status,
                            uint8_t  address)
{
    uint8_t data[8];
    uint8_t err;

    // configuration
    data[0] = (address << 1) | TWI_WRITE_BIT;       // number of transmitted byte
    data[1] = 0x81;                        // Set command byte for GetFullStatus1
    if((err=TWI_Master_Transceive_Message((uint8_t *)data,2))) 	// send buffer
        return err;
    //while (err = TWI_Master_Get_State()); 				// wait till sent

    data[0] = (address << 1) | TWI_READ_BIT;
    if ((err=TWI_Master_Transceive_Message((uint8_t *)data,8)))
        return err;
    err=TWI_Master_Get_Transceiver_Data((uint8_t *)TMC222Status,7);
    return err;
}


/*****************************************************************************
   Function: GetFullStatus2()
   Parameters:  *TargetPosition:Pointer to variable for the target position
                                or NULL.
                *SecurePosition:Pointer to variable for the secure position
                                or NULL.
                *ActualPosition:Pointer to variable for the actual position
                                or NULL.

   Return value: twi-error

   Purpose: Read all TMC222 position registers (using the GetFullStatus2 command).
            NULL can be used for values that are not needed.
******************************************************************************/

uint8_t
TMC222_GetFullStatus2      (int16_t*ActualPosition,
                            int16_t*TargetPosition,
                            int16_t*SecurePosition,
                            uint8_t address)
{
    uint8_t data[13];
    uint8_t  err;

    // configuration
    data[0] = (address << 1) | TWI_WRITE_BIT;       // number of transmitted byte
    data[1] = 0xfc;      // Set command byte for GetFullStatus2

    if((err=TWI_Master_Transceive_Message((uint8_t *)data,2))) 	// send buffer
        return err;
    //while (err = TWI_Master_Get_State()); 				// wait till sent

    data[0] = (address << 1) | TWI_READ_BIT;
    if((err=TWI_Master_Transceive_Message((uint8_t *)data,9)))
        return err;
    if((err=TWI_Master_Get_Transceiver_Data((uint8_t *)data,9)))
        return err;

    if(ActualPosition!=NULL) *ActualPosition=(data[1]<<8) | data[2];
    if(TargetPosition!=NULL) *TargetPosition=(data[3]<<8) | data[4];
    if(SecurePosition!=NULL) *SecurePosition=((data[6] & 0x07)<<8) | data[5];

    return 0;
}


/*****************************************************************************
   Function: GetOTPParam()

   Parameters:

   Return value:

   Purpose:
******************************************************************************/
/*uint8_t
GetOTPParam                         (uint8_t address)
{
    uint8_t data[13];
    uint8_t  err;


        // OTP@0x00
        // OTP@0x01
        // OTP@0x02
        // OTP@0x03
        // OTP@0x04
        // OTP@0x05
        // OTP@0x06
        // OTP@0x07
    }

    return err;
}*/



/*****************************************************************************
   Function: SetMotorParameters()
   Parameters: *TMC222Parameters: Pointer to TMC222Parameters_t structure
                                  containing all parameters.

   Return value: twi-error

   Purpose: Set the TMC222 motor parameters.
******************************************************************************/
uint8_t
TMC222_SetMotorParameters                     (TMC222_Parameters_t *TMC222Parameters,
        uint8_t address)
{
    uint8_t data[9];
    uint8_t i;
    uint8_t  err;

    // configuration
    data[0] = (address << 1) | TWI_WRITE_BIT;       // number of transmitted byte
    data[1]=0x89;  //Command byte for SetMotorParameters (0x89)
    data[2]=0xff;
    data[3]=0xff;

    for(i=0; i<(sizeof(TMC222_Parameters_t)); i++)
        data[i+4] = *(((uint8_t *) TMC222Parameters)+i);

    err=TWI_Master_Transceive_Message((uint8_t *)data,9);// send buffer
    return err;
}


/*****************************************************************************
   Function: SetPosition()
   Parameters: Position: Target position

   Return value: twi-error

   Purpose: Set the target position. This makes the motor move to the new
            target position (if the motor parameters have been set properly).
******************************************************************************/
uint8_t
TMC222_SetPosition(int16_t Position,uint8_t address)
{
    uint8_t data[6];
    uint8_t  err;

    // configuration
    data[0] = (address << 1) | TWI_WRITE_BIT;     //adrdress byte with dir bit
    data[1] = 0x8b;                         //Command byte for SetPosition (0x8b)
    data[2] = 0xff;
    data[3] = 0xff;
    data[4] = (uint8_t) (Position >> 8);
    data[5] = (uint8_t) Position & 0xff;

    //Start TWI communication
    err=TWI_Master_Transceive_Message((uint8_t *)data,6);// send buffer
    return err;
}


/*****************************************************************************
   Function: ResetPosition()
   Parameters: ---

   Return value: twi-error

   Purpose: Set actual position and target position to zero (without moving the
            motor).
******************************************************************************/
uint8_t
TMC222_ResetPosition(uint8_t address)
{
    return TWI_Master_Write_Byte(0x86,address);

}


/*****************************************************************************
   Function: GotoSecurePosition()
   Parameters: ---

   Return value: twi-error

   Purpose: Drive the motor to the secure position (that has been set using
   the SetMotorParameters command).
******************************************************************************/
uint8_t
TMC222_GotoSecurePosition(uint8_t address)
{
    //Command byte for GotoSecurePosition (0x84)
    return TWI_Master_Write_Byte(0x84,address);

}


/*****************************************************************************
   Function: ResetToDefault()
   Parameters: ---

   Return value: twi-error

   Purpose: Reset all values to the default values (from the OTP).
******************************************************************************/
uint8_t
TMC222_ResetToDefault(uint8_t address)
{
    //Command byte for ResetToDefault (0x87)
    return TWI_Master_Write_Byte(0x87,address);
}


/*****************************************************************************
   Function: RunInit()
   Parameters: VMin: Velocity 1 (0..15)
               VMax: Velocity 2 (0..15)
               Position1: Position 1
               Position2: Position 2

   Return value: twi-error

   Purpose: RunInit command (reference search) of the TMC222 (please see the
            TMC222 data sheet for details).
******************************************************************************/
uint8_t
TMC222_RunInit(uint8_t VMin, uint8_t VMax, int16_t Position1, int16_t Position2,uint8_t address)
{
    uint8_t data[9],err;

    //Send RunInit command and the data
    data[0] = (address << 1) | TWI_WRITE_BIT;     //adrdress byte with dir bit
    data[1]=0x88;  //Command byte for RunInit (0x88)
    data[2]=0xff;
    data[3]=0xff;
    data[4]=(VMax << 4) | (VMin & 0x0f);
    data[5]=BYTE1(Position1);
    data[6]=BYTE0(Position1);
    data[7]=BYTE1(Position2);
    data[8]=BYTE0(Position2);

    //Start TWI communication
    err=TWI_Master_Transceive_Message((uint8_t *)data,9);// send buffer
    return err;
}


/*****************************************************************************
   Function: HardStop()
   Parameterss: ---

   Return value: twi-error

   Purpose: Hard stop of the motor.
            Afterwards the error flags have to be reset using a GetFullStatus1
            command, as otherwise the motor will not run any more.
            (HardStop sets some error flags in the TMC222 that prevent running
             until they have been reset).
******************************************************************************/
uint8_t
TMC222_HardStop(uint8_t address)
{
    //Command byte for HardStop (0x85)
    return TWI_Master_Write_Byte(0x85,address);
}


/*****************************************************************************
   Function: SoftStop()
   Parameters: ---

   Return value: twi-error

   Purpose: Stop the motor using the normal decelaration.
******************************************************************************/
uint8_t
TMC222_SoftStop(uint8_t address)
{
    //Command byte for SoftStop (0x8f)
    return TWI_Master_Write_Byte(0x8f,address);
}


/*****************************************************************************
   Function: GetMotorParameters()
   Parameters TMC222Parameters: Pointer to data structure that will contain
              all parameters read from the TMC222.

   Return value: twi-error

   Purpose: This function uses a GetFullStatus1 ccommand and a GetFullStatus2
            command to read out motor parameters into a TTTMC222Parmaters
            data structure.
            After that, the desired values can be modified and the same data
            structure can then be used wit SetMotorParameters again.
******************************************************************************/
uint8_t TMC222_GetMotorParameters(TMC222_Parameters_t *TMC222Parameters,uint8_t address)
{
    TMC222_Status_t TMC222Status;
    uint8_t err;
    int16_t SecurePosition;

    if((err=TMC222_GetFullStatus1(&TMC222Status,address)))
        return err;

    TMC222Parameters->IRun=TMC222Status.IRun;
    TMC222Parameters->IHold=TMC222Status.IHold;
    TMC222Parameters->VMin=TMC222Status.VMin;
    TMC222Parameters->VMax=TMC222Status.VMax;
    TMC222Parameters->Acc=TMC222Status.Acc;
    TMC222Parameters->AccShape=TMC222Status.AccShape;
    TMC222Parameters->Shaft=TMC222Status.Shaft;
    TMC222Parameters->StepMode=TMC222Status.StepMode;

    if((err=TMC222_GetFullStatus2(NULL, &SecurePosition,NULL,address)))
        return err;
    TMC222Parameters->SecPosHi=BYTE1(SecurePosition);
    TMC222Parameters->SecPosLo=BYTE0(SecurePosition);
    return 0;
}



/*****************************************************************************
   Function: TMC222_GetMotionStatus()
   Parameters e TMC222.

   Return value: motion_status

   Purpose: This function uses a GetFullStatus1 command
            After that it returns the motion status
******************************************************************************/
uint8_t TMC222_GetMotionStatus(TMC222_Status_t * TMC222Status,uint8_t address)
{
    TMC222_GetFullStatus1(TMC222Status,address);
    return ((uint8_t)TMC222Status->Motion);
}
