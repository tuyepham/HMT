/*
 * MD2_Board.h
 *
 *  Created on: Nov 13, 2017
 *      Author: dell
 */

#ifndef APPLICATION_MD2_BOARD_H_
#define APPLICATION_MD2_BOARD_H_

#include <ti/drivers/PIN.h>
#include <ti/devices/cc26x0r2/driverlib/ioc.h>

extern const PIN_Config BoardGpioInitTable[];

#define CC2650EM_7ID
//#define CC2640R2EM_CXS
//#define MD2_ES2
#define MD2_CS2

#ifdef MD2_CS2

#define Board_BTN1                IOID_13
#define Board_BTN2                IOID_14

#define MD2_IO0_CTRL              IOID_0
#define MD2_IO1_CTRL              IOID_1

#define MD2_UART_RX               IOID_2          /* Debug: RXD */
#define MD2_UART_TX               IOID_3          /* Debug: TXD */

#define MD2_ACC_INT               IOID_4          /* Accelerometor Interrupt (Input) */
#define MD2_TS_INT                IOID_5          /* Temperature Sensing Interrupt (Input)*/

#define MD2_LED1_PIN              IOID_6          /* I2C SCL  */
#define MD2_LED2_PIN              IOID_7          /* I2C SDA  */

#define MD2_SPI_MISO              IOID_8          /* SPI MISO */
#define MD2_SPI_MOSI              IOID_9          /* SPI MOSI */
#define MD2_SPI_CLK               IOID_10         /* SPI CLK */
#define MD2_SPI_FL_CSn            IOID_11         /* SPI FLASH CN */
#define MD2_SPI_CSn               PIN_UNASSIGNED

#define Board_FLASH_CS_ON       0
#define Board_FLASH_CS_OFF      1

#define MD2_MOTOR_CTRL            IOID_12         /* VIBRATOR ENABLE */
#define MD2_MOTOR_PWM             IOID_15         /* VIBRATOR PWM */

//#define MD2_LCD_BOOST_EN          IOID_13         /* LCD ENABLE */
//#define MD2_LCD_RSTB              IOID_14         /* LCD RSTB */

#define MD2_PROG_RST              IOID_18         /* PROGRAM RESET */

#define MD2_7816_CLK              IOID_19
#define MD2_7816_RST              IOID_20
//#define MD2_SIM_DETECT            IOID_21
#define MD2_7816_IO               IOID_22

#define MD2_MCU_BUTTON_PIN        IOID_13         /* Button Input */
#define MD2_TCH_OUT               IOID_24         /* Touch Input */

#define MD2_CHRG_CDn              IOID_26
#define MD2_CHRG_LSCTRL           IOID_27
#define MD2_CHRG_RESETn           IOID_28
#define MD2_CHRG_INT              IOID_29
#define MD2_MCU_CHRG_MRn          IOID_30
#define MD2_CHRG_PG               IOID_21
#elif defined(MD2_ES2)
#define MD2_UART_RX               IOID_2          /* Debug: RXD */
#define MD2_UART_TX               IOID_3          /* Debug: TXD */

#define MD2_ACC_INT               IOID_4          /* Accelerometor Interrupt (Input) */
#define MD2_TS_INT                IOID_5          /* Temperature Sensing Interrupt (Input)*/

#define MD2_I2C_SCL               IOID_6          /* I2C SCL  */
#define MD2_I2C_SDA               IOID_7          /* I2C SDA  */

#define MD2_SPI_MISO              IOID_8          /* SPI MISO */
#define MD2_SPI_MOSI              IOID_9          /* SPI MOSI */
#define MD2_SPI_CLK               IOID_10         /* SPI CLK */
#define MD2_SPI_FL_CSn            IOID_11         /* SPI FLASH CN */
#define MD2_SPI_CSn               PIN_UNASSIGNED

#define Board_FLASH_CS_ON       0
#define Board_FLASH_CS_OFF      1

#define MD2_MOTOR_CTRL            IOID_12         /* VIBRATOR ENABLE */
#define MD2_MOTOR_PWM             IOID_15         /* VIBRATOR PWM */

#define MD2_LCD_BOOST_EN          IOID_13         /* LCD ENABLE */
#define MD2_LCD_RSTB              IOID_14         /* LCD RSTB */

#define MD2_PROG_RST              IOID_18         /* PROGRAM RESET */

#define MD2_7816_CLK              IOID_19
#define MD2_7816_RST              IOID_20
#define MD2_SIM_DETECT            IOID_21
#define MD2_7816_IO               IOID_22

#define MD2_MCU_BUTTON            IOID_23         /* Button Input */
#define MD2_TCH_OUT               IOID_24         /* Touch Input */

#define MD2_CHRG_CDn              IOID_26
#define MD2_CHRG_LSCTRL           IOID_27
#define MD2_CHRG_RESETn           IOID_28
#define MD2_CHRG_INT              IOID_29
#define MD2_MCU_CHRG_MRn          IOID_30

#else

#define MD2_UART_RX               IOID_2          /* Debug: RXD */
#define MD2_UART_TX               IOID_3          /* Debug: TXD */

