/*
 * MD2_Board.c
 *
 *  Created on: Nov 13, 2017
 *      Author: dell
 */

#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>

#include <ti/devices/cc26x0r2/driverlib/ioc.h>
#include <ti/devices/cc26x0r2/driverlib/udma.h>
#include <ti/devices/cc26x0r2/inc/hw_ints.h>
#include <ti/devices/cc26x0r2/inc/hw_memmap.h>

#include "MD2_Board.h"

/*
 *  =============================== Power ===============================
 */
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

const PowerCC26XX_Config PowerCC26XX_config = {
    .policyInitFxn      = NULL,
    .policyFxn          = &PowerCC26XX_standbyPolicy,
    .calibrateFxn       = &PowerCC26XX_calibrate,
    .enablePolicy       = TRUE,
#ifdef USE_RCOSC
    .calibrateRCOSC_LF  = TRUE,
#else
    .calibrateRCOSC_LF  = FALSE,
#endif
    .calibrateRCOSC_HF  = TRUE,
};

/*
 *  =============================== Crypto ===============================
 */
#include <ti/drivers/crypto/CryptoCC26XX.h>

CryptoCC26XX_Object cryptoCC26XXObjects[MD2_CRYPTOCOUNT];

const CryptoCC26XX_HWAttrs cryptoCC26XXHWAttrs[MD2_CRYPTOCOUNT] = {
    {
        .baseAddr       = CRYPTO_BASE,
        .powerMngrId    = PowerCC26XX_PERIPH_CRYPTO,
        .intNum         = INT_CRYPTO_RESULT_AVAIL_IRQ,
        .intPriority    = ~0,
    }
};

const CryptoCC26XX_Config CryptoCC26XX_config[MD2_CRYPTOCOUNT] = {
    {
         .object  = &cryptoCC26XXObjects[MD2_CRYPTO0],
         .hwAttrs = &cryptoCC26XXHWAttrs[MD2_CRYPTO0]
    },
};


/*
 *  =============================== Display ===============================
 */
// Config Display UART
#include <ti/display/Display.h>
#include <ti/display/DisplayUart.h>

#ifndef BOARD_DISPLAY_UART_STRBUF_SIZE
#define BOARD_DISPLAY_UART_STRBUF_SIZE    128
#endif

DisplayUart_Object     displayUartObject;

static char uartStringBuf[BOARD_DISPLAY_UART_STRBUF_SIZE];

const DisplayUart_HWAttrs displayUartHWAttrs = {
    .uartIdx      = MD2_UART0,
    .baudRate     = 115200,
    .mutexTimeout = (unsigned int)(-1),
    .strBuf       = uartStringBuf,
    .strBufLen    = BOARD_DISPLAY_UART_STRBUF_SIZE,
};

    const Display_Config Display_config[] = {{
         .fxnTablePtr = &DisplayUartMin_fxnTable,
         .object      = &displayUartObject,
         .hwAttrs     = &displayUartHWAttrs,
    }};


const uint_least8_t Display_count = sizeof(Display_config) / sizeof(Display_Config);

/*
 *  =============================== GPIO ===============================
 */

#include <ti/drivers/GPIO.h>
#include <ti/drivers/gpio/GPIOCC26XX.h>

/*
 * Array of Pin configurations
 * NOTE: The order of the pin configurations must coincide with what was
 *       defined in MD2_Board.h
 * NOTE: Pins not used for interrupts should be placed at the end of the
 *       array. Callback entries can be omitted from callbacks array to
 *       reduce memory usage.
 */

//GPIO_PinConfig gpioPinConfigs[] =
//{
//    /* Input pins */
//    MD2_TCH_OUT | GPIO_CFG_IN_PD | GPIO_CFG_IN_INT_BOTH_EDGES,  /* Button 1 */
//    MD2_MCU_BUTTON | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES,  /* Button 1 */
//    /* Output pins */
//    MD2_LCD_BOOST_EN | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_MED | GPIO_CFG_OUT_LOW ,  /* OLED BOOST EN */
//    MD2_LCD_RSTB | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_MED | GPIO_CFG_OUT_LOW,  /* OLED RSTB */
//    MD2_CHRG_RESETn | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_MED | GPIO_CFG_OUT_HIGH,
//    MD2_CHRG_LSCTRL | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_MED | GPIO_CFG_OUT_HIGH,  /* PMIC */
//};

