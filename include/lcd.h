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
/** @brief Инициализация дисплея */
void Lcd_Init(void);

/**
 * @brief           Очищает весь экран, заполняя его цветом color
 * @param color     Новый цвет экрана 
 */
void LCD_Clear(unsigned int color);

/** 
 * @brief        Построчная заливка заданной области экрана 
 * @param length Длина закрашиваемой зоны
 * @param width  Ширина закрашиваемой зоны
 * @param d_l    Координата по вертикали
 * @param d_w    Координата по горизонтали
 * @param color  Цвет заливки 
*/
void ClearMassDMA_Fast(uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w, uint16_t color);

/**
 * @brief           Вывод одного символа
 * @param  x        Горизонтальная координата
 * @param  y        Вертикальная координата
 * @param  ch       Код символа
 * @param  color    Цвет символа
 * @param  bg       Цвет фона
 * @param  scale    Масштаб
 */
void drawChar(uint16_t x, uint16_t y, char ch, uint16_t color, uint16_t bg, uint8_t scale);

/**
 * @brief           Вертикальный вывод строки 
 * @param  x        Горизонтальная координата колонки
 * @param  y        Вертикальная координата первого символа
 * @param  text     Заданный текст
 * @param  color    Цвет символов
 * @param  bg       Цвет фона
 * @param  scale    Масштаб
 */
void drawText(uint16_t x, uint16_t y, const char *text, uint16_t color, uint16_t bg, uint8_t scale);

/**
 * @brief          Отрисовка спрайта 8×8 (аналогично drawChar)
 * @param  x       Горизонтальная координата начала отрисовки
 * @param  y       Вертикальная координата начала отрисовки
 * @param  bmp     Массив 8 байт
 * @param  fg      Цвет спрайта
 * @param  bg      Цвет фона
 * @param  scale   Масштаб
 */
void draw_sprite_char(uint16_t x, uint16_t y, const uint8_t *bmp, uint16_t fg, uint16_t bg, uint8_t scale);

/**
 * @brief            Спрайт: сборка строк в DMA_BUF и отправка по DMA
 * @param  x         Координата d_w (горизонталь)
 * @param  y         Координата d_l (вертикаль)
 * @param  bmp       Данные Image2Cpp
 * @param  width     Ширина спрайта в пикселях
 * @param  height    Высота спрайта в пикселях
 * @param  fg        Цвет установленного бита
 * @param  bg        Цвет фона
 * @param  scale     Масштаб (шаг между блоками, размер блока = scale + 1)
 *
 * Один Address_set на весь спрайт, затем построчный DMA (аналог PrintMassDMA_Optimized).
 */
void draw_bitmap(uint16_t x, uint16_t y, const uint8_t *bmp, uint16_t width, uint16_t height, uint16_t fg, uint16_t bg, uint8_t scale);

/**
 * @brief              Перевод клетки сетки в координаты 
 * @param  col         Столбец 
 * @param  row         Дорожка 
 * @param  origin_y    Начало поля по d_l
 * @param  cell_pitch  Шаг сетки вдоль d_l
 * @param  sprite_sz   Размер спрайта (с учётом масштаба)
 * @param  ground_row  Дорожка земли
 * @param  ground_x    x на дорожке земли
 * @param  air_x       x на дорожке воздуха
 * @param  x           Выход: горизонталь 
 * @param  y           Выход: вертикаль 
 */
void grid_cell_to_pixel(int col, int row, uint16_t origin_y, uint16_t cell_pitch, uint16_t sprite_sz, int ground_row, uint16_t ground_x, uint16_t air_x, uint16_t *x, uint16_t *y);

/** 
 * @brief   Функция инверсии битов в байте 
 * @param b Исходный байт 
 * @return  Байт с развёрнутым порядком бит
 */ 
uint8_t reverse_bits(uint8_t b);

/**
 * @brief          Отрисовка одного пикселя
 * @param  x       Горизонтальная координата
 * @param  y       Вертикальная координата
 * @param  color   Цвет пикселя 
 */
void drawPixel(uint16_t x, uint16_t y, uint16_t color);

/**
 * @brief          Отрисовка залитого прямоугольника на дисплее
 * @param  length  Высота прямоугольника
 * @param  width   Ширина прямоугольника 
 * @param  x       Координата Y левого нижнего угла
 * @param  y       Координата X левого нижнего угла
 * @param  color   Цвет заливки
 */
void rect(uint16_t length, uint16_t width, uint16_t y, uint16_t x, uint16_t color);

#endif /* __LCD_H__ */
