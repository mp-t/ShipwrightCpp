#include "global.h"
#include "vt.h"

vu32 gIrqMgrResetStatus = 0;
volatile OSTime sIrqMgrResetTime = 0;
volatile OSTime gIrqMgrRetraceTime = 0;
u32 sIrqMgrRetraceCount = 0;

#define RETRACE_MSG 666
#define PRE_NMI_MSG 669
#define PRENMI450_MSG 671
#define PRENMI480_MSG 672
#define PRENMI500_MSG 673

#define STATUS_IDLE 0
#define STATUS_PRENMI 1
#define STATUS_NMI 2

void IrqMgr_AddClient(IrqMgr* thisx, IrqMgrClient* c, OSMesgQueue* msgQ) {
    OSIntMask prevInt;

    LogUtils_CheckNullPointer("thisx", thisx, "../irqmgr.c", 96);
    LogUtils_CheckNullPointer("c", c, "../irqmgr.c", 97);
    LogUtils_CheckNullPointer("msgQ", msgQ, "../irqmgr.c", 98);

    prevInt = osSetIntMask(1);

    c->queue = msgQ;
    c->prev = thisx->clients;
    thisx->clients = c;

    osSetIntMask(prevInt);

    if (thisx->resetStatus > STATUS_IDLE) {
        osSendMesg(c->queue, (OSMesg) & thisx->prenmiMsg, OS_MESG_NOBLOCK);
    }

    if (thisx->resetStatus >= STATUS_NMI) {
        osSendMesg(c->queue, (OSMesg) & thisx->nmiMsg, OS_MESG_NOBLOCK);
    }
}

void IrqMgr_RemoveClient(IrqMgr* thisx, IrqMgrClient* c) {
    IrqMgrClient* iter = thisx->clients;
    IrqMgrClient* lastIter = NULL;
    OSIntMask prevInt;

    LogUtils_CheckNullPointer("thisx", thisx, "../irqmgr.c", 129);
    LogUtils_CheckNullPointer("c", c, "../irqmgr.c", 130);

    prevInt = osSetIntMask(1);

    while (iter != NULL) {
        if (iter == c) {
            if (lastIter) {
                lastIter->prev = c->prev;
            } else {
                thisx->clients = c->prev;
            }
            break;
        }
        lastIter = iter;
        iter = iter->prev;
    }

    osSetIntMask(prevInt);
}

void IrqMgr_SendMesgForClient(IrqMgr* thisx, OSMesg msg) {
    IrqMgrClient* iter = thisx->clients;

    while (iter != NULL) {
        if (iter->queue->validCount >= iter->queue->msgCount) {
            // "irqmgr_SendMesgForClient: Message queue is overflowing mq=%08x cnt=%d"
            osSyncPrintf(
                VT_COL(RED, WHITE) "irqmgr_SendMesgForClient:メッセージキューがあふれています mq=%08x cnt=%d\n" VT_RST,
                iter->queue, iter->queue->validCount);
        } else {
            osSendMesg(iter->queue, msg, OS_MESG_NOBLOCK);
        }

        iter = iter->prev;
    }
}

void IrqMgr_JamMesgForClient(IrqMgr* thisx, OSMesg msg) {
    IrqMgrClient* iter = thisx->clients;

    while (iter != NULL) {
        if (iter->queue->validCount >= iter->queue->msgCount) {
            // "irqmgr_JamMesgForClient: Message queue is overflowing mq=%08x cnt=%d"
            osSyncPrintf(
                VT_COL(RED, WHITE) "irqmgr_JamMesgForClient:メッセージキューがあふれています mq=%08x cnt=%d\n" VT_RST,
                iter->queue, iter->queue->validCount);
        } else {
            // mistake? the function's name suggests it would use osJamMesg
            osSendMesg(iter->queue, msg, OS_MESG_NOBLOCK);
        }
        iter = iter->prev;
    }
}

void IrqMgr_HandlePreNMI(IrqMgr* thisx) {
    u64 temp = STATUS_PRENMI; // required to match

    gIrqMgrResetStatus = temp;
    thisx->resetStatus = STATUS_PRENMI;
    sIrqMgrResetTime = thisx->resetTime = osGetTime();

    osSetTimer(&thisx->timer, OS_USEC_TO_CYCLES(450000), 0ull, &thisx->queue, (OSMesg)PRENMI450_MSG);
    IrqMgr_JamMesgForClient(thisx, (OSMesg) & thisx->prenmiMsg);
}

void IrqMgr_CheckStack() {
    osSyncPrintf("irqmgr.c: PRENMIから0.5秒経過\n"); // "0.5 seconds after PRENMI"
    if (StackCheck_Check(NULL) == 0) {
        osSyncPrintf("スタックは大丈夫みたいです\n"); // "The stack looks ok"
    } else {
        osSyncPrintf("%c", 7);
        osSyncPrintf(VT_FGCOL(RED));
        // "Stack overflow or dangerous"
        osSyncPrintf("スタックがオーバーフローしたか危険な状態です\n");
        // "Increase stack size early or don't consume stack"
        osSyncPrintf("早々にスタックサイズを増やすか、スタックを消費しないようにしてください\n");
        osSyncPrintf(VT_RST);
    }
}

