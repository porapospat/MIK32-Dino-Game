/**
 * @file    dino_obstacles.c
 * @brief   Генерация движения, столкновения и отрисовка препятствий
 */

/* Includes ------------------------------------------------------------------*/
#include "dino_game.h"

/* Private functions ---------------------------------------------------------*/

/** @brief  Случайный тип препятствия */
static ObstacleKind dino_pick_obstacle_kind(void)
{
    switch (rand() % 3) {
    case 0:  return OBSTACLE_CACTUS;
    case 1:  return OBSTACLE_BIRD_HIGH;
    default: return OBSTACLE_BIRD_LOW;
    }
}

/**
 * @brief           Установка типа и дорожки препятствия
 * @param obstacles Указатель на массив препятствий
 * @param index     Индекс препятствия
 * @param kind      Тип препятствия
 */
static void dino_set_obstacle(Obstacles *obstacles, int index, ObstacleKind kind)
{
    obstacles->kind[index] = kind;
    obstacles->position_row[index] = (kind == OBSTACLE_CACTUS) ? 1 : 0;
}

/**
 * @brief      Вертикальное смещение низкой птицы
 * @param kind Тип препятствия
 */
static int16_t dino_bird_x_offset(ObstacleKind kind)
{
    return (kind == OBSTACLE_BIRD_LOW) ? DINO_BIRD_LOW_OFFSET : 0;
}

/**
 * @brief        Случайный кактус 32×32 по индексу препятствия
 * @param index  Индекс вида кактуса
 * @return       Вид кактуса соответствующий индексу
 */
static const uint8_t *dino_pick_cactus(int index)
{
    switch (index % 3) {
    case 1:  return cactus2;
    case 2:  return cactus3;
    default: return cactus1;
    }
}

/**
 * @brief             Проверка столкновения с одним препятствием
 * @param  dino       Состояние динозавра
 * @param  obstacles  Указатель на массив препятствий
 * @param  index      Индекс препятствия
 * @return            1 — столкновение, 0 — нет
 */
static bool dino_check_obstacle(const Dino *dino, const Obstacles *obstacles, int index)
{
    if (obstacles->position_col[index] != 0) return false;

    switch (obstacles->kind[index]) {
    case OBSTACLE_CACTUS:
        return dino->position_row == 1;

    case OBSTACLE_BIRD_HIGH:
        return dino->position_row == 0;

    case OBSTACLE_BIRD_LOW:
        return dino->position_row == 1 && !dino->down;

    default:
        return false;
    }
}

/* Exported functions --------------------------------------------------------*/
/**
 * @brief             Начальная расстановка препятствий
 * @param  obstacles  Указатель на массив препятствий
 */
void dino_generate_obstacles(Obstacles *obstacles)
{
    obstacles->position_col[0] = DINO_FIELD_COLS;
    dino_set_obstacle(obstacles, 0, dino_pick_obstacle_kind());

    for (int i = 1; i < MAX_OBSTACLES; i++) 
    {
        obstacles->position_col[i] = obstacles->position_col[i - 1] + MIN_OBSTACLE_DISTANCE;
        dino_set_obstacle(obstacles, i, dino_pick_obstacle_kind());
    }
}

/**
 * @brief             Сдвиг препятствий
 * @param  obstacles  Указатель на массив препятствий
 */
void dino_update_obstacles(Obstacles *obstacles)
{
    for (int i = 0; i < MAX_OBSTACLES; i++) 
    {
        obstacles->position_col[i]--;

        if (obstacles->position_col[i] < 0) 
        {
            dino_set_obstacle(obstacles, i, dino_pick_obstacle_kind());

            if (i == 0) 
            {
                obstacles->position_col[i] = obstacles->position_col[MAX_OBSTACLES - 1] + MIN_OBSTACLE_DISTANCE;
            } else 
            {
                obstacles->position_col[i] = obstacles->position_col[i - 1] + MIN_OBSTACLE_DISTANCE;
            }
        }
    }
}

/**
 * @brief             Проверка столкновения динозавра с препятствием
 * @param  dino       Состояние динозавра
 * @param  obstacles  Указатель на массив препятствий
 * @return            1 — столкновение, 0 — нет
 */
bool dino_check_collision(Dino *dino, Obstacles *obstacles)
{
    for (int i = 0; i < MAX_OBSTACLES; i++) 
    {
        if (dino_check_obstacle(dino, obstacles, i)) 
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief             Очистка препятствия с учётом типа
 * @param obstacles   Указатель на массив препятствий
 * @param index       Индекс препятствия
 */
void dino_clear_obstacle(const Obstacles *obstacles, int index)
{
    ObstacleKind kind = obstacles->kind[index];

    if (kind == OBSTACLE_CACTUS) 
    {
        dino_clear_sprite(obstacles->position_col[index], obstacles->position_row[index], DINO_SPRITE_W, DINO_SPRITE_H, 0);
        return;
    }

    dino_clear_sprite(obstacles->position_col[index], obstacles->position_row[index], DINO_BIRD_W, DINO_BIRD_H, dino_bird_x_offset(kind));
}

/**
 * @brief             Отрисовка препятствия с учётом типа
 * @param obstacles   Указатель на массив препятствий
 * @param index       Индекс препятствия
 */
void dino_draw_obstacle(const Obstacles *obstacles, uint32_t index)
{
    const uint8_t *bird;
    ObstacleKind kind = obstacles->kind[index];
    int col = obstacles->position_col[index];
    int row = obstacles->position_row[index];

    if (kind == OBSTACLE_CACTUS) 
    {
        dino_draw_sprite(col, row, DINO_SPRITE_W, DINO_SPRITE_H, dino_pick_cactus((int)index), 0);
        return;
    }

    if (kind == OBSTACLE_BIRD_LOW) 
    {
        bird = ((sDinoGame.frame + index) & 5) ? bird_bottom1 : bird_top1;
    } else 
    {
        bird = ((sDinoGame.frame + index) & 5) ? bird_bottom : bird_top;
    }

    dino_draw_sprite(col, row, DINO_BIRD_W, DINO_BIRD_H, bird, dino_bird_x_offset(kind));
}
