/**
 * @file    joystick.h
 * @brief   Драйвер джойстика
 */

#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <stdbool.h>
#include <stdint.h>

#include "bgpio.h"
#include "bus_spi.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal_irq.h"

#define GPIO_JOY_BT   GPIO_0
#define JOY_BUTTON    GPIO_PIN_14

#define JOY_LOW       1000
#define JOY_HIGH      3000

typedef struct {
    uint16_t x;
    uint16_t y;
    bool pressed;
    bool x_error;
    bool y_error;
} JoystickState_t;

void Joystick_Init(void);
int Joystick_Read(JoystickState_t *state);
bool Joystick_IsPressed(void);

#endif /* __JOYSTICK_H__ */
