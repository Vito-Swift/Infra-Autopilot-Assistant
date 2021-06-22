//
// Created by Vito Wu on 2021/6/21.
//

#include "LPHook/LamppostHookGeneral.hh"

std::atomic<bool> term_flag;
HookProg *globalHookProg;

void interrupt_handler(int dummy) {
    PRINTF_STAMP("Catch CTRL-C signal.\n");
    PRINTF_STAMP("Terminate detached threads and exit.\n");
    term_flag = true;
    pthread_exit(nullptr);
}

static inline void copy_opt(char **str, char *optarg) {
    if (NULL == ((*str) = strndup(optarg, 1024))) {
        PRINTF_ERR("[!] invalid input parameter\n");
    }
}

void options_init(Options *options) {
    options->hook_ip_port = HOOK_TCP_PORT;
}

void options_parse(Options *options, int argc, char **argv) {
    int c, opt_idx;
    if (argc == 1) {
        print_usage(argv[0]);
        options_free(options);
        exit(0);
    }

    while (-1 != (c = getopt_long(argc, argv, "h", hook_long_opts, &opt_idx))) {
        switch (c) {
            case 0:
                if (hook_long_opts[opt_idx].flag == 0) {
                    PRINTF_STAMP("\toption %s: %s\n", hook_long_opts[opt_idx].name, optarg ? optarg : "null");
                }
                break;

            case 'h':
                print_usage(argv[0]);
                options_free(options);
                exit(0);

            case OP_PORT:
                options->hook_ip_port = strtoul(optarg, NULL, 0);
                break;

            case '?':
                break;

            default:
                EXIT_WITH_MSG("[!] unknown parameter, exit...\n");
        }
    }
}

void options_free(Options *options) {
}

void hook_program_init(HookProg *prog, int argc, char **argv) {

}

void hook_program_run(HookProg *prog) {

}

void hook_program_exit(HookProg *prog) {

}
