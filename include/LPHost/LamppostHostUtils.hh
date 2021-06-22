//
// Created by Vito Wu on 2021/6/17.
//

#ifndef LAMPPOSTAUTOCARDEMO_LAMPPOSTHOSTUTILS_HH
#define LAMPPOSTAUTOCARDEMO_LAMPPOSTHOSTUTILS_HH

#include "utils.hh"

#include <getopt.h>
#include <atomic>
#include <math.h>

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
    std::string hook_ip_addr;           // IP address of hook node              (set only when launch on root)
    int hook_ip_port;                   // Listen port of hook node             (set only when launch on root)
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
    // thread to launch hook communicator
    pthread_t hook_thread;

    // Queue to store the coordinates of detected road blocks
    Queue<RBCoordinate> RoadBlockCoordinates;

    // Array to store the received coordinates (on root program)
    Vector<RBCoordinate> CollectedRBCoordinates;
    mutable std::mutex crb_mutex;
    std::condition_variable crb_c;
} LamppostHostProg;

inline void print_usage(const char *prg_name) {
    printf("\n"
           "Usage: %s [OPTIONS]                             \n"
           "                                                \n"
           "Options:                                        \n"
           "                                                \n"
           "    --config_file [CONFIG_FILE]                 \n"
           "                    Path to configuration file  \n"
           "    --mock_detection                            \n"
           "                    Mock roadblock detection by \n"
           "                    continually sending (0,0) to\n"
           "                    root lamppost node          \n"
           "                                                \n"
           "                                                \n"
           "Example:                                        \n"
           "    > ./%s --config_file ../_config/lamp1.ini   \n",
           prg_name, prg_name);
}


inline double calculateDistance(const RBCoordinate &c1, const RBCoordinate &c2) {
    double d = pow(c1.gps_x - c2.gps_x, 2) + pow(c1.gps_y - c2.gps_y, 2);
    return sqrt(d);
}

#endif //LAMPPOSTAUTOCARDEMO_LAMPPOSTHOSTUTILS_HH
