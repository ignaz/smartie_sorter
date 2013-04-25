/*
 * =====================================================================================
 *
 *       Filename:  twi_master.c
 *
 *    Description:  Basic routines to communicate via twi (I2C)
 *    							       This file is based on the AN315 from ATMEL
 *
 *        Version:  1.0
 *        Created:  12.08.2010 14:13:43
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ignaz Laepple (mn), ignaz.laepple@gmx.de
 *        Company:  FH-Regensburg
 *
 * =====================================================================================
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
//#include "config.h"
#include "twi_master.h"
#include <util/twi.h>
#include "uart.h"
#include "debug.h"


#if twi_debug
//static uint8_t temp_ui8_reg=0;
#endif


//Variables needed by this library to handle the TWI communication
static volatile uint8_t twi_buf[TWI_BUF_SIZE];
static volatile uint8_t twi_buf_ptr;
static volatile uint8_t twi_buf_cnt;
static volatile uint8_t twi_err;
static volatile uint8_t twi_status;



/*
 * ===  FUNCTION  ======================================================================
 *         Name:  TWI_Master_Init
 *  Description:  Initialises the TWI-unit
 *  							Sets Baudrate register,  enables TWI-unit,  clears twi_status / errors
 *  	  	Input: 	none
 *  	  Returns: 	none
 * =====================================================================================
 */
void
TWI_Master_Init (uint8_t twi_baudrate_reg)
{

    TWBR = (twi_baudrate_reg); 	  	// TWI bit rate:
    TWCR = _BV(TWINT);              // clear interrupt flag!!
    TWCR = _BV(TWEN); 			    // switch on TWI
    twi_status = TWI_STAT_RDY;      // set twi status
    twi_err = 0;                    // clear twi error
#if twi_debug
    uart_puts_P("\n\n\rTWI_Init...\n\rTWBR: ");
    uart_put_bin8(TWBR);
    uart_puts_P("\n\rTWCR: ");
    uart_put_bin8(TWCR);
#endif
}		/* -----  end of function TWI_Master_Init  ----- */


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  TWI_Master_Transceive_Message
 *  Description: 	Waits until TWI-Int is disabled and copies the message from the
 *  				pointed message to the twi_buf and starts a twi transmission
 *  	  	Input: 	pointer to the message,  message size
 *  	  Returns: 	error
 * =====================================================================================
 */
uint8_t
TWI_Master_Transceive_Message (volatile uint8_t *message, uint8_t messagesize)
{

    uint8_t i;

#if TWI_DEBUG
    uart_puts_P("\n\n\rTWI_Tx_Msg! TWCR: \n\r");
    do
    {
        uart_putc('\r');
        uart_put_bin8(TWCR);
        uart_putc('\t');
        uart_put_wait();
    }
    while (TWI_Master_Transceiver_Busy());		// Wait until previous transmision is done

#else
    while (TWI_Master_Transceiver_Busy());		// Wait until previous transmision is done
#endif


    if (messagesize <= TWI_BUF_SIZE ) 				// check messagesize
    {
        if(*(uint8_t *)message & TWI_READ_BIT)
        {
            twi_buf[0] = *(uint8_t *)message;
#if TWI_DEBUG
            uart_puts_P("\n\r\ttwi_buf[0]: ");
            uart_put_bin8(twi_buf[0]);
#endif
        }
        else
        {
            for(i=0; i<messagesize; i++)
            {
                twi_buf[i] = *(((uint8_t *) message)+i);
#if TWI_DEBUG
                uart_puts_P("\n\r\ttwi_buf[");
                uart_putc('0'+i);
                uart_puts_P("]: ");
                uart_put_bin8(twi_buf[i]);
#endif
            }
        }
        twi_buf_cnt = messagesize;
        TWI_Master_Start_Transceiver();
        return 0;
    }
    else
    {
        twi_err = TWI_ERR_BUF_OVF;
        twi_status = TWI_STAT_ERROR;
#ifdef TWI_DEBUG
        uart_puts_P("\n\rtwi_err:");
        uart_put_bin8(twi_err);
#endif
        return twi_err;
    }
}		/* -----  end of function TWI_Master_Start_Transceiver  ----- */


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  TWI_Master_Start_Transceiver
 *  Description: 	Waits until TWI-Int is disabled and intialises a twi transmission
 *  							with last message (same buffer)
 *  			Input: 	none
 *  	  Returns: 	none
 * =====================================================================================
 */
