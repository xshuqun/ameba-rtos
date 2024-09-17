/*
 * AT Command for Matter
*/
#include <platform_stdlib.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <sys_api.h>
#include <atcmd_service.h>
extern u32 deinitPref(void);
#if MATTER_OTA_REQUESTOR_APP
extern void amebaQueryImageCmdHandler();
extern void amebaApplyUpdateCmdHandler();
#endif
#if defined(CONFIG_MATTER_SECURE) && (CONFIG_MATTER_SECURE == 1)
extern void NS_ENTRY vMatterPrintSecureHeapStatus(void);
#endif
#ifdef CONFIG_EXAMPLE_MATTER_BRIDGE
extern void matter_bridge_add_device(void);
extern void matter_bridge_remove_device(void);
#endif

// Queue for matter shell
QueueHandle_t shell_queue;

void fATchipapp(void)
{
	DiagPrintf("[MATTER] factory reset\n");

	DiagPrintf("xPortGetTotalHeapSize = %d \n",xPortGetTotalHeapSize());
	DiagPrintf("xPortGetFreeHeapSize = %d \n",xPortGetFreeHeapSize());
	DiagPrintf("xPortGetMinimumEverFreeHeapSize = %d \n",xPortGetMinimumEverFreeHeapSize());

	deinitPref();
	
	wifi_disconnect();
	sys_reset();
}

void fATchipapp1(void)
{
#ifdef MATTER_OTA_REQUESTOR_APP
	DiagPrintf("Calling amebaQueryImageCmdHandler\r\n");
	amebaQueryImageCmdHandler();
#endif
}

void fATchipapp2(void)
{
#ifdef MATTER_OTA_REQUESTOR_APP
	DiagPrintf("Chip Test: amebaApplyUpdateCmdHandler\r\n");

	amebaApplyUpdateCmdHandler();
#endif
}

void fATmattershell(void *arg)
{
    if (arg != NULL)
    {
        if(strcmp(arg, "factoryreset") == 0) {
            fATchipapp();
        } else if(strcmp(arg, "queryimage") == 0) {
            fATchipapp1();
        } else if(strcmp(arg, "applyupdate") == 0) {
            fATchipapp2();
#if defined(CONFIG_MATTER_SECURE) && (CONFIG_MATTER_SECURE == 1)
        } else if(strcmp(arg, "secureheapstatus") == 0) {
            vMatterPrintSecureHeapStatus();
#endif
#ifdef CONFIG_EXAMPLE_MATTER_BRIDGE
        } else if(strcmp(arg, "bridge_dm_add") == 0) {
            matter_bridge_add_device();
        } else if(strcmp(arg, "bridge_dm_remove") == 0) {
            matter_bridge_remove_device();
#endif
        } else {
            xQueueSend(shell_queue, arg, pdMS_TO_TICKS(10));
        }
    }
    else
    {
        DiagPrintf("No arguments provided for matter shell, available commands:\n%s\n%s\n%s\n%s\n",
            "ATmatter factoryreset     : to factory reset the matter application",
            "ATmatter queryimage       : query image for matter ota requestor app",
            "ATmatter applyupdate      : apply update for matter ota requestor app",
            "ATmatter help             : to show other matter commands");
#if defined(CONFIG_MATTER_SECURE) && (CONFIG_MATTER_SECURE == 1)
        DiagPrintf( "ATmatter secureheapstatus : to check secure heap status\n");
#endif
    } 
}

log_item_t at_matter_items[] = {
#ifndef CONFIG_INIC_NO_FLASH
    {"matter", fATmattershell, {NULL, NULL}},
#endif
};

void at_matter_init(void)
{
    shell_queue = xQueueCreate(3, 256); // backlog 3 commands max
	atcmd_service_add_table(at_matter_items, sizeof(at_matter_items)/sizeof(at_matter_items[0]));
}

#if SUPPORT_LOG_SERVICE
log_module_init(at_matter_init);
#endif
