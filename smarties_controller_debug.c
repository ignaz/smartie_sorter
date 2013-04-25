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

#include <stdlib.h>
#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/pgmspace.h>
#include <util/twi.h>

#include "uart.h"
#include "twi_master.h"
#include "TMC222.h"
#include "TLC59116.h"
#include "ADJD_S311.h"
#include "debug.h"
#include "msg.h"

#include "color_sensor.h"
#include "motion_controll.h"


/* define CPU frequency in Mhz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 						12000000UL
#endif

/* UART baud rate */
#define UART_BAUD_RATE    9600

/* TWI baud rate */
#define TWI_BAUDRATE 			100UL


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
	uint8_t c8,ui8_i;
	uint8_t buffer[16];
    static TMC222_Status_t Sorter_Status,Revolver_Status;
    static TMC222_Parameters_t Sorter_Parameter, Revolver_Parameter;
    int16_t Revolver_Position = 0;

    static ADJD_S311_Data_t cs_data;
    static ADJD_S311_Offset_t cs_offset;
    static ADJD_S311_Param_t cs_parameter;

    static CS_Sensor_LED_t pwm_values;

    cs_parameter.CapRed     = 0xF;
    cs_parameter.CapGreen   = 0xF;
    cs_parameter.CapBlue    = 0xF;
    cs_parameter.CapClear   = 0xF;
    cs_parameter.IntRed     = 0x03FF;
    cs_parameter.IntGreen   = 0x03FF;
    cs_parameter.IntBlue    = 0x03FF;
    cs_parameter.IntClear   = 0x03FF;

    Revolver_Parameter.IHold = 8;
    Revolver_Parameter.IRun  = 15;
    Revolver_Parameter.VMin  =1;
    Revolver_Parameter.VMax  =3;
    Revolver_Parameter.Acc   =2;
    Revolver_Parameter.AccShape = 0;
    Revolver_Parameter.StepMode = 3;



    sei();
	SC_Init();


	uart_puts_P("\n\rSmarties-Sorter V3\n\r");

    //uart_put_bin8(TWCR);

	TWI_Master_Init(TWI_BAUDRATE_CNT(TWI_BAUDRATE,F_CPU));

	TLC59116_Init();


	ADJD_S311_Param_Set((ADJD_S311_Param_t *)&cs_parameter);
    ADJD_S311_Data_Get(&cs_data);

    //TMC222_GetFullStatus1(&Revolver_Status,0);
    //TMC222_SetMotorParameters(&Revolver_Parameter,0);

/*
    //ADJD_S311_Data_Get(&cs_data);
    buffer[0] = (ADJD_S311_ADDRESS<<1) | TW_WRITE;
    buffer[1] = ADJD_S311_REG_PARAM;

    for (ui8_i=0;ui8_i<(sizeof(ADJD_S311_Param_t));ui8_i++)
        buffer[ui8_i+2] = 0xC0;//(*(((uint8_t*)SensorParam)+ui8_i));

    TWI_Master_Transceive_Message(buffer,(sizeof(ADJD_S311_Param_t)+2));
*/
	while(1)
	{
	    uint8_t command = ((uint8_t)uart_getc());
	    switch(command)
	    {
	        case 'A':

                CS_Gain_Addapt(&cs_parameter,31);
                break;
            case 'W':
                CS_LED_Addapt(&pwm_values,600);
                break;

            case 'D':
                uart_puts_P("\n\rData:\n");
                CS_Color_Average_Get(&cs_data);

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
                CS_Color_Average_Get(&cs_data);
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
            case 'p':
                break;
            case 'P':
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
	    }
	}
	return 0;
}		/* -----  end of function main  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  SC_Init
 *  Description:
 * =====================================================================================
 */
	void
SC_Init ( void )
{
	// init IO-ports
		// PORTB
		PORTB |= _BV(PB4);

		// PORTD
		DDRD  &= ~ _BV(PD3); // Set INT_TWI as input
		PORTD |=   _BV(PD3); // Enable pullup on INT_TWI signal


	// Init timer

	// init modules
		// UART
		uart_init(0x8019);    // UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU ));

		// TWI
		//TWI_Master_Init(TWI_BAUDRATE_CNT(TWI_BAUDRATE,F_CPU));

		// LCD

}
	/* -----  end of function SC_Init  ----- */
