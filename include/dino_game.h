/**
 * @file    dino_game.h
 * @brief   Логика игры Dino Game
 */

#ifndef DINO_GAME_H
#define DINO_GAME_H

/* Includes ------------------------------------------------------------------*/
#include "lcd.h"
#include "dino_type.h"
#include "dino_sprites.h"
#include "joystick.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/* Define --------------------------------------------------------------------*/
#define DINO_SPRITE_W         32        /* Ширина спрайта*/
#define DINO_SPRITE_H         32        /* Высота спрайта*/
#define DINO_SPRITE_SCALE     1         /* размер спрайтов */
#define DINO_SPRITE_SZ        (DINO_SPRITE_W * DINO_SPRITE_SCALE)
#define DINO_FONT_SCALE       2         /* Размер шрифта текстовых сообщений */
#define DINO_CELL_W           36        /* шаг вдоль направления бега - по горизонтали */
#define DINO_ORIGIN_Y         60        /* шаг между дорожками (по вертикали соответственно) */
#define DINO_GROUND_LINE_X    100       /* отступ отрисовки от нижнего горизонтального края экрана */
#define DINO_GROUND_LENGHT    440       /* длина линии земли */
#define DINO_GROUND_LINE_Y    20        /* отступ отрисовки от левого вертикального края экрана*/
#define DINO_GROUND_LINE_W    2         /* толщина линии дороги */
#define DINO_GROUND_FEET_X    (DINO_GROUND_LINE_X + DINO_GROUND_LINE_W)
#define DINO_AIR_X            (DINO_GROUND_FEET_X + DINO_SPRITE_SZ + 12)

#define DINO_BIRD_W           32        /* ширина спрайта птицы */
#define DINO_BIRD_H           27        /* высота спрайта птицы */
#define DINO_BIRD_LOW_OFFSET  -17       /* смещение низкой птицы вниз, px */
#define DINO_DUCK_H           20        /* высота спрайта динозавра в приседе */

#define DINO_SMALL_CLOUD_W    43        /* ширина малого облака */
#define DINO_BIG_CLOUD_W      62        /* ширина большого облака */
#define DINO_CLOUD_H          17        /* высота спрайта облака */
#define DINO_ICON_X           210       /* вертикаль иконок солнца/луны */
#define DINO_CLOUD_X          185       /* вертикаль облаков */
#define DINO_CLOUD_PERIOD     2         /* сдвиг облака раз в N кадров */

#define DINO_COLOR_WHITE      0xFFFF    /* белый цвет */
#define DINO_COLOR_BLACK      0x0000    /* черный цвет*/
#define DINO_COLOR_GO         0x07E0    /* красный цвет GAME OVER*/

#define DINO_THEME_SCORE      100       /* смена режима каждые 100 очков */

#define DINO_DELAY_START      100       /* начальная задержка  */
#define DINO_DELAY_MIN        10        /* минимальная задержка */
#define DINO_DELAY_STEP       50        /* порог, ниже которого скорость растёт */

/* Exported variables --------------------------------------------------------*/
extern DinoGame sDinoGame;

/* Exported functions --------------------------------------------------------*/
/** @brief  Инициализация игры - Отриосвка заставки и первого кадра */
void DinoGame_Init(void);

/** @brief  Сброс счёта, препятствий и перерисовка поля */
void DinoGame_Reset(void);

/** @brief  Один шаг автомата игры: playing/game over/restart */
void DinoGame_Tick(void);

/**
 * @brief         Применение цветов соответствующего режима
 * @param theme   Режим солнца или луны
 */
void dino_apply_theme(DinoTheme theme);

/**
 * @brief  Смена режима солнце/луна при переходе через каждые 100 очков
 * @return true — режим изменился и экран перерисован
 */
bool dino_update_theme(void);

/**
 * @brief  Отрисовка статических элементов: дорога, счёт и иконка (луны или солнца)
 *         — рисуются один раз при старте и рестарте
 */
void dino_draw_static(void);

/** @brief  Отрисовка счёта в нижней части экрана (только при изменении) */
void dino_draw_score(void);

/** @brief  Отрисовка иконки солнца или луны */
void dino_draw_icon(void);

/** @brief  Экран Game Over */
void dino_game_over(void);

/* Sprites on field */
/**
 * @brief         Отрисовка спрайта в клетке игрового поля
 * @param col     Колонка
 * @param row     Дорожка
 * @param width   Ширина
 * @param height  Длина
 * @param bmp     Указатель на массив спрайта
 * @param x_off   Смещение по вертикали
 */
void dino_draw_sprite(int col, int row, int width, int height, const uint8_t *bmp, int16_t x_off);

/**
 * @brief         Очистка спрайта в клетке игрового поля
 * @param col     Колонка
 * @param row     Дорожка
 * @param width   Ширина
 * @param height  Длина
 * @param x_off   Смещение по вертикали
 */
void dino_clear_sprite(int col, int row, int width, int height, int16_t x_off);

/* Clouds */
/** @brief  Начальная расстановка облаков */
void dino_generate_clouds(void);

/** @brief  Сдвиг облаков на один шаг, как у препятствий */
void dino_update_clouds(void);

/**
 * @brief      Отрисовка облака в указанном столбце (на уровне иконок)
 * @param col  Столбец облака
 * @param index Индекс облака (тип спрайта)
 */
void dino_draw_cloud(int col, uint32_t index);

/**
 * @brief      Затирание облака в указанном столбце
 * @param col  Столбец облака
 * @param index Индекс облака (тип спрайта)
 */
void dino_clear_cloud(int col, uint32_t index);

/* Obstacles */
/**
 * @brief             Начальная расстановка препятствий
 * @param  obstacles  Указатель на массив препятствий
 */
void dino_generate_obstacles(Obstacles *obstacles);

/**
 * @brief             Сдвиг препятствий
 * @param  obstacles  Указатель на массив препятствий
 */
void dino_update_obstacles(Obstacles *obstacles);

/**
 * @brief             Проверка столкновения динозавра с препятствием
 * @param  dino       Состояние динозавра
 * @param  obstacles  Указатель на массив препятствий
 * @return            1 — столкновение, 0 — нет
 */
bool dino_check_collision(Dino *dino, Obstacles *obstacles);

/**
 * @brief             Отрисовка препятствия с учётом типа
 * @param obstacles   Указатель на массив препятствий
 * @param index       Индекс препятствия
 */
void dino_draw_obstacle(const Obstacles *obstacles, uint32_t index);

/**
 * @brief             Очистка препятствия с учётом типа
 * @param obstacles   Указатель на массив препятствий
 * @param index       Индекс препятствия
 */
void dino_clear_obstacle(const Obstacles *obstacles, int index);

/* Frame */
/**
 * @brief             Отрисовка одного кадра игры
 * @param  dino       Состояние динозавра
 * @param  obstacles  Указатель на массив препятствий
 */
void dino_draw_frame(const Dino *dino, const Obstacles *obstacles);

#endif /* __DINO_GAME_H__ */
