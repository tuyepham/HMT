#ifndef USER_DESIGN_H
#define USER_DESIGN_H

#ifdef __cplusplus
extern "C"
{
#endif
#include <ti/sysbios/knl/Semaphore.h>

#define FW_VERSION              "1.0"
#define USER_TASK_STACK_SIZE    704
#define USER_TASK_PRIORITY      1
#define TAG_ID_CMP_EVT                          Event_Id_02
#define TAG_ID_NOT_FOUND_EVT                    Event_Id_03
#define TAG_ID_FOUND_EVT                        Event_Id_04
#define BUTTON_PRESSED                          Event_Id_05
#define BUTTON_RELEASED                         Event_Id_06
#define START_SCAN                              Event_Id_07
#define STOP_SCAN                               Event_Id_08
#define USR_ALL_EVENTS                          (TAG_ID_CMP_EVT | TAG_ID_NOT_FOUND_EVT | TAG_ID_FOUND_EVT | BUTTON_PRESSED | \
                                                 BUTTON_RELEASED | STOP_SCAN | START_SCAN)

/*********************************************************************
 * MACROS
 */
#define TOTAL_TIME_CHECK                60 //s
#define NUM_TAG_NOT_FOUND               1
#define MAX_NUM_USR_ID_SUPPORT          10

#define REAL_TEST
#ifdef REAL_TEST
#define ID_DEFAULT                  "a5a5a5a5a5a5"
#define ID0_TRACKING_DEF            "ffff5000c9d2"
#define ID1_TRACKING_DEF            "ffff5000fb8d"
#define ID2_TRACKING_DEF            "ffff5000cad6"
#define ID3_TRACKING_DEF            "ffff4002383b"
#define ID4_TRACKING_DEF            "ffffa008c05f"
#else
#define ID0_TRACKING_DEF            "0xE77D29EE7304"//"FFFF5000C9D2"
#define ID1_TRACKING_DEF            "0xF5CF9839A427"//"FFFF5000FB8D"
#define ID2_TRACKING_DEF            "0xF0F8F20794DC"//FFFF5000CAD6"
#define ID3_TRACKING_DEF            "0xDF8326517214"//FFFF4002383B"
#define ID4_TRACKING_DEF            "0xE886F9AF81C9"//FFFFA008C05F"
#endif

#define GPIO_LOW    0
#define GPIO_HIGH   1

struct tag_id_t
{
    char Id[15];
    char Name[15];
    char Cur_Id[15];
    int8_t rssi;
};

extern struct tag_id_t tag_id[MAX_NUM_USR_ID_SUPPORT];

struct user_count
{
    volatile uint8_t    total_tag;
    volatile uint8_t    total_scan;
    volatile uint16_t prevent_hang_on_main;
    volatile uint16_t prevent_hang_on_timer;
    volatile uint16_t prevent_hang_ble;
    volatile uint16_t check_tracking;
    volatile uint8_t  count_no_tag;
};

extern struct user_count usr_cnt;

struct button_press_t
{
    uint8_t cur;
    uint8_t old;
};

extern struct button_press_t button_press;
/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task creation function for the Simple Observer.
 */
extern void SimpleBLEObserver_createTask(void);
extern Display_Handle dispHandle;

extern Semaphore_Handle    userSemHandle;
extern uint32_t   uevents;

void USER_createTask(void);
void usr_int(void);
static void Period_Handler(UArg a0);

#ifdef __cplusplus
}
#endif

#endif /* USER_DESIGN_H */
