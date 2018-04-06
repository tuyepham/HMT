/******************************************************************************

 @file       simple_observer.c

 @brief This file contains the Simple Observer sample application for use
        with the CC2650 Bluetooth Low Energy Protocol Stack.

 Group: CMCU, SCS
 Target Device: CC2640R2

 ******************************************************************************
 
 Copyright (c) 2011-2017, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 Release Name: simplelink_cc2640r2_sdk_1_40_00_45
 Release Date: 2017-07-20 17:16:59
 *****************************************************************************/

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
#include "util.h"
#include "GPIO.h"
/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"

#include "observer.h"
#include "board_key.h"
#include "Board.h"

#include "simple_observer.h"
#include "user_design.h"
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

// Maximum number of scan responses
#define DEFAULT_MAX_SCAN_RES                  100

// Scan duration in ms
#define DEFAULT_SCAN_DURATION                 10000



// Type of Display to open
#if !defined(Display_DISABLE_ALL)
  #if defined(BOARD_DISPLAY_USE_LCD) && (BOARD_DISPLAY_USE_LCD!=0)
    #define SBO_DISPLAY_TYPE Display_Type_LCD
  #elif defined (BOARD_DISPLAY_USE_UART) && (BOARD_DISPLAY_USE_UART!=0)
    #define SBO_DISPLAY_TYPE Display_Type_UART
  #else // !BOARD_DISPLAY_USE_LCD && !BOARD_DISPLAY_USE_UART
    #define SBO_DISPLAY_TYPE 0 // Option not supported
  #endif // BOARD_DISPLAY_USE_LCD && BOARD_DISPLAY_USE_UART
#else // BOARD_DISPLAY_USE_LCD && BOARD_DISPLAY_USE_UART
  #define SBO_DISPLAY_TYPE 0 // No Display
#endif // Display_DISABLE_ALL

// Task configuration
#define SBO_TASK_PRIORITY                     1

#ifndef SBO_TASK_STACK_SIZE
#define SBO_TASK_STACK_SIZE                   660
#endif

#define SBO_STATE_CHANGE_EVT                  0x0001
#define SBO_KEY_CHANGE_EVT                    0x0002

// Internal Events for RTOS application
#define SBO_ICALL_EVT                         ICALL_MSG_EVENT_ID // Event_Id_31
#define SBO_QUEUE_EVT                         UTIL_QUEUE_EVENT_ID // Event_Id_30


#define SBO_ALL_EVENTS                        (SBO_ICALL_EVT | SBO_QUEUE_EVT)


/*********************************************************************
 * TYPEDEFS
 */

// App event passed from profiles.
typedef struct
{
  appEvtHdr_t hdr; // event header
  uint8_t *pData;  // event data
} sboEvt_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Display Interface
Display_Handle dispHandle = NULL;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

// Entity ID globally used to check for source and/or destination of messages
static ICall_EntityID selfEntity;

static ICall_SyncHandle syncEvent;

// Clock object used to signal timeout
static Clock_Struct keyChangeClock;

// Queue object used for app messages
static Queue_Struct appMsg;
static Queue_Handle appMsgQueue;

// Task configuration
Task_Struct sboTask;
Char sboTaskStack[SBO_TASK_STACK_SIZE];

// GAP GATT Attributes
//static const uint8 simpleBLEDeviceName[GAP_DEVICE_NAME_LEN] = "Simple Observer";

// Number of scan results and scan result index
static uint8 scanRes = 0 ;
static int8 scanIdx = -1;

// Scan result list
static gapDevRec_t devList[DEFAULT_MAX_SCAN_RES];

// Scanning state
static uint8 scanning = FALSE;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void SimpleBLEObserver_init(void);
static void SimpleBLEObserver_taskFxn(UArg a0, UArg a1);

static void SimpleBLEObserver_handleKeys(uint8_t shift, uint8_t keys);
static void SimpleBLEObserver_processStackMsg(ICall_Hdr *pMsg);
static void SimpleBLEObserver_processAppMsg(sboEvt_t *pMsg);
static void SimpleBLEObserver_processRoleEvent(gapObserverRoleEvent_t *pEvent);
static void SimpleBLEObserver_addDeviceInfo(uint8 *pAddr, uint8 addrType);

