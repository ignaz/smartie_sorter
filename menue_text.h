/*
 * =====================================================================================
 *
 *       Filename:  menue_text.h
 *
 *    Description:  just progmem strings for menue entries!!!
 *
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

#ifndef _MENUE_TEXT_H
#define _MENUE_TEXT_H

#include "smarties.h"

const uint8_t PROGMEM MN_TXT_R2_LEV0[] = {"Prev      ENTER      Next"};

const uint8_t PROGMEM MN_TXT_R1_LEV1[] = {"Sensor:     Catcher:     "};

const uint8_t PROGMEM MN_COL[COLOR_MAX][4] =
{
    {'N','O',' ',0},
    {'r','e','d',0},
    {'o','r','a',0},
    {'y','e','l',0},
    {'g','r','e',0},
    {'b','l','u',0},
    {'v','i','o',0},
    {'p','i','n',0},
    {'b','r','o',0},
};


#endif  //_MENUE_TEXT_H
