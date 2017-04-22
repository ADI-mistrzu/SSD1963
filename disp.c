/*
 * disk.c
 *
 *  Created on: 8 lut 2016
 *      Author: Adrian Chmiel
 */

#include "ch.h"
#include "hal.h"

#include "ssd1963.h"
#include "disp.h"
#include "dejavu24.h"
#include "dejavu32.h"
//#include "font32.h"
#include "define.h"


void setpin_reset(uint8_t state)
{
	if(state)
		LCD_RST_ON;
	else
		LCD_RST_OFF;
}

void set_backlight(uint8_t light)
{
	LCD_cmd(SSD1963_SET_PWM_CONF);		//set PWM for BackLight
	LCD_data(0x06);
	LCD_data(light);
	LCD_data(0x01);						// Controlled by host (not DBC), enabled
	LCD_data(0xFF);						// DBC manual brightness (not used - zero would work)
	LCD_data(0x00);						// DBC minimum brightness
	LCD_data(0x01);						// Brightness prescaler - active when Transition Effect enable A5 = 1
}

void Set_Window( uint16_t x_start, uint16_t x_stop, uint16_t y_start, uint16_t y_stop )
{
	LCD_cmd( SSD1963_SET_COLUMN_ADDRESS );
		LCD_data( x_start>>8 );
		LCD_data( x_start );
		LCD_data( x_stop>>8 );
		LCD_data( x_stop );
	LCD_cmd( SSD1963_SET_PAGE_ADDRESS );
		LCD_data( y_start>>8 );
		LCD_data( y_start );
		LCD_data( y_stop>>8 );
		LCD_data( y_stop );
	LCD_cmd( SSD1963_WRITE_MEMORY_START );
}

void Fill_Window( uint32_t size, uint16_t color )
{
	while( size-- )
		LCD_data(color);
}

void Fill_Window_cust( uint16_t *data, uint16_t ln )
{
	while(ln--)
	{
		LCD_data(*data);
		data++;
	}
//	uint16_t i;
//	for(i=0; i<ln; i++)
//		LCD_data(data[i]);
}

void Draw_Pixel( uint16_t x, uint16_t y, uint16_t color )
{
	LCD_cmd( SSD1963_SET_COLUMN_ADDRESS );
		LCD_data( x>>8 );
		LCD_data( x );
		LCD_data( x>>8 );
		LCD_data( x );
	LCD_cmd( SSD1963_SET_PAGE_ADDRESS );
		LCD_data( y>>8 );
		LCD_data( y );
		LCD_data( y>>8 );
		LCD_data( y );
	LCD_cmd( SSD1963_WRITE_MEMORY_START );

	LCD_data(color);
}

void DrawBox( int16_t x_start, int16_t x_stop, int16_t y_start, int16_t y_stop, uint16_t color )
{
	uint32_t i;

	if(x_start<0)
		x_start = 0;
	if(x_stop<0)
		x_stop = 0;
	if(y_start<0)
		y_start = 0;
	if(y_stop<0)
		y_stop = 0;

	LCD_cmd( SSD1963_SET_COLUMN_ADDRESS );
	if( x_start>x_stop )
	{
		LCD_data( x_stop>>8 );
		LCD_data( x_stop );
		i = (x_start-x_stop)+1;
		LCD_data( x_start>>8 );
		LCD_data( x_start );
	}else{
		LCD_data( x_start>>8 );
		LCD_data( x_start );
		i = (x_stop-x_start)+1;
		LCD_data( x_stop>>8 );
		LCD_data( x_stop );
	}

	LCD_cmd( SSD1963_SET_PAGE_ADDRESS );
	if( y_start>y_stop )
	{
		LCD_data( y_stop>>8 );
		LCD_data( y_stop );
		i *= (y_start-y_stop)+1;
		LCD_data( y_start>>8 );
		LCD_data( y_start );
	}else{
		LCD_data( y_start>>8 );
		LCD_data( y_start );
		i *= (y_stop-y_start)+1;
		LCD_data( y_stop>>8 );
		LCD_data( y_stop );
	}
	i++;
	LCD_cmd( SSD1963_WRITE_MEMORY_START );

	while( i-- )
		LCD_data(color);
}

