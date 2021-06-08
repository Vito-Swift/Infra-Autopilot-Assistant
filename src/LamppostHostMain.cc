/**
 * @filename: LP_Main.cc.c
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 4/16/2021
 */

#include "LamppostHostGeneral.hh"

int main(int argc, char **argv) {
    LamppostHostProg prog;
    lamppost_program_init(&prog, argc, argv);
    lamppost_program_run(&prog);
    lamppost_program_exit(&prog);
    return 0;
}