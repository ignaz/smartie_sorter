/*
 * =====================================================================================
 *
 *       Filename:  twi_lcd.c
 *
 *    Description:  Basic routines for interfacing my modified lcd.c of Volker Oth via
 *    							TWI. The orginal files can easyly be replaced by the following ones
 *    							to use my twi2lcd instead of the parallel connected one.
 *    							lcd.c -> twi_lcd.c
 *    							lcd.h -> twi_lcd.h and lcd.h
 *    							the twi_lcd.h is used on both sides.
 *
 * 			 		 Uage:  See the C included lcd.h file for description of pins and functions.
 *
 *        Version:  1.0
 *        Created:  22.06.2010 20:15:59
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ignaz Laepple (mn), ignaz.laepple@gmx.de
 *        Company:  FH-Regensburg
 *
 * =====================================================================================
 */


#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "twi_lcd.h"
#include "twi_slave.h"
#include "twi_master.h"

/*
 * GLOBALS / BUFFER FOR TWI...
 */

volatile uint8_t twi_lcd_buf[50]=   {(TWI_LCD_ADRESS<<1),4,
                                    0,0,0,0,0,0,0,0,
                                    0,0,0,0,0,0,0,0,
                                    0,0,0,0,0,0,0,0,
                                    0,0,0,0,0,0,0,0,
                                    0,0,0,0,0,0,0,0,
                                    0,0,0,0,0,0,0,0};


 volatile uint8_t lcd_rot_enc_val=0;
 volatile uint8_t lcd_buttun_pressed=0;
/*
** PUBLIC FUNCTIONS
*/

/*************************************************************************
Send LCD controller instruction command
Input:   instruction to send to LCD controller, see HD44780 data sheet
Returns: none
*************************************************************************/
void lcd_command(uint8_t cmd)
{
	twi_lcd_buf[2] = TWI_LCD_COMMAND;
	twi_lcd_buf[3] = cmd;
    lcd_waitbusy();
	TWI_Master_Transceive_Message(twi_lcd_buf,4);
}


/*************************************************************************
Send data byte to LCD controller
Input:   data to send to LCD controller, see HD44780 data sheet
Returns: none
*************************************************************************/
void lcd_data(uint8_t data)
{
	twi_lcd_buf[2] = TWI_LCD_DATA;
	twi_lcd_buf[3] = data;
    lcd_waitbusy();
	TWI_Master_Transceive_Message(twi_lcd_buf,4);
}


/*************************************************************************
Set cursor to specified position
Input:    x  horizontal position  (0: left most position)
          y  vertical position    (0: first line)
Returns:  none
*************************************************************************/
void lcd_gotoxy(uint8_t x, uint8_t y)
{
#if LCD_LINES==1
    lcd_command((1<<LCD_DDRAM)+LCD_START_LINE1+x);
#endif
#if LCD_LINES==2
    if ( y==0 )
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE1+x);
    else
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE2+x);
#endif
#if LCD_LINES==4
    if ( y==0 )
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE1+x);
    else if ( y==1)
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE2+x);
    else if ( y==2)
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE3+x);
    else /* y==3 */
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE4+x);
#endif

}/* lcd_gotoxy */


/*************************************************************************
*************************************************************************/
int lcd_getxy(void)
{
#warning lcd_getxy not available in TWI-version!;
}


/*************************************************************************
Clear display and set cursor to home position
*************************************************************************/
void lcd_clrscr(void)
{
    lcd_command(1<<LCD_CLR);
}


/*************************************************************************
Set cursor to home position
*************************************************************************/
void lcd_home(void)
{
    lcd_command(1<<LCD_HOME);
}


/*************************************************************************
Display character at current cursor position
Input:    character to be displayed
Returns:  none
*************************************************************************/
void lcd_putc(char c)
{
    lcd_waitbusy();
	twi_lcd_buf[2] = TWI_LCD_PUTC;
	twi_lcd_buf[3] = c;
    lcd_waitbusy();
	TWI_Master_Transceive_Message(twi_lcd_buf,4);
}/* lcd_putc */


