//
// Created by Vito Wu on 2021/6/17.
//

#ifndef LAMPPOSTAUTOCARDEMO_LAMPPOSTHOSTUTILS_HH
#define LAMPPOSTAUTOCARDEMO_LAMPPOSTHOSTUTILS_HH

#include "utils.hh"

/**
 * Type: Options
 * Note: This structure is set to store the user options passed when init.
 *      The corresponding usage of each command is written as comment after
 *      each data field.
 */
typedef struct {
    int pan;                    // network pan of localhost
    int addr;                   // network address of localhost
    int root_pan;               // network pan of root lamppost
    int root_addr;              // network address of root lamppost
    bool mock_detection;        // if this option is enabled, road block detection will be mocked
    char* config_file_path;     // path to configuration file
    bool is_root_node;          // true if this lamppost is root node
} Options;
enum opt_types {
    OP_CONFIG_PATH = 1
};
static struct option lamppost_host_long_opts[] {
        {"config_path", required_argument, nullptr, OP_CONFIG_PATH},
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
    pthread_t communicator_thread;

    // data field to store termination flag
    pthread_mutex_t* terminate_mutex;
    int* terminate;

    std::vector <RBCoordinate> RoadBlockCoordinates;
} LamppostHostProg;

inline void print_usage(char* prg_name) {
    printf("\n"
           "Usage: %s [OPTIONS]                 \n"
           "\n"
           "Options:                                      \n"
           "                                              \n"
           "    --config_path [CONFIG_FILE]               \n"
           "                    Path to configuration file\n"
           "\n"
           "\n"
           "Example:\n"
           "    > ./%s --config_path ../_config/lamp1.ini");
}

#endif //LAMPPOSTAUTOCARDEMO_LAMPPOSTHOSTUTILS_HH
