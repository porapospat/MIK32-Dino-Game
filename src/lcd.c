/**
 * @file    lcd.c
 * @brief   Реализация драйвера TFT-дисплея на базе ILI9341 с сенсорной панелью:
 *          инициализация и базовая отрисовка.
 */

/* Includes ------------------------------------------------------------------*/
#include "lcd.h"

/* Private variables ---------------------------------------------------------*/
extern SPI_HandleTypeDef hspi0;

/* Private function prototypes -----------------------------------------------*/
static void Lcd_select(void);
static void Lcd_unselect(void);
static void Lcd_Writ_Bus(uint8_t d);
static void Lcd_Write_Command(unsigned char command);
static void Lcd_Write_Data(uint8_t data);
static void Address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
static void Lcd_Reset(void);
static void Lcd_Draw_Color(unsigned int color);
static uint16_t grid_lane_x(int row, int ground_row, uint16_t ground_x, uint16_t air_x);
static void ClearMassDMA(uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w, uint16_t color);
static void lcd_dma_end(void);
static void lcd_dma_send_row(uint16_t width);

/* Private functions ---------------------------------------------------------*/
/** @brief  Активация дисплея (CS = 0) */
static void Lcd_select(void) {
    HAL_GPIO_WritePin(GPIO_0, CS, GPIO_PIN_LOW);
}

/**  @brief  Деактивация дисплея (CS = 1) */
static void Lcd_unselect(void) {
    HAL_GPIO_WritePin(GPIO_0, CS, GPIO_PIN_HIGH);
}

/** @brief  Отправка 1 байт данных или команды */
static void Lcd_Writ_Bus(uint8_t d) {
    uint8_t out[] = {d};
    uint8_t in[sizeof(out)];
    /* одновременная передача и приём(стандартная опирация SPI) */
    HAL_SPI_Exchange(&hspi0, out, in, sizeof(out), SPI_TIMEOUT_DEFAULT); 
}

/** @brief Отправка команды (RS = 0) */
static void Lcd_Write_Command(unsigned char command) { 
    HAL_GPIO_WritePin(GPIO_0, RS, 0);   
    Lcd_Writ_Bus(command);
}

/** @brief Отправка данных (RS = 1) */
static void Lcd_Write_Data(uint8_t data) { 
    HAL_GPIO_WritePin(GPIO_0, RS, 1);   
    Lcd_Writ_Bus(data);
}

/** @brief Установка области памяти LCD, в которую будут записываться данные */
static void Address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) { 
    //(x1, y1) - левый нижний угл, (x2, y2) - правый верхний угол
    if(x1>x2){uint16_t tmp=x1;x1=x2;x2=tmp;}
    if(y1>y2){uint16_t tmp=y1;y1=y2;y2=tmp;}

    Lcd_Write_Command(0x2a);
    Lcd_Write_Data(x1 >> 8);
    Lcd_Write_Data(x1 & 0xFF);
    Lcd_Write_Data(x2 >> 8);
    Lcd_Write_Data(x2 & 0xFF);

    Lcd_Write_Command(0x2b);
    Lcd_Write_Data(y1 >> 8);
    Lcd_Write_Data(y1 & 0xFF);
    Lcd_Write_Data(y2 >> 8);
    Lcd_Write_Data(y2 & 0xFF);
    /* начать запись */
    Lcd_Write_Command(0x2c); 
}

/** @brief Аппаратный сброс дисплея */
static void Lcd_Reset(void)
{
    HAL_GPIO_WritePin(GPIO_0, RESET, 1);
    HAL_DelayMs(2);
    HAL_GPIO_WritePin(GPIO_0, RESET, 0);
    HAL_DelayMs(5);
    HAL_GPIO_WritePin(GPIO_0, RESET, 1);
    HAL_DelayMs(2);
}

