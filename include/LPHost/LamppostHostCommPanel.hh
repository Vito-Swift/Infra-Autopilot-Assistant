/**
 * @filename: LP_CommPanel.hh.h
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 4/17/2021
 */

#ifndef LAMPPOSTAUTOCARDEMO_LAMPPOSTHOSTCOMMPANEL_HH
#define LAMPPOSTAUTOCARDEMO_LAMPPOSTHOSTCOMMPANEL_HH

#include "LamppostHostUtils.hh"
#include <atomic>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

typedef struct SendThreadArgs {
    LamppostHostProg *hostProg;
    std::atomic<bool> *terminate_flag;
} SendThreadArgs_t;

void *LamppostHostSendThread(void *vargp);

typedef struct RecvThreadArgs {
    LamppostHostProg *hostProg;
    std::atomic<bool> *terminate_flag;
} RecvThreadArgs_t;

void *LamppostHostRecvThread(void *vargp);

typedef struct HookSendThreadArgs {
    LamppostHostProg *hostProg;
    std::atomic<bool> *terminate_flag;
} HookSendThreadArgs_t;

void *LamppostHostCommHookSendThread(void* vargp);

#endif //LAMPPOSTAUTOCARDEMO_LAMPPOSTHOSTCOMMPANEL_HH