static uint8_t SimpleBLEObserver_eventCB(gapObserverRoleEvent_t *pEvent);

static uint8_t SimpleBLEObserver_enqueueMsg(uint8_t event, uint8_t status,
                                            uint8_t *pData);

void SimpleBLEObserver_initKeys(void);

void SimpleBLEObserver_keyChangeHandler(uint8 keys);

uint8_t count_id_match(void);
uint8_t compare_id(char *cur_id);
/*********************************************************************
 * PROFILE CALLBACKS
 */

// GAP Role Callbacks
static const gapObserverRoleCB_t simpleBLERoleCB =
{
  SimpleBLEObserver_eventCB  // Event callback
};



/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SimpleBLEObserver_createTask
 *
 * @brief   Task creation function for the Simple Observer.
 *
 * @param   none
 *
 * @return  none
 */
void SimpleBLEObserver_createTask(void)
{
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = sboTaskStack;
  taskParams.stackSize = SBO_TASK_STACK_SIZE;
  taskParams.priority = SBO_TASK_PRIORITY;

  Task_construct(&sboTask, SimpleBLEObserver_taskFxn, &taskParams, NULL);
}

/*********************************************************************
 * @fn      SimpleBLEObserver_init
 *
 * @brief   Initialization function for the Simple Observer App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notification).
 *
 * @param   none
 *
 * @return  none
 */
void SimpleBLEObserver_init(void)
{
  // ******************************************************************
  // NO STACK API CALLS CAN OCCUR BEFORE THIS CALL TO ICall_registerApp
  // ******************************************************************
  // Register the current thread as an ICall dispatcher application
  // so that the application can send and receive messages.
  ICall_registerApp(&selfEntity, &syncEvent);

  // Hard code the DB Address till CC2650 board gets its own IEEE address
  //uint8 bdAddress[B_ADDR_LEN] = { 0x44, 0x44, 0x44, 0x44, 0x44, 0x44 };
  //HCI_EXT_SetBDADDRCmd(bdAddress);

  // Create an RTOS queue for message from profile to be sent to app.
  appMsgQueue = Util_constructQueue(&appMsg);

  Board_initKeys(SimpleBLEObserver_keyChangeHandler);

  dispHandle = Display_open(SBO_DISPLAY_TYPE, NULL);

  // Setup Observer Profile
  {
    uint8 scanRes = DEFAULT_MAX_SCAN_RES;
    GAPObserverRole_SetParameter(GAPOBSERVERROLE_MAX_SCAN_RES, sizeof(uint8_t),
                                 &scanRes );
  }

  // Setup GAP
  GAP_SetParamValue(TGAP_GEN_DISC_SCAN, DEFAULT_SCAN_DURATION);
  GAP_SetParamValue(TGAP_LIM_DISC_SCAN, DEFAULT_SCAN_DURATION);

  // Start the Device
  VOID GAPObserverRole_StartDevice((gapObserverRoleCB_t *)&simpleBLERoleCB);

//  HCI_EXT_SetTxPowerCmd(HCI_EXT_TX_POWER_5_DBM);
  HCI_EXT_SetRxGainCmd(HCI_EXT_RX_GAIN_HIGH);
  Display_print0(dispHandle, 0, 0, "BLE Observer");

}


/*********************************************************************
 * @fn      SimpleBLEObserver_taskFxn
 *
 * @brief   Application task entry point for the Simple Observer.
 *
 * @param   none
 *
 * @return  none
 */