void
TWI_Master_Start_Transceiver (void)
{

#if TWI_DEBUG
    uart_puts_P("\n\n\rTWI_Tx_Start!\ntwi_err:\n\r");
    do
    {
        uart_putc('\r');
        uart_put_bin8(twi_err);
        uart_putc('\t');
        uart_put_wait();
    }
    while (TWI_Master_Transceiver_Busy());		// Wait until previous transmision is done

#else
    while (TWI_Master_Transceiver_Busy());		// Wait until previous transmision is done
#endif

    twi_status = TWI_STAT_BSY; 					// set twi status (no use : done by TWIE)
    twi_err = 0; 								// clear twi error

    TWCR |= ( _BV(TWEA) | _BV(TWSTA) | _BV(TWIE)); 	// Initialise transmission via
    // TWI-startcondition,
    // enable TWI interrupt and
    // TWCR |= _BV(TWINT);                          // finally clear TWI interrupt flag

#if TWI_DEBUG
    uart_puts_P("\n\rTWCR: ");
    uart_put_bin8(TWCR);
#endif

}		/* -----  end of function TWI_Master_Start_Transceiver ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  TWI_Master_Get_State
 *  Description:  Waits until TWI-Int is disabled and returns the TWI_state
 *  			Input: 	none
 *  	  Returns: 	TWI_State
 * =====================================================================================
 */
uint8_t
TWI_Master_Get_State (void)
{
#if TWI_DEBUG
    uart_puts_P("\n\n\rTWI_Get_Status! twi_status:\n\r");
    do
    {
        uart_putc('\r');
        uart_put_bin8(twi_status);
        uart_putc('\t');
        uart_put_wait();
    }
    while(TWI_Master_Transceiver_Busy());
    return twi_status;
#else
    while(TWI_Master_Transceiver_Busy());
    return twi_status;
#endif

}		/* -----  end of function TWI_Master_Get_State  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  TWI_Master_Get_Error(void)
 *  Description:
 * =====================================================================================
 */
uint8_t
TWI_Master_Get_Error(void )
{
#if TWI_DEBUG
    uart_puts_P("\n\n\rTWI_Get_Error! twi_error:\n\r");
    do
    {
        uart_putc('\r');
        uart_put_bin8(twi_err);
        uart_putc('\t');
        uart_put_wait();
    }
    while(TWI_Master_Transceiver_Busy());
    return twi_err;
#else
    while(TWI_Master_Transceiver_Busy());
    return twi_err;
#endif
}		/* -----  end of function TWI_Master_Get_Error(void)  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  TWI_Master_Get_Transceiver_Data
 *  Description:  Waits until TWI-Int is disabled and initiates a TWI-reception
 *  							Call this function after you sent a request of data to the Slave
 *  							The first byte of the message has to be the slave address!!
 *  			Input: 	*message,  message_size
 *  	  Returns: 	none
 * =====================================================================================
 */
uint8_t
TWI_Master_Get_Transceiver_Data(volatile uint8_t * message, uint8_t message_size)
{
    uint8_t i;
    uint8_t * p_message = message;

#if TWI_DEBUG
    uart_puts_P("\n\n\rTWI_Get_Tx_Data:  ");
    while(TWI_Master_Transceiver_Busy())
        uart_put_wait();
#else
    while(TWI_Master_Transceiver_Busy());
#endif

    switch (twi_status)
    {
    case TWI_STAT_RX_COMPLETE:
#if TWI_DEBUG
        uart_puts_P(" RX complete");
#endif
        for(i=0; i<message_size; i++)
        {
            p_message[i]=twi_buf[i+1];
        }
        twi_status = TWI_STAT_RDY;
        break;
    case TWI_STAT_TX_COMPLETE:
#if TWI_DEBUG
        uart_puts_P(" no RX");
#endif
        twi_err = TWI_ERR_NO_RX;
    default :
        break;
    }
    return twi_err;

}		/* -----  end of function TWI_Master_Get_Transceiver_Data ----- */




/*
 * ===  FUNCTION  ======================================================================
 *         Name:  TWI_Master_Write_Register
 *  Description:  Waits until TWI-Int is disabled and sends two bytes (register and value)
 *                to the slave address
 *  	  Input:  message , address
 *      Returns:  none
 * =====================================================================================
 */
