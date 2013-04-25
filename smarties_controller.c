/*
 * =====================================================================================
 *
 *       Filename:  smarties_controller.c
 *
 *    Description:  main file for the statemachine controoling the smarties-sorter via twi
 *    							All globals and functions of this entity start with SC_
 *
 *        Version:  1.0
 *        Created:  29.08.2010 19:49:30
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ignaz Laepple (mn), ignaz.laepple@gmx.de
 *        Company:  FH-Regensburg
 *
 * =====================================================================================
 */

/**
@maipage Smarties-Controller
@defgroup smarties_sorter Smarties-Sorter

@brief Main file of the Smarties-Controller

This file is the main file of the Smarties-Controller.
After inintialising the Smarties-Sorter, the main function just calls the execution functions of the three statmachines.
Statemachine one controlls the whole sorting process including initialising and error handling.
Statemachine two handles the menue for the communcation with the user
Statemachine three is just a small statemachine to controll the timing of the smartiessilo

@author Ignaz Laepple ignaz.laepple@gmx.de

*/

/*@{*/


#include <stdlib.h>
#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/pgmspace.h>
#include <util/twi.h>

#include "uart.h"
#include "twi_master.h"
#include "twi_lcd.h"

#include "TMC222.h"
#include "TLC59116.h"
#include "ADJD_S311.h"
#include "debug.h"
#include "msg.h"

#include "colour_sensor.h"
#include "motion_controll.h"
#include "fsm.h"

/**
/* define CPU frequency in Mhz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 			    12000000UL
#endif

/* UART baud rate */
#define UART_BAUD_RATE      9600

/* TWI baud rate */
#define TWI_BAUDRATE 	    100UL


/*
 * === Prototypes  =====================================================================
 */

void SC_Init(void);

/*
 * === MAIN ============================================================================
 * =====================================================================================
 */
