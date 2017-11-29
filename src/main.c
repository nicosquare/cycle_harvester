/*
	ChibiOS - Copyright (C) 2006..2016 Giovanni Di Sirio

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

#include "ch.h"
#include "hal.h"
#include "test.h"

#include "main.h"
#include "hc05.h"

/* =================================================*/
/* General control variable                         */
/*                                                  */
/*===================================================*/

#define RUN_MAIN_THREAD                 FALSE

char * energy_measure = "10";

/* =================================================*/
/* Bluetooth test                                   */
/*             38400     115200                                */
/*===================================================*/

#define HAL_USE_BLUETOOTH                 TRUE

struct BluetoothDriver* BluetoothDriverForConsole;
extern struct BluetoothDeviceVMT hc05BtDevVMT;

/* =================================================*/
/* Thread definition                                */
/*                                                  */
/*===================================================*/

/*
 * Blinker thread #1.
 */

static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

    (void)arg;

    chRegSetThreadName("blinker");

    while (true) {
        palSetPad(GPIOD, 2);
        chThdSleepMilliseconds(1000);
        palClearPad(GPIOD, 2);
        chThdSleepMilliseconds(1000);
    }
}

/*
 * Bluetooth thread #2.
 */

static THD_WORKING_AREA(waThread2, 128);
static THD_FUNCTION(Thread2, arg) {

    (void)arg;

    chRegSetThreadName("bluetooth");

    static struct hc05_config_t myhc05_config = {
        .txport = gpioa_port,
        .txpin = 2,
        .rxport = gpioa_port,
        .rxpin = 3,
        .keyport = gpioa_port,
        .keypin = 4,
        .serialdriver = sd2
    };

    static struct BluetoothConfig myTestBluetoothConfig = {
        .name = "Modul",
        .pincode = "1234",
        .baudrate = b9600	,
        .usedmodule = hc05,
        .myhc05config = &myhc05_config

    };

    static struct BluetoothDriver myTestBluetoothDriver = {
        .vmt = &hc05BtDevVMT,
        .config = &myTestBluetoothConfig,
        .driverIsReady = 0,
        .commSleepTimeMs = 100
    };

    //set the test driver struct as the consolestruct
    BluetoothDriverForConsole = &myTestBluetoothDriver;

    btOpen(&myTestBluetoothDriver, &myTestBluetoothConfig);

    chThdSleepMilliseconds(1000);
    
    char * read_value = "";

    while(true) {

		btSend(&myTestBluetoothDriver, energy_measure, 5);
        chThdSleepMilliseconds(1000);

        int has_read = btRead(&myTestBluetoothDriver, read_value, 10);

        if(has_read == EXIT_FAILURE) {
            chprintf((BaseChannel *)&SD3, "There is not anything to read\n\r");
        } else {
            chprintf((BaseChannel *)&SD3, "Reading ok \n\r");
        }

        chprintf((BaseChannel *)&SD3, "Read value \n\r");
        chprintf((BaseChannel *)&SD3, read_value);
        chprintf((BaseChannel *)&SD3, "\n\r");

        chThdSleepMilliseconds(1000);
    }

}

/*===========================================================================*/
/* Initialization and main thread.                                           */
/*===========================================================================*/

int main(void) {

    /*
    	* Initialization of variables
    	*/

    int current_state = st_init;
    int next_state = st_measure;
    int response = 1;
    int http_status = 200;

    /*
    	* System initializations.
    	* - HAL initialization, this also initializes the configured device drivers
    	*   and performs the board-specific initializations.
    	* - Kernel initialization, the main() function becomes a thread and the
    	*   RTOS is active.
    	*/

    halInit();
    chSysInit();
    boardInit();

    /*
    * Activates the serial driver 1 using the driver default configuration.
    * PA9(TX) and PA10(RX) are routed to USART1.
    */

    sdStart(&SD3, NULL);

    /*
    * Creates the example threads.
    */

    chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO+1, Thread1, NULL);
    chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO+1, Thread2, NULL);

    /*
    * Normal main() thread activity, in this demo it does nothing except
    * sleeping in a loop and check the button state, when the button is
    * pressed the test procedure is launched.
    */

    // Change to
    while (RUN_MAIN_THREAD) {

        switch(current_state) {

        case st_init:

            // ...
            chprintf((BaseChannel *)&SD3, "Update User Interface \n\r");
            // ...
            chprintf((BaseChannel *)&SD3, "Wait a minute until next measure \n\r");
            // ...
            chThdSleepMilliseconds(1000);

            next_state = st_measure;

            break;

        case st_measure:

            // ...
            chprintf((BaseChannel *)&SD3, "Getting energy consumption measure \n\r");
            // ...
            chprintf((BaseChannel *)&SD3, "Calculate delta energy/ delta time \n\r");

            next_state = st_send_to_cu;

            break;

        case st_send_to_cu:

            // ...
            chprintf((BaseChannel *)&SD3, "Sending measure data to central unit \n\r");

            next_state = st_wait_ack;

            break;

        case st_wait_ack:

            // ...
            chprintf((BaseChannel *)&SD3, "Waiting for ACK confirmation \n\r");

            if(response) {
                next_state = st_store_in_bd;
            }
            else {
                next_state = st_wait_ack;
            }

            break;

        case st_store_in_bd:

            // ...
            chprintf((BaseChannel *)&SD3, "Storing information on BD \n\r");

            next_state = st_wait_bd_confirmation;

            break;

        case st_wait_bd_confirmation:

            // ...
            chprintf((BaseChannel *)&SD3, "Waiting for BD confirmation \n\r");

            if(response) {
                next_state = st_init;
            }
            else {
                next_state = st_store_in_bd;
            }

            break;

        default:
            next_state = current_state;

        }

        current_state = next_state;

    }
}
