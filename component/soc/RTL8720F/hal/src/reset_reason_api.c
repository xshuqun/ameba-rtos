/**************************************************************************//**
 * @file     reset_reason_api.c
 * @brief    This file implements the watch dog timer Mbed HAL API functions.
 *
 * @version  V1.00
 * @date     2019-06-21
 *
 * @note
 *
 ******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/
//#include "hal_sys_ctrl.h"
#include "reset_reason_api.h"
#include "ameba_reset.h"
#include <reboot.h>     /* reset_details_t */
#include "FreeRTOS.h"
#include "task.h"
#define BOOT_REASON_MASK    0x0003FFFF
/** Fetch the reset reason for the last system reset
 *
 * This function must return the contents of the system reset reason registers
 * cast to an appropriate platform independent reset reason. If multiple reset
 * reasons are set this function should return RESET_REASON_MULTIPLE. If the
 * reset reason does not match any existing platform independent value this
 * function should return RESET_REASON_PLATFORM. If no reset reason can be
 * determined this function should return RESET_REASON_UNKNOWN.
 *
 * This function is not idempotent, there is no guarantee that the system
 * reset reason will not be cleared between calls to this function altering the
 * return value between calls.
 *
 * Note: Some platforms contain reset reason registers that persist through
 * system resets. If the registers haven't been cleared before calling this
 * function multiple reasons may be set within the registers. If multiple reset
 * reasons are detected this function will return RESET_REASON_MULTIPLE.
 *
 * @return enum containing the last reset reason for the board.
 */
reset_reason_t hal_reset_reason_get(void)
{
	u32 reset_reason = BOOT_Reason();
	if ((reset_reason & AON_BIT_RSTF_SYS0_GLB) || (reset_reason & AON_BIT_RSTF_SYS1_GLB)) {
		return RESET_REASON_SOFTWARE;
	} else if ((reset_reason & AON_BIT_RSTF_WDG0_GLB) || (reset_reason & AON_BIT_RSTF_WDG1_GLB) || (reset_reason & AON_BIT_RSTF_WDG2_GLB) || (reset_reason & AON_BIT_RSTF_IWDG)) {
		return RESET_REASON_WATCHDOG;
	} else if (reset_reason == 0) {
		return RESET_REASON_POWER_ON;
	} else{
		return RESET_REASON_UNKNOWN;
	}
	
}

/** Fetch the raw platform specific reset reason register value
 *
 * This function must return the raw contents of the system reset reason
 * registers cast to a uint32_t value. If the platform contains reset reasons
 * that span multiple registers/addresses the value should be concatenated into
 * the return type.
 *
 * This function is not idempotent, there is no guarantee that the system
 * reset reason will not be cleared between calls to this function altering the
 * return value between calls.
 *
 * @return value containing the reset reason register for the given platform.
 *         If the platform contains reset reasons across multiple registers they
 *         will be concatenated here.
 */
 
uint32_t hal_reset_reason_get_raw(void)
{
    u32 reset_reason = BOOT_Reason();
    return reset_reason;
}

/** Clear the reset reason from registers
 *
 * Reset the value of the reset status registers, the reset reason will persist
 * between system resets on certain platforms so the registers should be cleared
 * before the system resets. Failing to do so may make it difficult to determine
 * the cause of any subsequent system resets.
 */
void hal_reset_reason_clear(void)
{
    u32 temp;
	temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_REASON_SW);
	/* Clear only boot reason bits [17:0], keep other bits unchanged */
	temp &= ~BOOT_REASON_MASK;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_REASON_SW, temp);
}

/** Set the reset reason to registers
 *
 * Set the value of the reset status registers, to let user applicatoin store
 * the reason before doing reset.
 */
/*void hal_reset_reason_set(reset_reason_t reason)
{
    hal_reset_reason_t hal_reason = HAL_RESET_REASON_SOFTWARE;
    switch(reason){
        case RESET_REASON_SOFTWARE:
            hal_reason = HAL_RESET_REASON_SOFTWARE;
            break;
        case RESET_REASON_WATCHDOG:
            hal_reason = HAL_RESET_REASON_WATCHDOG;
            break;
        default:
            break;
    }
    rtl8710c_reset_reason_set(hal_reason);
}*/

typedef struct {
    unsigned int    magic;
    unsigned int    debug_code;
} DEBUG_CODE_t;
//todo
//RAM_BSS_NOINIT_SECTION DEBUG_CODE_t g_DebugCode;
DEBUG_CODE_t g_DebugCode;
static unsigned int debug_code_in_ram = 0xbeefdead; /* last code before restart */

unsigned int halDebugCodeGet_inRam( void )
{
    DEBUG_CODE_t *pDebugCode = (DEBUG_CODE_t *)&g_DebugCode;

    if( pDebugCode->magic != 0xbeefbabe )
    {
        pDebugCode->magic = 0xbeefbabe;
        pDebugCode->debug_code = 0;
    }

    /* If first get, then return from BSS_NOINIT_SECTION and clear code in BSS_NOINIT_SECTION
       If not first get, then return debug_code_in_ram var */
    if( debug_code_in_ram == 0xbeefdead )
    {
        debug_code_in_ram = pDebugCode->debug_code; /* get last code */
        pDebugCode->debug_code = 0;  /* clear value in RAM*/
    }

    return( debug_code_in_ram );
}

void halDebugCodeSet_inRam( unsigned int flag )
{
    DEBUG_CODE_t *pDebugCode = (DEBUG_CODE_t *)&g_DebugCode;

    if( pDebugCode->magic != 0xbeefbabe )
    {
        pDebugCode->magic = 0xbeefbabe;
        pDebugCode->debug_code = 0;
    }
    else if( debug_code_in_ram == 0xbeefdead ) /* if code exist and first set, backup last code */
    {
        debug_code_in_ram = pDebugCode->debug_code; /* load last code to var */
        pDebugCode->debug_code = 0;  /* clear value */
    }
    pDebugCode->debug_code |= flag;
}


