// TODO(kiwi) REMOVE AFTER REFACTOR
#pragma ipa file

#include <egg/core.h>
#include <egg/util.h>

namespace EGG {

Exception* Exception::sException;
nw4r::db::ConsoleHandle Exception::sConsoleHandle;
void* Exception::sMapFileBuffer;

bool ExceptionCallBack_(nw4r::db::ConsoleHandle handle, void* pArg) {
    if (!handle) {
        return FALSE;
    }

    return TRUE;
}

Exception* Exception::create(u16 width, u16 height, u16 attr, Heap* pHeap) {
    if (!sException) {
        nw4r::db::Exception_Init();
        nw4r::db::DirectPrint_Init();
        if (!pHeap) {
            pHeap = pHeap->getCurrentHeap();
        }
        sException = new (pHeap) Exception(width, height, attr, pHeap);
    }

    return sException;
}

void Exception::setPadInfo(const u16* pPadInfo) {
    nw4r::db::Exception_SetUserCallback(ExceptionCallBack_, (void*)pPadInfo);
}

Exception::Exception(u16 width, u16 height, u16 attr, Heap* pHeap) {
    void* pBuffer = Heap::alloc((width + 1) * height + 44, 4, pHeap);
    sConsoleHandle =
        nw4r::db::Console_Create(pBuffer, width, height, 18, 0, attr);
    if (!sConsoleHandle) {
        return;
    }

    GXRenderModeObj* pRenderMode = BaseSystem::getVideo()->getRenderModeObj();
    if (!pRenderMode) {
        pRenderMode = &GXNtsc480IntDf;
    }
    nw4r::db::Exception_SetConsole(sConsoleHandle, pRenderMode);
    sConsoleHandle->posX = -120;
    sConsoleHandle->posY = 30;
    sConsoleHandle->isVisible = FALSE;
    sMapFileBuffer = Heap::alloc(16, 4, pHeap);
    nw4r::db::Exception_SetUserCallback(ExceptionCallBack_, 0);
}

} // namespace EGG
