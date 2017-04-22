/*
 * disk.h
 *
 *  Created on: 8 lut 2016
 *      Author: Mistrzu
 */

#ifndef OS_VARIOUS_DISP_DISP_H_
#define OS_VARIOUS_DISP_DISP_H_

#include "define.h"


#define	RED			0xF800
#define	GREEN		0x07E0
#define	BLUE		0x001F
#define	BLACK		0
#define	WHITE		0xFFFF

#define	Clear_display()		DrawBox(0, 800, 0, 482, BLACK)

void setpin_reset(uint8_t state);
void set_backlight(uint8_t percent);
void Set_Window( uint16_t x_start, uint16_t x_stop, uint16_t y_start, uint16_t y_stop );
void Fill_Window( uint32_t size, uint16_t color );
void Fill_Window_cust( uint16_t *data, uint16_t ln );
void Draw_Pixel( uint16_t x, uint16_t y, uint16_t color );
void DrawBox( int16_t x_start, int16_t x_stop, int16_t y_start, int16_t y_stop, uint16_t color );
void ReadBox(uint16_t x_start, uint16_t x_stop, uint16_t y_start, uint16_t y_stop, uint16_t *buf);
void scr_printf_24(uint16_t x, uint16_t y, uint8_t *s, uint16_t *txt_color);
void scr_printf_32(uint16_t x, uint16_t y, uint8_t *s, uint16_t *txt_color);
void scr_print_nr32(uint16_t x, uint16_t y, uint16_t nr, uint16_t *txt_color);
void scr_print_nr24(uint16_t x, uint16_t y, uint32_t nr, uint16_t *txt_color);

void init_display(void);


#endif /* OS_VARIOUS_DISP_DISP_H_ */
