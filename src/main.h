/*!
 * @file main.h
 * @brief Header file for main file in ChibiosRT.
 *
 * @{
 */

#define HAL_USE_HC_05_BLUETOOTH                 TRUE

#if HAL_USE_HC_05_BLUETOOTH || defined(__DOXYGEN__) || 1

#include "hal.h"
#include "bluetooth.h"
#include "stdlib.h"

/**
 * @brief FSM States
 */
enum fsm_states {
    st_init = 0,
    st_measure = 1,
    st_send_to_cu = 2,
    st_wait_ack = 3,
    st_store_in_bd = 4,
    st_wait_bd_confirmation = 5
};

#endif  //HAL_USE_HC05 || defined(__DOXYGEN__)