GPIO_PinConfig gpioPinConfigs[] =
{
 /* Output pins */
     MD2_LED1_PIN | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_MED | GPIO_CFG_OUT_LOW ,  /* OLED BOOST EN */
     MD2_LED2_PIN | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_MED | GPIO_CFG_OUT_LOW,  /* OLED RSTB */
     MD2_MCU_BUTTON_PIN | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES,  /* Button 1 */
#ifdef MD2_ES2
    MD2_7816_RST|GPIO_CFG_OUT_STD|GPIO_CFG_OUT_HIGH,
    MD2_PROG_RST|GPIO_CFG_IN_PU,
    //MD2_CHRG_RESETn | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_MED | GPIO_CFG_OUT_HIGH,
    MD2_CHRG_RESETn | GPIO_CFG_IN_PU,
//    MD2_MCU_CHRG_MRn | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_MED | GPIO_CFG_OUT_LOW,
#endif
    MD2_CHRG_LSCTRL | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_MED | GPIO_CFG_OUT_LOW,  /* PMIC */
    /* Input pins */
    MD2_TCH_OUT | GPIO_CFG_IN_PD | GPIO_CFG_IN_INT_BOTH_EDGES,  /* Button 1 */
#ifdef MD2_ES2
    MD2_SIM_DETECT|GPIO_CFG_IN_PU,
#endif


};

GPIO_CallbackFxn gpioCallbackFunctions[] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

const GPIOCC26XX_Config GPIOCC26XX_config = {
    .pinConfigs         = (GPIO_PinConfig *)gpioPinConfigs,
    .callbacks          = (GPIO_CallbackFxn *)gpioCallbackFunctions,
    .numberOfPinConfigs = MD2_GPIOCOUNT,
    .numberOfCallbacks  = sizeof(gpioCallbackFunctions)/sizeof(GPIO_CallbackFxn),
    .intPriority        = (~0)
};

/*
 *  =============================== I2C ===============================
*/

//#include <ti/drivers/I2C.h>
//#include <ti/drivers/i2c/I2CCC26XX.h>
//
//I2CCC26XX_Object i2cCC26xxObjects[MD2_I2CCOUNT];
//
//const I2CCC26XX_HWAttrsV1 i2cCC26xxHWAttrs[MD2_I2CCOUNT] = {
//    {
//        .baseAddr    = I2C0_BASE,
//        .powerMngrId = PowerCC26XX_PERIPH_I2C0,
//        .intNum      = INT_I2C_IRQ,
//        .intPriority = ~0,
//        .swiPriority = 0,
//        .sdaPin      = MD2_I2C_SDA,
//        .sclPin      = MD2_I2C_SCL,
//    }
//};
//
//const I2C_Config I2C_config[MD2_I2CCOUNT] = {
//    {
//        .fxnTablePtr = &I2CCC26XX_fxnTable,
//        .object      = &i2cCC26xxObjects[MD2_I2C0],
//        .hwAttrs     = &i2cCC26xxHWAttrs[MD2_I2C0]
//    },
//};
//
//const uint_least8_t I2C_count = MD2_I2CCOUNT;

/*
 *  =============================== PIN ===============================
 */
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>

const PIN_Config BoardGpioInitTable[] =
{
//    MD2_SPI_CSn | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MIN,  /* External flash chip select */
    MD2_UART_RX | PIN_INPUT_EN | PIN_PULLDOWN,                                              /* UART RX via debugger back channel */
    MD2_UART_TX | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL,                        /* UART TX via debugger back channel */
    MD2_SPI_MOSI | PIN_INPUT_EN | PIN_PULLDOWN,                                            /* SPI master out - slave in */
    MD2_SPI_MISO | PIN_INPUT_EN | PIN_PULLDOWN,                                            /* SPI master in - slave out */
    MD2_SPI_CLK | PIN_INPUT_EN | PIN_PULLDOWN,                                             /* SPI clock */

    PIN_TERMINATE
};

const PINCC26XX_HWAttrs PINCC26XX_hwAttrs = {
    .intPriority = ~0,
    .swiPriority = 0
};



/*
 *  =============================== GPTimer ===============================
 *  Remove unused entries to reduce flash usage both in Board.c and Board.h
 */
#include <ti/drivers/timer/GPTimerCC26XX.h>

GPTimerCC26XX_Object gptimerCC26XXObjects[MD2_GPTIMERCOUNT];

const GPTimerCC26XX_HWAttrs gptimerCC26xxHWAttrs[MD2_GPTIMERPARTSCOUNT] = {
    { .baseAddr = GPT0_BASE, .intNum = INT_GPT0A, .intPriority = (~0), .powerMngrId = PowerCC26XX_PERIPH_GPT0, .pinMux = GPT_PIN_0A, },
};