void IrqMgr_HandlePRENMI450(IrqMgr* thisx) {
    u64 temp = STATUS_NMI; // required to match
    gIrqMgrResetStatus = temp;
    thisx->resetStatus = STATUS_NMI;

    osSetTimer(&thisx->timer, OS_USEC_TO_CYCLES(30000), 0ull, &thisx->queue, (OSMesg)PRENMI480_MSG);
    IrqMgr_SendMesgForClient(thisx, (OSMesg) & thisx->nmiMsg);
}

void IrqMgr_HandlePRENMI480(IrqMgr* thisx) {
    u32 ret;

    osSetTimer(&thisx->timer, OS_USEC_TO_CYCLES(20000), 0ull, &thisx->queue, (OSMesg)PRENMI500_MSG);
    ret = osAfterPreNMI();
    if (ret) {
        // "osAfterPreNMI returned %d !?"
        osSyncPrintf("osAfterPreNMIが %d を返しました！？\n", ret);
        osSetTimer(&thisx->timer, OS_USEC_TO_CYCLES(1000), 0ull, &thisx->queue, (OSMesg)PRENMI480_MSG);
    }
}

void IrqMgr_HandlePRENMI500(IrqMgr* thisx) {
    IrqMgr_CheckStack();
}

void IrqMgr_HandleRetrace(IrqMgr* thisx) {
    if (gIrqMgrRetraceTime == 0ull) {
        if (thisx->retraceTime == 0) {
            thisx->retraceTime = osGetTime();
        } else {
            gIrqMgrRetraceTime = osGetTime() - thisx->retraceTime;
        }
    }
    sIrqMgrRetraceCount++;
    IrqMgr_SendMesgForClient(thisx, (OSMesg) & thisx->retraceMsg);
}

void IrqMgr_ThreadEntry(void* arg0) {
    OSMesg msg;
    IrqMgr* thisx = (IrqMgr*)arg0;
    u8 exit;

    msg = 0;
    osSyncPrintf("ＩＲＱマネージャスレッド実行開始\n"); // "Start IRQ manager thread execution"
    exit = false;

    while (!exit) {
        osRecvMesg(&thisx->queue, &msg, OS_MESG_BLOCK);
        switch ((u32)msg) {
            case RETRACE_MSG:
                IrqMgr_HandleRetrace(thisx);
                break;
            case PRE_NMI_MSG:
                osSyncPrintf("PRE_NMI_MSG\n");
                // "Scheduler: Receives PRE_NMI message"
                osSyncPrintf("スケジューラ：PRE_NMIメッセージを受信\n");
                IrqMgr_HandlePreNMI(thisx);
                break;
            case PRENMI450_MSG:
                osSyncPrintf("PRENMI450_MSG\n");
                // "Scheduler: Receives PRENMI450 message"
                osSyncPrintf("スケジューラ：PRENMI450メッセージを受信\n");
                IrqMgr_HandlePRENMI450(thisx);
                break;
            case PRENMI480_MSG:
                osSyncPrintf("PRENMI480_MSG\n");
                // "Scheduler: Receives PRENMI480 message"
                osSyncPrintf("スケジューラ：PRENMI480メッセージを受信\n");
                IrqMgr_HandlePRENMI480(thisx);
                break;
            case PRENMI500_MSG:
                osSyncPrintf("PRENMI500_MSG\n");
                // "Scheduler: Receives PRENMI500 message"
                osSyncPrintf("スケジューラ：PRENMI500メッセージを受信\n");
                exit = true;
                IrqMgr_HandlePRENMI500(thisx);
                break;
            default:
                // "Unexpected message received"
                osSyncPrintf("irqmgr.c:予期しないメッセージを受け取りました(%08x)\n", msg);
                break;
        }
    }

    osSyncPrintf("ＩＲＱマネージャスレッド実行終了\n"); // "End of IRQ manager thread execution"
}

void IrqMgr_Init(IrqMgr* thisx, void* stack, OSPri pri, u8 retraceCount) {
    LogUtils_CheckNullPointer("thisx", thisx, "../irqmgr.c", 346);
    LogUtils_CheckNullPointer("stack", stack, "../irqmgr.c", 347);

    thisx->clients = NULL;
    thisx->retraceMsg.type = OS_SC_RETRACE_MSG;
    thisx->prenmiMsg.type = OS_SC_PRE_NMI_MSG;
    thisx->nmiMsg.type = OS_SC_NMI_MSG;
    thisx->resetStatus = STATUS_IDLE;
    thisx->resetTime = 0;

    osCreateMesgQueue(&thisx->queue, thisx->msgBuf, ARRAY_COUNT(thisx->msgBuf));
    osSetEventMesg(OS_EVENT_PRENMI, &thisx->queue, (OSMesg)PRE_NMI_MSG);
    osViSetEvent(&thisx->queue, (OSMesg)RETRACE_MSG, retraceCount);
    osCreateThread(&thisx->thread, 0x13, IrqMgr_ThreadEntry, thisx, stack, pri);
    osStartThread(&thisx->thread);
}
