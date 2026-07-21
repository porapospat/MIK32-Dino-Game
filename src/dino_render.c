/**
 * @file    dino_render.c
 * @brief   Отрисовка поля, темы и облаков 
 */

/* Includes ------------------------------------------------------------------*/
#include "dino_game.h"

/* Private functions ---------------------------------------------------------*/
/**
 * @brief         Определение режима по текущему счёту (смена каждые 100 очков)
 * @param score   Текущий счёт
 * @return        Режим солнца или луны
 */
static DinoTheme dino_theme(uint32_t score_val)
{
    return ((score_val / DINO_THEME_SCORE) % 2) ? DINO_THEME_MOON : DINO_THEME_SUN;
}

/**
 * @brief       Координаты клетки игрового поля
 * @param col   Колонка спрайта
 * @param row   Дорожка спрайта
 * @param x     Координата по горизонтали
 * @param y     Координата по вертикали
 */
static void dino_cell_to_pixel(int col, int row, uint16_t *x, uint16_t *y)
{
    grid_cell_to_pixel(col, row, DINO_ORIGIN_Y, DINO_CELL_W, DINO_SPRITE_SZ, 1, DINO_GROUND_FEET_X, DINO_AIR_X, x, y);
}

/**
 * @brief          Формирование строки счёта
 * @param  buf     Буфер результата
 * @param  score   Текущий счёт
 */
static void dino_score(char *buf, uint32_t score_val)
{
    for (int i = 4; i >= 0; i--) 
    {
        buf[i] = (char)('0' + (score_val % 10));
        score_val /= 10;
    }

    buf[5] = '\0';
}

/**
 * @brief       Координата облака вдоль направления бега
 * @param col   Столбец облака
 * @param width Ширина спрайта облака
 * @return      Координата y
 */
static uint16_t dino_cloud_y(int col, uint16_t width)
{
    return (uint16_t)(DINO_ORIGIN_Y + (uint16_t)col * DINO_CELL_W + (DINO_CELL_W - width) / 2);
}

/**
 * @brief        Ширина облака по индексу
 * @param index  Индекс облака
 * @return       Ширина спрайта в пикселях
 */
static uint16_t dino_cloud_w(int index)
{
    return (index % 2) ? DINO_BIG_CLOUD_W : DINO_SMALL_CLOUD_W;
}

/** @brief  Случайное расстояние до следующего облака, клетки */
static int dino_pick_cloud_distance(void)
{
    return MIN_CLOUD_DISTANCE + (rand() % (MAX_CLOUD_DISTANCE - MIN_CLOUD_DISTANCE + 1));
}

/** @brief  Перерисовка игровой зоны при смене режима */
static void dino_draw_playfield(void)
{
    uint16_t play_w = (uint16_t)(DINO_AIR_X + DINO_SPRITE_SZ + 4 - DINO_GROUND_FEET_X);
    uint16_t play_l = (uint16_t)(DINO_FIELD_COLS * DINO_CELL_W);

    ClearMassDMA_Fast(play_l, play_w, DINO_ORIGIN_Y, DINO_GROUND_FEET_X, sDinoGame.color_bg);
}

/**
 * @brief        Случайное облако по индексу 
 * @param index  Индекс вида облака
 * @return       Вид облака соответствующий индексу
 */
static const uint8_t *dino_pick_cloud(int index)
{
    switch (index % 2) {
    case 1:  return cloud2;
    case 2:  return cloud3;
    default: return cloud1;
    }
}

/* Exported functions --------------------------------------------------------*/
/**
 * @brief         Применение цветов соответствующего режима
 * @param theme   Режим солнца или луны
 */
void dino_apply_theme(DinoTheme theme)
{
    if (theme == DINO_THEME_MOON) 
    {
        sDinoGame.color_fg = DINO_COLOR_WHITE;
        sDinoGame.color_bg = DINO_COLOR_BLACK;
    } else 
    {
        sDinoGame.color_fg = DINO_COLOR_BLACK;
        sDinoGame.color_bg = DINO_COLOR_WHITE;
    }

    sDinoGame.active_theme = theme;
}

