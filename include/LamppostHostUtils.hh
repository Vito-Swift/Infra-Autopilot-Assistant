//
// Created by Vito Wu on 2021/6/17.
//

#ifndef LAMPPOSTAUTOCARDEMO_LAMPPOSTHOSTUTILS_HH
#define LAMPPOSTAUTOCARDEMO_LAMPPOSTHOSTUTILS_HH

#include "utils.hh"

#include <getopt.h>
#include <atomic>

/**
 * Type: Options
 * Note: This structure is set to store the user options passed when init.
 *      The corresponding usage of each command is written as comment after
 *      each data field.
 */
typedef struct {
    std::string netaddr_str;            // bats network address of local node   (required)
    std::string root_net_addr;          // bats network address of root node    (required)
    int root_port;                      // bats port of root node               (required)
    int ctrl_zigbee_pan;                // zigbee pan of control node           (set only when launch on root)
    int ctrl_zigbee_addr;               // zigbee net address of control node   (set only when launch on root)
    int root_zigbee_pan;                // zigbee pan of root lamppost          (set only when launch on root)
    int root_zigbee_addr;               // zigbee net address of root lamppost  (set only when launch on root)
    bool mock_detection;                // if this option is enabled, road block detection will be mocked
    char *config_file_path;             // path to configuration file
    bool is_root_node;                  // true if this lamppost is root node
} Options;
enum opt_types {
    OP_CONFIG_PATH = 1,
    OP_MOCK_DETECTION,
};
static struct option lamppost_host_long_opts[]{
        {"config_file",    required_argument, nullptr, OP_CONFIG_PATH},
        {"mock_detection", no_argument,       nullptr, OP_MOCK_DETECTION},
};

/**
 * Type: LamppostHostProg
 * Note: This structure contains all the intermediate meta-data essential to
 *      the whole execution flow. In practice, users passed this option to
 *      LamppostProg_init(), LamppostProg_exec(), LamppostProg_exit() in
 *      order to control the execution flow
 */
typedef struct {
    // data structure to store options
    Options options;
    // thread to launch road block detection program
    pthread_t detection_thread;
    // thread to launch communicator
    pthread_t send_thread;
    pthread_t recv_thread;

    Queue<RBCoordinate> RoadBlockCoordinates;
} LamppostHostProg;

inline void print_usage(const char *prg_name) {
    printf("\n"
           "Usage: %s [OPTIONS]                 \n"
           "\n"
           "Options:                                      \n"
           "                                              \n"
           "    --config_file [CONFIG_FILE]               \n"
           "                    Path to configuration file\n"
           "    --mock_detection                          \n"
           "                    Mock roadblock detection by\n"
           "                    continually sending (0,0) to\n"
           "                    root lamppost node"
           "                                                \n"
           "                                                \n"
           "Example:                                        \n"
           "    > ./%s --config_file ../_config/lamp1.ini", prg_name, prg_name);
}

#endif //LAMPPOSTAUTOCARDEMO_LAMPPOSTHOSTUTILS_HH