static void SimpleBLEObserver_taskFxn(UArg a0, UArg a1)
{
  // Initialize application
  SimpleBLEObserver_init();

  // Application main loop
  for (;;)
  {
    uint32_t events;

    events = Event_pend(syncEvent, Event_Id_NONE, SBO_ALL_EVENTS,
                        ICALL_TIMEOUT_FOREVER);


    if (events)
    {
      ICall_EntityID dest;
      ICall_ServiceEnum src;
      ICall_HciExtEvt *pMsg = NULL;

      if (ICall_fetchServiceMsg(&src, &dest,
                                (void **)&pMsg) == ICALL_ERRNO_SUCCESS)
      {

        if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntity))
        {
          // Process inter-task message
          SimpleBLEObserver_processStackMsg((ICall_Hdr *)pMsg);
        }

        if (pMsg)
        {
          ICall_freeMsg(pMsg);
        }
      }
    }


    // If RTOS queue is not empty, process app message
    if (events & SBO_QUEUE_EVT)
    {
      while (!Queue_empty(appMsgQueue))
      {
        sboEvt_t *pMsg = (sboEvt_t *)Util_dequeueMsg(appMsgQueue);
        if (pMsg)
        {
          // Process message
          SimpleBLEObserver_processAppMsg(pMsg);

          // Free the space from the message
          ICall_free(pMsg);
        }
      }
    }


  }
}

/*********************************************************************
 * @fn      SimpleBLEObserver_processStackMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void SimpleBLEObserver_processStackMsg(ICall_Hdr *pMsg)
{
  switch (pMsg->event)
  {
    case GAP_MSG_EVENT:
      SimpleBLEObserver_processRoleEvent((gapObserverRoleEvent_t *)pMsg);
      break;

    default:
      break;
  }
}

/*********************************************************************
 * @fn      SimpleBLEObserver_processAppMsg
 *
 * @brief   Central application event processing function.
 *
 * @param   pMsg - pointer to event structure
 *
 * @return  none
 */
static void SimpleBLEObserver_processAppMsg(sboEvt_t *pMsg)
{
  switch (pMsg->hdr.event)
  {
    case SBO_STATE_CHANGE_EVT:
      SimpleBLEObserver_processStackMsg((ICall_Hdr *)pMsg->pData);

      // Free the stack message
      ICall_freeMsg(pMsg->pData);
      break;

    case SBO_KEY_CHANGE_EVT:
      SimpleBLEObserver_handleKeys(0, pMsg->hdr.state);
      break;

    default:
      // Do nothing.
      break;
  }
}

/*********************************************************************
 * @fn      SimpleBLEObserver_handleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
static void SimpleBLEObserver_handleKeys(uint8 shift, uint8 keys)
{
  (void)shift;  // Intentionally unreferenced parameter

  // Left key determines action to take
  if (keys & KEY_LEFT)
  {
    if (!scanning)
    {
      // Increment index
      scanIdx++;

        if (scanIdx >= scanRes)
        {
          // Prompt the user to begin scanning again.
          scanIdx = -1;
          Display_print0(dispHandle, 2, 0, "");
          Display_print0(dispHandle, 3, 0, "");
          Display_print0(dispHandle, 5, 0, "Discover ->");
        }
        else
        {
          // Display the indexed scanned device.
          Display_print1(dispHandle, 2, 0, "Device %d", (scanIdx + 1));
          Display_print0(dispHandle, 3, 0, Util_convertBdAddr2Str(devList[scanIdx].addr));
          Display_print0(dispHandle, 5, 0, "");
          Display_print0(dispHandle, 6, 0, "<- Next Option");
        }
    }
  }

  // Right key takes the actio the user has selected.
  if (keys & KEY_RIGHT)
  {
    if (scanIdx == -1)
    {
      if (!scanning)
      {
        scanning = TRUE;
        scanRes = 0;

        Display_print0(dispHandle, 2, 0, "Discovering...");
        Display_print0(dispHandle, 3, 0, "");
        Display_print0(dispHandle, 4, 0, "");
        Display_print0(dispHandle, 5, 0, "Cancel Discovery ->");
        Display_print0(dispHandle, 6, 0, "");

        GAPObserverRole_StartDiscovery(DEFAULT_DISCOVERY_MODE,
                                       DEFAULT_DISCOVERY_ACTIVE_SCAN,
                                       DEFAULT_DISCOVERY_WHITE_LIST);
      }
      else
      {
        // Cancel Scanning
        GAPObserverRole_CancelDiscovery();
      }
    }
  }
}

const char *AdvTypeStrings[] = {"Connectable undirected","Connectable directed", "Scannable undirected", "Non-connectable undirected", "Scan response"};
// ...
char *Util_convertBytes2Str(uint8_t *pData, uint8_t length)
{
  uint8_t     charCnt;
  char        hex[] = "0123456789ABCDEF";
  static char str[(3*31)+1];
  char        *pStr = str;

  //*pStr++ = '0';
  //*pStr++ = 'x';

  for (charCnt = 0; charCnt < length; charCnt++)
  {
    *pStr++ = hex[*pData >> 4];
    *pStr++ = hex[*pData++ & 0x0F];
    *pStr++ = ':';
  }
  pStr = NULL;

  return str;
}
/*********************************************************************
 * @fn      SimpleBLEObserver_processRoleEvent
 *
 * @brief   Observer role event processing function.
 *
 * @param   pEvent - pointer to event structure
 *
 * @return  none
 */
