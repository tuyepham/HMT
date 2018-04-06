/******************************************************************************

 @file usr_memory.c

 @brief main entry of the example application

 Group: WCS LPC
 Target Device: CC13xx

 ******************************************************************************
 
 Copyright (c) 2016-2017, Texas Instruments Incorporated
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
 Release Name: simplelink_cc13x0_sdk_1_30_00_06"
 Release Date: 2017-03-08 14:43:30
 *****************************************************************************/

/******************************************************************************
 Includes
 *****************************************************************************/

/******************************************************************************
 Constants
 *****************************************************************************/

#if defined( USE_FPGA ) || defined( DEBUG_SW_TRACE )
#include <driverlib/ioc.h>
#endif // USE_FPGA | DEBUG_SW_TRACE
/* Assert Reasons */
#include <string.h>
#include <ti/display/Display.h>
#include <ti/mw/extflash/ExtFlash.h>
#include <ti/drivers/NVS.h>
#include <osal_snv.h>
#include "hal_board_cfg.h"
#include "hal_defs.h"
#include "comdef.h"
#include <xdc/runtime/System.h>
#include <GPIO.h>
/* Header files required to enable instruction fetch cache */
#include <inc/hw_memmap.h>
#include <driverlib/vims.h>
#include "Board.h"
#include "usr_memory.h"
#include "user_design.h"

/* Extended Address offset in FCFG (LSB..MSB) */


/******************************************************************************
 External Variables
 *****************************************************************************/
//extern uint16_t Sensor_events;

extern Display_Handle dispHandle;
/******************************************************************************
 Global Variables
 *****************************************************************************/

/* Global memory storage for a PIN_Config table */


/******************************************************************************
 Local Variables
 *****************************************************************************/

//

void test_nv(void)
{
    char write_buffer[15];
    char read_buffer[15] = {0x00};
    int i;

    Display_printf(dispHandle, 0, 0, "test_nv");
//    osal_snv_init();
//    for(i = 0; i < sizeof(read_buffer); i++)
//        write_buffer[i] = TAG_ID_ADDR_0;

    memcpy((char*)write_buffer, "112233445566", 13);
    for(i = 0; i < sizeof(read_buffer); i++)
        Display_printf(dispHandle, 0, 0, "%x ", write_buffer[i]);
    if(SUCCESS != osal_snv_read(TAG_ID_ADDR_0,sizeof(read_buffer), (uint8_t *)read_buffer ))
    {
        Display_printf(dispHandle, 0, 0, "osal_snv_read  fail");
        if(SUCCESS != osal_snv_write(TAG_ID_ADDR_0, sizeof(write_buffer), (uint8_t *)write_buffer ))
               Display_printf(dispHandle, 0, 0, "osal_snv_write  fail");
    }
    Display_printf(dispHandle, 0, 0, "read_buffer: %s", read_buffer);
    for(i = 0; i < sizeof(read_buffer); i++)
        Display_printf(dispHandle, 0, 0, "%x ", read_buffer[i]);
//
//    for(i = 0; i < sizeof(read_buffer); i++)
//        write_buffer[i] = TAG_ID_ADDR_1;
//
//
//    if(SUCCESS != osal_snv_read(TAG_ID_ADDR_1,sizeof(read_buffer), (uint8_t *)read_buffer ))
//    {
//        Display_printf(dispHandle, 0, 0, "osal_snv_read  fail");
//        if(SUCCESS != osal_snv_write(TAG_ID_ADDR_1,sizeof(read_buffer), (uint8_t *)write_buffer ))
//            Display_printf(dispHandle, 0, 0, "osal_snv_write  fail");
//    }
//    for(i = 0; i < sizeof(read_buffer); i++)
//        Display_printf(dispHandle, 0, 0, "%x ", read_buffer[i]);

//    Display_printf(dispHandle, 0, 0, "Reset the device.");
//    Display_printf(dispHandle, 0, 0, "================================================");
}

uint8_t check_id_exist(char *cur_id)
{
    uint8_t i;

    for(i= 0; i< MAX_NUM_USR_ID_SUPPORT; i++)
    {
//      QPRINTF("check_id_exist[%d]: %s %s\r\n", i, tag_id[i].Id, cur_id);
        if(strstr(tag_id[i].Id, cur_id) != NULL)
        {
            Display_printf(dispHandle, 0, 0,"\x1B[32m Exist[%d]: %s -- id_index: %08x \x1B[0m\r\n", i, cur_id, usr_cnt.total_tag);
            return true;
        }
    }
    Display_printf(dispHandle, 0, 0,"Not Exist %s\r\n", cur_id);
    return false;

}

uint8_t save_id(char *cur_id)
{
    uint8_t id;

    if(check_id_exist(cur_id) == false)
    {
//        System_printf(tag_id[usr_cnt.total_tag].Id, "%s" ,cur_id);
        memcpy(tag_id[usr_cnt.total_tag].Id, cur_id, ID_TAG_SIZE);
        Display_printf(dispHandle, 0, 0,"\x1B[32m Saved[%d]: %s\x1B[0m\r\n", usr_cnt.total_tag, tag_id[usr_cnt.total_tag].Id);
//      led_ctrl(usr_cnt.total_tag, ON);
        usr_cnt.total_tag++;
        for(id = 0; id < usr_cnt.total_tag; id++)
        {
            Display_printf(dispHandle, 0, 0,"Cur_id_exist[%d]: %s\r\n", id, tag_id[id].Id);
        }
        return true;
    }

    return false;

}

