/**
 * @file    joystick.c
 * @brief   Драйвер джойстика
 */

/* Includes ------------------------------------------------------------------*/
#include "joystick.h"

/* Private variables ---------------------------------------------------------*/
/* Команды SPI: канал 6 — ось X, канал 7 — ось Y внешнего АЦП */
static const uint8_t master_output_ch6[] = {0b00000111, 0b10000000, 0};
static const uint8_t master_output_ch7[] = {0b00000111, 0b11000000, 0};

/* состояние кнопки */
static uint8_t joy_button = 0;

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Выбор внешнего АЦП джойстика на шине SPI (CS0)
 */
static void joystick_adc_select(void)
{
    HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS_ALL, __LOW);    // сброс всех CS
    HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS0, __HIGH);      // выбор АЦП джойстика (CS0)
}

/**
 * @brief  Снятие выбора внешнего АЦП
 */
static void joystick_adc_unselect(void)
{
    HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS_ALL, __LOW);  // снять выбор со всех устройств
}

/**
 * @brief           Чтение одной оси джойстика по SPI
 * @param  tx_buf   Буфер команды 
 * @param  rx_buf   Буфер приёма 3 байт ответа
 * @return          12-битное значение оси
 */
static uint16_t joystick_read_axis(const uint8_t *tx_buf, uint8_t *rx_buf)
{
    HAL_StatusTypeDef SPI_Status;

    __HAL_SPI_ENABLE(&hspi1);
    joystick_adc_select();  

    /* Передача и приём данных */
    SPI_Status = HAL_SPI_Exchange(&hspi1, (uint8_t *)tx_buf, rx_buf, 3, SPI_TIMEOUT_DEFAULT);
    if (SPI_Status != HAL_OK) {
        HAL_SPI_ClearError(&hspi1);
    }

    joystick_adc_unselect(); // Выбрать внешний АЦП CS
    __HAL_SPI_DISABLE(&hspi1);

    /* Сборка 12 бит: младший байт — rx[0], старшие 4 бита — rx[1] */
    return ((uint16_t)((rx_buf[1] << 8) | rx_buf[0])) & 0x0FFF;
}

/**
 * @brief  Инициализация GPIO кнопки джойстика и прерываний
 */
void Joystick_Init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_IRQ_CLK_ENABLE();

    gpio.Pin = JOY_BUTTON;
    gpio.Mode = HAL_GPIO_MODE_GPIO_INPUT;
    gpio.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_JOY_BT, &gpio);

    HAL_GPIO_InitInterruptLine(GPIO_MUX_PORT0_14_LINE_6, GPIO_INT_MODE_CHANGE);  // ENC_But
    HAL_EPIC_MaskLevelSet(HAL_EPIC_GPIO_IRQ_MASK);
    HAL_IRQ_EnableInterrupts();
}

/**
 * @brief  Проверка нажатия кнопки джойстика
 * @return true — кнопка нажата, false — отпущена
 */
bool Joystick_IsPressed(void)
{
    return joy_button != 0;
}

/**
 * @brief         Чтение обеих осей и состояния кнопки
 * @param  state  Указатель на структуру результата
 * @return        1 — чтение выполнено, 0 — state == NULL
 */
int Joystick_Read(JoystickState_t *state)
{
    uint8_t rx_x[3] = {0};
    uint8_t rx_y[3] = {0};

    if (state == NULL) {
        return 0;
    }

    state->x = joystick_read_axis(master_output_ch6, rx_x);
    for (uint32_t i = 0; i < 1000; i++);
    state->y = joystick_read_axis(master_output_ch7, rx_y);
    state->x_error = false;
    state->y_error = false;
    state->pressed = Joystick_IsPressed();
    return 1;
}

/**
 * @brief  Обработчик прерывания кнопки джойстика 
 */
void trap_handler(void)
{
    if (!EPIC_CHECK_GPIO_IRQ()) {
        return;
    }

    if (HAL_GPIO_LineInterruptState(GPIO_LINE_6)) {
        joy_button = (joy_button == 0) ? 1 : 0;  // переключить состояние
        GPIO_IRQ->CLEAR = 1 << 6;
    }
}
