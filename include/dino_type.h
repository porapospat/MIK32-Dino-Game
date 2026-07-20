/**
 * @file   dino_type.h
 * @brief  
 */

#ifndef TYPES_H
#define TYPES_H

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

/* Define --------------------------------------------------------------------*/
#define MAX_OBSTACLES         6     /* макс. количество препятствий в очереди */
#define MIN_OBSTACLE_DISTANCE 4     /* мин. расстояние между препятствиями, клетки */
#define MAX_CLOUDS            2     /* количество облаков на экране */
#define MIN_CLOUD_DISTANCE    5     /* мин. расстояние между облаками, клетки */
#define DINO_FIELD_COLS       11    /* видимых столбцов (420 px / DINO_CELL_W) */
#define DINO_FIELD_ROWS       2     /* дорожки: 0 — воздух, 1 — земля */

/* Exported type -------------------------------------------------------------*/
/* Структура состояния динозаврика*/
typedef struct {
    int position_row; /* дорожка: 0 — воздух, 1 — земля */
    int position_col; /* столбец на поле (позиция игрока) */
    bool jump;        /* Флаг прыжка динозаврика */
    bool down;        /* Флаг приседа динозаврика */
}Dino;

/* Тип препятствия */
typedef enum {
    OBSTACLE_CACTUS = 0,     /* кактус на земле — перепрыгивать */
    OBSTACLE_BIRD_HIGH = 1,  /* птица высоко — не прыгать */
    OBSTACLE_BIRD_LOW = 2,   /* птица низко — пригнуться */
} ObstacleKind;

/* Структура для отображения препятствий на поле */
typedef struct {
    int position_row[MAX_OBSTACLES];      /* дорожка препятствия */
    int position_col[MAX_OBSTACLES];      /* столбец препятствия */
    ObstacleKind kind[MAX_OBSTACLES];     /* тип препятствия */
}Obstacles;

/* Режимы игры*/
typedef enum {
    DINO_THEME_SUN = 0,             /* Дневной режим */
    DINO_THEME_MOON = 1,            /* Ночной режим */
}DinoTheme;

/* Состояния автомата игры */
typedef enum{
    DINO_STATE_PLAYING,              /* обычный игровой процесс */
    DINO_STATE_GAME_OVER_WAIT_PRESS, /* обработчик конца игры, ожидание нажатия кнопки рестарта */
    DINO_STATE_GAME_OVER             /* обработчик конца игры, ожидание отпускания кнопки*/
} GameState;

/* Струкутра игры*/
typedef struct{
    Dino dino;                       /* состояние динозаврика */
    Obstacles obstacles;             /* очередь препятствий на поле */
    DinoTheme active_theme;          /* текущий режим (день и ночь) */
    GameState state;                 /* текущее состояние игры */
    
    uint16_t color_fg;               /* цвет спрайтов */
    uint16_t color_bg;               /* цвет фона */
    uint32_t score;                  /* счётчик очков */
    uint32_t last_score;             /* последний отрисованный счёт */
    uint32_t frame;                  /* номер кадра */
    uint32_t delay_ms;               /* пауза между кадрами */

    Dino prev_dino;                  /* позиция динозавра на прошлом кадре  */
    Obstacles prev_obstacles;        /* позиции препятствий на прошлом кадре  */
    int cloud_col[MAX_CLOUDS];       /* столбцы облаков (движутся как препятствия) */
    int prev_cloud_col[MAX_CLOUDS];  /* столбцы облаков на прошлом кадре */
    bool prev_frame;                 /* есть ли данные прошлого кадра для затирания */

}DinoGame;

#endif /* __TYPES_H__ */
