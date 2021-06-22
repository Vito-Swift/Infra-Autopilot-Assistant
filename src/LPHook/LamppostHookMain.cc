//
// Created by Vito Wu on 2021/6/16.
//

#include "LPHook/LamppostHookGeneral.hh"

int main(int argc, char **argv) {
    HookProg prog;
    hook_program_init(&prog, argc, argv);
    hook_program_run(&prog);
    hook_program_exit(&prog);
    return EXIT_SUCCESS;
}
