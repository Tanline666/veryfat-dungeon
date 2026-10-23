#include <revolution/AX.h>
#include <revolution/OS.h>

#define AX_PRIORITY_STACKS (AX_PRIORITY_MAX + 1)

static AXVPB* __AXStackHead[AX_PRIORITY_STACKS];
static AXVPB* __AXStackTail[AX_PRIORITY_STACKS];
static AXVPB* __AXCallbackStack = NULL;

extern s32 __AXMaxVoices;

//! Assertions, debug from TFP2

/**
 * @brief Checks if voices in stack do not exceed max limit
 *
 * @return TRUE if they are under or equal to limit, FALSE if over
 *
 * @note Debug-only; function unused in retail
 */
static u32 __AXCheckStacks(void) {
    u32 i;
    u32 voices;

    voices = 0;

    for (i = 0; i < AX_PRIORITY_STACKS; i++) {
        AXVPB* pVoice = __AXStackHead[i];
        while (pVoice) {
            voices++;
            if (voices > __AXMaxVoices) {
                return FALSE;
            }
            pVoice = pVoice->next;
        }
    }
    return TRUE;
}

AXVPB* __AXGetStackHead(u32 priority) {
#if defined(DEBUG)
    OS_DEFAULT_ASSERT(priority < AX_PRIORITY_STACKS)
#endif
    return __AXStackHead[priority];
}

void __AXServiceCallbackStack(void) {
    AXVPB* it = __AXPopCallbackStack();

    while (it != NULL) {
        if (it->priority > AX_PRIORITY_FREE) {
            if (it->callback != NULL) {
                it->callback(it);
            }

            __AXRemoveFromStack(it);
            __AXPushFreeStack(it);
        }

        it = __AXPopCallbackStack();
    }
}

void __AXInitVoiceStacks(void) {
    u32 prio;

    __AXCallbackStack = NULL;

    for (prio = 0; prio < AX_PRIORITY_STACKS; prio++) {
        __AXStackHead[prio] = __AXStackTail[prio] = NULL;
    }
}

void __AXAllocInit(void) {
#if defined(DEBUG)
    OSReport("Initializing AXAlloc code module\n");
#endif
    __AXInitVoiceStacks();
}

void __AXPushFreeStack(AXVPB* p) {
#if defined(DEBUG)
    OS_DEFAULT_ASSERT(p->priority);
#endif
    p->next = __AXStackHead[AX_PRIORITY_FREE];
    __AXStackHead[AX_PRIORITY_FREE] = p;
    p->priority = AX_PRIORITY_FREE;
}

AXVPB* __AXPopFreeStack(void) {
    AXVPB* head = __AXStackHead[AX_PRIORITY_FREE];

    if (head != NULL) {
        __AXStackHead[AX_PRIORITY_FREE] = head->next;
    }

    return head;
}

void __AXPushCallbackStack(AXVPB* vpb) {
    vpb->next1 = __AXCallbackStack;
    __AXCallbackStack = vpb;
}

AXVPB* __AXPopCallbackStack(void) {
    AXVPB* head = __AXCallbackStack;

    if (head != NULL) {
        __AXCallbackStack = head->next1;
    }

    return head;
}

void __AXRemoveFromStack(AXVPB* p) {
#if defined(DEBUG)
    OS_DEFAULT_ASSERT(p->priority);
#endif
    u32 prio = p->priority;
    AXVPB* head = __AXStackHead[prio];
    AXVPB* tail = __AXStackTail[prio];

    if (head == tail) {
        __AXStackTail[prio] = NULL;
        __AXStackHead[prio] = NULL;
    } else if (p == head) {
        __AXStackHead[prio] = p->next;
        __AXStackHead[prio]->prev = NULL;
    } else if (p == tail) {
        __AXStackTail[prio] = p->prev;
        __AXStackTail[prio]->next = NULL;
    } else {
        head = p->prev;
        tail = p->next;
        head->next = tail;
        tail->prev = head;
    }
}

void __AXPushStackHead(AXVPB* vpb, u32 priority) {
#if defined(DEBUG)
    OS_DEFAULT_ASSERT(priority);
    OS_DEFAULT_ASSERT(priority < AX_PRIORITY_STACKS);
#endif
    vpb->next = __AXStackHead[priority];
    vpb->prev = NULL;

    if (vpb->next != NULL) {
        __AXStackHead[priority]->prev = vpb;
        __AXStackHead[priority] = vpb;
    } else {
        __AXStackTail[priority] = vpb;
        __AXStackHead[priority] = vpb;
    }

    vpb->priority = priority;
}

AXVPB* __AXPopStackFromBottom(u32 priority) {
#if defined(DEBUG)
    OS_DEFAULT_ASSERT(priority);
    OS_DEFAULT_ASSERT(priority < AX_PRIORITY_STACKS);
#endif
    AXVPB* vpb = NULL;

    if (__AXStackHead[priority] != NULL) {
        if (__AXStackHead[priority] == __AXStackTail[priority]) {
            vpb = __AXStackHead[priority];
            __AXStackTail[priority] = NULL;
            __AXStackHead[priority] = NULL;
        } else if (__AXStackTail[priority] != NULL) {
            vpb = __AXStackTail[priority];
            __AXStackTail[priority] = vpb->prev;
            __AXStackTail[priority]->next = NULL;
        }
    }

    return vpb;
}

void AXFreeVoice(AXVPB* p) {
#if defined(DEBUG)
    OS_DEFAULT_ASSERT(p);
    OS_ASSERT(p->priority, "Calling AXFreeVoice() for a voice that is already free\n");
#endif
    BOOL enabled = OSDisableInterrupts();

    __AXRemoveFromStack(p);

    if (p->pb.state == AX_VOICE_RUN) {
        p->depop = TRUE;
    }

    __AXSetPBDefault(p);
    __AXPushFreeStack(p);

#if defined(DEBUG)
    OS_ASSERT(__AXCheckStacks(), "Voice list is trashed!\n");
#endif

    OSRestoreInterrupts(enabled);
}

AXVPB* AXAcquireVoice(u32 prio, AXVoiceCallback callback, u32 userContext) {
    BOOL enabled = OSDisableInterrupts();
    AXVPB* vpb = __AXPopFreeStack();

    if (vpb == NULL) {
        u32 i;

        for (i = AX_PRIORITY_MIN; i < prio; i++) {
            vpb = __AXPopStackFromBottom(i);

            if (vpb != NULL) {
                if (vpb->pb.state == AX_VOICE_RUN) {
                    vpb->depop = TRUE;
                }

                if (vpb->callback != NULL) {
                    vpb->callback(vpb);
                }

                break;
            }
        }
    }

    if (vpb != NULL) {
        __AXPushStackHead(vpb, prio);
        vpb->callback = callback;
        vpb->userContext = userContext;
        __AXSetPBDefault(vpb);
    }

    OSRestoreInterrupts(enabled);
    return vpb;
}

void AXSetVoicePriority(AXVPB* vpb, u32 prio) {
    BOOL enabled = OSDisableInterrupts();

    __AXRemoveFromStack(vpb);
    __AXPushStackHead(vpb, prio);

    OSRestoreInterrupts(enabled);
}
