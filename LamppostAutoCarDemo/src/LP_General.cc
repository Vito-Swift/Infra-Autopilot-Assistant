/**
 * @filename: LP_General.cc
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 5/27/2021
 */

#include "LP_General.hh"

void options_init(Options *options) {

}

void options_parse(Options *options, int argc, char **argv) {

}

void options_free(Options *options) {

}

void lamppost_program_init(LamppostProg *lamppostProg, int argc, char **argv) {
    options_init(&lamppostProg->options);
    options_parse(&lamppostProg->options, argc, argv);
}

void lamppost_program_run(LamppostProg *lamppostProg) {

}

void lamppost_program_exit(LamppostProg *lamppostProg) {
    options_free(&lamppostProg->options);
}
