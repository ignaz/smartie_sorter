#ifndef _DEBUG_H
#define _DEBUG_H

#include "uart.h"

#define debug 0



#ifndef CS_DEBUG
#define CS_DEBUG 0      //Debugschalter für das Colorsensormodul (color_sensor.c)
#endif

#ifndef TWI_DEBUG
#define TWI_DEBUG 0     //Debugschalter für das TWI / I²C-Modul (twi_master.c)
#endif

#ifndef SM_DEBUG
#define SM_DEBUG 0      //Debugschalter für das Smartiesmodul /Farberkennung etc. (smarties.c)
#endif

#ifndef FSM_DEBUG
#define FSM_DEBUG 0     //Debugschalter für die state machine (fsm.c)
#endif


#endif // _DEBUG_H
