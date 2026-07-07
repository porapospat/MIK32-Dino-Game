/**
 * @file  main.c
 * @brief 
 */

/* Includes ------------------------------------------------------------------*/
#include "system_config.h"
#include "joystick.h"
#include "dino_game.h"
#include "lcd.h"

/* Private function prototypes -----------------------------------------------*/
static void periph_Init(void);

/* Private functions ---------------------------------------------------------*/
int main(void)
{
    SystemClock_Config();
    periph_Init();

    while (1) {
        if (DinoGame_Tick()) {
            while (!Joystick_IsPressed()) {
                HAL_DelayMs(50);
            }
            while (Joystick_IsPressed()) {
                HAL_DelayMs(50);
            }
            DinoGame_Reset();
        }
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
