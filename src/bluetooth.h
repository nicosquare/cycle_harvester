/*!
 * @file bluetooth.h
 * @brief Header file for bluetooth module in ChibiosRT.
 *
 * @addtogroup BLUETOOTH
 * @{
 */
#ifndef BLUETOOTH_H_INCLUDED
#define BLUETOOTH_H_INCLUDED

#include <hal.h>
#include <sys/queue.h>
#include <stdlib.h>

#define HAL_USE_BLUETOOTH                 TRUE

#if HAL_USE_BLUETOOTH || defined(__DOXYGEN__) || 1

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    Bluetooth configuration options
 * @{
 */
/**
 * @brief   Default bit rate.
 * @details Configuration parameter, this is the baud rate selected for the
 *          default configuration.
 */
#if !defined(BLUETOOTH_DEFAULT_BITRATE) || defined(__DOXYGEN__)
#define BLUETOOTH_DEFAULT_BITRATE      38400
#endif
/**
 * @brief   Maximum module name length.
 * @details Configuration parameter, this is the maximum name length for the bluetooth module.
 */
#if !defined(BLUETOOTH_MAX_NAME_LENGTH) || defined(__DOXYGEN__)
#define BLUETOOTH_MAX_NAME_LENGTH 32
#endif
/**
 * @brief   Maximum module pin length.
 * @details Configuration parameter, this is the maximum pin code length for the bluetooth module.
 */
#if !defined(BLUETOOTH_MAX_PINCODE_LENGTH) || defined(__DOXYGEN__)
#define BLUETOOTH_MAX_PINCODE_LENGTH 4
#endif
/**
 * @brief   Input buffer size.
 * @details Configuration parameter, this is the input buffer size
 */
#if !defined(BLUETOOTH_INPUT_BUFFER_LENGTH) || defined(__DOXYGEN__)
#define BLUETOOTH_INPUT_BUFFER_SIZE 128
#endif
/**
 * @brief   Output buffer size.
 * @details Configuration parameter, this is the output buffer size
 */
#if !defined(BLUETOOTH_OUTPUT_BUFFER_LENGTH) || defined(__DOXYGEN__)
#define BLUETOOTH_OUTPUT_BUFFER_SIZE 128
#endif

/** @} */

/*===========================================================================*/
/* Forward declarations                                                      */
/*===========================================================================*/

//these should only recieve an "OK\r\n" response from the module

#define BT_AT_TEST(driver) driver->vmt->btSendAtCommand(driver , "AT\r\n");
#define BT_RESET(driver) driver->vmt->btSendAtCommand(driver , "AT+RESET\r\n");
#define BT_RESTORE_DEFAULTS(driver) driver->vmt->btSendAtCommand(driver , "AT+ORGL\r\n");
#define BT_SET_NAME(driver,param) driver->vmt->btSendAtCommand(driver , "AT+NAME="param"\r\n");
#define BT_SET_ROLE(driver,param) driver->vmt->btSendAtCommand(driver , "AT+ROLE="param"\r\n");
#define BT_SET_DEVICE_TYPE(driver,param) driver->vmt->btSendAtCommand(driver , "AT+CLASS="param"\r\n");
#define BT_SET_INQUIRE_ACCESS_CODE(driver,param) driver->vmt->btSendAtCommand(driver , "AT+IAC="param"\r\n");
#define BT_SET_INQUIRE_ACCESS_MODE(driver,param1,param2,param3) driver->vmt->btSendAtCommand(driver , "AT+INQM="param1","param2","param3"\r\n");
#define BT_SET_PASSKEY(driver,param) driver->vmt->btSendAtCommand(driver , "AT+PSWD="param"\r\n");
#define BT_SET_SERIAL_PARAMETERS(driver,param1,param2,param3) driver->vmt->btSendAtCommand(driver , "AT+UART="param1","param2","param3"\r\n");
#define BT_SET_CONNECTION_MODE(driver,param) driver->vmt->btSendAtCommand(driver , "AT+CMODE="param"\r\n");
#define BT_SET_BIND_BT_ADDRESS(driver,param) driver->vmt->btSendAtCommand(driver , "AT+BIND="param"\r\n");
#define BT_SET_LED_DRIVE(driver,param1,param2) driver->vmt->btSendAtCommand(driver , "AT+POLAR="param1","param2"\r\n");
#define BT_SET_PAGE_SCAN_INQUIRE_SCAN(driver,param1,param2,param3,param4) driver->vmt->btSendAtCommand(driver , "AT+IPSCAN="param1","param2","param3","param4"\r\n");
#define BT_SET_SNIFF_ENERGY_PARAMETER(driver,param1,param2,param3,param4) driver->vmt->btSendAtCommand(driver , "AT+SNIFF="param1","param2","param3","param4"\r\n");
#define BT_SET_SAFE_ENCRYPTION_MODE(driver,param1,param2) driver->vmt->btSendAtCommand(driver , "AT+SENM="param1","param2"\r\n");
#define BT_DELETE_AUTHENTICATED_DEVICE(driver,param) driver->vmt->btSendAtCommand(driver , "AT+PMSAD="param"\r\n");
#define BT_DELETE_ALL_AUTHENTICATED_DEVICES(driver) driver->vmt->btSendAtCommand(driver , "AT+RMAAD\r\n");
#define BT_SEEK_AUTHENTICATED_DEVICE(driver,param) driver->vmt->btSendAtCommand(driver , "AT+FSAD="param"\r\n");
#define BT_INIT_SPP_PROFILE(driver) driver->vmt->btSendAtCommand(driver , "AT+INIT\r\n");
#define BT_CANCEL_BT_DEVICE(driver) driver->vmt->btSendAtCommand(driver , "AT+INQC\r\n");
#define BT_SET_PAIR(driver,param1,param2) driver->vmt->btSendAtCommand(driver , "AT+PAIR="param1","param2"\r\n");
#define BT_CONNECT_DEVICE(driver,param) driver->vmt->btSendAtCommand(driver , "AT+LINK="param"\r\n");
#define BT_ENTER_ENERGY_MODE(driver,param) driver->vmt->btSendAtCommand(driver , "AT+ENSNIFF="param"\r\n");
#define BT_EXIT_ENERGY_MODE(driver,param) driver->vmt->btSendAtCommand(driver , "AT+EXSNIFF="param"\r\n");

//these recieve something else from the module

#define BT_GET_ADDRESS(driver) driver->vmt->btSendAtCommand(driver , "AT+ADDR?\r\n");
#define BT_GET_VERSION(driver) driver->vmt->btSendAtCommand(driver , "AT+VERSION?\r\n");
#define BT_GET_NAME(driver) driver->vmt->btSendAtCommand(driver , "AT+NAME?\r\n");
#define BT_GET_ROLE(driver) driver->vmt->btSendAtCommand(driver , "AT+ROLE?\r\n");
#define BT_GET_DEVICE_TYPE(driver) driver->vmt->btSendAtCommand(driver , "AT+CLASS?\r\n");
#define BT_GET_INQUIRE_ACCESS_CODE(driver) driver->vmt->btSendAtCommand(driver , "AT+IAC?\r\n");
#define BT_GET_INQUIRE_ACCESS_MODE(driver) driver->vmt->btSendAtCommand(driver , "AT+INQM?\r\n");
#define BT_GET_REMOTE_NAME(driver, param) driver->vmt->btSendAtCommand(driver , "AT+RNAME?"param"\r\n");
#define BT_GET_PASSKEY(driver) driver->vmt->btSendAtCommand(driver , "AT+PSWD?\r\n");
#define BT_GET_SERIAL_PARAMETERS(driver) driver->vmt->btSendAtCommand(driver , "AT+UART?\r\n");
#define BT_GET_CONNECTION_MODE(driver) driver->vmt->btSendAtCommand(driver , "AT+CMODE?\r\n");
#define BT_GET_BIND_BT_ADDRESS(driver) driver->vmt->btSendAtCommand(driver , "AT+BIND?\r\n");
#define BT_GET_LED_DRIVE(driver) driver->vmt->btSendAtCommand(driver , "AT+POLAR?\r\n");
#define BT_GET_PAGE_SCAN_INQUIRE_SCAN(driver) driver->vmt->btSendAtCommand(driver , "AT+IPSCAN?\r\n");
#define BT_GET_SNIFF_ENERGY_PARAMETER(driver) driver->vmt->btSendAtCommand(driver , "AT+SNIFF?\r\n");
#define BT_GET_SAFE_ENCRYPTION_MODE(driver) driver->vmt->btSendAtCommand(driver , "AT+SENM?\r\n");
#define BT_GET_AUTHENTICATED_DEVICE_COUNT(driver) driver->vmt->btSendAtCommand(driver , "AT+ADCN?\r\n");
#define BT_GET_MOST_RECENT_ADDRESS(driver) driver->vmt->btSendAtCommand(driver , "AT+MRAD?\r\n");
#define BT_GET_WORK_STATUS(driver) driver->vmt->btSendAtCommand(driver , "AT+STATE?\r\n");
#define BT_INQUIRE_BT_DEVICE(driver) driver->vmt->btSendAtCommand(driver , "AT+INQ\r\n");
#define BT_DISCONNECT_DEVICE(driver) driver->vmt->btSendAtCommand(driver , "AT+DISC\r\n");

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief Communication standard bit rates.
 *
 *  Using these instead of writing the bit rate to a 32 bit int should help with 8 and 16 bit compatibility and portability
 */
enum btbitrate_t {
    b1200 = 0,
    b2400 = 1,
    b4800 = 2,
    b9600 = 3,
    b19200 = 4,
    b38400 = 5,
    b57600 = 6,
    b115200 = 7
};

/**
 * @brief Available bluetooth modules to use
 *
 * The "nomodule" entry is reserved, so there is no 0 in the enum
 */
enum btmodule_t {
    nomodule = 0,     //this should not be used
    hc05 = 1
};


/**
 * @brief BluetoothDriver configuration struct.
 *
 *  This struct contains multiple pointers to different bluetooth module config structs.
 *  Only one should be non-NULL. The unused ones shuld be set to NULL.
 *  If the unused pointers are a problem, #ifdef modulname method could be used to remove at coompile time
 *  The usedmodule variable shows which config pointer to use.
 *
 */
struct BluetoothConfig {
    char name[BLUETOOTH_MAX_NAME_LENGTH+1];
    char pincode[BLUETOOTH_MAX_PINCODE_LENGTH+1];
    enum btbitrate_t baudrate;
    thread_t *sendThread;
    thread_t *recieveThread;
    enum btmodule_t usedmodule;

    //config pointers from here
    struct hc05_config_t *myhc05config;

    //config pointers end here

};

/**
 * @brief BluetoothDriver object
 */
struct BluetoothDriver {
    struct BluetoothDeviceVMT *vmt;
    struct BluetoothConfig *config;
    input_queue_t *btInputQueue;
    output_queue_t *btOutputQueue;
    int driverIsReady;
    int commSleepTimeMs;
};


/**
 * @brief BluetoothDriver virtual methods table.
 */

struct BluetoothDeviceVMT {
    int (*sendBuffer)(struct BluetoothDriver *instance, char *buffer, int bufferlength);
    int (*sendByte)(struct BluetoothDriver *instance, int mybyte);
    int (*canRecieve)(struct BluetoothDriver *instance);
    int (*readBuffer)(struct BluetoothDriver *instance, char *buffer, int maxlength);
    int (*setPinCode)(struct BluetoothDriver *instance, char *pin, int pinlength);
    int (*setName)(struct BluetoothDriver *instance, char *newname, int namelength);
    int (*open)(struct BluetoothDriver *instance, struct BluetoothConfig *config);
    int (*close)(struct BluetoothDriver *instance);
    int (*resetModuleSettings) (struct BluetoothDriver * instance);
};


/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
int btSend(struct BluetoothDriver *instance, char *buffer, int bufferlength);
int btSendByte(struct BluetoothDriver *instance, int mybyte);
int btCanRecieve(struct BluetoothDriver *instance);
int btRead(struct BluetoothDriver *instance, char *buffer, int maxlen);
int btOpen(struct BluetoothDriver *instance, struct BluetoothConfig *config);
int btClose(struct BluetoothDriver *instance);
#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_BLUETOOTH */

#endif // BLUETOOTH_H_INCLUDED
/** @} */
