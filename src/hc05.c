/*!
 * @file hc05.c
 * @brief Source file for HC-05 SPP device for bluetooth module in ChibiosRT.
 *
 * @addtogroup BLUETOOTH
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "shell.h"
#include "chqueues.h"
#include "bluetooth.h"
#include "hc05.h"
#include "serial.h"
#include "serial_lld.h"
#include "mcuconf.h"
#include <string.h>

#define HAL_USE_HC_05_BLUETOOTH                 TRUE

#if HAL_USE_HC_05_BLUETOOTH || defined(__DOXYGEN__) || 1


/*===========================================================================*/
/* Local variables                 .                                         */
/*===========================================================================*/

/*!
 * \brief SerialConfig to be used for communication with HC-05
 */
static SerialConfig hc05SerialConfig = {38400,0,0,0};


/*!
 * \brief current state of the driver / HC-05
 */
static volatile enum hc05_state_t hc05CurrentState = st_unknown;


/*===========================================================================*/
/* VMT functions                                                             */
/*===========================================================================*/

/*!
 * \brief Sends the given buffer
 *
 * \param[in] instance A BluetoothDriver object
 * \param[in] buffer A pointer to a buffer to read from
 * \param[in] bufferlength The number of bytes to send
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int hc05sendBuffer(struct BluetoothDriver *instance, char *buffer, int bufferlength) {
	
	chprintf((BaseChannel *)&SD3, "HC-05 Send Buffer \n\r");

    if ( !instance || !buffer )
        return EXIT_FAILURE;
    if ( !bufferlength )
        return EXIT_SUCCESS;

    return sdWriteTimeout(instance->config->myhc05config->hc05serialpointer, buffer, bufferlength, TIME_IMMEDIATE) > 0
           ? EXIT_SUCCESS
           : EXIT_FAILURE;

}

/*!
 * \brief Sends the given byte
 *
 * \param[in] instance A BluetoothDriver object
 * \param[in] mybyte A byte to send
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int hc05sendByte(struct BluetoothDriver *instance, int mybyte) {

	chprintf((BaseChannel *)&SD3, "HC-05 Send Byte \n\r");

    if ( !instance )
        return EXIT_FAILURE;

    return sdPut(instance->config->myhc05config->hc05serialpointer, mybyte);
}


/*!
 * \brief Checks if there is incoming data
 *
 * \param[in] instance A BluetoothDriver object
 * \return 1 if there is data, 0 if there isn't
 */
int hc05canRecieve(struct BluetoothDriver *instance) {

	chprintf((BaseChannel *)&SD3, "HC-05 Can receive \n\r");

    if ( !instance )
        return EXIT_FAILURE;

	chprintf((BaseChannel *)&SD3, "HC-05 Can receive instance OK \n\r");

    return sdGetWouldBlock(instance->config->myhc05config->hc05serialpointer) == 0 ? 1 : 0;

}

/*!
 * \brief Reads from the bluetooth module into the buffer
 *
 * \param[in] instance A BluetoothDriver object
 * \param[in] buffer A pointer to a buffer to write into
 * \param[in] maxlength The maximum number of bytes to read (size of the buffer)
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int hc05readBuffer(struct BluetoothDriver *instance, char *buffer, int maxlength) {

	chprintf((BaseChannel *)&SD3, "HC-05 Read Buffer \n\r");

    if ( !instance || !buffer )
        return EXIT_FAILURE;
    if ( !maxlength )
        return EXIT_SUCCESS;

    return sdReadTimeout(instance->config->myhc05config->hc05serialpointer, buffer, maxlength, TIME_IMMEDIATE) > 0
           ? EXIT_SUCCESS
           : EXIT_FAILURE;

}

/*!
*	\brief Sends an AT command
*
*	\param[in] instance A BluetoothDriver object
*	\param[in] command AT command to use. Must be '\0' terminated string
*	\return EXIT_SUCCESS or EXIT_FAILURE
*/
int hc05sendAtCommand(struct BluetoothDriver *instance, char* command) {

	chprintf((BaseChannel *)&SD3, "HC-05 Send At Command \n\r");

    if ( !instance || !command )
        return EXIT_FAILURE;


    if (hc05CurrentState != st_ready_at_command)
    {
        hc05CurrentState = st_unknown;
        //enter AT mode here, but wait for threads to detect state change

        hc05SetModeAt(instance->config, 200);
        //chThdSleepMilliseconds(instance->commSleepTimeMs);
        chThdSleepMilliseconds(500);
    }


    sdWrite(instance->config->myhc05config->hc05serialpointer, command, strlen(command));
    sdWrite(instance->config->myhc05config->hc05serialpointer,"\r\n",2);

    chThdSleepMilliseconds(1000);

    hc05SetModeComm(instance->config, 200);

    return EXIT_SUCCESS;
}