int
main ( void )
{
    static uint16_t c16,cs_cnt=0;
    uint8_t buffer[16];

    static TMC222_Status_t Sorter_Status,Revolver_Status;
    static TMC222_Parameters_t Sorter_Parameter, Revolver_Parameter;

    static ADJD_S311_Data_t cs_data;
    static ADJD_S311_Offset_t cs_offset;
    static ADJD_S311_Param_t cs_parameter;

    static CS_Sensor_LED_t pwm_values;

    sei();
    SC_Init();

    uart_puts_P("\n\rSmarties-Sorter V3\n\r");

    TWI_Master_Init(50);//(uint8_t)TWI_BAUDRATE_CNT(TWI_BAUDRATE,F_CPU));

    uart_puts_P("\nHallo Welt");

    MC_Timer0_Init();

    TLC59116_Init();


    while(1)
    {

        //TODO: the following lines of code should be sepperated in an extra modul

        MC_FSM_Execute();

        FSM_Check_State();

        uint8_t command = ((uint8_t)uart_getc());

        switch(command)
        {
        case 'Y':
            SM_Colours_Restore();
            break;
        case 'A':
            CS_Gain_Addapt(&cs_parameter,31);
            break;
        case 'W':
            CS_LED_Addapt(&pwm_values,600);
            break;
        case 'D':
            uart_puts_P("\n\rData:\n");
            CS_Colour_Average_Get(&cs_data);

            uart_put_bin8((uint8_t) (cs_data.Red>>8));
            uart_put_bin8((uint8_t) cs_data.Red);
            uart_putc(' ');
            uart_put_uint16(cs_data.Red);
            uart_putc('\n');
            uart_put_bin8((uint8_t) (cs_data.Green>>8));
            uart_put_bin8((uint8_t) cs_data.Green);
            uart_putc(' ');
            uart_put_uint16(cs_data.Green);
            uart_putc('\n');
            uart_put_bin8((uint8_t) (cs_data.Blue>>8));
            uart_put_bin8((uint8_t) cs_data.Blue);
            uart_putc(' ');
            uart_put_uint16(cs_data.Blue);
            uart_putc('\n');
            uart_put_bin8((uint8_t)(cs_data.Clear>>8));
            uart_put_bin8((uint8_t) cs_data.Clear);
            uart_putc(' ');
            uart_put_uint16(cs_data.Clear);
            break;

        case 'd':
            uart_put_uint16(cs_cnt++);
            uart_putc(';');
            CS_Colour_Average_Get(&cs_data);
            uart_put_uint16(cs_data.Red);
            uart_putc(';');
            uart_put_uint16(cs_data.Green);
            uart_putc(';');
            uart_put_uint16(cs_data.Blue);
            uart_putc(';');
            uart_put_uint16(cs_data.Clear);
            uart_putc('\n');
            break;


        case 'O':
            uart_puts_P("\n\roffset:\n");
            ADJD_S311_Offset_Get(&cs_offset);

            uart_put_bin8((uint8_t) cs_offset.Red);
            uart_putc('\n');
            uart_put_bin8((uint8_t) cs_offset.Green);
            uart_putc('\n');
            uart_put_bin8((uint8_t) cs_offset.Blue);
            uart_putc('\n');
            uart_put_bin8((uint8_t) cs_offset.Clear);

            break;
        case 'i':
            ADJD_S311_Reg_Set(ADJD_S311_REG_CONFIG,(_BV(ADJD_S311_BIT_TOFS)));
            break;
        case 'o':
            ADJD_S311_Reg_Set(ADJD_S311_REG_CONFIG,0);
            break;
        case 'c':
            ADJD_S311_Offset_Clear();
            break;
        case 'l':
            TLC59116_GRP_PWM_Set(0xFF);
            break;
        case 'L':
            TLC59116_GRP_PWM_Set(0);
            break;

        case '+':
            MC_Conveyor_Set_Position(2);
            break;
        case '-':
            MC_Conveyor_Set_Position(-1);
            break;
        case 'Z':
            TMC222_SetPosition(0,0x61);
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
            MC_Catcher_Set_Position((uint8_t)(command-'0'));
            break;
        case 'T':
            TMC222_SetMotorParameters(&Revolver_Parameter,0);
            break;
        case 'K':
            uart_putc('\n');
            uart_put_bin8(TWI_Master_Read_Byte(0x20));
            break;
        case 'R':
            MC_Catcher_Init();
            break;
        case 'S':
            MC_Solenoid_On();
            break;
        case 's':
            MC_Solenoid_Off();
            break;
        case 'V':
            MC_Vibrator_On();
            break;
        case 'v':
            MC_Vibrator_Off();
            break;
        case 'M':
            //TMC222_ResetPosition(0x61);
            uart_put_bin8(TMC222_GetMotionStatus(&Sorter_Status,0x61));
            break;
        case 'C':
            MC_Conveyor_Init();
            break;
        case 'e':
            MC_Eject_Smartie();
            break;
        case 'h':
            uart_putc('0'+ SM_Colour_Attach(&cs_data));
            break;
        case 'p':
            fsm_pause ^= 0x01;
            break;
        case 'n':
            cur_mode = md_running;
            break;
        case 'm':
            cur_mode = md_learning;
            break;
        case 'b':
            cur_mode = md_init;
            break;
        }
    }
    return 0;
}		/* -----  end of function main  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  SC_Init
 *         Description:
 * =====================================================================================
 */
void
SC_Init ( void )
{
    // init IO-ports
    // PORTB
    PORTB |= _BV(PB4);

    // PORTD
    DDRD  &= ~ _BV(PD3);    // Set INT_TWI as input
    PORTD |=   _BV(PD3);    // Enable pullup on INT_TWI signal

    // init modules
    // UART
    uart_init(0x8019);      // UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU ));

    // TWI
    TWI_Master_Init(50);    //(uint8_t)TWI_BAUDRATE_CNT(TWI_BAUDRATE,F_CPU));

    // LCD

}
/* -----  end of function SC_Init  ----- */


/*@}*/