/** @brief Инициализация дисплея */
void Lcd_Init(void) {

    Lcd_Reset();

    // digitalWrite(CS,LOW);  //CS //для активации
    Lcd_select();

    Lcd_Write_Command(0xF7); //команда разблокировка
    Lcd_Write_Data(0xA9); //данные
    Lcd_Write_Data(0x51);
    Lcd_Write_Data(0x2C);
    Lcd_Write_Data(0x82);

    Lcd_Write_Command(0xC0); //установка опорного напряжения
    Lcd_Write_Data(0x11); //значения
    Lcd_Write_Data(0x09);

    Lcd_Write_Command(0xC1); //режим энергосбережения
    Lcd_Write_Data(0x41);

    Lcd_Write_Command(0xC5); //напряжение для включения  и выключения пикселей
    Lcd_Write_Data(0x00);
    Lcd_Write_Data(0x0A);
    Lcd_Write_Data(0x80);

    //ЧАСТОТА ОБНОВЛЕНИЯ КАДРОВ
    Lcd_Write_Command(0xB1); 
    // Lcd_Write_Data(0xB0);
    // Lcd_Write_Data(0x11);
    Lcd_Write_Data(0x00);
    Lcd_Write_Data(0x18);      //0x1F - 61hz, 0x1B - 70 hz, 0x18 - 79 Hz 

    Lcd_Write_Command(0xB4);  //режим инверсии, как пиксели будут переключатся между состояниями
    Lcd_Write_Data(0x02);

    Lcd_Write_Command(0xB6);  // режим отображения
    Lcd_Write_Data(0x02);
    Lcd_Write_Data(0x22);
    
    Lcd_Write_Command(0xB7);  // настройка источника данных
    Lcd_Write_Data(0xC6);

    Lcd_Write_Command(0xBE);  //для vscan
    Lcd_Write_Data(0x00);
    Lcd_Write_Data(0x04);
    // ПАРАМЕТРЫ ИНТЕРФЕЙСА
    Lcd_Write_Command(0xE9);  
    Lcd_Write_Data(0x00);
    //ЗАДАНИЕ ОРИЕНТАЦИИ 
    Lcd_Write_Command(0x36);  //MAC/MDAD-указывает направление, в котором данные записываются в LCD, влияет на ориентацию дисплея
    Lcd_Write_Data(0x08);     //0x28 | 0x80 - ровно, относительно надписи, 0x88 | 0x40 - влево, 0x08 - вправо, 0x28 | 0x40 - 180 относительно надписи "Отладочная плата"
    // ФОРМАТ ПИКСЕЛЕЙ
    Lcd_Write_Command(0x3A);  
    Lcd_Write_Data(0x66);     //0x66-18bit(RGB666), 0x55-16bit(RGB565), 0x77-24bit(RGB888)
    //POSITIVE GAMMA CORRECTION
    Lcd_Write_Command(0xE0);  
    Lcd_Write_Data(0x00);
    Lcd_Write_Data(0x07);
    Lcd_Write_Data(0x10);
    Lcd_Write_Data(0x09);
    Lcd_Write_Data(0x17);
    Lcd_Write_Data(0x0B);
    Lcd_Write_Data(0x41);
    Lcd_Write_Data(0x89);
    Lcd_Write_Data(0x4B);
    Lcd_Write_Data(0x0A);
    Lcd_Write_Data(0x0C);
    Lcd_Write_Data(0x0E);
    Lcd_Write_Data(0x18);
    Lcd_Write_Data(0x1B);
    Lcd_Write_Data(0x0F);   
    //NEGATIVE GAMMA CORRECTION
    Lcd_Write_Command(0xE1);    
    Lcd_Write_Data(0x00);
    Lcd_Write_Data(0x17);
    Lcd_Write_Data(0x1A);
    Lcd_Write_Data(0x04);
    Lcd_Write_Data(0x0E);
    Lcd_Write_Data(0x06);
    Lcd_Write_Data(0x2F);
    Lcd_Write_Data(0x45);
    Lcd_Write_Data(0x43);
    Lcd_Write_Data(0x02);
    Lcd_Write_Data(0x0A);
    Lcd_Write_Data(0x09);
    Lcd_Write_Data(0x32);
    Lcd_Write_Data(0x36);
    Lcd_Write_Data(0x0F);   

    //ВЫХОД ИЗ СПЯЩЕГО РЕЖИМА
    Lcd_Write_Command(0x11);    
    HAL_DelayMs(120);    
    //ВКЛЮЧЕНИЕ ДСИПЛЕЯ
    Lcd_Write_Command(0x29);    
    Lcd_unselect();
} 