typedef struct {
    unsigned int    magic;
    int  size;
    char szCaller[RESET_NAME_MAX];
    char szTaskName[RESET_NAME_MAX];
} NOINIT_RESET_DETAILS_t;
//todo
//RAM_BSS_NOINIT_SECTION NOINIT_RESET_DETAILS_t gNoinitResetDetails;
NOINIT_RESET_DETAILS_t gNoinitResetDetails;

//#define PRINTF_RESETDETAILS(fmt, ...)  rt_printf(fmt, ##__VA_ARGS__)
#define PRINTF_RESETDETAILS(fmt, ...)

/* __reboot_system() ������ ȣ��ǹǷ�,,, ���� �ּ�ȭ  */
void halResetDetailsWrite_inRam(int size, const char* szCaller)
{
    (void) size;
    static int count = 1;  // count : to prevent infinite loops
    if ((count ++) > 1)
    {
        return;
    }

    // get TaskName
    char *pcCurrentTask = "NoTsk";
    if( xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED )
    {
        pcCurrentTask = pcTaskGetName( NULL );
    }
    PRINTF_RESETDETAILS("\nWRITE size(%d)(%s)(%s)\n", size, szCaller, pcCurrentTask);

    // write NOINIT_RAM area
    gNoinitResetDetails.magic = 0xbeefbabe;
    gNoinitResetDetails.size = size;
    strncpy(gNoinitResetDetails.szCaller, ((szCaller) ? szCaller : "unkown"), RESET_NAME_MAX);
    strncpy(gNoinitResetDetails.szTaskName, pcCurrentTask, RESET_NAME_MAX);
    gNoinitResetDetails.szCaller[RESET_NAME_MAX - 1] = '\0';
    gNoinitResetDetails.szTaskName[RESET_NAME_MAX - 1] = '\0';

    return;
}

NOINIT_RESET_DETAILS_t mResetDetailsCopy; /* �ε��Ŀ� NOINIT ������ �ʱ�ȭ �ؾ���. ���� �����ý� ���� �ȵǱ� ���� */
int halResetDetailsRead_inRam(void *_details)
{
    reset_details_t* pDetailsOutput = (reset_details_t *)_details;
    if (_details == NULL)
    {
        return -1;
    }
    PRINTF_RESETDETAILS("\nREAD size:%d magic:0x%x\n", gNoinitResetDetails.size, gNoinitResetDetails.magic);
    if( gNoinitResetDetails.magic == 0xbeefbabe )  // have VALID values
    {
        // �ֹ�ram ���� ���� �ϰ�,      noinit_ram ������ �ʱ�ȭ ��.
        memcpy(&mResetDetailsCopy, &gNoinitResetDetails, sizeof(NOINIT_RESET_DETAILS_t));    
        memset(&gNoinitResetDetails, 0, sizeof(NOINIT_RESET_DETAILS_t));
    }

    /* if have valid values, then copy to argument var */
    if (mResetDetailsCopy.magic == 0xbeefbabe) // have VALID values
    {
        pDetailsOutput->size = mResetDetailsCopy.size;
        strncpy(pDetailsOutput->funcName, mResetDetailsCopy.szCaller, RESET_NAME_MAX);
        strncpy(pDetailsOutput->taskName, mResetDetailsCopy.szTaskName, RESET_NAME_MAX);
        pDetailsOutput->funcName[RESET_NAME_MAX - 1] = '\0';
        pDetailsOutput->taskName[RESET_NAME_MAX - 1] = '\0';
        return 0;  // return SUCCESS
    }
    else
    {
        pDetailsOutput->size = -1; // means INVALID
        pDetailsOutput->funcName[0] = '\0';
        pDetailsOutput->taskName[0] = '\0';        
        return -1;  // return fail
    }
}

typedef struct {
    unsigned int    magic;
    int  https_dev_reboot_cnt;
} NOINIT_HTTPS_DEV_REBOOT_CNT_t;
//todo
//RAM_BSS_NOINIT_SECTION NOINIT_HTTPS_DEV_REBOOT_CNT_t gNoinitHttpsDevRebootCnt;
NOINIT_HTTPS_DEV_REBOOT_CNT_t gNoinitHttpsDevRebootCnt;

void halHttpsDevRebootCntWrite_inRam(int cnt)
{
    // write NOINIT_RAM area
    gNoinitHttpsDevRebootCnt.magic = 0xbeefbabe;
    gNoinitHttpsDevRebootCnt.https_dev_reboot_cnt = cnt;

    return;
}

NOINIT_HTTPS_DEV_REBOOT_CNT_t mHttpsDevRebootCntCopy;

int halHttpsDevRebootCntRead_inRam(void)
{
    if( gNoinitHttpsDevRebootCnt.magic == 0xbeefbabe )  // have VALID values
    {
        // �ֹ�ram ���� ���� �ϰ�,      noinit_ram ������ �ʱ�ȭ ��.
        memcpy(&mHttpsDevRebootCntCopy, &gNoinitHttpsDevRebootCnt, sizeof(NOINIT_HTTPS_DEV_REBOOT_CNT_t));
        memset(&gNoinitHttpsDevRebootCnt, 0, sizeof(NOINIT_HTTPS_DEV_REBOOT_CNT_t));
    }

    /* if have valid values, then copy to argument var */
    if (mHttpsDevRebootCntCopy.magic == 0xbeefbabe) // have VALID values
    {
        return mHttpsDevRebootCntCopy.https_dev_reboot_cnt;
    }
    else
    {
        return 0;  // return 0
    }
}