uint8_t
TWI_Master_Write_Register(uint8_t reg, uint8_t value, uint8_t address)
{
    while (TWI_Master_Transceiver_Busy());	// Wait until previous transmision is done
    twi_buf[0] = (address <<1 ) | TW_WRITE;
    twi_buf[1] = reg;
    twi_buf[2] = value;
    twi_buf_cnt = 3;
    TWI_Master_Start_Transceiver();
    while (TWI_Master_Transceiver_Busy()); // Wait until previous transmision is done
    return 0;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  TWI_Master_Write_Byte
 *  Description:  Waits until TWI-Int is disabled and sends one byte
 *                to the slave address
 *  	  Input:  message , address
 *  	Returns:  none
 * =====================================================================================
 */
uint8_t
TWI_Master_Write_Byte(uint8_t byte, uint8_t address)
{
    while (TWI_Master_Transceiver_Busy());	// Wait until previous transmision is done
    twi_buf[0] = (address <<1 ) | TW_WRITE;
    twi_buf[1] = byte;
    twi_buf_cnt = 2;
    TWI_Master_Start_Transceiver();
    while (TWI_Master_Transceiver_Busy());  // Wait until transmision is done
    return 0;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  TWI_Master_Read_Byte
 *  Description:  Waits until TWI-Int is disabled and reads a single byte
 *                from the slave address
 *  	  Input:  address
 *  	Returns:  byte
 * =====================================================================================
 */
uint8_t
TWI_Master_Read_Byte(uint8_t address)
{
    while (TWI_Master_Transceiver_Busy());	// Wait until previous transmision is done
    twi_buf[0] = (address <<1 ) | TW_READ;
    twi_buf_cnt = 1;
    TWI_Master_Start_Transceiver();
    while (TWI_Master_Transceiver_Busy());  // Wait until transmision is done
    return (twi_buf[1]);
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  TWI_Master_Read_Register
 *  Description:  Waits until TWI-Int is disabled and addresses a given register.
 *                After transmision it reads the addressed reigster from the slave
 *                address.
 *  	  Input:  address
 *  	Returns:  byte
 * =====================================================================================
 */
uint8_t
TWI_Master_Read_Register(uint8_t reg,uint8_t address)
{
    TWI_Master_Write_Byte(reg,address);
    return (TWI_Master_Read_Byte(address));
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:   ISR(TWI_vect)
 *  Description: 	This function is the Interrupt Service Routine (ISR), and
 *  							automatically called when the TWI interrupt is triggered;
 *  							that is whenever a TWI event has occurred.
 *  							This function should not be called directly from the main application.
 *  			Input: 	none
 *  		Returns: 	none
 * =====================================================================================
 */

ISR(TWI_vect)
{
#if TWI_DEBUG
    //uart_puts_P("\n\rTWI ISR! TW_STATUS: ");
    //uart_put_bin8(TW_STATUS);
#endif
    switch (TW_STATUS)
    {
    case TW_START:                      // Start condition or
    case TW_REP_START:                  // repeated start condition transmitted
        // => we are becoming bus master!
        twi_buf_ptr = 0;				// Reset buffer position

    case TW_MT_SLA_ACK: 				// Slave addresse ack´ed
    case TW_MT_DATA_ACK:                // Slave data receive acked

        if (twi_buf_ptr >= twi_buf_cnt) 	// Buffer pointer at the end???
        {
            twi_status = TWI_STAT_TX_COMPLETE;      // set status
            twi_err = 0; 							// no error
            // clear TWINT, initiate stop condition disable TW-interrupt
            TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
        }
        else
        {
            TWDR = twi_buf[twi_buf_ptr++]; 	// Copy Data from current buffer position to data rgister
            // Post increment pointer
            // clear TWINT
            TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWIE);
        }
        break; 								// Leave state machine

    case TW_MR_DATA_ACK: 					// Data has been received and ACK transmitted
        twi_buf[twi_buf_ptr++]=TWDR; 		// Copy from data register to current buffer position

    case TW_MR_SLA_ACK: 					// SLA+R has been transmitted and Ack received
        if(twi_buf_ptr >= twi_buf_cnt)	    // Buffer pointer at byte befor the end??
        {
            // clear TWINT and don´t initiate ACK after next reception
            TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWIE);
        }
        else
        {
            // clear TWINT, initiate ACK after next reception
            TWCR = _BV(TWINT) | _BV(TWEA) | _BV(TWEN) | _BV(TWIE);
        }
        break; 								// Leave state machine

    case TW_MR_DATA_NACK: 					// Data byte has been received & NACK transmitted
        twi_buf[twi_buf_ptr] = TWDR; 		// Copy data from data register to current buffer position
        twi_status = TWI_STAT_RX_COMPLETE; 	// Set status
        twi_err = 0; 						// clear error
        // clear TWINT, initiate a stop condition and disable TW-interrupt
        TWCR = _BV(TWINT) | _BV(TWSTO)| _BV(TWEN);
        break;                              // Leave state machine

// posible errors
    case TW_MR_ARB_LOST:                    // Arbitrsation lost
        twi_err = TW_STATUS;
        // clear TWINT, initiate restart condition
        TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN) | _BV(TWIE);
        break;                              // Leave state machine

    case TW_MT_SLA_NACK:                // Slave does not ack (answer)
    case TW_MR_SLA_NACK:                // Slave does not ack (answer)
    case TW_MT_DATA_NACK:               // Data byte has been transmitted and NACK has been received
    case TW_BUS_ERROR:                  // general
    default:
        twi_status = TWI_STAT_ERROR;    // set twi status to error!!
        twi_err = (TW_STATUS + 1);      // write TWI_Staus and LSB to the twi error register
        // LSB is added because TW_BUS_ERROR = 0; (no usefull error code)
        // clear TWINT, generate stop condition and disable TW interrupt
        TWCR = (_BV(TWINT)|_BV(TWSTO));     // initialise a stop condition, clear interrupt flag
    }
}