static void SimpleBLEObserver_processRoleEvent(gapObserverRoleEvent_t *pEvent)
{
  switch ( pEvent->gap.opcode )
  {
    case GAP_DEVICE_INIT_DONE_EVENT:
      {
        Display_print0(dispHandle, 1, 0, Util_convertBdAddr2Str(pEvent->initDone.devAddr));
        Display_print0(dispHandle, 2, 0, "Initialized");

        // Prompt user to begin scanning.
        Display_print0(dispHandle, 5, 0, "Discover ->");
        GAPObserverRole_StartDiscovery(DEFAULT_DISCOVERY_MODE,
                                       DEFAULT_DISCOVERY_ACTIVE_SCAN,
                                       DEFAULT_DISCOVERY_WHITE_LIST);
      }
      break;

    case GAP_DEVICE_INFO_EVENT:
      {
        SimpleBLEObserver_addDeviceInfo(pEvent->deviceInfo.addr,
                                        pEvent->deviceInfo.addrType);
//        Display_printf(dispHandle, 3, 0,"%s %s - RSSI: %d", Util_convertBdAddr2Str(pEvent->deviceInfo.addr), &pEvent->deviceInfo.pEvtData[2], pEvent->deviceInfo.rssi);
//        int i;
//        for(i = 0; i < pEvent->deviceInfo.dataLen; i++)
//            Display_printf(dispHandle, 3, 0,"%02x %02x", pEvent->deviceInfo.pEvtData[0], pEvent->deviceInfo.pEvtData[1]);
//        Display_printf(dispHandle, 3, 0,"Name:%s - RSSI: %d", &pEvent->deviceInfo.pEvtData[2], pEvent->deviceInfo.rssi);
        if(pEvent->deviceInfo.eventType == GAP_ADRPT_SCAN_RSP)
        {
//            Display_printf(dispHandle, 4, 0, "ScanResponseAddr: %s", Util_convertBdAddr2Str(pEvent->deviceInfo.addr));
//            Display_printf(dispHandle, 5, 0, "ScanResponseData: %s", Util_convertBytes2Str(pEvent->deviceInfo.pEvtData, pEvent->deviceInfo.dataLen));
//            Display_printf(dispHandle, 3, 0,"Name:%s - RSSI: %d", &pEvent->deviceInfo.pEvtData[2], pEvent->deviceInfo.rssi);
            GPIO_toggle(MD2_LED1);
            if(strstr((char *)&pEvent->deviceInfo.pEvtData[2], "iTAG") != NULL)
            {
                uint8_t Id[6];
//                Display_printf(dispHandle, 3, 0,"RSSI: %d", pEvent->deviceInfo.rssi);
//                System_sprintf("ID: %s", Util_convertBdAddr2Str(pEvent->deviceInfo.addr));
                memset(tag_id[0].Name, NULL, 15);
                memset(tag_id[0].Cur_Id, NULL, 15);
//                System_sprintf(tag_id[0].Name, "%s", &pEvent->deviceInfo.pEvtData[2]);
                //System_sprintf(tag_id[0].Cur_Id, "%s", Util_convertBdAddr2Str(pEvent->deviceInfo.addr));
                Id[0] = pEvent->deviceInfo.addr[5];
                Id[1] = pEvent->deviceInfo.addr[4];
                Id[2] = pEvent->deviceInfo.addr[3];
                Id[3] = pEvent->deviceInfo.addr[2];
                Id[4] = pEvent->deviceInfo.addr[1];
                Id[5] = pEvent->deviceInfo.addr[0];
//                memcpy(tag_id[0].Cur_Id, Id, 6);
                System_sprintf(tag_id[0].Cur_Id, "%02x%02x%02x%02x%02x%02x", Id[0],Id[1],Id[2],Id[3],Id[4],Id[5]);
//                Display_printf(dispHandle, 3, 0,"%x%x%x%x%x%x == %s<====", Id[0],Id[1],Id[2],Id[3],Id[4],Id[5], tag_id[0].Cur_Id);
                uevents |= TAG_ID_CMP_EVT;
                Semaphore_post(userSemHandle);
            }
        }
        else if (pEvent->deviceInfo.eventType == 0)
        {
//              Display_printf(dispHandle, 6, 0, "Advertising Addr: %s RSSI: %d Advertising Type: %s", Util_convertBdAddr2Str(pEvent->deviceInfo.addr), pEvent->deviceInfo.rssi, AdvTypeStrings[pEvent->deviceInfo.eventType]);
//              Display_printf(dispHandle, 7, 0, "Advertising Data: %s", Util_convertBytes2Str(pEvent->deviceInfo.pEvtData, pEvent->deviceInfo.dataLen));
//              Display_printf(dispHandle, 3, 0,"Name:%s - RSSI: %d", &pEvent->deviceInfo.pEvtData[2], pEvent->deviceInfo.rssi);
//            Display_printf(dispHandle, 3, 0,"%x %x", pEvent->deviceInfo.pEvtData[0], pEvent->deviceInfo.pEvtData[1]);
            //iTag adv data: 02:01:05:02:0A:00:03:19:C1:03:03:02:E0:FF:31:03:19:00:00:02:01:06:05:03:F0:FF:12:18:85:9B:93:
              GPIO_toggle(MD2_LED2);
            if((pEvent->deviceInfo.pEvtData[0]==0x02) && (pEvent->deviceInfo.pEvtData[1]==0x01) && \
                (pEvent->deviceInfo.pEvtData[2]==0x05) || (pEvent->deviceInfo.pEvtData[3]==0x02) && \
                (pEvent->deviceInfo.pEvtData[4]==0x0A))
            {
                uint8_t Id[6];
//                Display_printf(dispHandle, 3, 0,"RSSI: %d", pEvent->deviceInfo.rssi);
//                System_sprintf("ID: %s", Util_convertBdAddr2Str(pEvent->deviceInfo.addr));
                memset(tag_id[0].Name, NULL, 15);
                memset(tag_id[0].Cur_Id, NULL, 15);
//                System_sprintf(tag_id[0].Name, "%s", &pEvent->deviceInfo.pEvtData[2]);
                //System_sprintf(tag_id[0].Cur_Id, "%s", Util_convertBdAddr2Str(pEvent->deviceInfo.addr));
                Id[0] = pEvent->deviceInfo.addr[5];
                Id[1] = pEvent->deviceInfo.addr[4];
                Id[2] = pEvent->deviceInfo.addr[3];
                Id[3] = pEvent->deviceInfo.addr[2];
                Id[4] = pEvent->deviceInfo.addr[1];
                Id[5] = pEvent->deviceInfo.addr[0];
                tag_id[0].rssi = pEvent->deviceInfo.rssi;
//                memcpy(tag_id[0].Cur_Id, Id, 6);
                System_sprintf(tag_id[0].Cur_Id, "%02x%02x%02x%02x%02x%02x", Id[0],Id[1],Id[2],Id[3],Id[4],Id[5]);
//                Display_printf(dispHandle, 3, 0,"%x%x%x%x%x%x == %s<====", Id[0],Id[1],Id[2],Id[3],Id[4],Id[5], tag_id[0].Cur_Id);
                uevents |= TAG_ID_CMP_EVT;
                Semaphore_post(userSemHandle);
            }
        }


      }
      break;

    case GAP_DEVICE_DISCOVERY_EVENT:
      {

        // Discovery complete.
        scanning = TRUE;

        // Copy results.
//        scanRes = pEvent->discCmpl.numDevs;
//        memcpy(devList, pEvent->discCmpl.pDevList,
//               (sizeof(gapDevRec_t) * pEvent->discCmpl.numDevs));

        Display_print1(dispHandle, 2, 0, "Devices Found %d", scanRes);

//        if ( scanRes > 0 )
//        {
//          Display_print0(dispHandle, 3, 0, "<- To Select");
//        }
//
//        // Initialize scan index.
//        scanIdx = -1;
//
//        // Prompt user that re-performing scanning at this state is possible.
//        Display_print0(dispHandle, 5, 0, "Discover ->");
        GAPObserverRole_StartDiscovery(DEFAULT_DISCOVERY_MODE,
                                       DEFAULT_DISCOVERY_ACTIVE_SCAN,
                                       DEFAULT_DISCOVERY_WHITE_LIST);
      }
      break;

    default:
      break;
  }
}