/*!
 * \brief Sets the pin/access code for the HC-05 module
 *
 *  When in AT mode, we can set the pin/access code of the module.
 *
 * \param[in] instance A BluetoothDriver object
 * \param[in] pin A pointer to a char array containing the new pin code
 * \param[in] pinlength The length of the new pin
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int hc05setPinCode(struct BluetoothDriver *instance, char *pin, int pinlength) {

	chprintf((BaseChannel *)&SD3, "HC-05 Set Pin Code \n\r");

    if ( !instance || !pin )
        return EXIT_FAILURE;

    char Command[] = "AT+PIN=";
    int cmdLen = strlen(Command);
    int bufLen = cmdLen + pinlength + 1;

    char *CmdBuf = chHeapAlloc(NULL, bufLen);

    if(!CmdBuf)
        return EXIT_FAILURE;
    else
        memset(CmdBuf, '\0', bufLen);

    strcpy(CmdBuf, Command);
    strncpy(CmdBuf+cmdLen, pin, pinlength);
    //must terminate the string with a \0
    *(CmdBuf+bufLen-1) = '\0';

    if ( (hc05sendAtCommand(instance, CmdBuf) == EXIT_FAILURE))
        return EXIT_FAILURE;

    chHeapFree(CmdBuf);
    return EXIT_SUCCESS;
}

/*!
 * \brief Sets the name for the HC-05 module
 *
 *  When in AT mode, we can set the name of the module.
 *
 * \param[in] instance A BluetoothDriver object
 * \param[in] newname A pointer to a char array containing the new name
 * \param[in] namelength The length of the new name
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int hc05setName(struct BluetoothDriver *instance, char *newname, int namelength) {

	chprintf((BaseChannel *)&SD3, "HC-05 Set name \n\r");

    if ( !instance || !newname )
        return EXIT_FAILURE;

    char Command[] = "AT+NAME=";
    int cmdLen = strlen(Command);
    int bufLen = cmdLen + namelength + 1;

    char *CmdBuf = chHeapAlloc(NULL, bufLen);

    if(!CmdBuf)
        return EXIT_FAILURE;
    else
        memset(CmdBuf, '\0', bufLen);

    strcpy(CmdBuf, Command);
    strncpy(CmdBuf+cmdLen, newname, namelength);
    //must terminate the string with a \0
    *(CmdBuf+bufLen-1) = '\0';

    if ( (hc05sendAtCommand(instance, CmdBuf) == EXIT_FAILURE))
        return EXIT_FAILURE;

    chHeapFree(CmdBuf);
    return EXIT_SUCCESS;
}

/*!
 * \brief Default reset for HC-05 module
 *
 *  When in AT mode, we can reset the settings of the module.
 *
 * \param[in] instance A BluetoothDriver object
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int hc05resetDefaults(struct BluetoothDriver *instance) {

	chprintf((BaseChannel *)&SD3, "HC-05 Reset defaults \n\r");

    if ( !instance )
        return EXIT_FAILURE;

    char Command[] = "AT+ORGL";
    int cmdLen = strlen(Command);
    int bufLen = cmdLen + 1;

    char *CmdBuf = chHeapAlloc(NULL, bufLen);

    if(!CmdBuf)
        return EXIT_FAILURE;
    else
        memset(CmdBuf, '\0', bufLen);

    strcpy(CmdBuf, Command);
    //must terminate the string with a \0
    *(CmdBuf+bufLen-1) = '\0';

    if ( (hc05sendAtCommand(instance, CmdBuf) == EXIT_FAILURE))
        return EXIT_FAILURE;

    chHeapFree(CmdBuf);
    return EXIT_SUCCESS;
}


/*!
 * \brief Starts the driver
 *
 *  Read config
 *  Set the apropriate port/pin settings
 *  Set the name/pin according to the config
 *  Initialize the serial driver
 *  Set the ready flag
 *
 * \param[in] instance A BluetoothDriver object
 * \param[in] config A BluetoothConfig the use
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int hc05open(struct BluetoothDriver *instance, struct  BluetoothConfig *config) {

	chprintf((BaseChannel *)&SD3, "Open HC05 module\n\r");

    if(!instance || !config || !(config->myhc05config))
        return EXIT_FAILURE;

    //flag
    hc05CurrentState = st_initializing;
    // set config location
    instance->config = config;
    //set up the key and reset pins... using external functions
    hc05_setkeypin(config);
    hc05_setresetpin(config);
    //set up the RX and TX pins
    hc05_settxpin(config);
    hc05_setrxpin(config);

    //serial driver
    hc05_updateserialconfig(config);
    hc05_startserial(config);

    //flag
    hc05CurrentState = st_ready_communication;

    //return to communication mode
    hc05SetModeComm(config, 200);

    return EXIT_SUCCESS;
}

/*!
 * \brief Stops the driver
 *
 * Clean up the communications channel and stop the driver.
 *
 * \param[in] instance A BluetoothDriver object
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int hc05close(struct BluetoothDriver *instance) {
	
	chprintf((BaseChannel *)&SD3, "Close HC05 module\n\r");

    if(!instance)
        return EXIT_FAILURE;

    //flag --> threads will stop
    hc05CurrentState = st_shutting_down;
    chThdSleepMilliseconds(100);
    //stop serial driver
    hc05_stopserial(instance->config);

    return EXIT_SUCCESS;
}

/*===========================================================================*/
/* VMT                                                                       */
/*===========================================================================*/