void ReadBox(uint16_t x_start, uint16_t x_stop, uint16_t y_start, uint16_t y_stop, uint16_t *buf)
{
	uint32_t i;

	LCD_cmd( SSD1963_SET_COLUMN_ADDRESS );
	if( x_start>x_stop )
	{
		LCD_data( x_stop>>8 );
		LCD_data( x_stop );
		i = (x_start-x_stop)+1;
		LCD_data( x_start>>8 );
		LCD_data( x_start );
	}else{
		LCD_data( x_start>>8 );
		LCD_data( x_start );
		i = (x_stop-x_start)+1;
		LCD_data( x_stop>>8 );
		LCD_data( x_stop );
	}

	LCD_cmd( SSD1963_SET_PAGE_ADDRESS );
	if( y_start>y_stop )
	{
		LCD_data( y_stop>>8 );
		LCD_data( y_stop );
		i *= (y_start-y_stop)+1;
		LCD_data( y_start>>8 );
		LCD_data( y_start );
	}else{
		LCD_data( y_start>>8 );
		LCD_data( y_start );
		i *= (y_stop-y_start)+1;
		LCD_data( y_stop>>8 );
		LCD_data( y_stop );
	}
	i++;
	LCD_cmd( SSD1963_READ_MEMORY_START );

	while( i-- )
		buf[i] = LCD_DATA16;
}

void scr_printf_24(uint16_t x, uint16_t y, uint8_t *s, uint16_t *txt_color)
{
	uint16_t i, j;
	uint16_t tmp_x = x;
	uint8_t chr;
/*
	uint32_t string[800];
	uint16_t txt_lenght = 0;

	do{
		txt_lenght += dejaVuSans_24_width[*s-32];
		s++;
	}while(*s);
*/

	do{
		again:

		if(*s>127)
			chr = '?';
		else
			chr = *s;

		if(chr==0x0D)
			return;
		if(chr==0x0A)
		{
			s++;
			goto again;
		}
		j = dejaVuSans_24_width[chr-32];
		LCD_cmd( SSD1963_SET_COLUMN_ADDRESS );
			LCD_data( (x)>>8 );
			LCD_data( x );
			LCD_data( (x+j)>>8 );
			LCD_data( x+j );
		LCD_cmd( SSD1963_SET_PAGE_ADDRESS );
			LCD_data( y>>8 );
			LCD_data( y );
			LCD_data( (y+23)>>8 );
			LCD_data( y+23 );
		LCD_cmd( SSD1963_WRITE_MEMORY_START );

		for(j=0; j<dejaVuSans_24_width[chr-32]*3; j++)
			for(i=0; i<8; i++)
				LCD_data((dejaVuSans_24_data[dejaVuSans_24_offset[chr-32]+j]>>i)&1?txt_color[1]:txt_color[0]);

		x += dejaVuSans_24_width[chr-32];
		s++;
/*		if((x+dejaVuSans_24_width[*s-32])>=800)
		{
			x = tmp_x;
			y += 32;
		}*/
	}while(*s);

	return;
}

