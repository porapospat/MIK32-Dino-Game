/**
 * @file  main.c
 * @brief Игра Dino Game для микроконтроллера MIK32: управление через джойстик 
 *        с выводом на TFT-дисплей ILI9341
 */

/* Includes ------------------------------------------------------------------*/
#include "system_config.h"
#include "dino_game.h"

/* Private function prototypes -----------------------------------------------*/
static void periph_Init(void);

/* Private functions ---------------------------------------------------------*/
int main(void)
{
    SystemClock_Config();
    periph_Init();

    while (1) {
        DinoGame_Tick();
    }
}

/** @brief Инициализация переферии */
static void periph_Init(void)
{
    HAL_Init();
    GPIO_Init();

    SPI0_Init();
    SPI1_Init();

    Lcd_Init();
    DMA_Init();

    Joystick_Init();
    DinoGame_Init();
}
