#ifndef RVL_SDK_DVD_ID_UTILS_H
#define RVL_SDK_DVD_ID_UTILS_H
#include <types.h>

#include <revolution/DVD/dvd.h>
#ifdef __cplusplus
extern "C" {
#endif

BOOL DVDCompareDiskID(const DVDDiskID* id1, const DVDDiskID* id2);
DVDDiskID* DVDGenerateDiskID(DVDDiskID* id, const char* game,
                             const char* publisher, u8 diskNum, u8 version);

#ifdef __cplusplus
}
#endif
#endif