/**
 * @brief HC05 BluetoothDriver virtual methods table.
 */
struct BluetoothDeviceVMT hc05BtDevVMT = {
    .sendBuffer = hc05sendBuffer,
    .sendByte = hc05sendByte,
    .canRecieve = hc05canRecieve,
    .readBuffer = hc05readBuffer,
    .setPinCode = hc05setPinCode,
    .setName = hc05setName,
    .open = hc05open,
    .close = hc05close,
    .resetModuleSettings = hc05resetDefaults
};

/*===========================================================================*/
/* Internal functions                                                        */
/*===========================================================================*/

/*!
 * \brief Sets up the TX pin of the serial line
 *
 *
 * \param[in] config A BluetoothConfig object
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int hc05_settxpin(struct BluetoothConfig *config) {

	chprintf((BaseChannel *)&SD3, "Set TX pin\n\r");

    if(!config || !(config->myhc05config))
        return EXIT_FAILURE;

    switch (config->myhc05config->txport) {

    case gpioa_port:
        palSetPadMode(GPIOA, config->myhc05config->txpin,
                      PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    case gpiob_port:
        palSetPadMode(GPIOB, config->myhc05config->txpin,
                      PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    case gpioc_port:
        palSetPadMode(GPIOC, config->myhc05config->txpin,
                      PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    case gpiod_port:
        palSetPadMode(GPIOD, config->myhc05config->txpin,
                      PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    case gpioe_port:
        palSetPadMode(GPIOE, config->myhc05config->txpin,
                      PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    default:
        return EXIT_FAILURE;
        break;
    }
    return EXIT_SUCCESS;
}

/*!
 * \brief Sets up the RX pin of the serial line
 *
 *
 * \param[in] config A BluetoothConfig object
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int hc05_setrxpin(struct BluetoothConfig *config) {

	chprintf((BaseChannel *)&SD3, "Set RX pin\n\r");

    if(!config || !(config->myhc05config))
        return EXIT_FAILURE;

    switch (config->myhc05config->rxport) {

    case gpioa_port:
        palSetPadMode(GPIOA, config->myhc05config->rxpin,
                      PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    case gpiob_port:
        palSetPadMode(GPIOB, config->myhc05config->rxpin,
                      PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    case gpioc_port:
        palSetPadMode(GPIOC, config->myhc05config->rxpin,
                      PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    case gpiod_port:
        palSetPadMode(GPIOD, config->myhc05config->rxpin,
                      PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    case gpioe_port:
        palSetPadMode(GPIOE, config->myhc05config->rxpin,
                      PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    default:
        return EXIT_FAILURE;
        break;
    }
    return EXIT_SUCCESS;
}

/*!
 * \brief Sets up the RTS pin of the serial line
 *
 *
 * \param[in] config A BluetoothConfig object
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int hc05_setrtspin(struct BluetoothConfig *config) {

	chprintf((BaseChannel *)&SD3, "Set RTS pin\n\r");

    if(!config || !(config->myhc05config))
        return EXIT_FAILURE;

    switch (config->myhc05config->rtsport) {

    case gpioa_port:
        palSetPadMode(GPIOA, config->myhc05config->rtspin,
                      PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    case gpiob_port:
        palSetPadMode(GPIOB, config->myhc05config->rtspin,
                      PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    case gpioc_port:
        palSetPadMode(GPIOC, config->myhc05config->rtspin,
                      PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    case gpiod_port:
        palSetPadMode(GPIOD, config->myhc05config->rtspin,
                      PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    case gpioe_port:
        palSetPadMode(GPIOE, config->myhc05config->rtspin,
                      PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    default:
        return EXIT_FAILURE;
        break;
    }
    return EXIT_SUCCESS;
}


/*!
 * \brief Sets up the CTS pin of the serial line
 *
 *
 * \param[in] config A BluetoothConfig object
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int hc05_setctspin(struct BluetoothConfig *config) {
	
	chprintf((BaseChannel *)&SD3, "Set CTS pin\n\r");

    if(!config || !(config->myhc05config))
        return EXIT_FAILURE;

    switch (config->myhc05config->ctsport) {

    case gpioa_port:
        palSetPadMode(GPIOA, config->myhc05config->ctspin,
                      PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    case gpiob_port:
        palSetPadMode(GPIOB, config->myhc05config->ctspin,
                      PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    case gpioc_port:
        palSetPadMode(GPIOC, config->myhc05config->ctspin,
                      PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    case gpiod_port:
        palSetPadMode(GPIOD, config->myhc05config->ctspin,
                      PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    case gpioe_port:
        palSetPadMode(GPIOE, config->myhc05config->ctspin,
                      PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    default:
        return EXIT_FAILURE;
        break;
    }
    return EXIT_SUCCESS;
}

/*!
 * \brief Sets up the reset pin for the module
 *
 *
 * \param[in] config A BluetoothConfig object
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int hc05_setresetpin(struct BluetoothConfig *config) {

	chprintf((BaseChannel *)&SD3, "Set RESET pin\n\r");
	
    if(!config || !(config->myhc05config))
        return EXIT_FAILURE;

    switch (config->myhc05config->resetport) {

    case gpioa_port:
        palSetPadMode(GPIOA, config->myhc05config->resetpin, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    case gpiob_port:
        palSetPadMode(GPIOB, config->myhc05config->resetpin, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    case gpioc_port:
        palSetPadMode(GPIOC, config->myhc05config->resetpin, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    case gpiod_port:
        palSetPadMode(GPIOD, config->myhc05config->resetpin, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    case gpioe_port:
        palSetPadMode(GPIOE, config->myhc05config->resetpin, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    default:
        return EXIT_FAILURE;
        break;
    }
    return EXIT_SUCCESS;
}

/*!
 * \brief Sets up the key pin for the module
 *
 *
 * \param[in] config A BluetoothConfig object
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int hc05_setkeypin(struct BluetoothConfig *config) {

	chprintf((BaseChannel *)&SD3, "Set KEY(EN) pin\n\r");

    if(!config || !(config->myhc05config))
        return EXIT_FAILURE;

    switch (config->myhc05config->keyport) {

    case gpioa_port:
        palSetPadMode(GPIOA, config->myhc05config->keypin, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    case gpiob_port:
        palSetPadMode(GPIOB, config->myhc05config->keypin, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    case gpioc_port:
        palSetPadMode(GPIOC, config->myhc05config->keypin, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    case gpiod_port:
        palSetPadMode(GPIOD, config->myhc05config->keypin, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    case gpioe_port:
        palSetPadMode(GPIOE, config->myhc05config->keypin, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        break;
    default:
        return EXIT_FAILURE;
        break;
    }
    return EXIT_SUCCESS;
}

/*!
 * \brief Updates the SerialConfig from the BluetoothConfig (change of baud rate)
 *
 *
 * \param[in] config A BluetoothConfig object
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int hc05_updateserialconfig(struct BluetoothConfig *config) {

	chprintf((BaseChannel *)&SD3, "Update serial config\n\r");


    if(!config || !(config->myhc05config))
        return EXIT_FAILURE;

    switch (config->baudrate) {

    case b1200:
        hc05SerialConfig.speed = 1200;
        break;
    case b2400:
        hc05SerialConfig.speed = 2400;
        break;
    case b4800:
        hc05SerialConfig.speed = 4800;
        break;
    case b9600:
        hc05SerialConfig.speed = 9600;
        break;
    case b19200:
        hc05SerialConfig.speed = 19200;
        break;
    case b38400:
        hc05SerialConfig.speed = 38400;
        break;
    case b57600:
        hc05SerialConfig.speed = 57600;
        break;
    case b115200:
        hc05SerialConfig.speed = 115200;
        break;
    default:
        hc05SerialConfig.speed = BLUETOOTH_DEFAULT_BITRATE;
        break;
    }

    return EXIT_SUCCESS;
}


/*!
 * \brief Starts the given serial driver
 *
 *
 * \param[in] config A BluetoothConfig object
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int hc05_startserial(struct BluetoothConfig *config) {
		
	chprintf((BaseChannel *)&SD3, "Start serial driver\n\r");

    if(!config || !(config->myhc05config))
        return EXIT_FAILURE;

    switch (config->myhc05config->serialdriver) {

#if STM32_SERIAL_USE_USART1 == TRUE
    case sd1:
        config->myhc05config->hc05serialpointer = &SD1;
        sdStart(&SD1, &hc05SerialConfig);
        break;
#endif

#if STM32_SERIAL_USE_USART2 == TRUE
    case sd2:
        config->myhc05config->hc05serialpointer = &SD2;
        sdStart(&SD2, &hc05SerialConfig);
        break;
#endif

#if STM32_SERIAL_USE_USART3 == TRUE
    case sd3:
        config->myhc05config->hc05serialpointer = &SD3;
        sdStart(&SD3, &hc05SerialConfig);
        break;
#endif

    default:
        return EXIT_FAILURE;
        break;
    }
    return EXIT_SUCCESS;
}


/*!
 * \brief Starts the given serial driver
 *
 *
 * \param[in] config A BluetoothConfig object
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int hc05_stopserial(struct BluetoothConfig *config) {
	
	chprintf((BaseChannel *)&SD3, "Stop serial driver\n\r");

    if(!config || !(config->myhc05config))
        return EXIT_FAILURE;

    switch (config->myhc05config->serialdriver) {

#if STM32_SERIAL_USE_USART1 == TRUE
    case sd1:
        sdStop(&SD1);
        break;
#endif
#if STM32_SERIAL_USE_USART2 == TRUE
    case sd2:
        sdStop(&SD2);
        break;
#endif
#if STM32_SERIAL_USE_USART3 == TRUE
    case sd3:
        sdStop(&SD3);
        break;
#endif
    default:
        return EXIT_FAILURE;
        break;
    }
    return EXIT_SUCCESS;
}

/*!
 * \brief Enters HC05 to AT command mode
 *
 *
 * \param[in] config A BluetoothConfig object
 * \param[in] timeout Time to wait in milliseconds
 */