/** @brief  Отрисовка иконки солнца или луны */
void dino_draw_icon(void)
{
    const uint8_t *icon = (sDinoGame.active_theme == DINO_THEME_MOON) ? moon : sun;
    draw_bitmap(DINO_ICON_X, 430, icon, DINO_SPRITE_W, DINO_SPRITE_H, sDinoGame.color_fg, sDinoGame.color_bg, DINO_SPRITE_SCALE);
}

/**
 * @brief       Затирание облака в указанном столбце
 * @param col   Столбец облака
 * @param index Индекс облака (тип спрайта)
 */
void dino_clear_cloud(int col, uint32_t index)
{
    uint16_t width = dino_cloud_w((int)index);

    if ((col < 0) || (col >= DINO_FIELD_COLS)) return;

    ClearMassDMA_Fast((uint16_t)(width + 1), (uint16_t)(DINO_CLOUD_H + 1), dino_cloud_y(col, width), DINO_CLOUD_X, sDinoGame.color_bg);
}

/**
 * @brief       Затирание только «хвоста» облака после сдвига на 1 клетку
 * @param col   Столбец облака до сдвига
 * @param index Индекс облака (тип спрайта)
 */
static void dino_clear_cloud_tail(int col, uint32_t index)
{
    uint16_t width = dino_cloud_w((int)index);
    uint16_t old;
    uint16_t tail;

    if ((col < 0) || (col >= DINO_FIELD_COLS)) return;

    old = dino_cloud_y(col, width);
    /* правый край старого спрайта длиной в один шаг клетки */
    tail = (uint16_t)(old + width + 1 - DINO_CELL_W);
    ClearMassDMA_Fast(DINO_CELL_W, (uint16_t)(DINO_CLOUD_H + 1), tail, DINO_CLOUD_X, sDinoGame.color_bg);
}

/**
 * @brief       Отрисовка облака в указанном столбце (ниже уровня иконок)
 * @param col   Столбец облака
 * @param index Индекс облака 
 */
void dino_draw_cloud(int col, uint32_t index)
{
    uint16_t width = dino_cloud_w((int)index);

    if ((col < 0) || (col >= DINO_FIELD_COLS)) return;

    draw_bitmap(DINO_CLOUD_X, dino_cloud_y(col, width), dino_pick_cloud((int)index), width, DINO_CLOUD_H, sDinoGame.color_fg, sDinoGame.color_bg, DINO_SPRITE_SCALE);
}

/**
 * @brief       Перерисовка облака только при сдвиге, clear — хвост или полный
 * @param index Индекс облака (тип облака)
 */
static void dino_redraw_cloud(uint32_t index)
{
    int prev = sDinoGame.prev_cloud_col[index];
    int cur = sDinoGame.cloud_col[index];

    if (!sDinoGame.prev_frame) 
    {
        /* первый кадр / смена темы — полная отрисовка */
        dino_draw_cloud(cur, index);
    } else if (cur != prev) 
    {
        if (prev == cur + 1) {
            /* обычный сдвиг на 1 клетку — затереть только хвост */
            dino_clear_cloud_tail(prev, index);
        } else {
            /* скачок — полное затирание старой позиции */
            dino_clear_cloud(prev, index);
        }
        dino_draw_cloud(cur, index);
    }

    sDinoGame.prev_cloud_col[index] = cur;
}

/** @brief  Сдвиг облаков на один шаг */
void dino_update_clouds(void)
{
    /* облака двигаются медленнее: шаг раз в DINO_CLOUD_PERIOD кадров */
    if ((sDinoGame.frame % DINO_CLOUD_PERIOD) != 0) {
        return;
    }

    for (int i = 0; i < MAX_CLOUDS; i++) 
    {
        sDinoGame.cloud_col[i]--;

        if (sDinoGame.cloud_col[i] < 0) 
        {
            int ref = (i == 0) ? (MAX_CLOUDS - 1) : (i - 1);
            sDinoGame.cloud_col[i] = sDinoGame.cloud_col[ref] + dino_pick_cloud_distance();
        }
    }
}

