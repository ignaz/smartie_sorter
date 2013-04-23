/*
 * =====================================================================================
 *
 *       Filename:  TMC222.h
 *
 *    Description: 	Header file for the TMC222.c based on the reference code from
 *    							Trinamic.com
 *
 *        Version:  1.0
 *        Created:  24.08.2010 22:16:47
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ignaz Laepple (mn), ignaz.laepple@gmx.de
 *        Company:  FH-Regensburg
 *
 * =====================================================================================
 */

#ifndef TMC222_H
#define TMC222_H

#include "twi_master.h"


#define BYTE3(var) *((uint8_t *) &var+3)
#define BYTE2(var) *((uint8_t *) &var+2)
#define BYTE1(var) *((uint8_t *) &var+1)
#define BYTE0(var) *((uint8_t *) &var)

/*****************************************************************************
Data structure definition for the TMC222 status information.
Use variables of this data type togehter with the GetFullStatus1() function.
*****************************************************************************/
typedef struct TMC222_Status_s{
    uint8_t adr:    8;
    uint8_t IHold:  4;
    uint8_t IRun:   4;
    uint8_t VMin:   4;
    uint8_t VMax:   4;
    uint8_t Acc:    4;
    uint8_t Shaft:  1;
    uint8_t StepMode: 2;
    uint8_t AccShape: 1;
    uint8_t TInfo:  2;
    uint8_t TW:     1;
    uint8_t TSD:    1;
    uint8_t UV2:    1;
    uint8_t ElDef:  1;
    uint8_t SteppLoss: 1;
    uint8_t VddReset: 1;
    uint8_t CPFail: 1;
    uint8_t NA:     1;
    uint8_t OVC2:   1;
    uint8_t OVC1:   1;
    uint8_t ESW:    1;
    uint8_t Motion: 3;
    uint16_t NA2:   16;
    } TMC222_Status_t;

/*****************************************************************************
Data structure definition for the TMC222 motor parameters.
Use variables of this type together with the SetMotorParameters()
and GetMotorParameters() functions.
*****************************************************************************/

typedef struct TMC222_Parameters_s{
  uint8_t IHold: 4;
  uint8_t IRun: 4;
  uint8_t VMin: 4;
  uint8_t VMax: 4;
  uint8_t Acc: 4;
  uint8_t Shaft: 1;
  uint8_t SecPosHi: 3;
  uint8_t SecPosLo: 8;
  uint8_t NA1: 2;
  uint8_t StepMode: 2;
  uint8_t AccShape: 1;
  uint8_t NA2: 3;
} TMC222_Parameters_t;


/*****************************************************************************
   Function: GetFullStatus1()
   Parameters:

   Return value: twi-error

   Purpose: Read out the status information of the TMC222.

******************************************************************************/
uint8_t
TMC222_GetFullStatus1      (TMC222_Status_t *TMC222Status,
                            uint8_t  address);

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
TMC222_GetFullStatus2      (int16_t*TargetPosition,
                            int16_t*SecurePosition,
                            int16_t*ActualPosition,
                            uint8_t address);


/*****************************************************************************
   Function: GetOTPParam()

   Parameters:

   Return value:

   Purpose:
******************************************************************************/
/*uint8_t
GetOTPParam     (uint8_t address);*/



/*****************************************************************************
   Function: SetMotorParameters()
   Parameters: *TMC222Parameters: Pointer to TMC222Parameters_t structure
                                  containing all parameters.

   Return value: twi-error

   Purpose: Set the TMC222 motor parameters.
******************************************************************************/
extern uint8_t
TMC222_SetMotorParameters       (TMC222_Parameters_t *TMC222Parameters,
                                 uint8_t address);


/*****************************************************************************
   Function: SetPosition()
   Parameters: Position: Target position

   Return value: twi-error

   Purpose: Set the target position. This makes the motor move to the new
            target position (if the motor parameters have been set properly).
******************************************************************************/
extern uint8_t
TMC222_SetPosition(int16_t Position,uint8_t address);

/*****************************************************************************
   Function: ResetPosition()
   Parameters: ---

   Return value: twi-error

   Purpose: Set actual position and target position to zero (without moving the
            motor).
******************************************************************************/
extern uint8_t
TMC222_ResetPosition(uint8_t address);

/*****************************************************************************
   Function: GotoSecurePosition()
   Parameters: ---

   Return value: twi-error

   Purpose: Drive the motor to the secure position (that has been set using
   the SetMotorParameters command).
******************************************************************************/
extern uint8_t
TMC222_GotoSecurePosition(uint8_t address);


/*****************************************************************************
   Function: ResetToDefault()
   Parameters: ---

   Return value: twi-error

   Purpose: Reset all values to the default values (from the OTP).
******************************************************************************/
extern uint8_t
TMC222_ResetToDefault(uint8_t address);


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
extern uint8_t
TMC222_RunInit      (uint8_t VMin,
                     uint8_t VMax,
                     int16_t Position1,
                     int16_t Position2,
                     uint8_t address);

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
extern uint8_t
TMC222_HardStop(uint8_t address);


/*****************************************************************************
   Function: SoftStop()
   Parameters: ---

   Return value: twi-error

   Purpose: Stop the motor using the normal decelaration.
******************************************************************************/
extern uint8_t
TMC222_SoftStop(uint8_t address);


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
extern uint8_t TMC222_GetMotorParameters(TMC222_Parameters_t *TMC222Parameters,uint8_t address);


/*****************************************************************************
   Function: TMC222_GetMotionStatus()
   Parameters e TMC222.

   Return value: motion_status

   Purpose: This function uses a GetFullStatus1 command
            After that it returns the motion status
******************************************************************************/
extern uint8_t TMC222_GetMotionStatus(TMC222_Status_t *TMC222Status,uint8_t address);

#endif //_TMC222_H