/**
 * @brief       Выделение и сдвиг нужных битов для каждого цветового канала
 * @param color Заданный цвет
 */
static void Lcd_Draw_Color(unsigned int color)
{
    Lcd_Write_Data((color>>8)&0xF8); // R
    Lcd_Write_Data((color>>3)&0xFC); // G
    Lcd_Write_Data(color<<3);        // B
}

/**
 * @brief           Очищает весь экран, заполняя его цветом color
 * @param color     Новый цвет экрана 
 */
void LCD_Clear(unsigned int color) 
{
    HAL_GPIO_WritePin(GPIO_0, CS, 0);
    Address_set(0, 0, 480 - 1, 320 - 1); // размер экрана
    for (int col = 0; col < 320; col++){
        for (int row = 0; row < 480; row++) {
            Lcd_Draw_Color(color);
        }
    }
    HAL_GPIO_WritePin(GPIO_0, CS, 1);
}

/**
 * @brief          Отрисовка залитого прямоугольника на дисплее
 * @param  length  Высота прямоугольника в пикселях 
 * @param  width   Ширина прямоугольника в пикселях 
 * @param  x       Координата Y левого нижнего угла
 * @param  y       Координата X левого нижнего угла
 * @param  color   Цвет заливки
 */
void rect(uint16_t length, uint16_t width, uint16_t y, uint16_t x, uint16_t color) {
    Lcd_select();
    HAL_GPIO_WritePin(GPIO_0, RS, 0);
    Address_set(x, y, x + width - 1, y + length - 1); // область рисования прямоугольника
    HAL_GPIO_WritePin(GPIO_0, RS, 1);
    
    // выделение и сдвиг нужных битов для каждого цветового канала
    uint8_t r = (color >> 8) & 0xF8;
    uint8_t g = (color >> 3) & 0xFC;
    uint8_t b = (color << 3);
    
    uint32_t pixels = (uint32_t)length * width;
    
    __HAL_SPI_ENABLE(&hspi0);
    
    while (pixels--) {
        while (!(hspi0.Instance->INT_STATUS & SPI_INT_STATUS_TX_FIFO_NOT_FULL_M));
        hspi0.Instance->TXDATA = r;
        hspi0.Instance->TXDATA = g;
        hspi0.Instance->TXDATA = b;
    }
    
    __HAL_SPI_DISABLE(&hspi0);
    Lcd_unselect();
}


/**
 * @brief       Функция инверсии битов в байте
 * @param b     Заданный исходный байт
 * @return      Байт с развёрнутым порядком бит
 */
uint8_t reverse_bits(uint8_t b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}

/**
 * @brief           Вывод одного символа
 * @param  x        Горизонтальная координата
 * @param  y        Вертикальная координата
 * @param  ch       Код символа
 * @param  color    Цвет символа
 * @param  bg       Цвет фона
 * @param  scale    Масштаб
 */
void drawChar(uint16_t x, uint16_t y, char ch, uint16_t color, uint16_t bg, uint8_t scale) {
    if (ch < 32 || ch > 126) return;
    const uint8_t *bmp = Char_font1[ch - 32];

    for (uint8_t col = 0; col < 5; col++) {
        uint8_t line = reverse_bits(bmp[col]);

        for (uint8_t row = 0; row < 7; row++) {
            uint8_t pixel_on = (line & (1 << row)) ? 1 : 0;
            uint16_t draw_color = pixel_on ? color : bg;
            ClearMassDMA(scale + 1, scale + 1, y + col * scale, x + row * scale, draw_color);
        }
    }
}


/**
 * @brief           Вертикальный вывод строки 
 * @param  x        Горизонтальная координата колонки
 * @param  y        Вертикальная координата первого символа
 * @param  text     Заданный текст
 * @param  color    Цвет символов
 * @param  bg       Цвет фона
 * @param  scale    Масштаб
 */
void drawText(uint16_t x, uint16_t y, const char *text, uint16_t color, uint16_t bg, uint8_t scale) {
    while (*text) 
    {
        drawChar(x, y, *text, color, bg, scale);
        y += 6 * scale; 
        text++;
    }
}