/** @brief  Начальная расстановка облаков */
void dino_generate_clouds(void)
{
    sDinoGame.cloud_col[0] = DINO_FIELD_COLS+1; // отрисовка на одну колонку раньше, чтобы не перекрывать иконку
    for (int i = 1; i < MAX_CLOUDS; i++) 
    {
        sDinoGame.cloud_col[i] = sDinoGame.cloud_col[i - 1] + dino_pick_cloud_distance();
    }

    for (int i = 0; i < MAX_CLOUDS; i++) 
    {
        sDinoGame.prev_cloud_col[i] = sDinoGame.cloud_col[i];
    }
}

/**
 * @brief  Отрисовка статических элементов: дорога, счёт и иконка (луны или солнца)
 *         — рисуются один раз при старте и рестарте
 */
void dino_draw_static(void)
{
    /* Отрисовка счёта */
    drawText(20, 20, "SCORE:", sDinoGame.color_fg, sDinoGame.color_bg, DINO_FONT_SCALE);
    /* Отрисовка иконки режима луна или солнце */
    dino_draw_icon();
    /* Отрисовка дороги */
    ClearMassDMA_Fast(DINO_GROUND_LENGHT, DINO_GROUND_LINE_W, DINO_GROUND_LINE_Y, DINO_GROUND_LINE_X, sDinoGame.color_fg);
}

/**
 * @brief  Смена режима солнце/луна при переходе через каждые 100 очков
 * @return true — режим изменился и экран перерисован
 */
bool dino_update_theme(void)
{
    DinoTheme new_theme = dino_theme(sDinoGame.score);

    if (new_theme == sDinoGame.active_theme) return false;

    dino_apply_theme(new_theme);
    ClearMassDMA_Fast(TFT_WIDTH, TFT_HEIGHT, 0, 0, sDinoGame.color_bg);
    dino_draw_static();
    dino_draw_playfield();
    sDinoGame.prev_frame = false;
    return true;
}

/**
 * @brief         Отрисовка спрайта в клетке игрового поля
 * @param col     Колонка
 * @param row     Дорожка
 * @param width   Ширина
 * @param height  Длина
 * @param bmp     Указатель на массив спрайта
 * @param x_off   Смещение по вертикали
 */
void dino_draw_sprite(int col, int row, int width, int height,
                      const uint8_t *bmp, int16_t x_off)
{
    uint16_t x, y;

    dino_cell_to_pixel(col, row, &x, &y);
    draw_bitmap((uint16_t)((int)x + x_off), y, bmp, width, height, sDinoGame.color_fg, sDinoGame.color_bg, DINO_SPRITE_SCALE);
}

/**
 * @brief         Очистка спрайта в клетке игрового поля
 * @param col     Колонка
 * @param row     Дорожка
 * @param width   Ширина
 * @param height  Длина
 * @param x_off   Смещение по вертикали
 */
void dino_clear_sprite(int col, int row, int width, int height, int16_t x_off)
{
    uint16_t x, y;

    if ((col < 0) || (col >= DINO_FIELD_COLS)) return;
    
    dino_cell_to_pixel(col, row, &x, &y);
    ClearMassDMA_Fast((uint16_t)(width + 1), (uint16_t)(height + 1), y, (uint16_t)((int)x + x_off), sDinoGame.color_bg);
}

/** @brief  Отрисовка счёта в нижней части экрана (только при изменении) */
void dino_draw_score(void)
{
    char score_buf[6]; /* строка SCORE: + 00000 */

    if (sDinoGame.score == sDinoGame.last_score) return;

    dino_score(score_buf, sDinoGame.score);
    drawText(20, 100, score_buf, sDinoGame.color_fg, sDinoGame.color_bg, DINO_FONT_SCALE);
    sDinoGame.last_score = sDinoGame.score;
}

