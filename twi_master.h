/*
 * =====================================================================================
 *
 *       Filename:  twi_master.h
 *
 *    Description:  Basic routines to communicate via twi (I2C)
 *    							This file is based on the AN315 form ATMEL
 *
 *        Version:  1.0
 *        Created:  04.09.2010 21:14:37
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ignaz Laepple (mn), ignaz.laepple@gmx.de
 *        Company:  FH-Regensburg
 *
 * =====================================================================================
 */


 /**
 @defgroup smarties_controller TWI library
 @code #include <twi_master.h> @endcode

 @brief Basic routines for communication as TWI-master

 This file contains a full featured driver for the TWI-unit in master mode.
 By now, only few devices are supported by this driver.

 This library is based on the ATMEL Apliction Note AN315

 @author Ignaz Laepple ignaz.laepple@gmx.de

*/

 /*@{*/

#ifndef _TWI_MASTER_H
#define _TWI_MASTER_H


#include <avr/interrupt.h>
#include <avr/io.h>

/*
 * ===  Defines  =======================================================================
 */

/**
 *  @name  Definitions for MCU Clock Frequency
 *  Adapt the MCU clock frequency in Hz to your target.
 */
#ifndef F_CPU
#define F_CPU                   12000000UL
#endif

/**
 *  @name  Definition for TWI-baudrate
 *  Adapt the TWI baudrate according to your needs.
 */
#ifndef TWI_BAUDRATE
#define TWI_BAUDRATE            100UL
#endif

#define TWI_BAUDRATE_CNT(baudRate,xtalCpu)     ((xtalCpu/(8*baudRate))-2)

/**
 *  @name  Definition for the TWI buffer size
 *  This buffer determines the length of the messages that can be received or transmitted.
 */
#define TWI_BUF_SIZE 			64

#define TWI_READ_BIT   1
#define TWI_WRITE_BIT  0

/**
 *  @name Definitions for states of the driver
 *  The constants define the various states in which the transceiver can be.
 */
#define TWI_STAT_RDY 			0   /**< Transceiver is idle and ready */
#define TWI_STAT_BSY 			1	/**< Transceiver is busy transceiving / "same" as TWIE */
#define TWI_STAT_TX_COMPLETE 	2	/**< Transmission done */
#define TWI_STAT_RX_COMPLETE 	3	/**< Receiption done */
#define TWI_STAT_ERROR 			8	/**< Error occoured */

// ERROR:
// Bit0 = 1 -> transmission error occoured in TWI-ISR
// Bit1 = 1 -> error occoured in stack-handling e.g: buf_ovf, rx-buf empty etc.

#define TWI_ERR_BUF_OVF 		2	// Message longer than buffer!!
#define TWI_ERR_NO_RX 			6   // No message received


/* -----  end of Defines  ----- */


// TODO :
// somthing like this!!
/*
typedef struct TWI_send_header_s{
    unsigned direction: 1;
    unsigned address:   7;
    unsigned reg:       8;
    unsigned data[];
}TWI_send_header_t;

typedef union {
    TWI_send_header_t send_header}
*/


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  TWI_Master_Init
 *  Description:  Initialises the TWI-unit
 *  							Sets Baudrate register,  enables TWI-unit,  clears twi_status / errors
 *  	  	Input: 	baudrate
 *  	  Returns: 	none
 * =====================================================================================
 */

 /**
 * TWI_Master_Init
 * This function initialises the TWI-master
 * it sets the baudrate-register, enables TWI-unit and clears twi_status / errors
 * @param baudrate a integer
 * @return nothing
 */
extern void
TWI_Master_Init (uint8_t baudrate);
/* -----  end of function TWI_Master_Init  ----- */


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  TWI_Master_Transceiver_Busy()
 *  Description:  Call this macro to test if the TWI_ISR is busy transmitting.
 *  			Input: 	none
 *  		Returns: 	TWCR & _BV(TWIE)
 * =====================================================================================
 */

 /**
 @brief    Checks whether transceiver is busy transmitting.
 @param    none
 @return   1 if transceiver is busy, 0 else
*/
#define TWI_Master_Transceiver_Busy() 	(TWCR & _BV(TWIE))


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  TWI_Master_Transceive_Message
 *  Description: 	Waits until TWI-Int is disabled and copies the message from the
 *  							pointed message to the twi_buf and starts a twi transmission
 *  	  	Input: 	pointer to the message,  message size
 *  	  Returns: 	error
 * =====================================================================================
 */
extern uint8_t
TWI_Master_Transceive_Message (uint8_t *message, uint8_t messagesize);

/* -----  end of function TWI_Master_Start_Transceiver  ----- */


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  TWI_Master_Start_Transceiver
 *  Description: 	Waits until TWI-Int is disabled and intialises a twi transmission
 *  							with last message (same buffer)
 *  			Input: 	none
 *  	  Returns: 	none
 * =====================================================================================
 */
extern void
TWI_Master_Start_Transceiver (void);

/* -----  end of function TWI_Master_Restart_Transceiver ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  TWI_Master_Get_State
 *  Description:  Waits until TWI-Int is disabled and returns the TWI_state
 *  			Input: 	none
 *  	  Returns: 	TWI_State
 * =====================================================================================
 */
extern uint8_t
TWI_Master_Get_State (void);

/* -----  end of function TWI_Master_Get_State  ----- */


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  TWI_Master_Get_Transceiver_Data
 *  Description:  Waits until TWI-Int is disabled and initiates a TWI-reception
 *  							Call this function after you sent a request of data to the Slave
 *  							The first byte of the message has to be the slave address!!
 *  			Input: 	*message,  message_size*
 *  	  Returns: 	none
 * =====================================================================================
 */
extern uint8_t
TWI_Master_Get_Transceiver_Data(uint8_t * message, uint8_t message_size);

/* -----  end of function TWI_Master_Get_Transceiver_Data ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  TWI_Master_Write_Register
 *  Description:  Waits until TWI-Int is disabled and sends two bytes (register and value)
 *                to the slave address
 *  	  Input:  message , address
 *      Returns:  none
 * =====================================================================================
 */
    extern uint8_t
TWI_Master_Write_Register(uint8_t reg, uint8_t value, uint8_t address);

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  TWI_Master_Write_Byte
 *  Description:  Waits until TWI-Int is disabled and sends one byte
 *                to the slave address
 *  	  Input:  message , address
 *  	Returns:  none
 * =====================================================================================
 */
    extern uint8_t
TWI_Master_Write_Byte(uint8_t byte, uint8_t address);

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  TWI_Master_Read_Byte
 *  Description:  Waits until TWI-Int is disabled and reads a single byte
 *                from the slave address
 *  	  Input:  address
 *  	Returns:  byte
 * =====================================================================================
 */
    extern uint8_t
TWI_Master_Read_Byte(uint8_t address);

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
    extern uint8_t
TWI_Master_Read_Register(uint8_t reg,uint8_t address);


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

extern ISR(TWI_vect);


/*@}*/

#endif // TWI_MASTER_H
