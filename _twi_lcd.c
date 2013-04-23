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
#include "lcd.h"
#include "twi_lcd.h"
#include "twi_master.h"

/*
 * GLOBALS / BUFFER FOR TWI...
 */

volatile uint8_t twi_lcd_buf[20]={TWI_LCD_ADRESS, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

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
	TWI_Send(twi_lcd_buf,4);
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
	TWI_Send(twi_lcd_buf,4);
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
#error lcd_getxy not available in TWI-version!;
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
	twi_lcd_buf[2] = TWI_LCD_PUTC;
	twi_lcd_buf[3] = c;
	TWI_Send(twi_lcd_buf,4);
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
		while ( (c = *s++) ) {
        twi_lcd_buf[i++] = c;
    }
		twi_lcd_buf[i++] = 0;
		TWI_Send(twi_lcd_buf,i);

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
		TWI_Send(twi_lcd_buf,i);

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
	twi_lcd_buf[2] = TWI_LCD_INIT;
	twi_lcd_buf[3] = dispAttr;
	TWI_Send(twi_lcd_buf,4);
}/* lcd_init */