const GPTimerCC26XX_Config GPTimerCC26XX_config[MD2_GPTIMERPARTSCOUNT] = {
    { &gptimerCC26XXObjects[MD2_GPTIMER0], &gptimerCC26xxHWAttrs[MD2_GPTIMER0A], GPT_A },
};

/*
 *  =============================== NVS ===============================
 */
#include <ti/drivers/NVS.h>
#include <ti/drivers/nvs/NVSSPI25X.h>
#include <ti/drivers/nvs/NVSCC26XX.h>

#define SECTORSIZE 0x1000
#define NVS_REGIONS_BASE 0x1B000

static uint8_t verifyBuf[64];

/*
 * Reserve flash sectors for NVS driver use by placing an uninitialized byte
 * array at the desired flash address.
 */
//#if defined(__TI_COMPILER_VERSION__)

/*
 * Place uninitialized array at NVS_REGIONS_BASE
 */
#pragma LOCATION(flashBuf, NVS_REGIONS_BASE);
#pragma NOINIT(flashBuf);
static char flashBuf[SECTORSIZE * 4];

/* Allocate objects for NVS and NVS SPI */
NVSCC26XX_Object nvsCC26xxObjects[1];
NVSSPI25X_Object nvsSPI25XObjects[1];

/* Hardware attributes for NVS */
const NVSCC26XX_HWAttrs nvsCC26xxHWAttrs[1] = {
    {
        .regionBase = (void *)flashBuf,
        .regionSize = SECTORSIZE * 4,
    },
};

/* Hardware attributes for NVS SPI */
const NVSSPI25X_HWAttrs nvsSPI25XHWAttrs[1] = {
    {
        .regionBaseOffset = 0,
        .regionSize = SECTORSIZE * 4,
        .sectorSize = SECTORSIZE,
        .verifyBuf = verifyBuf,
        .verifyBufSize = 64,
        .spiHandle = NULL,
        .spiIndex = 0,
        .spiBitRate = 4000000,
        .spiCsnGpioIndex = MD2_SPI_CSn,
    },
};

/* NVS Region index 0 and 1 refer to NVS and NVS SPI respectively */
const NVS_Config NVS_config[MD2_NVSCOUNT] = {
    {
        .fxnTablePtr = &NVSCC26XX_fxnTable,
        .object = &nvsCC26xxObjects[0],
        .hwAttrs = &nvsCC26xxHWAttrs[0],
    },
    {
        .fxnTablePtr = &NVSSPI25X_fxnTable,
        .object = &nvsSPI25XObjects[0],
        .hwAttrs = &nvsSPI25XHWAttrs[0],
    },
};

const uint_least8_t NVS_count = MD2_NVSCOUNT;

/*
 *  =============================== RF Driver ===============================
 *  Note: The BLE-Stack requires RF SWI priorities to be set to 5.
 */
#include <ti/drivers/rf/RF.h>

const RFCC26XX_HWAttrs RFCC26XX_hwAttrs = {
    .hwiCpe0Priority = ~0,
    .hwiHwPriority   = ~0,
    .swiCpe0Priority =  5,
    .swiHwPriority   =  5,
};

/*
 *  =============================== SPI DMA ===============================
 */
#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPICC26XXDMA.h>

SPICC26XXDMA_Object spiCC26XXDMAObjects[MD2_SPICOUNT];

const SPICC26XXDMA_HWAttrsV1 spiCC26XXDMAHWAttrs[MD2_SPICOUNT] = {
    {
        .baseAddr           = SSI0_BASE,
        .intNum             = INT_SSI0_COMB,
        .intPriority        = ~0,
        .swiPriority        = 0,
        .powerMngrId        = PowerCC26XX_PERIPH_SSI0,
        .defaultTxBufValue  = 0,
        .rxChannelBitMask   = 1<<UDMA_CHAN_SSI0_RX,
        .txChannelBitMask   = 1<<UDMA_CHAN_SSI0_TX,
        .mosiPin            = MD2_SPI_MOSI,
        .misoPin            = MD2_SPI_MISO,
        .clkPin             = MD2_SPI_CLK,
        .csnPin             = MD2_SPI_CSn
    },
};

const SPI_Config SPI_config[MD2_SPICOUNT] = {
    {
         .fxnTablePtr = &SPICC26XXDMA_fxnTable,
         .object      = &spiCC26XXDMAObjects[MD2_SPI0],
         .hwAttrs     = &spiCC26XXDMAHWAttrs[MD2_SPI0]
    },
};

const uint_least8_t SPI_count = MD2_SPICOUNT;

