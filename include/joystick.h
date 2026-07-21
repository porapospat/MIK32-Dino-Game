/**
 * @file    joystick.h
 * @brief   Драйвер джойстика
 */

#ifndef JOYSTICK_H
#define JOYSTICK_H

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

#include "bgpio.h"
#include "bus_spi.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal_irq.h"

/* Define --------------------------------------------------------------------*/
/* GPIO кнопки джойстика */
#define GPIO_JOY_BT   GPIO_0
#define JOY_BUTTON    GPIO_PIN_14

/* Пороги направлений XY */
#define JOY_LOW       1000
#define JOY_HIGH      3000

/* пауза для нажатий на кнопку джойстика против дребезга */
#define JOY_DEBOUNCE_MS   30   
/* пауза между чтением осей АЦП */
#define JOY_ADC_PAUSE_MS  1    

/* Exported variables --------------------------------------------------------*/

/* Exported type --------------------------------------------------------*/
/** @brief Структуа для описания состояния джойстика */
typedef struct {
    uint16_t x;
    uint16_t y;
    bool pressed;
    bool x_error;
    bool y_error;
}JoystickState;

/* Exported function -----------------------------------------------------------*/

/**
 * @brief  Инициализация GPIO кнопки джойстика и прерываний
 */
void Joystick_Init(void);

/**
 * @brief         Чтение обеих осей и состояния кнопки
 * @param  state  Указатель на структуру результата
 * @return        1 — чтение выполнено, иначе 0 
 */
int Joystick_Read(JoystickState *state);

/**
 * @brief  Проверка нажатия кнопки джойстика (с проверкой на дребезг)
 * @return true — кнопка нажата, false — отпущена
 */
bool Joystick_IsPressed(void);

#endif /* __JOYSTICK_H__ */
