//
// Created by Vito Wu on 2021/6/21.
//

#ifndef LAMPPOSTAUTOCARDEMO_LAMPPOSTHOOKGENERAL_HH
#define LAMPPOSTAUTOCARDEMO_LAMPPOSTHOOKGENERAL_HH

#include "utils.hh"

#include <stdint.h>
#include <stdio.h>
#include <getopt.h>
#include <cstring>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>
#include <vector>
#include <signal.h>

#include "LamppostHookCommPanel.hh"
#include "LamppostHookUtils.hh"

/*  ********************************************************
 *               Inline Functions and Prototypes
 * *********************************************************/

void options_init(Options *options);

void options_parse(Options *options, int argc, char **argv);

void options_free(Options *options);

void hook_program_init(HookProg *prog, int argc, char **argv);

void hook_program_run(HookProg *prog);

void hook_program_exit(HookProg *prog);

#endif //LAMPPOSTAUTOCARDEMO_LAMPPOSTHOOKGENERAL_HH
