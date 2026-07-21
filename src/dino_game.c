/**
 * @file    dino_game.c
 * @brief   Автомат состояний и игровой цикл Dino Game
 */

/* Includes ------------------------------------------------------------------*/
#include "dino_game.h"

/* Private variables ---------------------------------------------------------*/
DinoGame sDinoGame;

/* Private functions ---------------------------------------------------------*/
/**
 * @brief         Обработка джойстика: прыжок / присед / бег
 * @param  joy    Состояние джойстика
 */
static void dino_apply_joystick(const JoystickState *joy)
{
    if (joy->x < JOY_LOW) {          /* динозаврик в прыжке */
        sDinoGame.dino.jump = true;
        sDinoGame.dino.down = false;
        sDinoGame.dino.position_row = 0;
    } else if (joy->x > JOY_HIGH) {  /* динозаврик в приседе */
        sDinoGame.dino.jump = false;
        sDinoGame.dino.down = true;
        sDinoGame.dino.position_row = 1;
    } else {                         /* динозаврик в забеге */
        sDinoGame.dino.jump = false;
        sDinoGame.dino.down = false;
        sDinoGame.dino.position_row = 1;
    }
}

/** @brief  Сброс состояния игры */
static void dino_reset_state(void)
{
    sDinoGame.dino.position_row = 1;
    sDinoGame.dino.position_col = 0;
    sDinoGame.dino.jump = false;
    sDinoGame.dino.down = false;
    sDinoGame.score = 0;
    sDinoGame.frame = 0;
    sDinoGame.delay_ms = DINO_DELAY_START;
    sDinoGame.prev_frame = false;
    sDinoGame.state = DINO_STATE_PLAYING;
    dino_apply_theme(DINO_THEME_SUN);
    dino_generate_obstacles(&sDinoGame.obstacles);
    dino_generate_clouds();
}

/* Exported functions --------------------------------------------------------*/
/** @brief  Инициализация игры — заставка и первый кадр */
void DinoGame_Init(void)
{
    /* Сброс состояния игры */
    dino_reset_state();
    /* Очистка экрана */
    ClearMassDMA_Fast(TFT_WIDTH, TFT_HEIGHT, 0, 0, sDinoGame.color_bg);
    /* Вывод начала игры */
    drawText(270, 150, "DINO GAME", sDinoGame.color_fg, sDinoGame.color_bg, DINO_FONT_SCALE + 1);
    HAL_DelayMs(1000);
    /* Отрисовка статических элементов (дорога) */
    dino_draw_static();
    /* Отрисовка игрового поля */
    dino_draw_frame(&sDinoGame.dino, &sDinoGame.obstacles);
}

/** @brief  Сброс счёта, препятствий и перерисовка поля */
void DinoGame_Reset(void)
{
    /* Сброс */
    dino_reset_state();
    /* Очистка экрана */
    ClearMassDMA_Fast(TFT_WIDTH, TFT_HEIGHT, 0, 0, sDinoGame.color_bg);
    /* Отрисовка статических элементов (дорога, иконка луны или солнца) */
    dino_draw_static();
    /* Отрисовка игрового поля */
    dino_draw_frame(&sDinoGame.dino, &sDinoGame.obstacles);
}

/** @brief  Один шаг автомата игры: playing/game over/restart */
void DinoGame_Tick(void)
{
    switch (sDinoGame.state) {
    case DINO_STATE_PLAYING: {
        JoystickState joy;

        /* Чтение положения джойстика и состояния кнопки */
        Joystick_Read(&joy);
        /* Обработка джойстика */
        dino_apply_joystick(&joy);
        /* Перемещение препятствий и облаков */
        dino_update_obstacles(&sDinoGame.obstacles);
        dino_update_clouds();
        /* Отрисовка игрового поля */
        dino_draw_frame(&sDinoGame.dino, &sDinoGame.obstacles);

        /* Если динозаврик попал в препятствие */
        if (dino_check_collision(&sDinoGame.dino, &sDinoGame.obstacles)) 
        {
            dino_game_over();
            sDinoGame.state = DINO_STATE_GAME_OVER_WAIT_PRESS;
            return;
        }

        sDinoGame.score++;
        sDinoGame.frame++;

        if (sDinoGame.delay_ms > DINO_DELAY_STEP) sDinoGame.delay_ms--;

        HAL_DelayMs(sDinoGame.delay_ms); /* Задержка между кадрами */
        break;
    }

    case DINO_STATE_GAME_OVER_WAIT_PRESS:
        if (Joystick_IsPressed()) 
        {
            sDinoGame.state = DINO_STATE_GAME_OVER;
            HAL_DelayMs(50);
        }
        break;

    case DINO_STATE_GAME_OVER:
        if (!Joystick_IsPressed()) 
        {
            DinoGame_Reset();
            HAL_DelayMs(50);
        }
        break;

    default:
        sDinoGame.state = DINO_STATE_PLAYING;
        break;
    }
}
