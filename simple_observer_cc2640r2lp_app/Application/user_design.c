/*********************************************************************
 * INCLUDES
 */
#include <string.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/display/Display.h>
#include <icall.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/BIOS.h>
#include <GPIO.h>
#include "util.h"
/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"
#include "simple_observer.h"

#include "observer.h"
#include "board_key.h"
#include "Board.h"

#include "simple_observer.h"
#include "user_design.h"
#include "usr_memory.h"

static uint8 FoundAll = FALSE;
static uint32_t total_not_found = 0;
Task_Struct         userStruct;
uint8_t             userStack[USER_TASK_STACK_SIZE];
Semaphore_Struct    userSemStruct;
Semaphore_Handle    userSemHandle;
uint32_t   uevents;
static Clock_Handle Period_Handle;
static Clock_Struct Period_Clock;
uint32_t id_index = 0x00000000;
struct user_count usr_cnt = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
struct tag_id_t tag_id[MAX_NUM_USR_ID_SUPPORT];
struct button_press_t button_press;

static void Button_keyCallback(uint_least8_t index)
{
    if(GPIO_read(MD2_BUTTON)==0 )
    {
        uevents |= (BUTTON_PRESSED | TAG_ID_CMP_EVT);
        Semaphore_post(userSemHandle);
    }
    else
    {
        uevents |= TAG_ID_CMP_EVT;
        Semaphore_post(userSemHandle);
    }

}

void Button_Init(void)
{
        GPIO_setCallback(MD2_BUTTON, Button_keyCallback);
//        Util_constructClock(&DEBOUNCE_Clock, Deboune_keyChangeHandler, 25, 0, false, 0);
//        Util_constructClock(&Button_Clock, Button_keyChangeHandler, TIME_SHOW_INFO, 0, false, 0);
//        Util_constructClock(&LCDTimeout_Clock, LCD_Timeout_Handler, TIME_LCD_OFF_5S, 0, false, 0);
}

uint8_t count_id_match(void)
{
    uint8_t i;
    uint8_t number = 0;

    for(i= 0; i< usr_cnt.total_tag; i++)
    {
        if((id_index & (1<<i)) != 0)
            number++;
    }
    return number;
}

uint8_t compare_id(char *cur_id)
{
    uint8_t i;

    for(i= 0; i< usr_cnt.total_tag; i++)
    {
//      QPRINTF("ID[%d]: %s\r\n", i, gId_buf[i]);
//        Display_printf(dispHandle, 2, 0,"\x1B[32mcur_id: %s %s\x1B[0m", cur_id, tag_id[i].Id);
        if(strstr(tag_id[i].Id, cur_id) != NULL)
//        if(memcmp(tag_id[i].Id, cur_id, 6) == 0)
        {
//            Display_printf(dispHandle, 2, 0,"\x1B[32mFound New\x1B[0m");
            if((id_index&(1<<i)) != (1<<i))
            {
                id_index |= (1<<i);
                Display_printf(dispHandle, 2, 0,"\x1B[32mFound[%d]: %s -- id_index: %08x -- rssi: %d\x1B[0m", i, cur_id, id_index, tag_id[0].rssi);
//                Display_printf(dispHandle, 2, 0,"\x1B[32mName: %s\x1B[0m\r\n", tag_id[0].Name);

            }
            usr_cnt.total_scan = count_id_match();
            if((usr_cnt.total_scan == usr_cnt.total_tag) && (FoundAll == FALSE))
            {
                Display_printf(dispHandle, 2, 0,"Found all tag after %dS--> Start new scan\r\n",TOTAL_TIME_CHECK - (Clock_getTimeout(Period_Handle)/Clock_tickPeriod)/10000);
                usr_cnt.count_no_tag = 0;
                FoundAll = FALSE;
                id_index = 0x00;
                uevents |= TAG_ID_FOUND_EVT;
                Semaphore_post(userSemHandle);
                Util_startClock(&Period_Clock);
            }
            return true;
        }
    }
    return false;
}

