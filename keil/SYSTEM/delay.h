#ifndef __DELAY_H
#define __DELAY_H
#include "stdint.h"

#define  SYSTEM_SUPPORT_OS   1

#define   SYS    16

typedef unsigned           int u32;
typedef unsigned    short  int u16;
typedef unsigned           char u8;

void delay_init(u8 SYSCLK);
void delay_us(u32 nus);
void delay_xms(u16 nms);
void delay_ms(u16 nms);
#endif 
