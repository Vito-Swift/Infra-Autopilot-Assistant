/**
 * @filename: LP_General.cc
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 5/27/2021
 */

#include "LamppostHostGeneral.hh"

static int terminate;
static pthread_mutex_t terminate_mutex;

void interrupt_handler(int dummy) {
    PRINTF_STAMP("Catch CTRL-C signal.\n"
                 "Terminate detached threads and exit.\n");
    pthread_mutex_lock(&terminate_mutex);
    terminate = 1;
    pthread_mutex_unlock(&terminate_mutex);
}

void options_init(Options *options) {
    options->config_file_path = nullptr;
}

bool options_validate(Options *options) {

    return 0;
}

void options_parse(Options *options, int argc, char **argv) {

}

void options_free(Options *options) {

}

void lamppost_program_init(LamppostHostProg *lamppostProg, int argc, char **argv) {
    options_init(&lamppostProg->options);
    options_parse(&lamppostProg->options, argc, argv);
    terminate = 0;
    lamppostProg->terminate_mutex = &terminate_mutex;
    lamppostProg->terminate = &terminate;
}

void lamppost_program_run(LamppostHostProg *lamppostProg) {
    signal(SIGINT, interrupt_handler);

    int term_flag = 0;

    // launch thread to detect from video stream

    // launch thread to send coordinates of detected road block to root node

    // if the current node is root node, launch thread to 

    while (!term_flag) {
        // determine whether the program needs to terminate
        pthread_mutex_unlock(lamppostProg->terminate_mutex);
        term_flag = *(lamppostProg->terminate);
        pthread_mutex_lock(lamppostProg->terminate_mutex);
    }
}

void lamppost_program_exit(LamppostHostProg *lamppostProg) {
    options_free(&lamppostProg->options);
}
