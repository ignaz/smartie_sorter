/*
 * =====================================================================================
 *
 *       Filename:  msg.h
 *    Description:  This file contains all nescesary routines to handle the communication
 *    							via "USB" not low-level. It implements the message protokol.
 *
 *        Version:  1.0
 *        Created:  03.11.2010 16:59:12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ignaz Laepple (mn), ignaz.laepple@gmx.de
 *        Company:  FH-Regensburg
 *
 * =====================================================================================
 */
#ifndef _MSG_H
#define _MSG_H


#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "uart.h"
#include "TLC59116.h"


#define 	MSG_SIZE 32

/*
 * ===  MACRO  =========================================================================
 *         Name:  IS_WHITE_SPACE
 *  Description:
 * =====================================================================================
 */

#define IS_WHITE_SPACE(x) (x == 32)


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  MSG_Rx_Poll()
 *  Description:  This function checks if a new package is completely received
 *  							it calls MSG_Parse_Rx_Paket when the Rx complete (/n)
 * =====================================================================================
 */
extern void
MSG_Rx_Poll(void);

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  MSG_Parse_msg
 *  Description:
 * =====================================================================================
 */
extern void
MSG_Parse_msg (uint8_t ** ptr);


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  get_next_char
 *  Description:  returns next char excepting white space
 *        Input: 	pointer to "stream"
 *       return: 	character
 * =====================================================================================
 */
extern uint8_t
get_next_char (uint8_t ** ptr);

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  get_int
 *  Description:
 * =====================================================================================
 */
extern int16_t
get_int (uint8_t ** ptr);

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  get_uint
 *  Description:
 * =====================================================================================
 */
extern uint16_t
get_uint (uint8_t ** ptr);

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  get_parameter
 *  Description:
 * =====================================================================================
 */
extern void
get_parameter (uint8_t * ptr );

/* ===  FUNCTION  =====================================================================
 *         Name:  MSG_Parse_RX_Paket
 *  Description:  This function parses the received paket and calls ????????
 * =====================================================================================
 */
extern uint8_t
MSG_Parse_RX_Paket(void);

#endif // _MSG_H
