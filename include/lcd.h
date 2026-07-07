/**
 * @file    lcd.h
 * @brief   Драйвер TFT-дисплея на базе ILI9341:
 *          инициализация и базовая отрисовка.
*/

#ifndef LCD_H
#define LCD_H

/* Includes ----------------------------------------------------------------*/
#include "mik32_hal.h"
#include "font.h"
#include "bus_spi.h"
#include "bgpio.h"
#include "bdma.h"

/* Exported functions ------------------------------------------------------- */
void Lcd_Init(void);

void LCD_Clear(unsigned int color);
void ClearMassDMA_Fast(uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w, uint16_t color);

void drawChar(uint16_t x, uint16_t y, char ch, uint16_t color, uint16_t bg, uint8_t scale);
void drawText(uint16_t x, uint16_t y, const char *text, uint16_t color, uint16_t bg, uint8_t scale);

void draw_sprite_char(uint16_t x, uint16_t y, const uint8_t *bmp, uint16_t fg, uint16_t bg, uint8_t scale);
void draw_bitmap(uint16_t x, uint16_t y, const uint8_t *bmp, uint16_t width, uint16_t height, uint16_t fg, uint16_t bg, uint8_t scale);

void grid_cell_to_pixel(int col, int row, uint16_t origin_y, uint16_t cell_pitch, uint16_t sprite_sz, int ground_row, uint16_t ground_x, uint16_t air_x, uint16_t *x, uint16_t *y);

void drawPixel(uint16_t x, uint16_t y, uint16_t color);
void rect(uint16_t length, uint16_t width, uint16_t y, uint16_t x, uint16_t color);

#endif /* LCD_H */