int training_scan(char *cur_id)
{
    int id;

    button_press.cur = GPIO_read(MD2_BUTTON);
    if(button_press.cur == GPIO_LOW)
    {
        if(button_press.cur != button_press.old)
        {
            Display_printf(dispHandle, 0, 0,"Start Training <---------\r\n");
            usr_cnt.total_tag = 0;
//          all_led_off();
            for(id = 0; id < MAX_NUM_USR_ID_SUPPORT; id++){
                memset(tag_id[id].Id, NULL, 13);
//                Display_printf(dispHandle, 0, 0,"ID[%d]: %s\r\n", id, tag_id[id].Id);
            }
        }
        button_press.old = button_press.cur;
        save_id(cur_id);
        return true;
    }
    else
    {
        if(button_press.cur != button_press.old)
        {
            Display_printf(dispHandle, 0, 0,"Stop Training <---------\r\n");
            usr_cnt.check_tracking = 0;
            save_all_tag_id();
        }
        button_press.old = button_press.cur;
        return false;
    }
}


bool save_tag_id(uint8_t index, char *new_id)
{
    uint16_t addr = TAG_ID_ADDR_0 + index;
    uint8 status_snv;
//    char write_buffer[15];

//    memcpy((char*)write_buffer, "112233445566", 13);
    status_snv = osal_snv_write(addr, ID_TAG_SIZE, (uint8_t *)new_id );
    if(SUCCESS != status_snv)
            Display_printf(dispHandle, 0, 0, "osal_snv_write marker fail");

    return status_snv;
}

bool save_all_tag_id(void)
{
    uint8_t id;
    uint8_t marker[2] = {ROM_MARKER, usr_cnt.total_tag};

    /*total tag*/
    Display_printf(dispHandle, 0, 0,"usr_cnt.total_tag: %d<===========\r\n",usr_cnt.total_tag);
    if(SUCCESS != osal_snv_write(MARKER_ID_ADDR, 2, marker))
        Display_printf(dispHandle, 0, 0, "osal_snv_write marker fail");

    for(id = 0; id < usr_cnt.total_tag; id++)
    {
//        Display_printf(dispHandle, 0, 0, "save id: %s <===", tag_id[id].Id);
        save_tag_id(id, tag_id[id].Id);
    }


    return true;
}

void Read_Params(void)
{
    int id;
//    int i;

    //Read Tag ID
    for(id = 0; id < usr_cnt.total_tag; id++){
        memset(tag_id[id].Id, NULL, 13);
        if(SUCCESS != osal_snv_read(TAG_ID_ADDR_0 + id, ID_TAG_SIZE, (uint8_t *)tag_id[id].Id ))
        {
            Display_printf(dispHandle, 0, 0, "osal_snv_read fail");
        }
//        for(i = 0; i < 13; i++)
//            Display_printf(dispHandle, 0, 0, "%x ", tag_id[id].Id[i]);
    }

}

void Load_Params(void)
{
    uint8_t ROM_CHECK[2];
    uint8_t marker[2] = {ROM_MARKER, usr_cnt.total_tag};
    uint8_t id;
    uint8_t i;

    if(SUCCESS != osal_snv_read(MARKER_ID_ADDR, sizeof(ROM_CHECK), (uint8_t *)ROM_CHECK ))
    {
        Display_printf(dispHandle, 0, 0, "osal_snv_read fail");
//        usr_cnt.total_tag = NUM_USR_ID;
//        if(SUCCESS != osal_snv_write(MARKER_ID_ADDR, sizeof(marker), (uint8_t *)marker ))
//        {
//               Display_printf(dispHandle, 0, 0, "osal_snv_write  fail");
//        }
    }

    Display_printf(dispHandle, 0, 0, "ROM_CHECK: %x %x", ROM_CHECK[0], ROM_CHECK[1]);
    if (ROM_CHECK[0] == ROM_MARKER)
    {
        Display_printf(dispHandle, 0, 0,"-------------------------\r\n");
        Display_printf(dispHandle, 0, 0,"Read parameters\r\n");
        usr_cnt.total_tag = ROM_CHECK[1];
        Read_Params();
    }
    else
    {
        Display_printf(dispHandle, 0, 0,"-------------------------\r\n");
        Display_printf(dispHandle, 0, 0,"Write default parameters\r\n");
        usr_cnt.total_tag = 0;
        for(i = 0; i < usr_cnt.total_tag; i++)
        {
            System_sprintf(tag_id[i].Id, "%s" , ID_DEFAULT);
        }
//        System_sprintf(tag_id[0].Id, "%s" , ID0_TRACKING_DEF);
//        System_sprintf(tag_id[1].Id, "%s" , ID1_TRACKING_DEF);
//        System_sprintf(tag_id[2].Id, "%s" , ID2_TRACKING_DEF);
//        System_sprintf(tag_id[3].Id, "%s" , ID3_TRACKING_DEF);
//        System_sprintf(tag_id[4].Id, "%s" , ID4_TRACKING_DEF);

//        marker[1] = usr_cnt.total_tag;
//        if(SUCCESS != osal_snv_write(MARKER_ID_ADDR, sizeof(marker), (uint8_t *)marker ))
//        {
//               Display_printf(dispHandle, 0, 0, "osal_snv_write  fail");
//        }

        save_all_tag_id();
//        Write_Default_Params();
    }
    Display_printf(dispHandle, 0, 0,"Total tag: %d\r\n", usr_cnt.total_tag);
    for(id = 0; id < usr_cnt.total_tag; id++)
          Display_printf(dispHandle, 0, 0,"ID[%d]: %s", id, tag_id[id].Id);
}
