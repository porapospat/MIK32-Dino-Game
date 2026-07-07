/**
 * @file  bus_spi.h
 * @brief Драйвер SPI
 */

#ifndef BUS_SPI_H
#define BUS_SPI_H

#include "mik32_hal_spi.h"

extern SPI_HandleTypeDef hspi0;
extern SPI_HandleTypeDef hspi1;

void SPI0_Init(void);
void SPI1_Init(void);

#endif /* BUS_SPI_H */
