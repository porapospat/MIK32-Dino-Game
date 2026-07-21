/**
 * @file    dino_obstacles.c
 * @brief   Генерация движения, столкновения и отрисовка препятствий
 */

/* Includes ------------------------------------------------------------------*/
#include "dino_game.h"

/* Private functions ---------------------------------------------------------*/

/** @brief  Выбор случайного типа препятствия */
static ObstacleKind dino_rand_pick_obstacle(void)
{
    int r = rand() % 10;

    if (r < 6) {
        return OBSTACLE_CACTUS;      /* 60% */
    }
    if (r < 8) {
        return OBSTACLE_BIRD_HIGH;   /* 20% */
    }
    return OBSTACLE_BIRD_LOW;        /* 20% */
}

/**
 * @brief         Выбор типа препятствия без трёх одинаковых подряд
 * @param  second Предыдущее препятствие
 * @param  first  Предпредыдущее препятствие
 * @return        Тип препятствия
 */
static ObstacleKind dino_pick_obstacle(ObstacleKind first, ObstacleKind second)
{
    ObstacleKind kind = dino_rand_pick_obstacle();

    /* если уже было два одинаковых подряд препятствия — третье должно отличаться */
    if ((second == first) && (kind == second)) {
        do {
            kind = dino_rand_pick_obstacle();
        } while (kind == second);
    }

    return kind;
}

/** @brief  Выбор случайного расстояния до следующего препятствия, клетки */
static int dino_pick_obstacle_distance(void)
{
    return MIN_OBSTACLE_DISTANCE + (rand() % (MAX_OBSTACLE_DISTANCE - MIN_OBSTACLE_DISTANCE + 1));
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
    /* если птица - низколетящая, тогда отрисовка со смещением вниз по вертикали, иначе смещения нет */
    return (kind == OBSTACLE_BIRD_LOW) ? DINO_BIRD_LOW_OFFSET : 0;
}

/**
 * @brief        Выбро случайного кактуса 32×32 по индексу препятствия
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
        return dino->position_row == 1;                 /* препятствие на замле, ряд = 1 */

    case OBSTACLE_BIRD_HIGH:
        return dino->position_row == 0;                 /* препятствие в воздухе, ряд = 0 */

    case OBSTACLE_BIRD_LOW:
        return dino->position_row == 1 && !dino->down;  /* препятствие на замле, ряд = 1 */

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
    dino_set_obstacle(obstacles, 0, dino_rand_pick_obstacle());

    for (int i = 1; i < MAX_OBSTACLES; i++) 
    {
        ObstacleKind kind;
        obstacles->position_col[i] = obstacles->position_col[i - 1] + dino_pick_obstacle_distance();
        
        if (i >= 2) {
            /* проверка на частый повтор одинаковых препятствий, после двух уже прошедших */
            kind = dino_pick_obstacle(obstacles->kind[i - 1], obstacles->kind[i - 2]);
        } else {
            kind = dino_rand_pick_obstacle();
        }

        dino_set_obstacle(obstacles, i, kind);
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
        /* сдвиг препятствия */
        obstacles->position_col[i]--; 

         /* когда препятствие i ушло за левый край экрана (т.е. position_col < 0),
          * его нужно пересоздать справа — с новым типом */
        if (obstacles->position_col[i] < 0) 
        {
            /* УСТАНОВКА ТИПА ПРЕПЯТСТВИЯ */

            /* если индекс равен нулю тогда значение предыдущего (последнего) препятствия принимаем за 5, 
             * иначе принимаем значение вследыдущего (i - i: предпредыдущий) */
            int prev = (i == 0) ? (MAX_OBSTACLES - 1) : (i - 1);
             
            /* если значение предыдущего препятствия равно нулю, тогда значение предпредыдущего равен 5,
             * иначе значение предпредыдущего = предыдущий - 1 */
            int before_prev = (prev == 0) ? (MAX_OBSTACLES - 1) : (prev - 1);
            ObstacleKind kind = dino_pick_obstacle(obstacles->kind[prev], obstacles->kind[before_prev]);
            dino_set_obstacle(obstacles, i, kind);

            /* УСТАНОВКА ПОЗИЦИИ ПРЕПЯТСТВИЯ */

            /* позиция нового препятствия = позиция предыдущего + случайный зазор;
             * если индекс равен нулю тогда значение предыдущего — последнее в массиве = obstacles->position_col[MAX_OBSTACLES - 1],
             * иначе принимаем значение предпредыдущего = obstacles->position_col[i - 1] */
            if (i == 0)
            {
                obstacles->position_col[i] = obstacles->position_col[MAX_OBSTACLES - 1] + dino_pick_obstacle_distance();
            }
            else
            {
                obstacles->position_col[i] = obstacles->position_col[i - 1] + dino_pick_obstacle_distance();
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
        if (dino_check_obstacle(dino, obstacles, i)) return true;        
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
        /* затирание спрайта кактуса */
        dino_clear_sprite(obstacles->position_col[index], obstacles->position_row[index], DINO_SPRITE_W, DINO_SPRITE_H, 0);
        return;
    }
    /* затирание спрайта птеродактиля с учётом смещения*/
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
        /* отрисовка кактуса */
        dino_draw_sprite(col, row, DINO_SPRITE_W, DINO_SPRITE_H, dino_pick_cactus((int)index), 0);
        return;
    }

    if (kind == OBSTACLE_BIRD_LOW) 
    {
        /* установка положения низколетящего птеродактиля */
        bird = ((sDinoGame.frame + index) & 5) ? bird_bottom1 : bird_top1;
    } else 
    {
        /* установка положения высоколетящего птеродактиля */
        bird = ((sDinoGame.frame + index) & 5) ? bird_bottom : bird_top;
    }

    /* отрисовка низколетящего/высоколетящего птеродактиля */
    dino_draw_sprite(col, row, DINO_BIRD_W, DINO_BIRD_H, bird, dino_bird_x_offset(kind));
}
