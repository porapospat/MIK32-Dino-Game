/**
 * @file  main.c
 * @brief 
 */

#include "system_config.h"
#include "lcd.h"
#include "bus_spi.h"
#include "bdma.h"

#include "joystick.h"
#include "dino_game.h"

static void periph_Init(void);

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