/**
 * @brief          Отрисовка спрайта 8×8 (аналогично drawChar)
 * @param  x       Горизонтальная координата начала отрисовки
 * @param  y       Вертикальная координата начала отрисовки
 * @param  bmp     Массив 8 байт
 * @param  fg      Цвет спрайта
 * @param  bg      Цвет фона
 * @param  scale   Масштаб
 */
void draw_sprite_char(uint16_t x, uint16_t y, const uint8_t *bmp,
                      uint16_t fg, uint16_t bg, uint8_t scale)
{
    for (uint8_t col = 0; col < 8; col++) {
        uint8_t line = reverse_bits(bmp[col]);

        for (uint8_t row = 0; row < 8; row++) {
            uint16_t color = (line & (1 << row)) ? fg : bg;
            ClearMassDMA(scale + 1, scale + 1, y + col * scale, x + row * scale, color);
        }
    }
}

/**
 * @brief           Чтение пикселя 
 * @param  bmp      Указатель на данные спрайта
 * @param  width    Ширина битмапа в пикселях
 * @param  px       Координата X в битмапе
 * @param  py       Координата Y в битмапе
 * @return          1 — пиксель установлен, 0 — фон
 */
static uint8_t bitmap_bpp_get(const uint8_t *bmp, uint16_t width, uint16_t px, uint16_t py)
{
    uint16_t bpr = (width + 7) / 8;
    uint8_t byte = bmp[py * bpr + (px >> 3)];

    return ((byte >> (7 - (px & 7))) & 1);
}

/**
 * @brief           Преобразование RGB565 в RGB888 для DMA_BUF
 * @param  color    Цвет в формате RGB565
 * @param  rgb      Выходной буфер из 3 байт (порядок как в ClearMassDMA: G, B, R)
 */
static void lcd_rgb565_to_dma(const uint16_t color, uint8_t rgb[3])
{
    rgb[0] = (uint8_t)((color >> 3) & 0xFCu);
    rgb[1] = (uint8_t)(color << 3);
    rgb[2] = (uint8_t)((color >> 8) & 0xF8u);
}

/**
 * @brief           Длина оси спрайта на экране с учётом масштаба
 * @param  count    Число пикселей в битмапе по этой оси
 * @param  step     Шаг между блоками (scale)
 * @param  blk      Размер блока на экране (scale + 1)
 * @return          Длина в пикселях
 */
static uint16_t sprite_axis_span(uint16_t count, uint8_t step, uint8_t blk)
{
    if (count == 0) {
        return 0;
    }
    return (((count - 1) * step) + blk);
}

/**
 * @brief           Индекс пикселя спрайта по координате на экране
 * @param  pos      Координата
 * @param  origin   Начало спрайта 
 * @param  step     Шаг между блоками (scale)
 * @param  count    Число пикселей в битмапе по этой оси
 * @return          Индекс в битмапе (0 .. count-1)
 */
static uint16_t sprite_axis_index(uint16_t pos, uint16_t origin, uint8_t step, uint16_t count)
{
    uint16_t rel = (pos >= origin) ? (pos - origin) : 0;
    uint16_t idx = rel / step;

    if (idx >= count) {
        idx = count - 1;
    }
    return idx;
}

/**
 * @brief           Начало SPI-сессии (как PrintMassDMA_Optimized)
 * @param  length   Высота прямоугольника 
 * @param  width    Ширина прямоугольника 
 * @param  d_l      Начало по d_l (вертикаль)
 * @param  d_w      Начало по d_w (горизонталь)
 */
static void lcd_dma_begin(uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w)
{
    Lcd_select();
    Address_set(d_w, d_l, width - 1u + d_w, length - 1u + d_l);
    HAL_GPIO_WritePin(GPIO_0, RS, 1);
    hspi0.ErrorCode = HAL_SPI_ERROR_NONE;
    hspi0.Instance->TX_THR = 1;
    if (!(hspi0.Instance->ENABLE & SPI_ENABLE_M)) {
        __HAL_SPI_ENABLE(&hspi0);
    }
}

/**
 * @brief   Завершение SPI-сессии 
 */
