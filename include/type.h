/**
 * @file   type.h
 * @brief  
 */

#ifndef TYPES_H
#define TYPES_H

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>

/* Define --------------------------------------------------------------------*/
#define MAX_OBSTACLES         32    /* макс. количество препятствий в очереди */
#define MIN_OBSTACLE_DISTANCE 4     /* мин. расстояние между препятствиями, клетки */
#define DINO_FIELD_COLS       11    /* видимых столбцов (420 px / DINO_CELL_W) */
#define DINO_FIELD_ROWS       2     /* дорожки: 0 — воздух, 1 — земля */

/* Exported type -------------------------------------------------------------*/
/* Структура состояния динозаврика*/
typedef struct {
    int position_row; /* дорожка: 0 — воздух, 1 — земля */
    int position_col; /* столбец на поле (позиция игрока) */
    bool jump;        /* Флаг прыжка динозаврика */
    bool down;        /* Флаг приседа динозаврика */
} Dino;

/* Структура для отображения препятствий на поле */
typedef struct {
    int position_row[MAX_OBSTACLES]; /* Массив для хранения позиций препятствий в ряду */
    int position_col[MAX_OBSTACLES]; /* Массив для хранения позиций препятствий в столбце */
} Obstacles;

#endif /* __TYPES_H__ */
