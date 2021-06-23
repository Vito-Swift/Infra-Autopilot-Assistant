//
// Created by Vito Wu on 2021/6/21.
//

#ifndef LAMPPOSTAUTOCARDEMO_LAMPPOSTHOOKCOMMPANEL_HH
#define LAMPPOSTAUTOCARDEMO_LAMPPOSTHOOKCOMMPANEL_HH

#include <atomic>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "LamppostHookUtils.hh"
#include "CC2520_Helper.h"

typedef struct {
    int port;
    Queue<HookPacket> *PacketQueue;
    std::atomic<bool>* termination_flag;
} HookRecvArgs_t;

void *HookRecvThread(void *vargp);

typedef struct {

} HookSendArgs_t;

void *HookSendThread(void *vargp);

#endif //LAMPPOSTAUTOCARDEMO_LAMPPOSTHOOKCOMMPANEL_HH
