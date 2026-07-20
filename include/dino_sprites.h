/**
 * @file    dino_sprites.h
 * @brief   Битмап-спрайты Dino Game
 */

#ifndef DINO_SPRITES_H
#define DINO_SPRITES_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported variables --------------------------------------------------------*/

/* луна (32x32px) */
extern const uint8_t moon []; 
/* солнце (32x32px) */
extern const uint8_t sun [];
/* препятствие 1 - Птеродактиль в положении взмаха (32x32px) */
extern const uint8_t bird_bottom[];
/* препятствие 1 - Птеродактиль в положении спуска (32x32px) */
extern const uint8_t bird_top[];
/* препятствие 2 - Низкий птеродактиль в положении спуска (32x27px) */
extern const uint8_t bird_bottom1 [];
/* препятствие 2 - Низкий птеродактиль в положении взлета (32x27px) */
extern const uint8_t bird_top1 [];
/* препятствие 3 - Кактус вид 1 */
extern const uint8_t cactus1[];
/* препятствие 4 - Кактус вид 2 */
extern const uint8_t cactus2[];
/* препятствие 5 - Кактус вид 3 */
extern const uint8_t cactus3[];
/* динозаврик проиграл */
extern const uint8_t dino_dead[];
/* положение динозаврика в забеге 1 (32x32px) */
extern const uint8_t dino_run1x[];
/* положение динозаврика в забеге 2 (32x32px) */
extern const uint8_t dino_run2x[];
/* положение динозаврика в присяде 1 (32x20px) */ 
extern const uint8_t dino_duck1x [];
/* положение динозаврика в присяде 2 (32x20px) */
extern const uint8_t dino_duck2x [];
/* облако (43x32px) */ 
extern const uint8_t  cloud [];



#endif /* __DINO_SPRITES_H__ */
