/*
 * =====================================================================================
 *
 *       Filename:  msg.c
 *
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


#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "uart.h"
#include "TMC222.h"
#include "TLC59116.h"
#include "ADJD_S311.h"
#include "msg.h"

static volatile uint8_t msg_buf[MSG_SIZE];
static volatile uint8_t msg_ptr;
static volatile uint8_t * rd_ptr;

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  MSG_Rx_Poll()
 *  Description:  This function checks if a new package is completely received
 *  							it calls MSG_Parse_Rx_Paket when the Rx complete (/n)
 * =====================================================================================
 */
void
MSG_Rx_Poll(void)
{
    uint16_t c16;
    uint8_t c8;

    if((c16=uart_getc())==UART_NO_DATA);
    //uart_put_wait();
    else
    {
        c8 = (uint8_t) c16;
        uart_putc(c8); 							// just echo Byte

        if (msg_ptr < MSG_SIZE) 		// check message size
        {
            switch (c8)
            {
            case '\n':
            case '\r':
                msg_buf[msg_ptr++] = c8;
                *rd_ptr =(uint8_t *) msg_buf;
                MSG_Parse_msg(rd_ptr);
                msg_ptr = 0;
                break;
            case '%':
                msg_ptr = 0;
                msg_buf[msg_ptr++] = c8;
                break;
            default:
                msg_buf[msg_ptr++] = c8;
                //	break;
            }
        }
        else
        {
            uart_puts_P("\nMessage too long!!\n");
        }
    }
}		/* -----  end of function MSG_Rx_Poll()  ----- */



/*
 * ===  FUNCTION  ======================================================================
 *         Name:  MSG_Parse_msg
 *  Description:
 * =====================================================================================
 */

void
MSG_Parse_msg (uint8_t ** ptr)
{
    uint16_t 	ram_start, ram_length=1;
    uint8_t 	tmp_buff[8],ui1,ui2,ui8_i;
    uint8_t     twi_addresse;
    TMC222_Status_t TMC222Status;
    ADJD_S311_Data_t cs_data;

    if(get_next_char(ptr)=='%')
    {
        while (get_next_char(ptr) == '-')
        {
            switch (get_next_char(ptr))
            {
            case 'p':
                uart_puts_P("\nParameter ");
                switch (get_next_char(ptr))
                {
                case 'a':
                    uart_puts_P("a = ");
                    uart_putc((uint8_t)get_uint(ptr));
                    break;
                case 'b':
                    uart_puts_P("b = ");
                    uart_putc((uint8_t)get_uint(ptr));
                    break;
                default:
                    uart_puts_P("unknown");
                }
                break;
            case 'c':
                uart_puts_P("\nCommand ");
                switch (get_next_char(ptr))
                {
                case 'a':
                    uart_puts_P("a = ");
                    uart_putc((uint8_t)get_uint(ptr));
                    break;
                case 'b':
                    uart_puts_P("b = ");
                    uart_putc((uint8_t)get_uint(ptr));
                    break;
                default:
                    uart_puts_P("unknown");
                }
                break;
            case 'r':
                ram_start = get_uint(ptr);
                if (get_next_char(ptr)==';')
                    ram_length = get_uint(ptr);
                else ram_length = 8;
                while (ram_length--)
                    uart_putc(*(uint8_t *)(ram_start++));
                break;


            case 't':
                twi_addresse=(uint8_t)get_uint(ptr);
                uart_puts_P("\nTWI command @ ");
                uart_putc(twi_addresse);

                switch (get_next_char(ptr))
                {
                case 'i':
                    uart_puts_P(" GetFullStatus:");
                    //TMC222_GetFullStatus1(&TMC222Status,twi_addresse);
                    break;
                case 'p':
                    uart_puts_P(" Set Position:");
                    int16_t position = get_int(ptr);
                    //TMC222_SetPosition(position,twi_addresse);
                    break;
                default:
                    uart_puts_P("unknown");
                }
                break;
            case 'T':
                uart_puts_P("\n\r LED set Colour:");
                ui1 = (uint8_t)get_uint(ptr);
                if(get_next_char(ptr)==';')
                    ui2 = (uint8_t)get_uint(ptr);
                TLC59116_Set_PWM_Channel(ui1,ui2);
                break;

            case 'C':
                ADJD_S311_Data_Get(&cs_data);
                break;

            default:
                uart_puts_P("\nempty message! ");
            }
        }
    }
}
/* -----  end of function MSG_Parse_msg  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  get_next_char
 *  Description:  returns next char excepting white space
 *        Input: 	pointer to "stream"
 *       return: 	character
 * =====================================================================================
 */
uint8_t
get_next_char (uint8_t ** ptr)
{
    uint8_t * rd_ptr_tmp= *ptr;
    uint8_t c8;
    while (((c8 = *(rd_ptr_tmp++)) == ' ') || (c8 =='\t'));
    *ptr = rd_ptr_tmp;
    return c8;
}		/* -----  end of function get_next_char  ----- */


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  get_int
 *  Description:
 * =====================================================================================
 */
int16_t
get_int (uint8_t ** ptr)
{
    uint8_t c8;
    int16_t i16=0;
    while(((c8=get_next_char(ptr)) >= '0') && (c8 <='9'))
    {
        i16 = (i16*10)+(c8 - '0');
    }
    (*ptr) -=1;
    return i16;
}		/* -----  end of function get_int  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  get_uint
 *  Description:
 * =====================================================================================
 */
uint16_t
get_uint (uint8_t ** ptr)
{
    uint8_t c8;
    uint16_t ui16=0;
    while(((c8=get_next_char(ptr)) >= '0') && (c8 <='9'))
    {
        ui16 = (ui16*10)+(c8 -'0');
    }
    (*ptr)--;

    return ui16;
}		/* -----  end of function get_uint  ----- */



/*
 * ===  FUNCTION  ======================================================================
 *         Name:  get_parameter
 *  Description:
 * =====================================================================================
 */
void
get_parameter (uint8_t * ptr );
/* -----  end of function get_parameter  ----- */