/*********************************************************************
 * @fn      SimpleBLEObserver_eventCB
 *
 * @brief   Observer event callback function.
 *
 * @param   pEvent - pointer to event structure
 *
 * @return  TRUE if safe to deallocate event message, FALSE otherwise.
 */
static uint8_t SimpleBLEObserver_eventCB(gapObserverRoleEvent_t *pEvent)
{
  // Forward the role event to the application
  if (SimpleBLEObserver_enqueueMsg(SBO_STATE_CHANGE_EVT,
                                   SUCCESS, (uint8_t *)pEvent))
  {
    // App will process and free the event
    return FALSE;
  }

  // Caller should free the event
  return TRUE;
}

/*********************************************************************
 * @fn      SimpleBLEObserver_addDeviceInfo
 *
 * @brief   Add a device to the device discovery result list
 *
 * @return  none
 */
static void SimpleBLEObserver_addDeviceInfo(uint8 *pAddr, uint8 addrType)
{
  uint8 i;

  // If result count not at max
  if ( scanRes < DEFAULT_MAX_SCAN_RES )
  {
    // Check if device is already in scan results
    for ( i = 0; i < scanRes; i++ )
    {
      if (memcmp(pAddr, devList[i].addr, B_ADDR_LEN) == 0)
      {
        return;
      }
    }

    // Add addr to scan result list
    memcpy(devList[scanRes].addr, pAddr, B_ADDR_LEN );
    devList[scanRes].addrType = addrType;

    // Increment scan result count
    scanRes++;
  }
}

