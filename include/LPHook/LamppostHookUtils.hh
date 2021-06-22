//
// Created by Vito Wu on 2021/6/22.
//

#ifndef LAMPPOSTAUTOCARDEMO_LAMPPOSTHOOKUTILS_HH
#define LAMPPOSTAUTOCARDEMO_LAMPPOSTHOOKUTILS_HH

#include "utils.hh"
#include <getopt.h>
#include <atomic>
#include <cmath>

typedef struct {
    std::string hook_ip_addr;       // IP address of hook node, default to be localhost
    int hook_ip_port;               // Listen port of hook node
} Options;
enum opt_types {
    OP_PORT = 1,
};
static struct option hook_long_opts[]{
        {"port", optional_argument, nullptr, OP_PORT},
};

typedef struct {
    // data structure to store options
    Options options;
    // thread to receive packet from host program
    pthread_t recv_thread;
    // thread to communicate with control node
    pthread_t send_thread;

    // Queue to store packets received from host program
    Queue<HookPacket_t> PacketQueue;
} HookProg;

inline void print_usage(const char *prg_name) {
    printf("\n"
           "Usage: %s [OPTIONS]                             \n"
           "                                                \n"
           "Options:                                        \n"
           "                                                \n"
           "    --port [PORT]                               \n"
           "                    open port for receiving tcp \n"
           "                    packets from root host, default\n"
           "                    to be 1020                  \n"
           "                                                \n"
           "Example:                                        \n"
           "    > ./%s --port 1080                          \n",
           prg_name, prg_name);
}

#endif //LAMPPOSTAUTOCARDEMO_LAMPPOSTHOOKUTILS_HH
