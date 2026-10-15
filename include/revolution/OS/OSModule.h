#ifndef RVL_SDK_OS_MODULE_H
#define RVL_SDK_OS_MODULE_H
#include <types.h>

typedef struct OSModuleLink OSModuleLink;
typedef struct OSModuleInfo OSModuleInfo;

struct OSModuleLink {
    OSModuleInfo* pNext; // at 0x0
    OSModuleInfo* pPrev; // at 0x4
};

struct OSModuleInfo {
    u32 id;            // at 0x0
    OSModuleLink link; // at 0x4
    u32 numSections;   // at 0xC
    u32 sectionOffset; // at 0x10
    u32 nameOffset;    // at 0x14
    u32 nameSize;      // at 0x18
    u32 version;       // at 0x1C
};

#endif
