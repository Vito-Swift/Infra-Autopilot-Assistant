/**
 * @filename: LP_CommPanel.hh.h
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 4/17/2021
 */

#ifndef LAMPPOSTAUTOCARDEMO_LAMPPOSTHOSTCOMMPANEL_HH
#define LAMPPOSTAUTOCARDEMO_LAMPPOSTHOSTCOMMPANEL_HH

#include <atomic>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "LamppostHostUtils.hh"

typedef struct SendThreadArgs {
    LamppostHostProg *hostProg;
} SendThreadArgs_t;

void *LamppostHostSendThread(void *vargp);

typedef struct RecvThreadArgs {
    LamppostHostProg *hostProg;
} RecvThreadArgs_t;

void *LamppostHostRecvThread(void *vargp);

typedef struct HookSendThreadArgs {
    LamppostHostProg *hostProg;
} HookSendThreadArgs_t;

void *LamppostHostCommHookSendThread(void *vargp);

typedef struct {
    LamppostHostProg *hostProg;
} LmpCtlListenerArgs_t;

void *LamppostHostLmpCtlListenerThread(void *vargp);

#endif //LAMPPOSTAUTOCARDEMO_LAMPPOSTHOSTCOMMPANEL_HH
