/*
 * =====================================================================================
 *
 *       Filename:  fsm.c
 *    Description:  fninite state machine that controlls the sorting automat
 *
 *        Version:  1.0
 *        Created:  06.12.2010 19:09:32
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
#include <util/delay.h>

#include "uart.h"
#include "twi_master.h"
#include "TMC222.h"
#include "smarties.h"
#include "debug.h"
#include "motion_controll.h"
#include "fsm.h"

#include "twi_lcd.h"
#include "twi_mmi.h"

#include "menue_text.h"


//#define FSM_DEBUG 1 //defined @ debug.h

volatile uint8_t fsm_pause=1;

enum fsm_mode cur_mode = md_init;

enum fsm_state { st_reset,
                 st_init_catcher,   //blocking
                 st_init_conveyor,   //blocking
                 st_move_conveyor_wht,
                 st_init_cs,         //blocking

                 st_init_done,
                 st_enter_md_running,
                 st_enter_md_learning,
                 st_enter_md_pause,

                 st_leave_md_learning,

                 st_eject_smartie,
                 st_move_catcher,
                 st_get_colour,
                 st_attach_colour,

                 st_learn_colour,

                 st_await_new_smartie,
                 st_move_conveyor,
               };
enum fsm_state cur_state = st_reset, next_state = st_reset;

/****** FSM conditions ***************************************************
*************************************************************************/
static uint8_t cond_true(void)
{
    return 1;
}
static uint8_t cond_else(void)
{
    return 1;
}
static uint8_t cond_conveyor_idle(void)
{
    return MC_Is_Conveyor_Idle();
}
static uint8_t cond_catcher_idle(void)
{
    return MC_Is_Catcher_Idle();
}
static uint8_t cond_md_init(void)
{
    return (cur_mode == md_init)    ? 1 : 0 ;
}
static uint8_t cond_md_run(void)
{
    return (cur_mode == md_running) ? 1 : 0 ;
}
static uint8_t cond_md_learn(void)
{
    return (cur_mode == md_learning)? 1 : 0 ;
}
static uint8_t cond_md_not_learn(void)
{
    return (cur_mode != md_learning)? 1 : 0 ;
}
static uint8_t cond_md_pause(void)
{
    return (cur_mode == md_pause)   ? 1 : 0 ;
}

static uint8_t cond_all_done(void)
{
    if(!(MC_Is_Smartie_Ejected()))return 0;
    if(!(MC_Is_Catcher_Idle()))return 0;
    if(fsm_pause) return 0;
    // ??? if(!(SM_Is_Colour_Attached))
    return 1;
}


/**** FSM state table *****************************************************
**************************************************************************/

struct fsm_s
{
    enum fsm_state  cur_state;
    enum fsm_state  next_state;
    uint8_t         (*condition)(void);
};

const struct fsm_s PROGMEM fsm_table[]  =
{
    // actual state         next state              condition
    //
    {st_reset,              st_init_catcher,        cond_true},
    {st_init_catcher,       st_init_conveyor,       cond_catcher_idle},
    {st_init_conveyor,      st_move_conveyor_wht,   cond_conveyor_idle},
    {st_move_conveyor_wht,  st_init_cs,             cond_conveyor_idle},
    {st_init_cs,            st_init_done,           cond_true},

    {st_init_done,          st_enter_md_running,    cond_md_run},
    {st_init_done,          st_enter_md_learning,   cond_md_learn},
    {st_init_done,          st_enter_md_pause,      cond_md_pause},

    /* md_running:*/
    {st_enter_md_running,   st_eject_smartie,       cond_true},

    {st_eject_smartie,      st_move_catcher,        cond_true},
    {st_move_catcher,       st_get_colour,          cond_md_run},
    {st_get_colour,         st_attach_colour,       cond_true},
    {st_attach_colour,      st_await_new_smartie,   cond_true},
    {st_await_new_smartie,  st_move_conveyor,       cond_all_done},
    {st_move_conveyor,      st_eject_smartie,       cond_conveyor_idle},

    /*md_learning:*/
    {st_enter_md_learning,  st_eject_smartie,       cond_true},


    {st_move_catcher,       st_learn_colour,        cond_md_learn},
    {st_learn_colour,       st_get_colour,          cond_md_learn},
    {st_learn_colour,       st_leave_md_learning,   cond_md_not_learn},
    {st_leave_md_learning,  st_get_colour,          cond_true},


};