/** @brief  Экран Game Over со счётом */
void dino_game_over(void)
{
    /* Отрисовка проигравшего динозаврика */
    dino_draw_sprite(sDinoGame.dino.position_col, sDinoGame.dino.position_row, DINO_SPRITE_W, DINO_SPRITE_H, dino_dead, 0);
    /* Вывод подписи GAME OVER */
    drawText(270, 150, "GAME OVER", DINO_COLOR_GO, sDinoGame.color_bg, DINO_FONT_SCALE + 1);
    /* Отрисовка счётчика игры */
    dino_draw_score();
    /* Вывод подписи с предложением рестарта игры */
    drawText(230, 155, "BTN = RESTART", sDinoGame.color_fg, sDinoGame.color_bg, DINO_FONT_SCALE);
}

/**
 * @brief             Отрисовка одного кадра игры
 * @param  dino       Состояние динозавра
 * @param  obstacles  Указатель на массив препятствий
 */
void dino_draw_frame(const Dino *dino, const Obstacles *obstacles)
{
    const uint8_t *dino_sprite;

    dino_update_theme();
    /* Отрисовка счётчика игры */
    dino_draw_score();

    /* Затираем только старые позиции прошлого кадра */
    if (sDinoGame.prev_frame) {
        if (sDinoGame.prev_dino.down) 
        {
            dino_clear_sprite(sDinoGame.prev_dino.position_col, sDinoGame.prev_dino.position_row, DINO_SPRITE_W, DINO_DUCK_H, 0);
        } else 
        {
            dino_clear_sprite(sDinoGame.prev_dino.position_col, sDinoGame.prev_dino.position_row, DINO_SPRITE_W, DINO_SPRITE_H, 0);
        }

        for (int i = 0; i < MAX_OBSTACLES; i++) 
        {
            dino_clear_obstacle(&sDinoGame.prev_obstacles, i);
        }
        /* облака чистятся точечно в dino_redraw_cloud — только при сдвиге */
    }

    /* Отрисовка положения динозаврика в соответствии от ситуации */
    if (!dino->jump && !dino->down && dino->position_row == 1) 
    {
        dino_sprite = (sDinoGame.frame & 1) ? dino_run2x : dino_run1x;
        dino_draw_sprite(dino->position_col, dino->position_row, DINO_SPRITE_W, DINO_SPRITE_H, dino_sprite, 0);
    } 
    else if (dino->jump && dino->position_row == 0) 
    {
        dino_draw_sprite(dino->position_col, dino->position_row, DINO_SPRITE_W, DINO_SPRITE_H, dino_run2x, 0);
    } 
    else if (dino->down && dino->position_row == 1)
    {
        dino_sprite = (sDinoGame.frame & 1) ? dino_duck2x : dino_duck1x;
        dino_draw_sprite(dino->position_col, dino->position_row, DINO_SPRITE_W, DINO_DUCK_H, dino_sprite, 0);
    } 
    else 
    {
        dino_draw_sprite(dino->position_col, dino->position_row, DINO_SPRITE_W, DINO_SPRITE_H, dino_run1x, 0);
    }

    /* Отрисовка препятствий — кактусы и птеродактили */
    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        if (obstacles->position_col[i] < DINO_FIELD_COLS) 
        {
            dino_draw_obstacle(obstacles, i);
        }
    }

    /* Отрисовка облаков */
    for (int i = 0; i < MAX_CLOUDS; i++)
    {
        dino_redraw_cloud((uint32_t)i);
    }
    dino_draw_icon();

    sDinoGame.prev_dino = *dino;
    sDinoGame.prev_obstacles = *obstacles;
    sDinoGame.prev_frame = true;
}