static void Period_Handler(UArg a0)
{
//    GAPObserverRole_CancelDiscovery();
    usr_cnt.total_scan = count_id_match();
    if(usr_cnt.total_scan >= usr_cnt.total_tag)
    {
//        Display_printf(dispHandle, 0, 0,"\x1B[34m NO WARNING AFTER %ds!!!!! \x1B[0m\r\n", TOTAL_TIME_CHECK);
//              buzzer_gpio_ctrl(OFF);
//              show_led_result_scan();
//        buzzer_off();
        usr_cnt.count_no_tag = 0;
        id_index = 0x00;
        uevents |= TAG_ID_FOUND_EVT;
        Semaphore_post(userSemHandle);
    }
    else
    {
        usr_cnt.count_no_tag++;
//        Display_printf(dispHandle, 0, 0,"\x1B[31m count_no_tag: %d \x1B[0m\r\n", usr_cnt.count_no_tag);
        if(usr_cnt.count_no_tag >= NUM_TAG_NOT_FOUND)
        {
           //Display_printf(dispHandle, 0, 0,"\x1B[31m WARNING!!!!! HELP ME!!!! \x1B[0m\r\n");
//                  buzzer_on(BUZZ_VOL_HIGH);
//              buzzer_gpio_ctrl(ON);
//              show_led_result_scan();
            uevents |= TAG_ID_NOT_FOUND_EVT;
            Semaphore_post(userSemHandle);
            usr_cnt.count_no_tag = 0;
            id_index = 0x00;
        }
    }
//    uevents |= START_SCAN;
//    Semaphore_post(userSemHandle);
    FoundAll = FALSE;
    Util_startClock(&Period_Clock);
}
void usr_int(void)
{
//    uint8_t id;


//    System_sprintf(tag_id[0].Id, "%s" , ID0_TRACKING_DEF);
//    System_sprintf(tag_id[1].Id, "%s" , ID1_TRACKING_DEF);
//    System_sprintf(tag_id[2].Id, "%s" , ID2_TRACKING_DEF);
//    System_sprintf(tag_id[3].Id, "%s" , ID3_TRACKING_DEF);
//    System_sprintf(tag_id[4].Id, "%s" , ID4_TRACKING_DEF);
//    usr_cnt.total_tag = 2;
//    for(id = 0; id < usr_cnt.total_tag; id++)
//        Display_printf(dispHandle, 0, 0,"ID[%d]: %s", id, tag_id[id].Id);
    Display_printf(dispHandle, 0, 0, "Compile: %s %s, Version: %s",__DATE__, __TIME__, FW_VERSION);
    int i;
    for(i=0;i<3;i++){
        GPIO_write(MD2_LED1, 1);
        GPIO_write(MD2_LED2, 1);
        Task_sleep(100000);
        GPIO_write(MD2_LED1, 0);
        GPIO_write(MD2_LED2, 0);
        Task_sleep(100000);
    }
    Period_Handle = Util_constructClock(&Period_Clock, Period_Handler, TOTAL_TIME_CHECK*1000, 0, true, 0);
    Button_Init();
//    test_nv();
    Load_Params();
}

static void User_taskFxn(UArg a0, UArg a1)
{
    Display_printf(dispHandle, 0, 0, "Compile: %s %s, Version: %s",__DATE__, __TIME__, FW_VERSION);
    usr_int();
    // Application main loop
    for (;;)
    {
        /* Get access to resource */
        Semaphore_pend(userSemHandle, BIOS_WAIT_FOREVER);
        if(uevents & TAG_ID_CMP_EVT)
        {
           //compare_id(tag_id[0].Cur_Id);
            if(training_scan(tag_id[0].Cur_Id) == false)
            {
                if(compare_id(tag_id[0].Cur_Id) == true)
                {

                }
            }

//           test_nv();
//           Display_printf(dispHandle, 0, 0, "Button: %d", GPIO_read(MD2_BUTTON));
        }
        if(uevents & TAG_ID_FOUND_EVT)
        {
            Display_printf(dispHandle, 0, 0,"\x1B[34m NO WARNING AFTER %ds - Total Warning: %d\x1B[0m\r\n", TOTAL_TIME_CHECK, total_not_found);
            GPIO_write(MD2_LED1, 0);
            GPIO_write(MD2_LED2, 1);
        }
        if(uevents & TAG_ID_NOT_FOUND_EVT)
        {
            Display_printf(dispHandle, 0, 0,"\x1B[31m WARNING!!!!! - Total Warning: %d\x1B[0m\r\n", total_not_found);
            total_not_found++;
            GPIO_write(MD2_LED1, 1);
            GPIO_write(MD2_LED2, 0);
        }
        if(uevents & BUTTON_PRESSED)
        {
          Display_printf(dispHandle, 0, 0,"Button pressed\r\n");
        }
        if(uevents & BUTTON_RELEASED)
        {
          Display_printf(dispHandle, 0, 0,"Button released\r\n");
          Util_startClock(&Period_Clock);
        }
        if(uevents & START_SCAN)
        {
            Display_printf(dispHandle, 0, 0,"START_SCAN\r\n");
//            GAPObserverRole_StartDiscovery(DEFAULT_DISCOVERY_MODE,
//                                           DEFAULT_DISCOVERY_ACTIVE_SCAN,
//                                           DEFAULT_DISCOVERY_WHITE_LIST);
        }
        if(uevents & STOP_SCAN)
        {
           Display_printf(dispHandle, 0, 0,"STOP_SCAN\r\n");
//           GAPObserverRole_CancelDiscovery();
        }
        uevents &= (~USR_ALL_EVENTS);
    }
}


void USER_createTask(void)
{
    Task_Params taskParams;

    //Semaphore
    Semaphore_Params semParams;
    // Construct a Semaphore object to be use as a resource lock, inital count 1 /
    Semaphore_Params_init(&semParams);
    Semaphore_construct(&userSemStruct, 1, &semParams);

    // Obtain instance handle /
    userSemHandle = Semaphore_handle(&userSemStruct);
    // Configure task
    Task_Params_init(&taskParams);
    taskParams.stack = userStack;
    taskParams.stackSize = USER_TASK_STACK_SIZE;
    taskParams.priority = USER_TASK_PRIORITY;

    Task_construct(&userStruct, (Task_FuncPtr)User_taskFxn, &taskParams, NULL);
}