static void lcd_dma_end(void)
{
    if (!(hspi0.Instance->CONFIG & SPI_CONFIG_MANUAL_CS_M)) {
        __HAL_SPI_DISABLE(&hspi0);
        hspi0.Instance->ENABLE |= SPI_ENABLE_CLEAR_TX_FIFO_M | SPI_ENABLE_CLEAR_RX_FIFO_M;
    }
    volatile uint32_t unused = hspi0.Instance->INT_STATUS;
    (void)unused;
    Lcd_unselect();
}


/**
 * @brief           Отправка одной строки пикселей из DMA_BUF в дисплей
 * @param  width    Число пикселей в строке 
 */
static void lcd_dma_send_row(uint16_t width)
{
    // запуск DMA на всю строку (width*3 байт-1)
    HAL_DMA_Start(&hdma_ch0, (void *)DMA_BUF, (void *)&hspi0.Instance->TXDATA, width * 3 - 1);
    // ожидание завершения DMA
    HAL_DMA_Wait(&hdma_ch0, DMA_TIMEOUT_DEFAULT);
}

/**
 * @brief            Спрайт: сборка строк в DMA_BUF и отправка по DMA
 * @param  x         Координата d_w (горизонталь)
 * @param  y         Координата d_l (вертикаль)
 * @param  bmp       Данные Image2Cpp
 * @param  width     Ширина спрайта в пикселях
 * @param  height    Высота спрайта в пикселях
 * @param  fg        Цвет установленного бита
 * @param  bg        Цвет фона
 * @param  scale     Масштаб (шаг между блоками, размер блока = scale + 1)
 *
 * Один Address_set на весь спрайт, затем построчный DMA (аналог PrintMassDMA_Optimized).
 */
void draw_bitmap(uint16_t x, uint16_t y, const uint8_t *bmp,
                     uint16_t width, uint16_t height,
                     uint16_t fg, uint16_t bg, uint8_t scale)
{
    uint8_t fg_rgb[3];
    uint8_t bg_rgb[3];
    const uint8_t step = (scale == 0) ? 1 : scale;
    const uint8_t blk = scale + 1;
    uint16_t row_len;
    uint16_t row_wid;

    if (width == 0 || height == 0 || bmp == NULL) {
        return;
    }

    lcd_rgb565_to_dma(fg, fg_rgb);
    lcd_rgb565_to_dma(bg, bg_rgb);

    row_len = sprite_axis_span(width, step, scale + 1);
    row_wid = sprite_axis_span(height, step, blk);    

    if (row_wid > TFT_WIDTH) {
        for (uint16_t sy = 0; sy < height; sy++) {
            for (uint16_t sx = 0; sx < width; sx++) {
                uint16_t color = bitmap_bpp_get(bmp, width, sx, sy) ? fg : bg;

                ClearMassDMA(blk, blk, (y + sx * step), (x + (height - 1 - sy) * step), color);                
            }
        }
        return;
    }
    lcd_dma_begin(row_len, row_wid, y, x);

    for (uint16_t row = 0; row < row_len; row++) {
        for (uint16_t col = 0; col < row_wid; col++) {
            uint16_t sx;
            uint16_t sy;
            const uint8_t *rgb;

            sx = sprite_axis_index(y + row, y, step, width);
            sy = height - 1 - sprite_axis_index(x + col, x, step, height);
            
            rgb = bitmap_bpp_get(bmp, width, sx, sy) ? fg_rgb : bg_rgb;
            DMA_BUF[col * 3 + 0] = rgb[0];
            DMA_BUF[col * 3 + 1] = rgb[1];
            DMA_BUF[col * 3 + 2] = rgb[2];
        }
        lcd_dma_send_row(row_wid);
    }
    lcd_dma_end();
}


/**
 * @brief               Горизонталь спрайта по номеру дорожки сетки
 * @param  row          Номер дорожки
 * @param  ground_row   Дорожка «земли»
 * @param  ground_x     Координата x на дорожке земли
 * @param  air_x        Координата x на остальных дорожках
 * @return              Координата x для ClearMassDMA / drawChar
 */
static uint16_t grid_lane_x(int row, int ground_row, uint16_t ground_x, uint16_t air_x)
{
    if (row == ground_row) 
    {
        return ground_x;
    }
    return air_x;
}

