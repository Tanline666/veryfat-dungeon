#include <revolution/ESP.h>
#include <revolution/IPC.h>
#include <revolution/OS.h>

#include <string.h>

#define MENU_TITLE_ID 0x0000000100000002
#define TICKET_VIEW_SIZE 0xD8

// Force CW to align the file boundary to 64
u8 FORCE_BSS_ALIGN[0xABCD] ALIGN(64);

static u8 views[0xBD00] ALIGN(32);

BOOL __OSInReboot;

//! TODO(texline): Literally everything...

void __OSGetExecParams(OSExecParams* out) {
    if ((void*)OS_DOL_EXEC_PARAMS >= (void*)0x80000000) {
        memcpy(out, OS_DOL_EXEC_PARAMS, sizeof(OSExecParams));
    } else {
        out->WORD_0x0 = 0;
    }
}