void hc05SetModeAt(struct BluetoothConfig *config, uint16_t timeout) {

	chprintf((BaseChannel *)&SD3, "Set AT mode\n\r");

    if(!config)
        return;

    //reset module (low), pull key high

    palSetPad((((config->myhc05config->keyport) == gpioa_port) ? GPIOA :
               ((config->myhc05config->keyport) == gpiob_port) ? GPIOB :
               ((config->myhc05config->keyport) == gpioc_port) ? GPIOC :
               ((config->myhc05config->keyport) == gpiod_port) ? GPIOD :
               ((config->myhc05config->keyport) == gpioe_port) ? GPIOE :
               NULL),
              (config->myhc05config->keypin));


    chThdSleepMilliseconds(timeout);


    palClearPad((((config->myhc05config->resetport) == gpioa_port) ? GPIOA :
                 ((config->myhc05config->resetport) == gpiob_port) ? GPIOB :
                 ((config->myhc05config->resetport) == gpioc_port) ? GPIOC :
                 ((config->myhc05config->resetport) == gpiod_port) ? GPIOD :
                 ((config->myhc05config->resetport) == gpioe_port) ? GPIOE :
                 NULL),
                (config->myhc05config->resetpin));



    chThdSleepMilliseconds(timeout);   //wait for reset

    palSetPad((((config->myhc05config->resetport) == gpioa_port) ? GPIOA :
               ((config->myhc05config->resetport) == gpiob_port) ? GPIOB :
               ((config->myhc05config->resetport) == gpioc_port) ? GPIOC :
               ((config->myhc05config->resetport) == gpiod_port) ? GPIOD :
               ((config->myhc05config->resetport) == gpioe_port) ? GPIOE :
               NULL),
              (config->myhc05config->resetpin));


    chThdSleepMilliseconds(timeout);   //wait for module recovery
    //we should be in AT mode, with 38400 baud
    hc05CurrentState = st_ready_at_command;
};


