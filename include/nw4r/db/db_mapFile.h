#ifndef NW4R_DB_MAPFILE_H
#define NW4R_DB_MAPFILE_H
#include <nw4r/types_nw4r.h>

#include <revolution/OS.h>

namespace nw4r {
namespace db {

typedef struct MapFile {
    u8* pMapBuf;                     // at 0x0
    const OSModuleInfo* pModuleInfo; // at 0x4
    s32 fileEntry;                   // at 0x8
    struct MapFile* pNext;           // at 0xC
} MapFile;

typedef MapFile* MapFileHandle;

bool MapFile_Exists(void);
bool MapFile_QuerySymbol(u32 address, u8* pStrBuf, u32 strBufSize);

} // namespace db
} // namespace nw4r

#endif
