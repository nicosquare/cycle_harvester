/*
    ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "hal.h"

/**
 * @brief   PAL setup.
 * @details Digital I/O ports static configuration as defined in @p board.h.
 *          This variable is used by the HAL when initializing the PAL driver.
 */
#if HAL_USE_PAL || defined(__DOXYGEN__)
const PALConfig pal_default_config =
{
    {VAL_GPIOAODR, VAL_GPIOACRL, VAL_GPIOACRH},
    {VAL_GPIOBODR, VAL_GPIOBCRL, VAL_GPIOBCRH},
    {VAL_GPIOCODR, VAL_GPIOCCRL, VAL_GPIOCCRH},
    {VAL_GPIODODR, VAL_GPIODCRL, VAL_GPIODCRH},
    {VAL_GPIOEODR, VAL_GPIOECRL, VAL_GPIOECRH},
};
#endif

/*
 * Early initialization code.
 * This initialization must be performed just after stack setup and before
 * any other initialization.
 */
void __early_init(void) {

    stm32_clock_init();
}

/*
 * Board-specific initialization code.
 */
void boardInit(void) {

    /*
    	USART3 re-mapped to PC10-PC11
    */

    AFIO -> MAPR |= AFIO_MAPR_USART3_REMAP_PARTIALREMAP;

    /*
    	del AFIO typedef, el campo o atributo MAPR se cambia (se hace una OR), para remap en este caso se usa el USART
    	http://stm32.kosyak.info/doc/stm32f10x_8h_source.html ->> 02606 #define AFIO_MAPR_USART3_REMAP_PARTIALRMAP, y
    	02594 ****************************** Bit definition for AFIO_MAPR register ************************
    	http://stm32.kosyak.info/doc/struct_a_f_i_o___type_def.html -->> el Typedef AFIO
    */

}
