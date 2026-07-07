/**
 * @file    bdma.h
 * @brief   Драйвер DMA
 */

#ifndef BDMA_H
#define BDMA_H

#include "mik32_hal_dma.h"

#define TFT_WIDTH         480
#define DMA_BUFFER_SIZE   (TFT_WIDTH * 3)

extern uint8_t *DMA_BUF;
extern DMA_InitTypeDef hdma;
extern DMA_ChannelHandleTypeDef hdma_ch0;

void DMA_Init(void);

#endif /* __BDMA_H__ */