/*************************************************************************
Function: FSM_Execute()
Purpose:
Input:    none
Returns:  none
**************************************************************************/
void
FSM_Execute(enum fsm_state state)
{
    enum COLOUR temp_col=0;
    uint16_t temp_ui16 = 0;
    {
        switch (state)
        {
        case st_reset:
            break;
        case st_init_catcher:
            MC_Catcher_Init();
            break;
        case st_init_conveyor:
            MC_Conveyor_Init();
            break;
        case st_move_conveyor_wht:
            // TODO: implement a clearer positioning for the conveyor
            MC_Conveyor_Set_Position(-1);
            break;
        case st_init_cs:
            CS_Gain_Addapt(&cs_sensor_param,CS_MIN_VAL);
            CS_LED_Addapt(&cs_sensor_led,CS_MAX_VAL);
            MC_Conveyor_Set_Position(+1);
            break;

        case st_enter_md_running:
            for(uint8_t ui8 = 0; ui8 < MC_CONVEYOR_SLOTS; ui8 ++)
                mc_smartie_table[ui8] = 0;      //Unknown;cs_sensor_data$
            mc_conveyor_position_index = 0;
            break;
        case st_enter_md_learning:
            break;
        case st_leave_md_learning:
            SM_Colours_Store();
        case st_eject_smartie:
#if FSM_DEBUG
            uart_puts_P("\n C-Pos:");
            uart_put_uint16(mc_conveyor_position_index);
#endif
            MC_Eject_Smartie();
            break;
        case st_move_catcher:
            temp_col = (mc_smartie_table[((mc_conveyor_position_index+4)%MC_CONVEYOR_SLOTS)]);
            MC_Catcher_Set_Position(temp_col);
#if FSM_DEBUG
            uart_puts_P("\tColour C:");
            uart_put_uint16(temp_col);
#endif
            break;
        case st_learn_colour:
            uart_puts_P("\n Is Colour:");
            uart_put_uint16(mc_smartie_table[(mc_conveyor_position_index+9)%10]);
            while((temp_ui16 = uart_getc())==UART_NO_DATA);
            temp_col = ((uint8_t)temp_ui16)-'0';
            mc_smartie_table[(mc_conveyor_position_index+9)%10]=temp_col;
            SM_Colour_Correct(&cs_sensor_data,temp_col);
            break;
        case st_get_colour:
            CS_Colour_Average_Get(&cs_sensor_data);
            break;
        case st_attach_colour:
#if FSM_DEBUG
            uart_puts_P("\tColour S:");
            uart_put_uint16(mc_smartie_table[mc_conveyor_position_index] = SM_Colour_Attach(&cs_sensor_data));
#else
            mc_smartie_table[mc_conveyor_position_index] = SM_Colour_Attach(&cs_sensor_data);
#endif
            break;
        case st_await_new_smartie:
            break;
        case st_move_conveyor:
            MC_Conveyor_Set_Position(+2);
#if FSM_DEBUG
            uart_puts_P("\t nPos:");
            uart_put_uint16(mc_conveyor_position_index);
#endif
            break;
        }
    }
}

/*************************************************************************
Function: FSM_Check_State()
Purpose:
Input:    none
Returns:  none
**************************************************************************/
void
FSM_Check_State(void)
{
    for(uint8_t ui8=0; ui8<sizeof(fsm_table)/sizeof(struct fsm_s); ui8++)
    {
        if(cur_state == pgm_read_byte(&fsm_table[ui8].cur_state))
        {
            if(((uint8_t (*)(void))pgm_read_word(&fsm_table[ui8].condition))())
            {
                cur_state=pgm_read_byte(&fsm_table[ui8].next_state);

                FSM_Execute(cur_state);
                break;
            }
        }
    }
}







/********** MENUE ********************************************************
*************************************************************************/

enum menue_state {   mn_reset,
                     mn_start,
                     mn_init_pause,
                     mn_init,
                     mn_running_pause,
                     mn_running,
                     mn_running_update,
                     mn_learning_pause,
                     mn_learning,
                     mn_learning_request,
                     mn_dump_pause,
                     mn_dump,
                     mn_dump_update,
                     mn_settings_pause,
                     mn_settings,
                 };

