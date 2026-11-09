#include <nw4r/ef.h>

#include <cstring>

namespace nw4r {
namespace ef {

Resource Resource::mResource;

Resource* Resource::GetInstance() {
    return &mResource;
}

Resource::Resource() {
    Initialize();
}

void Resource::Initialize() {
    ut::List_Init(&mBREFFList, offsetof(EffectProject, projectlink));
    ut::List_Init(&mBREFTList, offsetof(TextureProject, projectlink));
    mNumEmitter = 0;
    mNumTexture = 0;
}

/******************************************************************************
 *
 * Binary resource implementation
 *
 ******************************************************************************/
struct FileHeader {
    char signature[4]; // at 0x0
    u8 byteorder[2];   // at 0x4
    u16 version;       // at 0x6
    u32 filesize;      // at 0x8
    u16 headersize;    // at 0xC
    u16 blocknum;      // at 0xE
};

struct BlockHeader {
    char type[4];   // at 0x0
    u32 headersize; // at 0x4
};

struct NameTable {
    u32 tablesize;   // at 0x0
    u16 numEntry;    // at 0x5
    u16 PADDING_0x4; // at 0x6
};

/* static */ void breffRelocate(EffectProject* pEffProject) {
    u8* ptr = reinterpret_cast<u8*>(pEffProject);
    ptr += pEffProject->headersize;

    u8* pData = ptr;
    NameTable* nameTable = reinterpret_cast<NameTable*>(ptr);

    pData += sizeof(NameTable);

    for (int i = 0; i < nameTable->numEntry; i++) {
        int len = (pData[0] << 8) + pData[1];
        pData += sizeof(u16);

        char* name = reinterpret_cast<char*>(pData);
        pData += len;

        u32 offset =
            (pData[0] << 24) + (pData[1] << 16) + (pData[2] << 8) + pData[3];
        pData += sizeof(u32);

        u32 size =
            (pData[0] << 24) + (pData[1] << 16) + (pData[2] << 8) + pData[3];
        pData += sizeof(u32);

        EmitterResource* eh = reinterpret_cast<EmitterResource*>(
            reinterpret_cast<u8*>(nameTable) + offset);

        // I think they really did this, the member is private...
        *reinterpret_cast<char**>(eh) = name;

        eh->LocateTracks();
    }
}

static u16 breffNumEmitter(EffectProject* pEffProject) {
    u8* ptr = reinterpret_cast<u8*>(pEffProject);
    ptr += pEffProject->headersize;

    NameTable* nameTable = reinterpret_cast<NameTable*>(ptr);
    return nameTable->numEntry;
}

static u16 breftNumTexture(TextureProject* project) {
    u8* ptr = reinterpret_cast<u8*>(project);
    ptr += project->headersize;

    NameTable* nameTable = reinterpret_cast<NameTable*>(ptr);
    return nameTable->numEntry;
}

static EmitterResource* breffSearchName(EffectProject* pEffProject,
                                        const char* pEffName) {

    u8* ptr = reinterpret_cast<u8*>(pEffProject);
    ptr += pEffProject->headersize;

    u8* pData = ptr;
    NameTable* nameTable = reinterpret_cast<NameTable*>(ptr);

    pData += sizeof(NameTable);

    for (int i = 0; i < nameTable->numEntry; i++) {
        int len = (pData[0] << 8) + pData[1];
        pData += sizeof(u16);

        char* name = reinterpret_cast<char*>(pData);
        pData += len;

        u32 offset =
            (pData[0] << 24) + (pData[1] << 16) + (pData[2] << 8) + pData[3];
        pData += sizeof(u32);

        // Skip size
        pData += sizeof(u32);

        if (std::strcmp(pEffName, name) == 0) {
            return reinterpret_cast<EmitterResource*>(
                reinterpret_cast<u8*>(nameTable) + offset);
        }
    }

    return NULL;
}

static EmitterResource* breffIndexOf(EffectProject* pEffProject, u16 idx) {
    u8* ptr = reinterpret_cast<u8*>(pEffProject);
    ptr += pEffProject->headersize;

    u8* pData = ptr;
    NameTable* nameTable = reinterpret_cast<NameTable*>(ptr);

    if (idx >= nameTable->numEntry) {
        return NULL;
    }

    pData += sizeof(NameTable);

    for (int i = 0; i < nameTable->numEntry; i++) {
        int len = (pData[0] << 8) + pData[1];
        pData += sizeof(u16);

        char* pEffName = reinterpret_cast<char*>(pData);
        pData += len;

        u32 offset =
            (pData[0] << 24) + (pData[1] << 16) + (pData[2] << 8) + pData[3];
        pData += sizeof(u32);

        // Skip size
        pData += sizeof(u32);

        if (i == idx) {
            return reinterpret_cast<EmitterResource*>(
                reinterpret_cast<u8*>(nameTable) + offset);
        }
    }

    return NULL;
}

static void breftRelocate(TextureProject* project) {
    u8* ptr = reinterpret_cast<u8*>(project);
    ptr += project->headersize;

    u8* pData = ptr;
    NameTable* nameTable = reinterpret_cast<NameTable*>(ptr);

    pData += sizeof(NameTable);

    for (int i = 0; i < nameTable->numEntry; i++) {
        int len = (pData[0] << 8) + pData[1];
        pData += sizeof(u16);

        char* name = reinterpret_cast<char*>(pData);
        pData += len;

        u32 offset =
            (pData[0] << 24) + (pData[1] << 16) + (pData[2] << 8) + pData[3];
        pData += sizeof(u32);

        u32 size =
            (pData[0] << 24) + (pData[1] << 16) + (pData[2] << 8) + pData[3];
        pData += sizeof(u32);

        TextureData* result = reinterpret_cast<TextureData*>(
            reinterpret_cast<u8*>(nameTable) + offset);

        result->name = name;
        result->texture = reinterpret_cast<u8*>(result) + sizeof(TextureData);
        result->tlut = reinterpret_cast<u8*>(result) + sizeof(TextureData) +
                       result->dataSize;
    }
}

static TextureData* breftSearchName(TextureProject* project,
                                    const char* pTexName) {

    u8* ptr = reinterpret_cast<u8*>(project);
    ptr += project->headersize;

    u8* pData = ptr;
    NameTable* nameTable = reinterpret_cast<NameTable*>(ptr);

    pData += sizeof(NameTable);

    for (int i = 0; i < nameTable->numEntry; i++) {
        int len = (pData[0] << 8) + pData[1];
        pData += sizeof(u16);

        char* name = reinterpret_cast<char*>(pData);
        pData += len;

        u32 offset =
            (pData[0] << 24) + (pData[1] << 16) + (pData[2] << 8) + pData[3];
        pData += sizeof(u32);

        // Skip size
        pData += sizeof(u32);

        if (std::strcmp(pTexName, name) == 0) {
            return reinterpret_cast<TextureData*>(
                reinterpret_cast<u8*>(nameTable) + offset);
        }
    }

    return NULL;
}

/******************************************************************************
 *
 * Resource interface
 *
 ******************************************************************************/
EffectProject* Resource::Add(u8* pData) {
    u8* ptr = pData;

    FileHeader* pFileHeader = reinterpret_cast<FileHeader*>(ptr);
    ptr += pFileHeader->headersize;

    BlockHeader* pBlockHeader = reinterpret_cast<BlockHeader*>(ptr);
    ptr += sizeof(BlockHeader);

    EffectProject* pEffProject = reinterpret_cast<EffectProject*>(ptr);

    memset(&pEffProject->projectlink, 0, sizeof(ut::Link));
    ut::List_Append(&mBREFFList, pEffProject);

    mNumEmitter += breffNumEmitter(pEffProject);
    breffRelocate(pEffProject);

    return pEffProject;
}

ResEmitter Resource::FindEmitter(const char* name,
                                 EffectProject* pEffProject) const {

    EmitterResource* const eh = _FindEmitter(name, pEffProject);
    return ResEmitter(eh);
}

EmitterResource* Resource::_FindEmitter(const char* name,
                                        EffectProject* pEffProject) const {

    EmitterResource* eh;

    if (pEffProject != NULL) {
        eh = breffSearchName(pEffProject, name);
        if (eh != NULL) {
            return eh;
        }
    }

    pEffProject = static_cast<EffectProject*>(ut::List_GetLast(&mBREFFList));

    while (pEffProject != NULL) {
        eh = breffSearchName(pEffProject, name);
        if (eh != NULL) {
            return eh;
        }

        pEffProject = static_cast<EffectProject*>(
            ut::List_GetPrev(&mBREFFList, pEffProject));
    }

    return NULL;
}

ResTexture Resource::FindTexture(const char* name,
                                 TextureProject* project) const {

    TextureData* result = _FindTexture(name, project);
    return ResTexture(result);
}

TextureData* Resource::_FindTexture(const char* name,
                                    TextureProject* project) const {

    TextureData* result;

    if (*name == 0) {
        return NULL;
    }

    if (project != NULL) {
        result = breftSearchName(project, name);
        if (result != NULL) {
            return result;
        }
    }

    project = static_cast<TextureProject*>(ut::List_GetLast(&mBREFTList));

    while (project != NULL) {
        result = breftSearchName(project, name);
        if (result != NULL) {
            return result;
        }

        project = static_cast<TextureProject*>(
            ut::List_GetPrev(&mBREFTList, project));
    }

    return NULL;
}

u32 Resource::NumEmitter(EffectProject* pEffProject) const {
    if (pEffProject != NULL) {
        return breffNumEmitter(pEffProject);
    }

    return mNumEmitter;
}

EmitterResource*
Resource::_GetEmitterIndexOf(u32 idx, EffectProject* pEffProject) const {
    if (pEffProject != NULL) {
        u16 numEmitter = breffNumEmitter(pEffProject);

        if (idx < numEmitter) {
            return breffIndexOf(pEffProject, idx);
        }

        return NULL;
    }

    pEffProject = static_cast<EffectProject*>(ut::List_GetFirst(&mBREFFList));

    while (pEffProject != NULL) {
        u16 numEmitter = breffNumEmitter(pEffProject);

        if (idx < numEmitter) {
            return breffIndexOf(pEffProject, idx);
        }

        idx -= numEmitter;

        pEffProject = static_cast<EffectProject*>(
            ut::List_GetNext(&mBREFFList, pEffProject));
    }

    return NULL;
}

TextureProject* Resource::AddTexture(u8* pData) {
    u8* ptr = pData;

    FileHeader* pFileHeader = reinterpret_cast<FileHeader*>(ptr);
    ptr += pFileHeader->headersize;

    BlockHeader* pBlockHeader = reinterpret_cast<BlockHeader*>(ptr);
    ptr += sizeof(BlockHeader);

    TextureProject* project = reinterpret_cast<TextureProject*>(ptr);

    memset(&project->projectlink, 0, sizeof(ut::Link));
    ut::List_Append(&mBREFTList, project);

    mNumTexture += breftNumTexture(project);
    breftRelocate(project);

    return project;
}

u32 Resource::relocateCurveTexture(u8* cmdPtr, const EmitterResource* eh) {
    NW4R_POINTER_ASSERT(cmdPtr, 685);

    u32 error = 0;

    AnimCurveHeader* header = reinterpret_cast<AnimCurveHeader*>(cmdPtr);

    AnimCurveKey* key = reinterpret_cast<AnimCurveKey*>(
        reinterpret_cast<u8*>(header) + sizeof(AnimCurveHeader));

    AnimCurveRandom* random = reinterpret_cast<AnimCurveRandom*>(
        reinterpret_cast<u8*>(key) + header->keyTable);

    AnimCurveRandomTable* randomTable = reinterpret_cast<AnimCurveRandomTable*>(
        reinterpret_cast<u8*>(random) + header->rangeTable);

    AnimCurveNameTable* nameTable = reinterpret_cast<AnimCurveNameTable*>(
        reinterpret_cast<u8*>(randomTable) + header->randomTable);

    if (!header->nameTable) {
        return NULL;
    }

    u8* name = reinterpret_cast<u8*>(nameTable) +
               offsetof(AnimCurveNameTable, datas) +
               nameTable->count * sizeof(AnimCurveNameTable::Name);

    for (int i = 0; i < nameTable->count; i++) {
        int size = *reinterpret_cast<u16*>(name);
        name += sizeof(u16);

        nameTable->datas[i].work = reinterpret_cast<u32>(
            _FindTexture(reinterpret_cast<char*>(name), NULL));

        if (*name && nameTable->datas[i].work == 0) {
            error++;
            if (eh) {
                NW4R_WARNING(FALSE, 711, "Cannot resolve Texture(%s from:%s)\n",
                             reinterpret_cast<char*>(name), eh->GetName());
            } else {
                NW4R_WARNING(FALSE, 716, "Cannot resolve Texture(%s)\n",
                             reinterpret_cast<char*>(name));
            }
        }

        name += size;
    }

    return error;
}

u32 Resource::relocateCurveChild(u8* pCmdList, const EmitterResource* eh) {
    u32 error = 0;

    AnimCurveHeader* header = reinterpret_cast<AnimCurveHeader*>(pCmdList);

    AnimCurveKey* key = reinterpret_cast<AnimCurveKey*>(
        reinterpret_cast<u8*>(header) + sizeof(AnimCurveHeader));

    AnimCurveRandom* random = reinterpret_cast<AnimCurveRandom*>(
        reinterpret_cast<u8*>(key) + header->keyTable);

    AnimCurveRandomTable* randomTable = reinterpret_cast<AnimCurveRandomTable*>(
        reinterpret_cast<u8*>(random) + header->rangeTable);

    AnimCurveNameTable* nameTable = reinterpret_cast<AnimCurveNameTable*>(
        reinterpret_cast<u8*>(randomTable) + header->randomTable);

    if (!header->nameTable) {
        return NULL;
    }

    u8* name = reinterpret_cast<u8*>(nameTable) +
               offsetof(AnimCurveNameTable, datas) +
               nameTable->count * sizeof(AnimCurveNameTable::Name);

    for (int i = 0; i < nameTable->count; i++) {
        int size = *reinterpret_cast<u16*>(name);
        name += sizeof(u16);

        nameTable->datas[i].work = reinterpret_cast<u32>(
            _FindEmitter(reinterpret_cast<char*>(name), NULL));

        if (nameTable->datas[i].work == 0) {
            error++;
            if (eh) {
                NW4R_WARNING(FALSE, 755, "Cannot resolve Child(%s from:%s)\n",
                             reinterpret_cast<char*>(name), eh->GetName());
            } else {
                NW4R_WARNING(FALSE, 760, "Cannot resolve Child(%s)\n",
                             reinterpret_cast<char*>(name));
            }
        }

        name += size;
    }

    return error;
}

u32 Resource::RelocateCommand() {
    u32 error = 0;

    for (u32 idx = 0; idx < NumEmitter(NULL); idx++) {
        u8 currentError = 0;

        EmitterResource* eh = _GetEmitterIndexOf(idx, NULL);
        if (eh == NULL) {
            continue;
        }

        ParticleParameterDesc* ppd = eh->GetParticleParameterDesc();
        TextureData** work = ppd->mTexture;
        u8* ptr = ppd->textureNames;

        for (int i = 0; i < TEX_LAYER_MAX; i++) {
            u16 size = *reinterpret_cast<u16*>(ptr);
            ptr += sizeof(u16);

            work[i] = _FindTexture(reinterpret_cast<const char*>(ptr), NULL);

            if (ptr[0] != 0 && work[i] == NULL) {
                currentError++;
            }

            ptr += size;
        }

        for (u16 pidx = 0; pidx < eh->NumPtclTrack(); pidx++) {
            u8* ptr = eh->GetPtclTrack(pidx);

#define pTrackAsHeader reinterpret_cast<AnimCurveHeader*>(ptr)

            if (pTrackAsHeader->magic == NW4R_EF_MAGIC_ANIMCURVE ||
                pTrackAsHeader->magic == NW4R_EF_MAGIC_ANIMBAKED) {
                // Load curveFlag and kindEnable
                const u8 curveType =
                    *reinterpret_cast<u8*>(&pTrackAsHeader->curveFlag);

                switch (curveType) {
                case AC_TYPE_PARTICLE_TEXTURE: {
                    currentError += relocateCurveTexture(ptr, eh);
                    break;
                }

                case AC_TYPE_CHILD:
                case AC_TYPE_POSTFIELD: {
                    currentError += relocateCurveChild(ptr, eh);
                    break;
                }
                }
            }

#undef pTrackAsHeader
        }

        EmitterDesc* ed = eh->GetEmitterDesc();
        if (currentError > 0) {
            ed->commonFlag &= ~ed->CMN_FLAG_RELOCATED;
        } else {
            ed->commonFlag |= ed->CMN_FLAG_RELOCATED;
        }

        error += currentError;
    }

    return error;
}

bool Resource::RemoveEffectProject(EffectProject* pEffProject) {
    EffectProject* pIt =
        static_cast<EffectProject*>(ut::List_GetFirst(&mBREFFList));

    for (; pIt != NULL; pIt = static_cast<EffectProject*>(
                            ut::List_GetNext(&mBREFFList, pIt))) {

        if (pIt != pEffProject) {
            continue;
        }

        ut::List_Remove(&mBREFFList, pEffProject);

        mNumEmitter -=
            pEffProject != NULL ? breffNumEmitter(pEffProject) : mNumEmitter;

        return true;
    }

    return false;
}

bool Resource::RemoveTextureProject(TextureProject* project) {
    TextureProject* pIt =
        static_cast<TextureProject*>(ut::List_GetFirst(&mBREFTList));

    for (; pIt != NULL; pIt = static_cast<TextureProject*>(
                            ut::List_GetNext(&mBREFTList, pIt))) {

        if (pIt != project) {
            continue;
        }

        ut::List_Remove(&mBREFTList, project);

        mNumTexture -= project != NULL ? breftNumTexture(project) : mNumTexture;

        return true;
    }

    return false;
}

} // namespace ef
} // namespace nw4r