void scr_printf_32(uint16_t x, uint16_t y, uint8_t *s, uint16_t *txt_color)
{
	uint16_t i, j;
	uint16_t tmp_x = x;
	uint8_t chr;

	do{
		again:
		if(*s>127)
			chr = '?';
		else
			chr = *s;

		if(chr==0x0D)
		{
			y += 32;
			s++;
			goto again;
		}
		if(chr==0x0A)
		{
			x = tmp_x;
			s++;
			goto again;
		}
		if(chr==' ')
		{
			DrawBox(x, x+6, y, y+31, txt_color[0]);
			x += 8;
			goto drawbox_out;
		}
		j = dejaVuSans_32_width[chr-32];
		LCD_cmd( SSD1963_SET_COLUMN_ADDRESS );
			LCD_data( (x)>>8 );
			LCD_data( x );
			LCD_data( (x+j)>>8 );
			LCD_data( x+j );
		LCD_cmd( SSD1963_SET_PAGE_ADDRESS );
			LCD_data( y>>8 );
			LCD_data( y );
			LCD_data( (y+31)>>8 );
			LCD_data( y+31 );
		LCD_cmd( SSD1963_WRITE_MEMORY_START );

		for(j=0; j<dejaVuSans_32_width[chr-32]*4; j++)
			for(i=0; i<8; i++)
				LCD_data((dejaVuSans_32_data[dejaVuSans_32_offset[chr-32]+j]>>i)&1?txt_color[1]:txt_color[0]);

		x += dejaVuSans_32_width[*s-32]+2;
		drawbox_out:
		s++;
		if((x+dejaVuSans_32_width[*s-32])>=800)
		{
			x = tmp_x;
			y += 32;
		}
	}while(*s);
}

void scr_print_nr32(uint16_t x, uint16_t y, uint16_t nr, uint16_t *txt_color)
{
	uint8_t txt[8];

	txt[0] = '0'+(uint8_t)(nr/10000);
	txt[1] = '0'+(uint8_t)(nr/1000)%10;
	txt[2] = '0'+(uint8_t)(nr/100)%10;
	txt[4] = '0'+(uint8_t)(nr/10)%10;
	txt[5] = '0'+(uint8_t)nr%10;
	txt[6] = 0;

	scr_printf_32(x, y, txt, txt_color);
}

void scr_print_nr24(uint16_t x, uint16_t y, uint32_t nr, uint16_t *txt_color)
{
	uint8_t i, txt[12];

	txt[0] = '0'+(nr/100000000);
	txt[1] = '0'+(nr/10000000)%10;
	txt[2] = '0'+(nr/1000000)%10;
	txt[3] = ' ';
	txt[4] = '0'+(nr/100000)%10;
	txt[5] = '0'+(nr/10000)%10;
	txt[6] = '0'+(nr/1000)%10;
	txt[7] = ' ';
	txt[8] = '0'+(nr/100)%10;
	txt[9] = '0'+(nr/10)%10;
	txt[10] = '0'+nr%10;
	txt[11] = 0;

	for(i=0; i<10; i++)
		if((txt[i]!='0')&(txt[i]!=' '))
		{
			DrawBox(x, x+(10*i), y, y+24, BLACK);
			break;
		}else{
			txt[i] = ' ';
		}

	scr_printf_24(x, y, txt, txt_color);
}

