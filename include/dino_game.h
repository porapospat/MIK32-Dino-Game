/**
 * @file    dino_game.h
 * @brief   Логика игры Dino Game
 */

#ifndef DINO_GAME_H
#define DINO_GAME_H

/* Includes ------------------------------------------------------------------*/
#include "type.h"
#include "joystick.h"
#include <stdbool.h>

/* Define --------------------------------------------------------------------*/
#define DINO_SPRITE_W       32        /* Ширина спрайта*/
#define DINO_SPRITE_H       32        /* Высота спрайта*/
#define DINO_SPRITE_SCALE   1         /* размер спрайтов */
#define DINO_SPRITE_SZ      (DINO_SPRITE_W * DINO_SPRITE_SCALE)
#define DINO_CELL_W         36        /* шаг вдоль направления бега - по горизонтали */
#define DINO_ORIGIN_Y       60        /* шаг между дорожками (по вертикали соответственно) */
#define DINO_GROUND_LINE_X  100       /* отступ отрисовки от нижнего горизонтального края экрана */
#define DINO_GROUND_LENGHT  440       /* длина линии земли */
#define DINO_GROUND_LINE_Y  20        /* отступ отрисовки от левого вертикального края экрана*/
#define DINO_GROUND_LINE_W  2         /* толщина линии дороги */
#define DINO_GROUND_FEET_X  (DINO_GROUND_LINE_X + DINO_GROUND_LINE_W)
#define DINO_AIR_X          (DINO_GROUND_FEET_X + DINO_SPRITE_SZ + 12)

#define DINO_COLOR_FG       0x0000    /* чёрный цвет персонажей, препятствий и линии земли */
#define DINO_COLOR_BG       0xFFFF    /* белый фон */
#define DINO_COLOR_GO       0x07E0    /* красный цвет для GAME OVER*/

#define DINO_DELAY_START    40         /* начальная задержка  */
#define DINO_DELAY_MIN      10         /* минимальная задержка */
#define DINO_DELAY_STEP     50         /* порог, ниже которого скорость растёт */

/* Exported functions --------------------------------------------------------*/
/** @brief  Инициализация игры - Отриосвка заставки и первого кадра */
void DinoGame_Init(void);

/** @brief  Сброс счёта, препятствий и перерисовка поля */
void DinoGame_Reset(void);

/**
 * @brief  Один кадр: ввод джойстика, логика, отрисовка
 * @return 1 — столкновение, 0 — игра продолжается
 */
bool DinoGame_Tick(void);

#endif /* __DINO_GAME_H__ */