/*********************************************************************
 * @fn      SimpleBLEObserver_keyChangeHandler
 *
 * @brief   Key event handler function
 *
 * @param   keys pressed
 *
 * @return  none
 */
void SimpleBLEObserver_keyChangeHandler(uint8 keys)
{
  SimpleBLEObserver_enqueueMsg(SBO_KEY_CHANGE_EVT, keys, NULL);
}


/*********************************************************************
 * @fn      SimpleBLEObserver_enqueueMsg
 *
 * @brief   Creates a message and puts the message in RTOS queue.
 *
 * @param   event - message event.
 * @param   state - message state.
 * @param   pData - message data pointer.
 *
 * @return  TRUE or FALSE
 */
static uint8_t SimpleBLEObserver_enqueueMsg(uint8_t event, uint8_t state,
                                           uint8_t *pData)
{
  sboEvt_t *pMsg;

  // Create dynamic pointer to message.
  if (pMsg = ICall_malloc(sizeof(sboEvt_t)))
  {
    pMsg->hdr.event = event;
    pMsg->hdr.state = state;
    pMsg->pData = pData;

    // Enqueue the message.
    return Util_enqueueMsg(appMsgQueue, syncEvent, (uint8_t *)pMsg);
  }

  return FALSE;
}

/*********************************************************************
*********************************************************************/