void init_display(void)
{
	rccEnableAHB(RCC_AHBENR_FSMCEN, 0);

//	FSMC_Bank1->BTCR[0+1] = FSMC_BTR1_ADDSET_0 | FSMC_BTR1_DATAST_2 | FSMC_BTR1_BUSTURN_0 ;
	FSMC_Bank1->BTCR[0+1] = (FSMC_BTR1_ADDSET_1 | FSMC_BTR1_ADDSET_3) |
							(FSMC_BTR1_DATAST_1 | FSMC_BTR1_DATAST_3) |
							(FSMC_BTR1_BUSTURN_1 | FSMC_BTR1_BUSTURN_3) ;

//	FSMC_Bank1->BTCR[1] =	FSMC_BTR1_DATAST_0;		// Timing Register, BTR1
	FSMC_Bank1E->BWTR[0] =	0;//0x0fffffff;			// Write timing register, BWTR1

//	FSMC_Bank1->BTCR[0] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
	FSMC_Bank1->BTCR[0] =	FSMC_BCR1_MWID_0 |
							FSMC_BCR1_WREN |		// Control Register, BCR1
							FSMC_BCR1_MBKEN;


	LCD_RST_ON;
	chThdSleepMilliseconds(50);
	LCD_RST_OFF;
	chThdSleepMilliseconds(50);

	LCD_cmd(SSD1963_SET_PWM_CONF);
	LCD_data(0x09);
	LCD_data(0);
	LCD_data(1);
	LCD_data(0);
	LCD_data(0);
	LCD_data(0);

	LCD_cmd(SSD1963_SET_PLL_MN);			    			// Set PLL with OSC = 10MHz (hardware)
	LCD_data(0x35);											// Multiplier N = 35, VCO (>250MHz)= OSC*(N+1), VCO = 340MHz
	LCD_data(0x02);											// Divider M = 2, PLL = 360/(M+1) = 120MHz
	LCD_data(0x54);											// Validate M and N values

	LCD_cmd(SSD1963_SET_PLL);
	LCD_data(0x01);
	chThdSleepMilliseconds(10);

	LCD_cmd(SSD1963_SET_PLL);
	LCD_data(0x03);
	chThdSleepMilliseconds(10);

	LCD_cmd(SSD1963_SOFT_RESET);			// Soft reset
	chThdSleepMilliseconds(10);

	LCD_cmd(SSD1963_SET_LSHIFT_FREQ);
	LCD_data(0x00);
	LCD_data(0xFF);
	LCD_data(0xFF);
/*
	LCD_cmd(SSD1963_SET_LSHIFT_FREQ);
	LCD_data((LCD_FPR >> 16) & 0xFF);
	LCD_data((LCD_FPR >> 8) & 0xFF);
	LCD_data(LCD_FPR & 0xFF);
*/

	LCD_cmd(SSD1963_SET_LCD_MODE);
	LCD_data(0x20);						// 24bit
	LCD_data(0x00);						// TFT mode
	LCD_data(mHIGH((TFT_WIDTH-1)));
	LCD_data(mLOW((TFT_WIDTH-1)));
	LCD_data(mHIGH((TFT_HEIGHT-1)));
	LCD_data(mLOW((TFT_HEIGHT-1)));
	LCD_data(0);

	LCD_cmd(SSD1963_SET_HORI_PERIOD);
	LCD_data(mHIGH(TFT_HSYNC_PERIOD));
	LCD_data(mLOW(TFT_HSYNC_PERIOD));
	LCD_data(mHIGH((TFT_HSYNC_PULSE + TFT_HSYNC_BACK_PORCH)));
	LCD_data(mLOW((TFT_HSYNC_PULSE + TFT_HSYNC_BACK_PORCH)));
	LCD_data(TFT_HSYNC_PULSE);
	LCD_data(0x00);
	LCD_data(0x00);
	LCD_data(0x00);

	LCD_cmd(SSD1963_SET_VERT_PERIOD);
	LCD_data(mHIGH(TFT_VSYNC_PERIOD));
	LCD_data(mLOW(TFT_VSYNC_PERIOD));
	LCD_data(mHIGH((TFT_VSYNC_PULSE + TFT_VSYNC_BACK_PORCH)));
	LCD_data(mLOW((TFT_VSYNC_PULSE + TFT_VSYNC_BACK_PORCH)));
	LCD_data(TFT_VSYNC_PULSE);
	LCD_data(0x00);
	LCD_data(0x00);

	LCD_cmd(SSD1963_SET_ADDRESS_MODE);
	LCD_data(1<<5);

	LCD_cmd(SSD1963_SET_GPIO_VALUE);	// all GPIO low
	LCD_data(0x08);
	LCD_cmd(SSD1963_SET_GPIO_CONF);		// all gpio output
	LCD_data(0x0F);
	LCD_data(0x01);

	LCD_cmd(SSD1963_SET_PIXEL_DATA_INTERFACE);
	LCD_data(SSD1963_PDI_16BIT565);

	DrawBox(0, 800, 0, 480, 0);
	LCD_cmd(SSD1963_SET_DISPLAY_ON);			// SET display on
}
