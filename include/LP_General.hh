/**
 * @filename: Lammpost.hh.h
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 4/17/2021
 */

#ifndef LAMPPOSTAUTOCARDEMO_LAMMPOST_HH
#define LAMPPOSTAUTOCARDEMO_LAMMPOST_HH

/*  ********************************************************
 *                 Included Headers and Macros
 * *********************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>
#include <vector>

#include "utils.hh"
#include "LP_CommPanel.hh"
#include "LP_RBDetection.hh"

#define MOCK


/*  ********************************************************
 *               Inline Functions and Prototypes
 * *********************************************************/

void options_init(Options *options, int argc, char** argv);

void options_parse(Options *options);

void options_free(Options *options);

void lamppost_program_init(LamppostProg *lamppostProg);

void lamppost_program_run(LamppostProg *lamppostProg, int argc, char** argv);

void lamppost_program_exit(LamppostProg *lamppostProg);

#endif //LAMPPOSTAUTOCARDEMO_LAMMPOST_HH