/**
 * @brief              Перевод клетки сетки в координаты 
 * @param  col         Столбец 
 * @param  row         Дорожка 
 * @param  origin_y    Начало поля по d_l
 * @param  cell_pitch  Шаг сетки вдоль d_l
 * @param  sprite_sz   Размер спрайта (с учётом масштаба)
 * @param  ground_row  Дорожка земли
 * @param  ground_x    x на дорожке земли
 * @param  air_x       x на дорожке воздуха
 * @param  x           Выход: горизонталь 
 * @param  y           Выход: вертикаль 
 */
void grid_cell_to_pixel(int col, int row, uint16_t origin_y, uint16_t cell_pitch, uint16_t sprite_sz, int ground_row, uint16_t ground_x, uint16_t air_x, uint16_t *x, uint16_t *y)
{
    *x = grid_lane_x(row, ground_row, ground_x, air_x);
    *y = (uint16_t)(origin_y + (uint16_t)col * cell_pitch + (cell_pitch - sprite_sz) / 2);
}

/**
 * @brief          Отрисовка одного пикселя
 * @param  x       Горизонтальная координата
 * @param  y       Вертикальная координата
 * @param  color   Цвет пикселя 
 */
void drawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    ClearMassDMA(1, 1, y, x, color);
}


static void ClearMassDMA(uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w, uint16_t color) {
    //HAL_SPI_Enable(&hspi0);
    Lcd_select();
    length++; width++;
    Address_set(d_w, d_l, width - 2 + d_w, length - 2 + d_l);
    HAL_GPIO_WritePin(GPIO_0, RS, 1);
    hspi0.ErrorCode = HAL_SPI_ERROR_NONE;
    hspi0.Instance->TX_THR = 1;

    // Извлекаем компоненты цвета
    uint8_t red   = (color >> 8) & 0xF8;
    uint8_t green = (color >> 3) & 0xFC;
    uint8_t blue  = (color << 3);

    if (!(hspi0.Instance->ENABLE & SPI_ENABLE_M)) 
    {
        __HAL_SPI_ENABLE(&hspi0);
    }

    for (uint16_t m = 0; m < length; m++) {   // строки
        // собрать строку в буфер
        for (uint16_t i = 0; i < width; i++) {
            DMA_BUF[i*3 + 2] = red;
            DMA_BUF[i*3 + 0] = green;
            DMA_BUF[i*3 + 1] = blue;      
        }
        
        lcd_dma_send_row(width);
    }

    lcd_dma_end();
}

/** 
 * @brief        Построчная заливка заданной области экрана 
 * @param length Длина закрашиваемой зоны
 * @param width  Ширина закрашиваемой зоны
 * @param d_l    Координата по вертикали
 * @param d_w    Координата по горизонтали
 * @param color  Цвет заливки 
*/
void ClearMassDMA_Fast(uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w, uint16_t color)
{
    Lcd_select();
    Address_set(d_w, d_l, width - 1 + d_w, length - 1 + d_l);
    HAL_GPIO_WritePin(GPIO_0, RS, 1);

    if (!(hspi0.Instance->ENABLE & SPI_ENABLE_M))
        __HAL_SPI_ENABLE(&hspi0);

    static uint8_t line_buf[480 * 3];

    uint8_t red   = (color >> 8) & 0xF8;
    uint8_t green = (color >> 3) & 0xFC;
    uint8_t blue  = (color << 3);

    // заполняем ОДИН раз
    uint8_t *p = line_buf;
    for (uint16_t i = 0; i < width; i++)
    {
        *p++ = red;
        *p++ = green;
        *p++ = blue;
    }

    // первая строка
    HAL_DMA_Start(&hdma_ch0, line_buf, (void*)&hspi0.Instance->TXDATA, width * 3 - 1);

    // остальные строки
    for (uint16_t y = 1; y < length; y++)
    {
        HAL_DMA_Wait(&hdma_ch0, DMA_TIMEOUT_DEFAULT);
        HAL_DMA_Start(&hdma_ch0, line_buf, (void*)&hspi0.Instance->TXDATA, width * 3 - 1);
    }

    HAL_DMA_Wait(&hdma_ch0, DMA_TIMEOUT_DEFAULT);

    lcd_dma_end();
}