/*
 *  =============================== UART ===============================
 */
#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTCC26XX.h>

UARTCC26XX_Object uartCC26XXObjects[MD2_UARTCOUNT];

const UARTCC26XX_HWAttrsV2 uartCC26XXHWAttrs[MD2_UARTCOUNT] = {
    {
        .baseAddr       = UART0_BASE,
        .powerMngrId    = PowerCC26XX_PERIPH_UART0,
        .intNum         = INT_UART0_COMB,
        .intPriority    = ~0,
        .swiPriority    = 0,
        .txPin          = MD2_UART_TX,
        .rxPin          = MD2_UART_RX,
        .ctsPin         = PIN_UNASSIGNED,
        .rtsPin         = PIN_UNASSIGNED
    }
};

const UART_Config UART_config[MD2_UARTCOUNT] = {
    {
        .fxnTablePtr = &UARTCC26XX_fxnTable,
        .object      = &uartCC26XXObjects[MD2_UART0],
        .hwAttrs     = &uartCC26XXHWAttrs[MD2_UART0]
    },
};

const uint_least8_t UART_count = MD2_UARTCOUNT;

/*
 *  =============================== UDMA ===============================
 */
#include <ti/drivers/dma/UDMACC26XX.h>

UDMACC26XX_Object udmaObjects[MD2_UDMACOUNT];

const UDMACC26XX_HWAttrs udmaHWAttrs[MD2_UDMACOUNT] = {
    {
        .baseAddr    = UDMA0_BASE,
        .powerMngrId = PowerCC26XX_PERIPH_UDMA,
        .intNum      = INT_DMA_ERR,
        .intPriority = ~0
    }
};

const UDMACC26XX_Config UDMACC26XX_config[MD2_UDMACOUNT] = {
    {
         .object  = &udmaObjects[MD2_UDMA0],
         .hwAttrs = &udmaHWAttrs[MD2_UDMA0]
    },
};

/*
 *  ========================= TRNG begin ====================================
 */
#include <TRNGCC26XX.h>

/* TRNG objects */
TRNGCC26XX_Object trngCC26XXObjects[MD2_TRNGCOUNT];

/* TRNG configuration structure, describing which pins are to be used */
const TRNGCC26XX_HWAttrs TRNGCC26XXHWAttrs[MD2_TRNGCOUNT] = {
    {
        .powerMngrId    = PowerCC26XX_PERIPH_TRNG,
    }
};

/* TRNG configuration structure */
const TRNGCC26XX_Config TRNGCC26XX_config[] = {
    {
         .object  = &trngCC26XXObjects[0],
         .hwAttrs = &TRNGCC26XXHWAttrs[0]
    },
    {NULL, NULL}
};

#include <ti/drivers/PWM.h>
#include <ti/drivers/pwm/PWMTimerCC26XX.h>

PWMTimerCC26XX_Object pwmtimerCC26xxObjects[MD2_PWMCOUNT];

const PWMTimerCC26XX_HwAttrs pwmtimerCC26xxHWAttrs[MD2_PWMCOUNT] = {
{ .pwmPin = MD2_MOTOR_CTRL, .gpTimerUnit = MD2_GPTIMER0A },
};

const PWM_Config PWM_config[MD2_PWMCOUNT] = {
{ &PWMTimerCC26XX_fxnTable, &pwmtimerCC26xxObjects[MD2_PWM0], &pwmtimerCC26xxHWAttrs[MD2_PWM0] },
};

const uint_least8_t PWM_count = MD2_PWMCOUNT;

/*
 *  =============================== Watchdog ===============================
 */
#include <ti/drivers/Watchdog.h>
#include <ti/drivers/watchdog/WatchdogCC26XX.h>

WatchdogCC26XX_Object watchdogCC26XXObjects[MD2_WATCHDOGCOUNT];

const WatchdogCC26XX_HWAttrs watchdogCC26XXHWAttrs[MD2_WATCHDOGCOUNT] = {
    {
        .baseAddr    = WDT_BASE,
        .reloadValue = 1000 /* Reload value in milliseconds */
    },
};

const Watchdog_Config Watchdog_config[MD2_WATCHDOGCOUNT] = {
    {
        .fxnTablePtr = &WatchdogCC26XX_fxnTable,
        .object      = &watchdogCC26XXObjects[MD2_WATCHDOG0],
        .hwAttrs     = &watchdogCC26XXHWAttrs[MD2_WATCHDOG0]
    },
};

const uint_least8_t Watchdog_count = MD2_WATCHDOGCOUNT;