enum menue_state cur_menue = mn_reset, next_menue =  mn_reset;

/****** MENUE conditions *************************************************
*************************************************************************/
/*
static uint8_t cond_right(void)
{
    if(rot_enc_buf>)
    return (rot_enc_dir == +1)    ? 1 : 0 ;}
static uint8_t cond_left(void)  {return (rot_enc_dir == -1)    ? 1 : 0 ;}
static uint8_t cond_pushed(void){return RotEnc_Pushed();}

/**** MENUE state table *****************************************************
**************************************************************************/
/*
const struct fsm_s PROGMEM menue_table[]  =
{
        // actual state     next state          condition
        {mn_reset,          mn_start,           cond_true},
        {mn_start,          mn_running_pause,   cond_pushed},
        {mn_start,          mn_init,            cond_md_init},

        //  Level 0

        {mn_init_pause,     mn_settings_pause,  cond_left},
        {mn_init_pause,     mn_init,            cond_pushed},
        {mn_init_pause,     mn_running_pause,   cond_right},

        {mn_running_pause,  mn_init_pause,      cond_left},
        {mn_running_pause,  mn_running,         cond_pushed},
        {mn_running_pause,  mn_learning_pause,  cond_right},

        {mn_learning_pause, mn_running_pause,   cond_left},
        {mn_learning_pause, mn_learning,        cond_pushed},
        {mn_learning_pause, mn_dump_pause,      cond_right},

        {mn_dump_pause,     mn_learning_pause,  cond_left},
        {mn_dump_pause,     mn_dump,            cond_pushed},
        {mn_dump_pause,     mn_settings_pause,  cond_right},

        {mn_settings_pause, mn_dump_pause,      cond_left},
        {mn_settings_pause, mn_settings,        cond_pushed},
        {mn_settings_pause, mn_init_pause,      cond_right},

        //  Level 0
        {mn_init,           mn_init_pause,      cond_md_init},
        {mn_running,        mn_running_pause,   cond_pushed}
};




/*************************************************************************
Function: MENUE_Execute()
Purpose:
Input:    none
Returns:  none
**************************************************************************/
/*    void
MENUE_Execute(enum fsm_state state)
{
    uint16_t temp_ui16 = 0;
    {
        switch (cur_menue)
        {
            case mn_init_pause:
                lcd_waitbusy();
                lcd_clrscr();
                lcd_waitbusy();
                lcd_puts_P("  Initialise sorter?\n");
                lcd_waitbusy();
                lcd_puts_p(MN_TXT_R2_LEV0);
                break;
            case mn_init:
                cur_mode = md_init;
                lcd_waitbusy();
                lcd_clrscr();
                lcd_waitbusy();
                lcd_puts_P("  Initialising...\n   Pleas wait!");
                break;

            case mn_running_pause:
                lcd_waitbusy();
                lcd_clrscr();
                lcd_waitbusy();
                lcd_puts_P("  Start sorter?\n");
                lcd_waitbusy();
                lcd_puts_p(MN_TXT_R2_LEV0);
                break;
            case mn_running:

                break;

            case mn_learning_pause:
                lcd_waitbusy();
                lcd_clrscr();
                lcd_waitbusy();
                lcd_puts_P("  Start learning?\n");
                lcd_waitbusy();
                lcd_puts_p(MN_TXT_R2_LEV0);
                break;
            case mn_learning:
                break;
            case mn_learning_request:
                break;

            case mn_settings_pause:
                lcd_waitbusy();
                lcd_clrscr();
                lcd_waitbusy();
                lcd_puts_P("  Settings?\n");
                lcd_waitbusy();
                lcd_puts_p(MN_TXT_R2_LEV0);
                break;
        }
    }
}

/*************************************************************************
Function: MENUE_Check_State()
Purpose:
Input:    none
Returns:  none
**************************************************************************/
/*void
MENUE_Check_State(void)
{
    for(uint8_t ui8=0;ui8<sizeof(fsm_table)/sizeof(struct fsm_s);ui8++)
    {
        if(cur_state == pgm_read_byte(&fsm_table[ui8].cur_state))
        {
            if(((uint8_t (*)(void))pgm_read_word(&fsm_table[ui8].condition))())
            {
                cur_state=pgm_read_byte(&fsm_table[ui8].next_state);
                FSM_Execute(cur_state);
                break;
            }
        }
    }
}*/

