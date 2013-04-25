/*
 * =====================================================================================
 *
 *       Filename:  twi.h
 *
 *    Description:  Headerfile for the twislave.c
 *
 *        Version:  1.0
 *        Created:  29.07.2010 18:52:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ignaz Laepple (mn), ignaz.laepple@gmx.de
 *        Company:  FH-Regensburg
 *
 * =====================================================================================
 */

#ifndef _TWISLAVE_H
#define _TWISLAVE_H

/*
 * defines
 */

#define LCD_BUFFER_SIZE 	32
#define TWI_BUFFER_SIZE     (4+(2*sizeof(lcd_block_t)))

#define BUF_0_ADR 			(4)
#define BUF_1_ADR 			(BUF_0_ADR + sizeof(lcd_block_t))
#define BUF_0_EMPTY 		0
#define BUF_0_WR 			1
#define BUF_1_EMPTY 		2
#define BUF_1_WR 			3
//#define BUF_BSY 			4

#define PORT_X_EVENT 		6
#define ROT_ENC_EVENT 	    7

/*
 * typedefs:
 */

typedef struct
{
    uint8_t 	command;
    uint8_t 	data[LCD_BUFFER_SIZE];
} lcd_block_t;

typedef union
{
    struct
    {
        uint8_t  	status;
        uint8_t  	port_x;
        uint16_t 	rot_enc_pos;
        lcd_block_t lcd[2];
    } reg;
    uint8_t buffer[TWI_BUFFER_SIZE];
} twi_t;


/*
 * globals
 */

extern volatile twi_t twi;
extern volatile uint8_t twi_buffer_adr;

/*
 * function prototypes
 */


extern void Twi_Slave_Init(uint8_t slave_address);

#endif //#ifdef _TWISLAVE_H