#define MD2_ACC_INT               IOID_12          /* Accelerometor Interrupt (Input) */
#define MD2_TS_INT                IOID_30          /* Temperature Sensing Interrupt (Input)*/

#define MD2_I2C_SCL               IOID_6          /* I2C SCL  */
#define MD2_I2C_SDA               IOID_7          /* I2C SDA  */

#define MD2_SPI_MISO              IOID_8          /* SPI MISO */
#define MD2_SPI_MOSI              IOID_9          /* SPI MOSI */
#define MD2_SPI_CLK               IOID_10         /* SPI CLK */
#define MD2_SPI_FL_CSn            IOID_11         /* SPI FLASH CN */
#define MD2_SPI_CSn               PIN_UNASSIGNED

#define Board_FLASH_CS_ON       0
#define Board_FLASH_CS_OFF      1

#define MD2_MOTOR_CTRL            IOID_24         /* VIBRATOR ENABLE */
#define MD2_MOTOR_PWM             PIN_UNASSIGNED         /* VIBRATOR PWM */

#define MD2_LCD_BOOST_EN          IOID_25         /* LCD ENABLE */
#define MD2_LCD_RSTB              IOID_29         /* LCD RSTB */

#define MD2_PROG_RST              PIN_UNASSIGNED         /* PROGRAM RESET */

#define MD2_7816_CLK              IOID_19
#define MD2_7816_RST              IOID_20
#define MD2_SIM_DETECT            PIN_UNASSIGNED
#define MD2_7816_IO               IOID_22

#define MD2_MCU_BUTTON            IOID_16         /* Button Input */
#define MD2_TCH_OUT               IOID_14         /* Touch Input */

#define MD2_CHRG_CDn              IOID_26
#define MD2_CHRG_LSCTRL           IOID_27
#define MD2_CHRG_RESETn           IOID_28
#define MD2_CHRG_INT              PIN_UNASSIGNED
#define MD2_MCU_CHRG_MRn          PIN_UNASSIGNED

#endif


//typedef enum MD2_GPIOName {
//    MD2_TOUCH_INPUT,
//    MD2_BUTTON_INPUT,
//    MD2_LCD_BOOST_EN_OUTPUT,
//    MD2_LCD_RSTB_OUT,
//    MD2_CHRG_RESET,
//    MD2_CHRG_LSCTRL_OUT,
//    MD2_GPIOCOUNT
//} MD2_GPIOName;
typedef enum MD2_GPIOName {
    MD2_LED1,
    MD2_LED2,
    MD2_BUTTON,
    MD2_GPIOCOUNT
} MD2_GPIOName;

/*!
 *  @def    CC2640R2_LAUNCHXL_CryptoName
 *  @brief  Enum of Crypto names
 */
typedef enum MD2_CryptoName {
    MD2_CRYPTO0 = 0,
    MD2_CRYPTOCOUNT
} MD2_CryptoName;

/*!
 *  @def    MD2_GPTimerName
 *  @brief  Enum of GPTimer parts
 */
typedef enum MD2_GPTimerName {
    MD2_GPTIMER0A = 0,
    MD2_GPTIMERPARTSCOUNT,
}MD2_GPTimerName;

/*!
 *  @def    MD2_GPTimers
 *  @brief  Enum of GPTimers
 */
typedef enum MD2_GPTimers {
    MD2_GPTIMER0 = 0,
    MD2_GPTIMERCOUNT,
} MD2_GPTimers;

typedef enum MD2_I2CName {
    MD2_I2C0 = 0,
    MD2_I2CCOUNT
}MD2_I2CName;

typedef enum MD2_SPIName {
    MD2_SPI0 = 0,
    MD2_SPICOUNT
} MD2_SPIName;

/*!
 *  @def    MD2_NVSName
 *  @brief  Enum of NVS names
 */
typedef enum MD2_NVSName {
    MD2_NVSCC26XX0 = 0,
    MD2_NVSSPI25X0,
    MD2_NVSCOUNT
} MD2_NVSName;

/*!
 *  @def    MD2_UARTName
 *  @brief  Enum of UARTs
 */
typedef enum MD2_UARTName {
    MD2_UART0 = 0,
    MD2_UARTCOUNT
} MD2_UARTName;

/*!
 *  @def    MD2_UDMAName
 *  @brief  Enum of DMA buffers
 */
typedef enum MD2_UDMAName {
    MD2_UDMA0 = 0,
    MD2_UDMACOUNT
} MD2_UDMAName;

/*!
 *  @def    MD2_TRNGName
 *  @brief  Enum of TRNG names on the board
 */
typedef enum MD2_TRNGName {
    MD2_TRNG0 = 0,
    MD2_TRNGCOUNT
} MD2_TRNGName;
/*!
 *  @def    MD2_PWMName
 *  @brief  Enum of PWM
 */
typedef enum MD2_PWMName {
    MD2_PWM0,
    MD2_PWMCOUNT,
}MD2_PWMName;

/*!
 *  @def    MD2_WatchdogName
 *  @brief  Enum of Watchdogs
 */
typedef enum MD2_WatchdogName {
    MD2_WATCHDOG0 = 0,
    MD2_WATCHDOGCOUNT
} MD2_WatchdogName;
#endif /* APPLICATION_MD2_BOARD_H_ */
