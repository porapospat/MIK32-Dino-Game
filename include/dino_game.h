/**
 * @file    dino_game.h
 * @brief   Логика игры Dino Game
 */

#ifndef DINO_GAME_H
#define DINO_GAME_H

/* Includes ------------------------------------------------------------------*/
#include "lcd.h"
#include "type.h"
#include "joystick.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


/* Define --------------------------------------------------------------------*/
#define DINO_SPRITE_W         32        /* Ширина спрайта*/
#define DINO_SPRITE_H         32        /* Высота спрайта*/
#define DINO_SPRITE_SCALE     1         /* размер спрайтов */
#define DINO_SPRITE_SZ        (DINO_SPRITE_W * DINO_SPRITE_SCALE)
#define DINO_CELL_W           36        /* шаг вдоль направления бега - по горизонтали */
#define DINO_ORIGIN_Y         60        /* шаг между дорожками (по вертикали соответственно) */
#define DINO_GROUND_LINE_X    100       /* отступ отрисовки от нижнего горизонтального края экрана */
#define DINO_GROUND_LENGHT    440       /* длина линии земли */
#define DINO_GROUND_LINE_Y    20        /* отступ отрисовки от левого вертикального края экрана*/
#define DINO_GROUND_LINE_W    2         /* толщина линии дороги */
#define DINO_GROUND_FEET_X    (DINO_GROUND_LINE_X + DINO_GROUND_LINE_W)
#define DINO_AIR_X            (DINO_GROUND_FEET_X + DINO_SPRITE_SZ + 12)

#define DINO_COLOR_WHITE      0xFFFF    /* белый цвет */
#define DINO_COLOR_BLACK      0x0000    /* черный цвет*/
#define DINO_COLOR_GO         0x07E0    /* красный цвет GAME OVER*/

#define DINO_THEME_SCORE      100       /* смена режима каждые 100 очков */

#define DINO_DELAY_START      40        /* начальная задержка  */
#define DINO_DELAY_MIN        10        /* минимальная задержка */
#define DINO_DELAY_STEP       50        /* порог, ниже которого скорость растёт */

/* Exported functions --------------------------------------------------------*/
/** @brief  Инициализация игры - Отриосвка заставки и первого кадра */
void DinoGame_Init(void);

/** @brief  Сброс счёта, препятствий и перерисовка поля */
void DinoGame_Reset(void);

/**
 * @brief  Один шаг автомата игры: playing/game over/restart
 */
void DinoGame_Tick(void);

#endif /* __DINO_GAME_H__ */
