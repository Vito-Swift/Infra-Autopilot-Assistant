/**
 * @filename: Lammpost.hh.h
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 4/17/2021
 */

#ifndef LAMPPOSTAUTOCARDEMO_LAMMPOST_HH
#define LAMPPOSTAUTOCARDEMO_LAMMPOST_HH

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

#define MOCK

typedef struct {
    int pan;
    int addr;
    bool mock_detection;
    double beacon_duration;
} Options;

typedef struct {
    uint32_t index;
    double gps_x;
    double gps_y;
} RBCoordinate;

typedef struct {
    Options options;
    pthread_t detection_handler;
    pthread_t comm_handler;
    pthread_mutex_t mutex_RBC;
    std::vector <RBCoordinate> RoadBlockCoordinates;
} LamppostProg;

inline void options_init(Options *options) {

}

inline void options_parse(Options *options) {

}

inline void options_free(Options *options) {

}

inline void lamppost_program_init(LamppostProg *lamppostProg) {

}

inline void lamppost_program_run(LamppostProg *lamppostProg) {

}

inline void lamppost_program_exit(LamppostProg *lamppostProg) {

}

#endif //LAMPPOSTAUTOCARDEMO_LAMMPOST_HH
