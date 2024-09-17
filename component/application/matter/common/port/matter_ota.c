/************************** 
* Matter OTA Related 
**************************/
#include "platform_stdlib.h"

#ifdef __cplusplus
 extern "C" {
#endif

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include "stdbool.h"
#include "flash_api.h"
#include "sys_api.h"
#include "chip_porting.h"
#include "os_wrapper.h"
#include "ameba_ota.h"

extern u32 IMG_ADDR[OTA_IMGID_MAX][2];

#define MATTER_OTA_HEADER_SIZE 32
#define MATTER_OTA_SECTOR_SIZE 4096
#define MATTER_OTA_FIRMWARE_LENGTH   0x1EC000

flash_t matter_ota_flash;
ota_context matterCtx = {0};
update_ota_ctrl_info matterOtaCtrl = {0};
update_ota_target_hdr matterOtaTargetHdr = {0};

bool matter_ota_first_sector_written = false;
uint32_t matter_ota_flash_sector_base;
uint32_t matter_ota_new_firmware_addr;

uint8_t matter_ota_header[MATTER_OTA_HEADER_SIZE] = {0};
uint8_t matter_ota_header_size = 0; // variable to track size of ota header
uint8_t matter_ota_buffer[MATTER_OTA_SECTOR_SIZE] = {0}; // 4KB buffer to be written to one sector
uint16_t matter_ota_buffer_size = 0; // variable to track size of buffer

uint8_t matter_ota_get_total_header_size()
{
    return MATTER_OTA_HEADER_SIZE;
}

uint8_t matter_ota_get_current_header_size()
{
    return matter_ota_header_size;
}

void matter_ota_prepare_partition()
{
    // reset header, data buffer and ota context
    matter_ota_first_sector_written = false;
    memset(matter_ota_buffer, 0, sizeof(matter_ota_buffer));
    memset(matter_ota_header, 0, sizeof(matter_ota_header));
    memset(&matterCtx, 0, sizeof(matterCtx));
    memset(&matterOtaCtrl, 0, sizeof(matterOtaCtrl));
    memset(&matterOtaTargetHdr, 0, sizeof(matterOtaTargetHdr));

	matterCtx.otactrl = &matterOtaCtrl;
	matterCtx.otaTargetHdr = &matterOtaTargetHdr;

    matter_ota_header_size = 0;
    matter_ota_buffer_size = 0;

    matterCtx.otactrl->ImgId = OTA_IMGID_APP;
    matterCtx.otactrl->ImageLen = MATTER_OTA_FIRMWARE_LENGTH;

    matterCtx.otaTargetHdr->FileImgHdr[OTA_INDEX_1].ImgID = OTA_IMGID_APP;
    matterCtx.otaTargetHdr->FileImgHdr[OTA_INDEX_2].ImgID = OTA_IMGID_APP;
    matterCtx.otaTargetHdr->FileImgHdr[OTA_INDEX_1].ImgLen = MATTER_OTA_FIRMWARE_LENGTH;
    matterCtx.otaTargetHdr->FileImgHdr[OTA_INDEX_2].ImgLen = MATTER_OTA_FIRMWARE_LENGTH;
    matterCtx.otaTargetHdr->FileImgHdr[OTA_INDEX_1].Offset = 0x1000; //Offset of Manifest in firmware
    matterCtx.otaTargetHdr->FileImgHdr[OTA_INDEX_2].Offset = 0x1000; //Offset of Manifest in firmware

	if (ota_get_cur_index(matterCtx.otactrl->ImgId) == OTA_INDEX_1) {
        matterCtx.otactrl->index = OTA_INDEX_1;
		matterCtx.otactrl->targetIdx = OTA_INDEX_2;
	}
    else
    {
        matterCtx.otactrl->index = OTA_INDEX_2;
		matterCtx.otactrl->targetIdx = OTA_INDEX_1;
	}

    matterCtx.otactrl->FlashAddr = IMG_ADDR[matterCtx.otactrl->ImgId][matterCtx.otactrl->targetIdx] - SPI_FLASH_BASE;
    matter_ota_new_firmware_addr = matterCtx.otactrl->FlashAddr;
    matter_ota_flash_sector_base = matter_ota_new_firmware_addr; // Note that the new fw address must be multiples of 4KB

    ota_printf(_OTA_INFO_, "ImgId: %lu\n", matterCtx.otactrl->ImgId);
    ota_printf(_OTA_INFO_, "index: %lu\n", matterCtx.otactrl->index);
    ota_printf(_OTA_INFO_, "targetIdx: %lu\n", matterCtx.otactrl->targetIdx);
	ota_printf(_OTA_INFO_, "FlashAddr: 0x%08x\n", (unsigned int)matterCtx.otactrl->FlashAddr);
	ota_printf(_OTA_INFO_, "ImageLen: %lu\n", matterCtx.otactrl->ImageLen);
	ota_printf(_OTA_INFO_, "ImgOffset: %lu\n", matterCtx.otactrl->ImgOffset);
}

int8_t matter_ota_store_header(uint8_t *data, uint32_t size)
{
    // check if overflow
    if (size + matter_ota_header_size > MATTER_OTA_HEADER_SIZE)
        return -1;

    memcpy(&(matter_ota_header[matter_ota_header_size]), data, size);
    matter_ota_header_size += size;

    return 1;
}

int8_t matter_ota_flash_burst_write(uint8_t *data, uint32_t size)
{
    if (size == 0)
    {
        return 1; // don't waste time, just return success
    }

    bool overflow = false;
    uint32_t sectorBase = matter_ota_flash_sector_base;
    uint32_t writeLength = MATTER_OTA_SECTOR_SIZE;
    int16_t bufferRemainSize = (int16_t) (MATTER_OTA_SECTOR_SIZE - matter_ota_buffer_size);

    if (!matter_ota_first_sector_written)
    {
        sectorBase += matter_ota_header_size; // leave first 32-bytes for header
        writeLength -= matter_ota_header_size;
        bufferRemainSize -= matter_ota_header_size;
    }

    if (bufferRemainSize >= size)
    {
        memcpy(matter_ota_buffer + matter_ota_buffer_size, data, size);
        matter_ota_buffer_size += size;
    }
    else
    {
        memcpy(matter_ota_buffer + matter_ota_buffer_size, data, bufferRemainSize);
        matter_ota_buffer_size += bufferRemainSize;
        overflow = true;
        size -= bufferRemainSize;
    }

    if (matter_ota_buffer_size == writeLength)
    {
        // buffer is full, time to erase sector and write buffer data to flash
        flash_erase_sector(&matter_ota_flash, matter_ota_flash_sector_base);
        flash_burst_write(&matter_ota_flash, sectorBase, writeLength, matter_ota_buffer);

        if (!matter_ota_first_sector_written)
        {
            matter_ota_first_sector_written = true;
        }

        matter_ota_flash_sector_base += MATTER_OTA_SECTOR_SIZE; // point to next sector
        memset(matter_ota_buffer, 0, sizeof(matter_ota_buffer)); // clear buffer after writing
        matter_ota_buffer_size = 0;
    }

    if (overflow) // write remaining data into the newly cleared buffer
    {
        // TODO: what if it overflows twice?
        memcpy(matter_ota_buffer + matter_ota_buffer_size, data + bufferRemainSize, size);
        matter_ota_buffer_size += size;
    }

    return 1;
}

int8_t matter_ota_flush_last()
{
    if (matter_ota_buffer_size > 0)
    {
        flash_erase_sector(&matter_ota_flash, matter_ota_flash_sector_base);
        flash_burst_write(&matter_ota_flash, matter_ota_flash_sector_base, matter_ota_buffer_size, matter_ota_buffer);

        matter_ota_flash_sector_base += MATTER_OTA_SECTOR_SIZE; // point to next sector
        memset(matter_ota_buffer, 0, sizeof(matter_ota_buffer)); // clear buffer after writing
        matter_ota_buffer_size = 0;
    }

    return 1;
}

int8_t matter_ota_update_signature()
{
    memcpy(&(matterCtx.otaTargetHdr->Manifest[matterCtx.otactrl->index]), matter_ota_header, sizeof(Manifest_TypeDef));
    if (!ota_update_manifest(matterCtx.otaTargetHdr, matterCtx.otactrl->targetIdx, matterCtx.otactrl->index)) {
        return -1;
    }

    return 1;
}

void matter_ota_platform_reset()
{
    rtos_time_delay_ms(20);
    sys_reset();
}

static void matter_ota_abort_task(void *pvParameters)
{
    uint32_t newFWBlkSize = (MATTER_OTA_FIRMWARE_LENGTH - 1) / MATTER_OTA_SECTOR_SIZE + 1;
    DiagPrintf("Cleaning up aborted OTA\r\n");
    DiagPrintf("Erasing %d sectors\r\n", newFWBlkSize);

    if (matter_ota_new_firmware_addr != 0)
    {
        for (size_t i=0; i<newFWBlkSize; i++)
        {
            rtos_time_delay_ms(2); // to avoid undefined behaviour when it suddenly resets the ameba during flash erase
            flash_erase_sector(&matter_ota_flash, matter_ota_new_firmware_addr + (i * MATTER_OTA_SECTOR_SIZE));
        }
    }

    DiagPrintf("Erasing sectors done!\r\n");
    vTaskDelete(NULL);
}

void matter_ota_create_abort_task()
{
    if (xTaskCreate(matter_ota_abort_task, "matter_ota_abort", 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
    {
        DiagPrintf("[%s] Failed to create matter_ota_abort_task\r\n", __FUNCTION__);
    }
}

#ifdef __cplusplus
}
#endif
