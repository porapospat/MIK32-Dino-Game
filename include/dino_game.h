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
#define DINO_SPRITE_W         32
#define DINO_SPRITE_H         32
#define DINO_SPRITE_SCALE     1
#define DINO_SPRITE_SZ        (DINO_SPRITE_W * DINO_SPRITE_SCALE)
#define DINO_CELL_W           36
#define DINO_ORIGIN_Y         60
#define DINO_GROUND_LINE_X    100
#define DINO_GROUND_LINE_W    2
#define DINO_GROUND_FEET_X    (DINO_GROUND_LINE_X + DINO_GROUND_LINE_W)
#define DINO_AIR_X            (DINO_GROUND_FEET_X + DINO_SPRITE_SZ + 12)

#define DINO_COLOR_FG         0x0000
#define DINO_COLOR_BG         0xFFFF
#define DINO_COLOR_GO         0x07E0

#define DINO_DELAY_START      40
#define DINO_DELAY_STEP       50

/* Exported functions --------------------------------------------------------*/
/**
 * @brief  Инициализация игры - Отриосвка заставки и первого кадра
 */
void DinoGame_Init(void);

/**
 * @brief  Сброс счёта, препятствий и перерисовка поля
 */
void DinoGame_Reset(void);

/**
 * @brief  Один кадр: ввод джойстика, логика, отрисовка
 * @return 1 — столкновение, 0 — игра продолжается
 */
bool DinoGame_Tick(void);

#endif /* __DINO_GAME_H__ */
