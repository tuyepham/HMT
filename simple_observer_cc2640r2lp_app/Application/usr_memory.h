/******************************************************************************

 @file usr_memory.h

 @brief This file contains function declarations common to timer/clock
        functions.

 Group: WCS LPC
 Target Device: CC2640R2F LAUNCHXL

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
#ifndef USR_MEMORY_H
#define USR_MEMORY_H

/******************************************************************************
 Includes
 *****************************************************************************/


#ifdef __cplusplus
extern "C"
{
#endif
/*User define*/
#define ROM_MARKER      0xA5
#define ID_TAG_SIZE     13
#define MARKER_ID_ADDR  0x80
#define TAG_ID_ADDR_0   0x81
#define TAG_ID_ADDR_1   0x82
#define TAG_ID_ADDR_2   0x83
#define TAG_ID_ADDR_3   0x84
#define TAG_ID_ADDR_4   0x85
#define TAG_ID_ADDR_5   0x86
#define TAG_ID_ADDR_6   0x87
#define TAG_ID_ADDR_7   0x88
#define TAG_ID_ADDR_8   0x89
#define TAG_ID_ADDR_9   0x8A


void test_nv(void);
void Load_Params(void);
int training_scan(char *cur_id);
bool save_all_tag_id(void);

#ifdef __cplusplus
}
#endif

#endif /* USR_MEMORY_H */