/*************************************************************************
Display string without auto linefeed
Input:    string to be displayed
Returns:  none
*************************************************************************/
void lcd_puts(const char *s)
/* print string on lcd (no auto linefeed) */
{
    register char c;
		uint8_t i = 3;

		twi_lcd_buf[2] = TWI_LCD_PUTS;
		while ( (c = *s++) )
            twi_lcd_buf[i++] = c;

		twi_lcd_buf[i++] = 0;
        lcd_waitbusy();
        TWI_Master_Transceive_Message(twi_lcd_buf,i);
}/* lcd_puts */


/*************************************************************************
Display string from program memory without auto linefeed
Input:     string from program memory be be displayed
Returns:   none
*************************************************************************/
void lcd_puts_p(const char *progmem_s)
/* print string from program memory on lcd (no auto linefeed) */
{
    register char c;
		uint8_t i = 3;

		twi_lcd_buf[2] = TWI_LCD_PUTS;
    while ( (c = pgm_read_byte(progmem_s++)) ) {
        twi_lcd_buf[i++] = c;
    }
		twi_lcd_buf[i++] = 0;
        lcd_waitbusy();
        TWI_Master_Transceive_Message(twi_lcd_buf,i);

}/* lcd_puts_p */


/*************************************************************************
Initialize display and select type of cursor
Input:    dispAttr LCD_DISP_OFF            display off
                   LCD_DISP_ON             display on, cursor off
                   LCD_DISP_ON_CURSOR      display on, cursor on
                   LCD_DISP_CURSOR_BLINK   display on, cursor on flashing
Returns:  none
*************************************************************************/
void lcd_init(uint8_t dispAttr)
{
    lcd_waitbusy();
	twi_lcd_buf[2] = TWI_LCD_INIT;
	twi_lcd_buf[3] = dispAttr;
    TWI_Master_Transceive_Message(twi_lcd_buf,4);
}/* lcd_init */

/*************************************************************************
Wait until display is in idle mode
Input:
Returns:
*************************************************************************/
void lcd_waitbusy(void)
{
	while (!((TWI_Master_Read_Byte(9))&(_BV(BUF_0_EMPTY)|_BV(BUF_1_EMPTY))));
}/* lcd_init */


/*************************************************************************
 Used for string assembly. ATTENTION no check whether position is inside the SRAM-string
Input:      pointer to string in SRAM,
            pointer to string in FLASH (PGM)
            position where to insert
Returns:    none
*************************************************************************/
/*void lcd_insert_string(char * ram_s,const char *progmem_s, uint8_t pos)
{
    register char c;

    while ( (c = pgm_read_byte(progmem_s++)) )
    {
        ram_s[pos++] = c;
    }
}

/*************************************************************************
 Used for string assembly.
Input:      pointer to string in SRAM,
            pointer to string in FLASH (PGM)
Returns:    none
*************************************************************************/
/*void lcd_copy_string_p(char * ram_s,const char *progmem_s)
{
    register char c;
    uint8_t pos = 0;

    while ( (c = pgm_read_byte(progmem_s++)) )
    {
        ram_s[pos++] = c;
    }
}
/*************************************************************************
Read out of the rotation encoder value
Input:      pointer to string in SRAM,
            pointer to string in FLASH (PGM)
Returns:    none
*************************************************************************/
void lcd_rot_enc_get(void)
{
    static int8_t rot_enc_val_old=0;
    int8_t rot_enc_val_cur;
    uint8_t ui8_tmp=0;
    ui8_tmp=TWI_Master_Read_Byte(TWI_LCD_ADRESS);
    if(ui8_tmp&_BV(ROT_ENC_EVENT))
    {
        rot_enc_val_cur = TWI_Master_Read_Register(3,TWI_LCD_ADRESS);
        lcd_rot_enc_val += rot_enc_val_cur - rot_enc_val_old;
        rot_enc_val_old = rot_enc_val_cur;
    }
    if(ui8_tmp&_BV(PORT_X_EVENT))
    {
        ui8_tmp = TWI_Master_Read_Register(2,TWI_LCD_ADRESS);;
        if ((ui8_tmp & _BV(5))==0) lcd_buttun_pressed = 1;
    }
}