/*!
 * \brief Enters HC05 to communication mode
 *
 *
 * \param[in] config A BluetoothConfig object
 * \param[in] timeout Time to wait in milliseconds
 */
void hc05SetModeComm(struct BluetoothConfig *config, uint16_t timeout) {

	chprintf((BaseChannel *)&SD3, "Set comm mode\n\r");

    if(!config)
        return;

    //reset module (low), pull key low
    palClearPad((((config->myhc05config->keyport) == gpioa_port) ? GPIOA :
                 ((config->myhc05config->keyport) == gpiob_port) ? GPIOB :
                 ((config->myhc05config->keyport) == gpioc_port) ? GPIOC :
                 ((config->myhc05config->keyport) == gpiod_port) ? GPIOD :
                 ((config->myhc05config->keyport) == gpioe_port) ? GPIOE :
                 NULL),
                (config->myhc05config->keypin));

    chThdSleepMilliseconds(timeout);


    palClearPad((((config->myhc05config->resetport) == gpioa_port) ? GPIOA :
                 ((config->myhc05config->resetport) == gpiob_port) ? GPIOB :
                 ((config->myhc05config->resetport) == gpioc_port) ? GPIOC :
                 ((config->myhc05config->resetport) == gpiod_port) ? GPIOD :
                 ((config->myhc05config->resetport) == gpioe_port) ? GPIOE :
                 NULL),
                (config->myhc05config->resetpin));


    chThdSleepMilliseconds(timeout);   //wait for reset

    palSetPad((((config->myhc05config->resetport) == gpioa_port) ? GPIOA :
               ((config->myhc05config->resetport) == gpiob_port) ? GPIOB :
               ((config->myhc05config->resetport) == gpioc_port) ? GPIOC :
               ((config->myhc05config->resetport) == gpiod_port) ? GPIOD :
               ((config->myhc05config->resetport) == gpioe_port) ? GPIOE :
               NULL),
              (config->myhc05config->resetpin));


    chThdSleepMilliseconds(timeout);   //wait for module recovery
    hc05CurrentState = st_ready_communication;
};

#endif //HAL_USE_HC_05_BLUETOOTH || defined(__DOXYGEN__)
/** @} */
